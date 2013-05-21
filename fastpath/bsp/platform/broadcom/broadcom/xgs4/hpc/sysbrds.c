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
          LOG_TRACE(LOG_CTX_MISC, "Using Port Expanders mapping for CXP360G V2 or below");

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
          LOG_TRACE(LOG_CTX_MISC, "Using Port Expanders mapping for CXP360G V3 or higher");

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
        /* Modified */
        #if 1
        if (sal_config_set(spn_PORTMAP"_49", "61:40") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_50", "61:40") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_51", "61:40") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_52", "61:40") != 0) return(L7_FAILURE);
        #else
        if (sal_config_set(spn_PORTMAP"_49", "61:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_50", "62:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_51", "63:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_52", "64:10") != 0) return(L7_FAILURE);
        #endif

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

#if 0
// slot 2 e 14 off
        if (sal_config_set(spn_PORTMAP"_1",   "5:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_2",   "6:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_3",   "7:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_4",   "8:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_5",   "9:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_6",  "10:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_7",  "11:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_8",  "12:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_9",  "13:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_10", "14:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_11", "15:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_12", "16:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_13", "17:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_14", "18:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_15", "19:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_16", "20:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_17", "21:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_18", "22:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_19", "23:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_20", "24:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_21", "25:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_22", "26:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_23", "27:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_24", "28:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_25", "29:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_26", "30:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_27", "31:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_28", "32:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_29", "33:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_30", "34:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_31", "35:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_32", "36:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_33", "37:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_34", "38:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_35", "39:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_36", "40:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_37", "41:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_38", "42:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_39", "43:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_40", "44:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_41", "45:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_42", "46:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_43", "47:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_44", "48:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_45", "53:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_46", "54:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_47", "55:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_48", "56:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_49", "57:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_50", "58:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_51", "59:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_52", "60:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_53", "61:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_54", "62:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_55", "63:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_56", "64:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_57", "65:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_58", "66:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_59", "67:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_60", "68:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_61", "69:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_62", "70:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_63", "71:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_64", "72:10") != 0) return(L7_FAILURE);
#endif

#if 0
        if (sal_config_set(spn_PORTMAP"_1",   "1:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_2",   "2:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_3",   "3:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_4",   "4:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_5",   "5:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_6",   "6:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_7",   "7:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_8",   "8:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_9",  "13:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_10", "14:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_11", "15:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_12", "16:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_13", "21:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_14", "22:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_15", "23:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_16", "24:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_17", "25:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_18", "26:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_19", "27:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_20", "28:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_21", "33:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_22", "34:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_23", "35:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_24", "36:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_25", "37:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_26", "38:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_27", "39:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_28", "40:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_29", "45:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_30", "46:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_31", "47:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_32", "48:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_33", "49:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_34", "50:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_35", "51:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_36", "52:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_37", "57:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_38", "58:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_39", "59:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_40", "60:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_41", "65:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_42", "66:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_43", "67:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_44", "68:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_45", "69:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_46", "70:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_47", "71:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_48", "72:10") != 0) return(L7_FAILURE);
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

//      if (sal_config_set(spn_PORTMAP"_1",   "1:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_2",   "2:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_3",   "3:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_4",   "4:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_5",   "5:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_6",   "6:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_7",   "7:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_8",   "8:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_9",   "9:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_10", "10:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_11", "11:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_12", "12:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_13", "13:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_14", "14:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_15", "15:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_16", "16:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_17", "17:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_18", "18:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_19", "19:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_20", "20:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_21", "21:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_22", "22:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_23", "23:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_24", "24:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_25", "25:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_26", "26:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_27", "27:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_28", "28:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_29", "29:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_30", "30:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_31", "31:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_32", "32:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_33", "33:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_34", "34:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_35", "35:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_36", "36:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_37", "37:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_38", "38:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_39", "39:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_40", "40:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_41", "41:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_42", "42:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_43", "43:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_44", "44:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_45", "45:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_46", "46:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_47", "47:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_48", "48:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_49", "49:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_50", "50:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_51", "51:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_52", "52:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_53", "53:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_54", "54:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_55", "55:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_56", "56:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_57", "57:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_58", "58:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_59", "59:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_60", "60:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_61", "61:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_62", "62:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_63", "63:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_64", "64:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_65", "65:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_66", "66:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_67", "67:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_68", "68:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_69", "69:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_70", "70:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_71", "71:10") != 0) return(L7_FAILURE);
//      if (sal_config_set(spn_PORTMAP"_72", "72:10") != 0) return(L7_FAILURE);

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

