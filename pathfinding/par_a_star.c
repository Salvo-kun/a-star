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
    int n_thread;                   // number of threads running
    int id_thread;                  // identifier/index of current thread
    heap_t *open_q;    
    hash_table_t  *closed_set;
    path_t **path;

    pthread_mutex_t *lock_condition;    // mutexes for path setting
    pthread_mutex_t *lock_path;
    int *path_owner;                    // owner of the lock_condition mutex 

    int *termination_flags;         // flags use to detect the terminate condition
};

struct msg_data
{
    vertex_t *n;
    vertex_t *n_successor;
    int true_cost;
};


int terminate_detection(message_queue_t *msg_q, int *termination_flags, int n_flags);
int compute_recipient(vertex_t *v, int n);
void *thread_search_path(void *args);

int par_a_star_path(graph_t *graph, vertex_t *src, vertex_t *dst, int (*heuristic)(vertex_t *, vertex_t *), path_t **path, int n_threads_to_use)
{
    int i, result;
    int path_owner = -1;
    printf("ASTAR PATH PAR\n");

    // Check parameters are not null before starting
    util_check_r(graph != NULL, "Graph cannot be null, returning...\n", 0);
    util_check_r(src != NULL, "Source nodes cannot be null, returning...\n", 0);
    util_check_r(dst != NULL, "Destination nodes cannot be null, returning...\n", 0);

    message_queue_t *msg_q = message_queue_create(n_threads_to_use);

    pthread_t *threads = (pthread_t*) util_malloc(n_threads_to_use * sizeof(pthread_t));
    util_check_r(threads != NULL, "Could not allocate threads, returning...\n", 0);
    
    struct thread_d *thread_data = (struct  thread_d*) util_malloc(n_threads_to_use * sizeof(struct thread_d));
    util_check_r(thread_data != NULL, "Could not allocate thread_data, returning...\n", 0);
    
    heap_t **open_q = (heap_t **) util_malloc(n_threads_to_use * sizeof(heap_t*));
    util_check_r(thread_data != NULL, "Could not allocate array open set, returning...\n", 0);

    hash_table_t **closed_set = (hash_table_t **) util_malloc(n_threads_to_use * sizeof(hash_table_t*));
    util_check_r(thread_data != NULL, "Could not allocate array of close set, returning...\n", 0);

    pthread_mutex_t *lock_condition = (pthread_mutex_t*) util_malloc(sizeof(pthread_mutex_t));
    util_check_r(lock_condition != NULL, "Could not allocate condition mutex, returning...\n", 0);
    util_check_r(pthread_mutex_init(lock_condition, NULL) == 0, "Could not initialized condition mutex, returning...\n", 0);

    pthread_mutex_t *lock_path = (pthread_mutex_t*) util_malloc(sizeof(pthread_mutex_t));
    util_check_r(lock_path != NULL, "Could not allocate path mutex, returning...\n", 0);
    util_check_r(pthread_mutex_init(lock_path, NULL) == 0, "Could not initialized path mutex, returning...\n", 0);


    *path = (path_t *)util_malloc(sizeof(path_t));
    // Check allocation was successful
    util_check_r(path != NULL, "Could not allocate path, returning...\n", 0);
    (*path)->nodes = stack_create();
    // Check allocation was successful
    util_check_r((*path)->nodes != NULL, "Could not allocate path's stack, returning...\n", 0);
    // incumbent cost initialized to infinite
    (*path)->cost = INT_MAX;
    (*path)->visited_nodes = 0;
    (*path)->revisited_nodes = 0;

    int *termination_flags = (int *) util_calloc(n_threads_to_use, sizeof(int));

    printf("Creating %d threads\n", n_threads_to_use);

    for (i = 0; i < n_threads_to_use; i++){
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

        // Creates the priority queue which will contain unvisited nodes
        open_q[i] = heap_create(0, 10);
        // Check allocation was successful
        util_check_r(open_q[i]!= NULL, "Could not allocate open set, returning...\n", 0);
        thread_data[i].open_q = open_q[i];

        // Create closed nodes set, which will contain visited nodes
        closed_set[i] = hash_table_create(10);
        // Check allocation was successful
        util_check_r(closed_set != NULL, "Could not allocate closed set, returning...\n", 0);
        thread_data[i].closed_set = closed_set[i];
        
        thread_data[i].path = path;

        if(compute_recipient(src, n_threads_to_use) == i){
            // Init open with source
            fprintf(stdout, "src node with id %d assigned to thread %d\n", src->id, i);
            result = heap_insert(thread_data[i].open_q, src->id, (void *)src, 0);
            // Check no errors occurred
            util_check_r(result, "Could not insert in the open set, returning...\n", 0);
        }

        pthread_create(&threads[i], NULL, thread_search_path, (void *)&thread_data[i]);
    }

    for (i = 0; i < n_threads_to_use; i++){
       pthread_join(threads[i], NULL);
       hash_table_destroy(closed_set[i], NULL);
       heap_destroy(open_q[i], NULL);
    }

    if((*path)->cost == INT_MAX){
        // Path not found
        (*path)->nodes = NULL;
        return 1;
    }

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
    int visited_nodes=0, revisited_nodes=0, result, key, open_is_empty;
    struct thread_d *thread_data = (struct thread_d *)args;
    vertex_t *data;
    struct msg_data *m_data;

    while(terminate_detection(thread_data->msg_q, thread_data->termination_flags, thread_data->n_thread)){
        data = NULL;
        m_data = NULL;
        
        // check if thread message queue is empty
        while (message_queue_count(thread_data->msg_q, thread_data->id_thread) != 0)
        {
            // set flags for termination detection
            thread_data->termination_flags[thread_data->id_thread] = 0;

            m_data = (struct msg_data *) util_malloc(sizeof(struct msg_data));
            // Check allocation was successful
            util_check_r(m_data != NULL, "Could not allocate struct message to receive data, returning...\n", 0);

            data = (vertex_t *)util_malloc(sizeof(vertex_t));
            // Check allocation was successful
            util_check_r(data != NULL, "Could not allocate data to extract from closed set, returning...\n", 0);

            // Get and remove from message queue a triplet (n_successor, g(n) + c(n, n_successor), n);
            message_queue_receive(thread_data->msg_q, (void**)&m_data, thread_data->id_thread);
            fprintf(stdout, "Thread %d received message\n",thread_data->id_thread);
            
            result = hash_table_get(thread_data->closed_set, m_data->n_successor->id, (void **)&data);
            
            // Check no errors occurred
            util_check_r(result, "Could not get from the closed set, returning...\n", 0);

            int heuristic_cost = thread_data->heuristic == NULL ? 0 : thread_data->heuristic(m_data->n_successor, thread_data->dst);

            if (data != NULL)
            {
                fprintf(stdout, "Node with id %d already in closed set\n", m_data->n_successor->id);
                free(data);
                if (m_data->true_cost < m_data->n_successor->true_cost)
                {

                    fprintf(stdout, "Found smaller cost for node with id %d, readding it to open set\n", m_data->n_successor->id);
                    result = hash_table_delete(thread_data->closed_set, m_data->n_successor->id);

                    // Check no errors occurred
                    util_check_r(result, "Could not delete from the closed set, returning...\n", 0);

                    result = heap_insert(thread_data->open_q, m_data->n_successor->id, (void *)m_data->n_successor, m_data->true_cost + heuristic_cost);

                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);

                    revisited_nodes++;
                    free(m_data);
                }
                else
                {
                    free(m_data);
                    continue;
                }

            }
            else{
                fprintf(stdout, "Node with id %d not in closed set\n", m_data->n_successor->id);
                free(data);

                int *position = (int *)util_malloc(sizeof(int));

                // Check allocation was successful
                util_check_r(position != NULL, "Could not allocate position to extract from open set, returning...\n", 0);

                result = heap_find(thread_data->open_q, m_data->n_successor->id, &position);

                // Check no errors occurred
                util_check_r(result, "Could not search in the open set, returning...\n", 0);

                if (position == NULL)
                {
                    fprintf(stdout, "Add node with id %d to open set\n", m_data->n_successor->id);
                    result = heap_insert(thread_data->open_q, m_data->n_successor->id, (void *)m_data->n_successor, m_data->true_cost);
                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the open set, returning...\n", 0);
                }
                else if (m_data->true_cost >= m_data->n_successor->true_cost)
                {
                    free(m_data);
                    free(position);
                    continue;
                }

                fprintf(stdout, "Updating costs and parent for node with id %d to open set\n", m_data->n_successor->id);
                m_data->n_successor->true_cost = m_data->true_cost;
                m_data->n_successor->heuristic_cost = heuristic_cost;
                m_data->n_successor->parent = m_data->n;
            }
        }

        
        open_is_empty = heap_is_empty(thread_data->open_q);
        fprintf(stdout, "Thread %d, Heap is empty: %d\n", thread_data->id_thread, open_is_empty);

        if(!open_is_empty)
        {
            fprintf(stdout, "Extracting data from open set\n");
            result = heap_extract(thread_data->open_q, (void*)&data, &key);
            // Check no errors occurred
            util_check_r(result, "Could not extract from the open set, returning...\n", 0);
        }

        if(open_is_empty || (data->true_cost + data->heuristic_cost) >= (*thread_data->path)->cost){
            thread_data->termination_flags[thread_data->id_thread] = 1;
            if(data != NULL)
                heap_insert(thread_data->open_q, key, data, data->true_cost + data->heuristic_cost);
            continue;
        }

        fprintf(stdout, "Add node with id %d to closed set\n", data->id);
        result = hash_table_insert(thread_data->closed_set, data->id, (void *)data);
        // Check no errors occurred
        util_check_r(result, "Could not insert in the closed set, returning...\n", 0);

        visited_nodes++;

        if (data->id == thread_data->dst->id){
            fprintf(stdout, "thread %d arrived to goal node with id %d\n", thread_data->id_thread, data->id);
          
            // acquire lock for checking condition
            pthread_mutex_lock(thread_data->lock_condition);
            if(data->true_cost < (*thread_data->path)->cost)
            {
                // set current thread as the path owner (can modify path->nodes)
                (*thread_data->path_owner) = thread_data->id_thread;
                // updating path cost, and additional info
                (*thread_data->path)->cost = data->true_cost;
                (*thread_data->path)->visited_nodes += visited_nodes;
                (*thread_data->path)->revisited_nodes += revisited_nodes;
            }
            // unlock mutex
            pthread_mutex_unlock(thread_data->lock_condition);

            // check if current thread is still the owner of the lock
            if((*thread_data->path_owner) == thread_data->id_thread){
                // acquire path lock
                pthread_mutex_lock(thread_data->lock_path);

                vertex_t *n = thread_data->dst;
                // destroy previous path and create a new one
                stack_destroy((*thread_data->path)->nodes, NULL);
                (*thread_data->path)->nodes = stack_create();
                // Check allocation was successful
                util_check_r((*thread_data->path)->nodes != NULL, "Could not allocate path's stack, returning...\n", 0);

                // check that current thread can modify the path (another thread has found a better solution)
                while (n != NULL && (*thread_data->path_owner) == thread_data->id_thread)
                {
                    result = stack_push((*thread_data->path)->nodes, (void *)&(n->id));
                    // Check no errors occurred
                    util_check_r(result, "Could not insert in the path's stack, returning...\n", 0);
                    n = n->parent;
                }
                pthread_mutex_unlock(thread_data->lock_path);
            }
        }

        edge_t *e = data->head;

        while (e != NULL)
        {        
            fprintf(stdout, "Checking edge from node with id %d to node with id %d\n", data->id, e->dest->id);
            int true_cost = data->true_cost + e->weight;

            m_data = (struct msg_data *) util_malloc(sizeof(struct msg_data));
            // Check allocation was successful
            util_check_r(m_data != NULL, "Could not allocate struct message to send data, returning...\n", 0);

            m_data->true_cost = true_cost;
            m_data->n = data;
            m_data->n_successor = e->dest;
            
            fprintf(stdout, "Thread %d send message to %d\n",thread_data->id_thread, compute_recipient(e->dest, thread_data->n_thread));
            message_queue_send(thread_data->msg_q, (void*)m_data, compute_recipient(e->dest, thread_data->n_thread));

            e = e->next;
        }
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
int terminate_detection(message_queue_t *msg_q, int *termination_flags, int n_flags){
    for(int i; i < n_flags; i++)
        if(termination_flags[i] == 0)
            return 1;
        
    if(message_queue_all_empty(msg_q))
        return 0;

    return 1;
}


int compute_recipient(vertex_t *v, int n){
    return v->id % n;
}