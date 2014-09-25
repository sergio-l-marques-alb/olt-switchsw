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
#include <stdlib.h>
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
#include "broad_hpc_db.h"

#include "ptin_globaldefs.h"  /* PTin added */
#include "logger.h"           /* PTin added */

#if (PTIN_BOARD == PTIN_BOARD_CXO640G)

const HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_V1[] =
/*  WC index  WC group  Inv.Lanes Inv.Pol. SlotIdx *
 * --------- --------- ---------  -------  -------*/
{{        0,        0,        0,       0,       2 },
{         1,        0,        0,       0,       3 },
{         2,        0,        0,       0,       4 },
{         3,        0,        0,       0,       5 },
{         4,        0,        0,       0,       6 },
{         5,        1,        0,       0,       7 },
{         6,        1,        0,       0,       8 },
{         7,        1,        0,       0,       9 },
{         8,        1,        0,       0,      10 },
{         9,        2,        0,       0,      11 },
{        10,        2,        0,       0,      12 },
{        11,        2,        0,       0,      13 },
{        12,        2,        0,       0,      14 },
{        13,        2,        0,       0,      15 },  
{        14,        3,        0,       0,      16 },
{        15,        3,        0,       0,      17 },
{        16,        3,        0,       0,      18 },
{        17,        3,        0,       0,      19 }};

const HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_V2[] =
/*  WC index  WC group  Inv.Lanes Inv.Pol. SlotIdx *
 * --------- --------- ---------  -------  -------*/
{{        0,        0,        1,       0,       6 },
{         1,        0,        1,       0,       2 },
{         2,        0,        1,       0,       3 },
{         3,        0,        1,       0,       5 },
{         4,        0,        1,       0,       4 },
{         5,        1,        1,       0,       7 },
{         6,        1,        1,       0,       8 },
{         7,        1,        1,       0,       9 },
{         8,        1,        1,       0,      10 },
{         9,        2,        1,       0,      11 },
{        10,        2,        1,       0,      12 },
{        11,        2,        1,       0,      13 },
{        12,        2,        1,       3,      18 },  /* RX and TX polarities inverted for WC 12: slot 18 */
{        13,        2,        1,       0,      19 },  
{        14,        3,        1,       0,      16 },
{        15,        3,        1,       0,      17 },
{        16,        3,        0,       0,      14 },  /* Lanes inverted for WC 16: slot 14 */ 
{        17,        3,        0,       0,      15 }}; /* Lanes inverted for WC 17: slot 15 */

L7_RC_t hpcBoardWCinit_bcm56846(void);

#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)
L7_RC_t hpcBoardWCinit_bcm56640(void);
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
    LOG_TRACE(LOG_CTX_STARTUP,"UNIT=0x%08x",lclUnitDesc->unitTypeDescriptor.unitTypeId);

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
      case UNIT_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID:   /* PTin added: new switch 56689 (Valkyrie2) */
        /* Enable trunk_128 bit. This will enable 128 trunks */
        /* and fixes LAG issue on XGS3 stacking              */
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);
#ifdef L7_STACKING_PACKAGE
        /* On Stacking packages, restrict FDB size to 16K MAX for FB2. */
        if (sal_config_set("l2_table_size", "0x3fff") != 0)
          return(L7_FAILURE);
