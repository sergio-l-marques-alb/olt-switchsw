/*
 * $Id: sal.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File: 	sal.c
 * Purpose:	Defines sal routines for eCos targets.
 */
#include <ctype.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/kernel/kapi.h>

#include <sal/core/spl.h>
#include <sal/core/boot.h>
#include <sal/core/time.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>

#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/appl/config.h>

#include <cyg/infra/diag.h>

#include <stdio.h> /* for printf */
#include <stdlib.h> /* for malloc */

#ifdef	NO_FILEIO
#undef	SAL_CONFIG_FILE_DISABLE
#define	SAL_CONFIG_FILE_DISABLE
#endif	/* NO_FILEIO */

#define K0_TO_K1(x)     ((unsigned)(x) | 0xA0000000)   /* kseg0 to kseg1 */
#define K1_TO_K0(x)     ((unsigned)(x) & 0x9FFFFFFF)   /* kseg1 to kseg0 */
#define K0_TO_PHYS(x)   ((unsigned)(x) & 0x1FFFFFFF)   /* kseg0 to physical */
#define K1_TO_PHYS(x)   ((unsigned)(x) & 0x1FFFFFFF)   /* kseg1 to physical */
#define PHYS_TO_K0(x)   ((unsigned)(x) | 0x80000000)   /* physical to kseg0 */
#define PHYS_TO_K1(x)   ((unsigned)(x) | 0xA0000000)   /* physical to kseg1 */

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
static unsigned int _sal_dma_alloc_max;
static unsigned int _sal_dma_alloc_curr;

#define SAL_DMA_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size, ilock) \
        a_curr += (a_size);                                             \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)

#define SAL_DMA_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size, ilock)        \
        a_curr -= (a_size)

/*
 * Function:
 *      sal_dma_alloc_resource_usage_get
 * Purpose:
 *      Provides Current/Maximum memory allocation.
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max - Memory usage high water mark
 */

void 
sal_dma_alloc_resource_usage_get(uint32 *alloc_curr, uint32 *alloc_max)
{
    if (alloc_curr != NULL) {
        *alloc_curr = _sal_dma_alloc_curr;
    }
    if (alloc_max != NULL) {
        *alloc_max = _sal_dma_alloc_max;
    }
}
#endif /* INCLUDE_BCM_SAL_PROFILE */
#endif /* BROADCOM_DEBUG */

#define DMA_MEMORY_UNCACHABLE

#ifdef DMA_MEMORY_UNCACHABLE
static void * cacheDmaMalloc (size_t bytes )
{
    char * pBuffer;

    if ((pBuffer = (char *) malloc(bytes)) == NULL)
        return ((void *) pBuffer);
    else {
    	   HAL_DCACHE_FLUSH(pBuffer, bytes);
    	   HAL_DCACHE_INVALIDATE(pBuffer, bytes);
        return ((void *) K0_TO_K1(pBuffer));
    }
}

static int cacheDmaFree ( void * pBuf )
{
    free ((void *)K1_TO_K0(pBuf));
    return (0);
}
#endif

/*
 * Function:
 *	sal_strcasecmp
 * Purpose:
 *	Compare two strings ignoring the case of the characters.
 * Parameters:
 *	s1 - first string to compare
 *	s2 - second string to compare
 * Returns:
 *	0 if s1 and s2 are identical.
 *	negative integer if s1 < s2.
 *	positive integer if s1 > s2.
 * Notes
 *	See man page of strcasecmp for more info.
 */

int
sal_strcasecmp(const char *s1, const char *s2)
{
    unsigned char c1, c2;

    do {
        c1 = *s1++;
        c1 = tolower(c1);
        c2 = *s2++;
        c2 = tolower(c2);
    } while (c1 == c2 && c1 != 0);

    return (int)c1 - (int)c2;
}

/*
 * Function:
 *	sal_strncasecmp
 * Purpose:
 *	Compare two strings ignoring the case of the characters.
 * Parameters:
 *	s1 - first string to compare
 *	s2 - second string to compare
 *	n - maximum number of characters to compare
 * Returns:
 *	0 if s1 and s2 are identical up to n characters.
 *	negative integer if s1 < s2 up to n characters.
 *	positive integer if s1 > s2 up to n characters.
 * Notes
 *	See man page of strncasecmp for more info.
 */

int
sal_strncasecmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1, c2;

    do {
        if (n-- < 1) {
            return 0;
        }
        c1 = *s1++;
        c1 = tolower(c1);
        c2 = *s2++;
        c2 = tolower(c2);
    } while (c1 == c2 && c1 != 0);

    return (int)c1 - (int)c2;
}

