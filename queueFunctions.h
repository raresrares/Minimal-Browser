/*
 *  This file contains only queue functions.
 * 
 */

void* InitQ(size_t size) {
    TQueue queue;

    queue = (TQueue) calloc(1, sizeof(Queue));
    
    if(!queue) return NULL;
    
    queue->size = size;
    queue->fq = NULL;
    queue->lq = NULL;
    
    return (void*)queue;
} 

int EnQ(void* queue, void* info) {
    TCelQ elem = (TCelQ) calloc(1, sizeof(CelQ));
    if (!elem) return 0;
    
    elem->info = calloc(1, SIZE_QUEUE(queue));
    if (!elem->info) return 0;

    memcpy(elem->info, info, SIZE_QUEUE(queue));
    elem->next = NULL;

    if(EMPTY_QUEUE(queue)) {
        L_QUEUE(queue) = elem;
        F_QUEUE(queue) = elem;
    } else {
        L_QUEUE(queue)->next = elem;
        L_QUEUE(queue) = elem;
    }

    return 1;
}

int DeQ(void* queue, void* info) {
    if(EMPTY_QUEUE(queue)) return 0;

    memcpy(info, F_QUEUE(queue)->info, SIZE_QUEUE(queue));

    TCelQ aux = F_QUEUE(queue);

    aux = F_QUEUE(queue);
    F_QUEUE(queue) = aux->next;

    if (F_QUEUE(queue) == NULL) { /* Only one element in queue */
        L_QUEUE(queue) = NULL;
    }
    
    free(aux->info);
    free(aux);

    return 1;
}