/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <cmd_nvedit.h>
#include <cmd_bootm.h>
#include <malloc.h>
#if defined(CONFIG_BOOT_RETRY_TIME) && defined(CONFIG_RESET_TO_RETRY)
#include <cmd_boot.h>		/* for do_reset() prototype */
#endif

#ifdef CFG_HUSH_PARSER
#include <hush.h>
#endif
#define CONFIG_START_LINUX     1 //HHTECH
#define MAX_DELAY_STOP_STR 32

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static int parse_line (char *, char *[]);
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
static int abortboot(int);
#endif

#undef DEBUG_PARSER

char        console_buffer[CFG_CBSIZE];		/* console I/O buffer	*/

static char erase_seq[] = "\b \b";		/* erase sequence	*/
static char   tab_seq[] = "        ";		/* used to expand TABs	*/

#ifdef CONFIG_BOOT_RETRY_TIME
static uint64_t endtime = 0;  /* must be set, default is instant timeout */
static int      retry_time = -1; /* -1 so can call readline before main_loop */
#endif

#define	endtick(seconds) (get_ticks() + (uint64_t)(seconds) * get_tbclk())

#ifndef CONFIG_BOOT_RETRY_MIN
#define CONFIG_BOOT_RETRY_MIN CONFIG_BOOT_RETRY_TIME
#endif

#ifdef CONFIG_MODEM_SUPPORT
int do_mdm_init = 0;
extern void mdm_init(void); /* defined in board.c */
#endif

/////////////////////////////////////////////////////////add by rzhuo hhcn 
#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)

#define U8 unsigned char
#define U32 unsigned int
#define IICBUFSIZE 0x20
static unsigned char _iicData[IICBUFSIZE];
static volatile int _iicDataCount;
static volatile int _iicStatus;
static volatile int _iicMode;
static int _iicPt;

int Disable_WatchDog(void);
void _Wr2410Iic(U32 slvAddr,U32 addr,U8 wdata);
void _Rd2410Iic(U32 slvAddr,U32 addr,U8 *rdata);
void Init_Iic(void);
void Run_IicPoll(void);
void IicPoll(void);
#define PDEBUG printf
//**************[ _Wr2410Iic ]*****************************************
void _Wr2410Iic(U32 slvAddr,U32 addr,U8 wdata)
{

	int i;
     //X1227 byte write mode
    _iicMode      = WRDATA;
    _iicPt        = 0;
    _iicData[0]   = addr/256;
    _iicData[1]   = addr;
    _iicData[2]   = wdata;

    _iicDataCount = 3;
   
    //rIICDS = slvAddr&0xfe;  
      (*(volatile unsigned long *)0x5400000c) = slvAddr&0xfe;            
	
      //Master Tx mode, Start(Write), IIC-bus data output enable
      //Bus arbitration sucessful, Address as slave status flag Cleared,
      //Address zero status flag cleared, Last received bit is 0
      
	//rIICSTAT      = 0xf0;      
	 (*(volatile unsigned long *)0x54000004) = 0xf0;
	  
      //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
       Run_IicPoll();

    _iicMode = POLLACK;

    while(1)
    {
        //rIICDS     =slvAddr&0xfe;
        (*(volatile unsigned long *)0x5400000c) = slvAddr&0xfe;
		
        _iicStatus = 0x100;              //To check if _iicStatus is changed 
        
        //rIICSTAT   = 0xf0;              //Master Tx, Start, Output Enable, Sucessful, Cleared, Cleared, 0
         (*(volatile unsigned long *)0x54000004) = 0xf0;
	
       // rIICCON    = 0xaf;              //Resumes IIC operation. 
       (*(volatile unsigned long *)0x54000000) = 0xaf;
	 while(_iicStatus==0x100)  
            Run_IicPoll();
              
        //if(!(_iicStatus & 0x1))
	//PDEBUG("ACK is received\n");
        break;                      //When ACK is received
    }
    //rIICSTAT = 0xd0;                    //Master Tx condition, Stop(Write), Output Enable
     (*(volatile unsigned long *)0x54000004) = 0xd0;  

   //rIICCON  = 0xaf;                    //Resumes IIC operation. 
   (*(volatile unsigned long *)0x54000000) = 0xaf;

   for(i = 0 ; i<100; i++);						//Wait until stop condtion is in effect.
    //Write is completed.
}


