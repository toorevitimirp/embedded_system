/*
 * linux/drivers/char/keyb_ps2.c
 *
 * Written for uClinux based on HHCF5272-LCD-IDE-R1 by HHTECH.
 * This program is written to support PS/2 keyboard.
 * It is different from pc keyboard system in several sides.
 * You can compare it with linux/drivers/char/pc_keyb.c if you want to 
 * konw what's difference between them.
 *
 */ 


#include <linux/config.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/kbd_ll.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/kbd_kern.h>
#include <linux/vt_kern.h>
#include <linux/smp_lock.h>
#include <linux/kd.h>
#include <linux/pm.h>

#include <linux/keyboard.h>

#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/system.h>

#include <asm/io.h>

#include <linux/sched.h>
#include <linux/kernel.h>

//HHTECH now use EINT4
//#define KEYBOARD_IRQ 3//37//67 
#define KEYBOARD_IRQ 4//37//67 
#define MBAR2_ADDR 0x80000000

/*
 * Translation of escaped scancodes to keycodes.
 * This is now user-settable.
 * The keycodes 1-88,96-111,119 are fairly standard, and
 * should probably not be changed - changing might confuse X.
 * X also interprets scancode 0x5d (KEY_Begin).
 *
 * For 1-88 keycode equals scancode.
 */

#define E0_KPENTER 96
#define E0_RCTRL   97
#define E0_KPSLASH 98
#define E0_PRSCR   99
#define E0_RALT    100
#define E0_BREAK   101  /* (control-pause) */
#define E0_HOME    102
#define E0_UP      103
#define E0_PGUP    104
#define E0_LEFT    105
#define E0_RIGHT   106
#define E0_END     107
#define E0_DOWN    108
#define E0_PGDN    109
#define E0_INS     110
#define E0_DEL     111

#define E1_PAUSE   119

/*
 * The keycodes below are randomly located in 89-95,112-118,120-127.
 * They could be thrown away (and all occurrences below replaced by 0),
 * but that would force many users to use the `setkeycodes' utility, where
 * they needed not before. It does not matter that there are duplicates, as
 * long as no duplication occurs for any single keyboard.
 */
#define SC_LIM 89

#define FOCUS_PF1 85           /* actual code! */
#define FOCUS_PF2 89
#define FOCUS_PF3 90
#define FOCUS_PF4 91
#define FOCUS_PF5 92
#define FOCUS_PF6 93
#define FOCUS_PF7 94
#define FOCUS_PF8 95
#define FOCUS_PF9 120
#define FOCUS_PF10 121
#define FOCUS_PF11 122
#define FOCUS_PF12 123

#define JAP_86     124
/* tfj@olivia.ping.dk:
 * The four keys are located over the numeric keypad, and are
 * labelled A1-A4. It's an rc930 keyboard, from
 * Regnecentralen/RC International, Now ICL.
 * Scancodes: 59, 5a, 5b, 5c.
 */
#define RGN1 124
#define RGN2 125
#define RGN3 126
#define RGN4 127

static unsigned char high_keys[128 - SC_LIM] = {
  RGN1, RGN2, RGN3, RGN4, 0, 0, 0,                   /* 0x59-0x5f */
  0, 0, 0, 0, 0, 0, 0, 0,                            /* 0x60-0x67 */
  0, 0, 0, 0, 0, FOCUS_PF11, 0, FOCUS_PF12,          /* 0x68-0x6f */
  0, 0, 0, FOCUS_PF2, FOCUS_PF9, 0, 0, FOCUS_PF3,    /* 0x70-0x77 */
  FOCUS_PF4, FOCUS_PF5, FOCUS_PF6, FOCUS_PF7,        /* 0x78-0x7b */
  FOCUS_PF8, JAP_86, FOCUS_PF10, 0                   /* 0x7c-0x7f */
};

/* BTC */
#define E0_MACRO   112
/* LK450 */
#define E0_F13     113
#define E0_F14     114
#define E0_HELP    115
#define E0_DO      116
#define E0_F17     117
#define E0_KPMINPLUS 118
/*
 * My OmniKey generates e0 4c for  the "OMNI" key and the
 * right alt key does nada. [kkoller@nyx10.cs.du.edu]
 */
