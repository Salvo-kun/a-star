#include <pthread.h>
#include "pathfinding.h"
#include "../msg_queue/msg_queue.h"

// Data structures

typedef struct thread_data_s
{
    // Pathfinding data
    graph_t *graph;
    vertex_t *src;
    vertex_t *dst;
    int (*heuristic)(vertex_t *, vertex_t *);

    // Supporting ADT
    message_queue_t *msg_q;
    heap_t *open_q;
    hash_table_t *closed_set;
    path_t **path;

    // Thread info and stats
    int n_thread;
    int id_thread;
    int visited_nodes;
    int revisited_nodes;

    // Concurrency controls for path
    pthread_mutex_t *path_lock;

    // Concurrency controls for termination
    pthread_cond_t **cond_variables;
    pthread_mutex_t *cond_v_mutex;
    int *termination_flags;
    int *terminate_counter;
    int *program_terminated;

    // Hash's support structure
    hash_t *hash_data;
} thread_data_t;

typedef struct msg_data_s
{
    vertex_t *n;
    vertex_t *n_successor;
    int true_cost;
} msg_data_t;

// Private methods prototypes

int terminate_detection(thread_data_t *data);
int compute_recipient(vertex_t *v, int n, hash_t *hash_data, vertex_t *min_node);
void *thread_search_path(void *args);

// Public methods

