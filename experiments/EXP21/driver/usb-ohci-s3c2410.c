/*
 *  linux/drivers/usb/usb-ohci-s3c2410.c
 *
 *  OHCI r1.0 compatible, CPU embedded USB host controller
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/pci.h>

#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/io.h>

#define OHCI_HW_DRIVER 1
#include "usb-ohci.h"

/*
 * S3C2410 user's guide 에 있는 1장 짜리 설명에 의하면,
 * OHCI 호환이라고 한다.
 *
 * UPLL 설정과 MISC 레지스터에서 포트 죽이고 살리고 하는 것을 빼먹지 말 것.
 *
 * 그리고, s3c2410 에서 USB slave 를 쓰려면 host 포트 하나를 죽여야 한다는 것
 * 잊지 말 것.
 * 하지만, 이렇게 해도 slave 신호가 root hub 쪽에 주는 영향을 막을 방법이 없다.
 * hub.c 를 고쳐서 root hub 이고, 1 host port 로 config 했을 경우,
 * 두 번째 포트를 아예 쳐다도 보지 않도록 했으니 참고할 것.
 */

int __devinit
hc_add_ohci(struct pci_dev *dev, int irq, void *membase, unsigned long flags,
	    ohci_t **ohci, const char *name, const char *slot_name);
extern void hc_remove_ohci(ohci_t *ohci);

static ohci_t *s3c2410_ohci;

static void __init s3c2410_ohci_configure(void)
{
#if CONFIG_MAX_ROOT_PORTS < 2
	/* 1 host port, 1 slave port*/
	MISCCR &= ~MISCCR_USBPAD;

#if 0 // confused - bushi
	/* wakeup port 0 */
	MISCCR &= ~MISCCR_USB0_SUSPEND;
	/* sleep port 1 */
	MISCCR |= MISCCR_USB1_SUSPEND;
#endif

#else
	/* 2 host port */
	MISCCR |= MISCCR_USBPAD;

#if 0 // confused - bushi
	/* wakeup port 0 */
	MISCCR &= ~MISCCR_USB0_SUSPEND;
	/* wakeup port 1 */
	MISCCR &= ~MISCCR_USB1_SUSPEND;
#endif

#endif

	/* UPLLCON */
	UPLLCON = FInsrt(0x78, fPLL_MDIV) | FInsrt(0x02, fPLL_PDIV) 
		| FInsrt(0x03, fPLL_SDIV);

	/* CLKCON */
	CLKCON |= CLKCON_USBH;

	udelay(11);
}

static int __init s3c2410_ohci_init(void)
{
	int ret;

	s3c2410_ohci_configure();

	/*
	 * Initialise the generic OHCI driver.
	 */
	// FIXME : io_p2v() ?
	ret = hc_add_ohci((struct pci_dev *)1, IRQ_USBH,
			  (void *)(io_p2v(USBHOST_CTL_BASE)), 0, &s3c2410_ohci,
			  "usb-ohci", "s3c2410");

	return ret;
}

static void __exit s3c2410_ohci_exit(void)
{
	hc_remove_ohci(s3c2410_ohci);

	/*
	 * Stop the USB clock.
	 */
	CLKCON &= ~CLKCON_USBH;
}

module_init(s3c2410_ohci_init);
module_exit(s3c2410_ohci_exit);
