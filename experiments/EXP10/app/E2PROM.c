//====================================================================
// File Name : E2PROM.c
// Function  : User Testing Iic Program
// Date      : July 16, 2004
// Version   : 0.0
//====================================================================

#include<stdio.h>
#include <fcntl.h>

main()
{
	int fd,i;
	unsigned char send[3],receiver[3];
        fd = open("/dev/X1227/e2prom", O_RDWR);
	if(fd < 0)
	{	
	  printf("Error\n");
	  return 0;
	}
	send[0] = 0xcc;
	send[1] = 0xbb;
	send[2] = 0xaa;
	write(fd,send,3);

	read(fd,receiver,3);
	for(i = 0; i<3 ;i++)
	{
	    printf("receive the num[%d] is %x\n",i,receiver[i]);
	}
	close(fd);
	return 0;

}

