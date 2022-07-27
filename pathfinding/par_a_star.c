#include <pthread.h>
#include "pathfinding.h"
#include "../msg_queue/msg_queue.h"

struct thread_d
{
    graph_t *graph;
    vertex_t *src;
    vertex_t *dst;
    int (*heuristic)(vertex_t *, vertex_t *);
    message_queue_t *msg_q;
    int n_thread;  // number of threads running
    int id_thread; // identifier/index of current thread
    heap_t *open_q;
    hash_table_t *closed_set;
    path_t **path;

    pthread_mutex_t *lock_condition; // mutexes for path setting
    pthread_mutex_t *lock_path;
    int *path_owner; // owner of the lock_condition mutex

    int *termination_flags; // flags use to detect the terminate condition

    pthread_cond_t **cond_variables;
    // pthread_mutex_t **cond_v_mutexes;
    pthread_mutex_t *cond_v_mutex;

    int *terminate_counter;
    int *program_terminated;
};

struct msg_data
{
    vertex_t *n;
    vertex_t *n_successor;
    int true_cost;
};

int terminate_detection(struct thread_d *data);
int compute_recipient(vertex_t *v, int n);
void *thread_search_path(void *args);

int par_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path, int n_threads_to_use)
{
    int i, result, terminate_counter = 0, program_terminated = 0;
    int path_owner = -1;
    printf("ASTAR PATH PAR\n");

    // Check parameters are not null before starting
    util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);
    util_check_r(src != NULL, "Source nodes cannot be null, returning...\n", 0);
    util_check_r(dst != NULL, "Destination nodes cannot be null, returning...\n", 0);

    message_queue_t *msg_q = message_queue_create(n_threads_to_use);

    pthread_t *threads = (pthread_t *)util_malloc(n_threads_to_use * sizeof(pthread_t));
    util_check_r(threads != NULL, "Could not allocate threads, returning...\n", 0);

    struct thread_d *thread_data = (struct thread_d *)util_malloc(n_threads_to_use * sizeof(struct thread_d));
    util_check_r(thread_data != NULL, "Could not allocate thread_data, returning...\n", 0);

    heap_t **open_q = (heap_t **)util_malloc(n_threads_to_use * sizeof(heap_t *));
    util_check_r(thread_data != NULL, "Could not allocate array open set, returning...\n", 0);

    hash_table_t **closed_set = (hash_table_t **)util_malloc(n_threads_to_use * sizeof(hash_table_t *));
    util_check_r(thread_data != NULL, "Could not allocate array of close set, returning...\n", 0);

    pthread_mutex_t *lock_condition = (pthread_mutex_t *)util_malloc(sizeof(pthread_mutex_t));
    util_check_r(lock_condition != NULL, "Could not allocate condition mutex, returning...\n", 0);
    util_check_r(pthread_mutex_init(lock_condition, NULL) == 0, "Could not initialized condition mutex, returning...\n", 0);

    pthread_mutex_t *lock_path = (pthread_mutex_t *)util_malloc(sizeof(pthread_mutex_t));
    util_check_r(lock_path != NULL, "Could not allocate path mutex, returning...\n", 0);
    util_check_r(pthread_mutex_init(lock_path, NULL) == 0, "Could not initialized path mutex, returning...\n", 0);

    *path = (path_t *)util_malloc(sizeof(path_t));
    // Check allocation was successful
    util_check_r(path != NULL, "Could not allocate path, returning...\n", 0);
    (*path)->nodes = stack_create();
    fprintf(stdout, "---STACK Creation---\n");
    // Check allocation was successful
    util_check_r((*path)->nodes != NULL, "Could not allocate path's stack, returning...\n", 0);
    // incumbent cost initialized to infinite
    (*path)->cost = INT_MAX;
    (*path)->visited_nodes = 0;
    (*path)->revisited_nodes = 0;

    int *termination_flags = (int *)util_calloc(n_threads_to_use, sizeof(int));

    pthread_cond_t **cond_variables = (pthread_cond_t **)util_malloc(n_threads_to_use * sizeof(pthread_cond_t *));
    util_check_r(cond_variables != NULL, "Could not allocate condition variables vector, returning...\n", 0);
    // pthread_mutex_t **cond_v_mutexes = (pthread_mutex_t **)util_malloc(n_threads_to_use * sizeof(pthread_mutex_t *));
    // util_check_r(cond_v_mutexes != NULL, "Could not allocate condition variable mutex vector, returning...\n", 0);

    /*
        =============
        SINGLE MUTEXX
        =============
    */

    pthread_mutex_t *cond_v_mutex = (pthread_mutex_t *)util_malloc(n_threads_to_use * sizeof(pthread_mutex_t));
    util_check_r(cond_v_mutex != NULL, "Could not allocate condition variable mutex vector, returning...\n", 0);

    util_check_r(pthread_mutex_init(cond_v_mutex, NULL) == 0, "Could not initialized condition variable mutex, returning...\n", 0);

    printf("Creating %d threads\n", n_threads_to_use);

    for (i = 0; i < n_threads_to_use; i++)
    {
        cond_variables[i] = (pthread_cond_t *)util_malloc(sizeof(pthread_cond_t));
        util_check_r(cond_variables[i] != NULL, "Could not allocate condition variable, returning...\n", 0);
        // cond_v_mutexes[i] = (pthread_mutex_t *)util_malloc(sizeof(pthread_mutex_t));
        // util_check_r(cond_v_mutexes[i] != NULL, "Could not allocate condition variable mutex, returning...\n", 0);

        // util_check_r(pthread_mutex_init(cond_v_mutexes[i], NULL) == 0, "Could not initialized condition variable mutex, returning...\n", 0);

        util_check_r(pthread_cond_init(cond_variables[i], NULL) == 0, "Could not initialized condition variable, returning...\n", 0);
    }

    for (i = 0; i < n_threads_to_use; i++)
    {
        thread_data[i].graph = graph;
        thread_data[i].src = src;
        thread_data[i].dst = dst;
        thread_data[i].heuristic = heuristic;
        thread_data[i].msg_q = msg_q;
        thread_data[i].n_thread = n_threads_to_use;
        thread_data[i].id_thread = i;
        thread_data[i].lock_condition = lock_condition;
        thread_data[i].lock_path = lock_path;
        thread_data[i].path_owner = &path_owner;
        thread_data[i].termination_flags = termination_flags;
        // thread_data[i].cond_v_mutexes = cond_v_mutexes;
        thread_data[i].cond_v_mutex = cond_v_mutex;
        thread_data[i].cond_variables = cond_variables;
        thread_data[i].terminate_counter = &terminate_counter;
        thread_data[i].program_terminated = &program_terminated;

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

        if (compute_recipient(src, n_threads_to_use) == i)
        {
            // Init open with source
            fprintf(stdout, "src node with id %d assigned to thread %d\n", src->id, i);
            result = heap_insert(thread_data[i].open_q, src->id, (void *)src, 0);
            // Check no errors occurred
            util_check_r(result, "Could not insert in the open set, returning...\n", 0);
        }

        pthread_create(&threads[i], NULL, thread_search_path, (void *)&thread_data[i]);
    }

    for (i = 0; i < n_threads_to_use; i++)
    {
        pthread_join(threads[i], NULL);
        hash_table_destroy(closed_set[i], NULL);
        heap_destroy(open_q[i], NULL);
        // pthread_mutex_destroy(cond_v_mutexes[i]);
        pthread_cond_destroy(cond_variables[i]);

        free(cond_variables[i]);
        // free(cond_v_mutexes[i]);
    }

    if ((*path)->cost == INT_MAX)
    {
        // Path not found
        (*path)->nodes = NULL;
        return 1;
    }

    // free(cond_v_mutexes);
    free(cond_variables);
    free(termination_flags);
    pthread_mutex_destroy(lock_condition);
    free(lock_condition);
    pthread_mutex_destroy(lock_path);
    free(lock_path);
    free(open_q);
    free(closed_set);
    free(thread_data);
    free(threads);
    message_queue_destroy(msg_q, NULL);
    return 1;
}

