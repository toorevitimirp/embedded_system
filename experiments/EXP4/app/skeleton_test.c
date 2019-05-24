#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>


char * device = "/dev/skeleton";
int skeleton_fd;
char cBuffer[100];
char readchar[100];
int main(int argc, char *argv[])
{
   int i,count;
	char * constchar;
	constchar = "This is a skeleton device driver\n";
	sprintf(cBuffer,constchar);
	printf("driver info is : %s\n",cBuffer);
	fflush(stdout);
        skeleton_fd = open(device, O_RDWR);
        if (skeleton_fd == -1) {
              printf("Unable to open skeleton device");
                exit(0);
        }

	count = write(skeleton_fd,cBuffer,100);
	count = read(skeleton_fd,readchar/*cBuffer*/,100);
	printf("driver info is : %s\n",readchar/*cBuffer*/);
	fflush(stdout);
	close(skeleton_fd);
}	

					
