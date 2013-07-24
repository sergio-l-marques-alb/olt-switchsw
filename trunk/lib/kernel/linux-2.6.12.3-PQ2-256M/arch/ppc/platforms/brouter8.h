#ifndef __brouter8_h__
#define __brouter8_h__

#include <linux/types.h>
#include <asm/irq.h>

#define BOOTROM_RESTART_ADDR	((uint)0xfff00100)

#define IMAP_ADDR		((uint)0x0F000000)
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

#define		IDE0_BASE_OFFSET			0x00000000

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

extern void brouter8_init(void);

#endif//__brouter8_h__ 
