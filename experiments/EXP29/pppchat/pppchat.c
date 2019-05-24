#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int system1(char *string){
    FILE *fp;
    int pid;
    int i;
    fp=fopen("/tmp/tmp1","w");
    fprintf(fp,"%s",string);
    i=fclose(fp);
    printf("before fork\n");
    if ((pid=vfork())<0){
        printf("vfork error\n");
    }
    else if (pid==0){
        execl("/bin/sh","/bin/sh","/tmp/tmp1",0);
	exit(0);
    }
    else{
        if(waitpid(pid,NULL,0)!=pid)
	    printf("wait pid error!<br>");
	unlink("/tmp/tmp1");
	return (0);    
    }
}

int main(int argc,char *argv[])
{
    char stringtowrite[200];
    
    //let executable
//    snprintf(stringtowrite,100,"chmod 0755 /etc/config/connect");
//    system1(stringtowrite);
       
    bzero(stringtowrite,200);
    snprintf(stringtowrite,200,"/mnt/pppd-ppc/pppd modem /dev/ttyS0 115200 crtscts connect '/mnt/pppd-ppc/chat -v -f /mnt/pppd-ppc/chat.ttyS1' name 16388 debug noauth noipdefault defaultroute ");//dial
//	snprintf(stringtowrite,200,"/mnt/pppd/pppd modem /dev/ttyS2 19200 crtscts connect '/mnt/pppd/chat -v -f /mnt/pppd/chat.ttyS1' name 16388 debug defaultroute ");
   system1(stringtowrite);
   return 0;
}


