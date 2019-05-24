//====================================================================
// File Name : 2410iic.c
// Function  : S3C2410 IIC-bus Master Tx/Rx mode Program
// Date      : July 16, 2004
// Version   : 0.0
//====================================================================
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/vmalloc.h>

#include "khead.h"

#define IICBUFSIZE 0x20
#define BIT_IIC        (0x1<<27)


#define rGPECON    (*(volatile unsigned long *)r_GPECON) //Port E control
#define rGPEUP     (*(volatile unsigned long *)r_GPEUP) //Pull-up control E

#define rIICCON  (*(volatile unsigned char *)r_IICCON) //IIC control
#define rIICSTAT (*(volatile unsigned char *)r_IICSTAT) //IIC status
#define rIICADD  (*(volatile unsigned char *)r_IICADD) //IIC address
#define rIICDS   (*(volatile unsigned char *)r_IICDS) //IIC data shift


#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)

#define Uart_Printf PDEBUG

#define U8 unsigned char
#define U32 unsigned int
static unsigned char _iicData[IICBUFSIZE];
static volatile int _iicDataCount;
static volatile int _iicStatus;
static volatile int _iicMode;
static int _iicPt;

extern void Test_Iic(void);
extern void Init_Iic(void);
extern void _Wr2410Iic_E2PROM(U32 slvAddr,U32 addr,U8 *buffer,U32 length);
extern void _Wr2410Iic(U32 slvAddr,U32 addr,U8 wdata);
extern void _Rd2410Iic_E2PROM(U32 slvAddr,U32 addr,U8 *buffer,U32 length);

void Run_IicPoll(void);
void IicPoll(void);

int address_map(void);
	
unsigned long r_GPECON,r_GPEUP;
unsigned long r_IICCON,r_IICSTAT,r_IICADD,r_IICDS;

//=================================================================== 
//*********************[ address_map ]*********************************
int address_map(void)
{
	r_GPECON	=__ioremap(0x56000040,4,0);
	r_GPEUP		=__ioremap(0x56000048,4,0);
	r_IICCON	=__ioremap(0x54000000,4,0);
	r_IICSTAT	=__ioremap(0x54000004,4,0);
	r_IICADD	=__ioremap(0x54000008,4,0);
	r_IICDS		=__ioremap(0x5400000c,4,0);

	return 0;
}

//*********************[ Delay ]*********************************
void Delay(int times)
{
	udelay(1000);
}


//**************[ _Wr2410Iic ]*****************************************
void _Wr2410Iic_E2PROM(U32 slvAddr,U32 addr,U8 *buffer,U32 length)
{
     //X1227 Page Write mode
     int i = 0;
    _iicMode      = WRDATA;
    _iicPt        = 0;
    _iicData[0]   = addr/256;
    _iicData[1]   = addr;
    for(i = 2; i<2+length; i++)
    {
    	_iicData[i]   = buffer[i-2];
    }
    
    _iicDataCount = length+2;
     
    rIICDS        = slvAddr&0xfe;            
      //Master Tx mode, Start(Write), IIC-bus data output enable
      //Bus arbitration sucessful, Address as slave status flag Cleared,
      //Address zero status flag cleared, Last received bit is 0
    rIICSTAT      = 0xf0;      
      //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
       Run_IicPoll();

    _iicMode = POLLACK;

    while(1)
    {
        rIICDS     =slvAddr&0xfe;
        _iicStatus = 0x100;              //To check if _iicStatus is changed 
        rIICSTAT   = 0xf0;              //Master Tx, Start, Output Enable, Sucessful, Cleared, Cleared, 0
        rIICCON    = 0xaf;              //Resumes IIC operation. 
	 while(_iicStatus==0x100)  
            Run_IicPoll();
              
        if(!(_iicStatus & 0x1))
		PDEBUG("ACK is received\n");
        break;                      //When ACK is received
    }
    rIICSTAT = 0xd0;                    //Master Tx condition, Stop(Write), Output Enable
    rIICCON  = 0xaf;                    //Resumes IIC operation. 
    Delay(1);                           //Wait until stop condtion is in effect.
    //Write is completed.
}

//**************[ _Wr2410_Iic ]*****************************************
void _Wr2410Iic(U32 slvAddr,U32 addr,U8 wdata)
{

     //X1227 byte write mode
    _iicMode      = WRDATA;
    _iicPt        = 0;
    _iicData[0]   = addr/256;
    _iicData[1]   = addr;
    _iicData[2]   = wdata;

    _iicDataCount = 3;

    rIICDS        = slvAddr&0xfe;
      //Master Tx mode, Start(Write), IIC-bus data output enable
      //Bus arbitration sucessful, Address as slave status flag Cleared,
      //Address zero status flag cleared, Last received bit is 0
    rIICSTAT      = 0xf0;
      //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
       Run_IicPoll();

    _iicMode = POLLACK;

    while(1)
    {
        rIICDS     =slvAddr&0xfe;
        _iicStatus = 0x100;              //To check if _iicStatus is changed
        rIICSTAT   = 0xf0;              //Master Tx, Start, Output Enable, Sucessful, Cleared, Cleared, 0
        rIICCON    = 0xaf;              //Resumes IIC operation.
         while(_iicStatus==0x100)
            Run_IicPoll();

        if(!(_iicStatus & 0x1))
                PDEBUG("ACK is received\n");
        break;                      //When ACK is received
    }
    rIICSTAT = 0xd0;                    //Master Tx condition, Stop(Write), Output Enable
    rIICCON  = 0xaf;                    //Resumes IIC operation.
    Delay(1);                           //Wait until stop condtion is in effect.    //Write is completed.
}


		
//************************[ _Rd2410Iic ]********************************
void _Rd2410Iic_E2PROM(U32 slvAddr,U32 addr,U8 *buffer,U32 length)
{
     //X1227 Random read && Sequential read
     int i = 0;
     PDEBUG("read---->0\n");
    _iicMode      = SETRDADDR;
    _iicPt        = 0;
    _iicData[0]   = addr/256;
    _iicData[1]   = addr;
    _iicDataCount = 2;
    
    rIICDS   = slvAddr&0xfe;
    rIICSTAT = 0xf0;                    //MasTx,Start  

    //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
        Run_IicPoll();
	
    _iicMode      = RDDATA;
    _iicPt        = 1;
    _iicDataCount = length;
    
    rIICDS   = slvAddr|0x01;
    rIICSTAT = 0xb0;                    //Master Rx,Start
    rIICCON  = 0xaf;                    //Resumes IIC operation.   
    while(_iicDataCount!=-1)
        Run_IicPoll();

    printk("In the Read Function\n");

    for(i = 0; i<length;i++)
    {
		buffer[i] = _iicData[i+2];
//		printk("The read num %d is %x\n",i,_iicData[i+2]);
    }
   
	
}

