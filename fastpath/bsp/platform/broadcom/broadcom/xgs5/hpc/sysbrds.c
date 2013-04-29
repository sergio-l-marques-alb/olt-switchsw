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
      /* PTin added: new platform */
      case UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1_ID:
        /* Enable trunk_128 bit. This will enable 128 trunks */
        /* and fixes LAG issue on XGS3 stacking              */
        if (sal_config_set(spn_TRUNK_EXTEND, "0x1") != 0)
          return(L7_FAILURE);

        /* Configure to use LCPLL reference clock */
        if (sal_config_set(spn_XGXS_LCPLL_XTAL_REFCLK, "1") != 0)
          return(L7_FAILURE);

        printf("%s: %s(%d) Hello!\r\n",__FILE__,__FUNCTION__,__LINE__);

#ifdef L7_STACKING_PACKAGE
        /* On Stacking packages, restrict FDB size to 16K MAX for FB2. */
        if (sal_config_set("l2_table_size", "0x3fff") != 0)
          return(L7_FAILURE);
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

