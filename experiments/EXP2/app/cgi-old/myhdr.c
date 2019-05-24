#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
//#include <syslog.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "myhdr.h"
//#include "../sash/sash.h"

///////////////////////////////////////////////////////////////////////////
//Control
///////////////////////////////////////////////////////////////////////////
//
int testlock(char *filename){
    int i;
    char filepath[40];
    //here filename may only be two :
    //1 /etc/config/  :12
    //2 /tmp/	      :5
    if(*(filename+11)=='/')
        snprintf(filepath,40,"/tmp/%slock",filename+12);
    else
        snprintf(filepath,40,"/tmp/%slock",filename+5);

    while((i=open(filepath,O_CREAT|O_EXCL,S_IRUSR|S_IWUSR))<0){
	sleep(1);
    }
    close(i);    
    return(1); 	
}	
void unlock(char *filename){
    int i;
    char filepath[40];
    //here filename may only be two :
    //1 /etc/config/  :12
    //2 /tmp/	      :5
    if(*(filename+11)=='/')
        snprintf(filepath,40,"/tmp/%slock",filename+12);
    else
        snprintf(filepath,40,"/tmp/%slock",filename+5);

    unlink(filepath);
}

///////////////////////////////////////////////////////////////////////////
//the function system:    
int system1(char *string){
    FILE *fp;
    int pid;
    int i;
    testlock("/tmp/tmp1");
    fp=fopen("/tmp/tmp1","w");
    handleaccesserror(fp,0);
    fprintf(fp,"%s",string);
    i=fclose(fp);
    handleaccesserror(&i,1);
    if ((pid=vfork())<0){
        ;
    }
    else if (pid==0){//child
        //execl("/bin/sh","/bin/sh","/tmp/tmp1",0);
        execl("/bin/sh","/bin/sh","/tmp/tmp1",(char*)0);
	exit(0);
    }
    else{//parent
        if(waitpid(pid,NULL,0)!=pid)
	    ;
        unlock("/tmp/tmp1");
    	unlink("/tmp/tmp1");
	return (0);    
    }
}
int faintsystem2(char *string){
    FILE *fp;
    int pid;
    int i;
    testlock("/tmp/tmp1");
    fp=fopen("/tmp/tmp1","w");
    handleaccesserror(fp,0);
    fprintf(fp,"%s",string);
    i=fclose(fp);
    handleaccesserror(&i,1);
    if ((pid=vfork())<0){
        printf("vfork error\n");
        }
    else if (pid==0){
        close(0);close(1);close(2);
        execl("/bin/sh","/bin/sh","/tmp/tmp1",0);
        _exit(0);
        }
    else{
         unlock("/tmp/tmp1");
         return(0);
       }
}
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//string processing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct splitback splitstring(char *content ,char item){ 
        struct splitback foreturn;
	int i=0;
	int j=0;
	foreturn.count=0;
	for(i=0;i<20;i++)
	    sprintf(foreturn.stringout[i],"\0");
	for(i=0;i<200;i++)
	{
                if(content[i]=='\0')
 	       {
 	       	if(j>59)
 	       		j=59;
 	       	foreturn.stringout[foreturn.count][j]=content[i];
 	       	foreturn.count++;
 	       	break;
 	       }
 	       if(content[i]!=item)
 	       {
 	       	if(j<59)
 	       	{
 	       	foreturn.stringout[foreturn.count][j]=content[i];
 	       	j++;
 	       	}
 	       }
 	       else
 	       {
 	       	foreturn.stringout[foreturn.count][j]='\0';
 	       	foreturn.count++;
 	       	if(foreturn.count>19)
 	       		break;
 	       	j=0;
 	       }
	}
	return foreturn;
}


