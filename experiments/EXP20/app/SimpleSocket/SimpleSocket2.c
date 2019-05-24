#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include<sys/types.h>
#include<sys/stat.h>
#include <stdlib.h>
#include "../gui/gui.h"
#include "../handpad/handpad.h"
#include "./trans/ethernet.c"
#define MAX 270000
                                                                                                                  
/*传输时用来显示提示方框和文字*/
void display()
{
 textout(30,20,"Now transport the File",0x07e0,0x1111);
 rectangle(40,40,190,90);
 textout(30,100,"Now having finished    %",0xf800,0x3333);
}
                                                                                                                  
/*主函数*/
main()
{
 /*存放接收到的文件内容*/
 char bmpbuf[MAX];
                                                                                                                  
 /*初始化液晶屏*/
 initgraph();
                                                                                                                   
 /*清屏*/
 clearscreen();
                                                                                                                  
 /*显示提示文字*/
 display();
                                                                                                                  
 /*将缓冲区置零,否则将会出现程序运行时异常,这一步非常重要*/
 memset(bmpbuf,0,MAX);
                                                                                                                  
 /*从服务器伺服程序接收指定的文件*/
 recvfile(bmpbuf,"look.bmp");
 clearscreen();
                                                                                                                  
 /*直接从缓冲区进行输出*/
 ShowBuf(bmpbuf,0,0);
}
