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
#include <sys/time.h>
#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;
extern int errno;


#define MAXLINE 255
/*application libraries headers*/
//#include <readline/readline.h>
//#include <readline/history.h>

//char * readline (const char *prompt);

//#define MYDEBUG

#define PLINE0
#define PDEBUG0
#if defined MYDEBUG
//#define PDEBUG(fmt, args...) printf(fmt, ##args)
#define PDEBUG(args...) fprintf(stderr,args)
#define	PFUNC() PDEBUG("<%s:%s:%d> enter\n", __FILE__, __FUNCTION__);
#define PAUSE() do{fflush(stdin); fprintf(stderr,"<%s><%d>press any key to continue\n",__FUNCTION__,  __LINE__); getchar();}while(0)
//#define PLINE fprintf(stderr,"<%s:%s:%d>\n", __FILE__, __FUNCTION__, __LINE__)
#define PLINE 
#else
#define PDEBUG(fmt, args...)
#define PFUNC()
#define PLINE
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

typedef signed char BOOLEAN;
typedef unsigned char UINT8;
typedef signed char SINT8;
typedef unsigned short UINT16;
typedef signed short SINT16;
typedef unsigned long UINT32;
typedef signed long SINT32;

typedef volatile BOOLEAN VBOOLEAN;
typedef volatile UINT8 VUINT8;
typedef volatile SINT8 VSINT8;
typedef volatile UINT16 VUINT16;
typedef volatile SINT16 VSINT16;
typedef volatile UINT32 VUINT32;
typedef volatile SINT32 VSINT32;

#define PRINT_VERSION fprintf(stderr,"Build time: %s, %s\n\n", __DATE__, __TIME__)

#ifndef PROFILE
#define DECLARE_TIME 
#define DEFINE_TIME 
#define START_TIME(n,s)  
#define ELAP_TIME(n)  
#define PRINT_TIME	
#else
#define MAXTM 100
#define MAINTM 10
#define DECLARE_TIME \
    extern struct timeval tm0[MAXTM], tm[MAXTM]; \
    extern long elap[MAXTM]; \
	extern char item[MAXTM][MAXTM];

#define DEFINE_TIME \
    struct timeval tm0[MAXTM], tm[MAXTM]; \
    long elap[MAXTM]; \
	char item[MAXTM][MAXTM];

#define START_TIME(n,s)  \
    gettimeofday(&tm0[n], NULL); \
	strcpy(item[n],s);	

#define ELAP_TIME(n)  \
	gettimeofday(&tm[n], NULL);    \
	elap[n]+=1000*1000*(tm[n].tv_sec-tm0[n].tv_sec)+(tm[n].tv_usec-tm0[n].tv_usec); \
	tm0[n]=tm[n]; \
	/*if(elap>0)	printf(x" has used :%ld us\n", elap);*/ 

#define PRINT_TIME	{	\
	int i;	\
	int total=0;	\
	/*while(item[i++]!=0){	*/\
	for(i=0;i<=MAINTM;i++){	\
		if(item[i][0]==0) break;	\
		total+=elap[i];	\
		fprintf(stderr,"%2d:\t%25s:\t%8.2f s\n", i,item[i], (float)elap[i]/1000000);	\
		PRINT_CH_TIME(i); \
	} \
	fprintf(stderr,"-------------------\n");	\
	fprintf(stderr,"\t%25s:\t%8.2f s.\n", "Total added:", (float)total/1000000);	\
}

//0-9 main timer use 10-99
//0 use 10-19
//1 use 20-29
#define PRINT_CH_TIME(i)	{\
		int j;	\
		for(j=(i+1)*10;j<(i+1)*10+10;j++){/*10 child timers*/	\
			if(item[j][0]==0) break;	\
			fprintf(stderr,"(%2d:\t%25s:\t\t%8.2f s)\n", j,item[j], (float)elap[j]/1000000);	\
		}	\
	}
	DECLARE_TIME;
#endif
#endif