/*
 * Function:
 *	sal_date_set
 * Purpose:
 *	Set system time/date
 * Parameters:
 *	val - new system time/date
 * Returns:
 *	0 - success
 *	-1 - failure
 */

int
sal_date_set(sal_time_t *val)
{
#if 0 
	
#if defined(MOUSSE) || defined(IDTRP334) \
    || defined(BMW) || defined(MBZ) || defined(IDT438) || defined(NSX) \
    || defined(ROBO_4704) || defined(ROBO_4702) || defined(METROCORE)

    struct tm		*tm;
    struct timespec 	tp;
#endif
    int			sts;

#if defined(MOUSSE) || defined(IDTRP334) \
    || defined(BMW) || defined(MBZ) || defined(IDT438) || defined(NSX) || \
    defined(ROBO_4704) || defined(ROBO_4702) || defined(METROCORE)

    tp.tv_sec  = *val;
    tp.tv_nsec = 0;

    clock_settime(CLOCK_REALTIME, &tp);

    tm = gmtime(val);

    tm->tm_year += 1900;
    tm->tm_mon += 1;

    sts = SYS_TOD_SET(tm->tm_year,	/* 1980-2079 */
		      tm->tm_mon,	/* 01-12 */
		      tm->tm_mday,	/* 01-31 */
		      tm->tm_hour,	/* 00-23 */
		      tm->tm_min,	/* 00-59 */
		      tm->tm_sec);	/* 00-59 */

#else /* !MOUSSE && !BMW && !IDTRP334 && !MBZ */
    printk("Don't know how to set date on this platform\n");
    sts = ERROR;
#endif /* !MOUSSE && !BMW && !IDTRP334 && !MBZ */

    return (sts == ERROR) ? -1 : 0;

#endif 
	return -1;
}

/*
 * Function:
 *	sal_date_get
 * Purpose:
 *	Get system date/time
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failure
 */

int
sal_date_get(sal_time_t *val)
{
#if 0 
    time(val);
#endif    
    return -1;
}

#if 0 
/*
 * Internal functions for VxWorks Control-C handling
 */

static void
abortFuncJob(WIND_TCB *taskId)
{
    int		tid;

    /*
     * This runs in the context of some task manager thread and sends a
     * signal back to the shell.
     */

    taskId->excInfo.valid = 0;

    tid = PTR_TO_INT(sal_thread_main_get());

    if (tid != 0) {
        kill(tid, SIGINT);
    }
}

static int
abortFuncHandler(void)
{
    /*
     * This runs at INTERRUPT level where we can't do much at all except
     * call the undocumented excJobAdd to schedule a function to be
     * called by some task manager thread.
     */

    excJobAdd(abortFuncJob, PTR_TO_INT(taskIdCurrent), 0, 0, 0, 0, 0);

    return 0;
}

int
abortFuncDefault(void)
{
    /*
     * There is no official way to get the default abort function from
     * vxWorks, so we'll try this:
     */

    ioctl(ioTaskStdGet(0,STD_IN), FIORFLUSH, 0);

    excJobAdd(shellRestart, TRUE, 0, 0, 0, 0, 0);

    return 0;
}

#endif 

void
sal_readline_init(void)
{
#if 0	
#ifdef INCLUDE_EDITLINE
    extern void rl_real_ttyset(int);
    /* Put console in raw mode for editline package */
    rl_real_ttyset(0);
#endif /* INCLUDE_EDITLINE */
#endif
}

void
sal_readline_term(void)
{
#if 0	
#ifdef INCLUDE_EDITLINE
    extern void rl_real_ttyset(int);
    rl_real_ttyset(1);
#endif /* INCLUDE_EDITLINE */
#endif
}

/*
 * Function:
 *	sal_stats
 * Purpose:
 *	Display statistics about the current platform.
 * Parameters:
 *	None
 */

#if defined(MOUSSE)
void
sal_stats(void)
{
    UINT32	tb, pci, cpu = 0;
    int		pci66;

    tb = sysMeasureTimebase();
    pci = 2 * tb;

    pci66 = (pci >= 45000000);

    switch (vxHid1Get() >> 27) {	/* HID1[0:4] = PLLRATIO */
    case 0x18:		/* 0b11000: 200 MHz CPU, PCI = 33 MHz */
        cpu = pci * 6;
        break;
    case 0x1a:		/* 0b11010, 266 MHz CPU, PCI = 33 or 66 MHz */
        cpu = pci66 ? pci * 4 : pci * 8;
        break;
    }

    printk("Timebase: %d.%06d MHz, PCI: %d.%06d MHz, CPU: %d.%06d MHz\n",
	   tb / 1000000, tb % 1000000,
	   pci / 1000000, pci % 1000000,
	   cpu / 1000000, cpu % 1000000);
}
#endif /* MOUSSE */

