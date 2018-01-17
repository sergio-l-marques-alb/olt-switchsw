/*
 * (C) Copyright 2004
 * Elmeg Communications Systems GmbH, Juergen Selent (j.selent@elmeg.de)
 *
 * Support for the Elmeg VoVPN Gateway Module
 * ------------------------------------------
 * Initialize Marvell M88E6095 Switch
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
//#include <mpc8260.h>
//#include <asm/m8260_pci.h>
#include <net.h>
#include <miiphy.h>

#include "mv88e6095.h"

#if defined(CONFIG_CMD_NET)

static char devname[] = "FM1_DTSEC1"; /*CONFIG_ETHPRIME;*/
static int		prtTab[M88X_PRT_CNT] = { 16 , 17 , 18 , 19 , 20 , 21 , 22 , 23 , 24 , 25 , 26 };
static int		phyTab[M88X_PHY_CNT] = { 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 };



/* Portos FE*/
static mv88x_regCfg_t	prtCfgFE[] = {
	{  0x04, 0x0000, 0x0077 },   /* PortSate = Forwarding */
	{  0x06, 0x0000, 0x0600 },   /* Port based VLAN table (bits 9,10:1) */
	{    -1, 0xffff, 0x0000 }
};


/* Porto FPGA */
static mv88x_regCfg_t	prtCfgFPGA[] = { /*porto 8*/
	{  0x01, 0x0000, 0x003E },   /* Force link up, 1000 FD */
	{  0x06, 0x0000, 0x0600 },   /* Port based VLAN table (bits 9,10:1) */
	{    -1, 0xffff, 0x0000 }
};

/* Porto UP1 UNICOM */
static mv88x_regCfg_t	prtCfgUnicom[] = { /*porto 10*/
    {  0x01, 0x0000, 0x003E },   /* Force link up, 1000 FD */
	{  0x06, 0x0000, 0x03ff },   /* Port based VLAN table (bits 9:0) */
	{    -1, 0xffff, 0x0000 }
};

/* Porto UP2 MATRIX */
static mv88x_regCfg_t	prtCfgMatrix[] = {/*porto 9*/
	{  0x01, 0x0000, 0x003E },   /* Force link up, 1000 FD */
	{  0x06, 0x0000, 0x05ff },   /* Port based VLAN table (bits 10:0) */
	{    -1, 0xffff, 0x0000 }
};



static mv88x_regCfg_t	*prtCfg[M88X_PRT_CNT] = {
	prtCfgFE,prtCfgFE,prtCfgFE,prtCfgFE,prtCfgFE,prtCfgFE,prtCfgFE,prtCfgFE,prtCfgFPGA,prtCfgMatrix,prtCfgUnicom
};

/* Debug Interface */
static mv88x_regCfg_t	phyCfg1[] = {
	{  0x00, 0x0000, 0xb100 },    /* AN on, 100Mbps, FD */
	{    -1, 0xffff, 0x0000 }
};



static mv88x_regCfg_t	*phyCfg[M88X_PHY_CNT] = {
	phyCfg1,phyCfg1,phyCfg1,phyCfg1,phyCfg1,phyCfg1,phyCfg1
};


int marvell_miiphy_read(const char *devname, unsigned char addr_sw, unsigned char addr, unsigned char reg, unsigned short *value)
{
	int ret;
    
        /* Transmit the read command */
        ret = miiphy_write(devname, addr_sw, 0, 0x9800 | (addr << 5) | reg);
        
        if (ret != 0){
            printf("Deu merda no write reg0 ret=%d\n\r",ret);
            return ret;
        }
    
        /* Read the data */
        ret = miiphy_read(devname, addr_sw,  1, value);

        if (ret != 0){
            printf("Deu merda no read reg1 ret=%d\n\r",ret);
            return ret;
        }
    

	return  *value & 0xffff;
}

int marvell_miiphy_write(const char *devname, unsigned char addr_sw, unsigned char addr, unsigned char reg, unsigned short val)
{                       
	int ret;

    
        /* Transmit the data to write */
        ret = miiphy_write(devname, addr_sw, 1, val);
        if (ret != 0){
            printf("Deu merda no write reg1 ret=%d\n\r",ret);
            return ret;
        }

    
        /* Transmit the write command */
        ret = miiphy_write(devname, addr_sw, 0, 0x9400 | (addr << 5) | reg);
         if (ret != 0){
            printf("Deu merda no write reg0 ret=%d\n\r",ret);
            return ret;
        }
    

	return 0;
}

