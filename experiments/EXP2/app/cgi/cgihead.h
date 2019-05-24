#ifndef __MYHDR_H_
#define __MYHDR_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "myhdr.h"

extern int accesstime();
extern int setaccesstime();

extern int showbohao();
extern int bohao();
extern int showsetmac();
extern int setmac();
extern int showminigui();
extern int minigui();
extern int showchump();
extern int chump();
extern int showbjdj();
extern int startbjdj();
extern int stopbjdj();
extern int showrecorder();
extern int recorder();
extern int playrecorder();
extern int showplaymtv();
extern int playmtv();

extern int processportcontrol();
extern int portcontrol();
extern int show_changerootpass();
extern int changerootpass();
extern int usertime();
extern int configuration();
extern int settimeshow();
extern int myreboot();
extern int beforereboot();
extern int reboot_system();
extern int myroute();
extern int modemsetup();
extern int settime();
extern int dsl();
extern int dns();
extern int saveconfigdata();
extern int myexit2(char *theip);
extern int timer();
extern int showsetip();
extern int setip();
extern int dynamicip();
extern int dnsload();
extern int showaccess();
extern int resetconf();
extern int resetconf1();
extern int showwanip();
extern int setdirectip();
extern int processsetdirectip();
extern int accesswanip();
extern int setaccesswanip();
extern int winsip();
extern int setwinsip();
extern int dialin();
extern int dialinconf();
extern int processdialinconf();
extern int startdialinserver();
extern int stopdialinserver();
extern int adddeldialinuser();
extern int pptpserverconf();
extern int processpptpserver();
extern int processpptpclient();
extern int pptpcontrol();
extern int adddelpptpserveruser();
extern int pptpserveruserman();
extern int processdynamicdhcp();
int pptpserverstart();
int pptpserverstartconf();
extern int manualipfwadm();
extern int ipport();
extern int ipportcontrol();
extern int editmail();
extern int editmailcontrol();
extern int pptpserverclose();
extern int pptpclientclose();
extern int landhcpconf();
//////////////////////////////////////////////
extern int about();
extern int backtomain();
extern int macip_list_super_manage();
extern int addsuper();
extern int landhcpconf();
extern int configuration();
extern int saveconf();

extern int configuration1();
#endif