#define E0_OK	124
/*
 * New microsoft keyboard is rumoured to have
 * e0 5b (left window button), e0 5c (right window button),
 * e0 5d (menu button). [or: LBANNER, RBANNER, RMENU]
 * [or: Windows_L, Windows_R, TaskMan]
 */
#define E0_MSLW	125
#define E0_MSRW	126
#define E0_MSTM	127

static unsigned char e0_keys[128] = {
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x00-0x07 */
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x08-0x0f */
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x10-0x17 */
  0, 0, 0, 0, E0_KPENTER, E0_RCTRL, 0, 0,	      /* 0x18-0x1f */
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x20-0x27 */
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x28-0x2f */
  0, 0, 0, 0, 0, E0_KPSLASH, 0, E0_PRSCR,	      /* 0x30-0x37 */
  E0_RALT, 0, 0, 0, 0, E0_F13, E0_F14, E0_HELP,	      /* 0x38-0x3f */
  E0_DO, E0_F17, 0, 0, 0, 0, E0_BREAK, E0_HOME,	      /* 0x40-0x47 */
  E0_UP, E0_PGUP, 0, E0_LEFT, E0_OK, E0_RIGHT, E0_KPMINPLUS, E0_END,/* 0x48-0x4f */
  E0_DOWN, E0_PGDN, E0_INS, E0_DEL, 0, 0, 0, 0,	      /* 0x50-0x57 */
  0, 0, 0, E0_MSLW, E0_MSRW, E0_MSTM, 0, 0,	      /* 0x58-0x5f */
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x60-0x67 */
  0, 0, 0, 0, 0, 0, 0, E0_MACRO,		      /* 0x68-0x6f */
  0, 0, 0, 0, 0, 0, 0, 0,			      /* 0x70-0x77 */
  0, 0, 0, 0, 0, 0, 0, 0			      /* 0x78-0x7f */
};


static int down_to_up=0;
static int keycount=0;
static int interrupts=0;