int chinesedeal(char* str)
{
char tmp[100];
int i=0;
int j=0;
int k=strlen(str);
for(i=0;i<=k;i++)
    if(str[i]=='%')
    {
	   if(str[i+1]>='A') 
	    if(str[i+2]>='A')
	    tmp[j]=(char)((str[i+1]-'A')*16+str[i+2]-'A'+0xAA);
	    else
	    tmp[j]=(char)((str[i+1]-'A')*16+str[i+2]-'0'+0xA0);
	   else
            if(str[i+2]>='A')
	    tmp[j]=(char)((str[i+1]-'0')*16+str[i+2]-'A'+0x0A);
	    else
	    tmp[j]=(char)((str[i+1]-'0')*16+str[i+2]-'0'+0x00);
            if(tmp[j]=='='||tmp[j]=='&')
	    {
		    printf("There are forbidden chars(&,=) in your input string!<br>");
		    exit(0);
	    }
	    j++;
            i+=2;
    }
    else
    if(str[i]=='+')
    {
	    tmp[j]=' ';
            j++;
    }
    else
    { 
	    tmp[j]=str[i];
	    j++;
    }
snprintf(str,100,"%s",tmp);
return 0;
}
int strfilter(char* str)
{
int len=strlen(str);
int i;
for(i=0;i<len;i++)
	if(str[i]<'0'||(str[i]>'9'&&str[i]<'A')||(str[i]>'Z'&&str[i]<'a')||str[i]>'z')
		return 0;
return 1;
}
int mystrproceed(const char* src,char* dest,int startpos,int count)
{
	int length=strlen(src);
	int i;
	if(startpos<0||startpos>=length)
	{
		dest[0]='\0';
		return 0;
	}
	else
	for(i=startpos;i<=length;i++)
	{
		if (i-startpos>=count)
		{
			dest[count-1]='\0';
			return 0;
		}
		dest[i-startpos]=src[i];
	}
	return (length-startpos);
}





//////////////////////////////////////////////////////////////////////////
//file operations
//////////////////////////////////////////////////////////////////////////
void linedeal(char *filename,char *content,int line){
    FILE *fpsrc;
    FILE *tmpfile;
    char srcline[100];
    int length;
    int i;
    int j;
    testlock(filename);
    fpsrc=fopen(filename,"r");
    handleaccesserror(fpsrc,0);
    testlock("/tmp/tmp");
        tmpfile=fopen("/tmp/tmp","w");
        handleaccesserror(tmpfile,0);
        for(i=1;i<line;i++){
            fgets(srcline,100,fpsrc);
            if(!feof(fpsrc))
		    fprintf(tmpfile,"%s",srcline);
	    else{
		printf("no this line!<br>"); 
		unlock("/tmp/tmp");
		exit(0);
	        }
            }
	fprintf(tmpfile,"%s",content);
	if(!feof(fpsrc))
	    j=fputc('\n',tmpfile);
	fgets(srcline,100,fpsrc);
	while(!feof(fpsrc)){
	    fgets(srcline,100,fpsrc);
	    if(!feof(fpsrc))
		    fprintf(tmpfile,"%s",srcline); 
	    }
        j=fclose(tmpfile);
        handleaccesserror(&j,1);
	j=fclose(fpsrc);
	handleaccesserror(&j,1);
        rename1("/tmp/tmp",filename);
        unlock("/tmp/tmp");
    unlock(filename);
}