#if defined(BMW)

extern int sysRamToCpuClkGet (void);
extern int sysPciToRamClkGet (void);

void
sal_stats(void)
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
    
    printk("Timebase: %d.%06d MHz, MEM: %d.%06d MHz, PCI: %d.%06d MHz, CPU: %d.%06d MHz\n",
	   tb / 1000000, tb % 1000000,
	   ramClk / 1000000, ramClk % 1000000,
	   pci / 1000000, pci % 1000000,
	   cpu / 1000000, cpu % 1000000);
}
#endif /* BMW */

#if defined(MBZ) || defined(ROBO_4704) || defined(ROBO_4702)
void
sal_stats(void)
{
    int		core, sb,pci;

    sys47xxClocks(&core, &sb, &pci);

    if (core) {
        printk("CPU: %d MHz, MEM: %d MHz, ", core, core);
    } else {
        printk("CPU: Unknown Mhz, MEM: Unknown Mhz, ");
    }

    if (sb) {
        printk("SB: %d MHz, ", sb);
    } else {
        printk("SB: Unknown MHz, ");
    }

    if (pci) {
        printk("PCI: %d MHz\n", pci);
    } else {
        printk("PCI: Unknown MHz\n");
    }
}
#endif /* MBZ */


/*
 * MIPS cache check (MIPS only)
 *
 *   May be useful while bringing up a MIPS BSP.
 *   Define it to 1 to get diagnostic output during boot-up.
 */

#define MIPS_CACHE_SANITY	0

#if MIPS_CACHE_SANITY

STATIC int
_sal_mips_cache_test(char *pfx, uint32 *buf, char *allocator)
{
    volatile uint32	*buf_c, *buf_u;
    int			t, rv = 0;

    printk("%sTesting %s\n", pfx, allocator);

    if (buf == NULL) {
        printk("%s==> %s returns NULL\n", pfx, allocator);
        return -1;
    }

    if (((uint32) buf & 0xe0000000) == 0xa0000000) {
        printk("%s==> %s returns uncached memory\n", pfx, allocator);
        return 0;
    }

    if (((uint32) buf & 0xe0000000) != 0x80000000) {
        printk("%s==> ERROR: %s returns unknown memory\n", pfx, allocator);
        return -1;
    }

    buf_c = (uint32 *) buf;
    buf_u = (uint32 *) (((uint32) buf & ~0xe0000000) | 0xa0000000);

    printk("%s==> buffer: cached=0x%08x uncached=0x%08x\n",
	   pfx, (uint32) buf_c, (uint32) buf_u);

    buf_c[0] = 0x44444444;
    buf_u[0] = 0xcccccccc;

    if (buf_c[0] != 0x44444444) {
        printk("%s==> ERROR: cache does not retain data\n", pfx);
        rv = -1;
    }

    buf_u[0] = 0x55555555;
    buf_c[0] = 0xaaaaaaaa;

    if (buf_u[0] != 0x55555555) {
        printk("%s==> write-through cache\n", pfx);
    } else {
        sal_dma_flush((void *) buf_c, 40);

        t = (buf_u[0] == 0x55555555);

        printk("%s==> write-back cache\n", pfx);

        if (t) {
	           printk("%s==> ERROR: cache flush ineffective\n", pfx);
            rv = -1;
        }
    }

    buf_c[0] = 0x77777777;
    sal_dma_flush((void *) buf_c, 40);
    buf_u[0] = 0x88888888;

    if (buf_c[0] != 0x77777777) {
        printk("%s==> cache flush also invalidates\n", pfx);
    } else {
        printk("%s==> cache flush does not invalidate\n", pfx);
    }

    buf_c[0] = 0x77777777;
    sal_dma_inval((void *) buf_c, 40);
    buf_u[0] = 0x88888888;

    if (buf_c[0] != 0x88888888) {
        printk("%s==> ERROR: invalidation ineffective\n", pfx);
        rv = -1;
    }

    buf_c[0] = 0x77777777;
    buf_u[0] = 0x88888888;
    sal_dma_inval((void *) buf_c, 40);

    if (buf_u[0] == 0x77777777) {
        printk("%s==> ERROR: invalidation writes back\n", pfx);
        rv = -1;
    }

    return rv;
}

