/*
This test.c is used to test the receive of the serial.
It proved that whenever it meet the 0xod,it will take it place 
with 0x0a. the others are correct.

*/

#include <stdio.h>
#include <termio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <error.h>
#include <string.h>

#define PORT "/dev/ttyS1"

int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B300};
int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 300};


int set_speed(int fd, int speed)
{
    int i;
    int status;
    struct termios options;

    if (tcgetattr(fd,&options)!= 0)
    {
        perror("set up serial port");
        return (-1);
    }

    for (i = 0; i < sizeof(speed_arr)/sizeof(int); i++)
    {
        if (speed == name_arr[i])
        {
	    tcflush(fd, TCIOFLUSH);
	    cfsetispeed(&options, speed_arr[i]);
            cfsetospeed(&options, speed_arr[i]);
	    status = tcsetattr(fd, TCSANOW, &options);
            if (status != 0)
	    {
		perror("tcsetattr ttys");
		return(-1);
	    }
	}
	tcflush(fd, TCIOFLUSH);
    }

    return 0;
}  


int set_Parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;

    if(tcgetattr(fd,&options)!=0){
	perror("Setup Serial Port");
	return -1;
    }
    
    options.c_cflag &= ~CSIZE;
  
    switch (databits) 
    {
	case 7:
	    options.c_cflag |= CS7;
	    break;
	case 8:
	    options.c_cflag |= CS8;
	    break;
	default:
	    fprintf(stderr,"Unsupported data size\n");
	    return -1;
    }

    switch (parity)
    {
	case 'n':
	case 'N':   /* no parity*/
	    options.c_cflag &= ~PARENB;	    // Clear parity enable 
	    options.c_iflag &= ~INPCK;	    // Disnable input parity checking
	    break;
	case 'o':
	case 'O':
	    options.c_cflag |= (PARODD | PARENB);   // ...... 
	    options.c_iflag |= INPCK;	    // Enable input parity checking
	    break;
	case 'e':
	case 'E':
	    options.c_cflag |= PARENB;	    // Enable parity 
	    options.c_cflag &= ~PARODD;	    // ......  
	    options.c_iflag |= INPCK;	    // Enable input parity checking 
	    break;
	case 'S':
	case 's':   /*as no parity*/
	    options.c_cflag &= ~PARENB;
	    options.c_cflag &= ~CSTOPB;
	    break;
	default:
	    fprintf(stderr,"Unsupported parity\n");
	    return -1;
    }

    switch (stopbits)
    {
	case 1:
	    options.c_cflag &= ~CSTOPB;
	    break;
	case 2:
	    options.c_cflag |= CSTOPB;
	    break;
	default:
	    fprintf(stderr,"Unsupported stop bits\n");
	    return -1;
    }

    options.c_cc[VTIME] = 150; // 20 seconds
    options.c_cc[VMIN] = 0;

    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
    
    if(tcsetattr(fd,TCSANOW,&options)!=0){
	perror("Setup Serial Port");
	return -1;
    }
    
    return 0;
}


int  main()
{
    int fd=0;
    int i,j;
   
    unsigned  char Reqbuf[4];
    unsigned char Resbuf[8];

    struct termios options;

    if ((fd=open(PORT,O_RDWR|O_NDELAY,0))<0)
    {
        printf("Open Port error!\n");
        return -1;
    }

    if(set_speed(fd,115200)== -1)
    {
	printf("Set Serial Port 0 Speed Error! \n");
	close(fd);
	return -1;
    }
    if(set_Parity(fd,8,1,'N')== -1)
    {
	printf("Set Serial Port 0 Parity Error! \n");
	close(fd);
	return -1;
    }

    tcgetattr(fd, &options);
    
  options.c_lflag = 0;
  options.c_oflag = 0;
//    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  //  options.c_oflag &= ~OPOST;
 
    options.c_cc[VTIME] = 240; // 24 seconds
    options.c_cc[VMIN] = 14;
    tcsetattr(fd, TCSANOW, &options);

    Reqbuf[0] = 0x7a;
    Reqbuf[1] = 0x0d;
    Reqbuf[2] = 0x0a;
    Reqbuf[3] = 0x6b;  //send a data of 4_byte

    while (1)
    {
        j = write(fd,Reqbuf,4);
        printf("\nhave writed %d bytes data!\n",j);
        usleep(900000);
     
        Resbuf[0] = 0x00;
        Resbuf[1] = 0x00;
 	Resbuf[2] = 0x00;
        Resbuf[3] = 0x00;        

        i = read(fd,Resbuf,8);
        printf("had read %d bytes data!\n",i);

	if (i>0)
	{	
            for (j=0;j<i;j++)
            {
                printf("the data[%d]is %x\n",j,Resbuf[j]);
            }
	}

     }

     return 0;
}
  






