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
	int electromotor_fd,count;	
		char ret;
	electromotor_fd = open("/dev/electromotor",O_RDWR);
	if (electromotor_fd <= 0){
		printf("open electromotor device error\n");
		return 0;
	}
	while(1)
	{	
	/* send message to electromotor and make it run */	
		ret = 0x7;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
		ret = 0x3;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
		ret = 0xb;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
		ret = 0x9;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
		ret = 0xd;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
		ret = 0xc;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
		ret = 0xe;	
		count = write(electromotor_fd,&ret,1);
		usleep(10000);
	
		//usleep(100000);
	}	
	close(electromotor_fd);
	return 0;
}

	