STATIC int
_sal_mips_cache_sanity(char *pfx)
{
    uint32		*buf;
    int			rv = 0;

    buf = sal_alloc(256, "ctest1");

    if (_sal_mips_cache_test(pfx, buf, "sal_alloc") < 0) {
        rv = -1;
    }

    if (buf != NULL) {
        sal_free(buf);
    }

    buf = sal_dma_alloc(256, "ctest2");

    if (_sal_mips_cache_test(pfx, buf, "sal_dma_alloc") < 0) {
        rv = -1;
    }

    if (buf != NULL) {
        sal_dma_free(buf);
    }

    return rv;
}

#endif /* MIPS_CACHE_SANITY */

/*
 * Function:
 *	sal_dma_alloc_init (internal)
 * Purpose:
 *	Initialize the VxWorks cacheFuncs
 * Parameters:
 *	None
 */

static int
sal_dma_alloc_init(void)
{
#if MIPS_CACHE_SANITY
    printk("Running cache sanity with cacheNullFuncs\n");
    sal_cacheFuncs = cacheNullFuncs;
    _sal_mips_cache_sanity("    ");
    printk("\nRunning cache sanity with cacheDmaFuncs\n");
    sal_cacheFuncs = cacheDmaFuncs;
    _sal_mips_cache_sanity("    ");
#endif /* MIPS_CACHE_SANITY */
    return 0;
}

/*
 * Function:
 *	sal_dma_flush
 * Purpose:
 *	Flush/writeback a region of memory from the cache
 * Parameters:
 *	addr - memory region address
 *	len - memory region length
 * Parameters:
 *	None
 */

void
sal_dma_flush(void *addr, int len)
{
    /* Only flush the cache when the address is a cached address */
    if ((((CYG_ADDRESS)addr) & CYGARC_KSEG_MASK) == CYGARC_KSEG_CACHED) {
        HAL_DCACHE_FLUSH(addr, len);
    }
}

/*
 * Function:
 *	sal_dma_inval
 * Purpose:
 *	Flush/writeback/invalidate a region of memory from the cache
 * Parameters:
 *	addr - memory region address
 *	len - memory region length
 * Parameters:
 *	None
 */

void 
sal_dma_inval(void *addr, int len)
{
    HAL_DCACHE_INVALIDATE(addr, len);
}

/*
 * Function:
 *	sal_dma_vtop
 * Purpose:
 *	Convert an address from virtual to physical
 * Parameters:
 *	addr - Virtual memory address
 * Returns:
 *	Physical memory address
 */

void *
sal_dma_vtop(void *addr)
{
    return ((void *) K1_TO_PHYS(addr));
}

/*
 * Function:
 *	sal_dma_ptov
 * Purpose:
 *	Convert an address from physical to virtual
 * Parameters:
 *	addr - Physical memory address
 * Returns:
 *	Virtual memory address
 */

void *
sal_dma_ptov(void *addr)
{
    return ((void *) PHYS_TO_K1(addr));
}

/*
 * Function:
 *	sal_appl_init
 * Purpose:
 *	Initialize the SAL abstraction layer for VxWorks.
 * Parameters:
 *	None
 * Returns:
 *	0 - success
 *	-1 - failed
 */

int
sal_appl_init(void)
{
    extern void sal_config_at_startup(int mode);	
    
    static	int	inited = FALSE;

    if (inited) {
        return(0);
    }
    inited = TRUE;

    /* IMPORTANT: don't use printk before console is initialized */
    sal_console_init();

    sal_dma_alloc_init();

    sal_readline_init();

    /*
     * Initialize flash device if present, ignore errors since it may
     * have been initialized if we booted from it, and/or may not be present.
     */
    (void)sal_flash_init(FALSE);
    
    /*
     * Set home directory to default.
     * Change directory to default.
     */
    (void)sal_homedir_set(NULL);
    (void)sal_cd(NULL); 
    
    /*
     * Initialize simple configuration database after flash has 
     * been initialized.
     */
    (void)sal_config_at_startup(1);
    (void)sal_config_refresh();
    (void)sal_config_at_startup(0);

    return(0);
}

/*
 * Function:
 *	sal_reboot
 * Purpose:
 *	Reboot the system.
 * Parameters:
 *	None
 */

void
sal_reboot(void)
{
    printf("Resetting...\n");
    sal_usleep(SECOND_USEC);	/* Time for messages to flush on serial */
    sysReboot();
}

/*
 * Function:
 *	sal_shell
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	None
 * Notes:
 *	Not supported for Linux kernel mode.
 */

int sal_telnet_active;		

