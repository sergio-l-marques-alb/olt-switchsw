/*
 * $Id: bfd.c,v 1.58 Broadcom SDK $
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
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/extender.h>

#include <soc/feature.h>
#include <soc/dpp/drv.h>

#if defined(BCM_PETRA_SUPPORT)


/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/



/****************************************************************************/
/*                      Internal functions implmentation                    */
/****************************************************************************/

/*
 * Function:
 *      _bcm_petra_extender_port_add_validate
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
int _bcm_petra_extender_port_add_validate(
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
 *      _bcm_gport_extender_is_valid_replace
 * Purpose:
 *      Validate a port extender object update operation.
 * Parameters:
 *      unit     - Device Number
 *      new_extender_port - A struct with the updated Port Extender attributes
 *      cur_extender_port - A struct with the current Port Extender attributes
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int _bcm_gport_extender_is_valid_replace(
        int unit,
        bcm_extender_port_t *new_extender_port,
        bcm_extender_port_t *cur_extender_port)
{
    BCMDNX_INIT_FUNC_DEFS;

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
    uint8 update = 0, with_id = 0 /*, encap_with_id = 0 */;
    SOC_PPD_LIF_ID global_lif_id;
    int local_in_lif_id = 0, local_out_lif_id = 0, remote_lif = 0, act_fec = 0;
    _bcm_lif_type_e used_type = _bcmDppLifTypeAny;
    bcm_extender_port_t extender_port_cur;
    BCMDNX_INIT_FUNC_DEFS;

    /* Parameter Validations */
    rv = _bcm_petra_extender_port_add_validate(unit, extender_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Consistency validations */

    /* Resolve the physical port */
    rv = _bcm_dpp_gport_to_phy_port(unit, extender_port->port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set API control states - update/with_id */
    update = (extender_port->flags & BCM_EXTENDER_PORT_REPLACE) ? TRUE : FALSE;
    with_id = (extender_port->flags & BCM_EXTENDER_PORT_WITH_ID) ? TRUE : FALSE;
    /* encap_with_id = (extender_port->flags & BCM_EXTENDER_PORT_ENCAP_WITH_ID) ? TRUE : FALSE; */

    /* Remote objects should be supplied with the ID they were created with */
    if(!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) && !(with_id)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In case port is not local BCM_EXTENDER_PORT_WITH_ID must be set")));
    }

    /* update pre proccessing*/


    /* In case a LIF is given, retrieve its type for validation */
    if (with_id) {

        rv = l2_interface_get_lif_from_port_id(extender_port, 0, 0, (SOC_PPD_LIF_ID*)&global_lif_id, BCM_GPORT_EXTENDER_PORT);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Calculate the extender_port_id field */
        rv = l2_interface_calculate_port_id(unit, act_fec, global_lif_id, extender_port, BCM_GPORT_EXTENDER_PORT);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_get_used_type(unit, extender_port->extender_port_id, &used_type);
        if (rv != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* In case of update operation, Validate the LIF usage type and the check
       whether the update operation is allowed. */
    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_EXTENDER_PORT_REPLACE can't be used without BCM_EXTENDER_PORT_WITH_ID as well")));
        }
        else if (used_type == _bcmDppLifTypeAny) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_EXTENDER_PORT_REPLACE, the extender_port_id must exist")));
        }
        else if (used_type != _bcmDppLifTypeExtender) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("extender_port_id already exists as non EXTENDER_PORT type")));
        }
        else {
            /* Retrieve the existing entry with the same extender_port_id */
            bcm_extender_port_t_init(&extender_port_cur);
            extender_port_cur.extender_port_id = extender_port->extender_port_id;
            rv = bcm_petra_extender_port_get(unit, &extender_port_cur);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Check if the update is possible */
            rv = _bcm_gport_extender_is_valid_replace(unit, extender_port, &extender_port_cur);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    else if (with_id && (used_type != _bcmDppLifTypeAny)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case extender_port_id already exists, BCM_EXTENDER_PORT_REPLACE should be used")));
    }

    /* Port level HW update for local gports? */


    /* Perform local extender operations */


        /* Allocate Global/Local LIFs */

        /* Update the In-LIF and Out-LIF SW DBs */
        if (!remote_lif) {
            rv = _bcm_dpp_in_lif_extender_match_add(unit, extender_port, local_in_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = _bcm_dpp_out_lif_usage_set(unit, local_out_lif_id, _bcmDppLifTypeExtender);
        BCMDNX_IF_ERR_EXIT(rv);

    /* Set the encap_id according to the Global LIF-ID */


    /* Perform extender SW DB operations */


        /* Add and entry to the forwarding SW DB */


        /* Update LIF usage type */


    /* Perform local HW extender operations */


        /* If update - Retrieve the In-LIF Info */


        /* Set up ISEM key and data */


        /* Update the ISEM */


        /* Update the HW In-LIF Info*/


        /* Set up ESEM key and data */


        /* Update the ESEM */


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
