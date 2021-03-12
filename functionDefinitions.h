/*
 *  This file contains only functions. 
 */

Resource* get_page_resources(const char *URL_Name, int *n){
    int i, j, name_len, url_name_len = strlen(URL_Name);

    unsigned int resource_nr = 0;
    uint32_t size = 0;
    if(URL_Name == NULL){
        return NULL;
    }

    for(i = 0; i < url_name_len; i++){
        resource_nr += URL_Name[i];
    }

    resource_nr %= 13;
    Resource *result = (Resource *)calloc(resource_nr, sizeof(Resource));
    *n = resource_nr;
    for(i = 0; i < resource_nr; i++){
        sprintf(result[i].name, "%s-(%d)", URL_Name, i);
        size = 0;
	name_len = strlen(result[i].name);

        for(j = 0; j < name_len; j++){
            size += result[i].name[j];
        }
        /* Some randomness */
        size ^= size << 3;
        size += size >> 5;
        size ^= size << 4;
        size += size >> 17;
        size ^= size << 25;
        size += size >> 6;

        /* 100MB Maximum size */
        result[i].dimension = size % 104857601;
        result[i].currently_downloaded = 0;
    }

    return result;
}

TBrowser Init_Browser() {
    TBrowser browser = (TBrowser) calloc(1, sizeof(Browser));
    if (!browser) return NULL;
    
    browser->tablist = NULL;
    browser->global_history = (TQueue) calloc(1, sizeof(Queue)); 
    if (!browser->global_history) return NULL;

    browser->download_priority = (TQueue) calloc(1, sizeof(Queue));
    if (!browser->download_priority) return NULL;
    
    browser->completed = (TQueue) calloc(1, sizeof(Queue));
    if (!browser->completed) return NULL;

    browser->current_tab = NULL;

    QUEUE(browser->global_history)->size = sizeof(Webpage);
    QUEUE(browser->download_priority)->size = sizeof(Resource);
    QUEUE(browser->completed)->size = sizeof(Resource);

    return browser;
}

void set_band(TBrowser browser, char * parameter) {
    browser->bandwidth = ((long)atof(parameter));
}

void newtab(TBrowser browser) {
    TTab tab, p;

    /* Allocate everything that's needed */
    tab = (TTab) calloc(1, sizeof(Tab));
    if (!tab) return;

    tab->current_page = (TWebpage) calloc(1, sizeof(Webpage));
    if (!tab->current_page) return;

    tab->back_stack = (void *) calloc(1, sizeof(Stack));
    if (!tab->back_stack) return;

    tab->forward_stack = (void *) calloc(1, sizeof(Stack));
    if (!tab->forward_stack) return;

    tab->next = NULL;

    /* Set the size of the back and forward stack */
    STACK(tab->back_stack)->size = sizeof(Webpage);
    STACK(tab->forward_stack)->size = sizeof(Webpage);

    /* Bring p to the end of the tab list then insert the tab */
    if (browser->tablist == NULL) {
        /* Or the start of it is the list is empty */
        browser->tablist = tab;
    } else {
        for (p = browser->tablist; p->next != NULL; p = p->next);
        p->next = tab;
    }

    /* Sets the current tab to the one that has been opened */
    browser->current_tab = tab;
}

void deltab(TBrowser browser) {
    TTab p = browser->tablist, previous = NULL;

    /* Brings p to the end of the tablist then deletes the tab */
    if (browser->tablist->next != NULL) {
        for(p = browser->tablist; p->next != NULL; previous = p, p = p->next);

        if (browser->current_tab == p)
            browser->current_tab = previous;

        previous->next = NULL;
    }

    free(p->back_stack);
    free(p->current_page);
    free(p->forward_stack);
    free(p);
}

void delbrowser(TBrowser *browser) {
    free((*browser)->global_history);
    free((*browser)->download_priority);

    TBrowser p = *browser;

    free(p);
}