void
sal_shell(void)
{
    extern void cyg_shell_thread(cyg_addrword_t data);	
    cyg_shell_thread(0);	
}

/*
 * Function:
 *	sal_config_init_defaults
 * Purpose:
 *	Set system environment variables.
 * Parameters:
 *	None
 * Returns:
 *	Nothing
 */

void
sal_config_init_defaults(void)
{
    (void)sal_config_set("os", "ecos");
}

/*
 * Function:
 *	sal_flash_init
 * Purpose:
 *	Mounts the ECOS RAM filesystem, restore from flash if valid.
 * Parameters:
 *	format - If TRUE, also erases and formats the flash disk.
 * Returns:
 *	0 - success
 *	-1 - failed
 */

int
sal_flash_init(int format)
{
#ifndef LVL7_FIXUP
    extern int flashFsRestore(void);	
    return (flashFsRestore());
#else
   return 0;
#endif
}

#if defined(ROBO_4704)
int
bcm47xxFlashBoot(char *fname)
{
    FILE		*fp = 0;
    char		*buf = 0;
    char		tmpc;
    int			rv = -1;
    int			i;
    int			entry;
    int			sRecords;

    if (strlen(fname) < 4) {
    bad_fname:
        printk("flashBoot: Illegal file %s, must end in .bin .img or .hex\n",
	       fname);
        goto done;
    }

    if (!strcmp(fname + strlen(fname) - 4, ".img") ||
        !strcmp(fname + strlen(fname) - 4, ".bin")) {
        sRecords = 0;
    } else if (!strcmp(fname + strlen(fname) - 4, ".hex")) {
        sRecords = 1;
    } else {
        goto bad_fname;		/* Above */
    }

    /* Re-probe in case jumper moved */
    if (flashDrvLibInit() == ERROR) {
        printk("flashBoot: Boot flash not found (jumpered right?)\n");
        goto done;
    }

    if ((buf = malloc(FLASH_BOOT_SIZE)) == 0) {
        printk("flashBoot: out of memory\n");
        goto done;
    }

    printf("Loading %s ... ", fname);

    if ((fp = sal_fopen(fname, "r")) == 0) {
        printk("\nflashBoot: could not open file %s\n", fname);
        goto done;
    }

    if (sRecords) {
        if ((i = srecLoad(fp, buf, FLASH_BOOT_SIZE, &entry)) < 0) {
            printk("\nflashBoot: failed reading S-record file %s: %s\n",
                   fname, srecErrmsg(i));
            goto done;
        }
    } else if ((i = fread(buf, 1, FLASH_BOOT_SIZE, fp)) != FLASH_BOOT_SIZE) {
        printk("\nflashBoot: failed reading 1024 kB from binary file %s\n",
               fname);
        goto done;
    }

    printk("%d\nErasing boot area ...", i);

    for (i = FLASH_BOOT_START_SECTOR;
         i < FLASH_BOOT_START_SECTOR + FLASH_BOOT_SIZE_SECTORS; i++) {
        if (flashEraseBank(i, 1) != OK) {
            printk("\nflashBoot: failed erasing -- PROM DESTROYED\n");
            goto done;
        }

        printk(".");
    }

    printk("done\nWriting boot data ...");

    if (flashBlkWrite(FLASH_BOOT_START_SECTOR, buf,
		      0, FLASH_BOOT_SIZE) != OK) {
        printk("\nflashBoot: failed writing -- PROM DESTROYED\n");
        goto done;
    }

    printk("\nDone\n");

    rv = 0;

 done:

    if (fp) {
        fclose(fp);
    }

    if (buf) {
        free(buf);
    }

    return rv;
}
#elif defined(MBZ) || defined(ROBO_4702)
int
bcm47xxFlashBoot(char *fname)
{
    FILE		*fp = 0;
    char		*buf = 0;
    char		tmpc;
    int			rv = -1;
    int			i;
    int			entry;
    int			sRecords;

    if (strlen(fname) < 4) {
    bad_fname:
        printk("flashBoot: Illegal file %s, must end in .img or .hex\n",
	       fname);
        goto done;
    }

    if (!strcmp(fname + strlen(fname) - 4, ".img")) {
        sRecords = 0;
    } else if (!strcmp(fname + strlen(fname) - 4, ".hex")) {
        sRecords = 1;
    } else {
        goto bad_fname;		/* Above */
    }

    /* Re-probe in case jumper moved */
    if (flashDrvLibInit() == ERROR) {
        printk("flashBoot: Boot flash not found (jumpered right?)\n");
        goto done;
    }

    if ((buf = malloc(512 * 1024)) == 0) {
        printk("flashBoot: out of memory\n");
        goto done;
    }

    printf("Loading %s ... ", fname);

    if ((fp = sal_fopen(fname, "r")) == 0) {
        printk("\nflashBoot: could not open file %s\n", fname);
        goto done;
    }

    if (sRecords) {
        if ((i = srecLoad(fp, buf, 512 * 1024, &entry)) < 0) {
            printk("\nflashBoot: failed reading S-record file %s: %s\n",
                   fname, srecErrmsg(i));
            goto done;
        }
    } else if ((i = fread(buf, 1, 512 * 1024, fp)) != 512 * 1024) {
        printk("\nflashBoot: failed reading 512 kB from binary file %s\n",
               fname);
        goto done;
    }

#ifndef NSSIOLE
    for(i = 0; i < (512 * 1024); i += 4) {
        tmpc = buf[i];
        buf[i] = buf[i + 3];
        buf[i + 3] = tmpc;

        tmpc = buf[i + 1];
        buf[i + 1] = buf[i + 2];
        buf[i + 2] = tmpc;
    }
#endif

    printk("%d\nErasing boot area ...", i);

    for (i = FLASH_BOOT_START_SECTOR;
         i < FLASH_BOOT_START_SECTOR + FLASH_BOOT_SIZE_SECTORS;
         i++) {

        if (flashEraseBank(i,1) != OK) {
            printk("\nflashBoot: failed erasing -- PROM DESTROYED\n");
            goto done;
        }

        printk(".");
    }

    printk("done\nWriting boot data ...");

    if (flashBlkWrite(FLASH_BOOT_START_SECTOR, buf, 0, 512 * 1024) != OK) {
        printk("\nflashBoot: failed writing -- PROM DESTROYED\n");
        goto done;
    }

    printk("\nDone\n");

    rv = 0;

 done:

    if (fp) {
        fclose(fp);
    }

    if (buf) {
        free(buf);
    }

    return rv;
}
#endif /* MBZ */




