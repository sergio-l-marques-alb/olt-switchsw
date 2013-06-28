/*
 * $Id: sysbrds.c,v 1.17 2003/08/06 10:13:49 jeffc Exp $
 * $Copyright: (c) 2002, 2003 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:        sysbrds.c
 * Purpose:
 * Requires:
 */

/* Some basic board descriptions */
/* all of our chips support the LEDPROC */
#define BCM_LEDPROC_SUPPORT

#include <stdio.h>
#include <soc/drv.h>
#include <soc/cm.h>

#include <bcm/error.h>

#include "sysbrds.h"
#include "bcm/stack.h"
#include "registry.h"
#include <appl/stktask/topo_brd.h>

/*
 * Board specific initialization code
 * used by the board definitions below.
 */
#include "ledlink.h"
#include "l7_common.h"
#include "hpc_db.h"
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm_int/esw/rcpu.h>
#include <bcm/l2.h>
#include "sysapi_hpc.h"
#include <sal/appl/config.h>

#include "soc/drv.h"
#include "broad_ids.h"

#include "ptin_globaldefs.h"  /* PTin added */
#include "logger.h"           /* PTin added */

#ifdef PTIN_WC_SLOT_MAP
#include "broad_hpc_db.h"

#define WC_MAP_FILE "/usr/local/ptin/var/wc_map.txt"

L7_RC_t hpcBoardWCinit_bcm56846(void);
#endif

int
bcm_sys_sa_init_56624(const bcm_sys_board_t* brd, int base)
{
    return 0;
}

int
bcm_sys_sa_init_56680(const bcm_sys_board_t* brd, int base)
{
    return 0;
}