//oprate file,insert or delete    
void filedeal(char *filename,char *content,int mode){
    FILE *fpsrc;
    FILE *tmpfile;
    char srcline[100];
    int length;
    int boolvar;
    int i;
    //delete
    testlock(filename);
    if(mode==0){
	fpsrc=fopen(filename,"r");
        handleaccesserror(fpsrc,0);
	if(fpsrc!=NULL)
	{
	testlock("/tmp/tmpfiledeal");
        tmpfile=fopen("/tmp/tmpfiledeal","w");
        handleaccesserror(tmpfile,0);
	
	while(!feof(fpsrc)){
            fgets(srcline,100,fpsrc);
            length=strlen(srcline);
            if(srcline[length-1]=='\n'){
               srcline[length-1]='\0';
	       if(strcmp(srcline,content)!=0)
		    fprintf(tmpfile,"%s\n",srcline);
            }
            else{
	       if(strcmp(srcline,content)!=0){
	           if(!feof(fpsrc))	
		   {
			  fprintf(tmpfile,"%s",srcline);
		   }
	       }
            }	    
        }
        i=fclose(tmpfile);
        handleaccesserror(&i,1);
	i=fclose(fpsrc);
	handleaccesserror(&i,1);
        rename1("/tmp/tmpfiledeal",filename);
        unlock("/tmp/tmpfiledeal");
	}
	}
    //insert
    else{
	    fpsrc=fopen(filename,"a");
	    handleaccesserror(fpsrc,0);	    
	    fprintf(fpsrc,"%s\n",content);
	    i=fclose(fpsrc);
	    handleaccesserror(&i,1); 
    } 
    unlock(filename);	
}

int handleaccesserror(void* info,int rw)
{
    if(rw==0)//error handling when reading
	if(info==NULL)
	  {
		//printf("Encountered an error when reading file\n");
		//exit(0);
	  }
    if(rw==1)//error handling when writing
        if(*((int*)info)!=0)
       	{
		//printf("Encountered an error when writing file\n");
                //exit(0);
	}
    return 0;
}
int newgetline(char* filepath,char* needle,char* getlinestring)
{
    FILE *fp;
    int boolvar;
    testlock(filepath);

   
   fp=fopen(filepath,"r");
    handleaccesserror(fp,0);
    boolvar=0;
    while(!feof(fp)){
        fgets(getlinestring,100,fp);
	if(getlinestring[strlen(getlinestring)-1]=='\n')
	    getlinestring[strlen(getlinestring)-1]='\0';
	if(strstr(getlinestring,needle)!=NULL){
	    boolvar=1;
	    break;
	    }
        }
    fclose(fp);
    unlock(filepath);
    if(boolvar==1)
	return(1);
    else
	return(0);    
}

int getline(char *filepath,char *needle,char *srcline){
    FILE *fp;
    int boolvar;
    testlock(filepath);
    fp=fopen(filepath,"r");
    handleaccesserror(fp,0);
    boolvar=0;
    while(!feof(fp)){
        fgets(srcline,100,fp);
	if(srcline[strlen(srcline)-1]=='\n')
	    srcline[strlen(srcline)-1]='\0';
	if(strstr(srcline,needle)!=NULL){
	    boolvar=1;
	    break;
	    }
        }
    fclose(fp);
    unlock(filepath);
    if(boolvar==1)
	return(1);
    else
    {    
	    snprintf(srcline,200,"no this user!");
	    return(0);    
    }
    
}
////////////////////////////////////////////////////////////////////////////
long int getfilelength(char *filename){
    FILE *fp;
    long int i;
    int j;
    testlock(filename);
    fp=fopen(filename,"r");
    handleaccesserror(fp,0);
    fseek(fp,0L,2);
    i=ftell(fp);
    j=fclose(fp);
    handleaccesserror(&j,1);
    unlock(filename);
    return(i);
}

int getfilelinecount(const char* filename)
{
	char tmp[200];
	int linecount=0;
	FILE* fp;
	testlock(filename);
	fp=fopen(filename,"r");
	handleaccesserror(fp,0);
	while(!feof(fp))
	{
		fgets(tmp,200,fp);
		linecount++;
	}
	fclose(fp);
	unlock(filename);
	return linecount;
}
int getlinenumber(const char* filename,const char* line)
///this function aims to get the count of the line which 
///includes the string 'line' in file 'filename'
{
	int retnumber=0;
	int i=0;
	FILE* fp;
	char linestringgot[100];
	char tmp[100];
	testlock(filename);
	fp=fopen(filename,"r");
	handleaccesserror(fp,0);
	while(!feof(fp)){
		fgets(linestringgot,100,fp);
		i++;
		if(linestringgot[strlen(linestringgot)-1]=='\n')
			linestringgot[strlen(linestringgot)-1]='\0';
		if(strstr(linestringgot,line))
			retnumber++;
	}
	unlock(filename);
	return retnumber;
}