int par_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path, int n_threads_to_use, hash_t *hash_data)
{
    int i, result, terminate_counter = 0, program_terminated = 0, first_recipient;
    //fprintf(stdout, "ASTAR PATH PAR\n");

    // Check parameters are not null before starting
    util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);
    util_check_r(src != NULL, "Source nodes cannot be null, returning...\n", 0);
    util_check_r(dst != NULL, "Destination nodes cannot be null, returning...\n", 0);

    message_queue_t *msg_q = message_queue_create(n_threads_to_use);
    util_check_r(msg_q != NULL, "Could not allocate message queue, returning...\n", 0);

    pthread_t *threads = (pthread_t *)util_malloc(n_threads_to_use * sizeof(pthread_t));
    util_check_r(threads != NULL, "Could not allocate threads, returning...\n", 0);

    thread_data_t *thread_data = (thread_data_t *)util_malloc(n_threads_to_use * sizeof(thread_data_t));
    util_check_r(thread_data != NULL, "Could not allocate thread_data, returning...\n", 0);

    heap_t **open_q = (heap_t **)util_malloc(n_threads_to_use * sizeof(heap_t *));
    util_check_r(open_q != NULL, "Could not allocate array of open sets, returning...\n", 0);

    hash_table_t **closed_set = (hash_table_t **)util_malloc(n_threads_to_use * sizeof(hash_table_t *));
    util_check_r(closed_set != NULL, "Could not allocate array of closed sets, returning...\n", 0);

    pthread_mutex_t *path_lock = (pthread_mutex_t *)util_malloc(sizeof(pthread_mutex_t));
    util_check_r(path_lock != NULL, "Could not allocate path mutex, returning...\n", 0);
    util_check_r(pthread_mutex_init(path_lock, NULL) == 0, "Could not initialize path mutex, returning...\n", 0);

    *path = (path_t *)util_malloc(sizeof(path_t));
    util_check_r(path != NULL, "Could not allocate path, returning...\n", 0);

    (*path)->cost = INT_MAX;
    (*path)->visited_nodes = 0;
    (*path)->revisited_nodes = 0;
    (*path)->nodes = stack_create();
    util_check_r((*path)->nodes != NULL, "Could not allocate path's stack, returning...\n", 0);

    int *termination_flags = (int *)util_malloc(n_threads_to_use * sizeof(int));
    util_check_r(termination_flags != NULL, "Could not allocate termination flags, returning...\n", 0);

    pthread_cond_t **cond_variables = (pthread_cond_t **)util_malloc(n_threads_to_use * sizeof(pthread_cond_t *));
    util_check_r(cond_variables != NULL, "Could not allocate condition variables vector, returning...\n", 0);

    pthread_mutex_t *cond_v_mutex = (pthread_mutex_t *)util_malloc(n_threads_to_use * sizeof(pthread_mutex_t));
    util_check_r(cond_v_mutex != NULL, "Could not allocate condition variable mutex vector, returning...\n", 0);
    util_check_r(pthread_mutex_init(cond_v_mutex, NULL) == 0, "Could not initialize condition variable mutex, returning...\n", 0);

    //fprintf(stdout, "Creating %d threads\n", n_threads_to_use);

    for (i = 0; i < n_threads_to_use; i++)
    {
        cond_variables[i] = (pthread_cond_t *)util_malloc(sizeof(pthread_cond_t));
        util_check_r(cond_variables[i] != NULL, "Could not allocate condition variable, returning...\n", 0);
        util_check_r(pthread_cond_init(cond_variables[i], NULL) == 0, "Could not initialize condition variable, returning...\n", 0);
    }

    //fprintf(stdout, "--- FIRST ITERATION ---\n");
    first_recipient = compute_recipient(src, n_threads_to_use, hash_data, NULL);
    //fprintf(stdout, "---- FIRST RECIPIENT %d ------\n", first_recipient);

    for (i = 0; i < n_threads_to_use; i++)
    {
        thread_data[i].graph = graph;
        thread_data[i].src = src;
        thread_data[i].dst = dst;
        thread_data[i].visited_nodes = 0;
        thread_data[i].revisited_nodes = 0;
        thread_data[i].heuristic = heuristic;
        thread_data[i].msg_q = msg_q;
        thread_data[i].n_thread = n_threads_to_use;
        thread_data[i].id_thread = i;
        thread_data[i].path_lock = path_lock;
        thread_data[i].termination_flags = termination_flags;
        thread_data[i].cond_v_mutex = cond_v_mutex;
        thread_data[i].cond_variables = cond_variables;
        thread_data[i].terminate_counter = &terminate_counter;
        thread_data[i].program_terminated = &program_terminated;
        thread_data[i].hash_data = hash_data;

        // Creates the priority queue which will contain unvisited nodes
        open_q[i] = heap_create(0, 10);
        // Check allocation was successful
        util_check_r(open_q[i] != NULL, "Could not allocate open set, returning...\n", 0);
        thread_data[i].open_q = open_q[i];

        // Create closed nodes set, which will contain visited nodes
        closed_set[i] = hash_table_create(10);
        // Check allocation was successful
        util_check_r(closed_set != NULL, "Could not allocate closed set, returning...\n", 0);
        thread_data[i].closed_set = closed_set[i];

        thread_data[i].path = path;

        if (first_recipient == i)
        {
            // Init open with source
            //fprintf(stdout, "src node with id %d assigned to thread %d\n", src->id, i);
            result = heap_insert(thread_data[i].open_q, src->id, (void *)src, 0);
            // Check no errors occurred
            util_check_r(result, "Could not insert in the open set, returning...\n", 0);
        }

        pthread_create(&threads[i], NULL, thread_search_path, (void *)&thread_data[i]);
    }

    for (i = 0; i < n_threads_to_use; i++)
    {
        pthread_join(threads[i], NULL);
        //fprintf(stdout, "Joined thread %d\n", i);

        // Copy stats back
        (*path)->visited_nodes += thread_data[i].visited_nodes;
        (*path)->revisited_nodes += thread_data[i].revisited_nodes;

        hash_table_destroy(closed_set[i], NULL);
        heap_destroy(open_q[i], NULL);
        pthread_cond_destroy(cond_variables[i]);
        util_free(cond_variables[i]);
    }

    if ((*path)->cost == INT_MAX)
    {
        // Path not found
        util_free(*path);
        *path = NULL;
        return 1;
    }
    else
    {
        // Path found, push nodes from the end
        vertex_t *n = thread_data->dst;
        // check that current thread can modify the path (another thread has found a better solution)
        while (n != NULL)
        {
            result = stack_push((*path)->nodes, (void *)&(n->id));
            // Check no errors occurred
            util_check_r(result, "Could not insert in the path's stack, returning...\n", 0);

            if (n == thread_data->src)
            {
                // Avoid going backward.
                break;
            }

            n = n->parent;
        }
    }

    util_free(cond_variables);
    util_free(termination_flags);
    pthread_mutex_destroy(path_lock);
    util_free(path_lock);
    util_free(open_q);
    util_free(closed_set);
    util_free(thread_data);
    util_free(threads);
    message_queue_destroy(msg_q, NULL);

    return 1;
}

// Private methods