void change_tab(TBrowser browser, char * parameter) {
    TTab p;
    int i = 0;
    int tab_index = ((int)atof(parameter));

    /* Changes the current tab */
    for (p = browser->tablist; i < tab_index; p = p->next, i++);
    
    browser->current_tab = p;
}

void print_open_tabs(TBrowser browser) {
    TTab p;
    int i = 0;

    for(p = browser->tablist; p != NULL; p = p->next, i++) {
        if (!strcmp(p->current_page->url, "")) {
            printf("(%d: empty)\n", i);
        } else {
            printf("(%d: %s)\n", i, p->current_page->url);
        }
    }
}

void go_to(TBrowser browser, char * parameter) {
    char url[21];

    strcpy(url, parameter);

    /* Only used for Push and Pop */
    TWebpage garbage = (TWebpage) calloc(1, sizeof(Webpage));
    if (!garbage) return;

    /* If it has something in it*/
    if (strcmp(browser->current_tab->current_page->url, "")) {
        PushS(browser->current_tab->back_stack,
              browser->current_tab->current_page);

        /* Empty the forward stack and free its elements */
        while (!EMPTY_STACK(browser->current_tab->forward_stack)) {
            PopS(browser->current_tab->forward_stack, garbage);
        }
    }

    browser->current_tab->current_page->resources =
        get_page_resources(url, &(browser->current_tab->current_page->num_res));

    strcpy(browser->current_tab->current_page->url, url);

    /* Adds page to the global history queue */
    EnQ(browser->global_history, browser->current_tab->current_page);
}

void back(TBrowser browser) {
    void * webpage = (TWebpage) calloc(1, sizeof(Webpage));
    if (!webpage) return;

    /* Push the current page in the forward stack, then Pop the top webpage
     * from the back stack and make it the current one */
    if (!EMPTY_STACK(browser->current_tab->back_stack)) {
        PushS(browser->current_tab->forward_stack, 
              browser->current_tab->current_page);

        PopS(browser->current_tab->back_stack, webpage);

        memcpy(browser->current_tab->current_page, webpage, sizeof(Webpage));
    } else {
        printf("can't go back, no pages in stack\n");
    }
}

void forward(TBrowser browser) {
    void * webpage = (TWebpage) calloc(1, sizeof(Webpage));
    if (!webpage) return;

    /* Push the current page in the back stack, then Pop the top webpage
     * from the forward stack and make it the current one */
    if (!EMPTY_STACK(browser->current_tab->forward_stack)) {
        PushS(browser->current_tab->back_stack,
              browser->current_tab->current_page);

        PopS(browser->current_tab->forward_stack, webpage);

        memcpy(browser->current_tab->current_page, webpage, sizeof(Webpage));
    } else {
        printf("can't go forward, no pages in stack\n");
    }
}

void history(TBrowser browser) {
    void * webpage = (TWebpage) calloc(1, sizeof(Webpage));
    if (!webpage) return;

    /* Auxiliary queue used for printing the browser history */
    void * auxQ = InitQ(sizeof(Webpage));
    if (!auxQ) return;

    while(!EMPTY_QUEUE(browser->global_history)) {
        DeQ(browser->global_history, webpage);

        printf("%s\n", ((TWebpage)webpage)->url);
        EnQ(auxQ, webpage);
    }

    while(!EMPTY_QUEUE(auxQ)) {
        DeQ(auxQ, webpage);

        EnQ(browser->global_history, webpage);
    }
}

void del_history(TBrowser browser, char * parameter) {
    int nr_entries = ((int)atof(parameter));
    void * webpage = (TWebpage) calloc(1, sizeof(Webpage));
    if (!webpage) return;

    if (nr_entries == 0) {
        while(!EMPTY_QUEUE(browser->global_history)) {
            DeQ(browser->global_history, webpage);
        }
    } else {
        for (int i = 0; i < nr_entries; i++) {
            DeQ(browser->global_history, webpage);
        }
    }
}