#endif
        #if 0
        if (sal_config_set(spn_POLLED_IRQ_MODE, "1") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TDMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_SCHAN_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MIIM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MEMCMD_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_L2MOD_DMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TABLE_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA disabled!");
        #else
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA are enabled!");
        #endif
        break;

      /* PTin added: new switch 5664x (Triumph3) SF */
      case UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID:

        /* Enable trunk_128 bit. This will enable 128 trunks */
        /* and fixes LAG issue on XGS3 stacking              */
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0)
          return(L7_FAILURE);

        /* Configure mmu lossy mode */
        if (sal_config_set(spn_MMU_LOSSLESS, "0") != 0)
          return(L7_FAILURE);

        #if 0
        if (sal_config_set(spn_POLLED_IRQ_MODE, "1") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TDMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_SCHAN_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MIIM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MEMCMD_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_L2MOD_DMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TABLE_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA disabled!");
        #else
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA are enabled!");
        #endif

        /* Ports mode */
        if (sal_config_set(spn_BCM56640_3X42_4X32, "1") != 0)
          return(L7_FAILURE);

        /* XE ports configuration */
        if (sal_config_set(spn_PBMP_XPORT_XE, "0x3fffffe0") != 0)
          return(L7_FAILURE);

        /* For CXO640G */
        #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
        if (hpcBoardWCinit_bcm56640() == L7_SUCCESS)
        {
          LOG_NOTICE(LOG_CTX_STARTUP,"WCs initialized successfully");
        }
        else
        {
          LOG_ERR(LOG_CTX_STARTUP,"Error initializing WCs");
        }
        #endif

        LOG_WARNING(LOG_CTX_STARTUP,"Triumph3-SF ready to be started!");
        break;

      /* PTin added: new switch 5664x (Triumph3) */
      case UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1_ID:
        /* Enable trunk_128 bit. This will enable 128 trunks */
        /* and fixes LAG issue on XGS3 stacking              */
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0)
          return(L7_FAILURE);

        /* Configure mmu lossy mode */
        if (sal_config_set(spn_MMU_LOSSLESS, "0") != 0)
          return(L7_FAILURE);

        #if 0
        if (sal_config_set(spn_POLLED_IRQ_MODE, "1") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TDMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_SCHAN_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MIIM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MEMCMD_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_L2MOD_DMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TABLE_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA disabled!");
        #else
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA are enabled!");
        #endif

      #if (SDK_VERSION_IS < SDK_VERSION(6,3,7,0))
        if (sal_config_set(spn_PORT_PHY_ADDR"_1",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_2",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_3",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_4",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_5",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_6",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_7",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_8",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_9",  "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_10", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_11", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_12", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_13", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_14", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_15", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_16", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_17", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_18", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_19", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_20", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_21", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_22", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_23", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_24", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_25", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_26", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_27", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_28", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_29", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_30", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_31", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_32", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_33", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_34", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_35", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_36", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_37", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_38", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_39", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_40", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_41", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_42", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_43", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_44", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_45", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_46", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_47", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_48", "0xff") != 0) return(L7_FAILURE);
        if (sal_config_set(spn_PORT_PHY_ADDR"_49", "0xff") != 0) return(L7_FAILURE);

        if (sal_config_set("phy_wcmod_1",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_2",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_3",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_4",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_5",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_6",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_7",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_8",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_9",  "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_10", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_11", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_12", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_13", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_14", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_15", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_16", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_17", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_18", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_19", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_20", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_21", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_22", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_23", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_24", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_25", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_26", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_27", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_28", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_29", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_30", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_31", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_32", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_33", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_34", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_35", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_36", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_37", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_38", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_39", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_40", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_41", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_42", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_43", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_44", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_45", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_46", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_47", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_48", "0") != 0) return(L7_FAILURE);
        if (sal_config_set("phy_wcmod_49", "0") != 0) return(L7_FAILURE);
      #endif

#ifdef L7_STACKING_PACKAGE
        /* On Stacking packages, restrict FDB size to 16K MAX for FB2. */
        if (sal_config_set("l2_table_size", "0x3fff") != 0)
          return(L7_FAILURE);
#endif
        break;

    /* PTin added: new switch 56340 (Helix4) */
    case UNIT_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID:
      /* Enable trunk_128 bit. This will enable 128 trunks */
      /* and fixes LAG issue on XGS3 stacking              */
      if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
        return(L7_FAILURE);

      /* Configure to use LCPLL reference clock */
      if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0)
        return(L7_FAILURE);

      /* PCI device ID override: when switch id is uncorrectly identified as 0xb34f */
      if (sal_config_set(spn_PCI_OVERRIDE_DEV, "0xb340") != 0)
        return(L7_FAILURE);

      /* Enable 12xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE mode for BCM56340 */
      if (sal_config_set(spn_BCM56340_4X10, "1") != 0)
        return(L7_FAILURE);

      /*
       * On 568xx devices, the XPORT block defaults to XE ports.  Uncomment the
       * following line to change all ports to HG ports.  A specific bitmap
       * may be provided to select some XE and some HG ports, with the set
       * bits initialized to HG ports.  Note that HG and XE ports may be
       * exchanged through the bcm_port_encap_set API.
       */
      if (sal_config_set(spn_PBMP_XPORT_XE, "0x3c000000000000") != 0)
        return(L7_FAILURE);

      /* Configuring GS port (ge48/49) */
      /* Disable signal auto-detection between SGMII and fiber
       *  Note this only works when auto-negotiation is enabled. */
      if (sal_config_set(spn_SERDES_AUTOMEDIUM"_49", "0") != 0)
        return(L7_FAILURE);
      /* Manually select SGMII (when auto-detection is off) */
      if (sal_config_set(spn_SERDES_FIBER_PREF"_49", "0") != 0)
        return(L7_FAILURE);

      /* Specifies the number of lanes used by each port in the flex port group.
       * portgroup_<port group>=<number of lanes>.
       * Applicable to BCM566xx and BCM565xx device family */
      if (sal_config_set(spn_PORTGROUP"_0", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_1", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_2", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_3", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_4", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_5", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_6", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_7", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_8", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_9", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_10", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_11", "4") != 0)
        return(L7_FAILURE);
      if (sal_config_set(spn_PORTGROUP"_12", "1") != 0)   /* WC group: 10G ports */
        return(L7_FAILURE);

      /* Configure mmu lossy mode */
      if (sal_config_set(spn_MMU_LOSSLESS, "0") != 0)
        return(L7_FAILURE);

#ifdef L7_STACKING_PACKAGE
        /* On Stacking packages, restrict FDB size to 16K MAX for FB2. */
        if (sal_config_set("l2_table_size", "0x3fff") != 0)
          return(L7_FAILURE);
#endif

      LOG_TRACE(LOG_CTX_STARTUP,"Helix4 ready to be started!");
      break;

      /* PTin added: new switch 56843 (Trident) */
      case UNIT_BROAD_40_TENGIG_56843_REV_1_ID:
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0) return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0) return(L7_FAILURE);

        /* Configure Portmaps: different mapping for CXP360G V2 and V3 */
        #ifdef MAP_CPLD
        if (cpld_map->reg.hw_ver <= 2)
        #else
        if (0)
        #endif
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

      /* PTin added: new switch 56846 (Trident-plus) */
      case UNIT_BROAD_64_TENGIG_56846_REV_1_ID:
        #if 0
        if (sal_config_set(spn_POLLED_IRQ_MODE, "1") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TDMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_SCHAN_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MIIM_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_MEMCMD_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_L2MOD_DMA_INTR_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TSLAM_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        if (sal_config_set(spn_TABLE_DMA_ENABLE, "0") != 0)
          return(L7_FAILURE);
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA disabled!");
        #else
        LOG_NOTICE(LOG_CTX_MISC,"Interrupts and DMA are enabled!");
        #endif

        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0) return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0) return(L7_FAILURE);

        /* Configure mmu lossy mode */
        if (sal_config_set(spn_MMU_LOSSLESS, "0") != 0)
          return(L7_FAILURE);

        /* For CXO640G */
        #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
        if (hpcBoardWCinit_bcm56846() == L7_SUCCESS)
        {
          LOG_NOTICE(LOG_CTX_STARTUP,"WCs initialized successfully");
        }
        else
        {
          LOG_ERR(LOG_CTX_STARTUP,"Error initializing WCs");
        }

        /* Disable BAM */
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

