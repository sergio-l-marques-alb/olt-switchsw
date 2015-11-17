/*
 * $Id: multicast.c,v 1.68 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 *
 * Soc_petra-B Multicast Implementation
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MCAST
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <soc/drv.h>

#include "bcm_int/common/debug.h"
#include <bcm_int/control.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/multicast.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/common/trunk.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/switch.h>

#include <soc/dpp/mbcm.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>


/* defines */
#define _BCM_PETRA_MULTICAST_TYPE_UNSUPPORTED_FLAGS \
( BCM_MULTICAST_TYPE_WLAN | BCM_MULTICAST_TYPE_VLAN )

 
#define SOC_SAND_TRUE  1
#define SOC_SAND_FALSE 0

#define _BCM_TRILL_ENCAP_PORT_SHIFT         16

#define BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE    (4096)


/*
 * Function:
 *      _bcm_petra_multicast_is_supported_mc_group
 * Purpose:
 *      Derive mc group and check is it a supported mc type
 * Parameters:
 *      group        (IN) BCM Multicast
 */
int
_bcm_petra_multicast_is_supported_mc_group(bcm_multicast_t group)
{
    int type;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (type != _BCM_MULTICAST_TYPE_L2 && type != _BCM_MULTICAST_TYPE_L3 
        && type != _BCM_MULTICAST_TYPE_VPLS && type != _BCM_MULTICAST_TYPE_TRILL
        && type != _BCM_MULTICAST_TYPE_MIM && type != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_NO_UNIT("MC group not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_eg_multicast_is_supported_mc_group
 * Purpose:
 *      Derive mc group and check is it a supported mc type
 * Parameters:
 *      group        (IN) BCM Multicast
 */
int
_bcm_arad_eg_multicast_is_supported_mc_group(int unit, bcm_multicast_t group, int *is_bitmap)
{
    int type;

    BCMDNX_INIT_FUNC_DEFS;
    *is_bitmap = 0;
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (type == _BCM_MULTICAST_TYPE_PORTS_GROUP
      ) {
        *is_bitmap = 1;
    } else if (type != _BCM_MULTICAST_TYPE_L2 && type != _BCM_MULTICAST_TYPE_L3 
        && type != _BCM_MULTICAST_TYPE_VPLS && type != _BCM_MULTICAST_TYPE_TRILL
        && type != _BCM_MULTICAST_TYPE_MIM && type != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_NO_UNIT("MC group not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

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
 _bcm_petra_multicast_group_to_id(bcm_multicast_t group, SOC_TMC_MULT_ID *multicast_id)
{
     BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
     *multicast_id = _BCM_MULTICAST_ID_GET(group);

     BCM_EXIT;
exit:
     BCMDNX_FUNC_RETURN;
}

/*
 * Function_bcm_petra_multicast_group_from_id
 *      _bcm_petra_multicast_group_to_id
 * Purpose:
 *      Convert a BCM-Multicast ID from Soc_petra-Multicast ID
 * Parameters:
 *      group        (IN) BCM Multicast
 *      multicast_id (OUT) Soc_petra Multicast ID
 */
int
 _bcm_petra_multicast_group_from_id(bcm_multicast_t *group, int type, SOC_TMC_MULT_ID multicast_id)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    _BCM_MULTICAST_GROUP_SET(*group, type, multicast_id);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ppd_port_bitmap_from_mult_vlan_pbmp
 * Purpose:
 *      Convert a VLAN PBMP to PPD port bitmap
 * Parameters:
 *      ports                 (IN) 
 *      vlan_membership_group (OUT) 
 */
int
 _bcm_petra_multicast_ppd_port_bitmap_from_mult_vlan_pbmp(int unit, uint32 *ports,SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *vlan_membership_group)
{
    int ndx;
    int nof_local_ports;
    int nof_longs_in_bitmap; 

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ports);
    BCMDNX_NULL_CHECK(vlan_membership_group);
    
    nof_local_ports =  SOC_DPP_DEFS_GET(unit, nof_logical_ports);
    nof_longs_in_bitmap = SOC_SAND_DIV_ROUND_UP(nof_local_ports,SOC_SAND_REG_SIZE_BITS); 

    for (ndx = 0; ndx < nof_longs_in_bitmap; ndx++) {
        ports[ndx] = vlan_membership_group->bitmap[ndx];
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}   

int
_bcm_petra_multicast_ppd_port_bitmap_to_mult_vlan_pbmp(int unit, uint32 *ports,SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *vlan_membership_group)
{
    int ndx;
    int nof_local_ports, nof_longs_in_bitmap;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ports);
    BCMDNX_NULL_CHECK(vlan_membership_group);
    
    /*nof_longs_in_bitmap differs between Soc_petra-B and ARAD*/
    nof_local_ports =  SOC_DPP_DEFS_GET(unit, nof_logical_ports);
    nof_longs_in_bitmap = SOC_SAND_DIV_ROUND_UP(nof_local_ports,SOC_SAND_REG_SIZE_BITS); 

    for (ndx = 0; ndx < nof_longs_in_bitmap; ndx++) { 
        vlan_membership_group->bitmap[ndx] = ports[ndx];
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_range_id_check
 * Purpose:
 *      Check if given multicast id is in mc egress vlan membership range.
 * Parameters:
 *      
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
 _bcm_petra_multicast_egress_vlan_membership_range_id_check(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_in_range)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_PETRAB(unit)) {
        SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE range_info;
        SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(&range_info);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_range_get,(unit, &range_info)));

        if (multicast_id_ndx < range_info.mc_id_low || multicast_id_ndx > range_info.mc_id_high) {
            *is_in_range = FALSE;
        } else {
            *is_in_range = TRUE;
        }
    } else { /* non Petra */
        *is_in_range = multicast_id_ndx <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high
          ? TRUE : FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_multicast_is_group_created
 * Purpose:
 *      Check if given multicast group is open
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN)  Multicast ID.
 *      is_egress        (IN)  non zero if this is an egress group
 *      is_open          (OUT) is multicast group opened
 */
int
 _bcm_arad_multicast_is_group_created(int unit, SOC_TMC_MULT_ID mcid, int is_egress, uint8 *is_open)
{
    uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
    BCMDNX_INIT_FUNC_DEFS;

    if (mcid >= nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range (0 - 0x%x)"), mcid, nof_mc_ids - 1));
    }
    /* Check if this multicast group exists (possibly empty) */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_does_group_exist, (unit, mcid, is_egress, is_open)));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_multicast_egress_is_group_exist
 * Purpose:
 *      Check if given multicast egress group id is existed.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
 _bcm_petra_multicast_egress_is_group_exist(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open)
{
    /* Check if this multicast group exists (possibly empty) */
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_PETRAB(unit)) { /* use the allocation manager for Petra */
        int res = bcm_dpp_am_mc_is_alloced(unit, multicast_id_ndx, TRUE);
        if (res == BCM_E_NOT_FOUND) {
            *is_open = FALSE;
        } else if ( res == BCM_E_EXISTS) {
            *is_open = TRUE;
        } else {
            BCMDNX_IF_ERR_EXIT(res == BCM_E_NONE ? BCM_E_INTERNAL : res);
        }
    } else { /* Non Petra uses the swdb */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, is_open));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_is_group_exist
 * Purpose:
 *      Check if given multicast ingress group id is existed.
 * Parameters: 
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
_bcm_petra_multicast_ingress_is_group_exist(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open)
{
    /* Check if this multicast group exists (possibly empty) */
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_PETRAB(unit)) { /* use the allocation manager for Petra */
        int res = bcm_dpp_am_mc_is_alloced(unit, multicast_id_ndx, FALSE);
        if (res == BCM_E_NOT_FOUND) {
            *is_open = FALSE;
        } else if ( res == BCM_E_EXISTS) {
            *is_open = TRUE;
        } else {
            BCMDNX_IF_ERR_EXIT(res == BCM_E_NONE ? BCM_E_INTERNAL : res);
        }
    } else { /* Non Petra uses the swdb */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, is_open));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_group_exists
 * Purpose:
 *      Check if given multicast ingress group id is open, returning no errors if possible.
 * Parameters: 
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is ingress multicast group opened
 */
int
_bcm_petra_multicast_ingress_group_exists(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (multicast_id_ndx >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range (0 - 0x%x)"),multicast_id_ndx, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1));
    } else {
        /* Check if this multicast group exists (possibly empty) according to software data structure */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_is_group_exist(unit, multicast_id_ndx, is_open));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

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
_bcm_petra_multicast_is_group_exist(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx,uint8 expected_open)
{
    int rv;
    uint8 is_open_ingress, is_open_egress;
    SOC_TMC_MULT_ID nof_mc_ids = (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || (type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS &&
      SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids)) ?
      SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;

    BCMDNX_INIT_FUNC_DEFS;
    is_open_egress = is_open_ingress = expected_open = expected_open ? SOC_SAND_TRUE : SOC_SAND_FALSE;
    /* Check that multicast_id_ndx is not out of range */
    if (multicast_id_ndx >= nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range (0 - 0x%x)"), multicast_id_ndx, nof_mc_ids - 1));
    }

    if (SOC_IS_PETRAB(unit)) {
        /* Check Ingress multicast */
        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            rv = _bcm_petra_multicast_ingress_is_group_exist(unit,multicast_id_ndx,&is_open_ingress);
            BCMDNX_IF_ERR_EXIT(rv);
        }
  
        /* Check Egress multicast */
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            rv = _bcm_petra_multicast_egress_is_group_exist(unit,multicast_id_ndx,&is_open_egress);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else { /* non Petra */
        /* Check Ingress multicast */
        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, &is_open_ingress));
        }
        /* Check Egress multicast */
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &is_open_egress));
        }
    }

     if (is_open_ingress != expected_open || is_open_egress != expected_open) {
        if (expected_open == SOC_SAND_TRUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MC group %u not open"), multicast_id_ndx));
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("MC group %u open"), multicast_id_ndx));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_close
 * Purpose:
 *      Close multicast ingress group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_ingress_close(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_close,(unit,multicast_id_ndx)));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_close
 * Purpose:
 *      Close multicast egress group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_egress_close(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    bcm_error_t rv;
    uint8 is_in_range;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_petra_multicast_egress_vlan_membership_range_id_check(unit,multicast_id_ndx,&is_in_range);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_in_range == TRUE) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_close,(unit,multicast_id_ndx)));
    } else {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_group_close,(unit,multicast_id_ndx)));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_group_close
 * Purpose:
 *      Close multicast group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_group_close(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
        if (SOC_IS_PETRAB(unit)) {
            BCMDNX_IF_ERR_EXIT(bcm_dpp_am_mc_dealloc(unit, multicast_id_ndx, FALSE));
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_multicast_update_ingress_mc_state(unit, multicast_id_ndx, FALSE));
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_close(unit, multicast_id_ndx));
    }
    if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
        if (SOC_IS_PETRAB(unit)) {
            BCMDNX_IF_ERR_EXIT(bcm_dpp_am_mc_dealloc(unit, multicast_id_ndx, TRUE));
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_multicast_update_egress_mc_state(unit, multicast_id_ndx, FALSE));
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_close(unit, multicast_id_ndx));
    } 
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_open
 * Purpose:
 *      Open multicast ingress group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_ingress_open(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    SOC_TMC_MULT_ING_ENTRY mc_group;
    uint32 mc_group_size = 0;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_ING_ENTRY_clear(&mc_group);

     /* Open empty multicast ingress group */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_open,(unit,multicast_id_ndx,&mc_group,mc_group_size,&mc_err)));

    if (mc_err) {
        if (mc_err == (SOC_TMC_ERROR) BCM_E_FULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("not enough multicast entries to create the ingress group")));
        }
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("unexpected error")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_open
 * Purpose:
 *      Open multicast egress group. - Petra only
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_egress_open(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    bcm_error_t rv;
    uint32 soc_sand_rv;
    uint8 insufficient_memory = FALSE, is_in_range;
    SOC_TMC_MULT_EG_ENTRY mc_group;
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP vlan_membership_group;
    uint32 mc_group_size;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_EG_ENTRY_clear(&mc_group);
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&vlan_membership_group);

    rv = _bcm_petra_multicast_egress_vlan_membership_range_id_check(unit,multicast_id_ndx,&is_in_range);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_in_range == TRUE) {
        /* Open empty multicast egress group vlan membership */
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_open,(unit, multicast_id_ndx, &vlan_membership_group)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        
        /* Open empty multicast egress group linked list */
        mc_group_size = 0;

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_group_open,(unit,multicast_id_ndx,&mc_group,mc_group_size,&insufficient_memory)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (insufficient_memory == TRUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Insufficient memory")));
        }

    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_group_open
 * Purpose:
 *      Open multicast group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_group_open(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_PETRAB(unit)) {
        /* Open Ingress group */
        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            int rv = bcm_dpp_am_mc_alloc(unit, SHR_RES_ALLOC_WITH_ID, &multicast_id_ndx, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_multicast_update_ingress_mc_state(unit, multicast_id_ndx, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
            rv = _bcm_petra_multicast_ingress_open(unit, multicast_id_ndx);
            if (rv < 0) {
                bcm_dpp_am_mc_dealloc(unit, multicast_id_ndx, FALSE);
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
                _bcm_dpp_wb_multicast_update_ingress_mc_state(unit, multicast_id_ndx, FALSE);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
            }
            BCMDNX_IF_ERR_EXIT(rv);
        } 
        /* Open Egress group */
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            int rv = bcm_dpp_am_mc_alloc(unit, SHR_RES_ALLOC_WITH_ID, &multicast_id_ndx, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
            rv = _bcm_dpp_wb_multicast_update_egress_mc_state(unit, multicast_id_ndx, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
            rv = _bcm_petra_multicast_egress_open(unit, multicast_id_ndx);
            if (rv < 0) {
                bcm_dpp_am_mc_dealloc(unit, multicast_id_ndx, TRUE);
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
                _bcm_dpp_wb_multicast_update_egress_mc_state(unit, multicast_id_ndx, FALSE);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
            }
            BCMDNX_IF_ERR_EXIT(rv);
        } 
    } else { /* none Petra */
        uint8 is_open;
        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, &is_open));
            if (is_open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Ingress multicast group already created")));
            }
        }
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &is_open));
            if (is_open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Egress multicast group already created")));
            }
        }
        /* Open Ingress group */
        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_open(unit,multicast_id_ndx));
        }
        /* Open Egress group */
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {

            if (multicast_id_ndx <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high
              ) {

                /* if this is a bitmap, open empty multicast egress group vlan membership */
                SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP vlan_membership_group;
                SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&vlan_membership_group);
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_open,(unit, multicast_id_ndx, &vlan_membership_group)));

            } else { /* open a non bitmap group */

                SOC_TMC_ERROR mc_err = BCM_E_NONE;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_group_set,
                    (unit, multicast_id_ndx, 1, 0, 0, 0, &mc_err)));
                if (mc_err) {
                    if (mc_err == (SOC_TMC_ERROR) BCM_E_FULL) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("not enough multicast entries to create the egress group")));
                    }
                    BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("unexpected error")));
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_encap_id_from_cud
 * Purpose:
 *      Convert CUD to encap_id.
 * Parameters:
 *      encap_id (OUT) bcm_if_t
 *      cud     (IN) 
 */
