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
    block_t *ne = e_realloc(old, sizeof(block_t) + len);
    if (unlikely(!ne))
        return NULL;
    if (ne != old) {
        pthread_mutex_lock(&lock);
        list_del(&old->list);
        list_add(&ne->list, &head);
        pthread_mutex_unlock(&lock);
    }
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
        return;
    struct list_head *node, *safe;
    for (node = head.next, safe = node->next; node != &head;
         node = safe, safe = node->next) {
        // cppcheck-suppress nullPointer
        e_free(container_of(node, block_t, list));
    }
    init_list_head(&head);
    pthread_mutex_unlock(&lock);
}

#if defined(__AUTOFREE_TEST__)

#include <stdio.h>

int main(void)
{
    for (size_t i = 0; i < 1000000; ++i) {
        evacalloc(1, 100);
        void *ptr = evamalloc(100);
        evarealloc(ptr, 10);
    }

    evaAutoFree();
    return 0;
}

#endif  // defined(__AUTOFREE_TEST__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
