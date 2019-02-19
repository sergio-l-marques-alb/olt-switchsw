/*
 * $Id: bfd.c,v 1.58 Broadcom SDK $
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
 *
 * File:    extender.c
 * Purpose: Manage a Controler Bridge as part of a Port Extender network.
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_EXTENDER

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/extender.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>

#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/extender.h>

#include <soc/feature.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPD/ppd_api_port.h>

#if defined(BCM_PETRA_SUPPORT)


/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/



/****************************************************************************/
/*                      Internal functions implmentation                    */
/****************************************************************************/

/*********** SW DBs translation functions *******************/

/*
 * Function:
 *      _bcm_dpp_in_lif_extender_match_add
 * Purpose:
 *      Store match information of an Extender port in an In-LIF SW DB
 * Parameters:
 *      unit    -       (IN) Device Number
 *      extender_port - (IN) Extender port information
 *      in_lif  -       (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_extender_match_add(int unit, bcm_extender_port_t *extender_port, int in_lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.in_lif_sw_resources.flags = extender_port->flags;
    gport_sw_resources.in_lif_sw_resources.port = extender_port->port;
    gport_sw_resources.in_lif_sw_resources.match1 = extender_port->match_vlan;
    gport_sw_resources.in_lif_sw_resources.match2 = extender_port->extended_port_vid;
    gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeExtender;
    gport_sw_resources.in_lif_sw_resources.gport_id =  extender_port->extender_port_id;
    gport_sw_resources.in_lif_sw_resources.vsi = 0;

    rv = _bcm_dpp_lif_sw_resources_set(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_in_lif_extender_match_get
 * Purpose:
 *      Obtain match information of an Extender port from an In-LIF SW DB
 * Parameters:
 *      unit    -       (IN) Device Number
 *      extender_port - (OUT) Extender port information
 *      in_lif  -       (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_in_lif_extender_match_get(int unit, bcm_extender_port_t *extender_port, int in_lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_lif_to_sw_resources(unit, in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    extender_port->flags = gport_sw_resources.in_lif_sw_resources.flags;
    extender_port->port = gport_sw_resources.in_lif_sw_resources.port;
    extender_port->match_vlan = gport_sw_resources.in_lif_sw_resources.match1;
    extender_port->extended_port_vid = gport_sw_resources.in_lif_sw_resources.match2;
    extender_port->extender_port_id = gport_sw_resources.in_lif_sw_resources.gport_id;
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      _bcm_dpp_extender_port_add_validate
 * Purpose:
 *      Initialize the Extender module
 * Parameters:
 *      unit     - Device Number
 *      extender_port - A struct with the Port Extender attributes
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_port_add_validate(
        int unit, 
        bcm_extender_port_t *extender_port)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(extender_port);

    /* Check that only supported flags are set */
    if ((extender_port->flags) & (~(BCM_EXTENDER_PORT_WITH_ID | BCM_EXTENDER_PORT_REPLACE | BCM_EXTENDER_PORT_INGRESS_WIDE))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported Extender Add flags")));
    }


    /* Validate the VLANs */
    BCM_DPP_VLAN_CHK_ID(unit, extender_port->match_vlan);
    BCM_DPP_VLAN_CHK_ID(unit, extender_port->extended_port_vid);

    /* The only supported PCP-DEI type is from the Outer tag */
    if (extender_port->pcp_de_select != BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported Extender PCP-DEI selection")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_extender_get_specified_global_lif
 * Purpose:
 *      Retrieve the Global LIF-ID according to the Extender-Port structure
 *      fields. The function uses given extender_port_id & encap_id fields
 *      according to the set flags.
 *      Return also an indication whether the the specified LIF is a Remote-LIF.
 * Parameters:
 *      unit     - Device Number
 *      extender_port - A struct with the Port Extender fields
 *      specified_global_lif_id - Returned Global LIF-ID or
 *          _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      is_remote_lif - Returned Remote-LIF indication 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_get_specified_global_lif(
        int unit,
        bcm_extender_port_t *extender_port,
        SOC_PPD_LIF_ID *specified_global_lif_id,
        int *is_remote_lif)
{
    uint8 with_id, encap_with_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Set default return values */
    *specified_global_lif_id = _BCM_GPORT_ENCAP_ID_LIF_INVALID;
    *is_remote_lif = FALSE;

    /* Retrieve the relevant flags */
    with_id = (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) ? TRUE : FALSE;
    encap_with_id = (extender_port->flags & BCM_EXTENDER_PORT_ENCAP_WITH_ID) ? TRUE : FALSE;

    /* If the encap_with_id flag is set, the Global LIF-ID is retrieved from the encap_id */
    if (encap_with_id) {
        *specified_global_lif_id = BCM_ENCAP_ID_GET(extender_port->encap_id);

        /* A Remote-LIF indication is encoded in the encap_id */
        if (BCM_ENCAP_REMOTE_GET(extender_port->encap_id)) {
            *is_remote_lif = TRUE;
        }
    }
    /* Otherwise, if with_id, retrieve the Global LIF-ID from the extender_port_id */
    else if (with_id) {
        rv = _bcm_dpp_gport_l2_intf_get_lif_from_port_id(extender_port, 0, 0, specified_global_lif_id, BCM_GPORT_EXTENDER_PORT);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_extender_is_valid_replace
 * Purpose:
 *      Validate the a Port-Extender update operation is valid, as
 *      modification of some fields isn't allowed and can only be achieved by
 *      deleting the Port-Extender and creating a new one.
 * Parameters:
 *      unit     - Device Number
 *      extender_port - A struct with the specified Port Extender fields
 *      extender_port_old - A struct with the Port Extender fields of an
 *                          existing Port-Extender
 *      is_remote_lif - Remote-LIF indication
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_is_valid_replace(
        int unit,
        bcm_extender_port_t *extender_port,
        bcm_extender_port_t *extender_port_old,
        int remote_lif)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_extender_global_lif_validate
 * Purpose:
 *      Validate a specified Global LIF-ID by retrieving a Local LIF-ID and
 *      verifying that the Local LIF-ID usage type is consistent with port add
 *      operation flags.
 * Parameters:
 *      unit     - Device Number
 *      specified_global_lif_id - The specified Global LIF-ID or
 *          _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      is_remote_lif - Remote-LIF indication
 *      extender_port - A struct with the Port Extender fields
 *      extender_port_old - A struct with the Port Extender fields.
 *                          Only in case of update.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_global_lif_validate(
        int unit,
        SOC_PPD_LIF_ID specified_global_lif_id,
        int is_remote_lif,
        bcm_extender_port_t *extender_port,
        bcm_extender_port_t *extender_port_old)
{
    int local_in_lif_id, rv;
    uint8 with_id, update;
    _bcm_lif_type_e used_type = _bcmDppLifTypeAny;
    BCMDNX_INIT_FUNC_DEFS;

    /* Retrieve the relevant flags */
    with_id = (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) ? TRUE : FALSE;
    update = (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) ? TRUE : FALSE;

    if ((!is_remote_lif) && (specified_global_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {

        /* Get the Local In-LIF ID for the specified Global LIF-ID */
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS,
                                                             specified_global_lif_id, &local_in_lif_id);
        /* The function may return not found, and may return E_NONE in some cases, both are valid. */
        if (rv != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv);

            /* Get the SW DB usage type for the In-LIF ID */
            rv = _bcm_dpp_lif_sw_resources_lif_usage_get(unit, local_in_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, &used_type, NULL);
 
            /* The function is allowed to fail for unallocated LIFs and the used_type value will still be correct. */
            if (rv != BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* Validate the specified In-LIF usage type with respect to the required port_add operation */
    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_EXTENDER_PORT_REPLACE can't be used without BCM_EXTENDER_PORT_WITH_ID as well")));
        }
        /* The In-LIF type is validated, except for Remote-LIF that doesn't store any In-LIF */
        else if (!is_remote_lif) {
            if (used_type == _bcmDppLifTypeAny) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_EXTENDER_PORT_REPLACE, the port_extender_id must exist")));
            }
            else if (used_type != _bcmDppLifTypeExtender) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("The port_extender_id already exists as non EXTENDER_PORT type")));
            }
        }

        /* Verify that replace is possible for the modified fields */
        rv = _bcm_dpp_extender_is_valid_replace(unit, extender_port, extender_port_old, is_remote_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (with_id && (used_type != _bcmDppLifTypeAny)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case extender_port_id already exists, BCM_EXTENDER_PORT_REPLACE should be used")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_extender_alloc
 * Purpose:
 *      Perform port extender LIF alloc manager allocations. Return a struct
 *      with the Global LIF-ID and the Local LIF-IDs.
 * Parameters:
 *      unit     - Device Number
 *      specified_global_lif_id - The specified Global LIF-ID or
 *          _BCM_GPORT_ENCAP_ID_LIF_INVALID
 *      extender_port - A struct with the Port Extender fields
 *      extender_lif_resources - Return structure with the LIF IDs
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_alloc(
        int unit,
        SOC_PPD_LIF_ID specified_global_lif_id,
        bcm_extender_port_t *extender_port,
        _bcm_dpp_gport_hw_resources *extender_lif_resources)
{
    uint8 with_id, encap_with_id, update;
    int global_lif_flags = 0, rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Retrieve the relevant flags */
    with_id = (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) ? TRUE : FALSE;
    encap_with_id = (extender_port->flags & BCM_EXTENDER_PORT_ENCAP_WITH_ID) ? TRUE : FALSE;
    update = (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) ? TRUE : FALSE;

    /* Set the Global-LIF allocation flags */
    global_lif_flags |= ((with_id | encap_with_id) && (specified_global_lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID)) ? 
        BCM_DPP_AM_FLAG_ALLOC_WITH_ID : 0;

    /* Reset the return structure */
    sal_memset(extender_lif_resources, 0, sizeof(*extender_lif_resources));
    extender_lif_resources->global_in_lif = specified_global_lif_id;

    if (!update) {

        if (SOC_IS_JERICHO(unit)) {
            bcm_dpp_am_local_inlif_info_t local_inlif_info;
            bcm_dpp_am_local_out_lif_info_t local_out_lif_info;

            sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
            sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

            /* Configure Local In-LIF allocation */
            local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_vpn_ac;
            local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(extender_port->flags, BCM_EXTENDER_PORT_INGRESS_WIDE) ? 
                                                               BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
            local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

            /* Configure Local In-LIF allocation */
            local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_lif_dynamic;
            local_out_lif_info.app_alloc_info.application_type =  bcm_dpp_am_egress_encap_app_out_ac;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

            /* Allocate the global and local lifs. */
            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, global_lif_flags, &(extender_lif_resources->global_in_lif),
                                                           &local_inlif_info, &local_out_lif_info, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Return the allocated local lifs */
            extender_lif_resources->global_out_lif = extender_lif_resources->global_in_lif;
            extender_lif_resources->local_in_lif = local_inlif_info.base_lif_id;
            extender_lif_resources->local_out_lif = local_out_lif_info.base_lif_id;
        } else {

            rv = bcm_dpp_am_l2_vpn_ac_alloc(unit, _BCM_DPP_AM_L2_VPN_AC_TYPE_DEFAULT, global_lif_flags,
                                            (SOC_PPD_LIF_ID *)&(extender_lif_resources->global_in_lif));
            BCMDNX_IF_ERR_EXIT(rv);

            /* Return the allocated local lifs */
            extender_lif_resources->local_in_lif = extender_lif_resources->global_in_lif;
            extender_lif_resources->global_out_lif = extender_lif_resources->local_out_lif = extender_lif_resources->global_in_lif;
        }
    } else { /* Update, No allocation just get the existing LIF information */

        rv = _bcm_dpp_gport_to_hw_resources(unit, extender_port->extender_port_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF,
                                            extender_lif_resources);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_sw_db_hash_extender_update
 * Purpose:
 *      Update the forwarding information SW DB as part of a Port-Extender
 *      creation or update operation (bcm_extender_port_add).
 * Parameters:
 *      unit     - Device Number
 *      is_update - Whether this is part of extender-port creation or update
 *      extender_port - A pointer to the extender-port information
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_sw_db_hash_extender_update(
        int unit,
        int is_update,
        bcm_extender_port_t *extender_port)
{
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Add 1:1 protection support here */


    /* Allocate a forwarding SW DB entry or use the allocated one in case of an
       Update operation (after deleting the content). */
    if (is_update) { 
        /* Remove from hash, reusue same memory*/
        rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &(extender_port->extender_port_id), 
                                           (sw_state_htb_data_t) &phy_port, TRUE);        
        if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
           return BCM_E_INTERNAL;
        }
    }

    /* Fill the forwarding SW DB entry */
    sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));

    phy_port.type = _bcmDppGportResolveTypeAC;
    phy_port.phy_gport = extender_port->port;                      /* physical port*/
    phy_port.encap_id = BCM_ENCAP_ID_GET(extender_port->encap_id); /* encap_id = out-AC */

    /* Insert the entry to the hash table */
    rv = _bcm_dpp_sw_db_hash_vlan_insert(unit,
                                         (sw_state_htb_key_t) &(extender_port->extender_port_id),
                                         (sw_state_htb_data_t) &phy_port);        
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT, (BSL_META_U(unit, "error(%s) Updating Forwarding SW DB hash for Extender-Port 0x%x\n"),
                                    bcm_errmsg(rv), BCM_GPORT_VLAN_PORT_ID_GET(extender_port->extender_port_id)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_init_extender_hw_inlif
 * Purpose:
 *      Initialize Extender In-LIF HW Key and Info structures .
 * Parameters:
 *      unit     - Device Number
 *      is_update - Whether this is part of extender-port creation or update
 *      extender_port_old - A pointer to the extender-port information of an
 *              updated Port Extender.
 *      in_extender_key - A pointer to the retrieved Port Extender In-LIF Key
 *      in_extender_info - A pointer to the retrieved Port Extender In-LIF Info
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_init_hw_inlif(
        int unit,
        int is_update,
        bcm_extender_port_t *extender_port_old,
        SOC_PPC_L2_LIF_EXTENDER_KEY *in_extender_key,
        SOC_PPC_L2_LIF_EXTENDER_INFO *in_extender_info)
{
    SOC_PPC_PORT_INFO port_info;
    int core, in_lif_id;
    uint32  pp_port, soc_sand_rv;
    uint8 found;
    BCMDNX_INIT_FUNC_DEFS;

    /* If update, get the information from the existing Port Extender In-LIF */
    if (is_update) {

        /* Get the Port information in order to retrieve the VLAN-Domain */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, extender_port_old->port, &pp_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, core, pp_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Format the Extender In-LIF Key */
        in_extender_key->extender_port_vid = extender_port_old->extended_port_vid;
        in_extender_key->vid = extender_port_old->match_vlan;
        in_extender_key->name_space = port_info.vlan_domain;

        /* Retrieve the LIF info from the HW by deriving the In-LIF from the Extender-key */
        soc_sand_rv = soc_ppd_l2_lif_extender_get(unit, in_extender_key, (SOC_PPD_LIF_ID *)&in_lif_id, in_extender_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Extender In-LIF Key not found: %d-%d-%d"),
                                in_extender_key->extender_port_vid, in_extender_key->vid, in_extender_key->name_space));
        }
    } else {
        SOC_PPC_L2_LIF_EXTENDER_KEY_clear(in_extender_key);
        SOC_PPC_L2_LIF_EXTENDER_INFO_clear(in_extender_info);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_extender_set_isem_entry
 * Purpose:
 *      Add an ISEM Entry for the Port Extender or update it.
 * Parameters:
 *      unit     - Device Number
 *      is_update - Whether this is part of extender-port creation or update
 *      extender_port - A pointer to the Port Extender configuration
 *      in_extender_key - A pointer to the Port Extender initial In-LIF Key
 *      in_extender_info - A pointer to the Port Extender initial In-LIF Info
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_dpp_extender_set_isem_entry(
        int unit,
        int is_update,
        bcm_extender_port_t *extender_port,
        _bcm_dpp_gport_hw_resources *extender_lif_resources,
        SOC_PPC_L2_LIF_EXTENDER_KEY *in_extender_key,
        SOC_PPC_L2_LIF_EXTENDER_INFO *in_extender_info)
{
    SOC_PPC_PORT_INFO port_info;
    int rv, core;
    uint32  pp_port, soc_sand_rv;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    BCMDNX_INIT_FUNC_DEFS;

    /* Configure the Key in case of a new entry */
    if (!is_update) {
        /* Get the Port information in order to retrieve the VLAN-Domain */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, extender_port->port, &pp_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, core, pp_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Format the Extender In-LIF Key */
        in_extender_key->extender_port_vid = extender_port->extended_port_vid;
        in_extender_key->vid = extender_port->match_vlan;
        in_extender_key->name_space = port_info.vlan_domain;
    }

    /* For a new entry, set default forwarding and learning values, untill there's vswitch assignment */
    if (!is_update) {

        /* by default initiate as P2P with drop destiantion, till attached to service VSI */
        in_extender_info->default_frwrd.default_frwd_type = SOC_PPD_L2_LIF_DFLT_FRWRD_SRC_LIF; 

        SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(in_extender_info->default_frwrd.default_forwarding), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set the learning deafault */
        in_extender_info->service_type = SOC_PPD_L2_LIF_AC_SERVICE_TYPE_AC2AC;
        in_extender_info->vsid = _BCM_PPD_VSI_P2P_SERVICE;
        in_extender_info->learn_record.learn_type = SOC_PPD_L2_LIF_AC_LEARN_DISABLE;
        rv = _bcm_dpp_lif_sw_resources_learn_gport_set(unit, extender_lif_resources->local_in_lif, extender_port->extender_port_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Set Protection attributes */


    /* Set General attributes */
    in_extender_info->is_extended = (extender_port->flags & BCM_EXTENDER_PORT_INGRESS_WIDE) ? TRUE : FALSE;
    in_extender_info->global_lif = extender_lif_resources->global_in_lif;
    in_extender_info->use_lif = 1;

    /* Set the ISEM Entry to the HW */
    soc_sand_rv = soc_ppd_l2_lif_extender_add(unit, in_extender_key, extender_lif_resources->local_in_lif,
                                              in_extender_info, &failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(failure_indication);

exit:
    BCMDNX_FUNC_RETURN;
}


/****************************************************************************/
/*                      API functions implmentation                         */
/****************************************************************************/

/*
 * Function:
 *      bcm_petra_extender_init
 * Purpose:
 *      Initialize the Extender module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_petra_extender_init(int unit)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (!_BCM_DPP_EXTENDER_IS_ENABLED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Port extender soc properties are not set")));
    }

    if (_BCM_DPP_EXTENDER_IS_INIT(unit)) {
        /* Module is initialized, do nothing. */
        BCM_EXIT;
    }

    /* 
     *  HW ACCESS
     */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) { /* Don't access HW during WB. */
#endif
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_init, (unit));
        BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif

    /* Set the port extender module to be initialized */
    SOC_DPP_CONFIG(unit)->extender.port_extender_init_status = soc_dpp_extender_init_status_init;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_extender_deinit
 * Purpose:
 *      De-Initialize the Extender module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_petra_extender_deinit(int unit)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (!_BCM_DPP_EXTENDER_IS_ENABLED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Port extender soc properties are not set")));
    }

    if (!_BCM_DPP_EXTENDER_IS_ENABLED(unit)) {
        /* Module is not initialized, do nothing. */
        BCM_EXIT;
    }

    /* 
     *  HW ACCESS
     */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) { /* Don't access HW during WB. */
#endif
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_deinit, (unit));
        BCMDNX_IF_ERR_EXIT(rv);
#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif


    /* Set the port extender module to be uninitialized */
    SOC_DPP_CONFIG(unit)->extender.port_extender_init_status = soc_dpp_extender_init_status_enabled;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_extender_port_add
 * Purpose:
 *      Create a Port Extender entity
 * Parameters:
 *      unit     - Device Number
 *      extender_port - A struct with the Port Extender attributes
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_petra_extender_port_add(
        int unit,
        bcm_extender_port_t *extender_port)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    uint8 update = 0, with_id = 0, encap_with_id = 0;
    SOC_PPD_LIF_ID specified_global_lif_id;
    int is_remote_lif = 0;
    _bcm_dpp_gport_hw_resources extender_lif_resources;
    bcm_extender_port_t extender_port_old;
    BCMDNX_INIT_FUNC_DEFS;

    /* Parameter Validations */
    rv = _bcm_dpp_extender_port_add_validate(unit, extender_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Resolve the physical port */
    rv = _bcm_dpp_gport_to_phy_port(unit, extender_port->port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set API control states - update/with_id */
    update = (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) ? TRUE : FALSE;
    with_id = (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) ? TRUE : FALSE;
    encap_with_id = (extender_port->flags & BCM_EXTENDER_PORT_ENCAP_WITH_ID) ? TRUE : FALSE;

    /* Remote objects should be supplied with the ID they were created with */
    if (!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) && !(with_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case port is not local BCM_EXTENDER_PORT_WITH_ID must be set")));
    }

    /* If a LIF is specified, determine the Global LIF-ID from the Extender-Port structure fields */
    rv = _bcm_dpp_extender_get_specified_global_lif(unit, extender_port, &specified_global_lif_id, &is_remote_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* In case of update, retrieve Global-LIF info */
    if (update) {
        bcm_extender_port_t_init(&extender_port_old);
        extender_port_old.extender_port_id = extender_port->extender_port_id;
        rv = bcm_petra_extender_port_get(unit, &extender_port_old);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Validate the Global LIF */
    rv = _bcm_dpp_extender_global_lif_validate(unit, specified_global_lif_id, is_remote_lif, extender_port, &extender_port_old);
    BCMDNX_IF_ERR_EXIT(rv);

    /*                                                      */
    /* Consistency validations:                             */
    /* 1. If with_id, retrieve the ISEM entry and verify    */
    /*    the extender_port_id is matched.                  */
    /*                                                      */ 




    /*                                                      */
    /* Perform local-ports-only extender operations         */
    /*                                                      */ 
    if (!is_remote_lif) {

        /* Allocate Global/Local LIFs */
        rv = _bcm_dpp_extender_alloc(unit, specified_global_lif_id, extender_port, &extender_lif_resources);
        BCM_IF_ERROR_RETURN(rv);

        /* Update the In-LIF SW DB */
        rv = _bcm_dpp_in_lif_extender_match_add(unit, extender_port, extender_lif_resources.local_in_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update LIF usage type */
        rv = _bcm_dpp_lif_sw_resources_lif_usage_set(unit, 0, extender_lif_resources.local_out_lif, 0, _bcmDppLifTypeExtender);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        extender_lif_resources.global_out_lif = extender_port->extender_port_id; 
    }

    /* Set the encap_id according to the Global LIF-ID */
    if (!(encap_with_id)) {
        extender_port->encap_id = extender_lif_resources.global_out_lif;
    }

    /* Store the extender_port_id after the allocation */
    if (!update) {
        /* set vlan_port_id */
        rv = _bcm_dpp_gport_l2_intf_calculate_port_id(unit, 0, extender_lif_resources.global_out_lif, extender_port, BCM_GPORT_EXTENDER_PORT);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*                                                      */
    /* Perform extender any-port SW DB operations           */
    /*                                                      */

    /* Add and entry to the forwarding SW DB */
    rv = _bcm_dpp_sw_db_hash_extender_update(unit, update, extender_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /*                                                      */
    /* Perform local-ports-only HW extender operations      */
    /*                                                      */
    if (_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) && (!is_remote_lif)) {

        /* If update - Retrieve the In-LIF Info */
        SOC_PPC_L2_LIF_EXTENDER_KEY     in_extender_key;
        SOC_PPC_L2_LIF_EXTENDER_INFO    in_extender_info;

        rv = _bcm_dpp_extender_init_hw_inlif(unit, update, &extender_port_old, &in_extender_key, &in_extender_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set up an ISEM entry for the Port Extender and the In-LIF table */
        rv = _bcm_dpp_extender_set_isem_entry(unit, update, extender_port, &extender_lif_resources,
                                              &in_extender_key, &in_extender_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_extender_port_get
 * Purpose:
 *      Retrieve a Port Extender entity attributes
 * Parameters:
 *      unit     - Device Number
 *      extender_port - A struct with the Port Extender attributes
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_petra_extender_port_get(
        int unit,
        bcm_extender_port_t *extender_port)
{
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_extender_port_delete
 * Purpose:
 *      Delete a Port Extender entity
 * Parameters:
 *      unit     - Device Number
 *      extender_port_id - The ID of the Port Extender to delete
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_petra_extender_port_delete(
        int unit,
        bcm_gport_t	extender_port_id)
{
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_extender_port_delete_all
 * Purpose:
 *      Delete all Port Extender entities
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int bcm_petra_extender_port_delete_all(
        int unit)
{
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_FUNC_RETURN;
}


#endif /* #if defined(BCM_PETRA_SUPPORT) */
