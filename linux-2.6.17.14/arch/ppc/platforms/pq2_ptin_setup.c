/*
 * arch/ppc/platforms/pq2_ptin_setup.c
 *
 * BROUTER8 platform support
 *
 * Author: Jose Pedro Matos <jose-v-matos@ptinovacao.pt>
 * Derived from: m8260_setup.c by Dan Malek, MVista
 *
 * Copyright 2005 PTInovação
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/config.h>
#ifndef CONFIG_PQ2_PTIN
#error "XPTO!!"
#endif

#include <linux/seq_file.h>

#include <asm/mpc8260.h>
#include <asm/machdep.h>
#include <asm/immap_cpm2.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <linux/ide.h>
#include <linux/delay.h>
#include <linux/mtd/physmap.h>

#ifdef RTC_I2C_DS1339
#include <asm/time.h>
#endif

extern void pq2_find_bridges(void);

static void (*callback_setup_arch)(void);

extern unsigned char __res[sizeof(bd_t)];

unsigned int pq2_ptin_bootmem_buffer = (unsigned int)-1;




//******************************************************************************
//******************************************************************************
#ifdef RTC_I2C_DS1339
//******************************************************************************
//******************************************************************************

  #define DS1339_ADDR     0x68

  #define DS1339_SECONDS  0x00
  #define DS1339_MINUTES  0x01
  #define DS1339_HOURS    0x02
  #define DS1339_DAY      0x03
  #define DS1339_DATE     0x04
  #define DS1339_MONTH    0x05
  #define DS1339_YEAR     0x06

  #define I2C_RXTX_LEN    128 // maximum tx/rx buffer length

//------------------------------------------------------------------------------
/*
  #define I2C_DIR0        cpm2_immr->im_cpm.cp_pbdir
  #define I2C_DAT0        cpm2_immr->im_cpm.cp_pbdat
  #define I2C_SCL0        0x00000020
  #define I2C_SDA0        0x00000010
*/

  #define I2C_DIR0        cpm2_immr->im_ioport.iop_pdird
  #define I2C_DAT0        cpm2_immr->im_ioport.iop_pdatd
  #define I2C_SCL0        0x00020000
  #define I2C_SDA0        0x00010000


//------------------------------------------------------------------------------

// (Optional). Any commands necessary to enable the I2C controller or configure ports.
// #define I2C_INIT (immr->im_cpm.cp_pbdir |= PB_SCL)
  #define I2C_INIT     (I2C_DIR0 |= I2C_SCL0)

// The code necessary to make the I2C data line active (driven).  If the data line
// is open collector, this define can be null.
// #define I2C_ACTIVE (immr->im_cpm.cp_pbdir |=  PB_SDA)
  #define I2C_ACTIVE   ((I2C_DIR0) |= I2C_SDA0)

// The code necessary to make the I2C data line tri-stated (inactive).  If the data
// line is open collector, this define can be null.
// #define I2C_TRISTATE (immr->im_cpm.cp_pbdir &= ~PB_SDA)
  #define I2C_TRISTATE ((I2C_DIR0) &= ~I2C_SDA0)

// Code that returns TRUE if the I2C data line is high, FALSE if it is low.
// #define I2C_READ ((immr->im_cpm.cp_pbdat & PB_SDA) != 0)
  #define I2C_READ     (((I2C_DAT0) &= I2C_SDA0) != 0)

// If <bit> is TRUE, sets the I2C data line high. If it is FALSE, it clears it (low).
// #define I2C_SDA(bit) /
//           if(bit) immr->im_cpm.cp_pbdat |=  PB_SDA; /
//           else    immr->im_cpm.cp_pbdat &= ~PB_SDA
  #define I2C_SDA(bit)  if(bit) (I2C_DAT0) |=  I2C_SDA0;\
                        else    (I2C_DAT0) &= ~I2C_SDA0

