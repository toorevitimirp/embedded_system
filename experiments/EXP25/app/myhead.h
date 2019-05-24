/* ****************** Common header file    ****************** */
/* Before include this file, you may define MYDEBUG */

#ifndef _MYHEAD_H_
#define _MYHEAD_H_

/*ANSI headers */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*Linux headers */
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;
extern int errno;

/*application libraries headers*/
//#include <readline/readline.h>
//#include <readline/history.h>

//char * readline (const char *prompt);

//#define MYDEBUG

#if defined MYDEBUG 
#define PDEBUG(fmt, args...) printf(fmt, ##args)
#define	PFUNC() PDEBUG("<%s:%s> enter\n", __FILE__, __FUNCTION__);
#define PAUSE() do{fflush(stdin); printf("<%s><%d>press any key to continue\n",__FUNCTION__,  __LINE__); getchar();}while(0)
#define PLINE() printf(__FILE__":"__FUNCTION__"%d\n", __LINE__)
#else
#define PDEBUG(fmt, args...)
#define PFUNC()
#define PLINE()
#define PAUSE()
#endif


//void show_prog(int prog, int max, char *head);

/* Typedefs for integer types */
typedef unsigned char U8;	/* unsigned 8 bit data  */
typedef unsigned short U16;	/* unsigned 16 bit data */
typedef unsigned int U32;	/* unsigned 32 bit data */
typedef char S8;		/* signed 8 bit data  */
typedef short S16;		/* signed 16 bit data */
typedef long S32;		/* signed 32 bit data */

typedef U8 *P_U8;		/* unsigned 8 bit data  */
typedef U16 *P_U16;		/* unsigned 16 bit data */
typedef U32 *P_U32;		/* unsigned 32 bit data */
typedef S8 *P_S8;		/* signed 8 bit data  */
typedef S16 *P_S16;		/* signed 16 bit data */
typedef S32 *P_S32;		/* signed 32 bit data */

typedef U16 TEXT;		/* 16-bit text data */
typedef P_U16 P_TEXT;		/* 16-bit text data */

typedef U8 BOOL;		/* Boolean, TRUE/FALSE */

typedef void VOID;		/* void */
typedef void *P_VOID;		/* pointer to void */

#endif