int
_bcm_petra_multicast_encap_id_from_cud(bcm_if_t *encap_id,uint32 cud)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    *encap_id = (bcm_if_t)cud;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_encode_replication
 * Purpose:
 *      Set the egress multicast replication encoding of the cud and destination port,
 *      from the local port and the encapsulation.
 * Parameters:
 *      unit       (IN) Deivce id
 *      out_port   (OUT) replication destination local port
 *      out_cud    (OUT) replication cud
 *      local_port (IN) local port
 *      encap_id   (IN)
 */
int
_bcm_petra_multicast_egress_encode_replication(int unit, uint32 *out_port, uint32 *out_cud, bcm_port_t local_port,bcm_if_t encap_id)
{
    int is_17b_cud_mode = SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE;
    bcm_port_t max_local_port = is_17b_cud_mode ? 126 : 254;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(out_port);
    BCMDNX_NULL_CHECK(out_cud);

    if (encap_id == BCM_IF_INVALID) {
        encap_id = SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
    } else if (encap_id < 0 || encap_id > SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress encapsulation ID: 0x%x, should be between 0 and 0x%x."), encap_id, SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud));
    }
    if (local_port == BCM_GPORT_INVALID) {
        if (is_17b_cud_mode) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("CUD only replications are not supported in egress multicast with 17 bit CUDs.")));
        }
    } else if (local_port & ARAD_MC_EGR_IS_BITMAP_BIT) { /* This is a bitmap replication */
        bcm_port_t bitmap = local_port & ~ARAD_MC_EGR_IS_BITMAP_BIT;
        if (bitmap < 0 || bitmap > ARAD_MC_MAX_BITMAPS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid bitmap: %d, should be between 0 and %d."), bitmap, ARAD_MC_MAX_BITMAPS));
        } else if (is_17b_cud_mode) {
            local_port |= (encap_id & (1 << 16)) >> 2;
            encap_id &= ~(1 << 16);
        }
    } else if (local_port < 0 || local_port > max_local_port) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid local port: %d, should be between 0 and %d."), local_port, max_local_port));
    } else if (is_17b_cud_mode) {
        local_port |= (encap_id & (1 << 16)) >> 9;
        encap_id &= ~(1 << 16);
    }
    *out_cud = encap_id;
    *out_port = local_port;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_mult_eg_entry_to_local_port
 * Purpose:
 *      Convert multicast egress entry from local port
 * Parameters:
 *      entry      (IN) multicast entry
 *      local_port (OUT) local port
 *      encap_id   (OUT)
 */