// If <bit> is TRUE, sets the I2C clock line high. If it is FALSE, it clears it (low).
// #define I2C_SCL(bit) /
//           if(bit) immr->im_cpm.cp_pbdat |=  PB_SCL; /
//           else    immr->im_cpm.cp_pbdat &= ~PB_SCL
  #define I2C_SCL(bit)  if(bit) (I2C_DAT0) |=  I2C_SCL0;\
                        else    (I2C_DAT0) &= ~I2C_SCL0

// This delay is invoked four times per clock cycle so this controls the rate of
// data transfer.
  #define I2C_SPEED        20

//------------------------------------------------------------------------------

  #define I2C_ACK		   0            // PD_SDA level to ack a byte
  #define I2C_NOACK	       1            // PD_SDA level to noack a byte

  #define I2C_DELAY          { for ( _i2c_sleep_ = 0 ; _i2c_sleep_ < I2C_SPEED ; _i2c_sleep_++ ); }

static int _i2c_sleep_;

  #define BCD2DEC(x)    (((x) & 0x000F)+(((x) & 0x00F0) >> 4)*10)

  #define DEC2BCD(x)    ((((x)/10)<<4)|((x) % 10))

//******************************************************************************
// Local functions
//******************************************************************************

static void          send_reset(void);
static void          send_start(void);
static void          send_stop(void);
static void          send_ack(int);
static int           write_byte(unsigned char byte);
static unsigned char read_byte(int);

static void          i2c_init(void);
static int           i2c_probe(unsigned char chip);
static int           i2c_read(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len);
static int           i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len);
static unsigned long pq2ptin_get_rtc_time(void);

//******************************************************************************
// Send a reset sequence consisting of 9 clocks with the data signal high
// to clock any confused device back into an idle state.  Also send a
// <stop> at the end of the sequence for belts & suspenders.
//******************************************************************************

static void
send_reset(void)
{
  int j;

  I2C_SCL(1);
  I2C_SDA(1);
  I2C_INIT;
  I2C_TRISTATE;
  for ( j = 0 ; j < 9 ; j++ ) {
    I2C_SCL(0);
    I2C_DELAY;
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    I2C_DELAY;
  }
  send_stop();
  I2C_TRISTATE;
}

//******************************************************************************
// START: High -> Low on SDA while SCL is High
//******************************************************************************

static void
send_start(void)
{
  I2C_DELAY;
  I2C_SDA(1);
  I2C_ACTIVE;
  I2C_DELAY;
  I2C_SCL(1);
  I2C_DELAY;
  I2C_SDA(0);
  I2C_DELAY;
}

//******************************************************************************
// STOP: Low -> High on SDA while SCL is High
//******************************************************************************

static void
send_stop(void)
{
  I2C_SCL(0);
  I2C_DELAY;
  I2C_SDA(0);
  I2C_ACTIVE;
  I2C_DELAY;
  I2C_SCL(1);
  I2C_DELAY;
  I2C_SDA(1);
  I2C_DELAY;
  I2C_TRISTATE;
}

//******************************************************************************
// ack should be I2C_ACK or I2C_NOACK
//******************************************************************************

static void
send_ack(int ack)
{
  I2C_ACTIVE;
  I2C_SCL(0);
  I2C_DELAY;

  I2C_SDA(ack);

  I2C_ACTIVE;
  I2C_DELAY;
  I2C_SCL(1);
  I2C_DELAY;
  I2C_DELAY;
  I2C_SCL(0);
  I2C_DELAY;
}

//******************************************************************************
// Send 8 bits and look for an acknowledgement.
//******************************************************************************

static int
write_byte(unsigned char data)
{
  int j;
  int nack;

  I2C_ACTIVE;
  for ( j = 0; j < 8; j++ ) {
    I2C_SCL(0);
    I2C_DELAY;
    I2C_SDA(data & 0x80);
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    I2C_DELAY;

    data <<= 1;
  }

  // Look for an <ACK>(negative logic) and return it.

  I2C_SCL(0);
  I2C_DELAY;
  I2C_SDA(1);
  I2C_TRISTATE;
  I2C_DELAY;
  I2C_SCL(1);
  I2C_DELAY;
  I2C_DELAY;
  nack = I2C_READ;
  I2C_SCL(0);
  I2C_DELAY;
  I2C_ACTIVE;

  return(nack); // not a nack is an ack
}

