/*
 * Copyright 2011-2012 Freescale Semiconductor, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * P2041 RDB board configuration file
 * Also supports P2040 RDB
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_P2041RDB
#define CONFIG_PHYS_64BIT 
#define CONFIG_PPC_P2041
#define CONFIG_CMD_REGINFO

/* High Level Configuration Options */
#define CONFIG_BOOKE
#define CONFIG_E500			/* BOOKE e500 family */
#define CONFIG_E500MC			/* BOOKE e500mc family */
#define CONFIG_SYS_BOOK3E_HV		/* Category E.HV supported */
#define CONFIG_MPC85xx			/* MPC85xx/PQ3 platform */
#define CONFIG_FSL_CORENET		/* Freescale CoreNet platform */
#define CONFIG_MP			/* support multiple processors */

#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE	0xe9f80000 /*0xeff80000 cxo64o  0xE8000000 - 0xE9FFFFFF*/
#endif

#ifndef CONFIG_RESET_VECTOR_ADDRESS
#define CONFIG_RESET_VECTOR_ADDRESS	0xe9fffffc /*0xeffffffc cxo64o  0xE8000000 - 0xE9FFFFFF*/
#endif

#define CONFIG_SYS_FSL_CPC		/* Corenet Platform Cache */
#define CONFIG_SYS_NUM_CPC		CONFIG_NUM_DDR_CONTROLLERS
#define CONFIG_FSL_ELBC			/* Has Enhanced localbus controller */
#define CONFIG_PCI			/* Enable PCI/PCIE */
/*#define CONFIG_PCIE1			/* PCIE controler 1 */
#define CONFIG_PCIE2			/* PCIE controler 2 */
#define CONFIG_PCIE3			/* PCIE controler 3 */
#define CONFIG_FSL_PCI_INIT		/* Use common FSL init code */
/*#define CONFIG_SYS_PCI_64BIT		/* enable 64-bit PCI resources */

/*#define CONFIG_SYS_SRIO*/
/*#define CONFIG_SRIO1			/* SRIO port 1 */
/*#define CONFIG_SRIO2			/* SRIO port 2 *//*
/*#define CONFIG_SRIO_PCIE_BOOT_MASTER*/
#define CONFIG_SYS_DPAA_RMAN		/* RMan */

#define CONFIG_FSL_LAW			/* Use common FSL init code */

#define CONFIG_ENV_OVERWRITE


#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE


	#define CONFIG_ENV_IS_IN_FLASH
	#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE \
			- CONFIG_ENV_SECT_SIZE)
	#define CONFIG_ENV_SIZE		0x2000
	#define CONFIG_ENV_SECT_SIZE	0x20000 /* 128K (one sector) */


#ifndef __ASSEMBLY__
unsigned long get_board_sys_clk(unsigned long dummy);
#endif
#define CONFIG_SYS_CLK_FREQ	get_board_sys_clk(0)

/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_SYS_CACHE_STASHING
#define CONFIG_BACKSIDE_L2_CACHE
#define CONFIG_SYS_INIT_L2CSR0		L2CSR0_L2E
#define CONFIG_BTB			/* toggle branch predition */

#define CONFIG_ENABLE_36BIT_PHYS

#ifdef CONFIG_PHYS_64BIT                                                     
#define CONFIG_ADDR_MAP                                                      
#define CONFIG_SYS_NUM_ADDR_MAP		64	/* number of TLB1 entries */        
#endif            

/* #define CONFIG_POST CONFIG_SYS_POST_MEMORY	 test POST memory test */ 
#define CONFIG_SYS_MEMTEST_START	0x00200000	 /*memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x00400000
/*#define CONFIG_SYS_ALT_MEMTEST */
#define CONFIG_PANIC_HANG	/* do not reset board on panic */

/*
 *  Config the L3 Cache as L3 SRAM
 */
#define CONFIG_SYS_INIT_L3_ADDR		CONFIG_RAMBOOT_TEXT_BASE
/* #ifdef CONFIG_PHYS_64BIT                                   HUGO */
/* #define CONFIG_SYS_INIT_L3_ADDR_PHYS	(0xf00000000ull | \   HUGO */
/* 		CONFIG_RAMBOOT_TEXT_BASE)                             HUGO */
/* #else                                                      HUGO */
#define CONFIG_SYS_INIT_L3_ADDR_PHYS	CONFIG_SYS_INIT_L3_ADDR
/*#endif*/
#define CONFIG_SYS_L3_SIZE		(1024 << 10)
#define CONFIG_SYS_INIT_L3_END (CONFIG_SYS_INIT_L3_ADDR + CONFIG_SYS_L3_SIZE)

