#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "include/AutoFree.h"

#define DEADBEEF (void*)0xDEADBEEF
#define MAXTHREAD 10

void *crazy(void *no)
{
    if (no)
        pthread_exit(DEADBEEF);
    for(int i=0; i < 100;++i){
        char *ptr = evamalloc(10);
        if (!ptr)
            pthread_exit(DEADBEEF);
        //ptr = evacalloc(1,10);
        //if (!ptr)
        //    pthread_exit(DEADBEEF);
         
        char *ne = evarealloc(ptr,100);
        if (!ne)
            pthread_exit(DEADBEEF);
        //printf("evamalloc = %p\tevarealloc = %p\n",(void*)ptr,(void*)ne);
    }
    evaAutoFree();
    pthread_exit(NULL);
}

int main(void)
{
    printf("pid:%d\n",getpid());
    pthread_t tid[MAXTHREAD];
    int total = 0;
    for(; total < MAXTHREAD; ++total){
        int err = pthread_create(tid+total,NULL,crazy,NULL);
        if (err){
            show_err("pthread_create",err);
            break;
        }
    }
    for(int i = 0; i < total;++i){
        void *ptr;
        printf("tid[%d] is waiting...",i);
        fflush(stdout);
        int err = pthread_join(tid[i],&ptr);
        if (err){
            show_err("pthread_join",err);
            continue;
        }
        if (ptr == DEADBEEF)
            fprintf(stderr,"tid[%d] failed\n",i);
        else
            printf("tid[%d] completed.\n",i);
    }
    evaAutoFree();
    return 0;
}