/*
 * Function:
 *	sal_flash_boot
 * Purpose:
 *	Flash the boot area of the flash with an image from a file.
 * Parameters:
 *	file - name of file with image.
 * Returns:
 *	0 - success
 *	-1 - failed
 */

int
sal_flash_boot(char *file)
{
    return(-1);
}

/*
 * Function:
 *	sal_led
 * Purpose:
 *	Display a pattern on the system LEDs.
 * Parameters:
 *	v - pattern to show on system LEDs.
 * Returns:
 *	Previous state of LEDs.
 */

uint32
sal_led(uint32 v)
{
    return 0;    
    
}

/*
 * Function:
 *	sal_led_string
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	s - string to show on LED display
 * Notes:
 *	Not supported for Linux kernel mode.
 */

void
sal_led_string(const char *s)
{
    COMPILER_REFERENCE(s);
}

/*
 * Function:
 *	sal_watchdog_arm
 * Purpose:
 *	Set up watch dog reset function.
 * Parameters:
 *	usec - 0 to disarm watchdog.
 *		non-zero to set up watchdog time in microseconds.
 * Returns:
 *	0 - success
 *	-1 - failure
 * Notes:
 *	If usec is non-zero, the watchdog timer is armed (or re-armed)
 *	for approximately usec microseconds (if the exact requested
 *	usec is not supported, the next higher available value is used).
 *
 *	If the timer is armed and expires, the system is reset.
 */

int
sal_watchdog_arm(uint32 usec)
{
    COMPILER_REFERENCE(usec);

#if defined(BMW)
    SYS_WATCHDOG_RESET();               /* Configure to reset system */
    return SYS_WATCHDOG_ARM(usec);
#endif

#if defined(MOUSSE)
    SYS_WATCHDOG_RESET();               /* Configure to reset system */
    m48_watchdog_arm(usec);
    return 0;
#endif

    return (-1);
}


#define DK_PCI 1

