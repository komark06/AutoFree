#if !defined(__ALLOC_H__)
#define __ALLOC_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void *e_malloc(size_t size)
{
    return malloc(size);
}

static inline void e_free(void *ptr)
{
    free(ptr);
}

static inline void *e_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

static inline void *e_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // !defined(__ALLOC_H__)