//******************************************************************************
// if ack == I2C_ACK, ACK the byte so can continue reading, else
// send I2C_NOACK to end the read.
//******************************************************************************

static unsigned char
read_byte(int ack)
{
  int  data;
  int  j;

  // Read 8 bits, MSB first.
  I2C_TRISTATE;
  data = 0;
  for ( j = 0; j < 8; j++ ) {
    I2C_SCL(0);
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    data <<= 1;
    data |= I2C_READ;
    I2C_DELAY;
  }
  send_ack(ack);

  return(data);
}

//******************************************************************************
// Initialization
//******************************************************************************


void
i2c_init(void)
{
  send_reset();  
}

//******************************************************************************
// Probe to see if a chip is present.
//******************************************************************************

int
i2c_probe(unsigned char addr)
{
  int rc;

  // Perform 1 byte read transaction
  send_start();
  rc = write_byte ((addr << 1) | 0);
  send_stop();

  return(rc ? 1 : 0);
}


//******************************************************************************
// Read bytes
//******************************************************************************

int
i2c_read(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
  int shift;
  
  // Do the addressing portion of a write cycle to set the
  // chip's address pointer.  If the address length is zero,
  // don't do the normal write cycle to set the address pointer,
  // there is no address pointer in this chip.
  send_start();
  if ( alen > 0 ) {
    if ( write_byte(chip << 1) ) { // write cycle
      send_stop();
      return(1);
    }
    shift = (alen-1) * 8;
    while ( alen-- > 0 ) {
      if ( write_byte(addr >> shift) ) {
        return(1);
      }
      shift -= 8;
    }
    send_stop();  // reportedly some chips need a full stop
    send_start();
  }

  // Send the chip address again, this time for a read cycle.
  // Then read the data.  On the last byte, we do a NACK instead
  // of an ACK(len == 0) to terminate the read.

  write_byte((chip << 1) | 1);  // read cycle
  while ( len-- > 0 ) {
    *buffer++ = read_byte(len == 0);
  }
  send_stop();
  return(0);
}

//******************************************************************************
// Write bytes
//******************************************************************************

int
i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
  int shift, failures = 0;

  send_start();
  if ( write_byte(chip << 1) ) { // write cycle
    send_stop();
    return(1);
  }
  shift = (alen-1) * 8;
  while ( alen-- > 0 ) {
    if ( write_byte(addr >> shift) ) {
      return(1);
    }
    shift -= 8;
  }

  while ( len-- > 0 ) {
    if ( write_byte(*buffer++) ) {
      failures++;
    }
  }
  send_stop();
  return(failures);
}

//******************************************************************************
//******************************************************************************
#endif //RTC
//******************************************************************************
//******************************************************************************





static int
pq2_ptin_show_cpuinfo(struct seq_file *m)
{
    /*
	bd_t	*binfo = (bd_t *)__res;

	seq_printf(m, "vendor\t\t: PT Inovação \n"
		      "machine\t\t: BROUTER8\n"
		      "bi_brgfreq\t\t: %d\n",
		      binfo->bi_brgfreq);
	return 0;
    */

	uint pvid, svid, phid1;
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	pvid = mfspr(SPRN_PVR);
	svid = mfspr(SPRN_SVR);

	seq_printf(m, "Vendor\t\t: Freescale Inc.\n");
	seq_printf(m, "core clock\t: %d MHz\n"
			"bus  clock\t: %d MHz\n",
			(int)(binfo->bi_intfreq / 1000000),
			(int)(binfo->bi_busfreq / 1000000));
	seq_printf(m, "PVR\t\t: 0x%x\n", pvid);
	seq_printf(m, "SVR\t\t: 0x%x\n", svid);

	/* Display cpu Pll setting */
	phid1 = mfspr(SPRN_HID1);
	seq_printf(m, "PLL setting\t: 0x%x\n", ((phid1 >> 24) & 0x3f));

	/* Display the amount of memory */
	seq_printf(m, "Memory\t\t: %d MB\n", (int)(binfo->bi_memsize / (1024 * 1024)));

    return(0);
}