void list_dl(TBrowser browser) {
    if (browser->current_tab->current_page == NULL) return;
    if ((browser->current_tab->current_page->resources) == NULL) return;

    for(int i = 0; i < browser->current_tab->current_page->num_res; i++) {
        printf("[%d - ", i);

        printf("\"%s\" ", 
               browser->current_tab->current_page->resources[i].name);

        printf(": %ld]\n",
               browser->current_tab->current_page->resources[i].dimension);                                       
    }
}

void download(TBrowser browser, char * parameter) {
    TCelQ i;
    int index = ((int)atof(parameter));
    Resource * resource = (Resource *) calloc(1, sizeof(Resource));
    if (!resource) return;

    if (browser->current_tab->current_page == NULL) return;
    if ((browser->current_tab->current_page->resources) == NULL) return;

    /* Adds the resource to the download_priority queue then sorts it
     * based on the resource's priority */

    memcpy (resource, browser->current_tab->current_page->resources + index,
            sizeof(Resource));    

    resource->currently_downloaded = 0;

    EnQ (browser->download_priority, resource);
    bubbleSort(F_QUEUE(browser->download_priority));

    for (i = F_QUEUE(browser->download_priority); i->next != NULL; i = i->next);
    L_QUEUE(browser->download_priority) = i;
}

void wait(TBrowser browser, char * parameter) {
    long time = ((long)atof(parameter));
    long bytes = time * (browser->bandwidth);
    void * aux = (Resource *) calloc(1, sizeof(Resource));
    if (!aux) return;

    long remaining, dimension, currently_downloaded;
    Resource * resource;
    TCelQ i;

    while (bytes != 0) {
        resource = (Resource *)(F_QUEUE(browser->download_priority)->info);

        dimension = resource->dimension;
        currently_downloaded = resource->currently_downloaded;

        remaining = dimension - currently_downloaded;

        /* I'm calculating the remaint of the resource in the top of the queue
         * then I'm comparing it with the bytes i.e. time * bandwidth */

        if (bytes == remaining) {
            resource->currently_downloaded = dimension;

            DeQ(browser->download_priority, aux);
            EnQ(browser->completed, aux);

            bytes = 0;
        } else if (bytes < remaining) {
            resource->currently_downloaded = resource->currently_downloaded + bytes;           

            bytes = 0;
        } else if (bytes > remaining) {
            resource->currently_downloaded = dimension;

            DeQ(browser->download_priority, aux);
            EnQ(browser->completed, aux);

            bytes -= remaining;
        }
    }

    /* After this I sort the queues */
    bubbleSort(F_QUEUE(browser->download_priority));
    
    for (i = F_QUEUE(browser->download_priority); i->next != NULL; i = i->next);
    L_QUEUE(browser->download_priority) = i;

    bubbleSort(F_QUEUE(browser->completed));
}

void downloads(TBrowser browser) { 
    if (browser->current_tab->current_page == NULL) return;
    printQ(F_QUEUE(browser->download_priority));
    printQ(F_QUEUE(browser->completed));
}

void printQ(TCelQ head) {
    if (head == NULL) 
       return;

    Resource * resource = (Resource *)head->info;

    printf("[\"%s\" ", resource->name);

    if (resource->currently_downloaded == resource->dimension) {
        printf(": completed]\n");
    } else if (resource->currently_downloaded == 0) {
        printf(": %ld/%ld]\n", resource->dimension, resource->dimension);
    } else {
        printf(": %ld/%ld]\n",
               resource->dimension - resource->currently_downloaded,
               resource->dimension);
    }
    
    printQ(head->next);
}

void bubbleSort(TCelQ start) { 
    int swapped , i; 
    TCelQ ptr1; 
    TCelQ lptr = NULL; 
  
    if (start == NULL) return;

    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->next != lptr) 
        { 
            if (PRIORITY(ptr1->info) > PRIORITY(ptr1->next->info))
            {
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            }
            ptr1 = ptr1->next; 
        }

        lptr = ptr1; 
    } while (swapped);
}
  
void swap(TCelQ a, TCelQ b) { 
    void * temp = a->info; 
    a->info = b->info; 
    b->info = temp; 
}