//************************[ _Rd2410Iic ]********************************
void _Rd2410Iic(U32 slvAddr,U32 addr,U8 *rdata)
{
    //X1227 random read mode	
    _iicMode      = SETRDADDR;
    _iicPt        = 0;
    _iicData[0]   = addr/256;
    _iicData[1]   = addr;
    _iicDataCount = 2;
    
//    rIICDS   = slvAddr&0xfe;
    (*(volatile unsigned long *)0x5400000c) = slvAddr&0xfe;    


//    rIICSTAT = 0xf0;                    //MasTx,Start  
 	(*(volatile unsigned long *)0x54000004) = 0xf0;

    //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1)
        Run_IicPoll();
	
    _iicMode      = RDDATA;
    _iicPt        = 1;
    _iicDataCount = 1;
    
   // rIICDS   = slvAddr|0x01;
     (*(volatile unsigned long *)0x5400000c) = slvAddr|0x01;
   
   //rIICSTAT = 0xb0;                    //Master Rx,Start
   (*(volatile unsigned long *)0x54000004) = 0xb0;   
   
   // rIICCON  = 0xaf;                    //Resumes IIC operation.   
    (*(volatile unsigned long *)0x54000000) = 0xaf;
    
    while(_iicDataCount!=-1)
        Run_IicPoll();

    *rdata = _iicData[2];
	
}
		
//**********************[ Run_IicPoll ]*********************************
void Run_IicPoll(void)
{
    unsigned char buffer;
    buffer =  (*(volatile unsigned long *)0x54000000);
    if(buffer & 0x10)                  
         IicPoll();
}       
    
//**********************[IicPoll ]**************************************
void IicPoll(void)
{
    U32 iicSt,i;
    
   // iicSt = rIICSTAT; 
    iicSt = (*(volatile unsigned long *)0x54000004);
   
    if(iicSt & 0x8){
	//PDEBUG("bus arbitration is failed\n");
			}   //When bus arbitration is failed.
    if(iicSt & 0x4){
	//PDEBUG("matched\n");
	}                   //When a slave address is matched with IICADD
    if(iicSt & 0x2){
	//PDEBUG("slave address 0000000b\n");
	}                   //When a slave address is 0000000b
    if(iicSt & 0x1){
	//PDEBUG("Ack isn't received\n");
	;
	}                   //When ACK isn't received

    switch(_iicMode)
    {
        case POLLACK:
		//PDEBUG("poll--ACK\n");
            _iicStatus = iicSt;
            break;

	 case RDDATA:
              //PDEBUG("read--ACK\n");
            if((_iicDataCount--)==0)
            {
                //_iicData[_iicPt++] = rIICDS;
                _iicData[_iicPt++] = (*(volatile unsigned long *)0x5400000c); 
            
                //rIICSTAT = 0x90;                //Stop MasRx condition 
                (*(volatile unsigned long *)0x54000004) = 0x80;
				
                //rIICCON  = 0xaf;                //Resumes IIC operation.
                (*(volatile unsigned long *)0x54000000) = 0xaf;
                for(i = 0; i<100; i++);                       //Wait until stop condtion is in effect.
                                                //Too long time... 
                                                //The pending bit will not be set after issuing stop condition.
                break;    
            }      
            //_iicData[_iicPt++] = rIICDS;
            _iicData[_iicPt++] =  (*(volatile unsigned long *)0x5400000c); 
                        //The last data has to be read with no ack.
            if((_iicDataCount)==0)
                //rIICCON = 0x2f;                 //Resumes IIC operation with NOACK.  
                 (*(volatile unsigned long *)0x54000000) = 0x2f;
            else 
               // rIICCON = 0xaf;                 //Resumes IIC operation with ACK
                 (*(volatile unsigned long *)0x54000000) = 0xaf;
            break;
	 
        case WRDATA:
		//PDEBUG("write--ACK\n");
            if((_iicDataCount--)==0)
            {
                //rIICSTAT = 0xd0;                //stop MasTx condition 
                (*(volatile unsigned long *)0x54000004) = 0xd0;
				
                //rIICCON  = 0xaf;                //resumes IIC operation.
                (*(volatile unsigned long *)0x54000000) = 0xaf;
				
                for(i = 0; i<100; i++);                       //wait until stop condtion is in effect.
                       //The pending bit will not be set after issuing stop condition.
                break;    
            }
	    // PDEBUG("poll--->write\n");
            	//rIICDS = _iicData[_iicPt++];        
            (*(volatile unsigned long *)0x5400000c) = _iicData[_iicPt++];  
			
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            
            //rIICCON = 0xaf;                     //resumes IIC operation.
             (*(volatile unsigned long *)0x54000000) = 0xaf;
            break;
			
        case SETRDADDR:
		//PDEBUG("SETRDADDR\n");
            if((_iicDataCount--)==0)
            {
                break;                  //IIC operation is stopped because of IICCON[4]    
            }
           // rIICDS = _iicData[_iicPt++];
            (*(volatile unsigned long *)0x5400000c) = _iicData[_iicPt++];  
            for(i=0;i<10;i++);          //for setup time until rising edge of IICSCL
            //rIICCON = 0xaf;             //resumes IIC operation.
            (*(volatile unsigned long *)0x54000000) = 0xaf;
            break;
			
        default:
            break;      
    }
}