void *thread_search_path(void *args)
{
    //fprintf(stdout, "Thread %d | Starting...\n", ((thread_data_t *)args)->id_thread);

    int result, open_is_empty;
    thread_data_t *thread_data = (thread_data_t *)args;
    vertex_t *data, *min_node;
    msg_data_t *msg_data_rcv, *msg_data_send;

    thread_data->termination_flags[thread_data->id_thread] = 0;

    while (terminate_detection(thread_data))
    {
        min_node = NULL;
        msg_data_send = NULL;

        // check if thread message queue is empty
        while (message_queue_count(thread_data->msg_q, thread_data->id_thread) != 0)
        {
            data = NULL;
            msg_data_rcv = NULL;

            msg_data_rcv = (msg_data_t *)util_malloc(sizeof(msg_data_t));
            //  Check allocation was successful
            util_check_r(msg_data_rcv != NULL, "Could not allocate struct message to receive data, returning...\n", 0);

            data = (vertex_t *)util_malloc(sizeof(vertex_t));
            // Check allocation was successful
            util_check_r(data != NULL, "Could not allocate data to extract from closed set, returning...\n", 0);

            // set flags for termination detection
            thread_data->termination_flags[thread_data->id_thread] = 0;

            // Get and remove from message queue a triplet (n_successor, g(n) + c(n, n_successor), n);
            message_queue_receive(thread_data->msg_q, (void **)&msg_data_rcv, thread_data->id_thread);
            //fprintf(stdout, "Thread %d |received message from node %d to node %d with cost %d\n", thread_data->id_thread, msg_data_rcv->n->id, msg_data_rcv->n_successor->id, msg_data_rcv->true_cost);

            result = hash_table_get(thread_data->closed_set, msg_data_rcv->n_successor->id, (void **)&data);

            // Check no errors occurred
            util_check_r(result, "Could not get from the closed set, returning...\n", 0);

            int heuristic_cost = thread_data->heuristic == NULL ? 0 : thread_data->heuristic(msg_data_rcv->n_successor, thread_data->dst);

            if (data != NULL)
            {
                //fprintf(stdout, "Thread %d | Node with id %d already in closed set\n", thread_data->id_thread, msg_data_rcv->n_successor->id);
                if (msg_data_rcv->true_cost < msg_data_rcv->n_successor->true_cost)
                {
                    //fprintf(stdout, "Thread %d | Found smaller cost for node with id %d, readding it to open set\n", thread_data->id_thread, msg_data_rcv->n_successor->id);
                    result = hash_table_delete(thread_data->closed_set, msg_data_rcv->n_successor->id);

                    // Check no errors occurred
                    util_check_r(result, "Could not delete from the closed set, returning...\n", 0);

                    result = heap_insert(thread_data->open_q, msg_data_rcv->n_successor->id, (void *)(msg_data_rcv->n_successor), msg_data_rcv->true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);

                    thread_data->revisited_nodes++;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                //fprintf(stdout, "Thread %d | Node with id %d not in closed set\n", thread_data->id_thread, msg_data_rcv->n_successor->id);

                int *position = (int *)util_malloc(sizeof(int));

                // Check allocation was successful
                util_check_r(position != NULL, "Could not allocate position to extract from open set, returning...\n", 0);

                result = heap_find(thread_data->open_q, msg_data_rcv->n_successor->id, &position);

                // Check no errors occurred
                util_check_r(result, "Could not search in the open set, returning...\n", 0);

                if (position == NULL)
                {
                    //fprintf(stdout, "Thread %d | Add node with id %d to open set with cost %d\n", thread_data->id_thread, msg_data_rcv->n_successor->id, msg_data_rcv->true_cost + heuristic_cost);
                    result = heap_insert(thread_data->open_q, msg_data_rcv->n_successor->id, (void *)(msg_data_rcv->n_successor), msg_data_rcv->true_cost + heuristic_cost);
                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);
                }
                else if (msg_data_rcv->true_cost >= msg_data_rcv->n_successor->true_cost)
                {
                    continue;
                }
                else
                {
                    //fprintf(stdout, "Thread %d | Updating node with id %d to open set with new cost %d\n", thread_data->id_thread, msg_data_rcv->n_successor->id, msg_data_rcv->true_cost + heuristic_cost);
                    result = heap_update(thread_data->open_q, msg_data_rcv->n_successor->id, msg_data_rcv->true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not update in the open set, returning...\n", 0);
                }
            }

            //fprintf(stdout, "Thread %d | Updating costs and parent for node with id %d to open set\n", thread_data->id_thread, msg_data_rcv->n_successor->id);
            msg_data_rcv->n_successor->true_cost = msg_data_rcv->true_cost;
            msg_data_rcv->n_successor->heuristic_cost = heuristic_cost;
            msg_data_rcv->n_successor->parent = msg_data_rcv->n;

            util_free(msg_data_rcv);
        }

        open_is_empty = heap_is_empty(thread_data->open_q);
        //fprintf(stdout, "Thread %d | Heap is %s empty\n", thread_data->id_thread, open_is_empty ? "" : "not");

        if (!open_is_empty)
        {
            //fprintf(stdout, "Thread %d | Extracting data from open set\n", thread_data->id_thread);

            min_node = (vertex_t *)util_malloc(sizeof(vertex_t));
            // Check allocation was successful
            util_check_r(min_node != NULL, "Could not allocate data to extract from closed set, returning...\n", 0);

            result = heap_peek(thread_data->open_q, (void *)&min_node, NULL);
            // Check no errors occurred
            util_check_r(result, "Could not peek into the open set, returning...\n", 0);
        }

        if (open_is_empty || (min_node->true_cost + min_node->heuristic_cost) >= (*thread_data->path)->cost)
        {
            thread_data->termination_flags[thread_data->id_thread] = 1;
            continue;
        }

        //fprintf(stdout, "Thread %d | Extracting data from open set\n", thread_data->id_thread);

        result = heap_extract(thread_data->open_q, (void *)&min_node, NULL);
        // Check no errors occurred
        util_check_r(result, "Could not extract from the open set, returning...\n", 0);

        //fprintf(stdout, "Thread %d | Add node with id %d to closed set\n", thread_data->id_thread, min_node->id);
        result = hash_table_insert(thread_data->closed_set, min_node->id, (void *)min_node);
        // Check no errors occurred
        util_check_r(result, "Could not insert in the closed set, returning...\n", 0);

        thread_data->visited_nodes++;

        if (min_node->id == thread_data->dst->id)
        {
            //fprintf(stdout, "Thread %d | Arrived to goal node with id %d\n", thread_data->id_thread, min_node->id);

            // acquire lock for checking condition
            pthread_mutex_lock(thread_data->path_lock);
            if (min_node->true_cost < (*thread_data->path)->cost)
            {
                (*thread_data->path)->cost = min_node->true_cost;
            }
            // unlock mutex
            pthread_mutex_unlock(thread_data->path_lock);
        }

        edge_t *e = min_node->head;

        while (e != NULL)
        {
            int recipient;
            msg_data_send = (msg_data_t *)util_malloc(sizeof(msg_data_t));
            // Check allocation was successful
            util_check_r(msg_data_send != NULL, "Could not allocate struct message to receive data, returning...\n", 0);

            //fprintf(stdout, "Thread %d | Checking edge from node with id %d to node with id %d\n", thread_data->id_thread, min_node->id, e->dest->id);
            int new_true_cost = min_node->true_cost + e->weight;

            msg_data_send->true_cost = new_true_cost;
            msg_data_send->n = min_node;
            msg_data_send->n_successor = e->dest;

            recipient = compute_recipient(e->dest, thread_data->n_thread, thread_data->hash_data, min_node);

            //fprintf(stdout, "Thread %d | send message to %d from node with id %d to node with id %d with cost %d\n", thread_data->id_thread, recipient, min_node->id, e->dest->id, new_true_cost);
            pthread_mutex_lock(thread_data->cond_v_mutex);
            message_queue_send(thread_data->msg_q, (void *)msg_data_send, recipient);
            pthread_cond_signal(thread_data->cond_variables[recipient]);
            pthread_mutex_unlock(thread_data->cond_v_mutex);

            e = e->next;
        }
    }

    pthread_exit(NULL);
}

int terminate_detection(thread_data_t *data)
{
    pthread_mutex_lock(data->cond_v_mutex);

    // while my message queue is empty and my open queue is not useful/empty i sleep check that I'm not the last one to go sleep
    while (data->termination_flags[data->id_thread] && message_queue_count(data->msg_q, data->id_thread) == 0 && !(*data->program_terminated))
    {
        // check terminate_counter < n_thread -1
        if ((*data->terminate_counter) >= data->n_thread - 1 && message_queue_all_empty(data->msg_q))
        {
            // last thread that will go sleep instead set program_terminated and wake up all others
            (*data->program_terminated) = 1;

            //fprintf(stdout, "--- Thread %d termination detected waking all ---\n", data->id_thread);
            for (int i = 0; i < data->n_thread; i++)
            {
                pthread_cond_signal(data->cond_variables[i]);
            }
        }
        else
        {
            // only one consumer for each condition variable, the condition to wake up is an arriving message, no one except me can receive a message from my queue
            (*data->terminate_counter) += 1;

            pthread_cond_wait(data->cond_variables[data->id_thread], data->cond_v_mutex);

            (*data->terminate_counter) -= 1;

            //fprintf(stdout, "---Bonjour, Thread %d---\n", data->id_thread);
        }
    }

    pthread_mutex_unlock(data->cond_v_mutex);

    if ((*data->program_terminated))
    {
        //fprintf(stdout, "---Terminating thread %d---\n", data->id_thread);
        return 0;
    }

    return 1;
}

int compute_recipient(vertex_t *v, int n, hash_t *hash_data, vertex_t *min_node)
{
    unsigned int hash_value;

    hash_value = hash_data->hash_func(v->id, hash_data);

    //fprintf(stdout, "Hash: %u\n", hash_value);
    return hash_value % n;
}