//**********************[ Run_IicPoll ]*********************************
void Run_IicPoll(void)
{
    if(rIICCON & 0x10)                  
       IicPoll();
}       
    
//**********************[IicPoll ]**************************************
void IicPoll(void)
{
    U32 iicSt,i;
    
    iicSt = rIICSTAT; 
    if(iicSt & 0x8){
	PDEBUG("bus arbitration is failed\n");
			}   //When bus arbitration is failed.
    if(iicSt & 0x4){
	PDEBUG("matched\n");
	}                   //When a slave address is matched with IICADD
    if(iicSt & 0x2){
	PDEBUG("slave address 0000000b\n");
	}                   //When a slave address is 0000000b
    if(iicSt & 0x1){
	PDEBUG("Ack isn't received\n");
	}                   //When ACK isn't received

    switch(_iicMode)
    {
        case POLLACK:
		PDEBUG("poll--ACK\n");
            _iicStatus = iicSt;
            break;

        case RDDATA:
              PDEBUG("read--ACK\n");
            if((_iicDataCount--)==0)
            {
                _iicData[_iicPt++] = rIICDS;
            
                rIICSTAT = 0x90;                //Stop MasRx condition 
                rIICCON  = 0xaf;                //Resumes IIC operation.
                Delay(1);                       //Wait until stop condtion is in effect.
                                                //Too long time... 
                                                //The pending bit will not be set after issuing stop condition.
                break;    
            }      
            _iicData[_iicPt++] = rIICDS;
                        //The last data has to be read with no ack.
            if((_iicDataCount)==0)
                rIICCON = 0x2f;                 //Resumes IIC operation with NOACK.  
            else 
                rIICCON = 0xaf;                 //Resumes IIC operation with ACK
            break;

        case WRDATA:
		PDEBUG("write--ACK\n");
            if((_iicDataCount--)==0)
            {
                rIICSTAT = 0xd0;                //stop MasTx condition 
                rIICCON  = 0xaf;                //resumes IIC operation.
                Delay(1);                       //wait until stop condtion is in effect.
                       //The pending bit will not be set after issuing stop condition.
                break;    
            }
		PDEBUG("poll--->write\n");
            rIICDS = _iicData[_iicPt++];        
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;                     //resumes IIC operation.
            break;

        case SETRDADDR:
		PDEBUG("SETRDADDR\n");
            if((_iicDataCount--)==0)
            {
                break;                  //IIC operation is stopped because of IICCON[4]    
            }
            rIICDS = _iicData[_iicPt++];
            for(i=0;i<10;i++);          //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;             //resumes IIC operation.
            break;

        default:
            break;      
    }
}

void Init_Iic(void)
{
	address_map();
	
 	rGPEUP  |= 0xc000;                  //Pull-up disable
  	rGPECON |= 0xa00000;                //GPE15:IICSDA , GPE14:IICSCL    
    
        //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
        rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);

        rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)
}

void Test_Iic(void)
{
    unsigned int save_E,save_PE;
    unsigned char buffer[3],read[3];
    unsigned int len,j;
//    static U8 rdata;
//    static U8 wdata;
    static U32 i;
    address_map();

    save_E   = rGPECON;
    save_PE  = rGPEUP;

    rGPEUP  |= 0xc000;                  //Pull-up disable
    rGPECON |= 0xa00000;                //GPE15:IICSDA , GPE14:IICSCL

    //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);

    rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)

    _Wr2410Iic(0xde,0x3f,0x02);	

    printk("--------->1\n");
    buffer[0] = 0x11;
    buffer[1] = 0x22;
    buffer[2] = 0x33;
    len = 3;
    i=0x0;
 
    _Wr2410Iic_E2PROM(0xae,i,buffer,len);
    for(j = 0; j<len; j++)
    {
    	printk("The writing data is:%x\n",buffer[j]);
    }

    printk("--------->2\n");
    _Rd2410Iic_E2PROM(0xaf,i,read,len);
    for(j = 0; j<len; j++)
    {
    	printk("The reading data is:%x \n",read[j]);
    }


    printk("--------->3\n");

    rGPECON = save_E;
    rGPEUP = save_PE;
}