int 
sal_memory_check(uint32 addr)
{
#if 0 	
	
    

#if !defined(IDTRP334) && !defined(IDT438) && !defined(NSX) && !defined(METROCORE)

#ifndef VX_NO_MEM_PROBE
    STATUS probeStatus;
#endif

    /* Sanity check that we can do DMA ... */

    if (!CACHE_DMA_IS_WRITE_COHERENT()) {
	printk("sal_memory_check: device requires cache coherent memory\n");
	return -1;
    }

    debugk(DK_PCI, "System Memory Top: 0x%x\n", PTR_TO_INT(sysMemTop()));

#ifndef VX_NO_MEM_PROBE		/* Make.local option */
    /*
     * Test physical memory for sanity's sake. Just see if we can read
     * below the end of DRAM.
     */

    if (vxMemProbe((char *)sysMemTop() - 24,
		   VX_READ, 4, (char*)&probeStatus) != OK) {
        printk("sal_memory_check: can't read last word in memory!\n");
        return -1;
    }

    /*
     * Now, test that you can read/write the memory and that it does not
     * generate a bus error.
     */

    if ((vxMemProbe((char *)addr,
		    VX_READ, 4, (char*)&probeStatus) != OK) ||
       	(vxMemProbe((char *)addr,
		    VX_WRITE, 4, (char*)&probeStatus) != OK)) {
        printk("sal_memory_check: vxMemProbe R/W error (addr=0x%x)\n",
	       addr);
        return -1;
    }

#endif /* VX_NO_MEM_PROBE */

#endif /* !defined(IDTRP334) && !defined(IDT438) && !defined(NSX) && !defined(METROCORE) */

#endif 

    return 0;
}

#ifdef ECOS_DEBUG

typedef struct mblock_s {
    uint32		start_sentinel;		/* value: 0xaaaaaaaa */
    char                *description;
    int                 size;
    struct mblock_s	*prev;
    struct mblock_s	*next;
    /* Variable user data; size S = (size + 3) / 4 words. */
    uint32		user_data[1];
    /* Then sentinel follows user data at user_data[S]; value: 0xbbbbbbbb */
    /* For fixed the robo4702 tx buffers' addresses need in 4K alignment */
#ifdef ROBO_4702
    uint32		align_addr;
#endif
} mblock_t;

static mblock_t *head = NULL;

#endif /* DEBUG */

/*
 * Function:
 *	sal_dma_alloc
 * Purpose:
 *	Allocate memory that can be DMAd into/out-of.
 * Parameters:
 *	sx - number of bytes to allocate
 *	s - string associated with allocate
 * Returns:
 *	Pointer to allocated memory or NULL.
 */

void *
sal_dma_alloc(unsigned int sz, char *s)
{
#ifdef ECOS_DEBUG
    mblock_t	*p;
    
/* For fixed the robo4702 tx buffers' addresses need in 4K alignment */
#ifdef ROBO_4702
    int		size_words = (sz + 3) / 4 + 1024;
#else
    int		size_words = (sz + 3) / 4;
#endif
    int		il;

    /*
     * Allocate space for block structure, user data, and the sentinel
     * at the end of the block (accounted for by user_data[1]).
     */

#ifdef DMA_MEMORY_UNCACHABLE
    p = cacheDmaMalloc(sizeof (mblock_t) + size_words * 4);
#else
    p = malloc(sizeof (mblock_t) + size_words * 4);
#endif

    if (p == NULL) {
	return NULL;
    }

    p->start_sentinel = 0xaaaaaaaa;
    p->description = s;
    p->size = sz;
    p->user_data[size_words] = 0xbbbbbbbb;

    il = intLock();
    if (head != NULL) {
        head->prev = p;
    }
    p->prev = NULL;
    p->next = head;
    head = p;
    intUnlock(il);

/* For fixed the robo4702 tx buffers' addresses need in 4K alignment */
#ifdef ROBO_4702
    p->align_addr = ((uint32)(&p->user_data[0]) + 4095) / 4096 * 4096;
    return (void *)p->align_addr;
#else
    return (void *)&p->user_data[0];
#endif

#else /* !ECOS_DEBUG */

#ifdef DMA_MEMORY_UNCACHABLE
    return (void *)cacheDmaMalloc(sz);
#else
    return (void *)malloc(sz);
#endif

#endif /* !ECOS_DEBUG */
}

#ifdef ECOS_DEBUG

/* To do: use real data segment limits for bad pointer detection */

#define GOOD_PTR(p)						\
	(PTR_TO_INT(p) >= 0x00001000U &&			\
	 PTR_TO_INT(p) < 0xfffff000U)

#define GOOD_START(p)						\
	(p->start_sentinel == 0xaaaaaaaa)

/* For fixed the robo4702 tx buffers' addresses need in 4K alignment */
#ifdef ROBO_4702
#define GOOD_END(p) \
	(p->user_data[(p->size + 3) / 4 + 1024] == 0xbbbbbbbb)
#else
#define GOOD_END(p)						\
	(p->user_data[(p->size + 3) / 4] == 0xbbbbbbbb)
#endif

#define GOOD_FIELD		0

#endif /* DEBUG */