int
_bcm_petra_multicast_egress_mult_eg_entry_to_local_port(SOC_TMC_MULT_EG_ENTRY entry, bcm_port_t *local_port,bcm_if_t *encap_id)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    BCMDNX_NULL_CHECK(local_port);
    BCMDNX_NULL_CHECK(encap_id);

    if (entry.type == SOC_TMC_MULT_EG_ENTRY_TYPE_OFP)
    {
        rv = _bcm_petra_multicast_encap_id_from_cud(encap_id,entry.cud);
        BCMDNX_IF_ERR_EXIT(rv);
    
        *local_port = entry.port;
    } else {
        /* Type not supported yet */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("Type is not supported yet")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_port_add
 * Purpose:
 *      Add port in multicast egress vlan membership  
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_port       (IN) local port
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_egress_vlan_membership_port_add(int unit,SOC_TMC_MULT_ID multicast_id_ndx,bcm_port_t local_port,bcm_if_t encap_id)
{
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_port_add,(unit, multicast_id_ndx, local_port, &mc_err)));
    if (mc_err) {
        if (mc_err == (SOC_TMC_ERROR) BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The port is already replicated to in the bitmap multicast group")));
        }
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("unexpected error")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_entry_port_add
 * Purpose:
 *      Add port in multicast egress linked list
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_port       (IN) local port
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_egress_entry_port_add(int unit,SOC_TMC_MULT_ID multicast_id_ndx,bcm_port_t local_port,bcm_if_t encap_id)
{
    SOC_TMC_MULT_EG_ENTRY entry;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_EG_ENTRY_clear(&entry);

    entry.type = SOC_TMC_MULT_EG_ENTRY_TYPE_OFP;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_encode_replication(unit, &entry.port, &entry.cud, local_port, encap_id));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_port_add,(unit,multicast_id_ndx,&entry,&mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ?  "not enough multicast entries to add to the egress MC group" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_EXISTS ? "The added group member already exists in the egress MC group " : "unexpected error")) ));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_port_remove
 * Purpose:
 *      Remove port in multicast egress vlan membership
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_port       (IN) local port to remove
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_egress_vlan_membership_port_remove(int unit,SOC_TMC_MULT_ID multicast_id_ndx,bcm_port_t local_port,bcm_if_t encap_id)
{
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_port_remove,(unit, multicast_id_ndx, local_port, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_NOT_FOUND ?
          "The port is already replicated to in the bitmap multicast group" : "unexpected error" )));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_entry_port_remove
 * Purpose:
 *      Remove port in multicast egress linked list
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_port       (IN) local port to remove
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_egress_entry_port_remove(int unit,SOC_TMC_MULT_ID multicast_id_ndx,bcm_port_t local_port,bcm_if_t encap_id)
{
    SOC_TMC_MULT_EG_ENTRY entry;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_EG_ENTRY_clear(&entry);

    entry.type = SOC_TMC_MULT_EG_ENTRY_TYPE_OFP;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_encode_replication(unit, &entry.port, &entry.cud, local_port, encap_id));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_port_remove,(unit, multicast_id_ndx, &entry, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ?  "not enough multicast entries for the operation" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_NOT_FOUND ? "The removed group member does not exist in the egress MC group " : "unexpected error")) ));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info
 * Purpose:
 *      Convert multicast egress entry from tm dest info and encap id
 * Parameters:
 *      unit       (IN) Deivce id
 *      entry      (OUT) multicast entry
 *      local_port (IN) local port
 *      encap_id   (IN)
 */
int
_bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(int unit, SOC_TMC_MULT_ING_ENTRY *entry, SOC_TMC_DEST_INFO dest_info,bcm_if_t encap_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(entry);

    entry->destination = dest_info;

    /* Convert encap id to CUD. */
    if (encap_id == BCM_IF_INVALID) {
        entry->cud = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
    } else if (encap_id < 0 || encap_id > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress encapsulation ID: 0x%x, should be between 0 and 0x%x"), encap_id, SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud));
    } else {
        entry->cud = encap_id;
    }

    if (!SOC_IS_PETRAB(unit)) { /* In Arad check for legal destination and encode the hardware fields here. Needed for Arad+ */
        int is_sys_port = 0, rv;
        if (entry->destination.type == SOC_TMC_DEST_TYPE_QUEUE) { /* verify direct Queue_id range */
            if (entry->destination.id > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Queue %u, is over %u."),
                  (unsigned)entry->destination.id, (unsigned)SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue));
            }
        } else if (entry->destination.type == SOC_TMC_DEST_TYPE_MULTICAST) { /* verify multicast_id range */
#ifndef SUPPORT_ARAD_MC_DEST_ININGRESS_MC_BUT_DOES_NOT_WORK_IN_HW
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("A ingress multicast destination of type multicast is not allowed on this device")));
#else
            if (SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode != ARAD_MC_DEST_ENCODING_0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MC destinations are not allowed in non allowed in this ingress multicast encoding mode.")));
            } else if (entry->destination.id >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is above 0x%x)"),
                  (unsigned)entry->destination.id, (unsigned)(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1)));
            }
#endif
        } else {
            is_sys_port = 1;
        }
        rv = handle_sand_result(MBCM_DPP_DRIVER_CALL(unit, /* encode hardware fields */
            mbcm_dpp_mult_ing_encode_entry,
            (unit, entry, &entry->destination.id, &entry->cud)));
        if (rv != BCM_E_NONE) {
            if (is_sys_port) { /* verify destination system port range */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Destination system port is probably greater than or equal to %u, or LAG destination used in an unsupported mode."),
                  (unsigned)SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports));
            } else {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Error encoding ingress multicast destination/cud.")));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_mult_ing_entry_to_tm_dest_info
 * Purpose:
 *      Convert multicast egress entry to tm dest info and encap id
 * Parameters:
 *      entry      (OUT) multicast entry
 *      local_port (IN) local port
 *      encap_id   (IN)
 */
int
_bcm_petra_multicast_ingress_mult_ing_entry_to_tm_dest_info(SOC_TMC_MULT_ING_ENTRY entry,SOC_TMC_DEST_INFO *dest_info,bcm_if_t *encap_id)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    BCMDNX_NULL_CHECK(dest_info);
    BCMDNX_NULL_CHECK(encap_id);

    *dest_info = entry.destination;

    rv = _bcm_petra_multicast_encap_id_from_cud(encap_id,entry.cud);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entry_add
 * Purpose:
 *      Add new entry to multicast ingress
 * Parameters:
 *      unit             (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      dest_info        (IN) dest_info to add
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_ingress_entry_add(int unit,SOC_TMC_MULT_ID multicast_id_ndx,SOC_TMC_DEST_INFO dest_info,bcm_if_t encap_id)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ING_ENTRY entry;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_ING_ENTRY_clear(&entry);

    rv =  _bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(unit, &entry, dest_info, encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_destination_add,(unit, multicast_id_ndx, &entry, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ?  "not enough multicast entries to add the replication" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_EXISTS ? "The added destination+encap already exists in the egress MC group " : "unexpected error")) ));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}       

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entry_remove
 * Purpose:
 *      Remove entry from multicast ingress
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      dest_info        (IN) dest_info to add
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_ingress_entry_remove(int unit,SOC_TMC_MULT_ID multicast_id_ndx,SOC_TMC_DEST_INFO dest_info,bcm_if_t encap_id)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ING_ENTRY entry;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_ING_ENTRY_clear(&entry);

    rv =  _bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(unit, &entry, dest_info, encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_destination_remove,(unit, multicast_id_ndx, &entry, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_NOT_FOUND ?
          "The removed replication does not exist in the ingress MC group " : "unexpected error") ));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}       

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_set
 * Purpose:
 *      Set entries to multicast egress vlan membership
 * Parameters:
 *      unit             (IN)
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_ports      (IN) local_ports bitmap
 */
int
_bcm_petra_multicast_egress_vlan_membership_set(int unit,SOC_TMC_MULT_ID multicast_id_ndx,bcm_pbmp_t local_ports)
{
    bcm_error_t rv;
    uint32 nof_local_ports,nof_longs_in_bitmap;
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP vlan_membership_group;
    uint32 ports[SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_MAX];

    
    BCMDNX_INIT_FUNC_DEFS;
    /* nof_longs_in_bitmap differs between Soc_petra-B and ARAD*/
    nof_local_ports =  SOC_DPP_DEFS_GET(unit, nof_logical_ports);
    nof_longs_in_bitmap = SOC_SAND_DIV_ROUND_UP(nof_local_ports,SOC_SAND_REG_SIZE_BITS); 
    
    
    /* Convert pbmp to multicast vlan bitmap */
    rv = pbmp_to_ppd_port_bitmap(unit, &local_ports, ports, nof_longs_in_bitmap);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_multicast_ppd_port_bitmap_to_mult_vlan_pbmp(unit,ports,&vlan_membership_group);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update group */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_update,(unit,multicast_id_ndx,&vlan_membership_group)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_entries_set
 * Purpose:
 *      Set entries to multicast egress linked list - Petra only
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_ports_arr  (IN) array of local_ports
 *      encap_id_arr     (IN) array of encap id
 */
int
_bcm_petra_multicast_egress_entries_set(int unit,SOC_TMC_MULT_ID multicast_id_ndx, int port_count, bcm_port_t *local_ports_arr,bcm_if_t *encap_id_arr)
{
    SOC_TMC_MULT_EG_ENTRY *entries = NULL;
    int ndx;
    uint8 insufficient_memory = FALSE;
    uint32 soc_sand_rv;
    uint32 mc_group_size;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ALLOC(entries, sizeof(SOC_TMC_MULT_EG_ENTRY) * port_count, "multicast eg entries");
    if (entries == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    
    /* Clear entries */
    for (ndx = 0; ndx < port_count; ndx++)
    {
        SOC_TMC_MULT_EG_ENTRY_clear(&(entries[ndx]));
    }

    for (ndx = 0; ndx < port_count; ndx++)
    {
        /* Convert local port to mult egress entry */
        entries[ndx].type = SOC_TMC_MULT_EG_ENTRY_TYPE_OFP;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_encode_replication(unit, &entries[ndx].port, &entries[ndx].cud, local_ports_arr[ndx], encap_id_arr[ndx]));
    }
    
    /* Set new linked list */
    mc_group_size = port_count;
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_group_update,(unit, multicast_id_ndx, entries, mc_group_size, &insufficient_memory)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (insufficient_memory == TRUE) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("bcm_petra_multicast_egress_entries_set failed: insufficient memory")));
    }  

