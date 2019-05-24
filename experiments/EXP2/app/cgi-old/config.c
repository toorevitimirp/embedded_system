#include <stdio.h>
#include <stdlib.h>
#include "myhdr.h"
#include "cgihead.h"

int main(int argc,char* argv[]){
    char *querystring;
    struct splitback string,tmp;
    int i;
    int flag = 1;
    flag = strcmp(argv[0],"/home/httpd/cgi-bin/config.cgi");
//get html var and split it    
    querystring=getenv("QUERY_STRING"); 
    string=splitstring(querystring,'&');
    for(i=0;i<(string.count);i++){
        tmp=splitstring(string.stringout[i],'=');
        snprintf(string.stringout[i],100,"%s",tmp.stringout[1]);
        }
// decide which function to use 
    if(strcmp(string.stringout[0],"8")==0)
	changerootpass();
    
    else if(strcmp(string.stringout[0],"7")==0)
	    show_changerootpass();

    else if(strcmp(string.stringout[0],"20")==0)
	 myreboot();
    else if(strcmp(string.stringout[0],"33")==0)
	intoreboot(); 
    else if(strcmp(string.stringout[0],"34")==0)
	reboot_system();
	    
    else if(strcmp(string.stringout[0],"49")==0)
	    showsetip();
    else if(strcmp(string.stringout[0],"48")==0)
             setip();
    else if(strcmp(string.stringout[0],"50")==0)
	     resetconf();
    else if(strcmp(string.stringout[0],"51")==0)
	     resetconf1();

    else if(strcmp(string.stringout[0],"52")==0)
	     settimeshow();
    else if(strcmp(string.stringout[0],"53")==0)
	     settime();
    else if(strcmp(string.stringout[0],"55")==0)
	     showbohao();
    else if(strcmp(string.stringout[0],"56")==0)
	     bohao();
    else if(strcmp(string.stringout[0],"58")==0)
	     showsetmac();
    else if(strcmp(string.stringout[0],"59")==0)
	     setmac();
    else if(strcmp(string.stringout[0],"61")==0)
	     showminigui();
    else if(strcmp(string.stringout[0],"62")==0)
	     minigui();
    else if(strcmp(string.stringout[0],"64")==0)
	     showchump();
    else if(strcmp(string.stringout[0],"65")==0)
	     chump();
    else if(strcmp(string.stringout[0],"67")==0)
	     showbjdj();
    else if(strcmp(string.stringout[0],"68")==0)
	     startbjdj();
    else if(strcmp(string.stringout[0],"69")==0)
	     stopbjdj();
    else if(strcmp(string.stringout[0],"70")==0)
	     showrecorder();
    else if(strcmp(string.stringout[0],"71")==0)
	     recorder();
    else if(strcmp(string.stringout[0],"72")==0)
	     playrecorder();
    else if(strcmp(string.stringout[0],"74")==0)
	     showplaymtv();
    else if(strcmp(string.stringout[0],"75")==0)
	     playmtv();
	     
    else if(strcmp(string.stringout[0],"88")==0)
	    dialin();
    else if(strcmp(string.stringout[0],"89")==0)
	    startdialinserver();
    else if(strcmp(string.stringout[0],"90")==0)
	    stopdialinserver();
    else if(strcmp(string.stringout[0],"150")==0)
	    about();
    
    else if(strcmp(string.stringout[0],"170")==0)
	    saveconf();
    else 
	printf("System error!");    
    
    return(0);
}