static void __init
pq2_ptin_setup_arch(void)
{
	printk("PQ2_PTIN Port\n");
	callback_setup_arch();
}
/* Map the IMMR, plus anything else we can cover
 * in that upper space according to the memory controller
 * chip select mapping.  Grab another bunch of space
 * below that for stuff we can't cover in the upper.
 */
static void __init
pq2_ptin_m8260_map_io(void)
{
	bd_t    *binfo = (bd_t *)__res;

	/* Map IMMR region to a 256k BAT */
	io_block_mapping(CPM_MAP_ADDR/*virt*/, IMAP_ADDR/*phys*/, 0x40000, _PAGE_IO);
	io_block_mapping(IMAP_ADDR/*virt*/, IMAP_ADDR/*phys*/, 0x40000, _PAGE_IO);
	/* Map I/O region to a 64MB BAT */
//	io_block_mapping(0, 0, (unsigned int)binfo->bi_memsize, _PAGE_RAM|_PAGE_USER);
/*XXX*/	//printk("%s @ %s : %d\n",__FILE__,__FUNCTION__,__LINE__);
}

static u8 br_ide_inb (unsigned long port)
{
        return (u8) in_8(port);
}

static u16 br_ide_inw (unsigned long port)
{
        return (u16) in_be16(port);
}

//static unsigned int br_ide_base= 0;
static void br_ide_insw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
                *(u16 *)addr = in_be16((void*)port);
                addr += 2;
        }
}

static void br_ide_outb (u8 val, unsigned long port)
{
	out_8(port, val);	
}

static void br_ide_outbsync (ide_drive_t *drive, u8 addr, unsigned long port)
{
        out_8(port, addr);
}

static void br_ide_outw (u16 val, unsigned long port)
{
        out_be16(port, val);
}

static void br_ide_outsw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
		out_be16(port,*(u16 *)addr);
                addr += 2;
        }
}

void pq2_ptin_hwif_iops (ide_hwif_t *hwif/*, unsigned long ide_base*/)
{
        hwif->OUTB      = br_ide_outb;
        hwif->OUTBSYNC  = br_ide_outbsync;
        hwif->OUTW      = br_ide_outw;
        hwif->OUTL      = NULL;
        hwif->OUTSW     = br_ide_outsw;
        hwif->OUTSL     = NULL;
        hwif->INB       = br_ide_inb;
        hwif->INW       = br_ide_inw;
        hwif->INL       = NULL;
        hwif->INSW      = br_ide_insw;
        hwif->INSL      = NULL;

//	br_ide_base= ide_base;

#if 0
{
        
        u16 cmd= ATA_CMD_PIO_READ;
        u32 lba= 0x00000001;
        u16 sector= 1;
        unsigned short buffer[256];

        if(0x00 == (hwif->INB(ide_base+7) & 0x08))
        {
        //***Set sector count*** //LBA[7:0]
        hwif->OUTB(ide_base+2,sector);
        hwif->OUTB(ide_base+3,lba & 0xFF);
        //***Set the LBA address of memory block to be written*** 
        hwif->OUTB(ide_base+4,(lba >> 8) & 0xFF);
        hwif->OUTB(ide_base+5,(lba >> 12) & 0xFF);
        hwif->OUTB(ide_base+6,0xE0);
        hwif->OUTB(ide_base+7,cmd);
        while ( (hwif->INB(ide_base+7) & 0x80) ) {//poll for busy bit (bit 7 of register), quit loop when busy bit =0
        }

        printk("Start read to buffer\n");
        //Read data to data buffer until DRQ is clear
	hwif->INSW(ide_base+8,buffer,256);
        printk("Finish Read: |ide_base:0x%X\n",ide_base);
#define printbuffer(x,c) \
{ \
  int j; \
  for (j= 0; j<c; j++) { \
    if(0 == (j % 10)) printk("\n"); \
    printk("0x%4.4X ", x[j]); \
  } \
  printk("\n"); \
} 

        printbuffer(buffer, 256);
        }
        else
        {
        printk("%s @ %d: Está busy antes de executar um cmd!\n",__FUNCTION__,__LINE__);
        }
}
#endif //debug
}