exit:
    if (entries) {
        BCM_FREE(entries);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_get
 * Purpose:
 *      Get entries from multicast egress vlan membership
 * Parameters:
 *      unit             (IN)
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_ports      (OUT) local ports bitmap
 */
int
_bcm_petra_multicast_egress_vlan_membership_get(int unit, SOC_TMC_MULT_ID multicast_id_ndx, bcm_pbmp_t *local_ports)
{
    uint32 nof_local_ports, nof_longs_in_bitmap;
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP vlan_membership_group;
    uint32 ports[SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_MAX];     

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&vlan_membership_group);
    BCM_PBMP_CLEAR(*local_ports);

    /* Get the pbmp port bitmap */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_get,(unit, multicast_id_ndx, &vlan_membership_group)));

    /* Convert multicast vlan bitmap to ppd port bitmap */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ppd_port_bitmap_from_mult_vlan_pbmp(unit,ports,&vlan_membership_group));
    
    /* Calculating number of longs in bitmap. Differs between soc_petra-B and ARAD*/
    nof_local_ports =  SOC_DPP_DEFS_GET(unit, nof_logical_ports);
    nof_longs_in_bitmap = SOC_SAND_DIV_ROUND_UP(nof_local_ports,SOC_SAND_REG_SIZE_BITS); 

    BCMDNX_IF_ERR_EXIT(pbmp_from_ppd_port_bitmap(unit, local_ports, ports, nof_longs_in_bitmap));
  
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_entries_get
 * Purpose:
 *      Get entries from multicast egress linked list - Petra only
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      port_count       (IN) nof ports to get
 *      local_ports_arr  (OUT) local ports array
 *      encap_id_arr     (OUT) encap id array
 *      exact_mc_group   (OUT) exact multicast group
 */
