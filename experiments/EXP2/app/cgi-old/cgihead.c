#include <sys/mount.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <date.h>
#include "myhdr.h"
#include "cgihead.h"
#include <resolv.h>
#include <netinet/in.h>
//#include <syslog.h>
#include <signal.h>

#define USER_MAX 256
#define IP_COUNT_MAX 100
#define PORT_COUNT_MAX 100
#define ONLINE_MAX 100
#define MAX_PORTFW 256
//###########################################################################wpq
//pid_t test_pid;

struct settime{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

int writeret=0;
//added by HN on 2002.01.04,
//current internet access 
static int curnetaccess=0;
//////////////////////////////////////////////////////////////////////////////////###########################################################################wpq
int show_changerootpass(){
    printf("Content-type:text/html\n\n"); 
#if defined (HHTECH_CGI_CHINESE)		
printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title></title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form action=/cgi-bin/config.cgi method=get>
<br><br><br>
<div align=center>
<table cellspacing=3 cellpadding=3 with=50% align=center border=0>
<tr><td><input type=hidden name=flag value=8>旧密码:</td>
    <td><input type=password size=10 name=oldpass></td></tr>
<tr><td>新密码:</td>
    <td><input type=password size=10 name=nopass></td></tr>
<tr><td>确认密码:</td>
    <td><input type=password size=10 name=repass></td></tr>
<tr><td colspan=2 align=center><input type=submit value=更新></td></tr>
</table>
</form>
</body>
</html>
");
#else
printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title></title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form action=/cgi-bin/config.cgi method=get>
<br><br><br>
<div align=center>
<table cellspacing=3 cellpadding=3 with=50% align=center border=0>
<tr><td><input type=hidden name=flag value=8>Old password:</td>
    <td><input type=password size=10 name=oldpass></td></tr>
<tr><td>New password:</td>
    <td><input type=password size=10 name=nopass></td></tr>
<tr><td>Confirm:</td>
    <td><input type=password size=10 name=repass></td></tr>
<tr><td colspan=2 align=center><input type=submit value=OK style=\"HEIGHT:240px;WIDTH:65px\"></td></tr>
</table>
</form>
</body>
</html>
");

#endif
    return 0;
}

int changerootpass(){
    char *querystring;
    char string[100];
    char pass[100];
    char string1[10];
    char string2[10];
    char string3[10];
    FILE *fp;
    int i,j;
    char* pwd;
    struct splitback splitedstring,tmp;
    querystring=getenv("QUERY_STRING");
    fp=fopen("/etc/config/passwd","r");
    handleaccesserror(fp,0);
    fgets(pass,100,fp);
    fclose(fp);
     for(i=0;i<100;i++){
	if(pass[i]=='\n'){
	    pass[i]='\0';
	    break;
	    }
        }
    pwd = strchr(pass,' ');
    if(pwd)
    {
        *pwd++=0;
	
    }
    //chinesedeal(querystring);
    printf("Content-type:text/html\n\n"); 
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");
    snprintf(string,100,"%s",querystring);
    splitedstring=splitstring(string,'&');
    for(i=0;i<splitedstring.count;i++)
    {
	    tmp=splitstring(splitedstring.stringout[i],'=');
	    snprintf(splitedstring.stringout[i],100,"%s",tmp.stringout[1]);
	    chinesedeal(splitedstring.stringout[i]);
            if(strcmp(splitedstring.stringout[i],"\0")==0)
	    {
#if defined (HHTECH_CGI_CHINESE)		
		    printf("<center>不允许输入空字符串,请重试!</center><br>");
#else
printf("<center>NULL string detected,please retry!</center><br>");
#endif
	            exit(0);
	    }
	    if(strlen(splitedstring.stringout[i])>10)
	    {
#if defined (HHTECH_CGI_CHINESE)		   
		    printf("<center>输入字符串过长!</center><br>");
#else
		    printf("<center>String too long!</center><br>");
#endif
		    exit(0);
	    }
    }
    if(strcmp(crypt(splitedstring.stringout[1],pwd),pwd)==0)
    {//correct old password
	if(strcmp(splitedstring.stringout[2],splitedstring.stringout[3])==0)
	{//two input don't match
	    testlock("/etc/config/passwd");
	    fp=fopen("/etc/config/passwd","w");
	    handleaccesserror(fp,0);
	    
	    fprintf(fp,"passwd %s\n",crypt(splitedstring.stringout[2],"hh"));
	    writeret=fclose(fp);
            handleaccesserror(&writeret,1);
	    unlock("/etc/config/passwd");
#if defined (HHTECH_CGI_CHINESE)	   
	    printf("<center>修改密码成功!</center>\n");
#else
printf("<center>Password changed successfully!</center>\n");
#endif
	}
	else
#if defined (HHTECH_CGI_CHINESE)	   
	       	printf("<center>新密码不匹配!</center>\n");
#else
printf("<center>New password dose not meet!</center>\n");
#endif
    }
    else{
#if defined (HHTECH_CGI_CHINESE)
    	    printf("<center>密码错误,请重试!</center>");
#else
	printf("<center>Incorrect password,please retry!</center>");
#endif
    }
    
    return(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int resetconf()
{
    printf("Content-type:text/html\n\n");
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
#if defined (HHTECH_CGI_CHINESE)
    printf("
            <body bgcolor=#F1F1E4 text=black>
	    <form method=get action=/cgi-bin/config.cgi>
            <br><br><br><br>
	    <div align=center>
	    <center>
	    <input type=hidden name=flag value=51><br>
	    所有配置将会被恢复为出厂状态!
            是否继续?<br>
	    <input type=radio name=save value=yes checked>是<br>
	    <input type=radio name=save value=no>否<br><br>
	    <input type=submit value=确认>
	    </center>
	    </form>
	    </body>
	    ");
#else
    printf("
            <body bgcolor=#F1F1E4 text=black>
	    <form method=get action=/cgi-bin/config.cgi>
            <br><br><br><br>
	    <div align=center>
	    <center>
	    <input type=hidden name=flag value=51><br>
	    All the configurations will be reset!<br>
            Continue?<br>
	    <input type=radio name=save value=yes checked>Yes<br>
	    <input type=radio name=save value=no>No<br><br>
	    <input type=submit value=confirm>
	    </center>
	    </form>
	    </body>
	    ");
#endif
     return 0;
}
int resetconf1()
{
     system1("cp /etc/default/* /etc/config/");
     system1("cp /etc/default/boa.conf /home/httpd");
     print_head();
     show_reboot_hint();
     return 0;
}
int saveconf()
{
    save();
    printf("Content-type:text/html\n\n");
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
#if defined (HHTECH_CGI_CHINESE)
    printf("
            <body bgcolor=#F1F1E4 text=black>
            <br><br><br><br>
	    <center>
	    配置文件已经成功保存!!
	    </center>
	    </body>
	    ");
#endif
    return 0;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////   
//reboot
int intoreboot(){
    printf("Content-typan=2 bgcoloe:text/html\n\n");	
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
#if defined (HHTECH_CGI_CHINESE)
    printf("
		<body bgcolor=#F1F1E4 text=black>
		<form method=get action=/cgi-bin/config.cgi>
		<br><br><br><br>
		<div align=center>
		<center>
		<input type=hidden name=flag value=20><br>
		是否要重启系统平台?<br>
		<br>
		<input type=radio name=save value=yes checked>是<br>
		<input type=radio name=save value=no>否<br><br>
		<input type=submit value=确认>
		</center>
		</form>
		</body>	
		");
#else
    printf("
		<body bgcolor=#F1F1E4 text=black>
		<form method=get action=/cgi-bin/config.cgi>
		<br><br><br><br>
		<div align=center>
		<center>
		<input type=hidden name=flag value=20><br>
		You are rebooting the router.<br>
		Save all configurations before reboot?<br>
		<input type=radio name=save value=yes checked>Yes<br>
		<input type=radio name=save value=no>No<br><br>
		<input type=submit value=confirm>
		</center>
		</form>
		</body>	
		");
#endif
    return(0);
}

int myreboot()
{
    char *querystring;	
    struct splitback splitedstring,tmp;
    int i;
    querystring=getenv("QUERY_STRING");
    splitedstring=splitstring(querystring,'&');
    for(i=0;i<splitedstring.count;i++);
    {
	     tmp=splitstring(splitedstring.stringout[i],'=');
	     snprintf(splitedstring.stringout[i],100,"%s",tmp.stringout[1]);
    }
    if(strstr(splitedstring.stringout[1],"yes")!=NULL){
	reboot_system();
	return(0);
    }
}

int reboot_system(){
    printf("Content-type:text/html\n\n");	
printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<head><meta http-equiv=refresh></head>");
    printf("<br><br><br><br><br><br><center>");
#if defined (HHTECH_CGI_CHINESE)
    printf("系统正在重新启动,请在几秒钟之后重新登录...<br><br><br>");
    printf("
	    <form action=/cgi-bin/config.cgi method=get target=main>
	    <input type=\"button\" value=登录 onClick=\"location='/login.html'\">
	    </form>
	    ");

    if(vfork() == 0){
    	close(0);
	close(1);
	execl("/sbin/reboot","/sbin/reboot",NULL);
	//printf("Exec PPTPD erro<br>");
	return 0;
	}

#else
    printf("The gateway is rebooting......<br>");
    printf("You can re-connect to the router after serveral seconds...<br>");
    printf("
	    <form action=/cgi-bin/config.cgi method=get target=main>
	    <input type=\"button\" value=Login onClick=\"location='/login.html'\">
	    </form>
	    ");

#endif

    printf("</center>");
    faintsystem2("reboot");
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////#########################################################################wpq
int dialin()
{
 char content[2000];
 char srcline[100];
 FILE* fp;
 
printf("Content-type:text/html\n\n");
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");
    
#if defined (HHTECH_CGI_CHINESE)
    printf("
		<html>
		<head><title>TELNET服务后台进程</title></head>
		<body bgcolor=#F1F1E4 text=black>
    		<form action=/cgi-bin/config.cgi method=get>
    		<br>
    		<div align=center>
    		<center>
       <tr><td><colspan=2><a href=/cgi-bin/config.cgi?flag=89 target=main>启动服务</a></td></tr><br><br>
       <tr><td><colspan=2><a href=/cgi-bin/config.cgi?flag=90 target=main>停止服务</a></td></tr><br><br>
                </center>
    		</form>
		</body>
		</html>
		");
#else
    printf("
		<html>
		<head><title>dialin</title></head>
		<body bgcolor=#F1F1E4 text=black>
    		<form action=/cgi-bin/config.cgi method=get>
    		<br>
    		<div align=center>
    		<center>
       <tr><td><colspan=2><a href=/cgi-bin/config.cgi?flag=95 target=main>Dialin Configuration</a></td></tr><br><br>
       	<tr><td><colspan=2><a href=/cgi-bin/config.cgi?flag=91 target=main>Dialin User Management</a></td></tr><br><br>
       <tr><td><colspan=2><a href=/cgi-bin/config.cgi?flag=89 target=main>Start Dialin Service</a></td></tr><br><br>
       <tr><td><colspan=2><a href=/cgi-bin/config.cgi?flag=90 target=main>Stop Dialin Service</a></td></tr><br><br>
                </center>
    		</form>
		</body>
		</html>
		");

#endif
}
int startdialinserver()
{
    int i;
    char stringtowrite[100];
    printf("Content-type:text/html\n\n"); 
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");
    //snprintf(stringtowrite,100,"/sbin/gettyd /dev/cua1");
    //system1(stringtowrite);
    if((i=vfork()) == 0){
    	close(0);
	close(1);
	execl("/bin/inetd","/bin/inetd",NULL);
	//printf("Exec PPTPD erro<br>");
	return 0;
	}
#if defined (HHTECH_CGI_CHINESE)
    printf("<center>成功启动TELNET服务!</center>");
#else
    printf("TELNET Service Started!");
#endif
   return 1;

} 
int stopdialinserver()
{
    int i;
    char stringtowrite[100];
    printf("Content-type:text/html\n\n"); 
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");
    snprintf(stringtowrite,100,"/bin/killall inetd");
    system1(stringtowrite);
#if defined (HHTECH_CGI_CHINESE)
    printf("<center>成功停止TELNET服务!</center>");
#else
    printf("Succeeded in Stopping TELNET Service!");
#endif
   return 1;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//########################################################################wpq
int showsetip(){
    FILE *fp;
    char srcline[100];
    struct splitback sp;
    int i;
    char ip[16];
    bzero(ip,16);
    getip("eth0",ip);
    printf("Content-type:text/html\n\n"); 

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>固定IP设置</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=48 checked>
IP:&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<input type=text size=20 name=ip value=%s>
<br>
子网掩码:&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<input type=text size=20 name=netmask value=255.255.255.0>
<br>
系统IP:&nbsp(可不填)&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<input type=text size=20 name=gateway>&nbsp
<br>
域名服务器IP:(可不填)&nbsp<input type=text size=20 name=dns>
<br>
<br>
<input type=submit value=确认><br><br><br>
</center>
</form>
</body>
</html>
",ip);
    return 0;
}

int setip(){
    char* querystring;
    char stringtowrite[100];
    struct splitback splitedstring,string;
    int i;
    int ad[4];
    int j;
    char bnogw=0;
    char bnodns=0;
    FILE *fp;
    
    querystring=getenv("QUERY_STRING");
    splitedstring=splitstring(querystring,'&');

    print_head();
    
    for(i=1;i<splitedstring.count;i++){
	string=splitstring(splitedstring.stringout[i],'=');
	snprintf(splitedstring.stringout[i],100,"%s",string.stringout[1]);
        if((j=verifyip(splitedstring.stringout[i],ad))==0)
	{    
#if defined (HHTECH_CGI_CHINESE)
	    if(i==3 && (strlen(splitedstring.stringout[3]) <= 1))
	    	bnogw=1;
	    else if(i==4 && (strlen(splitedstring.stringout[4]) <= 1))
		bnodns=1;
	    else{
		printf("<center>IP地址格式错误! 请重试!</center>");
	        return 0;
	    }
#endif
	} 
    }
    if(isnetmask(splitedstring.stringout[2],splitedstring.stringout[1])==0){
#if defined (HHTECH_CGI_CHINESE)
	printf("<center>子网掩码格式错误!请重试!</center>");
#else
	printf("<center>Invalid netmask format!Please retry.</center>");
#endif
	return 0;
    }

    snprintf(stringtowrite,100,"/sbin/ifconfig eth0 %s netmask %s",splitedstring.stringout[1],splitedstring.stringout[2]);
    /*if(vfork() == 0){
    	close(0);
	close(1);
	execl("/bin/ifconfig","/bin/ifconfig","eth0",splitedstring.stringout[1],"netmask",splitedstring.stringout[2],NULL);
	//printf("Exec PPTPD erro<br>");
	return 0;
    }*/

    j=0;
    for(i = 0;i < strlen(splitedstring.stringout[1]);i++){
	if(splitedstring.stringout[1][i] == '.')
		j++;
		
	if(j == 3){
		splitedstring.stringout[1][i+1] = '0';
		splitedstring.stringout[1][i+2] = '\0';
		break;
	}
    }

    fp=fopen("/jffs2/etc/ipset","w");
    fprintf(fp,"%s\n","#!/bin/sh");
    fclose(fp);

    fp=fopen("/jffs2/etc/ipset","a");
    fprintf(fp,"\n%s\n",stringtowrite);
    fclose(fp);

    if(!bnogw)//No gateway set
    {
	snprintf(stringtowrite,100,"route add default gw %s",splitedstring.stringout[3]);
	//linedeal("/etc/config/start",stringtowrite,3);
	//system1("route del default");
	//system1(stringtowrite);
	fp=fopen("/jffs2/ipset","a");
	fprintf(fp,"%s\n",stringtowrite);
	fclose(fp);
    }
//dns
    if(!bnodns)
    {
	snprintf(stringtowrite,100,"nameserver %s",splitedstring.stringout[4]);
	//linedeal("/etc/config/resolv.conf",stringtowrite,1);
	fp=fopen("/etc/config/resolv.conf","w");
	fprintf(fp,"%s\n",stringtowrite);
	fclose(fp);
    }
    save();
     show_reboot_hint();

/*
#if defined (HHTECH_CGI_CHINESE)
    printf("<center>IP修改成功!</center>");
#else
    printf("<center>Succeed!</center><br>");
#endif
*/
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////###########################################################################wpq
int about(){
    printf("Content-type:text/html\n\n");

    printf("
    <html>
    <head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
    <title>GW150 管理</title>
    </head>
    <body bgcolor=#F1F1E4>
    <br><br><br><br><br>
    <center>
    HHTECH<br>
    Firmware 版本 1.0<br><br>
    版权所有,2004-2005,华恒科技保留所有权利<br><br>
    关于本产品的最新更新请登录华恒网站查询<br><br>
    <a href=\"http://www.hhcn.com\">http://www.hhcn.com</a> 
    </center>
    <body>
    </body>
    </noframes>
    </html>  
    ");

}
///////////////////////////////////////////////////////////
int landhcpconf()
{

    char line[100];
    FILE* fp;
    int i;
    char iplow[16];
    char iphigh[16];
    char gw[16];
    struct splitback splitedstring,tmp;
    bzero(iplow,16);
    bzero(iphigh,16);
    bzero(gw,16);

    printf("Content-type:text/html\n\n");
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");

    fp=fopen("/etc/config/dhcpd.conf","r");
    bzero(line,100);
    
    fgets(line,100,fp);
    fgets(line,100,fp);
    tmp=splitstring(line,' ');
    strncpy(iplow,tmp.stringout[1],16);
    cut_return_from_fileline(tmp.stringout[2]);
    strncpy(iphigh,tmp.stringout[2],16);

    for(i=0;i<5;i++)
        fgets(line,100,fp);
    tmp=splitstring(line,' ');
    cut_return_from_fileline(tmp.stringout[2]);
    strcpy(gw,tmp.stringout[2]);


    fclose(fp);
    //printf("localip=%s<br>peerip=%s<br>",localip,peerip);
#if defined (HHTECH_CGI_CHINESE)
    printf("
	<html>
	<head>
	<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
	<title>DHCP</title>
	</head>
	<body bgcolor=#EEF7FF text=black>
	<form action=/cgi-bin/config.cgi method=get>
	<br><br><br>
	<div align=center>
	<table cellspacing=5 cellpadding=0 width=80%% border=0 align=center>
	<tr><td width=50%%><input type=hidden name=flag value=111 checked>分配IP范围:</td>
	<td width=50%%><input type=text size=14 name=ipdown value=%s>--></td><td width=50%><input type=text size=14 name=ipup value=%s></td></tr>
	<tr><td>平台:</td>
	<td><input type=text size=14 name=routers value=%s></td></tr>
	<tr><td colspan=2 align=middle><input type=submit value=确定></td></tr>
	</table>
	</form>
	</body>
	</html>",iplow,iphigh,gw
	);
#else
    printf("
	<html>
	<head>
	<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
	<title>DHCP</title>
	</head>
	<body bgcolor=#EEF7FF text=black>
	<form action=/cgi-bin/config.cgi method=get>
	<br><br><br>
	<div align=center>
	<table cellspacing=5 cellpadding=0 width=70%% border=0 align=center>
	<tr><td width=50%%><input type=hidden name=flag value=111 checked>IP range:</td>
	<td width=50%%><input type=text size=14 name=ipdown value=%s>--></td><td width=50%><input type=text size=14 name=ipup value=%s></td></tr>
	<tr><td>gateway:</td>
	<td><input type=text size=14 name=routers value=%s></td></tr>
	<tr><td colspan=2 align=middle><input type=submit value=OK></td></tr>
	</table>
	</form>
	</body>
	</html>",iplow,iphigh,gw
	);
#endif
     return 0;
}

int configuration1()
{
    printf("Content-type:text/html\n\n");

    printf("
    <html>
    <head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
    <title>GW150 管理</title>
    </head>
    <body bgcolor=#F1F1E4>
    <br><br><br><br><br>
    <center>
    HHTECH<br>
    Firmware 版本 1.0<br><br>
    版权所有,2004-2005,华恒科技保留所有权利<br><br>
    关于本产品的最新更新请登录华恒网站查询<br><br>
    <a href=\"http://www.hhcn.com\">http://www.hhcn.com</a> 
    </center>
    <body>
    </body>
    </noframes>
    </html>  
    ");
    }
int
configuration()
{
    FILE *fp;
    char line[100];
    char autoorset[20];

    char* string;
    char *querystring;
    char** tail;
    struct splitback splitedstring,tmp;
    char str0[6],str1[6],str2[6];
    //char defaulttime[6],maxtime[10];
    char stringtowrite[100];

    char ad[4];
    int i,j;
    unsigned long ipdown,ipup;
    FILE *in;
    struct in_addr inp;  
    char eth1ip[16];
    struct splitback eth1tmp;
    struct splitback tmp1,tmp2,tmp0;
	
    querystring=getenv("QUERY_STRING");
    snprintf(string,200,"%s",querystring);
    splitedstring=splitstring(string,'&');
    for(i=0;i<(splitedstring.count);i++){
        tmp=splitstring(splitedstring.stringout[i],'=');
        snprintf(splitedstring.stringout[i],100,"%s",tmp.stringout[1]);
    }
////////////////////////////////////////////////////////////////////////   
    printf("Content-type:text/html\n\n"); 
    printf("<head><meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now></head>");
    printf("<body bgcolor=#F1F1E4 text=black>");
//////i///////////////////////////////////////////////////////////////// 
    printf("<center><br><br>");
    if((j=verifyip(splitedstring.stringout[1],ad))==0)//invalid
    {
#if defined (HHTECH_CGI_CHINESE)
        printf("<center>输入IP范围的下限地址非法,请重试!</center>");
#else
	printf("Invalid IP input,please retry!");
#endif
        return 1;
    }
    if((j=verifyip(splitedstring.stringout[2],ad))==0)//invalid
    {
#if defined (HHTECH_CGI_CHINESE)
        printf("<center>输入IP范围的上限地址非法,请重试!</center>");
#else
	printf("Invalid IP input,please retry!");
#endif
        return 1;
    }
    if((j=verifyip(splitedstring.stringout[3],ad))==0)//invalid
    {
#if defined (HHTECH_CGI_CHINESE)
        printf("<center>系统地址非法,请重试!</center>");
#else
	printf("Invalid gateway IP,please retry!");
#endif
        return 1;
    }
/*
    if((j=verifyip(splitedstring.stringout[4],ad))==0)//invalid
    {
#if defined (HHTECH_CGI_CHINESE)
    printf("<center>DNS地址非法,请重试!</center>");
#else
    printf("Invalid DNS IP,please retry!");
#endif
    return 1;
    }
*/
 //////////////////////////////////////////////////////////////
 ///////
    tmp  = splitstring(splitedstring.stringout[1],'.');
    strcpy(str0,tmp.stringout[0]);
    strcpy(str1,tmp.stringout[1]);
    strcpy(str2,tmp.stringout[2]);
    ipdown = strtol(tmp.stringout[3],tail,10);
    tmp = splitstring(splitedstring.stringout[2],'.');
    ipup   = strtol(tmp.stringout[3],tail,10);
////////////////////////////////////
//check if the three input conflict
    tmp0 = splitstring(splitedstring.stringout[1],'.');
    tmp1 = splitstring(splitedstring.stringout[2],'.');
    tmp2 = splitstring(splitedstring.stringout[3],'.');
    if( strcmp(tmp0.stringout[0],tmp1.stringout[0]) || strcmp(tmp0.stringout[1],tmp1.stringout[1]) || strcmp(tmp0.stringout[2],tmp1.stringout[2]) || strcmp(tmp0.stringout[0],tmp2.stringout[0]) || strcmp(tmp0.stringout[1],tmp2.stringout[1]) || strcmp(tmp0.stringout[2],tmp2.stringout[2]) ) 
    {
 #if defined (HHTECH_CGI_CHINESE)
	printf("输入不匹配!<br><br>");
	printf("修改失败!<br>");
	
#else
	printf("INPUT ERROR!<br><br>");
	printf("MODIFY FAIL!");
#endif
	return 0;
   
    }
//////////
    ////////////////////////////////////////////////////////////////////////////
    //HN 2002/5/26
    unlink("/etc/config/dhcpd.log");
   /* clear_macipmask_from_ip_masq();*/

    ///////////////////////////////////////////////////////////////////////////
    //system1("cp /etc/default/dhcpd.conf /etc/config");
    ///////////////////////////////////////////////////////////////////////////
    testlock("/etc/config/dhcpd.conf");
    fp=fopen("/etc/config/dhcpd.conf","r");
    for(i=0;i<8;i++){
	fgets(line,100,fp);//to get dns
    }
    fgets(autoorset,20,fp);
    fgets(autoorset,20,fp);

    fclose(fp);
    
    fp=fopen("/etc/config/dhcpd.conf","w");

    snprintf(stringtowrite,100,"subnet %s.%s.%s.0 netmask 255.255.255.0 {",str0,str1,str2);
    fprintf(fp,"%s\n",stringtowrite);

   snprintf(stringtowrite,100,"range %s %s;",splitedstring.stringout[1],splitedstring.stringout[2]);
    fprintf(fp,"%s\n",stringtowrite);
   
   snprintf(stringtowrite,100,"default-lease-time 604800;");//604800s = a week
    fprintf(fp,"%s\n",stringtowrite);
   
   snprintf(stringtowrite,100,"max-lease-time 2592000;");    //2592000s = 30days
    fprintf(fp,"%s\n",stringtowrite);
   
   snprintf(stringtowrite,100,"option subnet-mask 255.255.0.0;");
    fprintf(fp,"%s\n",stringtowrite);
   
 snprintf(stringtowrite,100,"option broadcast-address %s.%s.%s.255;",str0,str1,str2);
    fprintf(fp,"%s\n",stringtowrite);
 
   snprintf(stringtowrite,100,"option routers %s;",splitedstring.stringout[3]);
    fprintf(fp,"%s\n",stringtowrite);


    fprintf(fp,"%s\n",line);//dns
    fprintf(fp,"%s",autoorset);//autodns or setdns
    fprintf(fp,"}\n");

    fclose(fp);

   unlock("/etc/config/dhcpd.conf");
    snprintf(stringtowrite,100,"ifconfig eth0 %s netmask 255.255.255.0 broadcast %s.%s.%s.255",splitedstring.stringout[3],str0,str1,str2);
   linedeal("/etc/config/start",stringtowrite,1);
    snprintf(stringtowrite,100,"route add -net %s.%s.%s.0 eth0",str0,str1,str2);

   linedeal("/etc/config/start",stringtowrite,2);
    show_reboot_hint();
}
//###########################################################################wpq
int settimeshow() {
	time_t tm;
	struct tm tm_time;
	time(&tm);
	memcpy(&tm_time,localtime(&tm), sizeof(tm_time));

if(vfork() == 0){
                execl("/bin/date","/bin/date",NULL);

                return 0;
        }


printf("Content-type:text/html\n\n");

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>系统时间</title>
</head>
<body bgcolor=#F1F1E4 text=black>


<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=53 checked>
<br>
设置系统时间
<br>
<br>
%s
<!--wpq add  it-->
<!--//%s-->
<br>
<br>
<input type=text size=4 name=year>年/<input type=text size=2 name=month>月/<input type=text size=2 name=day>日&nbsp&nbsp&nbsp<input type=text size=2 name=hour>&nbsp:&nbsp<input type=text size=2 name=minute>
<br>
<br>
<input type=submit value=确定>
<br>
<br>
<br>
当前系统时间
<br>
<br>
</center>
<br>
</form>
</body>
</html>
<!--The time is curent times,//###################################wpq-->
"//,
//asctime(&tm_time)
//######################################################################wpq
,localtime(&tm_time)
);
	return 0;
}


int settime() {
	char* querystring;
	struct splitback splitedstring,string;
	int i;
   	char stringtowrite[13];

	querystring=getenv("QUERY_STRING");
	splitedstring=splitstring(querystring,'&');

	for(i=1;i<splitedstring.count;i++){
		string=splitstring(splitedstring.stringout[i],'=');
		if(i==1)
			snprintf(splitedstring.stringout[i],5,"%s",string.stringout[1]);
		else
			snprintf(splitedstring.stringout[i],3,"%s",string.stringout[1]);
	}
#if 0
	/* month */
	if(splitedstring.stringout[2] > 12 || splitedstring.stringout[1] < 1){
		print_head();
		printf("<center>月份不能这样设定！ 请重试!</center>");
		return 0 ;
	}
	/* day */
	if(splitedstring.stringout[3] > 31 || splitedstring.stringout[1] < 1){
		print_head();
		printf("<center>天不能这样设定！ 请重试!</center>");
		return 0 ;
	}
	/* hour */
	if(splitedstring.stringout[4] > 24 || splitedstring.stringout[1] < 0){
		print_head();
		printf("<center>小时不能这样设定！ 请重试!</center>");
		return 0 ;
	}
	/* minute */
	if(splitedstring.stringout[5] > 59 || splitedstring.stringout[1] < 0){
		print_head();
		printf("<center>分钟不能这样设定！ 请重试!</center>");
		return 0 ;
	}
	/* year */
	if(splitedstring.stringout[1] < 1970){
		print_head();
		printf("<center>年份不能这样设定！ 请重试!</center>");
		return 0 ;
	}
#endif
//###################################################################curent time
//wpq
if(vfork() == 0){
                execl("/bin/date","/bin/date",NULL);

                return 0;
        }


	snprintf(stringtowrite,13,"%s%s%s%s%s",splitedstring.stringout[2],splitedstring.stringout[3],splitedstring.stringout[4],splitedstring.stringout[5],splitedstring.stringout[1]);

	if(vfork() == 0){
	        execl("/bin/date","/bin/date",stringtowrite,NULL);

	        return 0;
	}

	save();

	if(vfork() == 0){
                execl("/bin/date","/bin/date",stringtowrite,NULL);

                return 0;
        }

	settimeshow();

	return 0;
}
//######################################################################wpq
int showbohao(){

printf("Content-type:text/html\n\n");

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>GPRS拨号</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=56 checked>
<br>
欢迎使用GPRS拨号!
<br>
<br>
<input type=submit value=确定><br><br><br>
</center>
</form>
</body>
</html>
");
	return 0;
}

int bohao(){
	
	if(vfork() == 0){
	        execl("/cramfs/sbin/pppd","/cramfs/sbin/pppd","modem","/dev/ttyS1","115200","crtscts","connect","/sbin/chat -v -f /etc/config/chat.ttySx","debug", "noauth", "noipdefault", "defaultroute",NULL);

	        return 0;
	}
	save();
	showbohao();
	return 0;
}
//#########################################################################wpq
int showsetmac(){

    printf("Content-type:text/html\n\n"); 

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>MAC地址设置</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>

<br>
<br>
当前MAC地址
<br>
<br>

<input type=hidden name=flag value=59 checked>
<br>
MAC地址设置
<br>
<br>
<input type=text size=2 name=MAC地址1>&nbsp:&nbsp<input type=text size=2 name=MAC地址2>&nbsp:&nbsp<input type=text size=2 name=MAC地址3>&nbsp:&nbsp<input type=text size=2 name=MAC地址4>&nbsp:&nbsp<input type=text size=2 name=MAC地址5>&nbsp:&nbsp<input type=text size=2 name=MAC地址6>
<br>
<br>
<input type=submit value=确认><br><br><br>
</center>
</form>
</body>
</html>
");

	return 0;
}

int setmac(){
	char* querystring;
	struct splitback splitedstring,string;
	int i;
   	char stringtowrite[50];
	FILE *fp;

	querystring=getenv("QUERY_STRING");
	splitedstring=splitstring(querystring,'&');

	print_head();

	for(i=1;i<splitedstring.count;i++){
		string=splitstring(splitedstring.stringout[i],'=');
		snprintf(splitedstring.stringout[i],3,"%s",string.stringout[1]);
	}
	snprintf(stringtowrite,37,"/jffs2/sbin/setmac %s %s %s %s %s %s",splitedstring.stringout[1],splitedstring.stringout[2],splitedstring.stringout[3],splitedstring.stringout[4],splitedstring.stringout[5],splitedstring.stringout[6]);

	fp=fopen("/jffs2/etc/macset","w");
	fprintf(fp,"%s\n","#!/bin/sh");
	fclose(fp);

	fp=fopen("/jffs2/etc/macset","a");
	fprintf(fp,"\n%s\n",stringtowrite);
	fclose(fp);
#if 1
	fcntl(0/*STDOUT_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
	fcntl(1/*STDIN_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
	fcntl(2/*STDERR_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
#endif
#if 0 
	if(vfork() == 0){
	        execl("/jffs2/sbin/setmac","/jffs2/sbin/setmac",NULL);

	        return 0;
	}
#endif

	save();
	if(fork() == 0){
              execl("/jffs2/etc/macset","/jffs2/etc/macset",NULL);

                return 0;
      }

	show_reboot_hint();

	return 0;
}
//#######################################################################wpq
int showminigui(){
	
    printf("Content-type:text/html\n\n"); 

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>MINIGUI演示</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=62 checked>
<br>
欢迎您使用MINIGUI演示!
<br>
<br>
<input type=submit value=确认><br><br><br>
</center>
</form>
</body>
</html>
");

	return 0;
}

int minigui(){


	if(fork() == 0){
	        execl("/bin/sh","/bin/sh","/jffs2/guidemo/guitest",NULL);

	        return 0;
	}
	save();
	showminigui();
	return 0;
}
//##########################################################################wpq
int showchump(){
	
    printf("Content-type:text/html\n\n"); 

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>触摸屏演示</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=65 checked>
<br>
欢迎您使用触摸屏演示功能!
<br>
<br>
<input type=submit value=确认><br><br><br>
</center>
</form>
</body>
</html>
");

	return 0;
}

int chump(){
#if 0
	fcntl(0/*STDOUT_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
	fcntl(1/*STDIN_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
	fcntl(2/*STDERR_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
#endif
	if(vfork() == 0){
		execl("/bin/sh","/bin/sh","/jffs2/guidemo/guidemo",NULL);
		return 0;
	}

	save();
	showchump();
	return 0;
}
//#########################################################################wpq
int showbjdj(){
	
    printf("Content-type:text/html\n\n"); 

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>步进电机演示</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=68 checked>
<br>
欢迎您使用步进电机演示功能!
<br>
<br>
<input type=submit value=启动>&nbsp&nbsp&nbsp<input type=button value=停止 onClick=\"location='config.cgi?flag=69'\"><br><br><br>
</center>
</form>
</body>
</html>
");

	return 0;
}

int startbjdj(){
	FILE *fp;
	pid_t tmp;
	if((tmp=vfork()) == 0){
		execl("/cramfs/sbin/motor_test","/cramfs/sbin/motor_test",NULL);

		return 0;
	}
	else{
		//test_pid=tmp;
		fp=fopen("/tmp/motor_pid","w");
    		fprintf(fp,"%d\n",tmp);
    		fclose(fp);
	}		
	
	save();
	showbjdj();

	return 0;
}

int stopbjdj(){
	char pid[30];	
	//sprintf(pid,"%d",test_pid);
	FILE *fp;	
	fp=fopen("/tmp/motor_pid","r");
	memset(pid,0,30);
	fread(pid,1,10,fp);
	fclose(fp);
	if(vfork() == 0){
		execl("/bin/kill","/bin/kill","-9",pid,NULL);
		//execl("/bin/kill","/bin/kill","-9",test_pid,NULL);
	}
	//else{
	save();
	showbjdj();
	///}
	return 0;
}
//##########################################################################wpq
int showrecorder(){
	
    printf("Content-type:text/html\n\n"); 


printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>录音演示</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=71 checked>
<br>
欢迎您使用录音演示功能!
<br>
<br>
请输入录音时间：<input type=text size=2 name=录音时间>秒&nbsp&nbsp<input type=submit value=录音>
<br>
<br>
按键可以听到刚才录音的内容！&nbsp&nbsp<input type=button value=放音 onClick=\"location='config.cgi?flag=72'\">
<br>
<br><br><br>
</center>
</form>
</body>
</html>
");

	return 0;
}

int recorder(){
	char* querystring;
	struct splitback splitedstring,string;
	int i;
   	char stringtowrite[3];

	querystring=getenv("QUERY_STRING");
	splitedstring=splitstring(querystring,'&');

	for(i=1;i<splitedstring.count;i++){
		string=splitstring(splitedstring.stringout[i],'=');
		snprintf(splitedstring.stringout[i],3,"%s",string.stringout[1]);
	}
	snprintf(stringtowrite,3,"%s",splitedstring.stringout[1]);

	if(vfork() == 0){
	        execl("/jffs2/sbin/recorder","/jffs2/sbin/recorder","-n",stringtowrite,"&",NULL);

	        return 0;
	}
	save();
	showrecorder();
}

int playrecorder(){
	
	if(vfork() == 0){
		execl("/bin/cp","/bin/cp","/tmp/out.wav","/dev/sound/dsp",NULL);
		
		return 0;
	}
	
	save();
	showrecorder();
}

//###########################################################################wpq
int showplaymtv(){
	
    printf("Content-type:text/html\n\n"); 

printf("
<html>
<head>
<meta http-equiv=pragma content=no-cache><meta http-equiv=expire content=now>
<title>播放MTV</title>
</head>
<body bgcolor=#F1F1E4 text=black>
<form method=get action=/cgi-bin/config.cgi>
<br><br><br>
<div align=center>
<center>
<input type=hidden name=flag value=75 checked>
<br>
欢迎您MTV播放演示功能!
<br>
<br>
<input type=submit value=确认><br><br><br>
</center>
</form>
</body>
</html>
");

	return 0;
}

int playmtv(){

	mount("/dev/sda1","/tmp","vfat",0xc0ed0000,NULL);
	sleep(3);

#if 1
	fcntl(0/*STDOUT_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
	fcntl(1/*STDIN_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
	fcntl(2/*STDERR_FILENO*/,F_SETFD,1/*FD_CLOEXEC*/);
#endif
//print_head();
//	printf("<center>12345</center>");
	if(vfork() == 0){
//syslog("adasfafas");
	        execl("/jffs2/sbin/play","/jffs2/sbin/play",NULL);
	        //execl("/bin/sh","/bin/sh","/jffs2/mtv",NULL);

	        exit(255);
	}
	save();
	showplaymtv();
}
