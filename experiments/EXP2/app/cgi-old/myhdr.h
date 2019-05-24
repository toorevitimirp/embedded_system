#ifndef __MYHDR_H_
#define __MYHDR_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
struct splitback{
    int count;
    char stringout[20][60];
};
///////////////////////////////////////////////////////////////////////////
//string processing
///////////////////////////////////////////////////////////////////////////
struct splitback splitstring(char *content,char item);    
int chinesedeal(char* str);
int strfilter(char* str);
int mystrproceed(const char* src,char* dest,int startpos,int count);


///////////////////////////////////////////////////////////////////////////
//file operations
void linedeal(char *filename,char *content,int line);
void filedeal(char *filename,char *content,int mode);
int testlock(char *filename);
void unlock(char *filename);
int handleaccesserror(void *info,int rw);


int getline(char *filepath,char *needle,char* srcline);
int newgetline(char* filepath,char* needle,char*getlinestring);
/////////////////////////////////////////////////////////////////////////
//file and pipe
////////////////////////////////////////////////////////////////////////
long int getfilelength(char *filename);
int getlinenumber(const char* filename,const char* line);
int getfilelinecount(const char* filename);
int rename1(char *string1,char *string2);

FILE * popen1(char *command,char *rw);

/////////////////////////////////////////////////////////////////
//Helpers
int ischar(char *srcline);
int cutspace(char *);
void cut_return_from_fileline(char *);
int internetaccess();
int verifyip(char *i,int*);
int isnetmask(char *,char*);
int getip(char *,char *);
void clear_ipportfw_from_ip_masq();
void clear_macipmask_from_ip_masq();
///////////////////////////////////////////////////////////////////////////
//control functional
//////////////////////////////////////////////////////////////////////////
int system1(char *string);
int faintsystem2(char *string);

/////////////////////////////////////////////////////////////////////////
//Others
int timeslot(int,int,int,int,int,int);
int timeused(int,int,int,int,int);
//////////////////////////////////////////////////////////////////

void show_reboot_hint();
/////////////////////////////////////////////
//delete super
void set_all_common();
void delete_all_host_from_dhcpdconf();
//int delete_host_from_dhcpdconf(char *,char *);
/////////////////////////////////////////////
int set_super(char *,int );
int add_host_to_dhcpdconf(char *,char *);
void restart_dhcpd();
int get_dns(char *);
/////////////////////////////////////////////////////////////////
void enable_all_supers_internetaccess();
void execute_all_ipport_rules();
///////////////////////////////////////////////////////////////
int show_hint();
void print_head();
void save();

#endif

						 
