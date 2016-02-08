
#include <vxWorks.h>
#include <version.h>
#include <kernelLib.h>
#include <sysLib.h>
#include <bmw.h>
#include <stdlib.h>
#include <dosFsLib.h>

#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/vxworks/hal.h>
extern STATUS flashFsSync(void);
#include "stdio.h"

static int alpha8245_upgrade_image(char *fname, unsigned int flags, 
                 int (*f_loader)(char *fname, char *fbuf, 
                                 int bufSize, int *entry_pt))
{
    return 0;
}

static char * alpha8245_flash_dev_name(void)
{
    return "no flash fs";
}

static int alpha8245_format_flash(int format, unsigned int flags)
{
    return 0;
}

static int alpha8245_arm_wdog(unsigned int usec)
{
    return 0;
}

extern int sysRamToCpuClkGet (void);
extern int sysPciToRamClkGet (void);

static int alpha8245_print_info(void)
{
    UINT32	tb, pci, cpu = 0;
    int         ramClk;
    int         factor;

    tb = sysMeasureTimebase ();

    /* 
    * The system logic clock is the RAM clock which runs at
    * 4 times the decrementer rate
    */
    ramClk = tb << 2;

    /* 
    * The ramClk is some multiple of the pciClk.  Find the multiple. 
    * Note: the ClkGet() functions return the multiplier*2 in order
    * to avoid floating point arithmetic.  The BSP reads the PLL config
    * register to get the settings.  Can't use HID1 like mousse-- it's 
    * non-deterministic on the 8245.
    */

    factor = sysPciToRamClkGet ();
    pci = 2 * ramClk / factor;

    /* The CPU clock is a multiple of the RAM clock. */

    factor = sysRamToCpuClkGet();
    cpu = ramClk * (factor >> 1);
    if (factor & 0x1)   /* take care of the .5 multipliers */
       cpu += (ramClk >> 1);
    
    printf("Timebase: %d.%06d MHz, MEM: %d.%06d MHz, PCI: %d.%06d MHz, CPU: %d.%06d MHz\n",
	   tb / 1000000, tb % 1000000,
	   ramClk / 1000000, ramClk % 1000000,
	   pci / 1000000, pci % 1000000,
	   cpu / 1000000, cpu % 1000000);

    return 0;
}

static int alpha8245_led_write_string(const char * s)
{
#define MPC824X_ALPHAN_CHAR0	((volatile char *) 0x7c002003)
#define MPC824X_ALPHAN_CHAR1	((volatile char *) 0x7c002002)
#define MPC824X_ALPHAN_CHAR2	((volatile char *) 0x7c002001)
#define MPC824X_ALPHAN_CHAR3	((volatile char *) 0x7c002000)
    *MPC824X_ALPHAN_CHAR0 = *s ? *s++ : ' ';
    *MPC824X_ALPHAN_CHAR1 = *s ? *s++ : ' ';
    *MPC824X_ALPHAN_CHAR2 = *s ? *s++ : ' ';
    *MPC824X_ALPHAN_CHAR3 = *s ? *s++ : ' ';

    return 0;
}

static platform_hal_t alpha8245_hal_info = {
    .name                       = "alpha8245",
    .caps                       = (PLATFORM_CAP_PCI | PLATFORM_CAP_FLASH_FS),
    .f_dump_info                = alpha8245_print_info,
    .f_upgrade_image            = alpha8245_upgrade_image,
    .f_flash_device_name        = alpha8245_flash_dev_name,
    .f_format_fs                = alpha8245_format_flash,
    .f_fs_sync                  = flashFsSync,
    .f_tod_set                  = sysTodSet,
    .f_tod_get                  = sysTodGet,
    .f_start_wdog               = alpha8245_arm_wdog,
    .f_led_write_string         = alpha8245_led_write_string
};

int platform_attach(platform_hal_t **platform_info)
{
    if (!platform_info) {
        return -1;
    }

    *platform_info = &alpha8245_hal_info;
    return 0;
}

