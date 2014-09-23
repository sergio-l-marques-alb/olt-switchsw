#ifndef __pq2_ptin_h__
#define __pq2_ptin_h__

#include <linux/types.h>
#include <asm/irq.h>


#define RTC_I2C_DS1339 //suporte para I2C externo

#ifdef CONFIG_PCI

#define CONFIG_PCI_PQ2_PTIN

#define SOFT_PCI_RESET          0x00000001

//#define PCI_VENDOR_ID_BROADCOM 0x14E4
#define	PCI_DEVICE_ID_BCM56803 0xB80314E4


typedef enum{
    e_VendorId = 0x00,  
	e_DevId = 0x02, 
	e_PciCommand = 0x04, 
	e_PciStatus = 0x06, 
	e_RevId = 0x08, 
	e_ProgIntf = 0x09, 
	e_SubclassCode = 0xa, 
	e_BaseClass = 0x0b, 
	e_CacheLine = 0x0c, 
	e_LatencyTimer = 0x0d, 
	e_HeaderType = 0x0e, 
	e_BISTControl = 0x0f, 
	e_BaseAddrLow = 0x10, 
    e_BaseAddrHigh = 0x14, 
	e_SubsystemVendorId = 0x2c, 
	e_SubsystemId = 0x2e, 
	e_InterruptLine = 0x3c, 
	e_InterruptPin = 0x3d, 
	e_MinGnt = 0x3e, 
	e_MaxLat = 0x3f, 
    e_TrdyTimeout = 0x40, 
    e_Retry = 0x41,    
	} e_CfgReg;


/* PCI interrupt controller */
#define PCI_INT_STAT_REG	0xF8200000
#define PCI_INT_MASK_REG	0xF8200004
#define PIRQA			(NR_CPM_INTS + 0)
#define PIRQB			(NR_CPM_INTS + 1)
#define PIRQC			(NR_CPM_INTS + 2)
#define PIRQD			(NR_CPM_INTS + 3)

/*
 * PCI memory map definitions for MPC8266ADS-PCI.
 *
 * processor view
 *	local address		PCI address		target
 *	0x80000000-0x9FFFFFFF	0x80000000-0x9FFFFFFF	PCI mem with prefetch
 *	0xA0000000-0xBFFFFFFF	0xA0000000-0xBFFFFFFF	PCI mem w/o prefetch
 *	0xF4000000-0xF7FFFFFF	0x00000000-0x03FFFFFF	PCI IO
 *
 * PCI master view
 *	local address		PCI address		target
 *	0x00000000-0x1FFFFFFF	0x00000000-0x1FFFFFFF	MPC8266 local memory
 */

/* All the other PCI memory map definitions reside at syslib/m82xx_pci.h
   Here we should redefine what is unique for this board */
   /*
#define M82xx_PCI_SLAVE_MEM_LOCAL	0x00000000	
#define M82xx_PCI_SLAVE_MEM_BUS		0x00000000	
#define M82xx_PCI_SLAVE_MEM_SIZE	0x10000000	

#define M82xx_PCI_SLAVE_SEC_WND_SIZE	~(0x40000000 - 1U)
#define M82xx_PCI_SLAVE_SEC_WND_BASE	0x80000000		
*/
#define PCI_INT_TO_SIU 	SIU_INT_IRQ6

#endif /* CONFIG_PCI */


#define BOOTROM_RESTART_ADDR	((uint)0xfff00100)

#define IMAP_ADDR		((uint)0xFF000000)
#define CPM_MAP_ADDR            (IMAP_ADDR+KERNELBASE)

// CONFIG_MTD_PHYSMAP_START,
//        .size = CONFIG_MTD_PHYSMAP_LEN,

/* For our show_cpuinfo hooks. */
#define CPUINFO_VENDOR		"PT Inovacao"
#define CPUINFO_MACHINE		"MPC8280 PowerPC"

/******************************************************************/
/*IDE & PCMCIA                                                    */
/******************************************************************/
#define MAX_HWIFS               1

#define                 CFG_ATA_BASE_ADDR  		0x60C00000

#define		        IDE0_BASE_OFFSET			0x00000000

#define         	IDE0_DATA_REG_OFFSET    (8)
#define                 IDE0_ERROR_REG_OFFSET		(1)
#define                 IDE0_NSECTOR_REG_OFFSET		(2)
#define                 IDE0_SECTOR_REG_OFFSET		(3)
#define                 IDE0_LCYL_REG_OFFSET		(4)
#define                 IDE0_HCYL_REG_OFFSET		(5)
#define                 IDE0_SELECT_REG_OFFSET		(6)
#define                 IDE0_STATUS_REG_OFFSET		(7)
#define                 IDE0_CONTROL_REG_OFFSET		(0x0e)
#define                 IDE0_IRQ_REG_OFFSET		(0xa) /*??*/

#define             IDE0_INTERRUPT		(SIU_INT_IRQ1)

#define PTIN_IDE_EXTRA_TIMEOUT (HZ/2) /* To deal with the 64 MB Transcend */
                                  /* CompactFlash */

/*IDE & PCMCIA FIM*************************************************/

#define CONFIG_SERIAL_CPM_ALT_SMC2 1


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
	unsigned long	bi_cpmfreq;	/* CPM_CLK Freq, in MHz */
	unsigned long	bi_brgfreq;	/* BRG_CLK Freq, in MHz */
	unsigned long	bi_sccfreq;	/* SCC_CLK Freq, in MHz */
	unsigned long	bi_vco;		/* VCO Out from PLL, in MHz */
	unsigned long	bi_baudrate;	/* Console Baudrate */
} bd_t;

extern bd_t m8xx_board_info;

extern void pq2_ptin_init(void);




#endif//__pq2_ptin_h__ 