int
_bcm_petra_multicast_egress_entries_get(int unit,SOC_TMC_MULT_ID multicast_id_ndx, int port_count, bcm_port_t *local_ports_arr,bcm_if_t *encap_id_arr,uint32 *exact_mc_group_size)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_TMC_MULT_EG_ENTRY *entries = NULL;
    int ndx;
    uint8 is_open;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ALLOC(entries, sizeof(SOC_TMC_MULT_EG_ENTRY) * port_count, "multicast eg entries");
    if (entries == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    
    /* Clear entries */
    for (ndx = 0; ndx < port_count; ndx++)
    {
        SOC_TMC_MULT_EG_ENTRY_clear(&(entries[ndx]));
    }

    /* Get new linked list */
    if (port_count > 0) {
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_group_get,(unit, multicast_id_ndx, port_count, entries, exact_mc_group_size, &is_open)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        for (ndx = 0; ndx < *exact_mc_group_size; ndx++)
        {
            /* Convert local port from mult egress entry */
            rv =  _bcm_petra_multicast_egress_mult_eg_entry_to_local_port(entries[ndx], &local_ports_arr[ndx], &encap_id_arr[ndx]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        /* Only check multicast group size */
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_group_size_get,(unit, multicast_id_ndx, exact_mc_group_size)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    if (entries) {
        BCM_FREE(entries);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entries_set
 * Purpose:
 *      Set entries to multicast ingress
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      dest_info_arr   (IN) array of dest info tm
 *      encap_id_arr     (IN) array of encap id
 */
int
_bcm_petra_multicast_ingress_entries_set(int unit,SOC_TMC_MULT_ID multicast_id_ndx, int port_count, SOC_TMC_DEST_INFO *dest_info_arr,bcm_if_t *encap_id_arr)
{
    SOC_TMC_MULT_ING_ENTRY *entries = NULL;
    int ndx;
    uint32 mc_group_size;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ALLOC(entries, sizeof(SOC_TMC_MULT_ING_ENTRY) * port_count, "multicast eg entries");
    if (entries == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    
    /* Clear entries */
    for (ndx = 0; ndx < port_count; ndx++)
    {
        SOC_TMC_MULT_ING_ENTRY_clear(&(entries[ndx]));
    }

    for (ndx = 0; ndx < port_count; ndx++)
    {
        /* Convert local port to mult egress entry */
       BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(unit, &entries[ndx], dest_info_arr[ndx], encap_id_arr[ndx]));
    }
    
    /* Set new linked list */
    mc_group_size = port_count;
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_update,(unit, multicast_id_ndx, entries, mc_group_size, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ?  "not enough multicast entries for the operation" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "The same replication (destination + encap) may not appear multiple times in the input" : "unexpected error")) ));
    }

exit:
    if (entries) {
        BCM_FREE(entries);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entries_get
 * Purpose:
 *      Get entries from multicast ingress - Petra only
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      port_count       (IN) nof ports to get
 *      dest_info_arr    (OUT) dest_info_arr
 *      encap_id_arr     (OUT) encap id array
 *      exact_mc_group   (OUT) exact multicast group
 */
int
_bcm_petra_multicast_ingress_entries_get(int unit,SOC_TMC_MULT_ID multicast_id_ndx, int port_count, SOC_TMC_DEST_INFO *dest_info_arr,bcm_if_t *encap_id_arr,uint32 *exact_mc_group_size)
{
    bcm_error_t rv = BCM_E_NONE;
    SOC_TMC_MULT_ING_ENTRY *entries = NULL;
    int ndx;
    uint8 is_open;
    uint32 soc_sand_rv;
    uint32 mc_group_size;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ALLOC(entries, sizeof(SOC_TMC_MULT_ING_ENTRY) * port_count, "multicast eg entries");
    if (entries == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    
    /* Clear entries */
    for (ndx = 0; ndx < port_count; ndx++)
    {
        SOC_TMC_MULT_ING_ENTRY_clear(&(entries[ndx]));
    }

    for (ndx = 0; ndx < port_count; ndx++)
    {
        /* Convert local port from mult ingress entry */
       rv =  _bcm_petra_multicast_ingress_mult_ing_entry_to_tm_dest_info(entries[ndx], &dest_info_arr[ndx],&encap_id_arr[ndx]);
       BCMDNX_IF_ERR_EXIT(rv);
    }
    
    /* Get new linked list */
    mc_group_size = port_count;
    *exact_mc_group_size = 0;/* not really needed - just to avoid coverity defect */
    if (mc_group_size != 0) {
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_get,(unit,multicast_id_ndx,mc_group_size,entries,exact_mc_group_size,&is_open)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        for (ndx = 0; ndx < *exact_mc_group_size; ndx++)
        {
            /* Convert local port from mult egress entry */
           rv =  _bcm_petra_multicast_ingress_mult_ing_entry_to_tm_dest_info(entries[ndx], &dest_info_arr[ndx],&encap_id_arr[ndx]);
           BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        /* Only check multicast group size */
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_size_get,(unit,multicast_id_ndx,exact_mc_group_size)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    if (entries) {
        BCM_FREE(entries);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_create
 * Purpose:
 *      Allocate a multicast group index
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_create(
    int unit, 
    uint32 flags, 
    bcm_multicast_t *group)
{
    bcm_error_t rv;    
    int type;
    SOC_TMC_MULT_ID multicast_id;
    BCM_DPP_MULTICAST_TYPE tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS;
    uint8 expected_open;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    rv = BCM_E_NONE;    

    /* verify input parameters */
    BCMDNX_NULL_CHECK(group);
    
    if (flags & _BCM_PETRA_MULTICAST_TYPE_UNSUPPORTED_FLAGS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
    }
    
    type = flags & BCM_MULTICAST_TYPE_MASK;

    if (!(flags & BCM_MULTICAST_WITH_ID))
    {
        /* Multicast without ID is not supported */
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("flag BCM_MULTICAST_WITH_ID must be used")));
    } else if ((flags & (BCM_MULTICAST_TYPE_TRILL | BCM_MULTICAST_INGRESS_GROUP)) == (BCM_MULTICAST_TYPE_TRILL | BCM_MULTICAST_INGRESS_GROUP)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Trill multicast supported only for egress")));
    }

    /* Type is ECMP/flooding port => create virtual MC_ID*/
    if (flags & BCM_MULTICAST_TYPE_SUBPORT) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_SUBPORT, *group);
        BCM_EXIT;
    }

    rv = _bcm_petra_multicast_group_to_id(*group, &multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (flags & BCM_MULTICAST_TYPE_PORTS_GROUP) { /* handle the case where we are not creating a group, but just returning a representation of a bitmap */
        if (SOC_IS_PETRAB(unit)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_MULTICAST_TYPE_PORTS_GROUP not supported in Petra")));
        }
        if ((flags & ~(BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID)) != BCM_MULTICAST_TYPE_PORTS_GROUP) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags containing BCM_MULTICAST_TYPE_PORTS_GROUP should contain BCM_MULTICAST_WITH_ID and nothing else")));
        }

        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_PORTS_GROUP, multicast_id);
        BCM_EXIT;
    }

    /* Multicast ID is global, Indicate Flag decide which groups to create (by default ingress and egress) */
    if (flags & BCM_MULTICAST_INGRESS_GROUP) {
        if (~flags & BCM_MULTICAST_EGRESS_GROUP) {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY;
        }
    } else if (flags & BCM_MULTICAST_EGRESS_GROUP) {
        tm_mc_type = BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY;
    }
    expected_open = SOC_SAND_FALSE;
    rv = _bcm_petra_multicast_is_group_exist(unit, tm_mc_type, multicast_id, expected_open);
    BCMDNX_IF_ERR_EXIT(rv);
    
    switch(type) {
    case BCM_MULTICAST_TYPE_L2:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, multicast_id);      
        break;

    case BCM_MULTICAST_TYPE_L3:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3, multicast_id);
        break;

    case BCM_MULTICAST_TYPE_VPLS:   
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VPLS, multicast_id);
        break; 

    case BCM_MULTICAST_TYPE_TRILL:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_TRILL, multicast_id);  
        break;

    case BCM_MULTICAST_TYPE_MIM:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_MIM, multicast_id);      
        break;

    case BCM_MULTICAST_TYPE_EGRESS_OBJECT:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_EGRESS_OBJECT, multicast_id);      
        break;
                  
    case 0:
        /* TM type only */
        break;
    default:
        /* Configuration type is invalid */     
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified multicast type flags are not supported")));
    }

    rv = _bcm_petra_multicast_group_open(unit,tm_mc_type, multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_destroy
 * Purpose:
 *      Free a multicast group index
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_destroy(
    int unit, 
    bcm_multicast_t group)
{
    int rv;
    SOC_TMC_MULT_ID multicast_id;
    int type;
    BCM_DPP_MULTICAST_TYPE tm_mc_type;
    uint8 ingress_open = FALSE;
    uint8 egress_open = FALSE;
       
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (type != _BCM_MULTICAST_TYPE_L2 && type !=  _BCM_MULTICAST_TYPE_VPLS
        && type != _BCM_MULTICAST_TYPE_L3 && type != 0) {    
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid multicast group type")));
    }

    rv = _bcm_petra_multicast_group_to_id(group, &multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);


    /* Multicast ID is global, both ingress and egress multicast groups need to be destroyed if they exist.
       Check what exists. */
        rv = _bcm_petra_multicast_ingress_is_group_exist(unit, multicast_id, &ingress_open);
        BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_multicast_egress_is_group_exist(unit, multicast_id, &egress_open);
    BCMDNX_IF_ERR_EXIT(rv);

    if (egress_open == FALSE) {
        if (ingress_open == FALSE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The specified multicast group was not created")));
        } else {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY;
        }
    } else {
        if (ingress_open == FALSE) {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY;
        } else {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS;
        }
    }
   
    rv = _bcm_petra_multicast_group_close(unit, tm_mc_type, multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_egress_add
 * Purpose:
 *      Add a GPORT to the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_egress_add(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_MULT_EG_ENTRY entry;
    int is_arad_mechanism = 0;
    uint8 is_in_range, open;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    SOC_TMC_MULT_EG_ENTRY_clear(&entry);
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx));

    if (SOC_IS_PETRAB(unit)) {
        /* Check mc group type is supported */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group));
        open = SOC_SAND_TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY, multicast_id_ndx, open));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
    } else { /* non Petra B */
        int is_bitmap_group;
        BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
        /* Check if multicast group exists (was created) */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &open));
        if (is_bitmap_group) {
            is_in_range = 1;
        } else {
            if (!open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
        }
        if (!is_in_range) {
            is_arad_mechanism = 1;
        }
    }

    if (is_arad_mechanism && port == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
        if (!encap_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outlif only replication can not use outlif 0")));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_add(unit, multicast_id_ndx, BCM_GPORT_INVALID, encap_id));
    } else if (is_arad_mechanism && BCM_GPORT_IS_MCAST(port)) { /* bitmap replication */
        if (!_SHR_GPORT_MCAST_GET(port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bitmap replication can not use bitmap 0")));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_add(unit, multicast_id_ndx, BCM_GPORT_MCAST_GET(port) | ARAD_MC_EGR_IS_BITMAP_BIT, encap_id));
    } else {
        /* Derive local ports */
        uint32 local_port;
        _bcm_dpp_gport_info_t gport_info;

         BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

         BCM_PBMP_ITER(gport_info.pbmp_local_ports,local_port) { /* add ports in bitmap */
             if (is_in_range) {
                 BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_port_add(unit, multicast_id_ndx, local_port, encap_id));
             } else {
                 BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_add(unit, multicast_id_ndx, local_port, encap_id));
             }
         }
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_egress_delete
 * Purpose:
 *      Delete GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_egress_delete(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_MULT_EG_ENTRY entry;
    int is_arad_mechanism = 0;
    uint8 is_in_range, open;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    SOC_TMC_MULT_EG_ENTRY_clear(&entry);
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx));

    if (SOC_IS_PETRAB(unit)) {
        /* Check mc group type is supported */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group));
        open = SOC_SAND_TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY, multicast_id_ndx, open));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
    } else { /* non Petra B */
        int is_bitmap_group;
        BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
        /* Check if multicast group exists (was created) */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &open));
        if (is_bitmap_group) {
            is_in_range = 1;
        } else {
            if (!open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
        }
        if (!is_in_range) {
            is_arad_mechanism = 1;
        }
    }

    if (is_arad_mechanism && port == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
        if (!encap_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outlif only replication can not use outlif 0")));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_remove(unit, multicast_id_ndx, BCM_GPORT_INVALID, encap_id));
    } else if (is_arad_mechanism && BCM_GPORT_IS_MCAST(port)) { /* bitmap replication */
        if (!_SHR_GPORT_MCAST_GET(port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bitmap replication can not use bitmap 0")));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_remove(unit, multicast_id_ndx, BCM_GPORT_MCAST_GET(port) | ARAD_MC_EGR_IS_BITMAP_BIT, encap_id));
    } else {
        /* Derive local ports */
        uint32 local_port;
        _bcm_dpp_gport_info_t gport_info;

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

        BCM_PBMP_ITER(gport_info.pbmp_local_ports,local_port) { /* remove ports in bitmap */
            if (is_in_range) {
               BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_port_remove(unit, multicast_id_ndx, local_port, encap_id));
            } else {
               BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_remove(unit, multicast_id_ndx, local_port ,encap_id));
            }
        }
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_egress_delete_all
 * Purpose:
 *      Delete all GPORTs from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_egress_delete_all(
    int unit, 
    bcm_multicast_t group)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    BCMDNX_IF_ERR_EXIT(bcm_petra_multicast_egress_set(unit,group,0,NULL,NULL));
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *      bcm_petra_multicast_egress_get
 * Purpose:
 *      Retrieve a set of egress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      mc_index   - (IN) Multicast index
 *      port_max   - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of multicast GPORTs 
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of multicasts returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */
int 
bcm_petra_multicast_egress_get(
    int unit, 
    bcm_multicast_t group, 
    int port_max, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array, 
    int *port_count)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    bcm_port_t *local_ports_arr = NULL;
    uint32 group_size = 0;
    uint8 is_in_range, is_open;

    BCMDNX_INIT_FUNC_DEFS;
    /* verify input parameters */
    BCMDNX_NULL_CHECK(port_count);
    if (port_max > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(encap_id_array);
    }   else if (port_max < 0) {
        port_max = 0;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx)); /* get mcid of group */
    if (SOC_IS_PETRAB(unit)) {
        uint8 expected_open = SOC_SAND_TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check if mc group type is supported */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(
          unit, BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY, multicast_id_ndx, expected_open)); /* Verify that the multicast ID (group) exists */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
    } else { /* not Petra */
        int is_bitmap_group;
        BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
        /* Check multicast group exists (was created) */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &is_open));
        if (is_bitmap_group) {
            is_in_range = 1;
        } else {
            if (!is_open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
        }
    }

    if (is_in_range) { /* this is a bitmap */
        bcm_pbmp_t l2_pbmp;
        bcm_port_t port;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_get(unit, multicast_id_ndx, &l2_pbmp)); /* do the actual work */

        /* Convert pbmp to gport */
        PBMP_ITER(l2_pbmp, port) {
            if (port_max == 0) {
                break;
            }
            if (port_array != NULL) {
                /* Convert local port to GPORT */
                BCM_GPORT_LOCAL_SET(port_array[group_size],port);
            }
            if (encap_id_array != NULL) {
                encap_id_array[group_size] = BCM_IF_INVALID;
            }
            ++group_size;
            --port_max;
        }
        *port_count = group_size;

        if (port_max == 0 && port_array == NULL && encap_id_array == NULL) {
            BCM_PBMP_COUNT(l2_pbmp,*port_count);
        }
        
    } else { /* group is not a bitmap */

        if (SOC_IS_PETRAB(unit)) {

            BCMDNX_ALLOC(local_ports_arr, sizeof(bcm_port_t) * port_max, "local ports from gports");
            if (local_ports_arr == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed in allocation of temporary memory")));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entries_get(unit, multicast_id_ndx, port_max, local_ports_arr, encap_id_array, &group_size)); /* do the actual work */
            *port_count = group_size;
            if (port_max) {
                uint32 port_iter;
                for (port_iter = 0; port_iter < group_size; port_iter++) { /* Convert local ports to GPORTs */
                    BCM_GPORT_LOCAL_SET(port_array[port_iter],local_ports_arr[port_iter]);
                }
            }

        } else { /* non Petra */

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_get_group,(unit, multicast_id_ndx, port_max, port_array, encap_id_array, &group_size, &is_open))); /* do the actual work */
            if (!is_open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not clear if egress multicast group %u was created"), multicast_id_ndx));
            }
        
            /* if port_max is non zero and not all group replications could be returned,
             * return the number of returned replications (port_max).
             * Otherwise return the exact group size in replications (a bitmap pointer counts as one replication).
             */
            *port_count = port_max && group_size > port_max ? port_max : group_size;
        }
    }

