/*
 * $Id: trx_port.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        port.c
 * Purpose:     Port function implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRX_SUPPORT)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/xgs3.h>

#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      _bcm_trx_egr_src_port_outer_tpid_set
 * Description:
 *      Enable/Disable  outer tpid on all egress ports 
 * Parameters:
 *      unit - Device number
 *      tpid_index - Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_trx_egr_src_port_outer_tpid_set(int unit, int tpid_index, int enable)
{
    uint32      egr_src_port;
    uint32      tpid_enable;
    uint32      new_tpid_enable;
    bcm_port_t  port;
    int         rv;

    PBMP_E_ITER(unit, port) {
        rv = READ_EGR_SRC_PORTr(unit, port, &egr_src_port);
        BCM_IF_ERROR_RETURN(rv);
        tpid_enable = soc_reg_field_get(unit, EGR_SRC_PORTr, 
                                        egr_src_port, OUTER_TPID_ENABLEf);
        if (enable) {
            new_tpid_enable = (tpid_enable | (1 << tpid_index));
        } else {
            new_tpid_enable = (tpid_enable & ~(1 << tpid_index));
        } 

        if (new_tpid_enable != tpid_enable) {
            soc_reg_field_set(unit, EGR_SRC_PORTr, &egr_src_port,
                              OUTER_TPID_ENABLEf, new_tpid_enable);

            rv = WRITE_EGR_SRC_PORTr(unit, port, egr_src_port);
            BCM_IF_ERROR_RETURN(rv);
        }
    } 
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_source_trunk_map_set
 * Description:
 *      Helper funtion to modify fields in SOURCE_TRUNK_MAP memory.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within SOURCE_TRUNK_MAP table entry
 *      value - new field value
 * Return Value:
 *      BCM_E_XXX 
 */
int
_bcm_trx_source_trunk_map_set(int unit, bcm_port_t port, 
                              soc_field_t field, uint32 value)
{
    int num_bits_for_port;  
    bcm_module_t my_modid;
    int index;

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Get local module id. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Calculate table index. */
    num_bits_for_port = _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));

    index = (my_modid << num_bits_for_port) | port;

    return soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, index, field, value);
}

/*
 * Function:
 *      _bcm_esw_source_trunk_map_get
 * Description:
 *      Helper funtion to get fields in SOURCE_TRUNK_MAP memory.
 * Parameters:
 *      unit  - Device number
 *      port  - Port number
 *      field - Field name within SOURCE_TRUNK_MAP table entry
 *      value - New field value
 * Return Value:
 *      BCM_E_XXX 
 */
