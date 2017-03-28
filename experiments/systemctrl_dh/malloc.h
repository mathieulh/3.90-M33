#ifndef ____MALLOC_H____

#define ____MALLOC_H____

int  mallocinit();
void *malloc(size_t size);
void free(void *p);
int  mallocterminate();

#endif