/* sdk56820 */
static const unsigned char ledproc_56820[] = {
 0xE0, 0x28, 0x60, 0xF2, 0x67, 0x1B, 0x06, 0xF2,
 0x80, 0xD2, 0x14, 0x74, 0x01, 0x86, 0xF3, 0x12,
 0xF0, 0x85, 0x05, 0xD2, 0x05, 0x71, 0x19, 0x52,
 0x00, 0x3A, 0x28, 0x32, 0x00, 0x97, 0x75, 0x27,
 0x12, 0xA8, 0xFE, 0xF2, 0x02, 0x0A, 0x50, 0x32,
 0x01, 0x97, 0x75, 0x33, 0x12, 0xBC, 0xFE, 0xF2,
 0x02, 0x0A, 0x50, 0x12, 0xBC, 0xFE, 0xF2, 0x95,
 0x75, 0x45, 0x85, 0x12, 0xA8, 0xFE, 0xF2, 0x95,
 0x75, 0x91, 0x85, 0x77, 0x57, 0x12, 0xA8, 0xFE,
 0xF2, 0x95, 0x75, 0x4F, 0x85, 0x77, 0x8A, 0x16,
 0xF0, 0xDA, 0x02, 0x71, 0x8A, 0x77, 0x91, 0x06,
 0xF2, 0x12, 0x94, 0xF8, 0x15, 0x02, 0x02, 0xC1,
 0x74, 0x6E, 0x02, 0x04, 0xC1, 0x74, 0x6E, 0x02,
 0x08, 0xC1, 0x74, 0x6E, 0x77, 0x74, 0xC6, 0xF3,
 0x74, 0x91, 0x77, 0x8A, 0x06, 0xF2, 0x67, 0x7C,
 0x75, 0x83, 0x77, 0x91, 0x12, 0x80, 0xF8, 0x15,
 0x1A, 0x00, 0x57, 0x32, 0x0E, 0x87, 0x32, 0x0E,
 0x87, 0x57, 0x32, 0x0E, 0x87, 0x32, 0x0F, 0x87,
 0x57, 0x32, 0x0F, 0x87, 0x32, 0x0E, 0x87, 0x57,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

int
bcm_sys_sa_init_56820(const bcm_sys_board_t* brd, int base)
{

    /* Program led processor */
    soc_ledproc_config(0, ledproc_56820, sizeof(ledproc_56820));

    bcm_linkscan_register(0, hpc_default_led_linkscan_cb);

    return 0;
}


/*********************************************************************
* @purpose  Board specific driver configuration.
*
* @param    
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @end
*********************************************************************/
L7_RC_t hpcConfigBoardSet()
{
  HPC_UNIT_DESCRIPTOR_t       *lclUnitDesc;

  if ((lclUnitDesc = hpcLocalUnitDescriptorGet()) != L7_NULLPTR)
  {
    switch (lclUnitDesc->unitTypeDescriptor.unitTypeId)
    {

      case UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID: /* Scorpion */
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);
        sal_config_set("phy_led1_mode", "1");
        sal_config_set("phy_led2_mode", "3");
        sal_config_set("phy_led_ctrl","0xb");
        break;

      case UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID:
      case UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID:
      case UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID:
      case UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID:
      case UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID:
      case UNIT_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID:
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);

        sal_config_set("phy_led1_mode", "0xa");
        sal_config_set("phy_led2_mode", "0xa");
        sal_config_set("phy_led_select","0x128");          
        break;

      case UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID:
      case UNIT_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID:   /* PTin added: new platform */
        /* Enable trunk_128 bit. This will enable 128 trunks */
        /* and fixes LAG issue on XGS3 stacking              */
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);
#ifdef L7_STACKING_PACKAGE
        /* On Stacking packages, restrict FDB size to 16K MAX for FB2. */
        if (sal_config_set("l2_table_size", "0x3fff") != 0)
          return(L7_FAILURE);
#endif
        break;

      /* PTin added: new switch BCM56843 */
      case UNIT_BROAD_40_TENGIG_56843_REV_1_ID:
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0) return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0) return(L7_FAILURE);

        /* Configure Portmaps: different mapping for CXP360G V2 and V3 */
        if (cpld_map->reg.hw_ver <= 2)
        {
          LOG_TRACE(LOG_CTX_STARTUP, "Using Port Expanders mapping for CXP360G V2 or below");

          if (sal_config_set(spn_PORT_PHY_ADDR"_1",  "0x10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_2",  "0x11") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_3",  "0x12") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_4",  "0x13") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_5",  "0x08") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_6",  "0x09") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_7",  "0x0A") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_8",  "0x0B") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_9",  "0x00") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_10", "0x01") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_11", "0x02") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_12", "0x03") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_13", "0x0C") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_14", "0x0D") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_15", "0x0E") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_16", "0x0F") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_17", "0x04") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_18", "0x05") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_19", "0x06") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_20", "0x07") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_21", "0x2C") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_22", "0x2D") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_23", "0x2E") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_24", "0x2F") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_25", "0x24") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_26", "0x25") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_27", "0x26") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_28", "0x27") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_29", "0x30") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_30", "0x31") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_31", "0x32") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_32", "0x33") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_33", "0x28") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_34", "0x29") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_35", "0x2A") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_36", "0x2B") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_37", "0x20") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_38", "0x21") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_39", "0x22") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_40", "0x23") != 0) return(L7_FAILURE);
        }
        else
        {
          LOG_TRACE(LOG_CTX_STARTUP, "Using Port Expanders mapping for CXP360G V3 or higher");

          if (sal_config_set(spn_PORT_PHY_ADDR"_1",  "0x30") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_2",  "0x31") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_3",  "0x32") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_4",  "0x33") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_5",  "0x08") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_6",  "0x09") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_7",  "0x0A") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_8",  "0x0B") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_9",  "0x00") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_10", "0x01") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_11", "0x02") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_12", "0x03") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_13", "0x0C") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_14", "0x0D") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_15", "0x0E") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_16", "0x0F") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_17", "0x04") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_18", "0x05") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_19", "0x06") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_20", "0x07") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_21", "0x4C") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_22", "0x4D") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_23", "0x4E") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_24", "0x4F") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_25", "0x44") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_26", "0x45") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_27", "0x46") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_28", "0x47") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_29", "0x50") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_30", "0x51") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_31", "0x52") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_32", "0x53") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_33", "0x48") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_34", "0x49") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_35", "0x4A") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_36", "0x4B") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_37", "0x20") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_38", "0x21") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_39", "0x22") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORT_PHY_ADDR"_40", "0x23") != 0) return(L7_FAILURE);
        }

        if (sal_config_set(spn_PORTMAP"_1",  "9:10")  != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_2",  "10:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_3",  "11:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_4",  "12:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_5",  "13:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_6",  "14:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_7",  "15:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_8",  "16:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_9",  "17:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_10", "18:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_11", "19:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_12", "20:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_13", "21:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_14", "22:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_15", "23:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_16", "24:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_17", "25:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_18", "26:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_19", "27:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_20", "28:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_21", "45:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_22", "46:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_23", "47:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_24", "48:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_25", "49:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_26", "50:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_27", "51:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_28", "52:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_29", "53:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_30", "54:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_31", "55:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_32", "56:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_33", "57:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_34", "58:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_35", "59:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_36", "60:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_37", "61:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_38", "62:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_39", "63:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_40", "64:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_41", "29:10") != 0) return(L7_FAILURE); /* PTP interface */

        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe0",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe1",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe2",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe3",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe4",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe5",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe6",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe7",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe8",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe9",  "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe10", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe11", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe12", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe13", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe14", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe15", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe16", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe17", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe18", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe19", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe20", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe21", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe22", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe23", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe24", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe25", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe26", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe27", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe28", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe29", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe30", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe31", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe32", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe33", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe34", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe35", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe36", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe37", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe38", "0x01") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_MODE_REVERSE"_xe39", "0x01") != 0) return(L7_FAILURE);

        /* Disable BAM */
        #if 1
        if (sal_config_set(spn_PHY_AN_C73"_xe0",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe1",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe2",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe3",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe4",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe5",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe6",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe7",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe8",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe9",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe10", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe11", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe12", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe13", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe14", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe15", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe16", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe17", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe18", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe19", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe20", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe21", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe22", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe23", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe24", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe25", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe26", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe27", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe28", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe29", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe30", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe31", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe32", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe33", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe34", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe35", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe36", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe37", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe38", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe39", "0x02") != 0) return(L7_FAILURE);
        #endif

        break;

      /* PTin added: new switch BCM56846 */
      case UNIT_BROAD_64_TENGIG_56846_REV_1_ID:
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0) return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0) return(L7_FAILURE);

        /* For CXO640G-V1 */
        #if (PTIN_BOARD == PTIN_BOARD_CXO640G_V1 || PTIN_BOARD == PTIN_BOARD_CXO640G)
        if (cpld_map->reg.id == CPLD_ID_CXO640G_V1)
        {
          if (sal_config_set(spn_PORTMAP"_1",   "3:10") != 0) return(L7_FAILURE); // slot 2 lane 2
          if (sal_config_set(spn_PORTMAP"_2",   "4:10") != 0) return(L7_FAILURE); // slot 2 lane 3
                                                                                  // 
          if (sal_config_set(spn_PORTMAP"_3",   "7:10") != 0) return(L7_FAILURE); // slot 3 lane 2
          if (sal_config_set(spn_PORTMAP"_4",   "8:10") != 0) return(L7_FAILURE); // slot 3 lane 3

          if (sal_config_set(spn_PORTMAP"_5",   "9:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_6",  "10:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_7",  "11:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_8",  "12:10") != 0) return(L7_FAILURE);

          if (sal_config_set(spn_PORTMAP"_9",  "13:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_10", "14:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_11", "15:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_12", "16:10") != 0) return(L7_FAILURE);

          if (sal_config_set(spn_PORTMAP"_13", "19:10") != 0) return(L7_FAILURE); // slot 6 lane 2
          if (sal_config_set(spn_PORTMAP"_14", "20:10") != 0) return(L7_FAILURE); // slot 6 lane 3

          if (sal_config_set(spn_PORTMAP"_15", "23:10") != 0) return(L7_FAILURE); // slot 7 lane 2
          if (sal_config_set(spn_PORTMAP"_16", "24:10") != 0) return(L7_FAILURE); // slot 7 lane 3

          if (sal_config_set(spn_PORTMAP"_17", "25:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_18", "26:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_19", "27:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_20", "28:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_21", "29:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_22", "30:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_23", "31:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_24", "32:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_25", "33:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_26", "34:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_27", "35:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_28", "36:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_29", "37:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_30", "38:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_31", "39:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_32", "40:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_33", "41:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_34", "42:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_35", "43:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_36", "44:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_37", "45:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_38", "46:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_39", "47:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_40", "48:10") != 0) return(L7_FAILURE);

          if (sal_config_set(spn_PORTMAP"_41", "51:10") != 0) return(L7_FAILURE); // slot 14 lane 2
          if (sal_config_set(spn_PORTMAP"_42", "52:10") != 0) return(L7_FAILURE); // slot 14 lane 3

          if (sal_config_set(spn_PORTMAP"_43", "55:10") != 0) return(L7_FAILURE); // slot 15 lane 2
          if (sal_config_set(spn_PORTMAP"_44", "56:10") != 0) return(L7_FAILURE); // slot 15 lane 3

          if (sal_config_set(spn_PORTMAP"_45", "57:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_46", "58:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_47", "59:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_48", "60:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_49", "61:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_50", "62:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_51", "63:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_52", "64:10") != 0) return(L7_FAILURE);

          if (sal_config_set(spn_PORTMAP"_53", "67:10") != 0) return(L7_FAILURE); // slot 18 lane 2
          if (sal_config_set(spn_PORTMAP"_54", "68:10") != 0) return(L7_FAILURE); // slot 18 lane 3

          if (sal_config_set(spn_PORTMAP"_55", "71:10") != 0) return(L7_FAILURE); // slot 19 lane 2
          if (sal_config_set(spn_PORTMAP"_56", "72:10") != 0) return(L7_FAILURE); // slot 19 lane 3

          if (sal_config_set(spn_PORTMAP"_57",   "1:1") != 0) return(L7_FAILURE); // PTP

          // extra mapping to fill 64 ports in total
          if (sal_config_set(spn_PORTMAP"_58",   "2:1") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_59",   "5:1") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_60",   "6:1") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_61",  "65:1") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_62",  "66:1") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_63",  "69:1") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_64",  "70:1") != 0) return(L7_FAILURE);
        }
        else
        {
          /* Group Core 0 */
          /* Slot 2: WC1: lanes 7-8 */
          if (sal_config_set(spn_PORTMAP"_1",   "7:10") != 0) return(L7_FAILURE); // slot 2 lane 2
          if (sal_config_set(spn_PORTMAP"_2",   "8:10") != 0) return(L7_FAILURE); // slot 2 lane 3

          /* Slot 3: WC2: lanes 11-12 */
          if (sal_config_set(spn_PORTMAP"_3",  "11:10") != 0) return(L7_FAILURE); // slot 3 lane 2
          if (sal_config_set(spn_PORTMAP"_4",  "12:10") != 0) return(L7_FAILURE); // slot 3 lane 3

          /* Slot 4: WC4: lanes 17-20 */
          if (sal_config_set(spn_PORTMAP"_5",  "17:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_6",  "18:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_7",  "19:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_8",  "20:10") != 0) return(L7_FAILURE);

          /* Slot 5: WC3: lanes 13-16 */
          if (sal_config_set(spn_PORTMAP"_9",  "13:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_10", "14:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_11", "15:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_12", "16:10") != 0) return(L7_FAILURE);

          /* Slot 6: WC0: lanes 3-4 */
          if (sal_config_set(spn_PORTMAP"_13",  "3:10") != 0) return(L7_FAILURE); // slot 6 lane 2
          if (sal_config_set(spn_PORTMAP"_14",  "4:10") != 0) return(L7_FAILURE); // slot 6 lane 3

          /* Group Core 1 */
          /* Slot 7: WC5: lanes 23-24 */
          if (sal_config_set(spn_PORTMAP"_15", "23:10") != 0) return(L7_FAILURE); // slot 7 lane 2
          if (sal_config_set(spn_PORTMAP"_16", "24:10") != 0) return(L7_FAILURE); // slot 7 lane 3

          /* Slot 8: WC6: lanes 25-28 */
          if (sal_config_set(spn_PORTMAP"_17", "25:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_18", "26:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_19", "27:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_20", "28:10") != 0) return(L7_FAILURE);

          /* Slot 9: WC7: lanes 29-32 */
          if (sal_config_set(spn_PORTMAP"_21", "29:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_22", "30:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_23", "31:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_24", "32:10") != 0) return(L7_FAILURE);

          /* Slot 10: WC8: lanes 33-36 */
          if (sal_config_set(spn_PORTMAP"_25", "33:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_26", "34:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_27", "35:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_28", "36:10") != 0) return(L7_FAILURE);

          /* Group Core 2 */
          /* Slot 11: WC9: lanes 37-40 */
          if (sal_config_set(spn_PORTMAP"_29", "37:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_30", "38:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_31", "39:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_32", "40:10") != 0) return(L7_FAILURE);

          /* Slot 12: WC10: lanes 41-44 */
          if (sal_config_set(spn_PORTMAP"_33", "41:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_34", "42:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_35", "43:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_36", "44:10") != 0) return(L7_FAILURE);

          /* Slot 13: WC11: lanes 45-48 */
          if (sal_config_set(spn_PORTMAP"_37", "45:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_38", "46:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_39", "47:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_40", "48:10") != 0) return(L7_FAILURE);

          /* Group Core 3 */
          /* Slot 14: WC16: lanes 67-68 */
          if (sal_config_set(spn_PORTMAP"_41", "67:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_42", "68:10") != 0) return(L7_FAILURE);

          /* Slot 15: WC17: lanes 71-72 */
          if (sal_config_set(spn_PORTMAP"_43", "71:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_44", "72:10") != 0) return(L7_FAILURE);

          /* Slot 16: WC14: lanes 57-60 */
          if (sal_config_set(spn_PORTMAP"_45", "57:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_46", "58:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_47", "59:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_48", "60:10") != 0) return(L7_FAILURE);

          /* Slot 17: WC15: lanes 61-64 */
          if (sal_config_set(spn_PORTMAP"_49", "61:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_50", "62:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_51", "63:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_52", "64:10") != 0) return(L7_FAILURE);

          /* Group Core 2 */
          /* Slot 18: WC12: lanes 51-52 */
          if (sal_config_set(spn_PORTMAP"_53", "51:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_54", "52:10") != 0) return(L7_FAILURE);

          /* Slot 19: WC13: lanes 55-56 */
          if (sal_config_set(spn_PORTMAP"_55", "55:10") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PORTMAP"_56", "56:10") != 0) return(L7_FAILURE);
          /* End of 10G ports */

          /* PTP port: WC1: lane 5 */
          if (sal_config_set(spn_PORTMAP"_57",   "5:1") != 0) return(L7_FAILURE); // GC0: WC1  lane 0

          // extra mapping to fill 64 ports in total
          if (sal_config_set(spn_PORTMAP"_58",   "6:1") != 0) return(L7_FAILURE); // GC0: WC1  lane 1
          if (sal_config_set(spn_PORTMAP"_59",   "1:1") != 0) return(L7_FAILURE); // GC0: WC0  lane 0
          if (sal_config_set(spn_PORTMAP"_60",   "2:1") != 0) return(L7_FAILURE); // GC0: WC0  lane 1
          if (sal_config_set(spn_PORTMAP"_61",  "65:1") != 0) return(L7_FAILURE); // GC1: WC16 lane 0
          if (sal_config_set(spn_PORTMAP"_62",  "66:1") != 0) return(L7_FAILURE); // GC3: WC16 lane 1
          if (sal_config_set(spn_PORTMAP"_63",  "69:1") != 0) return(L7_FAILURE); // GC3: WC17 lane 0
          if (sal_config_set(spn_PORTMAP"_64",  "70:1") != 0) return(L7_FAILURE); // GC3: WC17 lane 1

          /* Swap RX and TX polarities of WC 12 (slot 18) */
          if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_53", "0x0f00") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_53", "0x0f00") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_54", "0xf000") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_54", "0xf000") != 0) return(L7_FAILURE);
          LOG_INFO(LOG_CTX_STARTUP, "Polarities inverted!");

          /* TX Map */
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_1",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_2",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_3",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_4",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_5",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_6",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_7",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_8",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_9",  "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_10", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_11", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_12", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_13", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_14", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_15", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_16", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_17", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_18", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_19", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_20", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_21", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_22", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_23", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_24", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_25", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_26", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_27", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_28", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_29", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_30", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_31", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_32", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_33", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_34", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_35", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_36", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_37", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_38", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_39", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_40", "0x0123") != 0) return(L7_FAILURE);

          /* Slots 14 and 15 are not inverted */
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_41", "0x3210") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_42", "0x3210") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_43", "0x3210") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_44", "0x3210") != 0) return(L7_FAILURE);

          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_45", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_46", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_47", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_48", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_49", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_50", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_51", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_52", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_53", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_54", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_55", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_56", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_57", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_58", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_59", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_60", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_61", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_62", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_63", "0x0123") != 0) return(L7_FAILURE);
          if (sal_config_set(spn_XGXS_TX_LANE_MAP"_64", "0x0123") != 0) return(L7_FAILURE);
        }
        #elif (PTIN_BOARD == PTIN_BOARD_CXO640G_V2)

        #ifdef PTIN_WC_SLOT_MAP
        if (hpcBoardWCinit_bcm56846() == L7_SUCCESS)
        {
          LOG_NOTICE(LOG_CTX_STARTUP,"WCs initialized successfully");
        }
        else
        {
          LOG_ERR(LOG_CTX_STARTUP,"Error initializing WCs");
        }
        #else
        /* For the future: new slot mapping */
        /* Slot 2, 3, 18, 19: 20G; e o resto a 40G */

        /* Slot 2: WC1: 7-8 (20G) */
        //if (sal_config_set(spn_PORTMAP"_1",   "7:10") != 0) return(L7_FAILURE);
        //if (sal_config_set(spn_PORTMAP"_2",   "8:10") != 0) return(L7_FAILURE);

        /* Slot 3: WC2: 11-12 (20G) */
        if (sal_config_set(spn_PORTMAP"_1",   "9:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_2",  "10:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_3",  "11:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_4",  "12:10") != 0) return(L7_FAILURE);

        /* Slot 4: WC4: 17-20 */
        if (sal_config_set(spn_PORTMAP"_5",  "17:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_6",  "18:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_7",  "19:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_8",  "20:10") != 0) return(L7_FAILURE);

        /* Slot 5: WC3: 13-16 */
        if (sal_config_set(spn_PORTMAP"_9",  "13:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_10", "14:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_11", "15:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_12", "16:10") != 0) return(L7_FAILURE);

        /* Slot 6: WC0: 1-4 */
        if (sal_config_set(spn_PORTMAP"_13",  "1:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_14",  "2:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_15",  "3:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_16",  "4:10") != 0) return(L7_FAILURE);

        /* Slot 7: WC5: 21-24 */
        if (sal_config_set(spn_PORTMAP"_17", "21:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_18", "22:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_19", "23:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_20", "24:10") != 0) return(L7_FAILURE);

        /* Slot 8: WC6: 25-28 */
        if (sal_config_set(spn_PORTMAP"_21", "25:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_22", "26:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_23", "27:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_24", "28:10") != 0) return(L7_FAILURE);

        /* Slot 9: WC7: 29-32 */
        if (sal_config_set(spn_PORTMAP"_25", "29:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_26", "30:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_27", "31:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_28", "32:10") != 0) return(L7_FAILURE);

        /* Slot 10: WC8: 33-36 */
        if (sal_config_set(spn_PORTMAP"_29", "33:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_30", "34:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_31", "35:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_32", "36:10") != 0) return(L7_FAILURE);

        /* Slot 11: WC9: 37-40 */
        if (sal_config_set(spn_PORTMAP"_33", "37:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_34", "38:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_35", "39:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_36", "40:10") != 0) return(L7_FAILURE);

        /* Slot 12: WC10: 41-44 */
        if (sal_config_set(spn_PORTMAP"_37", "41:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_38", "42:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_39", "43:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_40", "44:10") != 0) return(L7_FAILURE);

        /* Slot 13: WC11: 45-48 */
        if (sal_config_set(spn_PORTMAP"_41", "45:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_42", "46:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_43", "47:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_44", "48:10") != 0) return(L7_FAILURE);

        /* Slot 14: WC16: 65-68 */
        if (sal_config_set(spn_PORTMAP"_45", "65:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_46", "66:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_47", "67:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_48", "68:10") != 0) return(L7_FAILURE);

        /* Slot 15: WC17: 69-72 */
        if (sal_config_set(spn_PORTMAP"_49", "69:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_50", "70:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_51", "71:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_52", "72:10") != 0) return(L7_FAILURE);

        /* Slot 16: WC14: 57-60 */
        if (sal_config_set(spn_PORTMAP"_53", "57:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_54", "58:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_55", "59:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_56", "60:10") != 0) return(L7_FAILURE);

        /* Slot 17: WC15: 61-64 */
        if (sal_config_set(spn_PORTMAP"_57", "61:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_58", "62:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_59", "63:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_60", "64:10") != 0) return(L7_FAILURE);

        /* Slot 18: WC12: 51-52 (20G) */
        if (sal_config_set(spn_PORTMAP"_61", "49:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_62", "50:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_63", "51:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_64", "52:10") != 0) return(L7_FAILURE);

        /* Slot 19: WC13: 55-56 (20G) */
        //if (sal_config_set(spn_PORTMAP"_63", "55:10") != 0) return(L7_FAILURE);
        //if (sal_config_set(spn_PORTMAP"_64", "56:10") != 0) return(L7_FAILURE);

        LOG_INFO(LOG_CTX_STARTUP, "Portmap defined!");

        /* Swap RX and TX polarities of WC 12 */
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_61", "0x000f") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_61", "0x000f") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_62", "0x00f0") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_62", "0x00f0") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_63", "0x0f00") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_63", "0x0f00") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_64", "0xf000") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_64", "0xf000") != 0) return(L7_FAILURE);
        LOG_INFO(LOG_CTX_STARTUP, "Polarities inverted!");

        /* TX Map */
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_1",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_2",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_3",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_4",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_5",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_6",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_7",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_8",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_9",  "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_10", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_11", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_12", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_13", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_14", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_15", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_16", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_17", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_18", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_19", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_20", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_21", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_22", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_23", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_24", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_25", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_26", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_27", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_28", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_29", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_30", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_31", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_32", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_33", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_34", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_35", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_36", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_37", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_38", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_39", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_40", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_41", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_42", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_43", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_44", "0x0123") != 0) return(L7_FAILURE);

        /* Slots 14 and 15 are not inverted */
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_45", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_46", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_47", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_48", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_49", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_50", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_51", "0x3210") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_52", "0x3210") != 0) return(L7_FAILURE);

        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_53", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_54", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_55", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_56", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_57", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_58", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_59", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_60", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_61", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_62", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_63", "0x0123") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_XGXS_TX_LANE_MAP"_64", "0x0123") != 0) return(L7_FAILURE);
        #endif
        #endif

        /* Disable BAM */
        #if 1
        if (sal_config_set(spn_PHY_AN_C73"_xe0",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe1",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe2",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe3",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe4",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe5",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe6",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe7",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe8",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe9",  "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe10", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe11", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe12", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe13", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe14", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe15", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe16", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe17", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe18", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe19", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe20", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe21", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe22", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe23", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe24", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe25", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe26", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe27", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe28", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe29", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe30", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe31", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe32", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe33", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe34", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe35", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe36", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe37", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe38", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe39", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe40", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe41", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe42", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe43", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe44", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe45", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe46", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe47", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe48", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe49", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe50", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe51", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe52", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe53", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe54", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe55", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe56", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe57", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe58", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe59", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe60", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe61", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe62", "0x02") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_AN_C73"_xe63", "0x02") != 0) return(L7_FAILURE);
        #endif

        break;
      /* PTin end */

      default:
        break;
    }
  }

  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Board specific PHY configuration
