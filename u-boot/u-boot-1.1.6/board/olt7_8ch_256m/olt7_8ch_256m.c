/*
 * (C) Copyright 2005-2006
 * José Matos, PT Inovação, jose-v-matos@ptinovacao.pt
 *
 * (C) Copyright 2001-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Modified during 2001 by
 * Advanced Communications Technologies (Australia) Pty. Ltd.
 * Howard Walker, Tuong Vu-Dinh
 *
 * (C) Copyright 2001, Stuart Hughes, Lineo Inc, stuarth@lineo.com
 * Added support for the 16M dram simm on the 8260ads boards
 *
 * (C) Copyright 2003-2004 Arabella Software Ltd.
 * Yuli Barcohen <yuli@arabellasw.com>
 * Added support for SDRAM DIMMs SPD EEPROM, MII, Ethernet PHY init.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <ioports.h>
#include <mpc8260.h>
#include <asm/m8260_pci.h>
#include <pci.h>
#include <i2c.h>
#include <spd.h>
#include <miiphy.h>
#include <watchdog.h>
#include <asm/cpm_8260.h>

/*
 * I/O Port configuration table
 *
 * if conf is 1, then that port pin will be configured at boot time
 * according to the five values podr/pdir/ppar/psor/pdat for that entry
 */

#define CFG_FCC1    1

/*



 */

const iop_conf_t iop_conf_tab[4][32] = {
  {
    //PPARA= 0x33c3fc3f;00110011110000111111110000111111
    //PSORA= 0x03c0003f;00000011110000000000000000111111
    //PDIRA= 0x80043c0c;10000000000001000011110000001100

    /* {conf, ppar, psor, pdir, podr, pdat }  */
    /* PA31 */    
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,1,0,0},
    {1,1,1,1,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,0,1,0,0},
    {1,1,0,1,0,0},
    {1,1,0,1,0,0},
    {1,1,0,1,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,0,0,1,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,1,0,0},


  },

  {
    //	PPARB= 0x0fffcfff;00001111111111111100111111111111
    //	PSORB= 0x0fffcfff;00001111111111111100111111111111
    //	PDIRB= 0x00000000;00000000000000000000000000000000
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},

  },

  {
    //  	PPARC= 0x0001f3ff;00000000000000011111001111111111
    //	PSORC= 0x00000000;00000000000000000000000000000000
    //	PDIRC= 0x00010000;00000000000000010000000000000000
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,1,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
  },

  {
    //	PPARD= 0x08ff5000;00001000111111110101000000000000
    //	PSORD= 0x083f1000;00001000001111110001000000000000
    //	PDIRD= 0x00405000;00000000010000000101000000000000
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,1,1,0,0},
    {1,0,0,0,0,0},
    {1,1,0,1,0,0},
    {1,0,0,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,1,0,0,0},
    {1,1,0,1,0,0},
    {1,1,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0},
    {1,1,1,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},

  },
};


