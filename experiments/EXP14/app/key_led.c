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

#include "led_ioctl.h"

#define NOKEY 0

int main()
{
	int keybd_fd,led_fd,count;	
        char ret[7];  //ret[i] is the data send to LED[i]
      
        keybd_fd = open("/dev/keybd", O_RDONLY); //open keyboard
	if(keybd_fd<=0)
	{
                printf("open keybd device error!\n");
		return 0;
	}

        led_fd = open("/dev/led",O_RDWR); //open led.
	if (led_fd <= 0){
		printf("open led device error\n");
		return 0;
	}

	ret[0] = NOKEY;         //maintain the latest key value
        for (count=1; count<7; count++)
        {
                ret[count] = 0xdf;        //initiate to char '8'.
        }
                      
        ioctl(led_fd,IOCTRL_LED_1);       //light the first led:led[1]
        count = write(led_fd,ret+1,1);
	if (count != 1){
		printf("write device led error\n");
		return 0;
	}

        //light led[2]~led[6] with the init value '8'
        ioctl(led_fd,IOCTRL_LED_2);
	count = write(led_fd,ret+2,1);
        ioctl(led_fd,IOCTRL_LED_3);
	count = write(led_fd,ret+3,1);
        ioctl(led_fd,IOCTRL_LED_4);
	count = write(led_fd,ret+4,1);
        ioctl(led_fd,IOCTRL_LED_5);
	count = write(led_fd,ret+5,1);
        ioctl(led_fd,IOCTRL_LED_6);
	count = write(led_fd,ret+6,1);

        //poll endless
	while(1)
	{	
                read(keybd_fd,ret,1);

		if (ret[0] != NOKEY)   //when key pressed 
		{
//			printf("key = %c\n",ret[0]);
			
                        //keep ret[1~6] the latest 6 pressed key value.
                        for( count=7; count>0; count--)     
                        {
				ret[count] = ret[count-1];  //right rotate 
                        }
  
                        //light led[1~6]
                        ioctl(led_fd,IOCTRL_LED_1);
			count = write(led_fd,ret+1,1);
                        ioctl(led_fd,IOCTRL_LED_2);
			count = write(led_fd,ret+2,1);
                        ioctl(led_fd,IOCTRL_LED_3);
			count = write(led_fd,ret+3,1);
                        ioctl(led_fd,IOCTRL_LED_4);
			count = write(led_fd,ret+4,1);
                        ioctl(led_fd,IOCTRL_LED_5);
			count = write(led_fd,ret+5,1);
                        ioctl(led_fd,IOCTRL_LED_6);
			count = write(led_fd,ret+6,1);

		}//end if
		usleep(100000);       //free cpu a while
	}//end while	

	close(keybd_fd);
	close(led_fd);
	return 0;
}

