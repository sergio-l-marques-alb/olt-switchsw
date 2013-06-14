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

        /* For CXO640G-V1 */
        #if (PTIN_BOARD == PTIN_BOARD_CXO640G_V1)
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

        #elif (PTIN_BOARD == PTIN_BOARD_CXO640G)
        /* For CXO640G-V2 */

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
        if (sal_config_set(spn_PORTMAP"_61",  "65:1") != 0) return(L7_FAILURE); // GC1: WC0  lane 0
        if (sal_config_set(spn_PORTMAP"_62",  "66:1") != 0) return(L7_FAILURE); // GC3: WC16 lane 0
        if (sal_config_set(spn_PORTMAP"_63",  "69:1") != 0) return(L7_FAILURE); // GC3: WC17 lane 0
        if (sal_config_set(spn_PORTMAP"_64",  "70:1") != 0) return(L7_FAILURE); // GC3: WC17 lane 1

        /* Swap RX and TX polarities of WC 12 (slot 18) */
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_53", "0x0f00") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_53", "0x0f00") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_54", "0xf000") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_54", "0xf000") != 0) return(L7_FAILURE);
        LOG_INFO(LOG_CTX_MISC, "Polarities inverted!");

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

        #elif (PTIN_BOARD == PTIN_BOARD_CXO640G_V2)
        /* For the future: new slot mapping */
        /* Slot 2, 3, 18, 19: 20G; e o resto a 40G */

        /* Slot 2: WC1: 5-6 (20G) */
        if (sal_config_set(spn_PORTMAP"_1",   "7:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_2",   "8:10") != 0) return(L7_FAILURE);

        /* Slot 3: WC2: 9-10 (20G) */
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

        /* Slot 18: WC12: 49-50 (20G) */
        if (sal_config_set(spn_PORTMAP"_61", "51:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_62", "52:10") != 0) return(L7_FAILURE);

        /* Slot 19: WC13: 53-54 (20G) */
        if (sal_config_set(spn_PORTMAP"_63", "55:10") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORTMAP"_64", "56:10") != 0) return(L7_FAILURE);

        LOG_INFO(LOG_CTX_MISC, "Portmap defined!");

        /* Swap RX and TX polarities of WC 12 */
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_61", "0x0f00") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_61", "0x0f00") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_TX_POLARITY_FLIP"_62", "0xf000") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PHY_XAUI_RX_POLARITY_FLIP"_62", "0xf000") != 0) return(L7_FAILURE);
        LOG_INFO(LOG_CTX_MISC, "Polarities inverted!");

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