//*********************[ Init ]*********************************
void Init_Iic()
{
	
    //rGPEUP  |= 0xc000;                  //Pull-up disable
     (*(volatile unsigned long *)0x56000048) |= 0xc000; 
	
    //rGPECON |= 0xa00000;                //GPE15:IICSDA , GPE14:IICSCL    
      (*(volatile unsigned long *)0x56000040) |= 0xa00000;
    
    //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
    //rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);
     (*(volatile unsigned long *)0x54000000) = (1<<7) | (0<<6) | (1<<5) | (0xf);

    //rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)
    (*(volatile unsigned long *)0x54000004) = 0x10;
}

//*********************[ Test_Iic ]*********************************
int Disable_WatchDog(void)
{
    unsigned int save_E,save_PE;
    static U8 rdata;
    static U8 wdata;
    static U32 i;
	

    Init_Iic();
   
     //printf("--------->1\n"); 
    wdata = 0x18;
    i=0x0010;	
    _Wr2410Iic(0xde,0x003f,0x02);
    _Wr2410Iic(0xde,0x003f,0x06);
    _Wr2410Iic(0xde,i,wdata);	
    _Wr2410Iic(0xde,0x003f,0x0);
    printf("Write %02x to Watchdog ",wdata);
 

    //printf("--------->2\n");
    _Rd2410Iic(0xdf,i,&(rdata)); 
    printf("and it is %02x now\n",rdata);
     

    //printf("--------->3\n");

    return 0;
}
//////////////////////////////////////////////////////////////////////////