#if 1
static void
mv88e6095_dump( int devAddr )
{
	int		i, j;
	unsigned short	val[12];

    printf( "\n" );
	printf( "M88E6095 Register Dump\n" );
	printf( "==========================================================\n" );
	printf( "PortNo    1    2    3    4    5    6   7   8   9   10   11\n" );
	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_STAT,&val[i] );

	printf( "STAT   %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_ID,&val[i] );

	printf( "ID     %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_CNTL,&val[i] );

	printf( "CNTL  %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_VLAN,&val[i] );

	printf( "VLAN   %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_PAV,&val[i] );

	printf( "PAV    %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_RX,&val[i] );

	printf( "RX     %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	for (i=0; i<11; i++)
		marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_TX,&val[i] );

	printf( "TX     %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10] );

	printf( "---------------------------------------\n" );
	printf( "PhyNo     1    2    4    5    6   7   8\n" );
	for (i=0; i<9; i++) {
		for (j=0; j<8; j++) {
			marvell_miiphy_read(devname, devAddr, phyTab[j],i,&val[j] );
		}
		printf( "0x%02x   %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
			i,val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7] );
	}
	for (i=0x10; i<0x1d; i++) {
		for (j=0; j<8; j++) {
			marvell_miiphy_read(devname, devAddr, phyTab[j],i,&val[j] );
		}
		printf( "0x%02x   %04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx\n",
			i,val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7] );
	}
}
#endif

int
mv88e6095_initialize( int devAddr )
{
	static char	*_f = "mv88e6095_initialize:";
	mv88x_regCfg_t	*p;
	int		err;
	int		i;
	unsigned short	val;

	/*** reset all phys into powerdown ************************************/
	for (i=0, err=0; i<M88X_PHY_CNT; i++) {
		err += marvell_miiphy_read(devname, devAddr, phyTab[i],M88X_PHY_CNTL,&val );
		/* keep SpeedLSB, Duplex */
		val &= 0x2100;
		/* set SWReset, AnegEn, PwrDwn, RestartAneg */
		val |= 0x9a00;
		err += marvell_miiphy_write(devname, devAddr,phyTab[i],M88X_PHY_CNTL,val );
	}
	if (err) {
		printf( "%s [ERR] reset phys\n",_f );
		return( -1 );
	}

	/*** disable all ports ************************************************/
	for (i=0, err=0; i<M88X_PRT_CNT; i++) {
		err += marvell_miiphy_read(devname, devAddr,prtTab[i],M88X_PRT_CNTL,&val );
		val &= 0xfffc;
		err += marvell_miiphy_write(devname, devAddr,prtTab[i],M88X_PRT_CNTL,val );
	}
	if (err) {
		printf( "%s [ERR] disable ports\n",_f );
		return( -1 );
	}


	/*** initialize ports *************************************************/
	for (i=0; i<M88X_PRT_CNT; i++) {
		if ((p = prtCfg[i]) == NULL) {
			continue;
		}
		while (p->reg != -1) {
			err = 0;
			err += marvell_miiphy_read(devname, devAddr,prtTab[i],p->reg,&val );
			val &= p->msk;
			val |= p->val;
			err += marvell_miiphy_write(devname, devAddr,prtTab[i],p->reg,val );
			if (err) {
				printf( "%s [ERR] config port %d register %d\n",_f,i,p->reg );
				/* XXX what todo */
			}
			p++;
		}
	}

	/*** initialize phys **************************************************/
	for (i=0; i<M88X_PHY_CNT; i++) {
		if ((p = phyCfg[i]) == NULL) {
			continue;
		}
		while (p->reg != -1) {
			err = 0;
			err += marvell_miiphy_read(devname, devAddr, phyTab[i],p->reg,&val );
			val &= p->msk;
			val |= p->val;
			err += marvell_miiphy_write(devname, devAddr, phyTab[i],p->reg,val );
			if (err) {
				printf( "%s [ERR] config phy %d register %d\n",_f,i,p->reg );
				/* XXX what todo */
			}
			p++;
		}
	}
	udelay(100000);

//mv88e6095_dump(devAddr);

	return( 0 );
}

int dump_marvel ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  int phy_addr = CONFIG_SYS_FM1_DTSEC1_PHY_ADDR;

  if (argc == 2)
    phy_addr = simple_strtoul(argv[1], NULL, 16);

  mv88e6095_dump(phy_addr);

  return 0;
}

U_BOOT_CMD(
          marvel,  2,  1,  dump_marvel,
          "Dump all registers from Marvel 88E6095",
          "marvel <base addr> - Dump all registers from Marvel 88E6095 at <base addr>"
          );

#endif
