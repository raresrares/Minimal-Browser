/*
 *  This file contains only stack functions.
 *
 */

void* InitS(size_t size) {
    TStack stack;

    stack = (TStack) calloc(1, sizeof(Stack));
    
    if(!stack) return NULL;
    
    stack->size = size;
    stack->top = NULL;
    
    return (void*)stack;
}

int PushS(void* stack, void* info) {
    TCelS elem = (TCelS) calloc(1, sizeof(CelS));
    if (!elem) return 0;

    elem->info = calloc(1, SIZE_STACK(stack));
    if (!elem->info) return 0;

    memcpy(elem->info, info, SIZE_STACK(stack));

    if (EMPTY_STACK(stack)) {
        TOP_STACK(stack) = elem; 
    } else {
        elem->next = TOP_STACK(stack);
        TOP_STACK(stack) = elem;
    }

    return 1;
}

int PopS(void* stack, void * info) {
    if (EMPTY_STACK(stack)) return 0;

    memcpy(info, TOP_STACK(stack)->info, SIZE_STACK(stack));

    TCelS aux = TOP_STACK(stack);

    TOP_STACK(stack) = aux->next;
    
    free(aux->info);
    free(aux);

    return 1;
}