/* adc0809_test.c
 *  this is a adc0809 char device driver test program.
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

#include "adc0809_ioctl.h"

int main()
{
	int adc_fd,count;
	int select = 0;	
	int i = 0;
	adc_fd = open("/dev/adc0809",O_RDWR);
	if (adc_fd <= 0){
		printf("open adc0809 device error\n");
		return 0;
	}

	printf("please input which input do u want:\n");
	scanf("%d",&select);
	ioctl(adc_fd,select,select);//start collect and convert
	while(1)
	{	
		char ret[2];
		count = read(adc_fd,ret,1);//read ad converter's result
		if (count != 0){
		  printf("key = %d\n", ret[0]);
		}
		usleep(100000);
	}	
	close(adc_fd);
	return 0;
}