#ifdef CONFIG_PHYS_64BIT                           
#define CONFIG_SYS_DCSRBAR		0xf0000000         
#define CONFIG_SYS_DCSRBAR_PHYS		0xf00000000ull 
#endif                                             



/*
 * DDR Setup
 */

#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

 #define CONFIG_DIMM_SLOTS_PER_CTLR	1
 #define CONFIG_CHIP_SELECTS_PER_CTRL	4 /*(4 * CONFIG_DIMM_SLOTS_PER_CTLR)*/

/*#define CONFIG_DDR_SPD*/
#define CONFIG_FSL_DDR3

/*#define CONFIG_SYS_SPD_BUS_NUM	0 */
/*#define SPD_EEPROM_ADDRESS	0x52 */
#define CONFIG_SYS_SDRAM_SIZE	2048	/* for fixed parameter use */

/*
 * Local Bus Definitions
 */

/* Set the local bus clock 1/8 of platform clock */
#define CONFIG_SYS_LBC_LCRR		LCRR_CLKDIV_8

/*
 * This board doesn't have a promjet connector.
 * However, it uses commone corenet board LAW and TLB.
 * It is necessary to use the same start address with proper offset.
 */
#define CONFIG_SYS_FLASH_BASE		0xe0000000

#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE


#define CONFIG_SYS_FLASH_BR_PRELIM \
		(BR_PHYS_ADDR((CONFIG_SYS_FLASH_BASE_PHYS + 0x8000000)) | \
		BR_PS_16 | BR_V)
#define CONFIG_SYS_FLASH_OR_PRELIM \
		((0xf8000ff7 & ~OR_GPCM_SCY & ~OR_GPCM_EHTR) \
		 | OR_GPCM_SCY_8 | OR_GPCM_EHTR_CLEAR)


#define CONFIG_FSL_CPLD


#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS	45 /* count down from 45/5: 9..1 */

#define CONFIG_SYS_MAX_FLASH_BANKS	1		/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	256     /*1024 cxo64o  0xE8000000 - 0xE9FFFFFF sectors per device */ 
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000		/* Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500		/* Write Timeout (ms) */

#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE

#if defined(CONFIG_RAMBOOT_PBL)
#define CONFIG_SYS_RAMBOOT
#endif



#define CONFIG_SYS_BR0_PRELIM  CONFIG_SYS_FLASH_BR_PRELIM /* NOR Base Address */
#define CONFIG_SYS_OR0_PRELIM  CONFIG_SYS_FLASH_OR_PRELIM /* NOR Options */


#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_SYS_FLASH_AMD_CHECK_DQ7
#define CONFIG_SYS_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE_PHYS + 0x8000000}

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_EARLY_INIT_R	/* call board_early_init_r function */
#define CONFIG_MISC_INIT_R

#define CONFIG_HWCONFIG

/* define to use L1 as initial stack */
#define CONFIG_L1_INIT_RAM
#define CONFIG_SYS_INIT_RAM_LOCK
#define CONFIG_SYS_INIT_RAM_ADDR	0xffd00000	/* Initial L1 address */

#define CONFIG_SYS_INIT_RAM_ADDR_PHYS	CONFIG_SYS_INIT_RAM_ADDR
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS_HIGH 0
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS_LOW CONFIG_SYS_INIT_RAM_ADDR_PHYS

#define CONFIG_SYS_INIT_RAM_SIZE	0x00004000

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - \
					GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(1024 * 1024)

/* Serial Port - controlled on board with jumper J8
 * open - index 2
 * shorted - index 1
 */
#define CONFIG_CONS_INDEX	1
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		(get_bus_freq(0)/2)

#define CONFIG_SYS_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_CCSRBAR+0x11C500)
#define CONFIG_SYS_NS16550_COM2	(CONFIG_SYS_CCSRBAR+0x11C600)
#define CONFIG_SYS_NS16550_COM3	(CONFIG_SYS_CCSRBAR+0x11D500)
#define CONFIG_SYS_NS16550_COM4	(CONFIG_SYS_CCSRBAR+0x11D600)

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER

