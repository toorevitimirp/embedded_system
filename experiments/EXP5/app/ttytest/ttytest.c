#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rs232.h"

int main(int argc, char *argv[])
{

	int ret,portno,nWritten,nRead;
	char buf[256];
	portno=0;
	while(1)
	{
		ret=OpenCom(portno,"/dev/ttyS1",115200);
		if(ret==-1)
		{
			perror("The /dev/ttyS1 open error.");
			exit(1);
		}
		nWritten=ComWrt(portno,"abc",3);
		printf("/dev/ttyS1 has send %d chars!\n",nWritten);
		printf("\nRecieving data!**");
		fflush(stdout);
		nRead=ComRd(0,buf,256,3000);
		if(nRead>0)
		{
			printf("*****OK\n");
		}
		else
			printf("Timeout\n");
		if((ret=CloseCom(portno)==-1))
		{
			perror("Close com");
			exit(1);
		}
		printf("\n\n");
	}
	printf("Exit now.\n");
	return;
}