int
_bcm_trx_source_trunk_map_get(int unit, bcm_port_t port, 
                              soc_field_t field, uint32 *value)
{
    uint32 buf[SOC_MAX_MEM_FIELD_WORDS];
    int num_bits_for_port;  
    bcm_module_t my_modid;
    int index;

    /* Input parameters check */
    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    /* Memory field is valid check. */ 
    if (!SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, field)) {
        return (BCM_E_UNAVAIL);
    }

    /* Port sanity check. */
    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return (BCM_E_PORT);
    }

    /* Get local module id. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Calculate table index. */
    num_bits_for_port = _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));

    index = (my_modid << num_bits_for_port) | port;

    if ((index > soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm)) ||
        (index < soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm))) {
        return (BCM_E_INTERNAL);
    }

    /* Read table entry. */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY, index, buf));

    /* Read requested field value. */
    *value = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm, buf, field);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_trx_port_force_vlan_set
 * Description:
 *     To set the force vlan attribute of a port
 * Parameters:
 *     unit        device number
 *     port        port number
 *     vlan        vlan identifier
 *                 (0 - 4095) - use this VLAN id if egress packet is tagged
 *     pkt_prio    egress packet priority (-1, 0..7)
 *                 any negative priority value disable the priority
 *                 override if the egress packet is tagged
 *     flags       bit fields
 *                 BCM_PORT_FORCE_VLAN_ENABLE - enable force vlan on this
 *                                              port
 *                 BCM_PORT_FORCE_VLAN_UNTAG - egress untagged when force
 *                                             vlan is enabled on this port
 *
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_trx_port_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                            int pkt_prio, uint32 flags)
{
    bcm_port_cfg_t pcfg;
    uint32 reg_val;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!BCM_VLAN_VALID(vlan)) {
        return BCM_E_PARAM;
    }

    if (pkt_prio > 7) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_port_cfg_get(unit, port,
                                                             &pcfg));
    reg_val = 0;

    if (flags & BCM_PORT_FORCE_VLAN_ENABLE) {
        pcfg.pc_pvlan_enable = 1;
        soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                          PVLAN_ENABLEf, 1);
        if (!(flags & BCM_PORT_FORCE_VLAN_UNTAG)) {
            soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                              PVLAN_PVIDf, vlan);
            if (pkt_prio >= 0) {
                soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                                  PVLAN_PRIf, pkt_prio);
                soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                                  PVLAN_RPEf, 1);
            }
        } else {
            soc_reg_field_set(unit, EGR_PVLAN_EPORT_CONTROLr, &reg_val,
                              PVLAN_UNTAGf, 1);
        }
    } else {
        pcfg.pc_pvlan_enable = 0;
    }

    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));
    SOC_IF_ERROR_RETURN(WRITE_EGR_PVLAN_EPORT_CONTROLr(unit, port, reg_val));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_trx_port_force_vlan_get
 * Description:
 *     To get the force vlan attribute of a port
 * Parameters:
 *     unit        device number
 *     port        port number
 *     vlan        pointer to vlan identifier
 *                 (0 - 4095) - use this VLAN id if egress packet is tagged
 *                 valid only when BCM_PORT_FORCE_VLAN_ENABLE is set and
 *                 BCM_PORT_FORCE_VLAN_UNTAG is clear
 *     pkt_prio    egress packet priority (-1, 0 - 7)
 *                 valid only when BCM_PORT_FORCE_VLAN_ENABLE is set and
 *                 BCM_PORT_FORCE_VLAN_UNTAG is clear
 *     flags       bit fields
 *                 BCM_PORT_FORCE_VLAN_ENABLE - enable force vlan on this
 *                                              port
 *                 BCM_PORT_FORCE_VLAN_UNTAG - egress untagged when force
 *                                             vlan is enabled on this port
 *
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_trx_port_force_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vlan,
                            int *pkt_prio, uint32 *flags)
{
    bcm_port_cfg_t pcfg;
    uint32 reg_val;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (!vlan || !pkt_prio || !flags) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_port_cfg_get(unit, port,
                                                             &pcfg));
    SOC_IF_ERROR_RETURN(READ_EGR_PVLAN_EPORT_CONTROLr(unit, port,
                                                      &reg_val));

    *flags = 0;
    if (pcfg.pc_pvlan_enable) {
        *flags |= BCM_PORT_FORCE_VLAN_ENABLE;
        if (!soc_reg_field_get(unit, EGR_PVLAN_EPORT_CONTROLr, reg_val,
                               PVLAN_UNTAGf)) {
            *vlan = soc_reg_field_get(unit, EGR_PVLAN_EPORT_CONTROLr,
                                      reg_val, PVLAN_PVIDf);
            if (soc_reg_field_get(unit, EGR_PVLAN_EPORT_CONTROLr, reg_val,
                                  PVLAN_RPEf)) {
                *pkt_prio = soc_reg_field_get(unit,
                                              EGR_PVLAN_EPORT_CONTROLr,
                                              reg_val, PVLAN_PRIf);
            } else {
                *pkt_prio = -1;
            }
        } else {
            *flags |= BCM_PORT_FORCE_VLAN_UNTAG;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_port_dtag_mode_set
 * Description:
 *      Set the double-tagging mode of a port.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mode - Double-tagging mode, one of:
 *              BCM_PORT_DTAG_MODE_NONE            No double tagging
 *              BCM_PORT_DTAG_MODE_INTERNAL        Service Provider port
 *              BCM_PORT_DTAG_MODE_EXTERNAL        Customer port
 *              BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG  Remove customer tag
 *              BCM_PORT_DTAG_ADD_EXTERNAL_TAG     Add customer tag
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      BCM_PORT_DTAG_MODE_INTERNAL is for service provider ports.
 *              A tag will be added if the packet does not already
 *              have the internal TPID (see bcm_port_tpid_set below).
 *              Internally this sets DT_MODE and clears IGNORE_TAG.
 *      BCM_PORT_DTAG_MODE_EXTERNAL is for customer ports.
 *              The service provider TPID will always be added
 *              (see bcm_port_tpid_set below).
 *              Internally this sets DT_MODE and sets IGNORE_TAG.
 */

int
_bcm_trx_port_dtag_mode_set(int unit, bcm_port_t port, int mode)
{
    bcm_vlan_action_set_t action;
    _bcm_port_info_t      *pinfo;
    int                   dt_mode;
    int                   color_mode;
    int                   rv;

    dt_mode = (mode & (BCM_PORT_DTAG_MODE_INTERNAL | \
                       BCM_PORT_DTAG_MODE_EXTERNAL));

    if (!IS_HG_PORT(unit, port)) {
        /* Modify default ingress actions */
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_default_action_get(unit, port, &action));
        action.dt_outer      = bcmVlanActionNone;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.dt_inner      = bcmVlanActionNone;
        action.dt_inner_prio = bcmVlanActionNone;
        action.ot_outer      = bcmVlanActionNone;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ot_inner      = bcmVlanActionNone;
        action.it_outer      = bcmVlanActionAdd;
        action.it_inner      = bcmVlanActionNone;
        action.it_inner_prio = bcmVlanActionDelete;
        action.ut_outer      = bcmVlanActionAdd;
        if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG) {
            action.ut_inner = bcmVlanActionAdd;
        } else {
            action.ut_inner = bcmVlanActionNone;
        }
        rv = _bcm_trx_vlan_port_default_action_set(unit, port, &action);
        BCM_IF_ERROR_RETURN(rv);

        /* Modify default egress actions */
        rv = _bcm_trx_vlan_port_egress_default_action_get(unit, port, &action);
        BCM_IF_ERROR_RETURN(rv);

        action.ot_inner      = bcmVlanActionNone;
        if (mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
            action.dt_inner = bcmVlanActionDelete;
            action.dt_inner_prio = bcmVlanActionDelete;
        } else {
            action.dt_inner = bcmVlanActionNone;
            action.dt_inner_prio = bcmVlanActionNone;
        }
        action.dt_outer      = bcmVlanActionNone;
        action.dt_outer_prio = bcmVlanActionNone;
        action.ot_outer      = bcmVlanActionNone;
        action.ot_outer_prio = bcmVlanActionNone;
        rv = _bcm_trx_vlan_port_egress_default_action_set(unit, port, &action);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = _bcm_port_info_get(unit, port, &pinfo);
    BCM_IF_ERROR_RETURN(rv);

    pinfo->dtag_mode = mode;
    if (mode == BCM_PORT_DTAG_MODE_NONE) {
        /* Set the default outer TPID. */
        rv = bcm_esw_port_tpid_set(unit, port, 
                                   _bcm_fb2_outer_tpid_default_get(unit));
        BCM_IF_ERROR_RETURN(rv);
    } else if (mode == BCM_PORT_DTAG_MODE_INTERNAL) {
        /* Add the default outer TPID. */
        rv = bcm_esw_switch_control_port_get(unit, port, 
                                             bcmSwitchColorSelect, &color_mode);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_esw_port_tpid_add(unit, port, _bcm_fb2_outer_tpid_default_get(unit),
                                   color_mode);
        BCM_IF_ERROR_RETURN(rv);

    } else if (mode == BCM_PORT_DTAG_MODE_EXTERNAL) {
        /* Disable all outer TPIDs. */
        BCM_IF_ERROR_RETURN (bcm_esw_port_tpid_delete_all(unit, port));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_lport_tab_default_entry_add
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      prof    -  (IN) LPORT memory profile. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_trx_lport_tab_default_entry_add(int unit, soc_profile_mem_t *prof) 
{
    lport_tab_entry_t buf;
    uint32 index;
    void *entry[1];
    int rv;

    /* Input parameters check. */
    if (NULL == prof) {
        return (BCM_E_PARAM);
    }

    sal_memcpy(&buf, soc_mem_entry_null(unit, LPORT_TABm),
               sizeof(soc_mem_entry_null(unit, LPORT_TABm)));

    /* Enable IFP. */
    soc_LPORT_TABm_field32_set(unit, &buf, FILTER_ENABLEf, 1);

    /* Enable VFP. */
    soc_LPORT_TABm_field32_set(unit, &buf, VFP_ENABLEf, 1);

    /* Set port filed select index to PFS index max - 1. */
    if (SOC_MEM_FIELD_VALID(unit, LPORT_TABm, FP_PORT_FIELD_SEL_INDEXf)) {
        soc_LPORT_TABm_field32_set(unit, &buf, FP_PORT_FIELD_SEL_INDEXf,
                                   (soc_mem_index_max(unit, FP_PORT_FIELD_SELm) - 1));
    }

    entry[0] = &buf;
    rv = soc_profile_mem_add(unit, prof, entry, 1, &index);
    if (index != soc_mem_index_min(unit, LPORT_TABm)) {
        /* Something went horribly wrong. */
        return (BCM_E_INTERNAL);
    }
    return (rv);
}

#else /* BCM_TRX_SUPPORT */
int bcm_esw_trx_port_not_empty;
#endif  /* BCM_TRX_SUPPORT */