void *thread_search_path(void *args)
{
    int visited_nodes = 0, revisited_nodes = 0, result, key, open_is_empty;
    struct thread_d *thread_data = (struct thread_d *)args;
    vertex_t *data, *data2;
    struct msg_data *m_data, *m_data2;

    while (terminate_detection(thread_data))
    {
        data2 = NULL;
        m_data2 = NULL;

        m_data2 = (struct msg_data *)util_malloc(sizeof(struct msg_data));
        // Check allocation was successful
        util_check_r(m_data2 != NULL, "Could not allocate struct message to receive data, returning...\n", 0);

        data2 = (vertex_t *)util_malloc(sizeof(vertex_t));
        // Check allocation was successful
        util_check_r(data2 != NULL, "Could not allocate data to extract from closed set, returning...\n", 0);

        // check if thread message queue is empty
        while (message_queue_count(thread_data->msg_q, thread_data->id_thread) != 0)
        {
            data = NULL;
            m_data = NULL;

            m_data = (struct msg_data *)util_malloc(sizeof(struct msg_data));
            // Check allocation was successful
            util_check_r(m_data != NULL, "Could not allocate struct message to receive data, returning...\n", 0);

            data = (vertex_t *)util_malloc(sizeof(vertex_t));
            // Check allocation was successful
            util_check_r(data != NULL, "Could not allocate data to extract from closed set, returning...\n", 0);

            // set flags for termination detection
            thread_data->termination_flags[thread_data->id_thread] = 0;

            // Get and remove from message queue a triplet (n_successor, g(n) + c(n, n_successor), n);
            message_queue_receive(thread_data->msg_q, (void **)&m_data, thread_data->id_thread);
            fprintf(stdout, "Thread %d received message\n", thread_data->id_thread);

            result = hash_table_get(thread_data->closed_set, m_data->n_successor->id, (void **)&data);

            // Check no errors occurred
            util_check_r(result, "Could not get from the closed set, returning...\n", 0);

            int heuristic_cost = thread_data->heuristic == NULL ? 0 : thread_data->heuristic(m_data->n_successor, thread_data->dst);

            if (data != NULL)
            {
                fprintf(stdout, "%d | Node with id %d already in closed set\n", thread_data->id_thread, m_data->n_successor->id);
                if (m_data->true_cost < m_data->n_successor->true_cost)
                {

                    fprintf(stdout, "%d | Found smaller cost for node with id %d, readding it to open set\n", thread_data->id_thread, m_data->n_successor->id);
                    result = hash_table_delete(thread_data->closed_set, m_data->n_successor->id);

                    // Check no errors occurred
                    util_check_r(result, "Could not delete from the closed set, returning...\n", 0);

                    result = heap_insert(thread_data->open_q, m_data->n_successor->id, (void *)(m_data->n_successor), m_data->true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);

                    revisited_nodes++;
                }
                else
                {
                    fprintf(stdout, "%d | Skipping\n", thread_data->id_thread);

                    // free(data);
                    // free(m_data);
                    continue;
                }
            }
            else
            {
                fprintf(stdout, "%d | Node with id %d not in closed set\n", thread_data->id_thread, m_data->n_successor->id);

                int *position = (int *)util_malloc(sizeof(int));

                // Check allocation was successful
                util_check_r(position != NULL, "Could not allocate position to extract from open set, returning...\n", 0);

                result = heap_find(thread_data->open_q, m_data->n_successor->id, &position);

                // Check no errors occurred
                util_check_r(result, "Could not search in the open set, returning...\n", 0);

                if (position == NULL)
                {
                    fprintf(stdout, "%d | Add node with id %d to open set\n", thread_data->id_thread, m_data->n_successor->id);
                    result = heap_insert(thread_data->open_q, m_data->n_successor->id, (void *)(m_data->n_successor), m_data->true_cost + heuristic_cost);
                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);
                }
                else if (m_data->true_cost >= m_data->n_successor->true_cost)
                {
                    fprintf(stdout, "%d | Skipping\n", thread_data->id_thread);

                    // free(data);
                    // free(m_data);
                    // free(position);
                    continue;
                }
                else
                {
                    fprintf(stdout, "%d | Updating node with id %d to open set\n", thread_data->id_thread, m_data->n_successor->id);
                    result = heap_update(thread_data->open_q, m_data->n_successor->id, m_data->true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not update in the open set, returning...\n", 0);
                }

                // free(position);
            }

            fprintf(stdout, "Updating costs and parent for node with id %d to open set\n", m_data->n_successor->id);
            m_data->n_successor->true_cost = m_data->true_cost;
            m_data->n_successor->heuristic_cost = heuristic_cost;
            m_data->n_successor->parent = m_data->n;
        }

        open_is_empty = heap_is_empty(thread_data->open_q);
        fprintf(stdout, "Thread %d, Heap is %s empty\n", thread_data->id_thread, open_is_empty ? "" : "not");

        if (!open_is_empty)
        {
            fprintf(stdout, "%d | Extracting data from open set\n", thread_data->id_thread);
            result = heap_extract(thread_data->open_q, (void *)&data2, &key);
            // Check no errors occurred
            util_check_r(result, "Could not extract from the open set, returning...\n", 0);
        }

        if (open_is_empty || (data2->true_cost + data2->heuristic_cost) >= (*thread_data->path)->cost)
        {
            thread_data->termination_flags[thread_data->id_thread] = 1;
            if (!open_is_empty)
                heap_insert(thread_data->open_q, key, data2, data2->true_cost + data2->heuristic_cost);
            // sleep(1);
            continue;
        }

        fprintf(stdout, "%d | Add node with id %d to closed set\n", thread_data->id_thread, data2->id);
        result = hash_table_insert(thread_data->closed_set, data2->id, (void *)data2);
        // Check no errors occurred
        util_check_r(result, "Could not insert in the closed set, returning...\n", 0);

        visited_nodes++;

        if (data2->id == thread_data->dst->id)
        {
            fprintf(stdout, "thread %d arrived to goal node with id %d\n", thread_data->id_thread, data2->id);

            // acquire lock for checking condition
            pthread_mutex_lock(thread_data->lock_condition);
            if (data2->true_cost < (*thread_data->path)->cost)
            {
                // set current thread as the path owner (can modify path->nodes)
                (*thread_data->path_owner) = thread_data->id_thread;
                // updating path cost, and additional info
                (*thread_data->path)->cost = data2->true_cost;
                (*thread_data->path)->visited_nodes += visited_nodes;
                visited_nodes = 0;
                (*thread_data->path)->revisited_nodes += revisited_nodes;
            }
            // unlock mutex
            pthread_mutex_unlock(thread_data->lock_condition);

            // check if current thread is still the owner of the lock
            if ((*thread_data->path_owner) == thread_data->id_thread)
            {
                // acquire path lock
                pthread_mutex_lock(thread_data->lock_path);

                vertex_t *n = thread_data->dst;
                // destroy previous path and create a new one
                stack_destroy((*thread_data->path)->nodes, NULL);
                fprintf(stdout, "---STACK Destroy---\n");
                (*thread_data->path)->nodes = stack_create();
                fprintf(stdout, "---STACK Creation2---\n");
                // Check allocation was successful
                util_check_r((*thread_data->path)->nodes != NULL, "Could not allocate path's stack, returning...\n", 0);

                // check that current thread can modify the path (another thread has found a better solution)
                while (n != NULL && (*thread_data->path_owner) == thread_data->id_thread)
                {
                    fprintf(stdout, "---STACK Pushing---\n");
                    result = stack_push((*thread_data->path)->nodes, (void *)&(n->id));
                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the path's stack, returning...\n", 0);
                    n = n->parent;
                }
                pthread_mutex_unlock(thread_data->lock_path);
            }
        }

        edge_t *e = data2->head;

        while (e != NULL)
        {
            int recipent;
            m_data2 = (struct msg_data *)util_malloc(sizeof(struct msg_data));
            // Check allocation was successful
            util_check_r(m_data2 != NULL, "Could not allocate struct message to receive data, returning...\n", 0);

            fprintf(stdout, "%d | Checking edge from node with id %d to node with id %d\n", thread_data->id_thread, data2->id, e->dest->id);
            int new_true_cost = data2->true_cost + e->weight;

            m_data2->true_cost = new_true_cost;
            m_data2->n = data2;
            m_data2->n_successor = e->dest;

            recipent = compute_recipient(e->dest, thread_data->n_thread);

            fprintf(stdout, "Thread %d send message to %d\n", thread_data->id_thread, recipent);
            pthread_mutex_lock(thread_data->cond_v_mutex);
            message_queue_send(thread_data->msg_q, (void *)m_data2, recipent);
            pthread_cond_signal(thread_data->cond_variables[recipent]);
            pthread_mutex_unlock(thread_data->cond_v_mutex);

            e = e->next;
        }

        // free(data);
        // free(m_data);
    }

    pthread_exit(NULL);
}