#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)
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
L7_RC_t hpcConfigWCmap_read(char *filename, L7_uint32 *slot_mode)
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
  memset(slot_mode, 0x00, sizeof(L7_uint32)*PTIN_SYS_SLOTS_MAX);

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
      LOG_WARNING(LOG_CTX_STARTUP, "Invalid slot id (%u) in line %u", slot_idx, i);
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
#endif

#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
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

  /* Pointer to WC map */
  HAPI_WC_SLOT_MAP_t *WCSlotMap;

  /* Validate arguments */
  if (wcMap==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Select WC map */
  WCSlotMap = is_matrix_protection() ? dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT :
                                       dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK;

  memset(bw_max, 0x00, sizeof(bw_max));
  memset(ports_per_segment, 0x00, sizeof(ports_per_segment));
  number_of_ports = 0;

  for (port=0; port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    if (wcMap[port].wcSpeedG == 0) continue;

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
    if (WCSlotMap[wc_index].slotIdx != slot)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Inconsistence with slot references (port=%u, WC=%u)", port, wc_index);
      return L7_FAILURE;
    }

    /* Get WC group */
    wc_group = WCSlotMap[wc_index].wcGroup;
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
  L7_uint16  i;
  L7_uint16  port_idx, slot_idx;
  L7_uint16  wc_idx, wc_lane;
  L7_uint16  speedG;
  L7_uint8   invLanes, invPol;
  bcm_port_t bcm_port;
  HAPI_WC_SLOT_MAP_t *ptr;
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_1G   = {L7_PORT_DESC_BCOM_1G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_10G  = {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_40G  = {L7_PORT_DESC_BCOM_40G_KR4};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_100G = {L7_PORT_DESC_BCOM_100G_BKP};
  char param_name[51], param_value[21];
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;

  L7_uint32           slot_mode[PTIN_SYS_SLOTS_MAX];
  HAPI_WC_PORT_MAP_t  wcMap[L7_MAX_PHYSICAL_PORTS_PER_UNIT];

  HAPI_WC_SLOT_MAP_t *WCSlotMap;

  memset(slot_mode, 0x00, sizeof(slot_mode));
  memset(wcMap, 0x00, sizeof(wcMap));

  LOG_INFO(LOG_CTX_STARTUP,"Board is %s matrix.", (is_matrix_protection() ? "Protection" : "Working"));

  LOG_DEBUG(LOG_CTX_STARTUP,"Initializing WC map:");

  /* Different WC base maps */

  /* Copy base WC map to Working WC map */
  switch (matrix_board_version())
  {
  case 1:
    memcpy(dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK,        /* Copy to working */
           dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_V1,
           sizeof(HAPI_WC_SLOT_MAP_t)*WC_MAX_NUMBER);
    break;
  default:
    memcpy(dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK,        /* Copy to working */
           dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_V2,
           sizeof(HAPI_WC_SLOT_MAP_t)*WC_MAX_NUMBER);
    break;
  }

  /* Copy to protection WC map */
  memcpy(dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT,
         dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK,
         sizeof(HAPI_WC_SLOT_MAP_t)*WC_MAX_NUMBER);

  /* Invert WCs for the protection matrix */
  for (wc_idx=0; wc_idx<WC_MAX_NUMBER; wc_idx++)
  {
    ptr = &dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT[wc_idx];

    /* Skip not used WCs */
    if (ptr->slotIdx < 0)
      continue;

    /* Validate slot index */
    if (ptr->slotIdx == 0 || ptr->slotIdx > PTIN_SYS_SLOTS_MAX)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid slot index (%u) for WC %u!",ptr->slotIdx, wc_idx);
      return L7_FAILURE;
    }

    /* Invert WCs */
    ptr->slotIdx = PTIN_SYS_SLOTS_MAX - ptr->slotIdx + 1;

    LOG_DEBUG(LOG_CTX_STARTUP," WC%02u: WCgroup=%u slot=%-2u (invLanes=0x%02x invPol=0x%02x)",
              ptr->wcIndex, ptr->wcGroup, ptr->slotIdx, ptr->invert_lanes, ptr->invert_polarities);
  }

  /* Select WC map */
  WCSlotMap = is_matrix_protection() ? dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT :
                                       dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK;

  LOG_INFO(LOG_CTX_STARTUP,"Trying to open \"%s\" file...",WC_MAP_FILE);

  /* Read map from file */
  /* Get slot modes from file */
  if (hpcConfigWCmap_read(WC_MAP_FILE, slot_mode) == L7_SUCCESS)
  {
    /* Test new map */
    if (hpcConfigWCmap_build(slot_mode, L7_NULLPTR) == L7_SUCCESS)
    {
      memcpy(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1, slot_mode, sizeof(slot_mode));
      LOG_INFO(LOG_CTX_STARTUP,"Slot mode list is valid! Slot mode list updated successfully");
    }
    else
    {
      LOG_ERR(LOG_CTX_STARTUP,"Error validating WC map! Assuming default slot mode list.");
    }
  }
  else
  {
    LOG_WARNING(LOG_CTX_STARTUP,"Error opening file \"%s\". Going to assume default slot mode list.",WC_MAP_FILE);
  }
  LOG_DEBUG(LOG_CTX_STARTUP,"Slot map:");
  for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
  {
    LOG_DEBUG(LOG_CTX_STARTUP," Slot %02u: Mode=%u", i+1, dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1[i]);
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
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_STARTUP,"Port map:");
  for (i=0; i<L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++)
  {
    LOG_DEBUG(LOG_CTX_STARTUP," Port %02u: Slot=%02u WCidx=%02u WClane=%u Speed=%uG",
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[i].portNum,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[i].slotNum,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[i].wcIdx,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[i].wcLane,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[i].wcSpeedG);
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

    if (speedG == 0)  break;

    invLanes = WCSlotMap[wc_idx].invert_lanes;          /* Invert lanes? */
    invPol   = WCSlotMap[wc_idx].invert_polarities;     /* Invert polarities? */

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
      sprintf(param_value, "0x0123");
      if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);
    }

    /* Invert RX polarity? */
    if (invPol & 1)
    {
      sprintf(param_name,  "%s_%u", spn_PHY_XAUI_RX_POLARITY_FLIP, bcm_port);
      sprintf(param_value, "%u", /*(speedG<=10) ? ((L7_uint16) 0x000f << (wc_lane*4)) :*/ 0xffff);
      if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);
    }
    /* Invert TX polarity? */
    if (invPol & 2)
    {
      sprintf(param_name,  "%s_%u", spn_PHY_XAUI_TX_POLARITY_FLIP, bcm_port);
      sprintf(param_value, "%u", /*(speedG<=10) ? ((L7_uint16) 0x000f << (wc_lane*4)) :*/ 0xffff);
      if (sal_config_set(param_name, param_value) != 0)  return(L7_FAILURE);
    }
  }

  /* Effective number of ports */
  dapiBroadPhysicalCardEntry_CARD_BROAD_64_TENGIG_56846_REV_1.numOfSlotMapEntries = port_idx;
  dapiBroadPhysicalCardEntry_CARD_BROAD_64_TENGIG_56846_REV_1.numOfPortMapEntries = port_idx;

  /* Update maximum number of interfaces */
  for (i = 0; i < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; i++)
  {
    sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(i));

    if (sysapiHpcCardInfoPtr != L7_NULLPTR)
      sysapiHpcCardInfoPtr->numOfNiPorts = port_idx;
    else
      LOG_ERR(LOG_CTX_STARTUP,"Error updating number of ports for slotIndex %u!", i);
  }

  LOG_INFO(LOG_CTX_STARTUP,"WC map applied successfully with %u ports!",port_idx);

  return L7_SUCCESS;
}