exit:
    if(local_ports_arr) {
       BCM_FREE(local_ports_arr);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_egress_set
 * Purpose:
 *      Assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count   - (IN) Number of ports in replication list
 *      port_array   - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_egress_set(
    int unit, 
    bcm_multicast_t group, 
    int port_count, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    uint32 local_port;
    int ndx, port_local_arr_ndx = 0;
    int is_arad_mechanism = 0;
    bcm_pbmp_t pbmp;
    uint8 is_in_range, open;
    bcm_port_t* local_port_arr = NULL;
    uint32 *arad_local_port_arr = NULL;
    bcm_if_t* encap_id_arr = NULL;
    _bcm_dpp_gport_info_t gport_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_PBMP_CLEAR(pbmp);

    /* verify input parameters */
    if (port_count > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(encap_id_array);
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group,&multicast_id_ndx));
    if (SOC_IS_PETRAB(unit)) {
        /* Check mc group type is supported */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group));
        open = SOC_SAND_TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY, multicast_id_ndx, open));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */

        /* Allocate local ports arr (include also from trunk ports) */
        BCMDNX_ALLOC(local_port_arr, sizeof(bcm_port_t) * (port_count * BCM_TRUNK_MAX_PORTCNT), "local ports from gports");
        BCMDNX_ALLOC(encap_id_arr, sizeof(bcm_if_t) * (port_count * BCM_TRUNK_MAX_PORTCNT), "encap ids");
        if (local_port_arr == NULL || encap_id_arr == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed in allocation of temporary memory")));
        }
        sal_memset(local_port_arr, 0, sizeof(bcm_port_t) * (port_count * BCM_TRUNK_MAX_PORTCNT));
        sal_memset(encap_id_arr, 0, sizeof(bcm_if_t) * (port_count * BCM_TRUNK_MAX_PORTCNT));
    } else { /* non Petra B */
        int is_bitmap_group;
        {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
            /* Check if multicast group exists (was created) */
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &open));
            if (is_bitmap_group) {
                is_in_range = 1;
            } else {
                if (!open) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
                if(!is_in_range) {
                    is_arad_mechanism = 1;
                }
            }
        }
        if (!is_in_range) {
            BCMDNX_ALLOC(arad_local_port_arr, sizeof(uint32) * (port_count * BCM_TRUNK_MAX_PORTCNT), "local ports from gports");
            BCMDNX_ALLOC(encap_id_arr, sizeof(bcm_if_t) * (port_count * BCM_TRUNK_MAX_PORTCNT), "encap ids");
            if (arad_local_port_arr == NULL || encap_id_arr == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed in allocation of temporary memory")));
            }
        }
    }

    /* Resolve GPORT to local ports bitmap and local ports arr + encap */
    for (ndx = 0; ndx < port_count ; ndx++) {
        if (is_arad_mechanism == 1 && port_array[ndx] == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
            if (!encap_id_array[ndx]) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outlif only replication can not use outlif 0")));
            }
            arad_local_port_arr[port_local_arr_ndx] = BCM_GPORT_INVALID;
            encap_id_arr[port_local_arr_ndx++] = encap_id_array[ndx];
        } else if (is_arad_mechanism == 1 && BCM_GPORT_IS_MCAST(port_array[ndx])) { /* bitmap replication */
            arad_local_port_arr[port_local_arr_ndx] =  BCM_GPORT_MCAST_GET(port_array[ndx]) | ARAD_MC_EGR_IS_BITMAP_BIT;
            if (arad_local_port_arr[port_local_arr_ndx] == ARAD_MC_EGR_IS_BITMAP_BIT) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bitmap replication can not use bitmap 0")));
            }
            encap_id_arr[port_local_arr_ndx++] = encap_id_array[ndx];
        } else { /* should be a local port */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port_array[ndx], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
            if (is_in_range) {
                BCM_PBMP_OR(pbmp, gport_info.pbmp_local_ports);
            } else {
                BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
                    if (is_arad_mechanism) {
                        arad_local_port_arr[port_local_arr_ndx] = local_port;
                    } else {
                        local_port_arr[port_local_arr_ndx] = local_port;
                    }
                    encap_id_arr[port_local_arr_ndx++] = encap_id_array[ndx];
                }
            }
        }
    }

    if (is_in_range == TRUE) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_set(unit, multicast_id_ndx, pbmp));
    } else {
        if (SOC_IS_PETRAB(unit)) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entries_set(unit, multicast_id_ndx, port_local_arr_ndx, local_port_arr, encap_id_arr));
        } else {
            SOC_TMC_ERROR mc_err = BCM_E_NONE;
            for (ndx = 0; ndx < port_local_arr_ndx; ndx++) /* encode replication data */
            {
                uint32 cud; /* needed since encap_id_arr[ndx] is not uint32 */
                BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_encode_replication(unit, arad_local_port_arr + ndx, &cud, arad_local_port_arr[ndx], encap_id_arr[ndx]));
                encap_id_arr[ndx] = cud;
            }
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_group_set,
                (unit, multicast_id_ndx, 0, port_local_arr_ndx, arad_local_port_arr, encap_id_arr, &mc_err)));
            if (mc_err) {
                BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ?  "not enough multicast entries for the operation" :
                  (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "The same group member (port + encap) may not appear multiple times in the input" : "unexpected error")) ));
            }
        }
    }