/***************************************************************************
 * Watch for 'delay' seconds for autoboot stop or autoboot delay string.
 * returns: 0 -  no key string, allow autoboot
 *          1 - got key string, abort
 */
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
# if defined(CONFIG_AUTOBOOT_KEYED)
static __inline__ int abortboot(int bootdelay)
{
	int abort = 0;
	uint64_t etime = endtick(bootdelay);
	struct
	{
		char* str;
		u_int len;
		int retry;
	}
	delaykey [] =
	{
		{ str: getenv ("bootdelaykey"),  retry: 1 },
		{ str: getenv ("bootdelaykey2"), retry: 1 },
		{ str: getenv ("bootstopkey"),   retry: 0 },
		{ str: getenv ("bootstopkey2"),  retry: 0 },
	};
	
	char presskey [MAX_DELAY_STOP_STR];
	u_int presskey_len = 0;
	u_int presskey_max = 0;
	u_int i;

#  ifdef CONFIG_AUTOBOOT_PROMPT
	printf (CONFIG_AUTOBOOT_PROMPT, bootdelay);
#  endif

#  ifdef CONFIG_AUTOBOOT_DELAY_STR
	if (delaykey[0].str == NULL)
		delaykey[0].str = CONFIG_AUTOBOOT_DELAY_STR;
#  endif
#  ifdef CONFIG_AUTOBOOT_DELAY_STR2
	if (delaykey[1].str == NULL)
		delaykey[1].str = CONFIG_AUTOBOOT_DELAY_STR2;
#  endif
#  ifdef CONFIG_AUTOBOOT_STOP_STR
	if (delaykey[2].str == NULL)
		delaykey[2].str = CONFIG_AUTOBOOT_STOP_STR;
#  endif
#  ifdef CONFIG_AUTOBOOT_STOP_STR2
	if (delaykey[3].str == NULL)
		delaykey[3].str = CONFIG_AUTOBOOT_STOP_STR2;
#  endif

	for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i ++) {
		delaykey[i].len = delaykey[i].str == NULL ?
				    0 : strlen (delaykey[i].str);
		delaykey[i].len = delaykey[i].len > MAX_DELAY_STOP_STR ?
				    MAX_DELAY_STOP_STR : delaykey[i].len;

		presskey_max = presskey_max > delaykey[i].len ?
				    presskey_max : delaykey[i].len;

#  if DEBUG_BOOTKEYS
		printf("%s key:<%s>\n",
		       delaykey[i].retry ? "delay" : "stop",
		       delaykey[i].str ? delaykey[i].str : "NULL");
#  endif
	}

	/* In order to keep up with incoming data, check timeout only
	 * when catch up.
	 */
	while (!abort && get_ticks() <= etime) {
		for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i ++) {
			if (delaykey[i].len > 0 &&
			    presskey_len >= delaykey[i].len &&
			    memcmp (presskey + presskey_len - delaykey[i].len,
		        	    delaykey[i].str,
				    delaykey[i].len) == 0) {
#  if DEBUG_BOOTKEYS
				printf("got %skey\n",
				       delaykey[i].retry ? "delay" : "stop");
#  endif

#  ifdef CONFIG_BOOT_RETRY_TIME
				/* don't retry auto boot */
				if (! delaykey[i].retry)
					retry_time = -1;
#  endif
				abort = 1;
			}
		}

		if (tstc()) {
			if (presskey_len < presskey_max) {
				presskey [presskey_len ++] = getc();
			}
			else {
				for (i = 0; i < presskey_max - 1; i ++)
					presskey [i] = presskey [i + 1];
				
				presskey [i] = getc();
			}
		}
	}
#  if DEBUG_BOOTKEYS
	if (!abort)
		printf("key timeout\n");
#  endif

	return abort;
}

# else	/* !defined(CONFIG_AUTOBOOT_KEYED) */

static __inline__ int abortboot(int bootdelay)
{
	int abort = 0;

	printf("Hit any key to stop autoboot: %2d ", bootdelay);

#if defined CONFIG_ZERO_BOOTDELAY_CHECK
        /*
         * Check if key already pressed
         * Don't check if bootdelay < 0
         */
	if (bootdelay >= 0) {
		if (tstc()) {	/* we got a key press	*/
			(void) getc();  /* consume input	*/
			printf ("\b\b\b 0\n");
			return 1; 	/* don't auto boot	*/
		}
        }
#endif

	while (bootdelay > 0) {
		int i;

		--bootdelay;
		/* delay 100 * 10ms */
		for (i=0; !abort && i<100; ++i) {
			if (tstc()) {	/* we got a key press	*/
				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				(void) getc();  /* consume input	*/
				break;
			}
			udelay (10000);
		}

		printf ("\b\b\b%2d ", bootdelay);
	}

	putc ('\n');

	return abort;
}
# endif	/* CONFIG_AUTOBOOT_KEYED */
#endif	/* CONFIG_BOOTDELAY >= 0  */

/****************************************************************************/