static void keyboard_interrupt(int irq,void *dev_id,struct pt_regs * regs)
{
#if 1
	unsigned char scancode = 0,scancode1 = 0;
	int i;
	unsigned long vaddr;
#endif	
#if 0
	interrupts=(interrupts+1)%3;
	if(interrupts==0)
		scancode=0x1c;
	if(interrupts==1)
		scancode=0xf0;
	if(interrupts==2)
		scancode=0x1c;
#endif	
#if 0
	keycount++;
//	if(keycount%300==0)
		printk("#");
	return;
#endif
//	printk("#####keyboard interrupt\n");
#if 1
//        *(volatile unsigned long *)(0x80000000+0x0c0)=0x00000020;//yehuang

//HHTECH now use nGCS4
//	vaddr=ioremap(/*0x48000010*/0x18000000,2);

        SRCPND &= (~0x00000010);    //bit4
        INTPND = INTPND;
        EINTPEND &= (~0x00000010);  //clear interrupt

	vaddr=ioremap(/*0x48000010*/0x20000000,2);
	scancode = *(volatile unsigned char*)vaddr/*(0x30000000)*/;
	iounmap(vaddr);
	/*
	 * the procedure below is to convert keyboard 
	 * scancode to system scancode
 	 *
	 */
//	printk("scancode = %x\n",scancode);
	switch(scancode){
		case(0xE0):	scancode = 0xE0; /*e0_state = 1;*/ break;
		case(0xE1):	scancode = 0xE1; /*e1_state = 1;*/ break;
		case(0x1C):	scancode = 0x1E; break;/*A*/
		case(0x32):	scancode = 0x30; break;/*B*/
		case(0x21):	scancode = 0x2E; break;/*B*/
		case(0x23):	scancode = 0x20; break;/*D*/
		case(0x24):     scancode = 0x12; break;/*E*/
		case(0x2B):   	scancode = 0x21; break;/*F*/
		case(0x34): 	scancode = 0x22; break;/*G*/
		case(0x33):     scancode = 0x23; break;/*H*/
		case(0x43):	scancode = 0x17; break;/*I*/
		case(0x3B):     scancode = 0x24; break;/*J*/
		case(0x42): 	scancode = 0x25; break;/*K*/
		case(0x4B):	scancode = 0x26; break;/*L*/
		case(0x3A):	scancode = 0x32; break;/*M*/
		case(0x31):	scancode = 0x31; break;/*N*/
		case(0x44):	scancode = 0x18; break;/*O*/
		case(0x4D):	scancode = 0x19; break;/*P*/
		case(0x15):	scancode = 0x10; break;/*Q*/
		case(0x2D):	scancode = 0x13; break;/*R*/
		case(0x1B):	scancode = 0x1F; break;/*S*/
		case(0x2C):	scancode = 0x14; break;/*T*/
		case(0x3C):	scancode = 0x16; break;/*U*/
		case(0x2A):	scancode = 0x2F; break;/*V*/
		case(0x1D):	scancode = 0x11; break;/*W*/
		case(0x22):	scancode = 0x2D; break;/*X*/
		case(0x35):	scancode = 0x15; break;/*Y*/
		case(0x1A):	scancode = 0x2C; break;/*Z*/
		
		case(0x45):	scancode = 0x0B; break;/*0*/
		case(0x16):	scancode = 0x02; break;/*1*/
		case(0x1E):	scancode = 0x03; break;/*2*/
		case(0x26):	scancode = 0x04; break;/*3*/
		case(0x25):	scancode = 0x05; break;/*4*/
		case(0x2E):	scancode = 0x06; break;/*5*/
		case(0x36):	scancode = 0x07; break;/*6*/
		case(0x3D):	scancode = 0x08; break;/*7*/
		case(0x3E):	scancode = 0x09; break;/*8*/
		case(0x46):	scancode = 0x0A; break;/*9*/
		case(0x0E):	scancode = 0x29; break;/*`*/
		case(0x4E):	scancode = 0x0C; break;/*-*/
		case(0x55):	scancode = 0x0D; break;/*=*/
		case(0x5D):	scancode = 0x2B; break;/*\*/
		case(0x66):	scancode = 0x0E; break;/*BKSP*/
		case(0x29):	scancode = 0x39; break;/*SPACE*/
		case(0x0D):	scancode = 0x0F; break;/*TAB*/
		case(0x58):	scancode = 0x3A; break;/*CAPS*/

		case(0x12):	scancode = 0x2A; break;/*L SHIFT*/
		case(0x14):	scancode = 0x1D; break;/*L CTRL*/
		case(0x1F):	scancode = 0x5B; break;/*L GUI*/
		case(0x11):	scancode = 0x38; break;/*L ALT*/
		case(0x59):	scancode = 0x36; break;/*R SHIFT*/
		/*R CTRL's scancode equal to L CRTL*/		
	        case(0x27):	scancode = 0x5C; break;/*R GUI*/
		/*R ALT's scancode equal to L ALT*/		
		case(0x2F):	scancode = 0x5D; break;/*APPS*/
		case(0x5A):	scancode = 0x1C; break;/*ENTER*/
		case(0x76):	scancode = 0x01; break;/*ESC*/
		case(0x05):	scancode = 0x3B; break;/*F1*/
		case(0x06):	scancode = 0x3C; break;/*F2*/
		case(0x04):	scancode = 0x3D; break;/*F3*/
		case(0x0C):	scancode = 0x3E; break;/*F4*/
		case(0x03):	scancode = 0x3F; break;/*F5*/
		case(0x0B):	scancode = 0x40; break;/*F6*/
		case(0x83):	scancode = 0x41; break;/*F7*/
		case(0x0A):	scancode = 0x42; break;/*F8*/
		case(0x01):	scancode = 0x43; break;/*F9*/
		case(0x09):	scancode = 0x44; break;/*F10*/
		case(0x78):	scancode = 0x57; break;/*F11*/
		case(0x07):	scancode = 0x58; break;/*F12*/
		
		
		
	//	case(0x):	scancode = 0x; break;/*PRNT SCRN*/
		case(0x7E):	scancode = 0x46; break;/*SCROLL*/
	//	case(0x):	scancode = 0x1e; break;/*PAUSE*/
		
		case(0x54):	scancode = 0x1A; break;/*[*/
		case(0x70):	scancode = 0x52; break;/*INSERT*/
		case(0x6C):	scancode = 0x47; break;/*HOME*/
		case(0x7D):	scancode = 0x49; break;/*PG UP*/
		case(0x71):	scancode = 0x53; break;/*DELET*/
		case(0x69):	scancode = 0x4F; break;/*END*/
		case(0x7A):	scancode = 0x51; break;/*PG DN*/
		case(0x75):	scancode = 0x48; break;/*U ARROW*/
		case(0x6B):	scancode = 0x4B; break;/*L ARROW*/
		case(0x72):	scancode = 0x50; break;/*D ARROW*/
		case(0x74):	scancode = 0x4D; break;/*R ARROW*/
		case(0x77):	scancode = 0x45; break;/*NUM*/
		/*equal to the next*/		
		case(0x4A):	scancode = 0x35; break;/*KP/ */
		case(0x7C):	scancode = 0x37; break;/*KP * */
		case(0x7B):	scancode = 0x4A; break;/*KP -*/
		case(0x79):	scancode = 0x4E; break;/*KP +*/
		
		
		/*equal to the other*/		
		#if 0
		case(0x70):	scancode = 0x52; break;/*KP 0*/
		case(0x69):	scancode = 0x4F; break;/*KP 1*/
		case(0x72):	scancode = 0x50; break;/*KP 2*/
		case(0x7A):	scancode = 0x51; break;/*KP 3*/
		case(0x6B):	scancode = 0x4B; break;/*KP 4*/
		case(0x73):	scancode = 0x4C; break;/*KP 5*/
		case(0x74):	scancode = 0x4D; break;/*KP 6*/
		case(0x6C):	scancode = 0x47; break;/*KP 7*/
		case(0x75):	scancode = 0x48; break;/*KP 8*/
		case(0x7D):	scancode = 0x49; break;/*KP 9*/
#endif
		case(0x5B):	scancode = 0x1B; break;/*]*/
		case(0x4C):	scancode = 0x27; break;/*;*/
		case(0x52):	scancode = 0x28; break;/*'*/
		case(0x41):	scancode = 0x33; break;/*,*/
		case(0x49):	scancode = 0x34; break;/*.*/
		
	    	/*those three keys haven't been take effect*/	
		case(0x37):	scancode = 0x5E; break;/*POWER*/
		case(0x3F):	scancode = 0x5F; break;/*SLEEP*/
		case(0x5E):	scancode = 0x63; break;/*WAKE*/
		
		
		case(0x50):	scancode = 0x6D; break;/*MEDIA SELECT*/
		case(0x48):	scancode = 0x6C; break;/*E-MAIL*/
		case(0x40):	scancode = 0x6B; break;/*MY COMPUTER*/
		case(0x10):	scancode = 0x65; break;/*WWW SEARCH*/
		case(0x38):	scancode = 0x6A; break;/*WWW BACK*/
		case(0x30):	scancode = 0x69; break;/*WWW FORWARD*/
		case(0x28):	scancode = 0x68; break;/*WWW STOP*/
		case(0x20):	scancode = 0x67; break;/*WWW REFRESH*/
		case(0x18):	scancode = 0x66; break;/*WWW FAVORITES*/


		case(0xF0):	down_to_up = 1;  
		goto goout;
	}
//	printk("%x\n",down_to_up);	
	
	if(down_to_up){
		scancode |=0x80;
                down_to_up = 0 ;
	}
        handle_scancode(scancode,!(scancode & 0x80));   
goout:  //*(volatile unsigned long *)0x10000020 |= 0x00800000;
#endif
}


