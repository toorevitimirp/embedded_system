#include <stdio.h>
#include <fcntl.h>

main()
{
	   int i = 0;
		unsigned char Send[8] =
		{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
		int fd;
		fd = open("/dev/CAN/MCP2510",O_RDWR);
		printf("Send data to Can bus\n");
	   write(fd,Send,8);
		for(i = 0 ; i < 8; i++)
			printf("Send num[%d] is %x\n",i,Send[i]);

	   close(fd);
	   return;
}

