#include <stdio.h>
#include <fcntl.h>

main()
{
	   int i = 0;
	   unsigned char Receive[8];
		int fd;
		fd = open("/dev/CAN/MCP2510",O_RDWR);
		printf("Receive data to Can bus\n");
		read(fd,Receive,8);
		for(i = 0; i < 8; i++)
			printf("Receive num[%d] is %x\n",i,Receive[i]);
		close(fd);
		return;
}