/*
 * Function:
 *	sal_dma_free
 * Purpose:
 *	Free memory allocated by sal_dma_alloc
 * Parameters:
 *	addr - pointer to memory to free.
 * Returns:
 *	Nothing.
 */

#if defined(NSX) || defined(METROCORE)
void
sal_dma_free(void *addr)
{
#ifdef DMA_MEMORY_UNCACHABLE
    cacheDmaFree(addr);
#else
    free(addr);
#endif
}

#else

void
sal_dma_free(void *addr)
{
#ifdef ECOS_DEBUG

    mblock_t	*p;
    int		il;
    int		size_words;

    assert(GOOD_PTR(addr));	/* Use macro to beautify assert message */

/* For fixed the robo4702 tx buffers' addresses need in 4K alignment */
#ifdef ROBO_4702
    p = head;
    while (p != NULL) {
        if (p->align_addr == (uint32)addr) {
            break;
        }
        p = p->next;
    }
#else
    p = (mblock_t *)
    ((char *) addr - (int)(sizeof (*p) - sizeof (p->user_data)));
#endif

    assert(GOOD_START(p));	/* Use macro to beautify assert message */
    assert(GOOD_END(p));

    il = intLock();
    if (p == head) {
        if (p->prev != NULL) {
            intUnlock(il);
            assert(GOOD_FIELD);
        } else {
            head = p->next;
            if (head != NULL) {
                head->prev = NULL;
            }
            intUnlock(il);
        }
    } else {
        if (p->prev == NULL) {
            intUnlock(il);
            assert(GOOD_FIELD);
        } else {
            p->prev->next = p->next;
            if (p->next != NULL) {
                p->next->prev = p->prev;
            }
            intUnlock(il);
        }
    }

/* For fixed the robo4702 tx buffers' addresses need in 4K alignment */
#ifdef ROBO_4702
    size_words = (p->size + 3) / 4 + 1024;
#else
    size_words = (p->size + 3) / 4;
#endif

    /*
     * Detect redundant frees and memory being used after freeing
     * by filling entire block with 0xcc bytes.
     * (Similar to the way VxWorks fills unused memory with 0xee bytes).
     */

    memset(p, 0xcc, sizeof (mblock_t) + size_words * 4);

#ifdef DMA_MEMORY_UNCACHABLE
    cacheDmaFree(p);
#else
    free(p);
#endif

#else /* !DEBUG */
#ifdef DMA_MEMORY_UNCACHABLE
    cacheDmaFree(addr);
#else
    free(addr);
#endif
#endif

}
#endif /* NSX || METROCORE */

#ifdef ECOS_DEBUG
void 
sal_dma_alloc_stat(void *param)
{
    mblock_t *p;
    char *last_desc = "";
    int repeat;
    int rep_count = 0;
    int tot_size = 0;
    int grand_tot = 0;

    repeat = PTR_TO_INT(param);
    for (p = head; p != NULL; p = p->next) {
        grand_tot += p->size;
        if (repeat) { /* Don't show repetitions */
            if (!strcmp(p->description, last_desc)) {
                rep_count++;
                tot_size += p->size;
            } else {
                if (rep_count) {
                    printf("...to %8p, repeats %4d times.  "
                           "Total size 0x%08x\n",
                           &p->user_data[0], rep_count+1, tot_size);
                    rep_count = 0;
                    tot_size = 0;
                }
                printf("%8p: 0x%08x %s\n",
                       &p->user_data[0],
                       p->size,
                       p->description != NULL ? p->description : "???");
                tot_size += p->size;
                last_desc = p->description;
            }
        } else { /* Show every entry */
            printf("%8p: 0x%08x %s\n",
                   &p->user_data[0],
                   p->size,
                   p->description != NULL ? p->description : "???");
        }
    }
    printf("Grand total of %d bytes allocated\n", grand_tot);
}
#endif /* DEBUG */

/*
 * Function:
 *	sal_rand
 * Purpose:
 *	Get random number.
 * Parameters:
 *	None
 * Returns:
 *	Random number between 0 and SAL_RAND_MAX (2147483647).
 * Notes:
 *	VxWorks rand() returns a 15-bit random number.
 */

int
sal_rand(void)
{
    return ((rand() << 22) ^
	    (rand() << 11) ^
	    (rand() <<  0)) & SAL_RAND_MAX;
}

/*
 * Function:
 *	sal_srand
 * Purpose:
 *	For compatibility only.
 * Parameters:
 *	seed - new seed for random number generator
 */

void
sal_srand(unsigned int seed)
{	
    srand(seed);
}