*
* @param    
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments these ports are referred to as XPORTS
*       
* @end
*********************************************************************/
L7_RC_t hpcConfigBoardPhySet(int       portNo,
                             L7_uint32 cardTypeId,
                             HAPI_CARD_SLOT_MAP_t *hapiSlotMapPtr)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Board specific POST PHY configuration
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments these ports are referred to as XPORTS
*
* @end
*********************************************************************/
L7_RC_t hpcConfigBoardPhyPostSet(int       portNo,
                             L7_uint32 cardTypeId,
                             HAPI_CARD_SLOT_MAP_t *hapiSlotMapPtr,
                             L7_uint32 phyCap)
{
  return L7_SUCCESS;
}

#ifdef PTIN_WC_SLOT_MAP
/**
 * Read a filename with the WC mapping
 * 
 * @param filename  : Source file name
 * @param slot_mode : Array of 20 slots with the portmode for 
 *                    each one
 * 
 * @return L7_RC_t : L7_SUCCESS - Success
 *                   L7_FAILURE - Error processing file
 */
static L7_RC_t hpcConfigWCmap_read(char *filename, L7_uint32 *slot_mode)
{
  FILE *fp;
  char seps[]=" ,\t\n";
  char line[255];
  char *token;
  L7_uint32 slot_idx, mode;
  L7_uint32 i;

  /* Validate arguments */
  if (filename==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid provided filename");
    return L7_FAILURE;
  }

  /* Open file for reading */
  fp = fopen(filename,"r");
  if(fp == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Erro a abrir o ficheiro %s", filename);
    return L7_FAILURE;
  }

  /* Clear output array */
  memset(slot_mode, 0xff, sizeof(L7_uint32)*PTIN_SYS_SLOTS_MAX);

  i = 0;
  while(!feof(fp)) 
  {
    i++;
    fgets(line,255,fp);            // le uma linha do ficheiro

    /* Get slot index */
    token = strtok(line, seps);    // configura os separadores
    if(token==NULL)                // se a linha e nula coninua a leitura de linhas.
      continue;

    slot_idx = strtol(token, NULL, 10);

    /* Validate values */
    if (slot_idx < PTIN_SYS_LC_SLOT_MIN || slot_idx > PTIN_SYS_LC_SLOT_MAX)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Invalid slot id (%u) in line %u", slot_idx, i);
      continue;
    }

    /* Get slot mode */
    token = strtok(NULL, seps);    // configura os separadores
    if(token==NULL)                // se a linha e nula coninua a leitura de linhas.
      continue;

    mode = strtol(token, NULL, 10);

    /* Validate values */
    if (mode < WC_SLOT_MODE_NONE || mode >= WC_SLOT_MODE_MAX)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Invalid slot mode (%u) in line %u", mode, i);
      continue;
    }

    slot_mode[slot_idx-1] = mode;
    LOG_TRACE(LOG_CTX_STARTUP, "Line=%u: slotIdx=%u slotmode=%u", i, slot_idx, mode);
  }
  fclose(fp);

  return L7_SUCCESS;
}

