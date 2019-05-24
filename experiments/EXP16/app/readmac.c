#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	int fd,i;
	unsigned char send[] = {0x00,0x13,0xf6,0x6c,0x87,0x89};
	unsigned char receive[6];

	if((fd = open("/dev/X1227/e2prom",O_RDWR)) < 0)
	{
		perror("Open device error!\n");
		exit(255);
	}
/*
	for(i=0;i<6;i++){
		printf("send[%d] = %x\n",i,send[i]);
	}

	write(fd,send,6);
*/
	read(fd,receive,6);
	for(i=0;i<6;i++)
	{
		printf("The data[%d] is %x.\n",i,receive[i]);
	}
	return 0;
}