exit:
    BCM_FREE(local_port_arr);
    BCM_FREE(arad_local_port_arr);
    BCM_FREE(encap_id_arr);
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_fabric_distribution_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_fabric_distribution_t *ds_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_multicast_fabric_distribution_get is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_fabric_distribution_set(
    int unit, 
    bcm_multicast_t group, 
    bcm_fabric_distribution_t ds_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_multicast_fabric_distribution_set is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_group_get(
    int unit, 
    bcm_multicast_t group, 
    uint32 *flags)
{
    int rv;    
    SOC_TMC_MULT_ID multicast_id_ndx;
    int type;
    uint8 is_open = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        *flags = BCM_MULTICAST_TYPE_L2;
    } else if (_BCM_MULTICAST_IS_VPLS(group)) {
        *flags = BCM_MULTICAST_TYPE_VPLS;
    } else if (_BCM_MULTICAST_IS_L3(group)) {
        *flags = BCM_MULTICAST_TYPE_L3;
    } else if (_BCM_MULTICAST_IS_PORTS_GROUP(group)) {
        *flags = BCM_MULTICAST_TYPE_PORTS_GROUP;
    } else if (type != 0) {
        /* This is not supported type, nor type 0 (TM) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported group type")));
    }
     
    *flags |= BCM_MULTICAST_WITH_ID; 

    rv = _bcm_petra_multicast_group_to_id(group, &multicast_id_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_petra_multicast_ingress_is_group_exist(unit,multicast_id_ndx,&is_open);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_open != FALSE) {
            *flags |= BCM_MULTICAST_INGRESS_GROUP;
        }
    rv = _bcm_petra_multicast_egress_is_group_exist(unit,multicast_id_ndx,&is_open);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_open != FALSE) {
        *flags |= BCM_MULTICAST_EGRESS_GROUP;
    } else if (!(BCM_MULTICAST_INGRESS_GROUP & *flags)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MC group 0x%x was not created"), multicast_id_ndx));
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Get reserved MC group init range for warmboot 
 *  Don't allocate these during reload, allow initialization
 *  to occur normally.
 */
int
bcm_petra_multicast_group_reserved_range_get(
                                             int unit, 
                                             uint32 *mc_id_lo, 
                                             uint32 *mc_id_hi)
{
    bcm_error_t rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    if ((mc_id_lo == NULL) || (mc_id_hi == NULL)) {
        rv = BCM_E_PARAM;
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (SOC_IS_ARAD(unit)) {
        
        rv = BCM_E_INTERNAL;
    }
    *mc_id_lo = 0;
    *mc_id_hi = BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE;
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_init(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t multicast_id, temp_id = 0;
    uint32 flags = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) { 
        BCM_EXIT;
    }
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    if (!SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NONE, (_BSL_BCM_MSG("Multicast is not being initialized")));
    }
    if (SOC_DPP_PP_ENABLE(unit)) {

        if (!SOC_WARM_BOOT(unit)) {  /* if coldboot */
            /* Multicast egress groups 0-4K bitmap types are opened by default since they are reserved for vlan(VSI) 0-4K */
            if (SOC_IS_ARAD(unit) && SAL_BOOT_QUICKTURN) {         
              int range_high, range_low;
              SOC_TMC_MULT_ID multicast_id_ndx;

              range_low = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_low; 
              range_high = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;

              for (multicast_id = 0; multicast_id < BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE; ++multicast_id) {     
                  if (!SOC_IS_PETRAB(unit) || multicast_id < range_low || multicast_id > range_high) {
                      /* In case of link list needs to call to regular open sequence */
                      temp_id = multicast_id;
                      flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP;
                                    
                      rv = bcm_petra_multicast_create(unit, flags, &temp_id);
                      BCMDNX_IF_ERR_EXIT(rv);
                  } else {
                      /* In case of Petra vlan membership, only needs to call to allocation management */
                      multicast_id_ndx = multicast_id;
                      rv = bcm_dpp_am_mc_alloc(unit, SHR_RES_ALLOC_WITH_ID, &multicast_id_ndx, TRUE);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }      
                }        
            } else {
                for (multicast_id = 0; multicast_id < BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE; ++multicast_id) {
                    temp_id = multicast_id;
                    flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP;
                    rv = bcm_petra_multicast_create(unit,flags,&temp_id);
                      BCMDNX_IF_ERR_EXIT(rv);
                }
            }      
        }              
    }

#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        rv = _bcm_dpp_wb_multicast_state_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_detach(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
#ifdef BCM_WARM_BOOT_SUPPORT
    /* tear down warm boot state */
    if (SOC_IS_PETRAB(unit)) { /* Petra only multicast code */
	    BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_multicast_state_deinit(unit));
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif
    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_l2_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_vlan_t vlan, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    int encap_type;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    rv = _bcm_dpp_gport_resolve(unit,port, 0 ,&dest_id,encap_id,&encap_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_l3_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t intf, 
    bcm_if_t *encap_id)
{
    SOC_PPD_VSI_ID vsi;
    bcm_vlan_t vid;


    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    unit = (unit);

    /* encap id is simply intf id (RIF) */ 
    vsi = intf;
    vid = vsi;

    *encap_id = SOC_PPD_CUD_VSI_GET(unit,vid);
 
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_egress_object_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_if_t intf, 
    bcm_if_t *encap_id)
{
    SOC_PPD_FEC_ID fec_index;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE
      protect_type;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
      working_fec;
    SOC_PPD_FRWRD_FEC_ENTRY_INFO
      protect_fec;
    SOC_PPD_FRWRD_FEC_PROTECT_INFO
      protect_info;
    bcm_if_t tmp = 0;
    int rv;

    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(encap_id);

    unit = (unit);

    /* get encap id from FEC entry */ 

    /* intf egress object can be FEC or Tunnel or RIF. For RIF use bcm_petra_multicast_l3_encap_get */
    if (_BCM_PETRA_L3_ITF_IS_ENCAP(intf)) {
        tmp = _BCM_PETRA_L3_ITF_VAL_GET(intf);
        
        rv = _bcm_l3_encap_id_to_eep(unit,tmp,encap_id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* FEC */
        rv = _bcm_l3_intf_to_fec(unit,intf,&fec_index);
        BCM_IF_ERROR_RETURN(rv);

        /* read FEC */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(
            unit,
            fec_index,
            &protect_type,
            &working_fec,
            &protect_fec,
            &protect_info
          );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *encap_id = working_fec.eep;
    }
    

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_multicast_mim_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t mim_port_id, 
    bcm_if_t *encap_id)
{
    int rv, gport_type;
    SOC_SAND_PP_DESTINATION_ID  dest_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    /* Resolve fec and encap_id (EEP/I-SID) from mim port id */
    rv = _bcm_dpp_gport_resolve(unit, mim_port_id, _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK, &dest_id, (int*)encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* The encap for trill : port - fec eep */
int 
bcm_petra_multicast_trill_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t intf, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID  dest_id;
    int                     encap_type;
    SOC_PPD_FEC_ID              fec_index;
    bcm_trill_port_t        trill_port;
    
    unsigned int soc_sand_dev_id;
   
    SOC_PPD_FRWRD_FEC_ENTRY_INFO    fec_entry[2];
    SOC_PPD_FRWRD_FEC_PROTECT_INFO  protect_info;
    SOC_PPD_FRWRD_FEC_PROTECT_TYPE  protect_type;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(encap_id);
   
   
    if (BCM_GPORT_IS_TRILL_PORT(port) && SOC_IS_ARAD(unit)) {
        bcm_trill_port_t_init(&trill_port);
        trill_port.trill_port_id = port;

        rv = bcm_petra_trill_port_get(unit,&trill_port);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_l3_intf_to_fec(unit, trill_port.egress_if, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);

        /* get encap ID from FEC - */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_index,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *encap_id = fec_entry[0].eep;
    } else {
        rv = _bcm_l3_intf_to_fec(unit, intf, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_resolve(unit,port,0, &dest_id,encap_id,&encap_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if(dest_id.dest_type != SOC_SAND_PP_DEST_SINGLE_PORT) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("TODO err message")));
        }

        /* get encap ID from FEC - */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_index,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *encap_id = (dest_id.dest_val << _BCM_TRILL_ENCAP_PORT_SHIFT)  | fec_entry[0].eep;
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_vlan_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t vlan_port_id, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    int encap_type;
    int encap_val;


    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    BCMDNX_NULL_CHECK(encap_id);
    if(!BCM_GPORT_IS_VLAN_PORT(vlan_port_id)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given gport is not vlan port")));
    }
    rv = _bcm_dpp_gport_resolve(unit,vlan_port_id,0, &dest_id,encap_id,&encap_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if(encap_type == _BCM_DPP_GPORT_IN_TYPE_AC || encap_type == _BCM_DPP_GPORT_IN_TYPE_FEC) {
        encap_val = *encap_id;

        *encap_id = SOC_PPD_CUD_AC_GET(unit,encap_val);

    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_vpls_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t mpls_port_id, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    int encap_type;
    int lif_id, is_local;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    unit = (unit);

    /* Resolve encap id by mpls port */
    rv = _bcm_dpp_gport_resolve(unit,mpls_port_id,0, &dest_id,encap_id,&encap_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if this is FEC+VC, then get EEP from SW-state */
    if(encap_type == _BCM_DPP_GPORT_IN_TYPE_FEC_VC) {
        rv = _bcm_dpp_gport_to_lif(unit, mpls_port_id, &lif_id, NULL, NULL, &is_local);
        *encap_id = lif_id;
    }
    else if (encap_type == _BCM_DPP_GPORT_TYPE_ENCAP || encap_type == _BCM_DPP_GPORT_IN_TYPE_FEC) {
        /* Translate AC to CUD */
        *encap_id = SOC_PPD_CUD_AC_GET(unit,*encap_id);
    } else {
        /* Trnslate EEP to CUD */
        *encap_id = SOC_PPD_CUD_EEP_GET(unit,*encap_id);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_l2gre_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t l2gre_port_id, 
    bcm_if_t *encap_id)
{
    int rv, gport_type;
    SOC_SAND_PP_DESTINATION_ID  dest_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    /* Resolve fec and encap_id (EEP/I-SID) from mim port id */
    rv = _bcm_dpp_gport_resolve(unit, l2gre_port_id, _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK, &dest_id, (int*)encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_vxlan_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t vxlan_port_id, 
    bcm_if_t *encap_id)
{
    int rv, gport_type;
    SOC_SAND_PP_DESTINATION_ID  dest_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);
    rv = _bcm_dpp_gport_resolve(unit, vxlan_port_id, _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK, &dest_id, (int*)encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_multicast_ingress_add
 * Purpose:
 *      Add a GPORT to the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_add(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_DEST_INFO soc_petra_dest_info;
    uint8 expected_open;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    SOC_TMC_DEST_INFO_clear(&soc_petra_dest_info);
   
    /* Check mc group type is supported */
    rv = _bcm_petra_multicast_is_supported_mc_group(group);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Convert GPORT to TM dest */
    rv = _bcm_dpp_gport_to_tm_dest_info(unit, port, &soc_petra_dest_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  In Arad,
     *  an encap-id of 0 for a destination which is not an
     *  ERP system port or FMQ
     *  needs to be changed 
     */ 
    if (SOC_IS_ARAD(unit) && !encap_id && !BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        if (soc_petra_dest_info.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
          uint32    fap_id;
          uint32    fap_port_id = 0;
          /* get module+port from system port, in order to check if this is an ERP port*/
          rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, soc_petra_dest_info.id, &fap_id, &fap_port_id)));
          BCM_SAND_IF_ERR_EXIT(rv);
          if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
              encap_id = BCM_IF_INVALID;
          }
      } else {
        encap_id = BCM_IF_INVALID;
      }
    }

    rv = _bcm_petra_multicast_group_to_id(group,&multicast_id_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check multicast ID exists */
    expected_open = SOC_SAND_TRUE;
    rv = _bcm_petra_multicast_is_group_exist(unit,BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY,multicast_id_ndx,expected_open);
    BCMDNX_IF_ERR_EXIT(rv);
      
    /* Add multicast entry */
    rv = _bcm_petra_multicast_ingress_entry_add(unit,multicast_id_ndx,soc_petra_dest_info,encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_ingress_delete
 * Purpose:
 *      Delete GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_delete(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_DEST_INFO soc_petra_dest_info;
    uint8 expected_open;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    SOC_TMC_DEST_INFO_clear(&soc_petra_dest_info);
  
    /* Check mc group type is supported */
    rv = _bcm_petra_multicast_is_supported_mc_group(group);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Convert GPORT to TM dest */
    rv = _bcm_dpp_gport_to_tm_dest_info(unit, port, &soc_petra_dest_info);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_multicast_group_to_id(group,&multicast_id_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check multicast ID exists */
    expected_open = SOC_SAND_TRUE;
    rv = _bcm_petra_multicast_is_group_exist(unit,BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY,multicast_id_ndx,expected_open);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  In Arad,
     *  an encap-id of 0 for a destination which is not an
     *  ERP system port or FMQ
     *  needs to be changed 
     */
    if (SOC_IS_ARAD(unit) && !encap_id && !BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
      if (soc_petra_dest_info.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
        uint32    fap_id;
        uint32    fap_port_id = 0;
        /* get module+port from system port, in order to check if this is an ERP port*/
        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, soc_petra_dest_info.id, &fap_id, &fap_port_id)));
        BCM_SAND_IF_ERR_EXIT(rv);
        if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
            encap_id = BCM_IF_INVALID;
        }
      } else {
        encap_id = BCM_IF_INVALID;
      }
    }

    /* Remove entry */
    rv = _bcm_petra_multicast_ingress_entry_remove(unit,multicast_id_ndx,soc_petra_dest_info,encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_ingress_delete_all
 * Purpose:
 *      Delete all GPORTs from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_delete_all(
    int unit, 
    bcm_multicast_t group)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_multicast_ingress_set(unit,group,0,NULL,NULL));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *      bcm_petra_multicast_ingress_get
 * Purpose:
 *      Retrieve a set of ingress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      mc_index   - (IN) Multicast index
 *      port_max   - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of multicast GPORTs 
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of multicasts returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */
int 
bcm_petra_multicast_ingress_get(
    int unit, 
    bcm_multicast_t group, 
    int port_max, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array, 
    int *port_count)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    int port_iter, ndx;
    SOC_TMC_DEST_INFO *dest_info_arr = NULL;
    uint32 group_size;
    uint8 open;
       
    BCMDNX_INIT_FUNC_DEFS;

    /* verify input parameters */
    BCMDNX_NULL_CHECK(port_count);
    if (port_max > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(port_count);
    } else if (port_max < 0) {
        port_max = 0;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group,&multicast_id_ndx)); /* get group mcid */

    if (SOC_IS_PETRAB(unit)) {
        /* Check multicast ID exists */
        open = SOC_SAND_TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY, multicast_id_ndx, open));

        /* Allocate dest info entries */
        BCMDNX_ALLOC(dest_info_arr, sizeof(SOC_TMC_DEST_INFO) * port_max, "multicast ing entries");
        if (dest_info_arr == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed in allocation of temporary memory")));
        }
        /* Clear dest info entries */
        for (ndx = 0; ndx < port_max; ndx++) {
            SOC_TMC_DEST_INFO_clear(&dest_info_arr[ndx]);
        }
        /* Get multicast entries */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_entries_get(unit, multicast_id_ndx, port_max, dest_info_arr, encap_id_array, &group_size));
        *port_count = group_size;

        if (port_max != 0) {
            for (port_iter = 0; port_iter < *port_count; port_iter++) { /* Convert tm dest info to gports */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_from_tm_dest_info(unit, &port_array[port_iter], &dest_info_arr[port_iter])); 
            }
        }

    } else { /* not Petra */

        /* Check multicast group exists (was created) */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, &open));
        if (!open) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_get_group,(unit, multicast_id_ndx, port_max, port_array, encap_id_array, &group_size, &open))); /* do the actual work */
        if (!open) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not clear if ingress multicast group %u was created"), multicast_id_ndx));
        }
        /* if port_max is non zero and not all group replications could be returned,
         * return the number of returned replications (port_max).
         * Otherwise return the exact group size in replications (a bitmap pointer counts as one replication).
         */
        *port_count = port_max && group_size > port_max ? port_max : group_size;
    }
    
