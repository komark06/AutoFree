# AutoFree
This project aims to solve the problem of forgetting to free memory in C.

When using `malloc` to allocate memory, it needs to be `free` after being used.
It is possible to forget to free memory, which will cause **memory leak**.
In this project, it uses doubly linked list to track down every memory block that has been allocated.
In order to free all memory blocks, you only need to call `evaAutoFree`.

# How to use
To track down memory blocks, you need to allocate memory through the below functions.
```c
void* evamalloc(size_t len);
void* evacalloc(size_t elem, size_t len);
void* evarealloc(void *ptr, size_t len);
void evafree(void *ptr);
void evaAutoFree(void);
```
Its feature can be deduced from function name. 
`evamalloc` is the replacement for `malloc`. `evacalloc` is the replacement for `calloc`. The same rule also applies to `evarealloc` and `evafree`.
Use them as you normally do. When you want to free all memory blocks, just call `evaAutoFree`.

# Contributing
Use pull requests.
Install git hooks to make sure you have a consistent coding style and static analytics.
```shell
make hooks
```

# License
AutoFree is released under the BDS two-clause license. See the LICENSE file for more details.