/**
 * Build a WC map from the array of port modes
 * 
 * @param slot_mode : Slot modes
 * @param retMap    : Map to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS - Valid map 
 *                   L7_NOT_SUPPORTED - Map not valid
 *                   L7_FAILURE - Error processing file
 */
static L7_RC_t hpcConfigWCmap_build(L7_uint32 *slot_mode, HAPI_WC_PORT_MAP_t *retMap)
{
  L7_int  i;
  L7_int  slot, port;
  L7_uint lane, total_lanes;

  L7_int  wc_index, wc_group, speedG;
  L7_uint32 bw_max[WC_MAX_GROUPS], ports_per_segment[WC_MAX_GROUPS/WC_SEGMENT_N_GROUPS];
  L7_BOOL wclanes_in_use[WC_MAX_NUMBER][WC_MAX_LANES];

  HAPI_WC_PORT_MAP_t wcMap[L7_MAX_PHYSICAL_PORTS_PER_UNIT];

  /* Validate arguments */
  if (slot_mode==L7_NULLPTR || retMap==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Clear temp var */
  memset(&wcMap, 0x00, sizeof(wcMap));
  memset(bw_max, 0x00, sizeof(bw_max));
  memset(ports_per_segment, 0x00, sizeof(ports_per_segment));
  memset(wclanes_in_use, 0x00, sizeof(wclanes_in_use));

  /* Run all provided slots */
  for (slot=1, port=0; slot<=PTIN_SYS_SLOTS_MAX && port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; slot++)
  {
    switch (slot_mode[slot-1])
    {
    case WC_SLOT_MODE_1x1G:
      speedG = 1;   total_lanes = 1;
      break;
    case WC_SLOT_MODE_2x1G:
      speedG = 1;   total_lanes = 2;
      break;
    case WC_SLOT_MODE_3x1G:
      speedG = 1;   total_lanes = 3;
      break;
    case WC_SLOT_MODE_4x1G:
      speedG = 1;   total_lanes = 4;
      break;
    case WC_SLOT_MODE_1x10G:
      speedG = 10;  total_lanes = 1;
      break;
    case WC_SLOT_MODE_2x10G:
      speedG = 10;  total_lanes = 2;
      break;
    case WC_SLOT_MODE_3x10G:
      speedG = 10;  total_lanes = 3;
      break;
    case WC_SLOT_MODE_4x10G:
      speedG = 10;  total_lanes = 4;
      break;
    case WC_SLOT_MODE_1x40G:
      speedG = 40;  total_lanes = 1;
      break;
    case WC_SLOT_MODE_2x40G:
      speedG = 40;  total_lanes = 2;
      break;
    case WC_SLOT_MODE_3x40G:
      speedG = 40;  total_lanes = 3;
      break;
    default:
      speedG = 0;   total_lanes = 0;
    }

    /* Run all lanes of each slot */
    for (i=0; i<total_lanes && port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++)
    {
      /* Find the first WC connected to this slot */
      for (wc_index=0; wc_index<WC_MAX_NUMBER; wc_index++)
      {
        /* Is this WC assigned to this slot? */
        if (dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].slotIdx != slot)
          continue;

        /* We have found a WC. Find the first free lane of this WC  */
        for (lane=0; lane<WC_MAX_LANES && wclanes_in_use[wc_index][lane]; lane++);
        /* Not found: go to the next WC */
        if (lane>=WC_MAX_LANES)
          continue;

        /* We have a WC index and a lane */

        /* Get WC group*/
        wc_group = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].wcGroup;
        if (wc_group >= WC_MAX_GROUPS)
        {
          LOG_ERR(LOG_CTX_STARTUP,"Invalid WC group (%u) for WC %u", wc_group, wc_index);
          return L7_FAILURE;
        }

        /* Check if there is available BW to use this lane: if there is, we have found a valid WC */
        if ((bw_max[wc_group]+speedG) <= WC_GROUP_MAX_BW &&
            (ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]+1) <= WC_SEGMENT_MAX_PORTS)
          break;
      }
      /* If no free and valid lanes were found, we have an error */
      if (wc_index >= WC_MAX_NUMBER)
      {
        LOG_ERR(LOG_CTX_STARTUP,"No WC found for slot %u, lane %u", slot, i);
        return L7_NOT_SUPPORTED;
      }

      /* We have a valid WC, and a valid lane */
      wcMap[port].portNum  = port;
      wcMap[port].slotNum  = slot;
      wcMap[port].wcIdx    = wc_index;
      wcMap[port].wcLane   = lane;
      wcMap[port].wcSpeedG = speedG;
      LOG_TRACE(LOG_CTX_STARTUP,"Port %u: slotNum %u, wcIdx=%u, wcLane=%u wcSpeedG=%u", port,
                wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);

      /* Lane is now in use */
      wclanes_in_use[wc_index][lane] = L7_TRUE;

      /* Update temp variables */
      ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]++;
      bw_max[wc_group] += speedG;
      port++;
    }
  }

  /* Fill remaining ports, with 1G ports */
  while (port<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
  {
    /* Run all WCs searching for free lanes */
    for (wc_index=0; wc_index<WC_MAX_NUMBER; wc_index++)
    {
      /* Skip not used WCs */
      if (dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].slotIdx < 0)
        continue;

      /* Find the first free lane of this WC  */
      for (lane=0; lane<WC_MAX_LANES && wclanes_in_use[wc_index][lane]; lane++);
      /* Not found: go to the next WC */
      if (lane>=WC_MAX_LANES)
        continue;

      /* We have a valid WC and lane */

      /* Get WC group*/
      wc_group = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].wcGroup;
      if (wc_group >= WC_MAX_GROUPS)
      {
        LOG_ERR(LOG_CTX_STARTUP,"Invalid WC group (%u) for WC %u", wc_group, wc_index);
        return L7_FAILURE;
      }

      /* Check if there is available BW to use this lane: if there is, we have found a valid WC */
      if ((bw_max[wc_group]+1) <= WC_GROUP_MAX_BW &&
          (ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]+1) <= WC_SEGMENT_MAX_PORTS)
        break;
    }
    /* If no free and valid lanes were found, we have an error */
    if (wc_index >= WC_MAX_NUMBER)
    {
      LOG_ERR(LOG_CTX_STARTUP,"No WC found for slot %u, lane %u", slot, i);
      return L7_NOT_SUPPORTED;
    }

    /* We have a valid WC, and a valid lane */
    wcMap[port].portNum  = port;
    wcMap[port].slotNum  = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].slotIdx;
    wcMap[port].wcIdx    = wc_index;
    wcMap[port].wcLane   = lane;
    wcMap[port].wcSpeedG = 1;       /* 1 Gbps */
    LOG_TRACE(LOG_CTX_STARTUP,"Port %u: slotNum %u, wcLane=%u wcSpeedG=1", port,
              wcMap[port].slotNum, wcMap[port].wcLane, wcMap[port]);

    /* Lane in use */
    wclanes_in_use[wc_index][lane] = L7_TRUE;

    /* Update temp variables */
    ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]++;
    bw_max[wc_group] += 1;    /* 1 Gbps */
    port++;
  }

  /* Return port map */
  memcpy(retMap, &wcMap, sizeof(wcMap));

  return L7_SUCCESS;
}

