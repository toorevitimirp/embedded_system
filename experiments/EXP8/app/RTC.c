//====================================================================
// File Name : RTC.c
// Function  : User Application
// Date      : July 16, 2004
// Version   : 0.0
//====================================================================

#include<stdio.h>
#include <fcntl.h>

main()
{
	int fd;
	int i = 0;
	unsigned char CurTime [6];		//Current Time
	unsigned char SetTime[] =        //Set Time
	{
		0x30,0x00,  //second
		0x31,0x50,  //minute
		0x32,0x12,  //hour
		0x33,0x15,  //date
		0x34,0x09,  //month
                0x35,0xA4,  //year
	};
	fd = open("/dev/X1227/rtc",O_RDWR);
	write(fd,SetTime,18);
	/*Read Time*/
//	sleep(10);
	read(fd,CurTime,6);
	for(i = 0; i< 6; i++)
	{		
	    printf("the current time %d is %x\n",i, CurTime[i]);
	}
	close(fd);
	return;
}
