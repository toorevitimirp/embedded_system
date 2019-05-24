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


void _Wr2410Iic(U32 slvAddr,U32 addr,U32 wdata);
void _Rd2410Iic(U32 slvAddr,U32 addr,U32 *rdata);


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
void _Wr2410Iic(U32 slvAddr,U32 addr,U32 wdata)
{
    _iicMode      = WRDATA;
    _iicPt        = 0;
    _iicData[0]   = (U8)addr;
    _iicData[1]   = wdata/256;
    _iicData[2]   = wdata;
    _iicDataCount = 3;
    Uart_Printf("MSB is: %2x \n",_iicData[1]);
    Uart_Printf("LSB is: %2x \n",_iicData[2]);
    
    rIICDS        = slvAddr&0xfe;            //0x30
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
        _iicStatus = 0x100;             //To check if _iicStatus is changed 
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

		
//************************[ _Rd2410Iic ]********************************
void _Rd2410Iic(U32 slvAddr,U32 addr,U32 *rdata)
{
	PDEBUG("read---->0\n");
    _iicMode      = SETRDADDR;
    _iicPt        = 0;
    _iicData[0]   = (U8)addr;
    _iicDataCount = 1;
	PDEBUG("read---->1\n");
    
    rIICDS   = slvAddr&0xfe;			//0x31
    rIICSTAT = 0xf0;                    //MasTx,Start  
      //Clearing the pending bit isn't needed because the pending bit has been cleared.
	PDEBUG("read---->2\n");
    while(_iicDataCount!=-1)
        Run_IicPoll();
	
    _iicMode      = RDDATA;
    _iicPt        = 0;
    _iicDataCount = 2;
    
    rIICDS   = slvAddr|0x01;
    rIICSTAT = 0xb0;                    //Master Rx,Start
    rIICCON  = 0xaf;                    //Resumes IIC operation.   
    while(_iicDataCount!=-1)
        Run_IicPoll();

    *rdata = _iicData[1]*256+_iicData[2];
	
}

//**********************[ Run_IicPoll ]*********************************
void Run_IicPoll(void)
{
    if(rIICCON & 0x10)                  //Tx/Rx Interrupt Enable
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
            rIICDS = _iicData[_iicPt++];        //_iicData[0] has dummy.
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


//*********************[ Command ]*********************************
unsigned char command[] = {
	0x00, 0x0F, 0x06,  //Sysclock
	0x01, 0x00, 0x00,
	0x02, 0xaf, 0xff,  //line in
	0x03, 0x00, 0x00,
	0x04, 0x02, 0x02,  //default
	0x10, 0x00, 0x00,
	0x11, 0x00, 0x00,
	0x12, 0x00, 0x00,
	0x13, 0x00, 0x00,
	0x14, 0x00, 0x00,
	0x20, 0x00, 0x00,
	0x21, 0x00, 0x00,
	0x22, 0x00, 0x0C,  //mic
	0x23, 0x00, 0x00
};


//*********************[ uda1380_init ]*********************************
int uda1380_init(void)
{
    int ic;
    unsigned int msb, lsb, data, reg;
  
    address_map();
	
    rGPEUP  |= 0xc000;                  //Pull-up disable
    rGPECON |= 0xa00000;                //GPE15:IICSDA , GPE14:IICSCL    
    
    //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);

    rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)
   
    for (ic = 0; ic < 42; ic += 3) {
		reg = command[ic];
		msb = command[ic + 1];
		lsb = command[ic + 2];
		data = msb << 8 | lsb;
        	_Wr2410Iic(0x30,(U8)reg,data);
    }
    return 0;
}

