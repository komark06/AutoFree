#if !defined(__AUTOFREE_H__)
#define __AUTOFREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  //size_t

// allocate memory by malloc
void *evamalloc(size_t len);

// allocate memory by calloc
void *evacalloc(size_t elem, size_t len);

// allocate memory by realloc
void *evarealloc(void *ptr, size_t len);

// free memory by free
void evafree(void *ptr);

// free all memory chuck
void evaAutoFree(void);

#ifdef __cplusplus
}
#endif

#endif /* !defined(__AUTOFREE_H__) */
