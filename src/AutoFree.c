#ifdef __cplusplus
extern "C" {
#endif

#include "AutoFree.h"
#include <pthread.h>
#include <stdint.h>
#include "alloc.h"
#include "compiler.h"
#include "linux-like-list.h"

struct memblock {
    struct list_head list;
    char data[];
};
typedef struct memblock block_t;

/* global variable */
static struct list_head head = {&head, &head};
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static inline void *init_alloc(block_t *obj)
{
    if (unlikely(!obj))
        return NULL;
    pthread_mutex_lock(&lock);
    list_add(&obj->list, &head);
    pthread_mutex_unlock(&lock);
    return obj->data;
}

void *evamalloc(size_t len)
{
    if (unlikely(SIZE_MAX - sizeof(block_t) < len))
        return NULL;
    return init_alloc(e_malloc(sizeof(block_t) + len));
}

void *evacalloc(size_t elem, size_t len)
{
    if (unlikely((SIZE_MAX - sizeof(block_t)) / elem < len))
        return NULL;
    return init_alloc(e_calloc(1, sizeof(block_t) + elem * len));
}

void *evarealloc(void *ptr, size_t len)
{
    if (!ptr)
        return evamalloc(len);
    if (unlikely(SIZE_MAX - sizeof(block_t) < len))
        return NULL;
    // cppcheck-suppress nullPointer
    block_t *old = container_of(ptr, block_t, data);
    pthread_mutex_lock(&lock);
    if (node_exist(&old->list, &head) == 0) {  // node is not on the list
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    block_t *ne = e_realloc(old, sizeof(block_t) + len);
    if (unlikely(!ne)) {
        pthread_mutex_unlock(&lock);
        return NULL;
    } else if (ne != old) {
        ne->list.prev->next = &ne->list;
        ne->list.next->prev = &ne->list;
    }
    pthread_mutex_unlock(&lock);
    return ne->data;
}

void evafree(void *ptr)
{
    if (!ptr)
        return;
    // cppcheck-suppress nullPointer
    block_t *obj = container_of(ptr, block_t, data);
    pthread_mutex_lock(&lock);
    list_del(&obj->list);
    pthread_mutex_unlock(&lock);
    e_free(obj);
}

void evaAutoFree(void)
{
    pthread_mutex_lock(&lock);
    if (list_empty(&head))
        goto leave;
    struct list_head *node, *safe;
    for (node = head.next, safe = node->next; node != &head;
         node = safe, safe = node->next) {
        // cppcheck-suppress nullPointer
        e_free(container_of(node, block_t, list));
    }
    init_list_head(&head);
leave:
    pthread_mutex_unlock(&lock);
}

#if defined(__AUTOFREE_TEST__)

#include <stdio.h>
#include <string.h>

#define SMALL_CHUNK_SIZE 10
#define BIG_CHUNK_SZIE SMALL_CHUNK_SIZE * 2

#define MAX_THREAD 1000u

#define DEADBEEF (void *) 0xdeadbeef

static void show_err(const char *str, const int err)
{
    fprintf(stderr, "%s: %s\n", str, strerror(err));
}

// cppcheck-suppress constParameter
void *check(void *id)
{
    if (id)
        pthread_exit(DEADBEEF);
    for (size_t i = 0; i < 100; ++i) {
        evacalloc(1, SMALL_CHUNK_SIZE);
        void *ptr = evamalloc(SMALL_CHUNK_SIZE);
        if (!ptr)
            pthread_exit(DEADBEEF);
        evarealloc(ptr, BIG_CHUNK_SZIE);
    }
    // evaAutoFree();
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid[MAX_THREAD];
    unsigned int total = 0;
    printf("Create %u threads to test.\n", MAX_THREAD);
    for (; total < MAX_THREAD; ++total) {
        int err = pthread_create(tid + total, NULL, check, NULL);
        if (err) {
            fprintf(stderr, "tid[%u]", total);
            show_err("pthread_create", err);
            break;
        }
    }
    for (unsigned int i = 0; i < total; ++i) {
        void *ptr;
        int err = pthread_join(tid[i], &ptr);
        if (err) {
            fprintf(stderr, "tid[%u]", i);
            show_err("pthread_join", err);
            continue;
        }
        if (ptr)
            fprintf(stderr, "tid[%u] evamalloc failed\n", i);
    }
    // evaAutoFree();
    puts("DONE");
    return 0;
}

#endif  // defined(__AUTOFREE_TEST__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