EXPORT_SYMBOL(pq2_ptin_hwif_iops);
EXPORT_SYMBOL(pq2_ptin_bootmem_buffer);

#ifdef CONFIG_MTD_PHYSMAP
static struct mtd_partition pq2_ptin_partitions[1] = {
        {
         .name = "U-BOOT",
         .size = 0x00040000,
         .offset = 0,
         .mask_flags = 0,
         },
};

static int __init pq2_ptin_setup_mtd(void)
{
	physmap_configure(0xfff00000, 0x40000, 1, NULL);
	physmap_map.size= 0x40000;
        physmap_set_partitions(pq2_ptin_partitions, 1);
        return 0;
}

arch_initcall(pq2_ptin_setup_mtd);
#endif






static long
pq2ptin_init_rtc_time(void)
{
#ifdef RTC_I2C_DS1339
  unsigned long time;
  struct rtc_time t;

  unsigned char x=0x20;

  unsigned long tmp_pdird, tmp_ppard, tmp_psord, tmp_podrd;

  tmp_pdird= cpm2_immr->im_ioport.iop_pdird;
  tmp_ppard= cpm2_immr->im_ioport.iop_ppard;
  tmp_psord= cpm2_immr->im_ioport.iop_psord;
  tmp_podrd= cpm2_immr->im_ioport.iop_podrd;

  cpm2_immr->im_ioport.iop_ppard&= ~(I2C_SDA0|I2C_SCL0); //general purpose
  cpm2_immr->im_ioport.iop_pdird|= I2C_SCL0;

  i2c_init();
  if ( i2c_probe( DS1339_ADDR ) == 0 ) {
    i2c_write(DS1339_ADDR,0x0e/*CONTROL REGISTER*/,1,&x,1);
    // Load rtc
    time = pq2ptin_get_rtc_time();
    to_tm(time, &t);
    printk("RTC: DS1339 present - System time: %04d/%02d/%02d %02d:%02d:%02d\n",
           t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );
  }
  else {
    printk("RTC: DS1339 not present.\n");
  }

  cpm2_immr->im_ioport.iop_pdird= tmp_pdird;
  cpm2_immr->im_ioport.iop_ppard= tmp_ppard;
  cpm2_immr->im_ioport.iop_psord= tmp_psord;
  cpm2_immr->im_ioport.iop_podrd= tmp_podrd;
#endif
  return(0);
}

static int
pq2ptin_set_rtc_time(unsigned long time)
{
#ifdef RTC_I2C_DS1339

    unsigned char buf[I2C_RXTX_LEN];
    struct rtc_time t;

    unsigned long tmp_pdird, tmp_ppard, tmp_psord, tmp_podrd;

    tmp_pdird= cpm2_immr->im_ioport.iop_pdird;
    tmp_ppard= cpm2_immr->im_ioport.iop_ppard;
    tmp_psord= cpm2_immr->im_ioport.iop_psord;
    tmp_podrd= cpm2_immr->im_ioport.iop_podrd;

    cpm2_immr->im_ioport.iop_ppard&= ~(I2C_SDA0|I2C_SCL0); //general purpose
    cpm2_immr->im_ioport.iop_pdird|= I2C_SCL0;

    to_tm(time, &t);
    buf[DS1339_SECONDS] = DEC2BCD(t.tm_sec); 
    buf[DS1339_MINUTES] = DEC2BCD(t.tm_min);
    buf[DS1339_HOURS  ] = DEC2BCD(t.tm_hour);
    buf[DS1339_DAY    ] = DEC2BCD(t.tm_wday+1); 
    buf[DS1339_DATE   ] = DEC2BCD(t.tm_mday); 
    buf[DS1339_MONTH  ] = DEC2BCD(t.tm_mon);
    buf[DS1339_YEAR   ] = DEC2BCD(t.tm_year % 100);
    if ( t.tm_year >= 2000 ) buf[DS1339_MONTH] |= 0x80;
    i2c_write(DS1339_ADDR , 0x00 , 1 , buf , 7 );

    cpm2_immr->im_ioport.iop_pdird= tmp_pdird;
    cpm2_immr->im_ioport.iop_ppard= tmp_ppard;
    cpm2_immr->im_ioport.iop_psord= tmp_psord;
    cpm2_immr->im_ioport.iop_podrd= tmp_podrd;

#endif

    //((volatile immap_t *)IMAP_ADDR)->im_sitk.sitk_rtck = KAPWR_KEY;
    //((volatile immap_t *)IMAP_ADDR)->im_sit.sit_rtc = time;
    //((volatile immap_t *)IMAP_ADDR)->im_sitk.sitk_rtck = ~KAPWR_KEY;

    return(0);
}

