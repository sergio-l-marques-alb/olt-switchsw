/* $Id: multicast.h,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/

#ifndef _BCM_INT_DPP_MULTICAST_H
#define _BCM_INT_DPP_MULTICAST_H

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>

#define _BCM_PETRA_MULTICAST_TM_TYPE (0)



/* Types */
typedef enum
{
  /*
   *  Multicast ingress type
   */
  BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY=0,
  BCM_DPP_MULTICAST_TYPE_INGRESS = 0,
  /*
   *  Multicast egress type
   */
  BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY=1,
  BCM_DPP_MULTICAST_TYPE_EGRESS = 1,
  /* 
   *  Multicast ingress & egress
   */ 
  BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS=2,
  /* 
   *  Multicast nof types
   */  
  BCM_DPP_MULTICAST_NOF_TYPES=3
}BCM_DPP_MULTICAST_TYPE;


/*
 * Function:
 *      _bcm_petra_multicast_group_to_id
 * Purpose:
 *      Convert a BCM-Multicast ID to Soc_petra-Multicast ID
 * Parameters:
 *      group        (IN) BCM Multicast
 *      multicast_id (OUT) Soc_petra Multicast ID
 */
int
 _bcm_petra_multicast_group_to_id(bcm_multicast_t group, SOC_TMC_MULT_ID *multicast_id);


int
 _bcm_petra_multicast_group_from_id(bcm_multicast_t *group, int type, SOC_TMC_MULT_ID multicast_id);



/*
 * Function:
 *      _bcm_petra_multicast_ingress_group_exists, _bcm_petra_multicast_egress_group_exists
 * Purpose:
 *      Check if given multicast ingress/egress group id is open, returning no errors if possible.
 * Parameters: 
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
_bcm_petra_multicast_ingress_group_exists(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open);

/*
 * Function:
 *      _bcm_petra_multicast_is_group_exist
 * Purpose:
 *      Check if given multicast group id is existed or not according to expected_open.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      expected_open    (IN) Expected multicast group status.
 */
int
_bcm_petra_multicast_is_group_exist(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx,uint8 expected_open);

/*
 * Function:
 *      _bcm_petra_multicast_group_reserved_range_get
 * Purpose:
 *      Get the range of multicast IDs reserved during initialization
 * Parameters:
 *      unit (IN)
 *      mc_id_lo(OUT)
 *      mc_id_hi(OUT) 
 */
int
bcm_petra_multicast_group_reserved_range_get(int unit, uint32 *mc_id_lo, uint32 *mc_id_hi);

#endif    /* !_BCM_INT_DPP_VLAN_H */
