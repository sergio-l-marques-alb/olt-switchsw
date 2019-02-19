/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * DESCRIPTION: The Broadcom iProc random number generator RNG200 driver
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/hw_random.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <mach/io_map.h>

/* Registers */
#define RNG_CTRL                                            0x00
#define    RNG_CTRL_RNG_RBGEN                               0x00001FFF
#define RNG_SOFT_RESET                                      0x04
#define    RNG_SOFT_RESET_RNG_SOFT_RESET                    0x00000001
#define RBG_SOFT_RESET                                      0x08
#define    RBG_SOFT_RESET_RNG_SOFT_RESET                    0x00000001
#define RNG_INT_STATUS                                      0x18
#define    RNG_INT_STATUS_TOTAL_BITS_COUNT_IRQ              0x00000001
#define    RNG_INT_STATUS_MASTER_FAIL_LOCKOUT_IRQ           0x80000000
#define RNG_FIFO_DATA                                       0x20
#define    RNG_FIFO_DATA_RNG_FIFO_NUM                       0xFFFFFFFF
#define RNG_FIFO_COUNT                                      0x24
#define    RNG_FIFO_COUNT_RNG_FIFO_COUNT                    0x000000FF

/* RNG200 core */
static void *regbase;
#define REG(x)  (regbase + (x))

/* lock for data access */
static DEFINE_MUTEX(lock);
static atomic_t bus_is_probed;

static void ioclrbit32(unsigned int bits, void *addr)
{
    iowrite32(ioread32(addr) & ~bits, addr);
}

static void iosetbit32(unsigned int bits, void *addr)
{
    iowrite32(ioread32(addr) | bits,  addr);
}

static void rng200_enable(void)
{
    iosetbit32(RNG_CTRL_RNG_RBGEN, REG(RNG_CTRL));
}

static void rng200_disable(void)
{
    ioclrbit32(RNG_CTRL_RNG_RBGEN, REG(RNG_CTRL));
}

static void rng200_restart(void)
{
    /* Disable it first (if enabled) */
    rng200_disable();

    /* Clear all interrupt status */
    iowrite32(0xFFFFFFFFUL, REG(RNG_INT_STATUS));

    /* Reset RNG */
    iosetbit32(RBG_SOFT_RESET_RNG_SOFT_RESET, REG(RBG_SOFT_RESET));
    iosetbit32(RNG_SOFT_RESET_RNG_SOFT_RESET, REG(RNG_SOFT_RESET));
    udelay(10);
    ioclrbit32(RNG_SOFT_RESET_RNG_SOFT_RESET, REG(RNG_SOFT_RESET));
    ioclrbit32(RBG_SOFT_RESET_RNG_SOFT_RESET, REG(RBG_SOFT_RESET));
    
    /* Enable RNG */
    rng200_enable();
    
    /* Wait for warm up (using default value) */
    while(!(ioread32(REG(RNG_INT_STATUS)) & 
        RNG_INT_STATUS_TOTAL_BITS_COUNT_IRQ))
        ;
}

static unsigned int rng200_available_words(void)
{
    /* Perform error checking here */
    if (ioread32(REG(RNG_INT_STATUS)) & 
        RNG_INT_STATUS_MASTER_FAIL_LOCKOUT_IRQ) {

        /* Reset RNG in case of FAIL conditions */
        rng200_restart();
    }
    
    return ioread32(REG(RNG_FIFO_COUNT)) & RNG_FIFO_COUNT_RNG_FIFO_COUNT;
}

static u32 rng200_get_random_number(void)
{
    while(rng200_available_words() == 0);
    return (u32)ioread32(REG(RNG_FIFO_DATA));
}

static void rng200_init(void)
{
    regbase = ioremap(IPROC_CCS_RNG_REG_BASE, 0x1000);
    rng200_restart();
}

static void rng200_exit(void)
{
    rng200_disable();
    if (regbase) {
        iounmap(regbase);
        regbase = NULL;
    }
}

static int rng_data_present(struct hwrng *rng, int wait)
{
    unsigned int data, i;

    for (i = 0; i < 20; i++) {
        data = rng200_available_words() ? 1 : 0;
        if (data || !wait)
            break;
        /*
         * RNG produces data fast enough.  We *could* use the RNG IRQ, but
         * that'd be higher overhead ... so why bother?
         */
        udelay(10);
    }

    return (int)data;
}

static int rng_data_read(struct hwrng *rng, u32 *data)
{
    if (!atomic_read(&bus_is_probed))
        return -ENODEV;

    /* lock it here since other kernel drivers can access it */
    mutex_lock(&lock);
    *data = rng200_get_random_number();
    mutex_unlock(&lock);
    return 4;
}

static struct hwrng rng_ops = {
    .name = "iproc-rng200",
    .data_present = rng_data_present,
    .data_read = rng_data_read,
};

static int __devinit rng_probe(struct platform_device *pdev)
{
    int ret;

    /* We only accept one device, and it must have an id of -1 */
    if (pdev->id != -1)
        return -ENODEV;

    atomic_set(&bus_is_probed, 0);

    rng200_init();

    /* register to the Linux RNG framework */
    ret = hwrng_register(&rng_ops);
    if (ret)
        goto err_out;

    atomic_set(&bus_is_probed, 1);

    return 0;

err_out:
    return ret;
}

static int __devexit rng_remove(struct platform_device *pdev)
{
    atomic_set(&bus_is_probed, 0);
    hwrng_unregister(&rng_ops);
    return 0;
}

static int rng_suspend(struct platform_device *dev, pm_message_t msg)
{
    rng200_disable();
    return 0;
}

static int rng_resume(struct platform_device *dev)
{
    rng200_enable();
    return 0;
}

static struct platform_driver rng_driver = {
    .driver = {
        .name = "iproc-rng200",
        .owner = THIS_MODULE,
    },
    .suspend   = rng_suspend,
    .resume    = rng_resume,
    .remove    = __devexit_p(rng_remove),
    .probe     = rng_probe,
};

static struct platform_device board_rng_device = {
	.name		=	"iproc-rng200",
	.id		    =	-1,
};

static char banner[] __initdata = KERN_INFO "Broadcom iProc RNG200 Driver\n";

static int __init rng_init(void)
{
	int r;

    printk(banner);

    r = platform_driver_register(&rng_driver);
    if (r) {
        return r;
    }

    r = platform_device_register(&board_rng_device);
    if (r) {
        platform_driver_unregister(&rng_driver);
    }
    return r;
}

static void __exit rng_exit(void)
{
    rng200_exit();
    platform_device_unregister(&board_rng_device);
    platform_driver_unregister(&rng_driver);
}

module_init(rng_init);
module_exit(rng_exit);

MODULE_AUTHOR("Broadcom");
MODULE_DESCRIPTION("iProc RNG200 Device Driver");
MODULE_LICENSE("GPL");
