/* dac0832_test.c
 *  this is a dac0832 char device driver test program.
 * Any problem pls contact support@hhcn.com
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <syslog.h>
#include <signal.h>

int main()
{
	int dac_fd,count;
	int select = 0;	
	int i = 0;
	dac_fd = open("/dev/dac0832",O_RDWR);
	if (dac_fd <= 0){
		printf("open dac0832 device error\n");
		return 0;
	}

	printf("please input  v:\n");
	scanf("%d",&select);
	//printf("select:%d\n",select);
	//count = write(dac_fd,select,1);
	while(1)
	{	
		char ret[2];
		ret[0] = select;
			//printf("key = %c\n",ret[0]);
		count = write(dac_fd,ret,1);
		if (count != 0){
		  printf("data = %d\n", ret[0]);
		  //printf("read device dac error\n");
		  //return 0;
		}
		usleep(100000);
	}	
	close(dac_fd);
	return 0;
}