//the function rename
int rename1(char *string1,char *string2){
    int pid;
    
    if ((pid=vfork())<0){
        printf("vfork error\n");
        }
    else if (pid==0){  
       execl("/bin/move","/bin/move",string1,string2,0);
       exit(0);
       }
    else{
        if(waitpid(pid,NULL,0)!=pid)
	    printf("wait pid error!<br>");	
        return(0);
        }
}
///////////////////////////////////////////////////////////////////////////
FILE * popen1(command, rw)
char * command;
char * rw;
{
   int pipe_fd[2];
   int pid, reading;

   if( pipe(pipe_fd) < 0 ) return NULL;
   reading = (rw[0] == 'r');

   pid = vfork();
   if( pid < 0 ) { close(pipe_fd[0]); close(pipe_fd[1]); return NULL; }
   if( pid == 0 )
   {
      close(pipe_fd[!reading]);
      close(reading);
      if( pipe_fd[reading] != reading )
      {
	 dup2(pipe_fd[reading], reading);
         close(pipe_fd[reading]);
      }
      close(1);

      execl("/bin/ifconfig","ifconfig",(char*)0);
      _exit(255);
   }

   close(pipe_fd[reading]);
   return fdopen(pipe_fd[!reading], rw);
}
///////////////////////////////////////////////////////////////////////////
//Helpers
int ischar(char *srcline){
    int i;	
    int boolvar;
    boolvar=0;
    for(i=0;i<strlen(srcline);i++){
        if(srcline[i]>'9'||srcline[i]<'0'){
	    boolvar=1;
	    break;
	    }
        }
    return(boolvar);
    }


int cutspace(char *pstr)
{
    int i;
    char *tmp=NULL;
    tmp=strstr(pstr,"+");
    if(tmp)
    {
        *tmp=0;
    }
    return 0;
}


void cut_return_from_fileline(char *line)
{
    int i;
    for(i=0;i<strlen(line);i++){
	if(line[i]==';' || line[i]=='\n'){
		line[i]=0;
		return;
	}
    }
}

