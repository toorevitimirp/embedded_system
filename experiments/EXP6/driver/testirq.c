 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>
#include <asm/io.h>


void testirq_interrupt(int,void *,struct pt_regs *);
static int testirq_init(void);

/**********************************************************/
//Interrupt Function

void testirq_interrupt(int irq,void *d,struct pt_regs *regs)
{
	/*clear interrupt register for EINT5*/
        SRCPND &= (~0x00000010);    //bit4
        INTPND = INTPND;
        EINTPEND &= (~0x00000020);  //bit5

        printk("Entered an interrupt! Beginning interrupt service!\n");
       
}


/****************************************************/
//Initialize Function
static int __init testirq_init(void)
{	
	static int result;
	unsigned long gpfup;
//	long pullup;

        set_external_irq(IRQ_EINT5, EXT_FALLING_EDGE, GPIO_PULLUP_DIS);
	
	gpfup = ioremap(0x56000058,4); //pull_up for port f
	(*(volatile unsigned long *)gpfup) = 0;
	
        disable_irq(IRQ_EINT5);
        enable_irq(IRQ_EINT5);

	result=request_irq(IRQ_EINT5,&testirq_interrupt,SA_INTERRUPT,"testirq",NULL);
	if (result)
	{
		printk("Can't get assigned irq %d,result=%d\n",IRQ_EINT5,result);
		return result;
	}


	printk("Testirq interrupt registered ok!!!\n");
	
        return 0;
}

static void __exit testirq_exit(void){
        disable_irq(IRQ_EINT5);
	free_irq(IRQ_EINT5, NULL);
        printk("exit ok\n");
}

module_init(testirq_init);
module_exit(testirq_exit);