static unsigned long
pq2ptin_get_rtc_time(void)
{
  unsigned long nowtime;

  /* Get time from the RTC. */

#ifdef RTC_I2C_DS1339

    unsigned char buf[I2C_RXTX_LEN];
    struct rtc_time t;

    unsigned long tmp_pdird, tmp_ppard, tmp_psord, tmp_podrd;

    tmp_pdird= cpm2_immr->im_ioport.iop_pdird;
    tmp_ppard= cpm2_immr->im_ioport.iop_ppard;
    tmp_psord= cpm2_immr->im_ioport.iop_psord;
    tmp_podrd= cpm2_immr->im_ioport.iop_podrd;

    cpm2_immr->im_ioport.iop_ppard&= ~(I2C_SDA0|I2C_SCL0); //general purpose
    cpm2_immr->im_ioport.iop_pdird|= I2C_SCL0;

    if ( i2c_read(DS1339_ADDR , 0x00 , 1 , buf , 7 ) == 0 ) {
      t.tm_sec  = (int)BCD2DEC(buf[DS1339_SECONDS] & 0x7F);
      t.tm_min  = (int)BCD2DEC(buf[DS1339_MINUTES] & 0x7F);
      t.tm_hour = (int)BCD2DEC(buf[DS1339_HOURS]   & 0x3F);
      t.tm_wday = (int)BCD2DEC(buf[DS1339_DAY]     & 0x07) - 1;
      t.tm_mday = (int)BCD2DEC(buf[DS1339_DATE]    & 0x3F);
      t.tm_mon  = (int)BCD2DEC(buf[DS1339_MONTH]   & 0x1F);
      t.tm_year = (int)BCD2DEC(buf[DS1339_YEAR]          ) + 1900;
      if ( ( buf[DS1339_MONTH] & 0x80 ) != 0 ) t.tm_year += 100;
      nowtime = mktime(t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    }
    else {
      nowtime = 0; //(unsigned long)(((immap_t *)IMAP_ADDR)->im_sit.sit_rtc);
    }
    cpm2_immr->im_ioport.iop_pdird= tmp_pdird;
    cpm2_immr->im_ioport.iop_ppard= tmp_ppard;
    cpm2_immr->im_ioport.iop_psord= tmp_psord;
    cpm2_immr->im_ioport.iop_podrd= tmp_podrd;
#else

    nowtime = 0;//(unsigned long)(((immap_t *)IMAP_ADDR)->im_sit.sit_rtc);
#endif


  return(nowtime);
}






void pq2_ptin_init(void)
{
	ppc_md.setup_io_mappings= pq2_ptin_m8260_map_io;

	/* Anything special for this platform */
	ppc_md.show_cpuinfo	= pq2_ptin_show_cpuinfo;

#ifdef RTC_I2C_DS1339
      ppc_md.time_init      = pq2ptin_init_rtc_time;
      ppc_md.set_rtc_time   = pq2ptin_set_rtc_time;
      ppc_md.get_rtc_time   = pq2ptin_get_rtc_time;      
#endif

	callback_setup_arch	= ppc_md.setup_arch;
	ppc_md.setup_arch	= pq2_ptin_setup_arch;

}

