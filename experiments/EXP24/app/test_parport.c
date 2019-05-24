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
	int parport_fd,count;	
	unsigned char buf[2];
	parport_fd = open("/dev/parport",O_RDWR);
	if (parport_fd <= 0){
		printf("open parport device error\n");
		return 0;
	}
	buf[0]=0xa5;buf[1]=0x03;
	count = write(parport_fd,buf,2);
	printf("Write %d char to parprot\n",count);
	read(parport_fd,buf,1);
	printf("Read %02x from parport\n",buf[0]);
		
	close(parport_fd);
	return 0;
}

	