/**
 * @brief check the termination of the algorithm.
 *
 * @param msg_q is the message queue
 * @param termination_flags is the array of flags
 * @param n_flags is the size of the termination_flags array
 * @return int 0 if the termination condition occurs, otherwise return 1
 */
int terminate_detection(struct thread_d *data)
{
    int i;

    // no condition variable solution

    // for (int i = 0; i < n_flags; i++)
    //     if (termination_flags[i] == 0)
    //         return 1;

    // if (message_queue_all_empty(msg_q))
    //     return 0;

    // return 1;

    pthread_mutex_lock(data->cond_v_mutex);
    // while my message queue is empty and my open queue is not useful/empty i sleep check that I'm not the last one to go sleep
    while (data->termination_flags[data->id_thread] && message_queue_count(data->msg_q, data->id_thread) == 0 && !(*data->program_terminated))
    {
        // check terminate_counter < n_thread -1
        if ((*data->terminate_counter) >= data->n_thread - 1 && message_queue_all_empty(data->msg_q))
        {
            // last thread that will go sleep instead set program_terminated and wake up all others
            (*data->program_terminated) = 1;

            fprintf(stdout, "---Termination detected Waking all %d---\n", data->id_thread);
            for (i = 0; i < data->n_thread; i++)
            {
                pthread_cond_signal(data->cond_variables[i]);
            }
        }
        else
        {
            // only one consumer for each condition variable, the condition to wake up is an arriving message, no one except me can receive a message from my queue
            fprintf(stdout, "---Going Sleeeeeep, thread %d---\n", data->id_thread);
            (*data->terminate_counter) += 1;

            pthread_cond_wait(data->cond_variables[data->id_thread], data->cond_v_mutex);

            (*data->terminate_counter) -= 1;

            fprintf(stdout, "---Bonjour, Thread %d---\n", data->id_thread);

            // else
        }
    }

    pthread_mutex_unlock(data->cond_v_mutex);

    if ((*data->program_terminated))
    {
        fprintf(stdout, "---Terminating thread %d---\n", data->id_thread);
        return 0;
    }
    return 1;
}

int compute_recipient(vertex_t *v, int n)
{
    return v->id % n;
}