/* pass open firmware flat tree */
#define CONFIG_OF_LIBFDT
#define CONFIG_OF_BOARD_SETUP
#define CONFIG_OF_STDOUT_VIA_ALIAS

/* new uImage format support */
#define CONFIG_FIT
#define CONFIG_FIT_VERBOSE	/* enable fit_format_{error,warning}() */

/* I2C */
#define CONFIG_FSL_I2C		/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C		/* I2C with hardware support */
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_I2C_CMD_TREE
#define CONFIG_SYS_I2C_SPEED		400000
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_OFFSET		0x118000
#define CONFIG_SYS_I2C2_OFFSET		0x118100


/*
 * General PCI
 * Memory space is mapped 1-1, but I/O space must start from 0.
 */

      
/*###########################ASSUCATADO PARA TESTAR ENDERECAMENTO EQUIVALENTE A TA48GE  #####################*/
/* controller 2, Slot 2, tgtid 2, Base address 201000 */
#define CONFIG_SYS_PCIE2_MEM_VIRT	0xa0000000
#define CONFIG_SYS_PCIE2_MEM_BUS	0xa0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS	0xa0000000
#define CONFIG_SYS_PCIE2_MEM_SIZE	0x08000000	/*  */
#define CONFIG_SYS_PCIE2_IO_VIRT	0xf8010000
#define CONFIG_SYS_PCIE2_IO_BUS		0x00000000
#define CONFIG_SYS_PCIE2_IO_PHYS	0xf8010000
#define CONFIG_SYS_PCIE2_IO_SIZE	0x00010000	/* 64k */
/*#######################FIM DO  ASSUCATANÇO PARA TESTAR ENDERECAMENTO EQUIVALENTE A TA48GE  ######################*/

/* controller 3, Slot 1, tgtid 1, Base address 202000 */
#define CONFIG_SYS_PCIE3_MEM_VIRT	0xc0000000
#define CONFIG_SYS_PCIE3_MEM_BUS	0xc0000000
#define CONFIG_SYS_PCIE3_MEM_PHYS	0xc0000000
#define CONFIG_SYS_PCIE3_MEM_SIZE	0x20000000	/* 512M */
#define CONFIG_SYS_PCIE3_IO_VIRT	0xf8020000
#define CONFIG_SYS_PCIE3_IO_BUS		0x00000000
#define CONFIG_SYS_PCIE3_IO_PHYS	0xf8020000
#define CONFIG_SYS_PCIE3_IO_SIZE	0x00010000	/* 64k */

/* Qman/Bman */
#define CONFIG_SYS_DPAA_QBMAN		/* Support Q/Bman */
#define CONFIG_SYS_BMAN_NUM_PORTALS	10
#define CONFIG_SYS_BMAN_MEM_BASE	0xf4000000
#define CONFIG_SYS_BMAN_MEM_PHYS	CONFIG_SYS_BMAN_MEM_BASE
#define CONFIG_SYS_BMAN_MEM_SIZE	0x00200000
#define CONFIG_SYS_QMAN_NUM_PORTALS	10
#define CONFIG_SYS_QMAN_MEM_BASE	0xf4200000
#define CONFIG_SYS_QMAN_MEM_PHYS	CONFIG_SYS_QMAN_MEM_BASE
#define CONFIG_SYS_QMAN_MEM_SIZE	0x00200000

#define CONFIG_SYS_DPAA_FMAN
#define CONFIG_SYS_DPAA_PME
/* Default address of microcode for the Linux Fman driver */
#define CONFIG_SYS_QE_FMAN_FW_IN_NOR
#define CONFIG_SYS_QE_FMAN_FW_ADDR	0xe9f40000 /*0xEFF40000 cxo640*/

#define CONFIG_SYS_QE_FMAN_FW_LENGTH	0x10000
#define CONFIG_SYS_FDT_PAD		(0x3000 + CONFIG_SYS_QE_FMAN_FW_LENGTH)

#ifdef CONFIG_SYS_DPAA_FMAN
#define CONFIG_FMAN_ENET
#define CONFIG_PHY_REALTEK
#endif

#ifdef CONFIG_PCI
#define CONFIG_PCI_PNP			/* do pci plug-and-play */
/*#define CONFIG_E1000 HUGO */

#define CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup */
#define CONFIG_DOS_PARTITION
#endif	/* CONFIG_PCI */



