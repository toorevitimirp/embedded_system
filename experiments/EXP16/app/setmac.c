#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char * argv[])
{
	int fd,i,j;
	unsigned char receive[6];
	unsigned char send[6];

	if((fd = open("/dev/X1227/e2prom",O_RDWR)) < 0)
	{
		perror("Open device error!\n");
		exit(255);
	}
	for(i=0;i<6;i++){
		j = i + 1;
		send[i] = strtol(argv[j],NULL,16);
		//printf("send[%d] = %x    ",i,send[i]);
		//usleep(10);
		//printf("  argv[%d] = %s\n",j,argv[j]);
		//usleep(10);
	}

	if(argc == 7)
	{
		write(fd,send,6);
	}
	close(fd);

	printf("The new MAC address : %2X:%2X:%2X:%2X:%2X:%2X .\n",send[0],send[1],send[2],send[3],send[4],send[5]);
/*
	if((fd = open("/dev/X1227/e2prom",O_RDWR)) < 0)
	{
		perror("Open device error!\n");
		exit(255);
	}
	read(fd,receive,6);
	for(i=0;i<6;i++)
	{
		printf("The data[%d] is %x.\n",i,receive[i]);
	}
	close(fd);
*/
	return 0;
}
