
#include <vxWorks.h>
#include <version.h>
#include <kernelLib.h>
#include <sysLib.h>
#include <stdlib.h>
#include <dosFsLib.h>

#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/vxworks/hal.h>
#include "mbz.h"
#include "flashDrvLib.h"
#include "flashFsLib.h"
#include "srecLoad.h"

static int robo_4702_upgrade_image(char *fname, unsigned int flags, 
                 int (*f_loader)(char *fname, char *fbuf, 
                                 int bufSize, int *entry_pt))
{
    char		*buf = 0;
    int			rv = -1;
    int			i = 0;
    int			entry;
    int                 bootImgSize = 512 * 1024;

    if (flashDrvLibInit() == ERROR) {
        printf("flashBoot: Boot flash not found (jumpered right?)\n");
        goto done;
    }

    if ((buf = malloc(bootImgSize)) == 0) {
	printf("flashBoot: out of memory\n");
	goto done;
    }

    if (f_loader(fname, buf, bootImgSize, &entry) < 0) {
	printf("flashBoot: Failed to read image.\n");
	goto done;
    }

#if !defined(NSSIOLE) && !defined(BCM_ICS)
    for(i = 0; i < (512 * 1024); i += 4) {
        tmpc = buf[i];
        buf[i] = buf[i + 3];
        buf[i + 3] = tmpc;

        tmpc = buf[i + 1];
        buf[i + 1] = buf[i + 2];
        buf[i + 2] = tmpc;
    }
#endif

    printf("%d\nErasing boot area ...", i);

    for (i = FLASH_BOOT_START_SECTOR;
         i < FLASH_BOOT_START_SECTOR + FLASH_BOOT_SIZE_SECTORS; i++) {
        printf("FLASH_BOOT_START = 0x%08x\n", FLASH_BOOT_START);
        printf("FLASH_SECTOR_SIZE = %d\n", FLASH_SECTOR_SIZE);
        printf("FLASH_BOOT_START_SECTOR = %d\n", FLASH_BOOT_START_SECTOR);
        printf("FLASH_BOOT_SIZE_SECTORS = %d\n", FLASH_BOOT_SIZE_SECTORS);
    

        if (flashEraseBank(i, 1) != OK) {
	    printf("\nflashBoot: failed erasing -- PROM DESTROYED\n");
            goto done;
        }

        printf(".");
    }

    printf("done\nWriting boot data ...");

    if (flashBlkWrite(FLASH_BOOT_START_SECTOR, buf,
		      0, FLASH_BOOT_SIZE) != OK) {
        printf("\nflashBoot: failed writing -- PROM DESTROYED\n");
        goto done;
    }

    printf("\nDone\n");

    rv = 0;

 done:

    if (buf) {
	free(buf);
    }

    return rv;
}

static int robo_4702_print_info(void)
{
    int		core, sb,pci;

    sys47xxClocks(&core, &sb, &pci);

    if (core) {
        printf("CPU: %d MHz, MEM: %d MHz, ", core, core);
    } else {
        printf("CPU: Unknown Mhz, MEM: Unknown Mhz, ");
    }

    if (sb) {
        printf("SB: %d MHz, ", sb);
    } else {
        printf("SB: Unknown MHz, ");
    }

    if (pci) {
        printf("PCI: %d MHz\n", pci);
    } else {
        printf("PCI: Unknown MHz\n");
    }
    return 0;
}


static char * robo_4702_flash_dev_name(void)
{
    return FLASH_FS_NAME;
}

static int robo_4702_format_flash(int format, unsigned int flags)
{
    int	rv = 0;

    if (format) {
	extern STATUS tffsBCM47xxFormat(void);
	tffsBCM47xxFormat();
    }
    return(rv);
}

static int robo_4702_led_write_string(const char * s)
{
    sysLedDsply(s);
    return 0;
}

static platform_hal_t robo_4702_hal_info = {
    "robo_4702",                                   /* name */
    0,                                             /* flags */
    (PLATFORM_CAP_DMA_MEM_UNCACHABLE |
     PLATFORM_CAP_PCI | PLATFORM_CAP_FLASH_FS),    /* caps */
    0,                                             /* bus_caps */
    robo_4702_print_info,                          /* f_dump_info */
    robo_4702_upgrade_image,                       /* f_upgrade_image */
    robo_4702_flash_dev_name,                      /* f_flash_device_name */
    robo_4702_format_flash,                        /* f_format_fs */
    flashFsSync,                                   /* f_fs_sync */
    NULL,                                          /* f_fs_check */
    sysTodSet,                                     /* f_tod_set */
    sysTodGet,                                     /* f_tod_get */
    NULL,                                          /* f_start_wdog */
    NULL,                                          /* f_reboot */
    robo_4702_led_write_string                     /* f_led_write_string */
};

int platform_attach(platform_hal_t **platform_info)
{
    if (!platform_info) {
        return -1;
    }

    *platform_info = &robo_4702_hal_info;
    return 0;
}