//HHTECH main.c for ppcboot
void main_loop (void)
{


#ifndef CFG_HUSH_PARSER
	static char lastcommand[CFG_CBSIZE] = { 0, };
	int len;
	int rc = 1;
	int flag;
#endif

#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	char *s;
	int bootdelay;
#endif
#ifdef CONFIG_PREBOOT
	char *p;
#endif

#if defined(CONFIG_VFD) && defined(VFD_TEST_LOGO)
	ulong bmp = 0;		/* default bitmap */
	extern int trab_vfd (ulong bitmap);

#ifdef CONFIG_MODEM_SUPPORT
	if (do_mdm_init)
		bmp = 1;	/* alternate bitmap */
#endif
	trab_vfd (bmp);
#endif	/* CONFIG_VFD && VFD_TEST_LOGO */

#ifdef CONFIG_MODEM_SUPPORT
	debug ("DEBUG: main_loop:   do_mdm_init=%d\n", do_mdm_init);
	if (do_mdm_init) {
		uchar *str = strdup(getenv("mdm_cmd"));
		setenv ("preboot", str);  /* set or delete definition */
		if (str != NULL)
			free (str);
		mdm_init(); /* wait for modem connection */
	}
#endif  /* CONFIG_MODEM_SUPPORT */

#ifdef CFG_HUSH_PARSER
	ppcboot_hush_start ();
#endif

#ifdef CONFIG_PREBOOT
	if ((p = getenv ("preboot")) != NULL) {
# ifdef CONFIG_AUTOBOOT_KEYED
		int prev = disable_ctrlc(1);	/* disable Control C checking */
# endif

# ifndef CFG_HUSH_PARSER
		run_command (p, 0);
# else
		parse_string_outer(p, FLAG_PARSE_SEMICOLON |
				    FLAG_EXIT_FROM_LOOP);
# endif

# ifdef CONFIG_AUTOBOOT_KEYED
		disable_ctrlc(prev);	/* restore Control C checking */
# endif
	}
#endif /* CONFIG_PREBOOT */

#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	s = getenv ("bootdelay");
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

#if 0
	printf ("### main_loop entered:\n\n");
#endif

# ifdef CONFIG_BOOT_RETRY_TIME
	s = getenv ("bootretry");
	if (s != NULL)
		retry_time = (int)simple_strtoul(s, NULL, 10);
	else
		retry_time =  CONFIG_BOOT_RETRY_TIME;
	if (retry_time >= 0 && retry_time < CONFIG_BOOT_RETRY_MIN)
		retry_time = CONFIG_BOOT_RETRY_MIN;
# endif	/* CONFIG_BOOT_RETRY_TIME */

	s = getenv ("bootcmd");
	if (bootdelay >= 0 && s && !abortboot (bootdelay)) {
# ifdef CONFIG_AUTOBOOT_KEYED
		int prev = disable_ctrlc(1);	/* disable Control C checking */
# endif

# ifndef CFG_HUSH_PARSER
		run_command (s, 0);
# else
		parse_string_outer(s, FLAG_PARSE_SEMICOLON |
				    FLAG_EXIT_FROM_LOOP);
# endif

# ifdef CONFIG_AUTOBOOT_KEYED
		disable_ctrlc(prev);	/* restore Control C checking */
# endif
	}
#endif	/* CONFIG_BOOTDELAY */

	/*
	 * Main Loop for Monitor Command Processing
	 */
#ifdef CFG_HUSH_PARSER
	parse_file_outer();
	/* This point is never reached */
	for (;;);
#else
///////////////////////////////////////////////////////add by rzhuo hhcn
Disable_WatchDog();
/////////////////////////////////////////////////////////////////
/* add by HHTECH for auto_boot kernel */
#ifdef CONFIG_START_LINUX
	{
		char c = 'y';
		unsigned long timedata;
		printf("start linux now(y/n):");
		timedata = 0;
		for (;;) {
			while (!tstc()) {	/* while no incoming data */
				if (timedata++ > 3000 * 100 *3)
					goto bootm;	/* timed out */
			}
		WATCHDOG_RESET();	/* Trigger watchdog, if needed */
		c = getc();
		}

bootm:
		if(c == 'y'||c == 'Y'){
			strcpy(lastcommand , "bootm 30008000 30800000\r");
			flag = 0;
			rc = run_command (lastcommand, flag);
			if (rc <= 0) {
				/* invalid command or not repeatable, forget it */
				lastcommand[0] = 0;
			}
		}
		else{
			printf("\n\n");
		}
	}	
#endif


	for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
		if (rc >= 0) {
			/* Saw enough of a valid command to
			 * restart the timeout.
			 */
			reset_cmd_timeout();
		}
#endif
		len = readline (CFG_PROMPT);

		flag = 0;	/* assume no special flags for now */
		if (len > 0)
			strcpy (lastcommand, console_buffer);
		else if (len == 0)
			flag |= CMD_FLAG_REPEAT;
#ifdef CONFIG_BOOT_RETRY_TIME
		else if (len == -2) {
			/* -2 means timed out, retry autoboot
			 */
			printf("\nTimed out waiting for command\n");
# ifdef CONFIG_RESET_TO_RETRY
			/* Reinit board to run initialization code again */
			do_reset (NULL, 0, 0, NULL);
# else
			return;		/* retry autoboot */
# endif
		}
#endif

		if (len == -1)
			printf ("<INTERRUPT>\n");
		else
			rc = run_command (lastcommand, flag);

		if (rc <= 0) {
			/* invalid command or not repeatable, forget it */
			lastcommand[0] = 0;
		}
	}