int kbd_translate(unsigned char scancode, unsigned char *keycode,
		char raw_mode)
{
	static int prev_scancode;
	
	/* special prefix scancodes.. */
	if (scancode == 0xe0 || scancode == 0xe1) {
		prev_scancode = scancode;
		return 0;
	}

	/* 0xFF is sent by a few keyboards, ignore it. 0x00 is error */
	if (scancode == 0x00 || scancode == 0xff) {
		prev_scancode = 0;
		return 0;
	}

	scancode &= 0x7f;

	if (prev_scancode) {
	  /*
	   * usually it will be 0xe0, but a Pause key generates
	   * e1 1d 45 e1 9d c5 when pressed, and nothing when released
	   */
	  if (prev_scancode != 0xe0) {
	      if (prev_scancode == 0xe1 && scancode == 0x1d) {
		  prev_scancode = 0x100;
		  return 0;
	      } else if (prev_scancode == 0x100 && scancode == 0x45) {
		  *keycode = E1_PAUSE;
		  prev_scancode = 0;
	      } else {
		  prev_scancode = 0;
		  return 0;
	      }
	  } else {
	      prev_scancode = 0;
	      /*
	       *  The keyboard maintains its own internal caps lock and
	       *  num lock statuses. In caps lock mode E0 AA precedes make
	       *  code and E0 2A follows break code. In num lock mode,
	       *  E0 2A precedes make code and E0 AA follows break code.
	       *  We do our own book-keeping, so we will just ignore these.
	       */
	      /*
	       *  For my keyboard there is no caps lock mode, but there are
	       *  both Shift-L and Shift-R modes. The former mode generates
	       *  E0 2A / E0 AA pairs, the latter E0 B6 / E0 36 pairs.
	       *  So, we should also ignore the latter. - aeb@cwi.nl
	       */
	      if (scancode == 0x2a || scancode == 0x36)
		return 0;

	      if (e0_keys[scancode])
		*keycode = e0_keys[scancode];
	      else {
		  return 0;
	      }
	  }
	} else if (scancode >= SC_LIM) {
	    /* This happens with the FOCUS 9000 keyboard
	       Its keys PF1..PF12 are reported to generate
	       55 73 77 78 79 7a 7b 7c 74 7e 6d 6f
	       Moreover, unless repeated, they do not generate
	       key-down events, so we have to zero up_flag below */
	    /* Also, Japanese 86/106 keyboards are reported to
	       generate 0x73 and 0x7d for \ - and \ | respectively. */
	    /* Also, some Brazilian keyboard is reported to produce
	       0x73 and 0x7e for \ ? and KP-dot, respectively. */

	  *keycode = high_keys[scancode - SC_LIM];

 	} else
	  *keycode = scancode;
 	return 1;
}


