#ifndef __OE_MALLOC_H__

#define __OE_MALLOC_H_

int  mallocinit();
#define malloc oe_malloc
#define free  oe_free
void *oe_malloc(size_t size);
void oe_free(void *p);
int  mallocterminate();

#endif