#endif /*CFG_HUSH_PARSER*/
}

/***************************************************************************
 * reset command line timeout to retry_time seconds
 */
#ifdef CONFIG_BOOT_RETRY_TIME
void reset_cmd_timeout(void)
{
	endtime = endtick(retry_time);
}
#endif

/****************************************************************************/

/*
 * Prompt for input and read a line.
 * If  CONFIG_BOOT_RETRY_TIME is defined and retry_time >= 0,
 * time out when time goes past endtime (timebase time in ticks).
 * Return:	number of read characters
 *		-1 if break
 *		-2 if timed out
 */
int readline (const char *const prompt)
{
	char   *p = console_buffer;
	int	n = 0;				/* buffer index		*/
	int	plen = 0;			/* prompt length	*/
	int	col;				/* output column cnt	*/
	char	c;

	/* print prompt */
	if (prompt) {
		plen = strlen (prompt);
		puts (prompt);
	}
	col = plen;

	for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
		while (!tstc()) {	/* while no incoming data */
			if (retry_time >= 0 && get_ticks() > endtime)
				return (-2);	/* timed out */
		}
#endif
		WATCHDOG_RESET();		/* Trigger watchdog, if needed */

#ifdef CONFIG_SHOW_ACTIVITY
		while (!tstc()) {
			extern void show_activity(int arg);
			show_activity(0);
		}
#endif		
		c = getc();

		/*
		 * Special character handling
		 */
		switch (c) {
		case '\r':				/* Enter		*/
		case '\n':
			*p = '\0';
			puts ("\r\n");
			return (p - console_buffer);

		case 0x03:				/* ^C - break		*/
			console_buffer[0] = '\0';	/* discard input */
			return (-1);

		case 0x15:				/* ^U - erase line	*/
			while (col > plen) {
				puts (erase_seq);
				--col;
			}
			p = console_buffer;
			n = 0;
			continue;

		case 0x17:				/* ^W - erase word 	*/
			p=delete_char(console_buffer, p, &col, &n, plen);
			while ((n > 0) && (*p != ' ')) {
				p=delete_char(console_buffer, p, &col, &n, plen);
			}
			continue;

		case 0x08:				/* ^H  - backspace	*/
		case 0x7F:				/* DEL - backspace	*/
			p=delete_char(console_buffer, p, &col, &n, plen);
			continue;

		default:
			/*
			 * Must be a normal character then
			 */
			if (n < CFG_CBSIZE-2) {
				if (c == '\t') {	/* expand TABs		*/
					puts (tab_seq+(col&07));
					col += 8 - (col&07);
				} else {
					++col;		/* echo input		*/
					putc (c);
				}
				*p++ = c;
				++n;
			} else {			/* Buffer full		*/
				putc ('\a');
			}
		}
	}
}

