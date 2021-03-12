/*
 *  This file contains definitions of structures & functions. 
 * 
 */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* Macrodefinitions so I don't lose my mind */
#define STACK(s) (((TStack)s))
#define TOP_STACK(s) ((STACK(s))->top)
#define EMPTY_STACK(s) (TOP_STACK(s) == NULL)
#define SIZE_STACK(s) ((STACK(s))->size)

#define QUEUE(q) (((TQueue)q))
#define F_QUEUE(q) ((QUEUE(q))->fq)
#define L_QUEUE(q) ((QUEUE(q))->lq)
#define EMPTY_QUEUE(q) (F_QUEUE(q) == NULL && L_QUEUE(q) == NULL)
#define SIZE_QUEUE(q) ((QUEUE(q))->size)

#define RESOURCE(r) (((Resource *)r))
#define PRIORITY(r) (RESOURCE(r)->dimension - RESOURCE(r)->currently_downloaded)

typedef struct celS {
    void * info;          /* adress of information */
    struct celS *next;    /* adress of the next stack cell */
} CelS, *TCelS;

typedef struct stack { 
    size_t size;         /* size of element */
    TCelS top;           /* adress of the top of the stack */
} Stack, *TStack;

typedef struct celQ { 
    void * info;          /* adress of information */       
    struct celQ *next;    /* adress of the next queue cell */
} CelQ, *TCelQ;

typedef struct queue {
    size_t size;          /* size of element */
    TCelQ fq, lq;         /* adress of the first & last queue cell */
} Queue, *TQueue;

typedef struct resource{
    char name[100];
    unsigned long dimension;
    unsigned long currently_downloaded;
} Resource;

typedef struct webpage {
    char url[30];

    unsigned int num_res;
    Resource *resources;
} Webpage, *TWebpage;

typedef struct tab {
    TWebpage current_page;

    void* back_stack;
    void* forward_stack;

    struct tab *next;
} Tab, *TTab;

typedef struct browser {
    TTab tablist;

    void* global_history; /* queue */
    void* download_priority; /* queue */
    void* completed; /* queue */

    TTab current_tab;

    long bandwidth;
} Browser, *TBrowser;

Resource* get_page_resources(const char *URL_Name, int *n);

void* InitS(size_t size);
int PushS(void* stack, void* info);
int PopS(void* stack, void* info);

void* InitQ(size_t size);
void printQ (TCelQ head);
int EnQ(void* queue, void* info);
int DeQ(void* queue, void* elem);

TBrowser Init_Browser();
void delbrowser(TBrowser *browser);

void newtab(TBrowser browser);
void change_tab(TBrowser browser, char * parameter);
void deltab(TBrowser browser);
void print_open_tabs(TBrowser browser);

void back(TBrowser browser);
void go_to(TBrowser browser, char * parameter);
void forward(TBrowser browser);

void history(TBrowser browser);
void del_history(TBrowser browser, char * parameter);

void list_dl(TBrowser browser);
void downloads(TBrowser browser);
void printQ(TCelQ head);

void download(TBrowser browser, char * parameter);
void wait(TBrowser browser, char * parameter);

void bubbleSort(TCelQ start);
void swap(TCelQ  a, TCelQ b);