#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)

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
  L7_int    port;

  /* Validate arguments */
  if (wcMap==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Run all ports */
  for (port=0; port < L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    /* Should we validate this entry? */
    if (wcMap[port].slotNum < 0 || wcMap[port].wcSpeedG == 0)
      continue;

    /* Validate slotnum */
    if (wcMap[port].slotNum < PTIN_SYS_LC_SLOT_MIN || wcMap[port].slotNum > PTIN_SYS_LC_SLOT_MAX)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid slot (%u) for port %u", wcMap[port].slotNum, port);
      return L7_FAILURE;
    }

    /* Validate lane */
    if (wcMap[port].wcLane < 0 || wcMap[port].wcLane >= WC_MAX_LANES)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid lane (%u) for port %u", wcMap[port].wcLane, port);
      return L7_FAILURE;
    }
    
    /* Validate speed */
    if (wcMap[port].wcSpeedG != 1 && wcMap[port].wcSpeedG != 10 && wcMap[port].wcSpeedG != 20 && wcMap[port].wcSpeedG != 40)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Invalid speed (%u) for port %u", wcMap[port].wcSpeedG, port);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Initialize Warpcores for BCM56640
 * 
 * @return L7_RC_t 
 */
L7_RC_t hpcBoardWCinit_bcm56640(void)
{
  L7_int  gport_idx, fport_idx, bport_idx, port_idx, offset, i;
  L7_int  slot, lane, speed, portgroup;
  HAPI_CARD_SLOT_MAP_t *dapiBroadBaseCardSlotMap;
  HAPI_WC_PORT_MAP_t wcMap[L7_MAX_PHYSICAL_PORTS_PER_UNIT];
  L7_uint32 slot_mode[PTIN_SYS_SLOTS_MAX];
  char param_name[51], param_value[21];
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_1G     = {L7_PORT_DESC_BCOM_1G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_10G_AN = {L7_PORT_DESC_BCOM_XAUI_10G_1G};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_10G    = {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN};
  SYSAPI_HPC_PORT_DESCRIPTOR_t port_descriptor_40G    = {L7_PORT_DESC_BCOM_40G_KR4};

  memset(wcMap, 0x00, sizeof(wcMap));
  memset(slot_mode, 0x00, sizeof(slot_mode));

  LOG_INFO(LOG_CTX_STARTUP,"Board is %s matrix.", (is_matrix_protection() ? "Protection" : "Working"));

  /* Read map from file */
  /* Get slot modes from file */
  if (hpcConfigWCmap_read(WC_MAP_FILE, slot_mode) == L7_SUCCESS)
  {
    /* Test new map */
    if (hpcConfigWCmap_build(slot_mode, L7_NULLPTR) == L7_SUCCESS)
    {
      memcpy(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1, slot_mode, sizeof(slot_mode));
      LOG_INFO(LOG_CTX_STARTUP,"Slot mode list is valid! Slot mode list updated successfully");
    }
    else
    {
      LOG_ERR(LOG_CTX_STARTUP,"Error validating WC map! Assuming default slot mode list.");
    }
  }
  else
  {
    LOG_WARNING(LOG_CTX_STARTUP,"Error opening file \"%s\". Going to assume default slot mode list.",WC_MAP_FILE);
  }

  /* Printing out slot mode list */
  LOG_DEBUG(LOG_CTX_STARTUP,"Slot map:");
  for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
  {
    LOG_DEBUG(LOG_CTX_STARTUP," Slot %02u: Mode=%u", i+1, dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1[i]);
  }

  /* Create map */
  if (hpcConfigWCmap_build(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1, wcMap) == L7_SUCCESS)
  {
    memcpy(dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1, wcMap, sizeof(wcMap));
    LOG_INFO(LOG_CTX_STARTUP,"WC map updated successfully");
  }
  else
  {
    LOG_ERR(LOG_CTX_STARTUP,"Error creating WC map! Assuming default WC map.");
    return L7_FAILURE;
  }

  /* Printing out port map */
  LOG_DEBUG(LOG_CTX_STARTUP,"Port map:");
  for (i=0; i<L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++)
  {
    LOG_DEBUG(LOG_CTX_STARTUP," Port %02u: Slot=%2d WCidx=%2d WClane=%d Speed=%2uG",
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[i].portNum,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[i].slotNum,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[i].wcIdx,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[i].wcLane,
              dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[i].wcSpeedG);
  }

  /* Validate map */
  if (hpcConfigWCmap_validate(dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Not valid WC map!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_STARTUP,"WC map is valid!");

  /* Pointer to port list */
  dapiBroadBaseCardSlotMap = dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1;

  memset(dapiBroadBaseCardSlotMap, 0x00, sizeof(HAPI_CARD_SLOT_MAP_t)*L7_MAX_PHYSICAL_PORTS_PER_UNIT);

  bport_idx = 0;
  fport_idx = 0;
  gport_idx = 0;
  /* Portgroup mapping */
  for (port_idx = 0; port_idx < L7_MAX_PHYSICAL_PORTS_PER_UNIT; port_idx++)
  {
    LOG_INFO(LOG_CTX_STARTUP,"port_idx=%d",port_idx);
    slot  = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[port_idx].slotNum;
    lane  = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[port_idx].wcLane;
    speed = dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[port_idx].wcSpeedG;
    LOG_INFO(LOG_CTX_STARTUP,"data obtained for port_idx=%2d: slot=%d, lane=%d, speed=%2d", port_idx, slot, lane, speed);

    /* Speed == 0, signals end of port sweeping */
    if (speed == 0)
      break;

    /* Backplane port */
    if (slot >= PTIN_SYS_LC_SLOT_MIN && slot <= PTIN_SYS_LC_SLOT_MAX)
    {
      LOG_INFO(LOG_CTX_STARTUP,"Slot valid for port_idx=%d (%d)", port_idx, slot);

      /* Validate frontal port index */
      if (bport_idx >= CXO160G_BACKPLANE_PORTS)
      {
        LOG_INFO(LOG_CTX_STARTUP, "Only %u backplane ports are allowed!", CXO160G_BACKPLANE_PORTS);
        return L7_FAILURE;
      }

      /* Calculate portgroup from slot id */
      portgroup = (is_matrix_protection()) ? (PTIN_SYS_LC_SLOT_MAX - slot) : (slot - PTIN_SYS_LC_SLOT_MIN);

      /* Only apply portgroup configuration once port WC */
      if (lane == 0)
      {
        LOG_INFO(LOG_CTX_STARTUP, "port_idx=%d: portgroup=%d", port_idx, portgroup); 
        sprintf(param_name, spn_PORTGROUP"_%u", portgroup);
        sprintf(param_value, "%u", (speed == 40) ? 4 : ((speed == 20) ? 2 : 1));

        LOG_INFO(LOG_CTX_STARTUP, "slot=%d: sal_config_set(%s,%s)", slot, param_name, param_value);
        if (sal_config_set(param_name, param_value) != 0)
          return(L7_FAILURE);
      }

      /* Update port list */
      dapiBroadBaseCardSlotMap[port_idx].slotNum = 0;
      dapiBroadBaseCardSlotMap[port_idx].portNum = port_idx;
      dapiBroadBaseCardSlotMap[port_idx].bcm_cpuunit = 0;
      dapiBroadBaseCardSlotMap[port_idx].bcm_port    = (portgroup * CXO160G_BACKPLANE_PORT_LANES) + CXO160G_BACKPLANE_BCMPORT_BASE + lane;

      bport_idx++;
    }
    /* Not backplane ports */
    else
    {
      /* Update port list */
      dapiBroadBaseCardSlotMap[port_idx].slotNum = 0;
      dapiBroadBaseCardSlotMap[port_idx].portNum = port_idx;
      dapiBroadBaseCardSlotMap[port_idx].bcm_cpuunit = 0;

      /* Frontal 10G ports */
      if (speed > 1)
      {
        /* Validate frontal port index */
        if (fport_idx >= CXO160G_FRONTAL_PORTS)
        {
          LOG_INFO(LOG_CTX_STARTUP, "Only %u frontal ports are allowed!", CXO160G_FRONTAL_PORTS);
          return L7_FAILURE;
        }

        /* Get offset to calculate respective bcm_port */
        if (fport_idx < CXO160G_FRONTAL_PORTS_LOCAL)
        {
          if (!is_matrix_protection())
            offset = CXO160G_FRONTAL_BCMPORT_BASE
                     + (fport_idx * CXO160G_FRONTAL_PORT_LANES);
          else
            offset = CXO160G_FRONTAL_BCMPORT_BASE + (CXO160G_FRONTAL_PORTS_LOCAL * CXO160G_FRONTAL_PORT_LANES)
                     + ((CXO160G_FRONTAL_PORTS_LOCAL - fport_idx - 1) * CXO160G_FRONTAL_PORT_LANES);
          //offset += (fport_idx * CXO160G_FRONTAL_PORT_LANES);
        }
        else
        {
          if (!is_matrix_protection())
            offset = CXO160G_FRONTAL_BCMPORT_BASE + (CXO160G_FRONTAL_PORTS_LOCAL * CXO160G_FRONTAL_PORT_LANES)
                     + ((2*CXO160G_FRONTAL_PORTS_LOCAL - fport_idx - 1) * CXO160G_FRONTAL_PORT_LANES);
          else
            offset = CXO160G_FRONTAL_BCMPORT_BASE
                     + ((fport_idx - CXO160G_FRONTAL_PORTS_LOCAL) * CXO160G_FRONTAL_PORT_LANES);
          //offset += ((fport_idx - CXO160G_FRONTAL_PORTS_LOCAL) * CXO160G_FRONTAL_PORT_LANES);
        }

        dapiBroadBaseCardSlotMap[port_idx].bcm_port = offset + lane;
        fport_idx++;
      }
      /* 1G port */
      else
      {
        /* Validate giga port index */
        if (gport_idx >= CXO160G_GIGA_PORTS)
        {
          LOG_INFO(LOG_CTX_STARTUP, "Only 4 giga ports are allowed!");
          return L7_FAILURE;
        }

        dapiBroadBaseCardSlotMap[port_idx].bcm_port = gport_idx + CXO160G_GIGA_BCMPORT_BASE;
        gport_idx++;
      }
    }

    /* Update phy mode */
    switch (speed)
    {
      case 1:
        hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[port_idx] = port_descriptor_1G;
        break;
      case 10:
        hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[port_idx] = (slot < 0) ? port_descriptor_10G_AN : port_descriptor_10G;
        break;
      case 20:
        hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[port_idx] = port_descriptor_10G;
        break;
      case 40:
      case 100:
        hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[port_idx] = port_descriptor_40G;
        break;
      default:
        hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[port_idx] = port_descriptor_10G;
        break;
    }
  }

  /* Printing out port list */
  for (i = 0; i < port_idx; i++)
  {
    LOG_INFO(LOG_CTX_STARTUP, "slotNum=%u portNum=%2u bcm_cpuunit=%d bcm_port=%2d",
             dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[i].slotNum,
             dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[i].portNum,
             dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[i].bcm_cpuunit,
             dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[i].bcm_port);
  }

  /* Effective number of ports */
  dapiBroadPhysicalCardEntry_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1.numOfSlotMapEntries = port_idx;
  dapiBroadPhysicalCardEntry_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1.numOfPortMapEntries = port_idx;

  /* Update maximum number of interfaces */
  for (i = 0; i < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; i++)
  {
    sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(i));

    if (sysapiHpcCardInfoPtr != L7_NULLPTR)
      sysapiHpcCardInfoPtr->numOfNiPorts = port_idx;
    else
      LOG_ERR(LOG_CTX_STARTUP,"Error updating number of ports for slotIndex %u!", i);
  }

  LOG_INFO(LOG_CTX_STARTUP,"WC map applied successfully with %u ports!",port_idx);


  #if 0
  /* Specifies the number of lanes used by each port in the flex port group.
   * portgroup_<port group>=<number of lanes>.
   * Applicable to BCM566xx and BCM565xx device family */
  if (sal_config_set(spn_PORTGROUP"_0", "1") != 0)    /* 40G ports */
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_1", "1") != 0)
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_2", "1") != 0)
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_3", "4") != 0)    /* 10G XSGMII ports */
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_4", "4") != 0)
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_5", "4") != 0)
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_6", "4") != 0)
    return(L7_FAILURE);
  if (sal_config_set(spn_PORTGROUP"_7", "1") != 0)
    return(L7_FAILURE);
  #endif

  return L7_SUCCESS;
}
#endif