/****************************************************************************/

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
	char *s;

	if (*np == 0) {
		return (p);
	}

	if (*(--p) == '\t') {			/* will retype the whole line	*/
		while (*colp > plen) {
			puts (erase_seq);
			(*colp)--;
		}
		for (s=buffer; s<p; ++s) {
			if (*s == '\t') {
				puts (tab_seq+((*colp) & 07));
				*colp += 8 - ((*colp) & 07);
			} else {
				++(*colp);
				putc (*s);
			}
		}
	} else {
		puts (erase_seq);
		(*colp)--;
	}
	(*np)--;
	return (p);
}

/****************************************************************************/

int parse_line (char *line, char *argv[])
{
	int nargs = 0;

#ifdef DEBUG_PARSER
	printf ("parse_line: \"%s\"\n", line);
#endif
	while (nargs < CFG_MAXARGS) {

		/* skip any white space */
		while ((*line == ' ') || (*line == '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		printf ("parse_line: nargs=%d\n", nargs);
#endif
			return (nargs);
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && (*line != ' ') && (*line != '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		printf ("parse_line: nargs=%d\n", nargs);
#endif
			return (nargs);
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	printf ("** Too many args (max. %d) **\n", CFG_MAXARGS);

#ifdef DEBUG_PARSER
	printf ("parse_line: nargs=%d\n", nargs);
#endif
	return (nargs);
}

/****************************************************************************/

static void process_macros (const char *input, char *output)
{
	char c, prev;
	const char *varname_start = NULL;
	int inputcnt  = strlen (input);
	int outputcnt = CFG_CBSIZE;
	int state = 0;	/* 0 = waiting for '$'	*/
			/* 1 = waiting for '('	*/
			/* 2 = waiting for ')'	*/

#ifdef DEBUG_PARSER
	char *output_start = output;

	printf ("[PROCESS_MACROS] INPUT len %d: \"%s\"\n", strlen(input), input);
#endif

	prev = '\0';			/* previous character	*/

	while (inputcnt && outputcnt) {
	    c = *input++;
	    inputcnt--;

	    /* remove one level of escape characters */
	    if ((c == '\\') && (prev != '\\')) {
		if (inputcnt-- == 0)
			break;
		prev = c;
	    	c = *input++;
	    }

	    switch (state) {
	    case 0:			/* Waiting for (unescaped) $	*/
		if ((c == '$') && (prev != '\\')) {
			state++;
		} else {
			*(output++) = c;
			outputcnt--;
		}
		break;
	    case 1:			/* Waiting for (	*/
		if (c == '(') {
			state++;
			varname_start = input;
		} else {
			state = 0;
			*(output++) = '$';
			outputcnt--;

			if (outputcnt) {
				*(output++) = c;
				outputcnt--;
			}
		}
		break;
	    case 2:			/* Waiting for )	*/
		if (c == ')') {
			int i;
			char envname[CFG_CBSIZE], *envval;
			int envcnt = input-varname_start-1; /* Varname # of chars */

			/* Get the varname */
			for (i = 0; i < envcnt; i++) {
				envname[i] = varname_start[i];
			}
			envname[i] = 0;

			/* Get its value */
			envval = getenv (envname);

			/* Copy into the line if it exists */
			if (envval != NULL)
				while ((*envval) && outputcnt) {
					*(output++) = *(envval++);
					outputcnt--;
				}
			/* Look for another '$' */
			state = 0;
		}
		break;
	    }

	    prev = c;
	}

	if (outputcnt)
		*output = 0;

#ifdef DEBUG_PARSER
	printf ("[PROCESS_MACROS] OUTPUT len %d: \"%s\"\n",
		strlen(output_start), output_start);
#endif
}

/****************************************************************************
 * returns:
 *	1  - command executed, repeatable
 *	0  - command executed but not repeatable, interrupted commands are
 *	     always considered not repeatable
 *	-1 - not executed (unrecognized, bootd recursion or too many args)
 *           (If cmd is NULL or "" or longer than CFG_CBSIZE-1 it is
 *           considered unrecognized)
 *
 * WARNING:
 *
 * We must create a temporary copy of the command since the command we get
 * may be the result from getenv(), which returns a pointer directly to
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */

int run_command (const char *cmd, int flag)
{
	cmd_tbl_t *cmdtp;
	char cmdbuf[CFG_CBSIZE];	/* working copy of cmd		*/
	char *token;			/* start of token in cmdbuf	*/
	char *sep;			/* end of token (separator) in cmdbuf */
	char finaltoken[CFG_CBSIZE];
	char *str = cmdbuf;
	char *argv[CFG_MAXARGS + 1];	/* NULL terminated	*/
	int argc;
	int repeatable = 1;

#ifdef DEBUG_PARSER
	printf ("[RUN_COMMAND] cmd[%p]=\"", cmd);
	puts (cmd ? cmd : "NULL");	/* use puts - string may be loooong */
	puts ("\"\n");
#endif

	clear_ctrlc();		/* forget any previous Control C */

	if (!cmd || !*cmd) {
		return -1;	/* empty command */
	}

	if (strlen(cmd) >= CFG_CBSIZE) {
		puts ("## Command too long!\n");
		return -1;
	}

	strcpy (cmdbuf, cmd);

	/* Process separators and check for invalid
	 * repeatable commands
	 */

#ifdef DEBUG_PARSER
	printf ("[PROCESS_SEPARATORS] %s\n", cmd);
#endif
	while (*str) {

		/*
		 * Find separator, or string end
		 * Allow simple escape of ';' by writing "\;"
		 */
		for (sep = str; *sep; sep++) {
			if ((*sep == ';') &&	/* separator		*/
			    ( sep != str) &&	/* past string start	*/
			    (*(sep-1) != '\\'))	/* and NOT escaped	*/
				break;
		}

		/*
		 * Limit the token to data between separators
		 */
		token = str;
		if (*sep) {
			str = sep + 1;	/* start of command for next pass */
			*sep = '\0';
		}
		else
			str = sep;	/* no more commands for next pass */
#ifdef DEBUG_PARSER
		printf ("token: \"%s\"\n", token);
#endif

		/* find macros in this token and replace them */
		process_macros (token, finaltoken);

		/* Extract arguments */
		argc = parse_line (finaltoken, argv);

		/* Look up command in command table */
		if ((cmdtp = find_cmd(argv[0])) == NULL) {
			printf ("Unknown command '%s' - try 'help'\n", argv[0]);
			return -1;	/* give up after bad command */
		}

		/* found - check max args */
		if (argc > cmdtp->maxargs) {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return -1;
		}

#if (CONFIG_COMMANDS & CFG_CMD_BOOTD)
		/* avoid "bootd" recursion */
		if (cmdtp->cmd == do_bootd) {
#ifdef DEBUG_PARSER
			printf ("[%s]\n", finaltoken);
#endif
			if (flag & CMD_FLAG_BOOTD) {
				printf ("'bootd' recursion detected\n");
				return -1;
			}
			else
				flag |= CMD_FLAG_BOOTD;
		}
#endif	/* CFG_CMD_BOOTD */

		/* OK - call function to do the command */
		if ((cmdtp->cmd) (cmdtp, flag, argc, argv) != 0) {
			return (-1);
		}

		repeatable &= cmdtp->repeatable;

		/* Did the user stop this? */
		if (had_ctrlc ())
			return 0;	/* if stopped then not repeatable */
	}

	return repeatable;
}

/****************************************************************************/

#if (CONFIG_COMMANDS & CFG_CMD_RUN)
int do_run (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i;
	int rcode = 1;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	for (i=1; i<argc; ++i) {
#ifndef CFG_HUSH_PARSER
	    if (run_command (getenv (argv[i]), flag) != -1) ++rcode;
#else
   	    if (parse_string_outer(getenv (argv[i]),
		    FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) == 0) ++rcode;
#endif
	}
	return ((rcode == i) ? 0 : 1);
}
#endif