void reset_phy (void)
{
  printf("reset_phy: Nao se liberta o reset do switch\n\r");
  return(0);
  /*retirar bridge de reset */
#define PLD_BRIDGE_5xEther_RESET	0x23
  *(unsigned char*)((CFG_BR2_PRELIM & 0xFFFF8000)+PLD_BRIDGE_5xEther_RESET)= 0x01;
  udelay(1000);    
//	miiphy_write(CFG_PHY_ADDR+0, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
//	miiphy_write(CFG_PHY_ADDR+1, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
  bb_miiphy_write(NULL, CFG_PHY_ADDR+0, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
  bb_miiphy_write(NULL, CFG_PHY_ADDR+1, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
#define MARVEL_PORTCONTROL 0x4
#define MARVEL_PORTCONTROL_FORWARDING 0x0003
//    miiphy_write(CFG_PHY_ADDR+0x8, MARVEL_PORTCONTROL, MARVEL_PORTCONTROL_FORWARDING);/* port control 0 */
//    miiphy_write(CFG_PHY_ADDR+0x9, MARVEL_PORTCONTROL, MARVEL_PORTCONTROL_FORWARDING);/* port control 1 */
//    miiphy_write(CFG_PHY_ADDR+0xd, MARVEL_PORTCONTROL, MARVEL_PORTCONTROL_FORWARDING);/* port control 5 */
  bb_miiphy_write(NULL, CFG_PHY_ADDR+0x8, MARVEL_PORTCONTROL, MARVEL_PORTCONTROL_FORWARDING);/* port control 0 */
  bb_miiphy_write(NULL, CFG_PHY_ADDR+0x9, MARVEL_PORTCONTROL, MARVEL_PORTCONTROL_FORWARDING);/* port control 1 */
  bb_miiphy_write(NULL, CFG_PHY_ADDR+0xd, MARVEL_PORTCONTROL, MARVEL_PORTCONTROL_FORWARDING);/* port control 5 */

}

#define DIR_READ 	    0x08
#define DIR_WRITE       0xF7
#define DATA_READ       0x04
#define DATA_WRITE      0x02
#define DATA_CLK        0x01

// ini claudia
// todas as funcoes comecadas por emilox16 passaram para olt7_8ch
// fim claudia

inline void marvel_mdio_dir_read(int read)
{
  volatile unsigned char *fpga= (volatile unsigned char *)(MDIO_MACHINE_ADDR); 
  if ( read ) {
    (*fpga) |= DIR_READ;
  }
  else {
    (*fpga) &= DIR_WRITE;
  }
}

inline int marvel_mdio_read(void)
{
  volatile unsigned char *fpga= (volatile unsigned char *)(MDIO_MACHINE_ADDR); 
  return((*fpga) & DATA_READ)>>2;
}

inline void marvel_mdio_write(int bit)
{
  volatile unsigned char *fpga= (volatile unsigned char *)(MDIO_MACHINE_ADDR); 
  if ( bit ) (*fpga) |= DATA_WRITE;
  else    (*fpga) &= ~DATA_WRITE;
}

inline void marvel_mdc_write(int bit)
{
  volatile unsigned char *fpga= (volatile unsigned char *)(MDIO_MACHINE_ADDR); 
  if ( bit ) (*fpga) |= DATA_CLK;
  else    (*fpga) &= ~DATA_CLK;
}


#define ns2clk(ns) (ns / (1000000000 / CONFIG_8260_CLKIN) + 1)
static unsigned long UPMBTable[] =
{
  0xffffec00,  0x0ffcec00,  0x0ffcec00,  0x0ffcfc00, //Words 0 to 3
  0x0ffcfc04,  0x0fffec00,  0x0fffec00,  0xffffec01, //Words 4 to 7
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 8 to 11
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 12 to 15
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 16 to 19
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc01, //Words 20 to 23
  0xffffec00,  0x00ffec00,  0x00ffec00,  0x00fffc00, //Words 24 to 27
  0x00fffc00,  0x0fffec00,  0x0fffec00,  0xffffec05, //Words 28 to 31
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 32 to 35
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 36 to 39
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 40 to 43
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc01, //Words 44 to 47
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 48 to 51
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc00, //Words 52 to 55
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc01, //Words 56 to 59
  0xfffffc00,  0xfffffc00,  0xfffffc00,  0xfffffc01  //Words 60 to 63
};

static unsigned long UPMCTable[] = 
{
  0xffffec00,  0x0fffec00,  0x0fffec00,  0x0ffcec00, //Words 0 to 3
  0x0ffcec00,  0x0ffcec00,  0x0ffcec04,  0xffffec01, //Words 4 to 7
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 8 to 11
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 12 to 15
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 16 to 19
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec01, //Words 20 to 23
  0xffffec00,  0x00ffec00,  0x00ffec00,  0x00ffec00, //Words 24 to 27
  0x00ffec00,  0x0fffee00,  0x0fffec00,  0xffffec05, //Words 28 to 31
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 32 to 35
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 36 to 39
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 40 to 43
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec01, //Words 44 to 47
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 48 to 51
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec00, //Words 52 to 55
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec01, //Words 56 to 59
  0xffffec00,  0xffffec00,  0xffffec00,  0xffffec01  //Words 60 to 63
};

void pulsa_reset_compactflash(void)
{
  volatile immap_t *immap = (immap_t *) CFG_IMMR;
  volatile memctl8260_t *memctl = &immap->im_memctl;
  volatile uchar *pld_addr;

  memctl->memc_or2= CFG_OR2_PRELIM;
  memctl->memc_br2= CFG_BR2_PRELIM;

  pld_addr= CFG_BR2_PRELIM & 0xFFFF8000;
  pld_addr[0x22]= 0x0;
  udelay(1000);
  pld_addr[0x22]= 0x1;
  udelay(2);
  pld_addr[0x22]= 0x0;
}

static inline unsigned int dram_test(unsigned int maxmem) 
{
  unsigned int rv= 0;
  int flag= 1; 
  volatile unsigned int *addr;


  flag= 1;
  for ( addr=0; (addr<(void*)maxmem); addr+= 0x100 ) {
    addr[0]= (unsigned int)addr;
    if ( 0 == ((unsigned int)addr % 0x10000) ) {
      WATCHDOG_RESET();
    }
    //\printf("addr:%x\n",addr);
  }
  rv= 0;
  for ( addr=0;flag && (addr<(void*)maxmem); addr+= 0x100 ) {
    flag= (addr[0] == (unsigned int)addr);
    if ( 0 == ((unsigned int)addr % 0x10000) ) {
      WATCHDOG_RESET();
      //              printf(".");
    }
    if ( !flag ) {
      rv = -1;
    }
  }
  //printf("\n");
  return (rv);
}



static inline int dram_size(volatile memctl8260_t *memctl)
{
  volatile uchar *ramaddr;
  unsigned long i, j, rv;
  /* Init 60x bus SDRAM */
  memctl->memc_mptpr=CFG_MPTPR;//MPTPR: Divide Bus clock by 41
  memctl->memc_psrt= CFG_PSRT; //PSRT : Divide MPTPR output by 20

  //WM32      0x0471010c      1111_1100_0000(64 Mbytes) 0000_0(LSDAM) 01(4 banks) 0_100( row start at A8) 1_00(13 row address lines) 0(PMSEL) 1_(IBID interbank interleave = false)  28-31=0 (reserved)
  memctl->memc_or1= CFG_OR1_PRELIM;
  //BR1         10108      0000_0000_0000_0000_0(Base Address) 00(reserved) 1_1(32 Bit) 00(DECC) 0(WP Write Protect) _010(SDRAM 60x) 0(EMEC) _00(ATOM) 0(DR) 1(Valid) 
  memctl->memc_br1= CFG_BR1_PRELIM;//BR1  : SDRAM @0x00000000, 64bit, no parity
  ramaddr = (uchar *) CFG_SDRAM_BASE;

#define PSDMR_PRECHARGE    0x28000000
#define PSDMR_RFEN         0x40000000
#define PSDMR_CBR_REFRESH  0x08000000
#define PSDMR_MODE_PROG    0x18000000

  memctl->memc_psdmr=     CFG_PSDMR_BASE | PSDMR_PRECHARGE;
  *ramaddr= 0xFF;                 //Access SDRAM
  memctl->memc_psdmr=     CFG_PSDMR_BASE | PSDMR_CBR_REFRESH;
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  *ramaddr= 0xFF;                 //Access SDRAM
  memctl->memc_psdmr=     CFG_PSDMR_BASE | PSDMR_MODE_PROG;
  *ramaddr= 0xFF;                 //Access SDRAM
  memctl->memc_psdmr=     CFG_PSDMR_BASE | PSDMR_RFEN;
  *ramaddr= 0xFF;                 //Access SDRAM

  //return 64;

#define MEGABYTE  0x100000
#define TEST_STEP 16

  j= i= rv= 0;
  do {
    j= dram_test( CFG_SDRAM_BASE + MEGABYTE * (i+1) );
    if ( 0==j ) {
      rv= ( i + TEST_STEP )/* *MEGABYTE */;
      printf("%3d\b\b\b" , rv );
    }
    i += TEST_STEP;
  } while ( ( j != -1 ) && ( i < 256 ) );

  return( rv );
}

#if 0
static void serial_smc2_init(void)
{
  smc_uart_t *up;
  volatile smc_t *sp;
  volatile immap_t *im = (immap_t *)CFG_IMMR;
  sp = (smc_t *) &(im->im_smc[1]);
  *(ushort *)(&im->im_dprambase[PROFF_SMC2_BASE]) = PROFF_SMC2;
  up = (smc_uart_t *)&im->im_dprambase[PROFF_SMC2];
  memset(up, 0x00, sizeof(smc_uart_t));
  /* Disable transmitter/receiver.
        */
  sp->smc_smcmr &= ~(SMCMR_REN | SMCMR_TEN);
}
#endif


long int initdram (int board_type)
{
  long int msize = 64;
  volatile immap_t *immap = (immap_t *) CFG_IMMR;
  volatile memctl8260_t *memctl = &immap->im_memctl;
  volatile unsigned long *cardflashaddr;
  volatile unsigned long *fpgaaddr;
  int i;

//	serial_smc2_init();

  immap->im_siu_conf.sc_ppc_acr  = 0x00000002;//60x Bus Arbiter Configuration Register (PPC_ACR)
  immap->im_siu_conf.sc_ppc_alrh = 0x01267893;//Bus arbitration-level registers
  immap->im_siu_conf.sc_tescr1   = 0x00004000;//60x Bus transfer error status and control register 1

  msize= dram_size(memctl);

  /* UPMB CardFlash */
  memctl->memc_mbmr= 0x10000000;//MBMR OP(2-3)=01  write to array pag.466 11.3.5Machine A/B/C Mode Registers (MxMR)
  cardflashaddr= (CFG_BR4_PRELIM & 0xFFF80000);
  for ( i=0; i<64; ++i ) {
    memctl->memc_mdr = UPMBTable[i];//11.3.6 Memory Data Register (MDR)
    *cardflashaddr= 0x10000000+i;
  }
  memctl->memc_mbmr= 0x00040000;

  /* UPMC FPGA */
  memctl->memc_mcmr= 0x10000000;//MBMR OP(2-3)=01  write to array pag.466 11.3.5Machine A/B/C Mode Registers (MxMR)
  fpgaaddr= (CFG_BR6_PRELIM & 0xFFF80000);
  for ( i=0; i<64; ++i ) {
    memctl->memc_mdr = UPMCTable[i];//11.3.6 Memory Data Register (MDR)
    *fpgaaddr= 0x10000000+i;
  }
  memctl->memc_mcmr= 0x00040000;

//BCTLx 
  immap->im_siu_conf.sc_siumcr|= 0x00050000; //CS10PC(12-13):/BCTL1 =01 ; BCTLC=01 /BCTL0= /WR e /BCTL1=/OE  
//    immap->im_clkrst.car_sccr |= M826X_SCCR_PCI_MODE_EN;
//    immap->im_siu_conf.sc_siumcr =
//        (immap->im_siu_conf.sc_siumcr & ~SIUMCR_LBPC11)
//        | SIUMCR_LBPC01;

  /* return total 60x bus SDRAM size */
  return(msize * 1024 * 1024);
}

int checkboard (void)
{
  puts ("BOARD: OLT7-8CH - Portugal Telecom Inovacao, SA\n");
  return (0);
}

#if 0
unsigned char olt7_8ch_brg_map[] = {
  0,      /* BRG1 for SMC1 */
  1,      /* BRG2 for SMC2 */
  4,      /* BRGx for SCC1 */
  5,      /* BRGx for SCC2 */
  6,      /* BRGx for SCC3 */
  7,      /* BRGx for SCC4 */
};
#endif


#ifdef	CONFIG_PCI
struct pci_controller hose;

extern void pci_mpc8250_init(struct pci_controller *);

void pci_init_board(void)
{
  pci_mpc8250_init(&hose);
}
#endif

#if defined(CONFIG_SHOW_BOOT_PROGRESS)
void show_boot_progress(int progress)
{
  printf("Boot reached stage %d\n", progress);
}
#endif

