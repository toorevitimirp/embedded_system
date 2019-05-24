/****************************************************************************/

/*
 *	fastmem.c -- Simple routines to access fast memory.
 *
 *	(C) Copyright 2002, Greg Ungerer (gerg@snapgear.com)
 */

/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "../myhead.h"
#include "fastmem.h"

/****************Table in sram****************/
#ifdef SRAM_OPT_TABLE
void	*sram_table_base = (void *)SRAM_TABLE_OFFSET;
size_t	sram_table_alloced;

void * table_apply_sram(int size)
{
	void *p;
	int i;

	p = sram_table_base;
	sram_table_base += size;
	sram_table_alloced += size;
	if(sram_table_alloced > SRAM_TABLE_MAXLEN){
		fprintf(stderr, "sram table overflowed!\n");
		exit(-1);
	}
	PDEBUG("sram table allocated:%d\n", sram_table_alloced);

	return (p);
}

void get_sram_table_info(void **p, int *alloced)
{
	*p = (void *)SRAM_TABLE_OFFSET;
	*alloced=sram_table_alloced;
}
#endif


/****************Fast buffer****************/
#ifdef SRAM_OPT_BUF
void	*sram_buf_base = (void *)SRAM_BUF_OFFSET;
size_t	sram_buf_alloced;

void *sram_alloc_buf(size_t size)
{
	void *p;
	int i;

	p = sram_buf_base;
	sram_buf_base += size;
	sram_buf_alloced += size;
	if(sram_buf_alloced > SRAM_BUF_MAXLEN){
		fprintf(stderr, "sram buffer overflowed!\n");
		exit(-1);
	}
	PDEBUG("sram buffer:0x%p, allocated size:%d\n", p, sram_buf_alloced);


	return(p);
}

void get_sram_buf_info(void **p, int *alloced)
{
	*p = (void*)SRAM_BUF_OFFSET;
	*alloced=sram_buf_alloced;
}
#endif
/****************Fast malloc****************/
#ifdef SRAM_OPT_HEAP
#define	MAXALLOCS	24	
struct atable {
	int	alloced;
	void	*p;
	size_t	size;
};

struct atable	fmem_alloctable[MAXALLOCS];
int		fmem_atablei;

void	*fmem_base = (void *)SRAM_OFFSET_BEGIN;
size_t	fmem_alloced;

void *fmalloc(size_t size)
{
	void *p;
	int i;

	/* See if exact match already */
	for (i = 0; (i < MAXALLOCS); i++) {
		if (size == fmem_alloctable[i].size) {
			if (fmem_alloctable[i].alloced == 0) {
				fmem_alloctable[i].alloced = 1;
				return(fmem_alloctable[i].p);
			}
		}
	}

	p = fmem_base;
	fmem_base += size;
	fmem_alloced += size;
	fmem_alloctable[fmem_atablei].alloced = 1;
	fmem_alloctable[fmem_atablei].size = size;
	fmem_alloctable[fmem_atablei].p = p;
	fmem_atablei++;
	if(fmem_alloced > SRAM_HEAP_MAXLEN){
		fprintf(stderr, "Heap overflowed!\n");
		exit(-1);
	}

	return(p);
}

void ffree(void *ptr)
{
	int i;

	for (i = 0; (i < MAXALLOCS); i++) {
		if (ptr == fmem_alloctable[i].p) {
			fmem_alloctable[i].alloced = 0;
			return;
		}
	}

	//free(ptr);
}

void get_sram_heap_info(void **p, int *alloced)
{
	*p = (void*)SRAM_OFFSET_BEGIN;
	*alloced=fmem_alloced;
}

#else
	void *fmalloc(size_t size)
{
	return(malloc(size));
}

void ffree(void *ptr)
{
	free(ptr);
}
#endif /* SRAM_OPT_HEAP */

void print_sram_usage()
{
	int size;
	void *p;

	printf("---------------------\n");
	printf("SRAM usage(Address:0x%x, Total length:0x%x):\n", SRAM_OFFSET_BEGIN, SRAM_LENGTH);
#ifdef SRAM_OPT_HEAP
	get_sram_heap_info(&p, &size);
	printf("\tHeap usage: \t%p-%p\t\t0x%x\n", p, p+size, size);
#endif
#ifdef MOV_SP
	printf("\tStack usage: \t%p-%p\t\t0x%x\n", (void*)SRAM_STACK_OFFSET, (void*)SRAM_STACK_OFFSET+SRAM_STACK_MAXLEN, SRAM_STACK_MAXLEN);
#endif
#ifdef SRAM_OPT_TABLE
	get_sram_table_info(&p, &size);
	printf("\tTable usage: \t%p-%p\t\t0x%x\n", p, p+size, size);
#endif
#ifdef SRAM_OPT_BUF
	get_sram_buf_info(&p, &size);
	printf("\tBuffer usage: \t%p-%p\t\t0x%x\n", p, p+size, size);
#endif
#ifdef MOV_FUNC
	printf("\tFunc usage: \t%p-%p\t\t0x%x\n", (void*)SRAM_FUNC_OFFSET, (void*)SRAM_FUNC_OFFSET+SRAM_FUNC_MAXLEN, SRAM_FUNC_MAXLEN);
#endif

}