#ifdef CONFIG_FMAN_ENET
#define CONFIG_SYS_FM1_DTSEC1_PHY_ADDR	0x1 
#define CONFIG_SYS_FM1_DTSEC2_PHY_ADDR	0x4  /*era 0x2   HUGO - sgmii lane C para o Marvell 88e6095*/
#define CONFIG_SYS_FM1_DTSEC3_PHY_ADDR	0x5
#define CONFIG_SYS_FM1_DTSEC4_PHY_ADDR	0x6
#define CONFIG_SYS_FM1_DTSEC5_PHY_ADDR	0x7

#define CONFIG_SYS_FM1_DTSEC1_RISER_PHY_ADDR	0x1c
#define CONFIG_SYS_FM1_DTSEC2_RISER_PHY_ADDR	0x1d
#define CONFIG_SYS_FM1_DTSEC3_RISER_PHY_ADDR	0x1e
#define CONFIG_SYS_FM1_DTSEC4_RISER_PHY_ADDR	0x1f

#define CONFIG_SYS_FM1_10GEC1_PHY_ADDR	0

#define CONFIG_SYS_TBIPA_VALUE	8
#define CONFIG_MII		/* MII PHY management */
#define CONFIG_ETHPRIME		"FM1@DTSEC1"
#define CONFIG_PHY_GIGE		/* Include GbE speed/duplex detection */
#endif

/*
 * Environment
 */
#define CONFIG_LOADS_ECHO		/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	/* allow baudrate change */

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_ERRATA
#define CONFIG_CMD_GREPENV
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_SETEXPR

#ifdef CONFIG_PCI
#define CONFIG_CMD_PCI
#define CONFIG_CMD_NET
#endif


#define CONFIG_CMD_EXT2

#define CONFIG_MMC

#ifdef CONFIG_MMC
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR       CONFIG_SYS_MPC85xx_ESDHC_ADDR
#define CONFIG_SYS_FSL_ESDHC_BROKEN_TIMEOUT
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory	*/
#define CONFIG_CMDLINE_EDITING			/* Command-line editing */
#define CONFIG_AUTO_COMPLETE			/* add autocompletion support */
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */
#ifdef CONFIG_CMD_KGDB
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */
#endif
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE
#define CONFIG_SYS_HZ		1000		/* decrementer freq 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(64 << 20)	/* Initial Memory for Linux */
#define CONFIG_SYS_BOOTM_LEN	(64 << 20)	/* Increase max gunzip size */

#ifdef CONFIG_CMD_KGDB
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

/*
 * Environment Configuration
 */
#define CONFIG_ROOTPATH		"/opt/nfsroot"
#define CONFIG_BOOTFILE		"uImage"
#define CONFIG_UBOOTPATH	u-boot.bin

/* default location for tftp and bootm */
#define CONFIG_LOADADDR		1000000

#define CONFIG_BOOTDELAY	3	/* -1 disables auto-boot */

#define CONFIG_BAUDRATE	115200

#define __USB_PHY_TYPE	utmi



#define	CONFIG_EXTRA_ENV_SETTINGS\
	"hwconfig=fsl_ddr:ctlr_intlv=cacheline,"\
	"bank_intlv=cs0_cs1\0"\
	"netdev=eth0\0"\
	"uboot=" __stringify(CONFIG_UBOOTPATH) "\0"\
	"ubootaddr=" __stringify(CONFIG_SYS_TEXT_BASE) "\0"\
	"consoled=ttyS0\0"\
    "baudrate=115200\0"\
    "fdtaddr=c00000\0"\
    "uImageaddr=1000000\0"\
	"fdtfile=/cxo640.dtb\0"\
    "uImagefile=/cxo640.z\0"			


#define CONFIG_SDBOOT\
    "mmcinfo; fatload mmc 0:1 1000000 $uImagefile;  fatload mmc 0:1 $fdtaddr $fdtfile;"\
	"bootm $uImageaddr - $fdtaddr"

#define CONFIG_BOOTARGS "root=/dev/mmcblk0p2 rw ip=off rootfstype=ext3 rootdelay=5 console=ttyS0,115200"



#define CONFIG_BOOTCOMMAND		CONFIG_SDBOOT

#ifdef CONFIG_SECURE_BOOT
#include <asm/fsl_secure_boot.h>
#endif

#endif	/* __CONFIG_H */
