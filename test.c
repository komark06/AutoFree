#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "include/AutoFree.h"

#define DEADBEEF (void*)0xDEADBEEF
#define MAXTHREAD 100
#define TIMES 10
#define NORMAL_BLOCK_SIZE 100
#define LARGER_BLOCK_SIZE 1024

#define XSTR(x) STR(x)
#define STR(x) #x

#define TOTAL_SIZE (MAXTHREAD*TIMES*LARGER_BLOCK_SIZE)
#if TOTAL_SIZE > 1048576
#error "total size is too large. Muse be smaller than " XSTR(TOTAL_SIZE) 
#endif

void *crazy(void *p)
{
    if (!p)
        pthread_exit(DEADBEEF);
    unsigned int id = *(unsigned int*)p;
    for(int i = 0; i < TIMES;++i){
        char *ptr;
        ptr = evamalloc(NORMAL_BLOCK_SIZE);
        if (!ptr)
            pthread_exit(DEADBEEF);
        char *ne = evarealloc(ptr,LARGER_BLOCK_SIZE);
        if (!ne)
            pthread_exit(DEADBEEF);
        printf("%u: %d times.\n",id,i);
    }
    evaAutoFree();
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid[MAXTHREAD];
    unsigned int id[MAXTHREAD];
    for(int i = 0; i < MAXTHREAD; ++i)
        id[i] = i+1;
    int total = 0;
    for(; total < MAXTHREAD; ++total){
        int err = pthread_create(tid+total,NULL,crazy,id+total);
        if (err){
            show_err("pthread_create",err);
            break;
        }
    }
    for(int i = 0; i < total;++i){
        void *ptr;
        int err = pthread_join(tid[i],&ptr);
        if (err){
            show_err("pthread_join",err);
            continue;
        }
        if (ptr == DEADBEEF)
            fprintf(stderr,"tid[%d] failed\n",i);
        else
            printf("tid[%d] completed.\n",i);
        fflush(stdout);
    }
    evaAutoFree();
    puts("Finish!!!");
    printf("Probale maximum size: %u\n",TOTAL_SIZE);
    return 0;
}