exit:
    if (dest_info_arr) {
       BCM_FREE(dest_info_arr);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_ingress_set
 * Purpose:
 *      Assign the complete set of ingress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count   - (IN) Number of ports in replication list
 *      port_array   - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_set(
    int unit, 
    bcm_multicast_t group, 
    int port_count, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ID multicast_id_ndx;
    int ndx;
    SOC_TMC_DEST_INFO *dest_info_arr = NULL;
    bcm_if_t *encap_id_array_copy = NULL;
    uint8 expected_open;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    if (port_count > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(encap_id_array);
    }
    
    /* Check mc group type is supported */
    rv = _bcm_petra_multicast_is_supported_mc_group(group);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate dest info entries */
    BCMDNX_ALLOC(dest_info_arr, sizeof(SOC_TMC_DEST_INFO) * port_count, "dest info entries");
    if (dest_info_arr == NULL) {
        BCMDNX_IF_ERR_EXIT(BCM_E_MEMORY);
    }

    /* Init entries */
    for (ndx = 0; ndx < port_count; ndx ++) {
        SOC_TMC_DEST_INFO_clear(&dest_info_arr[ndx]);
    }

    for (ndx = 0; ndx < port_count; ndx ++) {
        /* Convert GPORT to TM dest */
        rv = _bcm_dpp_gport_to_tm_dest_info(unit, port_array[ndx], &dest_info_arr[ndx]);
        BCMDNX_IF_ERR_EXIT(rv);

       /* 
        *  In Arad,
        *  an encap-id of 0 for a destination which is not an
        *  ERP system port or FMQ
        *  needs to be changed 
        */
        if (SOC_IS_ARAD(unit) && !encap_id_array[ndx] && !BCM_GPORT_IS_MCAST_QUEUE_GROUP(port_array[ndx])) {
            int replace = 0;
            if (dest_info_arr[ndx].type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
                uint32    fap_id;
                uint32    fap_port_id = 0;
                /* get module+port from system port, in order to check if this is an ERP port*/
                rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, dest_info_arr[ndx].id, &fap_id, &fap_port_id)));
                BCM_SAND_IF_ERR_EXIT(rv);
                if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
                    replace = 1;
                }
            } else {
                replace = 1;
            }
            if (replace) {
                if (!encap_id_array_copy) { /* if the encapsulation ids copy array was not allocated, allocate it and populate it from the original */
                    int i;
                    BCMDNX_ALLOC(encap_id_array_copy, sizeof(bcm_if_t) * port_count, "encap_ids");
                    if (encap_id_array_copy == NULL) {
                        BCMDNX_IF_ERR_EXIT(BCM_E_MEMORY);
                    }
                    for (i = 0; i < port_count; ++i) {
                        encap_id_array_copy[i] = encap_id_array[i];
                    }
                }
                encap_id_array_copy[ndx] = BCM_IF_INVALID;
            }
        }

    }

    rv = _bcm_petra_multicast_group_to_id(group,&multicast_id_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check multicast ID exists */
    expected_open = SOC_SAND_TRUE;
    rv = _bcm_petra_multicast_is_group_exist(unit,BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY,multicast_id_ndx,expected_open);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update existed multicast ing group */
    rv = _bcm_petra_multicast_ingress_entries_set(unit, multicast_id_ndx, port_count, dest_info_arr,
                                                  encap_id_array_copy ? encap_id_array_copy : encap_id_array);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCM_FREE(dest_info_arr);
    BCM_FREE(encap_id_array_copy);
    BCMDNX_FUNC_RETURN;
}


/* New APIs needed for Jericho */
int bcm_petra_multicast_get(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int replication_max, 
    bcm_multicast_replication_t *out_rep_array, 
    int *rep_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The API is not yet implemented")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* set multicast destinations */
int bcm_petra_multicast_set(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The API is not yet implemented")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* remove multicast destinations */
int bcm_petra_multicast_delete(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The API is not yet implemented")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Add multicast destinations */
int bcm_petra_multicast_add(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The API is not yet implemented")));
exit:
    BCMDNX_FUNC_RETURN;
}
