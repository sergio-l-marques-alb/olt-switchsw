#ifndef __ptin860t_h__
#define __ptin860t_h__

#ifndef __ASSEMBLY__
#include <linux/types.h>
#include <asm/irq.h>

#define BOOTROM_RESTART_ADDR	((uint)0xfff00100)

#define CPM_MAP_ADDR            ((uint)0xFF000000)

#define IMAP_ADDR               ((uint)0xff000000)
#define IMAP_SIZE               ((uint)(64 * 1024))


/*********************************************************************/
/* Ethernet                                                          */
/*********************************************************************/
/* Interrupt level assignments.
 */
 #define FEC_INTERRUPT   SIU_LEVEL1      /* FEC interrupt */
 #define PHY_INTERRUPT   SIU_IRQ2        /* PHY link change interrupt */

/*********************************************************************/
#if defined(CONFIG_PTIN860T)
 
# if defined(CONFIG_SCC1_ENET)
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.
 */
#define PA_ENET_RXD     ((ushort)0x0001)        /* PA 15 */
#define PA_ENET_TXD     ((ushort)0x0002)        /* PA 14 */
#define PA_ENET_TCLK    ((ushort)0x0800)        /* PA  4 */
#define PA_ENET_RCLK    ((ushort)0x0200)        /* PA  6 */

#define PB_ENET_TENA    ((uint)0x00001000)      /* PB 19 */

#define PC_ENET_CLSN    ((ushort)0x0010)        /* PC 11: COL */
#define PC_ENET_RENA    ((ushort)0x0020)        /* PC 10: CD  */

/* Control bits in the SICR to route TCLK (CLK4) and RCLK (CLK2) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#define SICR_ENET_MASK  ((uint)0x000000FF)
#define SICR_ENET_CLKRT ((uint)0x0000002F)
 
# elif defined(CONFIG_SCC_ENET)
#  error No SCC defined for Ethernet
# endif
 
#endif  /* CONFIG_PTIN860T */


/******************************************************************/

/******************************************************************/
/*IDE & PCMCIA                                                    */
/******************************************************************/

#define PCMCIA_MEM_ADDR		((uint)0x50000000)
#define PCMCIA_MEM_SIZE		((uint)(64 << 20))

#define CFG_PCMCIA_MEM_ADDR     PCMCIA_MEM_ADDR
#define CFG_PCMCIA_MEM_SIZE     PCMCIA_MEM_SIZE

#define CFG_ATA_IDE0_OFFSET     0x0000

#define CFG_ATA_BASE_ADDR       PCMCIA_MEM_ADDR

/* Offset for data I/O - data register is memory mapped        */
#define CFG_ATA_DATA_OFFSET     (CFG_PCMCIA_MEM_SIZE)
/* Offset for normal register accesses - task file is memory mapped*/
#define CFG_ATA_REG_OFFSET      (2 * CFG_PCMCIA_MEM_SIZE)       

/* Offset for alternate registers       */
//#define CFG_ATA_ALT_OFFSET    0x0100

#define MAX_HWIFS               1

#define IDE0_BASE_OFFSET        0
#define IDE0_DATA_REG_OFFSET    (PCMCIA_MEM_SIZE)
#define IDE0_ERROR_REG_OFFSET   (2 * PCMCIA_MEM_SIZE + 1)
#define IDE0_NSECTOR_REG_OFFSET (2 * PCMCIA_MEM_SIZE + 2)
#define IDE0_SECTOR_REG_OFFSET  (2 * PCMCIA_MEM_SIZE + 3)
#define IDE0_LCYL_REG_OFFSET    (2 * PCMCIA_MEM_SIZE + 4)
#define IDE0_HCYL_REG_OFFSET    (2 * PCMCIA_MEM_SIZE + 5)
#define IDE0_SELECT_REG_OFFSET  (2 * PCMCIA_MEM_SIZE + 6)
#define IDE0_STATUS_REG_OFFSET  (2 * PCMCIA_MEM_SIZE + 7)
#define IDE0_CONTROL_REG_OFFSET (2 * PCMCIA_MEM_SIZE + 0x0E)

//#define IDE0_CONTROL_REG_OFFSET       0x0106 //frg???
#define IDE0_IRQ_REG_OFFSET     0x000A  /* not used //frg????                   */
//#define IDE0_IRQ_REG_OFFSET   (2 * PCMCIA_MEM_SIZE + 0x0A) /* not used */

#define PTIN_IDE_EXTRA_TIMEOUT 20 /* To deal with the 64 MB Transcend */
                                  /* CompactFlash */

/* Interrupt level assignments.
 */
#define FEC_INTERRUPT   SIU_LEVEL1      /* FEC interrupt */
#define PHY_INTERRUPT   SIU_IRQ2        /* PHY link change interrupt */

/* We don't use the 8259.
 */
#define NR_8259_INTS    0
#define IDE0_INTERRUPT  SIU_LEVEL6      /* IDE / PCMCIA Interrupt */

/*IDE & PCMCIA FIM*************************************************/

/*WATCHDOG*********************************************************/
//#define CONFIG_8xx_WDT 1
/******************************************************************/


/* For our show_cpuinfo hooks. */
#define CPUINFO_VENDOR		"PT Inovacao"
#define CPUINFO_MACHINE		"MPC860 PowerPC"


typedef struct bd_info {
	unsigned long	bi_memstart;	/* start of DRAM memory */
	unsigned long	bi_memsize;	/* size	 of DRAM memory in bytes */
	unsigned long	bi_flashstart;	/* start of FLASH memory */
	unsigned long	bi_flashsize;	/* size	 of FLASH memory */
	unsigned long	bi_flashoffset; /* reserved area for startup monitor */
	unsigned long	bi_sramstart;	/* start of SRAM memory */
	unsigned long	bi_sramsize;	/* size	 of SRAM memory */
	unsigned long	bi_immr_base;	/* base of IMMR register */
	unsigned long	bi_bootflags;	/* boot / reboot flag (for LynxOS) */
	unsigned long	bi_ip_addr;	/* IP Address */
	unsigned char	bi_enetaddr[6];	/* Ethernet adress */
	unsigned short	bi_ethspeed;	/* Ethernet speed in Mbps */
	unsigned long	bi_intfreq;	/* Internal Freq, in MHz */
	unsigned long	bi_busfreq;	/* Bus Freq, in MHz */
	unsigned long	bi_baudrate;	/* Console Baudrate */
	/* second onboard ethernet port */
        unsigned char   bi_enet1addr[6];
} bd_t;

extern bd_t m8xx_board_info;

extern void ptin860t_init(void);
#endif/* !__ASSEMBLY__ */

#endif//__ptin860t_h__ 