void keyboard_init(void)
{
	int result;
	unsigned long vaddr;
	unsigned long gpfup;
	printk("#####keyboard init\n");
//HHTECH
	set_external_irq(IRQ_EINT4, EXT_RISING_EDGE , GPIO_PULLUP_DIS);
	gpfup = ioremap(0x56000058,4);
	(*(volatile unsigned long *)gpfup) = 0;
	disable_irq(IRQ_EINT4);
	//enable_irq(IRQ_EINT4);
	result=request_irq(IRQ_EINT4,keyboard_interrupt,/*SA_INTERRUPT*/NULL,(const char*)"test_keyboard",NULL);
#if 0
	result=request_irq(KEYBOARD_IRQ,
		    keyboard_interrupt,
		    /*SA_SHIRQ*/NULL,	
		    (const char*)"test_keyboard",
		    NULL);
#endif
	printk("result is %d\n",result);
	if(result){
		printk("error\n");
	}
	else{
		printk("####requesting irq is ok!\n");
	}
#if 1
	vaddr=ioremap(/*0x48000010*/0x56000088,4);
//HHTECH now use EINT4
//	*(volatile unsigned long*)vaddr|=0x00004000;
	*(volatile unsigned long*)vaddr|=0x00040000;
//	printk("%x ",scancode);
	iounmap(vaddr);
#endif
#if 0
	/* yehuang L->H */
	*(volatile unsigned long *)(0x80000000+0x00c) |=0x00000020;
	/* yehuang assign interrupt level */
	*(volatile unsigned long *)(0x80000000+0x150)|=0x00200000;
        *(volatile unsigned long *)(0x80000000+0x0c4) |=0x00000020;
//	*(volatile unsigned long *)(0x80000000+0x0c4) &=0xffffffdf;
#endif
}

//module_init(keyboard_init);