/**
 * Validate map
 * 
 * @param wcMap : WC map
 * 
 * @return L7_RC_t : L7_SUCCESS - Valid map 
 *                   L7_NOT_SUPPORTED - Map not valid
 *                   L7_FAILURE - Error processing file
 */
static L7_RC_t hpcConfigWCmap_validate(HAPI_WC_PORT_MAP_t *wcMap)
{
  L7_int    i;
  L7_int    wc_index, wc_group;
  L7_int    slot, port;
  L7_uint32 speedG, number_of_ports;
  L7_uint32 bw_max[WC_MAX_GROUPS], ports_per_segment[WC_MAX_GROUPS/WC_SEGMENT_N_GROUPS];

  /* Validate arguments */
  if (wcMap==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid arguments");
    return L7_FAILURE;
  }

  memset(bw_max, 0x00, sizeof(bw_max));
  memset(ports_per_segment, 0x00, sizeof(ports_per_segment));
  number_of_ports = 0;

  for (port=0; port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    /* Slot index */
    slot = wcMap[port].slotNum;

    /* Not defined */
    if (slot < 0)
    {
      continue;
    }
    if (slot >= PTIN_SYS_SLOTS_MAX)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid slot (%u) for port %u", slot, port);
      return L7_FAILURE;
    }

    /* Get wc index */
    wc_index = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[port].wcIdx;
    if (wc_index >= WC_MAX_NUMBER)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid WC index (%u)", wc_index);
      return L7_FAILURE;
    }
    if (dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].slotIdx != slot)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Inconsistence with slot references (port=%u, WC=%u)", port, wc_index);
      return L7_FAILURE;
    }

    /* Get WC group */
    wc_group = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_index].wcGroup;
    if (wc_group>=WC_MAX_GROUPS)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid WC group (%u) or WC index (%u)", wc_group, wc_index);
      return L7_FAILURE;
    }

    /* Speed in Gbps */
    speedG = wcMap[port].wcSpeedG;
    if (speedG!=1 && speedG!=10 && speedG!=40 && speedG!=100)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid speed (%u) for port %u", speedG, port);
      return L7_FAILURE;
    }

    bw_max[wc_group] += speedG;
    ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]++;
    number_of_ports++;
  }

  /* Check bw for each WC group */
  for (i=0; i<WC_MAX_GROUPS; i++)
  {
    if (bw_max[i] > WC_GROUP_MAX_BW)
    {
      LOG_ERR(LOG_CTX_STARTUP,"WC group %u is higher then %u Gbps", i, WC_GROUP_MAX_BW);
      return L7_NOT_SUPPORTED;
    }
  }

  /* Check port segments */
  for (i=0; i<WC_MAX_GROUPS/WC_SEGMENT_N_GROUPS; i++)
  {
    if (ports_per_segment[i] > WC_SEGMENT_MAX_PORTS)
    {
      LOG_ERR(LOG_CTX_STARTUP,"WC segment %u has more than %u ports (%u)", i, WC_SEGMENT_MAX_PORTS, ports_per_segment[i]);
      return L7_NOT_SUPPORTED;
    }
  }

  return L7_SUCCESS;
}