int verifyip(char* ip,int* ret)
{
char verifystring[20];
char tmpstring[4];
int i,j;
int tmpcount=0;
int k=0;
int intcount=0;
if(strlen(ip)<7)
return 0;
snprintf(verifystring,20,"%s",ip);
    i=0;
    j=0;//count the dots in verifystring
    if(strcmp(verifystring,"\0")!=0)
     {
      while(verifystring[i]!='\0')
       {
       if(verifystring[i]=='.')
       {
       if(tmpcount==0)
       {
       return (0);
       }
       tmpstring[tmpcount]='\0';
       k=atoi(tmpstring);
       if(k>255||k<0)
       {
       return (0);
       }
       intcount++;
       tmpcount=0;
       ret[j]=k;
       j++;
       i++;
       }
       else 
       if(verifystring[i]>='0'&&verifystring[i]<='9')
       {
       if(tmpcount>2)
       {
	return (0);
       }
       else
       tmpstring[tmpcount]=verifystring[i];
       i++;tmpcount++;
       if(verifystring[i]=='\0')
           {
	   tmpstring[tmpcount]='\0';
           k=atoi(tmpstring);
            if(k>255||k<0)
             {
              return(0);
              }
	    ret[intcount]=k;
	    intcount++;   
           }
       }
       else
       {
       return(0);
       }
       }
       if(j!=3||intcount!=4)
       {
       return(0);
       }

     }
     else return 0;
return 1;
}
int isnetmask(char *netmask,char *ip)
{
struct in_addr inp;
int i=0;
unsigned long int xxx;
int netnumber;
int flag=0;
int iptype;
int ad[4];
if(verifyip(ip,ad)==0)
{
#if defined (HHTECH_CGI_CHINESE)
	printf("<center>IP地址错误: %s</center><br>",ip);
#else
printf("<center>ip invalid: %s</center><br>",ip);
#endif
		return 0;
}
iptype=ad[0];

if(iptype<128)
{
	netnumber=24;        //ip address of type A
        goto check;
}
if(iptype<192)
{
	netnumber=16;       //ip address of type B
	goto check;
}
if(iptype<224)
{
     netnumber=8;        //ip address of type C
}
else
{
#if defined (HHTECH_CGI_CHINESE)
	printf("<center>D,E 输入IP地址</center><br>");
#else
printf("<center>D,E type address</center><br>");
#endif
        return 0;
}
check:
if(verifyip(netmask,ad)==0)
{    
#if defined (HHTECH_CGI_CHINESE)
	printf("<center>网络掩码错误</center><br>");
#else
printf("<center>netmask invalid</center><br>");
#endif
	return 0;
}
xxx=(ad[0]<<24)|(ad[1]<<16)|(ad[2]<<8)|ad[3];
for(i=0;i<32;i++)
{ 
	
	if(flag==0&&i>netnumber+1)
	{
		return 0;
	}
	if((xxx|0x0001)==xxx)
		flag=1;
	else 
		if(flag)
			return 0;
	xxx>>=1;
}
return 1;
}

//input name string of interface,e.g."eth0",and return the ip of eth0
//if no such interface,return -1
int getip(char *interface,char *ip)
{
    FILE *fp;
    int fd,n,i;
    char string[200];
    char *strtmp;
    
    fp = popen1("ifconfig","r");
    pclose(fp);
    fd=open("/tmp/ipaddr1",O_RDWR);
    if (fd < 0 ) 
	    return -2;
    n = read(fd,string,200);
    if (n < 0 ) 
	    return -2;
    close(fd);

    if(!(strtmp=strstr(string,interface)))
    {
     return -1;
    }
    ////////////////////////////////////////////////////////////////
    strtmp += 17;//ppp0    inet addr: 61.180.168.88
    while(!(*strtmp<='9' && *strtmp>='1')){
	if(*strtmp=='<')
	    return -4;//not connected!!
	strtmp++; 
    }
    i=0;

    bzero(ip,16);
    while(*strtmp != '<')
    {
        ip[i] = *strtmp;
	i++;
	strtmp++;
    }
    return 1;
}
////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
void show_reboot_hint(){
    //printf("Content-type:text/html\n\n");
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<html>");
    printf("<body bgcolor=#F1F1E4 text=black>");
    printf("<br><br><br><center>");
    printf("<h1>设置成功!</h1><br>");
    printf("<font size=4>您的设置将在系统重新启动后生效,需要现在重新启动吗?</font>");
    printf("
	    <form action=/cgi-bin/config.cgi method=get target=main>
	    <input type=hidden name=flag value=34>
	    <input type=submit value=\"  是  \">
	    <input type=\"button\" value=\"  否  \" onClick=\"location='/login.html'\">
	    </form>
	    ");

    printf("</center>
	    </body>
	    </html>
	");

}
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int backtomain(){
    printf("Content-type:text/html\n\n");

    printf("
    <html>
    <head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
    <title>GW150 管理</title>
    </head>
    <body>
    <center>
    <img src=\"/system.jpg\" width=\"588\" height=\"402\" board=\"0\"><br>
    </center>
    </body>
    </noframes>
    </html>  
    ");
}

void print_head(){
    printf("Content-type:text/html\n\n"); 
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");
}

void save()
{
    system1("/bin/killall -10 flatfsd");
}