/**
 * Initialize WCs
 * 
 * @return L7_rc_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t hpcBoardWCinit_bcm56846(void)
{
  L7_uint16  port_idx, slot_idx;
  L7_uint16  wc_idx, wc_lane;
  L7_uint16  speedG;
  L7_uint8   invLanes, invPol;
  bcm_port_t bcm_port;
  HAPI_WC_SLOT_MAP_t *ptr;
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_1G   = {L7_PORT_DESC_BCOM_1G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_10G  = {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_40G  = {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_100G = {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN};
  char param_name[51], param_value[21];

  L7_uint32           slot_mode[PTIN_SYS_SLOTS_MAX];
  HAPI_WC_PORT_MAP_t  wcMap[L7_MAX_PHYSICAL_PORTS_PER_UNIT];

  memset(slot_mode, 0x00, sizeof(slot_mode));
  memset(wcMap, 0x00, sizeof(wcMap));

  LOG_INFO(LOG_CTX_STARTUP,"Board is %u matrix.", (cpld_map->reg.slot_id==0) ? "Working" : "Protection");

  /* If we are in protection side, we have to invert WCs */
  for (wc_idx=0; wc_idx<WC_MAX_NUMBER; wc_idx++)
  {
    ptr = &dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_idx];

    /* Skip not used WCs */
    if (ptr->slotIdx < 0)
      continue;

    /* Validate slot index */
    if (ptr->slotIdx == 0 || ptr->slotIdx > PTIN_SYS_SLOTS_MAX)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid slot index (%u) for WC %u!",ptr->slotIdx, wc_idx);
      return L7_FAILURE;
    }

    /* If we are in protection side, we have to invert WCs */
    if (cpld_map->reg.slot_id!=0)
    {
      ptr->slotIdx = PTIN_SYS_SLOTS_MAX - ptr->slotIdx + 1;
    }

    LOG_DEBUG(LOG_CTX_STARTUP," WC%02u: WCgroup=%u slot=%-2u (invLanes=0x%02x invPol=0x%02x)",
              ptr->wcIndex, ptr->wcGroup, ptr->slotIdx, ptr->invert_lanes, ptr->invert_polarities);
  }

  LOG_INFO(LOG_CTX_STARTUP,"Trying to open \"%s\" file...",WC_MAP_FILE);

  /* Get slot modes from file */
  if (hpcConfigWCmap_read(WC_MAP_FILE, slot_mode)==L7_SUCCESS)
  {
    memcpy(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1, slot_mode, sizeof(slot_mode));
    LOG_INFO(LOG_CTX_STARTUP,"Slot mode list updated successfully");
  }
  else
  {
    LOG_WARNING(LOG_CTX_STARTUP,"Error opening file \"%s\". Going to assume default slot mode list.",WC_MAP_FILE);
  }

  /* Create map */
  if (hpcConfigWCmap_build(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1, wcMap)==L7_SUCCESS)
  {
    memcpy(dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1, wcMap, sizeof(wcMap));
    LOG_INFO(LOG_CTX_STARTUP,"WC map updated successfully");
  }
  else
  {
    LOG_ERR(LOG_CTX_STARTUP,"Error creating WC map! Assuming default WC map.");
  }

  /* Validate map */
  if (hpcConfigWCmap_validate(dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Not valid WC map!");
    return L7_FAILURE;
  }

  LOG_INFO(LOG_CTX_STARTUP,"WC map is valid!");

  /* Run all ports */
  for (port_idx=0; port_idx<L7_MAX_PHYSICAL_PORTS_PER_UNIT; port_idx++)
  {
    slot_idx = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx].slotNum;             /* Slot index */
    wc_idx   = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx].wcIdx;               /* WC index */
    wc_lane  = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx].wcLane;              /* WC lane index */
    speedG   = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx].wcSpeedG;            /* Speed in GB */

    invLanes = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_idx].invert_lanes;          /* Invert lanes? */
    invPol   = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[wc_idx].invert_polarities;     /* Invert polarities? */

    bcm_port = dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx].bcm_port;          /* bcm_port value */

    /* Update speed */
    switch (speedG)
    {
      case 1:
        hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx] = port_descriptor_1G;
        break;
      case 10:
        hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx] = port_descriptor_10G;
        break;
      case 40:
        hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx] = port_descriptor_40G;
        break;
      case 100:
        hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx] = port_descriptor_100G;
        break;
      default:
        hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[port_idx] = port_descriptor_1G;
        break;
    }
    
    /* Apply wc map */
    sprintf(param_name,  "%s_%u", spn_PORTMAP, bcm_port);
    sprintf(param_value, "%u:%u", wc_idx*4+wc_lane+1, speedG);
    if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);

    /* Invert lanes? */
    if (invLanes)
    {
      sprintf(param_name,  "%s_%u", spn_XGXS_TX_LANE_MAP, bcm_port);
      sprintf(param_value, "0x3210");
      if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);
    }

    /* Invert RX polarity? */
    if (invPol & 1)
    {
      sprintf(param_name,  "%s_%u", spn_PHY_XAUI_RX_POLARITY_FLIP, bcm_port);
      sprintf(param_value, "%u", ((L7_uint16) 0x000f << (wc_lane*4)));
      if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);
    }
    /* Invert TX polarity? */
    if (invPol & 2)
    {
      sprintf(param_name,  "%s_%u", spn_PHY_XAUI_TX_POLARITY_FLIP, bcm_port);
      sprintf(param_value, "%u", ((L7_uint16) 0x000f << (wc_lane*4)));
      if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);
    }
  }

  LOG_INFO(LOG_CTX_STARTUP,"WC map applied successfully!");

  return L7_SUCCESS;
}
#endif

