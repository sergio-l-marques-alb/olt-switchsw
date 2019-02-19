/*
 * $Id: enduro_oam.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
 * 
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
 */

#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/l3x.h>
#include <soc/enduro.h>

#include <bcm/l3.h>
#include <bcm/oam.h>

#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/enduro.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_ENDURO_SUPPORT)



#define _BCM_OAM_WARM_BOOT_DISABLED

#define BCM_OAM_LM_COUNTER_INDEX_INVALID  -1
#define LEVEL_BIT_COUNT 3
#define LEVEL_COUNT (1 << (LEVEL_BIT_COUNT))
#define MAX_ENDPOINT_LEVEL (LEVEL_COUNT - 1)
#define MANGLED_GROUP_NAME_LENGTH (BCM_OAM_GROUP_NAME_LENGTH)

#define UNSUPPORTED_ENDPOINT_FLAGS \
    (BCM_OAM_ENDPOINT_PORT_STATE_TX | \
    BCM_OAM_ENDPOINT_INTERFACE_STATE_TX)

#define CHECK_INIT \
    if (!oam_info_p->initialized) \
    { \
        return BCM_E_INIT; \
    }

#define SET_OAM_INFO oam_info_p = &en_oam_info[unit];

#define SET_GROUP(_group_index_) group_p = \
    oam_info_p->groups + _group_index_;

#define SET_ENDPOINT(_endpoint_index_) endpoint_p = \
    oam_info_p->endpoints + _endpoint_index_;

#define VALIDATE_GROUP_INDEX(_group_index_) \
    if ((_group_index_) < 0 || (_group_index_) >= oam_info_p->group_count) \
    { \
        return BCM_E_PARAM; \
    } \

#define VALIDATE_ENDPOINT_INDEX(_endpoint_index_) \
    if ((_endpoint_index_) < 0 || \
        (_endpoint_index_) >= oam_info_p->endpoint_count) \
    { \
        return BCM_E_PARAM; \
    } \


/* Cache of ING_SERVICE_PRI_MAP Profile Table */
static soc_profile_mem_t *ing_pri_map_profile[BCM_MAX_NUM_UNITS] = {NULL};
#define ING_SERVICE_PRI_MAP_PROFILE_DEFAULT  0

static _bcm_oam_info_t en_oam_info[BCM_MAX_NUM_UNITS];

static _bcm_oam_fault_t en_group_faults[] =
{
    {CURRENT_XCON_CCM_DEFECTf, STICKY_XCON_CCM_DEFECTf,
        BCM_OAM_GROUP_FAULT_CCM_XCON, 0x08},

    {CURRENT_ERROR_CCM_DEFECTf, STICKY_ERROR_CCM_DEFECTf,
        BCM_OAM_GROUP_FAULT_CCM_ERROR, 0x04},

    {CURRENT_SOME_RMEP_CCM_DEFECTf, STICKY_SOME_RMEP_CCM_DEFECTf,
        BCM_OAM_GROUP_FAULT_CCM_TIMEOUT, 0x02},

    {CURRENT_SOME_RDI_DEFECTf, STICKY_SOME_RDI_DEFECTf,
        BCM_OAM_GROUP_FAULT_REMOTE, 0x01},

    {0, 0, 0, 0}
};

static _bcm_oam_fault_t en_endpoint_faults[] =
{
    {CURRENT_RMEP_PORT_STATUS_DEFECTf, STICKY_RMEP_PORT_STATUS_DEFECTf,
        BCM_OAM_ENDPOINT_FAULT_PORT_DOWN, 0x08},

    {CURRENT_RMEP_INTERFACE_STATUS_DEFECTf,
        STICKY_RMEP_INTERFACE_STATUS_DEFECTf,
        BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN, 0x04},

    {CURRENT_RMEP_CCM_DEFECTf, STICKY_RMEP_CCM_DEFECTf,
        BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT, 0x20},

    {CURRENT_RMEP_LAST_RDIf, STICKY_RMEP_LAST_RDIf,
        BCM_OAM_ENDPOINT_FAULT_REMOTE, 0x10},

    {0, 0, 0, 0}
};

static _bcm_oam_interrupt_t en_interrupts[] =
{
    {ANY_RMEP_TLV_PORT_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_PORT_DOWN_INTRf, bcmOAMEventEndpointPortDown},

    {ANY_RMEP_TLV_PORT_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_PORT_UP_INTRf, bcmOAMEventEndpointPortUp},

    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_DOWN_INTRf, bcmOAMEventEndpointInterfaceDown},

    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_INTRf, bcmOAMEventEndpointInterfaceUp},

    {XCON_CCM_DEFECT_STATUSr, INVALIDf, FIRST_MA_INDEXf,
        ERROR_CCM_DEFECT_INTRf, bcmOAMEventGroupCCMxcon},

    {ERROR_CCM_DEFECT_STATUSr, INVALIDf, FIRST_MA_INDEXf,
        ERROR_CCM_DEFECT_INTRf, bcmOAMEventGroupCCMError},

    {SOME_RDI_DEFECT_STATUSr, FIRST_RMEP_INDEXf, FIRST_MA_INDEXf,
        SOME_RDI_DEFECT_INTRf, bcmOAMEventGroupRemote},

    {SOME_RMEP_CCM_DEFECT_STATUSr, FIRST_RMEP_INDEXf, FIRST_MA_INDEXf,
        SOME_RMEP_CCM_DEFECT_INTRf, bcmOAMEventGroupCCMTimeout},

    {INVALIDr, INVALIDf, 0}
};

static soc_field_t en_interrupt_enable_fields[bcmOAMEventCount] =
{
    /* This must be in the same order as the bcm_oam_event_type_t enum */

    ANY_RMEP_TLV_PORT_DOWN_INT_ENABLEf,
    ANY_RMEP_TLV_PORT_UP_INT_ENABLEf,
    ANY_RMEP_TLV_INTERFACE_DOWN_INT_ENABLEf,
    ANY_RMEP_TLV_INTERFACE_UP_INT_ENABLEf,
    XCON_CCM_DEFECT_INT_ENABLEf,
    ERROR_CCM_DEFECT_INT_ENABLEf,
    SOME_RDI_DEFECT_INT_ENABLEf,
    SOME_RMEP_CCM_DEFECT_INT_ENABLEf
};

static uint32 en_ccm_periods[] =
{
    BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED,
    BCM_OAM_ENDPOINT_CCM_PERIOD_3MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_100MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_1S,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10S,
    BCM_OAM_ENDPOINT_CCM_PERIOD_1M,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10M,
    _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED
};

static void *_bcm_en_oam_alloc_clear(unsigned int size, char *description)
{
    void *block_p;

    block_p = sal_alloc(size, description);

    if (block_p != NULL)
    {
        sal_memset(block_p, 0, size);
    }

    return block_p;
}

static int _bcm_en_oam_handle_interrupt(int unit, soc_field_t fault_field)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_interrupt_t *interrupt_p;
    uint32 interrupt_status;
    bcm_oam_group_t group_index;
    bcm_oam_endpoint_t endpoint_index;
    _bcm_oam_event_handler_t *event_handler_p;
    uint32 flags = 0;
    uint32 address;

    SET_OAM_INFO;

    CHECK_INIT;

    bcm_esw_oam_lock(unit);

    for (interrupt_p = en_interrupts;
        interrupt_p->status_register != INVALIDr;
        ++interrupt_p)
    {
        address = soc_reg_addr(unit, interrupt_p->status_register,
            REG_PORT_ANY, 0);

        if (BCM_FAILURE(soc_reg32_read(unit, address, &interrupt_status)))
        {
            continue;
        }

        if (soc_reg_field_get(unit, interrupt_p->status_register,
            interrupt_status, VALIDf) &&
            oam_info_p->event_handler_count[interrupt_p->event_type] > 0)
        {
            group_index = (interrupt_p->group_index_field != INVALIDf) ?
                soc_reg_field_get(unit, interrupt_p->status_register,
                    interrupt_status, interrupt_p->group_index_field) :
                BCM_OAM_GROUP_INVALID;

            endpoint_index = (interrupt_p->endpoint_index_field != INVALIDf) ?
                soc_reg_field_get(unit, interrupt_p->status_register,
                    interrupt_status, interrupt_p->endpoint_index_field) :
                BCM_OAM_ENDPOINT_INVALID;

            if (endpoint_index != BCM_OAM_ENDPOINT_INVALID)
            {
                /* Find the logical endpoint for this RMEP */

                endpoint_index = oam_info_p->remote_endpoints[endpoint_index];
            }

            flags |= soc_reg_field_get(unit, interrupt_p->status_register,
                interrupt_status, MULTIf) ? BCM_OAM_EVENT_FLAGS_MULTIPLE : 0;

            for (event_handler_p = oam_info_p->event_handler_list_p;
                event_handler_p != NULL;
                event_handler_p = event_handler_p->next_p)
            {
                if (SHR_BITGET(event_handler_p->event_types.w,
                    interrupt_p->event_type))
                {
                    event_handler_p->cb(unit, flags, interrupt_p->event_type,
                        group_index, endpoint_index,
                        event_handler_p->user_data);
                }
            }
        }

        /* Clear interrupt */

        if (BCM_FAILURE(soc_reg32_write(unit, address, 0)))
        {
            continue;
        }
    }

    return bcm_esw_oam_unlock(unit);
}

#ifndef _BCM_OAM_WARM_BOOT_DISABLED
#ifdef BCM_WARM_BOOT_SUPPORT
static int _bcm_en_oam_warm_boot(int unit)
{
    _bcm_oam_info_t *oam_info_p;
    int group_index;
    maid_reduction_entry_t maid_reduction_entry;
    ma_state_entry_t ma_state_entry;
    int maid_reduction_valid;
    int ma_state_valid;
    _bcm_oam_endpoint_t *endpoint_p;
    int l3_entry_count;
    int l3_entry_index;
    l3_entry_ipv4_unicast_entry_t l3_entry;
    uint32 level_bitmap;
    int level;
    int lmep_index;
    lmep_entry_t lmep_entry;

    SET_OAM_INFO;

    /* Get groups */

    for (group_index = 0; group_index < oam_info_p->group_count;
        ++group_index)
    {
        SOC_IF_ERROR_RETURN(READ_MAID_REDUCTIONm(unit,
            MEM_BLOCK_ANY, group_index, &maid_reduction_entry));

        SOC_IF_ERROR_RETURN(READ_MA_STATEm(unit, MEM_BLOCK_ANY, group_index,
            &ma_state_entry));

        maid_reduction_valid = soc_MAID_REDUCTIONm_field32_get(unit,
            &maid_reduction_entry, VALIDf);

        ma_state_valid = soc_MA_STATEm_field32_get(unit,
            &ma_state_entry, VALIDf);

        if (maid_reduction_valid && ma_state_valid)
        {
            if (!maid_reduction_valid && !ma_state_valid)
            {
                return BCM_E_INTERNAL;
            }

            oam_info_p->groups[group_index].in_use = 1;
        }
    }

    /* Get endpoints */

    endpoint_p = oam_info_p->endpoints;
    l3_entry_count = soc_mem_index_count(unit, L3_ENTRY_IPV4_UNICASTm);

    for (l3_entry_index = 0; l3_entry_index < l3_entry_count; ++l3_entry_index)
    {
        SOC_IF_ERROR_RETURN(READ_L3_ENTRY_IPV4_UNICASTm(unit, MEM_BLOCK_ANY,
            l3_entry_index, &l3_entry));

        if (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry, VALIDf))
        {
            switch (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry, KEY_TYPEf))
            {
                case TR_L3_HASH_KEY_TYPE_RMEP:
                    endpoint_p->remote_index =
                        soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                            RMEP__RMEP_PTRf);

                    SOC_IF_ERROR_RETURN(READ_RMEPm(unit, MEM_BLOCK_ANY,
                        endpoint_p->remote_index, &rmep_entry));

                    if (!soc_RMEPm_field32_get(unit, &rmep_entry, VALIDf))
                    {
                        return BCM_E_INTERNAL;
                    }

                    endpoint_p->in_use = 1;
                    endpoint_p->is_remote = 1;

                    endpoint_p->group_index = soc_RMEPm_field32_get(unit,
                        &rmep_entry, MAID_INDEXf);

                    endpoint_p->name = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                        &l3_entry, RMEP__MEPIDf);

                    endpoint_p->level = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                        &l3_entry, RMEP__MDLf);

                    endpoint_p->vlan = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                        &l3_entry, RMEP__VIDf);

                    endpoint_p->glp = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                        &l3_entry, RMEP__SGLPf);

                    SHR_BITSET(oam_info_p->remote_endpoints_in_use,
                        endpoint_p->remote_index);

                    ++endpoint_p;

                    break;

                case TR_L3_HASH_KEY_TYPE_LMEP:
                    level_bitmap = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                        &l3_entry, MDL_BITMAPf);

                    for (level = 0; level < LEVEL_COUNT; ++level)
                    {
                        if (level_bitmap & (1 << level))
                        {
                            /* There's an endpoint here at this level */

                            endpoint_p->local_rx_index =
                                soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                    &l3_entry, LMEP__MA_BASE_PTRf) <<
                                        LEVEL_BIT_COUNT |
                                soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                    &l3_entry, LMEP__MDLf);

                            SOC_IF_ERROR_RETURN(READ_MA_INDEXm(unit,
                                MEM_BLOCK_ANY, endpoint_p->local_rx_index,
                                &ma_index_entry));

                            endpoint_p->in_use = 1;
                            endpoint_p->is_remote = 0;
                            endpoint_p->local_rx_enabled = 1;

                            endpoint_p->group_index =
                                soc_MA_INDEXm_field32_get(unit,
                                    &ma_index_entry, MA_PTRf);

                            /* Name is not used for receive-only endpoints */

                            endpoint_p->level = level;

                            endpoint_p->vlan =
                                soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                    &l3_entry, LMEP__VIDf);

                            endpoint_p->glp =
                                soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                    &l3_entry, LMEP__SGLPf);

                            SHR_BITSET(oam_info_p->local_rx_endpoints_in_use,
                                endpoint_p->local_rx_index);

                            ++endpoint_p;
                        }
                    }

                    break;

                default:
                    continue;
            }
        }
    }

    for (lmep_index = 0; lmep_index < oam_info_p->local_tx_endpoint_count;
        ++lmep_index)
    {
        SOC_IF_ERROR_RETURN(READ_LMEPm(unit, MEM_BLOCK_ANY, lmep_index,
            &lmep_entry));

        if (soc_LMEPm_field32_get(unit, &lmep_entry, VALIDf))
        {
            endpoint_p->in_use = 1;
            endpoint_p->is_remote = 0;
            endpoint_p->local_tx_enabled = 1;
            endpoint_p->local_tx_index = lmep_index;

            endpoint_p->group_index = soc_LMEPm_field32_get(unit,
                &lmep_entry, MAID_INDEXf);

            endpoint_p->name = soc_LMEPm_field32_get(unit, &lmep_entry,
                MEPIDf);

            endpoint_p->level = soc_LMEPm_field32_get(unit, &lmep_entry,
                MDLf);

            endpoint_p->vlan = soc_LMEPm_field32_get(unit, &lmep_entry,
                VLAN_IDf);

            /* glp not used for transmit-only local endpoints */

            SHR_BITSET(oam_info_p->local_tx_endpoints_in_use, lmep_index);

            ++endpoint_p;
        }
    }
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* _BCM_OAM_WARM_BOOT_DISABLED */

static void _bcm_en_oam_uninstall_vfp(int unit, _bcm_oam_endpoint_t *endpoint_p)
{
	_bcm_oam_info_t *oam_info_p;

	SET_OAM_INFO;

	if (endpoint_p->vfp_entry) {
        (void)bcm_esw_field_entry_destroy(unit, endpoint_p->vfp_entry);
        oam_info_p->vfp_entry_count--;
        if (oam_info_p->vfp_entry_count == 0) {
            (void)bcm_esw_field_group_destroy(unit, oam_info_p->vfp_group);
            oam_info_p->vfp_group = 0;
        }
        endpoint_p->vfp_entry = 0;
    }
}

static void _bcm_en_oam_uninstall_fp(int unit,  _bcm_oam_endpoint_t *endpoint_p)
{
	_bcm_oam_info_t *oam_info_p;

	SET_OAM_INFO;

    if (endpoint_p->fp_entry_tx) {
    	(void)bcm_esw_field_entry_destroy(unit, endpoint_p->fp_entry_tx);
    	endpoint_p->fp_entry_tx = 0;
    }
    if (endpoint_p->fp_entry_rx) {
        (void)bcm_esw_field_entry_destroy(unit, endpoint_p->fp_entry_rx);
        endpoint_p->fp_entry_rx = 0;
    }
    if (endpoint_p->vp) {
        if (endpoint_p->in_use) {
            oam_info_p->fp_vp_entry_count--;
        }
        if (oam_info_p->fp_vp_entry_count == 0) {
           (void)bcm_esw_field_group_destroy(unit, oam_info_p->fp_vp_group);
            oam_info_p->fp_vp_group = 0;
        } 
        endpoint_p->vp = 0;  
    } else {
        if (endpoint_p->in_use) {
            oam_info_p->fp_glp_entry_count--;
        }
        if (oam_info_p->fp_glp_entry_count == 0) {
            (void)bcm_esw_field_group_destroy(unit, oam_info_p->fp_glp_group);
            oam_info_p->fp_glp_group = 0;
        }
    }
}

static int _bcm_en_oam_install_vfp(int unit, int endpoint_index, 
    bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_endpoint_t *endpoint_p;
    bcm_mac_t mac_ones = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    
    SET_OAM_INFO;
    SET_ENDPOINT(endpoint_index);
    
    if (oam_info_p->vfp_group == 0) {
        /* Create group first */
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_create(unit,
                                 oam_info_p->vfp_qs,
                                 BCM_FIELD_GROUP_PRIO_ANY,
                                 &oam_info_p->vfp_group));
    }
    BCM_IF_ERROR_RETURN    
        (bcm_esw_field_entry_create(unit,
                                    oam_info_p->vfp_group,
                                    &endpoint_p->vfp_entry));

    BCM_IF_ERROR_RETURN
        (bcm_esw_field_qualify_DstMac(unit, endpoint_p->vfp_entry,
                                      endpoint_info->src_mac_address, mac_ones));
        
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_qualify_SrcMac(unit, endpoint_p->vfp_entry,
                                      endpoint_info->dst_mac_address, mac_ones));
                             
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_qualify_OuterVlanId(unit, endpoint_p->vfp_entry,
                                           endpoint_info->vlan, 0x0fff));
                             
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->vfp_entry,
                                  bcmFieldActionOamPbbteLookupEnable,
                                  1, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_entry_install(unit, endpoint_p->vfp_entry));
        
    oam_info_p->vfp_entry_count++;
    
    return BCM_E_NONE;
}

static int _bcm_en_oam_install_fp(int unit, int endpoint_index, 
    bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_endpoint_t *endpoint_p;
    bcm_field_group_t group;
    bcm_gport_t gport;
    
    SET_OAM_INFO;
    SET_ENDPOINT(endpoint_index);
    
    if (endpoint_p->vp && oam_info_p->fp_vp_group == 0) {
        /* Create group first */
         BCM_IF_ERROR_RETURN(bcm_esw_field_group_create(unit,
                                 oam_info_p->fp_vp_qs,
                                 BCM_FIELD_GROUP_PRIO_ANY,
                                 &oam_info_p->fp_vp_group));
    }
    
    if (!endpoint_p->vp && oam_info_p->fp_glp_group == 0) {
        /* Create group first */
         BCM_IF_ERROR_RETURN(bcm_esw_field_group_create(unit,
                                 oam_info_p->fp_glp_qs,
                                 BCM_FIELD_GROUP_PRIO_ANY,
                                 &oam_info_p->fp_glp_group));
    }

    if (endpoint_p->vp) {
        group = oam_info_p->fp_vp_group;
    } else {
        group = oam_info_p->fp_glp_group;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_field_entry_create(unit,
                                    group,
                                    &endpoint_p->fp_entry_tx));

    BCM_IF_ERROR_RETURN
        (bcm_esw_field_entry_create(unit,
                                    group,
                                    &endpoint_p->fp_entry_rx));
    
    if (endpoint_p->vp) {
    	BCM_GPORT_MIM_PORT_ID_SET(gport, endpoint_p->vp);
    	
    	BCM_IF_ERROR_RETURN
    	    (bcm_esw_field_qualify_SrcMimGport(unit, endpoint_p->fp_entry_tx,
                                               gport));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_DstMimGport(unit, endpoint_p->fp_entry_rx,
                                               gport));
    } else {
    	BCM_IF_ERROR_RETURN
    	    (bcm_esw_field_qualify_SrcPort(unit, endpoint_p->fp_entry_tx,
                    0, 0x3f, endpoint_info->gport, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_OuterVlanId(unit, endpoint_p->fp_entry_tx,
                                               endpoint_info->vlan, 0xfff));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_DstPort(unit, endpoint_p->fp_entry_rx,
                    0, 0x3f, endpoint_info->gport, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_OuterVlanId(unit, endpoint_p->fp_entry_rx,
                                               endpoint_info->vlan, 0xfff));
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
                                  bcmFieldActionOamLmepEnable,
                                  1, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
            bcmFieldActionOamLmEnable,
            endpoint_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT? 1:0, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
            bcmFieldActionOamDmEnable,
            endpoint_info->flags & BCM_OAM_ENDPOINT_DELAY_MEASUREMENT? 1:0, 0));
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
                                  bcmFieldActionOamLmepMdl,
                                  endpoint_info->level, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
             bcmFieldActionOamUpMep,
             endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING? 1:0, 0));
    
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT)
    {
        /* Allocate LM counter and priority mapping table */
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
                                  bcmFieldActionOamLmBasePtr,
                                  endpoint_p->lm_counter_index, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
                                  bcmFieldActionOamServicePriMappingPtr,
                                  endpoint_p->pri_map_index, 0));
    }
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_tx,
             bcmFieldActionOamTx,
             endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING ? 1:0, 0));
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_entry_install(unit, endpoint_p->fp_entry_tx));
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
                                  bcmFieldActionOamLmepEnable,
                                  1, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
            bcmFieldActionOamLmEnable,
            endpoint_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT? 1:0, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
            bcmFieldActionOamDmEnable,
            endpoint_info->flags & BCM_OAM_ENDPOINT_DELAY_MEASUREMENT? 1:0, 0));
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
                                  bcmFieldActionOamLmepMdl,
                                  endpoint_info->level, 0));
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
                 bcmFieldActionOamUpMep,
                 endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING? 1:0, 0));
    
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT)
    {
        /* Allocate LM counter and priority mapping table */
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
                                      bcmFieldActionOamLmBasePtr,
                                      endpoint_p->lm_counter_index, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
                                      bcmFieldActionOamServicePriMappingPtr,
                                      endpoint_p->pri_map_index, 0));
    }
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, endpoint_p->fp_entry_rx,
                 bcmFieldActionOamTx,
                 endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING ? 0:1, 0));

    BCM_IF_ERROR_RETURN
        (bcm_esw_field_entry_install(unit, endpoint_p->fp_entry_rx));
    
    if (endpoint_p->vp) {
        oam_info_p->fp_vp_entry_count++;
    } else {
        oam_info_p->fp_glp_entry_count++;
    }
    return BCM_E_NONE;
}

static void _bcm_en_oam_free_memory(_bcm_oam_info_t *oam_info_p)
{
    sal_free(oam_info_p->remote_endpoints);
    sal_free(oam_info_p->remote_endpoints_in_use);
    sal_free(oam_info_p->local_rx_endpoints_in_use);
    sal_free(oam_info_p->local_tx_endpoints_in_use);
    sal_free(oam_info_p->lm_counter_in_use);
    sal_free(oam_info_p->endpoints);
    sal_free(oam_info_p->groups);
}

/*
 * Function:
 *      bcm_en_oam_init
 * Purpose:
 *      Initialize the OAM subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_init(
    int unit)
{
    _bcm_oam_info_t *oam_info_p;
    bcm_oam_endpoint_t endpoint_index;
    bcm_port_t port_index;
    uint32 register_value, temp_index;
    soc_mem_t mem;
    uint32 mem_entries[8];
    void *entries[1];
    int result, i;

    SET_OAM_INFO;

    if (!oam_info_p->initialized)
    {
        oam_info_p->group_count = soc_mem_index_count(unit, MA_STATEm);

        oam_info_p->groups =
            _bcm_en_oam_alloc_clear(oam_info_p->group_count *
                sizeof(_bcm_oam_group_t), "_bcm_oam_group");

        if (oam_info_p->groups == NULL)
        {
            return BCM_E_MEMORY;
        }

        oam_info_p->remote_endpoint_count = soc_mem_index_count(unit, RMEPm);
        oam_info_p->local_tx_endpoint_count = soc_mem_index_count(unit, LMEPm);

        oam_info_p->local_rx_endpoint_count =
            soc_mem_index_count(unit, MA_INDEXm);

        oam_info_p->endpoint_count = oam_info_p->remote_endpoint_count +
            oam_info_p->local_tx_endpoint_count +
            oam_info_p->local_rx_endpoint_count;

        oam_info_p->endpoints =
            _bcm_en_oam_alloc_clear(oam_info_p->endpoint_count *
                sizeof(_bcm_oam_endpoint_t), "_bcm_oam_endpoint");

        if (oam_info_p->endpoints == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }

        oam_info_p->local_tx_endpoints_in_use =
            _bcm_en_oam_alloc_clear(SHR_BITALLOCSIZE(oam_info_p->
                local_tx_endpoint_count),
            "local_tx_endpoints_in_use");

        if (oam_info_p->local_tx_endpoints_in_use == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }
        
        /* Both TX and RX for one MEP */
        oam_info_p->lm_counter_count = 
            soc_mem_index_count(unit, OAM_LM_COUNTERSm)/2;

        oam_info_p->lm_counter_in_use =
            _bcm_en_oam_alloc_clear(SHR_BITALLOCSIZE(oam_info_p->
                lm_counter_count),
                "lm_counter_in_use");

        if (oam_info_p->lm_counter_in_use == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }

        oam_info_p->local_rx_endpoints_in_use =
            _bcm_en_oam_alloc_clear(SHR_BITALLOCSIZE(oam_info_p->
                local_rx_endpoint_count),
            "local_rx_endpoints_in_use");

        if (oam_info_p->local_rx_endpoints_in_use == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }

        for (endpoint_index = 0;
            endpoint_index < oam_info_p->endpoint_count;
            ++endpoint_index)
        {
            oam_info_p->endpoints[endpoint_index].lm_counter_index =
                BCM_OAM_LM_COUNTER_INDEX_INVALID;
        }

        oam_info_p->remote_endpoints_in_use =
            _bcm_en_oam_alloc_clear(SHR_BITALLOCSIZE(oam_info_p->
                remote_endpoint_count),
            "remote_endpoints_in_use");

        if (oam_info_p->remote_endpoints_in_use == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }

        oam_info_p->remote_endpoints =
            sal_alloc(oam_info_p->remote_endpoint_count *
                sizeof(bcm_oam_endpoint_t), "rmep reverse lookup");

        if (oam_info_p->remote_endpoints == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }

        for (endpoint_index = 0;
            endpoint_index < oam_info_p->remote_endpoint_count;
            ++endpoint_index)
        {
            oam_info_p->remote_endpoints[endpoint_index] =
                BCM_OAM_ENDPOINT_INVALID;
        }

        oam_info_p->mutex = sal_mutex_create("oam_info.mutex");

        if (oam_info_p->mutex == NULL)
        {
            _bcm_en_oam_free_memory(oam_info_p);

            return BCM_E_MEMORY;
        }

        soc_enduro_oam_handler_register(_bcm_en_oam_handle_interrupt);

#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit))
        {
#ifdef _BCM_OAM_WARM_BOOT_DISABLED
            return BCM_E_UNAVAIL;
#else
            _bcm_en_oam_warm_boot(unit);
#endif /* _BCM_OAM_WARM_BOOT_DISABLED */
        }
        else
#endif /* BCM_WARM_BOOT_SUPPORT */
        {
            /* Enable OAM message reception on all ports */

            PBMP_ALL_ITER(unit, port_index)
            {
                bcm_esw_port_control_set(unit, port_index,
                                         bcmPortControlOAMEnable, 1);
            }

            /* Enable CCM reception timeouts */

            register_value = 0;

            soc_reg_field_set(unit, OAM_TIMER_CONTROLr, &register_value,
                TIMER_ENABLEf, 1);

            soc_reg_field_set(unit, OAM_TIMER_CONTROLr, &register_value,
                CLK_GRANf, 1);

            result = WRITE_OAM_TIMER_CONTROLr(unit, register_value);

            if (SOC_FAILURE(result))
            {
                _bcm_en_oam_free_memory(oam_info_p);

                return result;
            }

            /* Common information for outgoing CCM packets */

            result = WRITE_LMEP_COMMON_1r(unit, _BCM_OAM_MAC_DA_UPPER_32);

            if (SOC_FAILURE(result))
            {
                _bcm_en_oam_free_memory(oam_info_p);

                return result;
            }

            /* Enable CCM transmission */

            register_value = 0;

            soc_reg_field_set(unit, OAM_TX_CONTROLr, &register_value,
                TX_ENABLEf, 1);

            soc_reg_field_set(unit, OAM_TX_CONTROLr, &register_value,
                CMIC_BUF_ENABLEf, 1);

            result = WRITE_OAM_TX_CONTROLr(unit, register_value);

            if (SOC_FAILURE(result)) {
                _bcm_en_oam_free_memory(oam_info_p);
                return result;
            }
            
            register_value = 0;

            soc_reg_field_set(unit, LMEP_COMMON_2r, &register_value,
                DA_15_3f, _BCM_OAM_MAC_DA_LOWER_13 >> 3);

            soc_reg_field_set(unit, LMEP_COMMON_2r, &register_value,
                L3f, 1);

            result = WRITE_LMEP_COMMON_2r(unit, register_value);

            if (SOC_FAILURE(result))
            {
                _bcm_en_oam_free_memory(oam_info_p);

                return result;
            }
        }
        
        oam_info_p->vfp_entry_count = 0;
        
        BCM_FIELD_QSET_INIT(oam_info_p->vfp_qs);
        BCM_FIELD_QSET_ADD(oam_info_p->vfp_qs, bcmFieldQualifyDstMac);
        BCM_FIELD_QSET_ADD(oam_info_p->vfp_qs, bcmFieldQualifySrcMac);
        BCM_FIELD_QSET_ADD(oam_info_p->vfp_qs, bcmFieldQualifyOuterVlanId);
        BCM_FIELD_QSET_ADD(oam_info_p->vfp_qs, bcmFieldQualifyStageLookup);
    
        oam_info_p->fp_vp_entry_count = 0;
        oam_info_p->fp_glp_entry_count = 0;

        BCM_FIELD_QSET_INIT(oam_info_p->fp_vp_qs);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_vp_qs, bcmFieldQualifyOuterVlanId);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_vp_qs, bcmFieldQualifyDstMimGport);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_vp_qs, bcmFieldQualifySrcMimGport);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_vp_qs, bcmFieldQualifyStageIngress);
        
        BCM_FIELD_QSET_INIT(oam_info_p->fp_glp_qs);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_glp_qs, bcmFieldQualifyOuterVlanId);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_glp_qs, bcmFieldQualifyDstPort);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_glp_qs, bcmFieldQualifySrcPort);
        BCM_FIELD_QSET_ADD(oam_info_p->fp_glp_qs, bcmFieldQualifyStageIngress);
        
        /* Enable ingress filter for CPU port. */
        bcm_esw_port_control_set(unit, CMIC_PORT(unit), 
                                 bcmPortControlFilterIngress, 1);
        
        /* Initialize the ING_VLAN_TAG_ACTION_PROFILE table */

        if (ing_pri_map_profile[unit] == NULL) {
            ing_pri_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Ing Pri Map Profile Mem");
            if (ing_pri_map_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(ing_pri_map_profile[unit]);
        }

        /* Create profile table cache (or re-init if it already exists) */
        mem = ING_SERVICE_PRI_MAPm;

        SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, 1,
                                               ing_pri_map_profile[unit]));

        /* Initialize the ING_SERVICE_PRI_MAP_PROFILE_DEFAULT. All priorities
         * Map to zero.
         */
        for (i = 0; i < 8; i++) {
            mem_entries[i] = 0;
        }
        entries[0] = &mem_entries;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, ing_pri_map_profile[unit],
                                 (void *) &entries, 8, &temp_index));
        oam_info_p->initialized = 1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_detach
 * Purpose:
 *      Shut down the OAM subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_detach(
    int unit)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_event_handler_t *event_handler_p;
    _bcm_oam_event_handler_t *event_handler_to_delete_p;
    bcm_port_t port_index;
    int rv;

    SET_OAM_INFO;

    if (!oam_info_p->initialized)
    {
        return BCM_E_NONE;
    }

    /* Disable CCM transmission */

    SOC_IF_ERROR_RETURN(WRITE_OAM_TX_CONTROLr(unit, 0));
    
    /* Disable OAM message reception on all ports */

    PBMP_ALL_ITER(unit, port_index)
    {
        BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit,
            port_index, bcmPortControlOAMEnable, 0));
    }

    soc_enduro_oam_handler_register(NULL);

    event_handler_p = oam_info_p->event_handler_list_p;

    while (event_handler_p != NULL)
    {
        event_handler_to_delete_p = event_handler_p;
        event_handler_p = event_handler_p->next_p;

        sal_free(event_handler_to_delete_p);
    }

    _bcm_en_oam_free_memory(oam_info_p);
    
    /* De-initialize the ING_SERVICE_PRI_MAP table */
    rv = soc_profile_mem_destroy(unit, ing_pri_map_profile[unit]);
    SOC_IF_ERROR_RETURN(rv);

    sal_free(ing_pri_map_profile[unit]);
    ing_pri_map_profile[unit] = NULL;
    
    oam_info_p->initialized = 0;

    return BCM_E_NONE;
}

static void _bcm_en_oam_group_name_mangle(uint8 *name_p,
    uint8 *mangled_name_p)
{
    uint8 *byte_p = name_p + BCM_OAM_GROUP_NAME_LENGTH - 1;
    int bytes_left = BCM_OAM_GROUP_NAME_LENGTH;

    while (bytes_left > 0)
    {
        *mangled_name_p = *byte_p;

        ++mangled_name_p;
        --byte_p;
        --bytes_left;
    }
}

/*
 * Function:
 *      bcm_en_oam_group_create
 * Purpose:
 *      Create or replace an OAM group object
 * Parameters:
 *      unit - (IN) Unit number.
 *      group_info - (IN/OUT) Pointer to an OAM group structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_group_create(
    int unit, 
    bcm_oam_group_info_t *group_info)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_group_t *group_p;
    int replace;
    int group_index;
    maid_reduction_entry_t maid_reduction_entry;
    ma_state_entry_t ma_state_entry;
    uint8 mangled_group_name[MANGLED_GROUP_NAME_LENGTH];

    SET_OAM_INFO;

    CHECK_INIT;

    replace = group_info->flags & BCM_OAM_GROUP_REPLACE;

    if (group_info->flags & BCM_OAM_GROUP_WITH_ID)
    {
        group_index = group_info->id;

        VALIDATE_GROUP_INDEX(group_index);

        if (!replace && (oam_info_p->groups[group_index].in_use))
        {
            return BCM_E_EXISTS;
        }
    }
    else
    {
        if (replace)
        {
            return BCM_E_PARAM;
        }

        for (group_index = 0; group_index < oam_info_p->group_count;
            ++group_index)
        {
            if (!oam_info_p->groups[group_index].in_use)
            {
                break;
            }
        }

        if (group_index >= oam_info_p->group_count)
        {
            return BCM_E_FULL;
        }

        group_info->id = group_index;
    }

    SET_GROUP(group_index);

    memcpy(group_p->name, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);

    /* MAID_REDUCTION entry */

    _bcm_en_oam_group_name_mangle(group_p->name, mangled_group_name);

    sal_memset(&maid_reduction_entry, 0, sizeof(maid_reduction_entry_t));

    soc_MAID_REDUCTIONm_field32_set(unit, &maid_reduction_entry, REDUCED_MAIDf,
        soc_draco_crc32(mangled_group_name, MANGLED_GROUP_NAME_LENGTH));

    soc_MAID_REDUCTIONm_field32_set(unit, &maid_reduction_entry, SW_RDIf,
        (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) ? 1 : 0);

    soc_MAID_REDUCTIONm_field32_set(unit, &maid_reduction_entry, VALIDf, 1);

    SOC_IF_ERROR_RETURN(WRITE_MAID_REDUCTIONm(unit, MEM_BLOCK_ALL,
        group_index, &maid_reduction_entry));

    /* MA_STATE entry */

    sal_memset(&ma_state_entry, 0, sizeof(ma_state_entry_t));

    soc_MA_STATEm_field32_set(unit, &ma_state_entry, VALIDf, 1);

    SOC_IF_ERROR_RETURN(WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, group_index,
        &ma_state_entry));

    /* Local control block */

    group_p->in_use = 1;

    return BCM_E_NONE;
}

static int _bcm_en_oam_read_clear_faults(int unit, int entry_index,
    _bcm_oam_fault_t *faults, soc_mem_t mem, uint32 *entry_p,
    uint32 *fault_bits_p, uint32 *persistent_fault_bits_p,
    uint32 clear_persistent_fault_bits)
{
    _bcm_oam_fault_t *fault_p;
    uint32 ccm_read_control_reg_value = 0;

    for (fault_p = faults; fault_p->mask != 0; ++fault_p)
    {
        /* Current fault state */

        if (soc_mem_field32_get(unit, mem, entry_p,
            fault_p->current_field) != 0)
        {
            *fault_bits_p |= fault_p->mask;
        }

        /* Sticky fault state */

        if (soc_mem_field32_get(unit, mem, entry_p,
            fault_p->sticky_field) != 0)
        {
            *persistent_fault_bits_p |= fault_p->mask;

            /* Clear persistent faults if requested */

            if (clear_persistent_fault_bits & fault_p->mask)
            {
                soc_reg_field_set(unit, CCM_READ_CONTROLr,
                    &ccm_read_control_reg_value, BITS_TO_CLEARf,
                    fault_p->clear_sticky_mask);
            }
        }
    }

    /* If any clear bits were set, do the clear now */

    if (ccm_read_control_reg_value != 0)
    {
        soc_reg_field_set(unit, CCM_READ_CONTROLr,
            &ccm_read_control_reg_value, ENABLE_CLEARf, 1);

        soc_reg_field_set(unit, CCM_READ_CONTROLr,
            &ccm_read_control_reg_value, MEMORYf, 1);

        soc_reg_field_set(unit, CCM_READ_CONTROLr,
            &ccm_read_control_reg_value, INDEXf, entry_index);

        SOC_IF_ERROR_RETURN(WRITE_CCM_READ_CONTROLr(unit,
            ccm_read_control_reg_value));
    }

    return BCM_E_NONE;
}

static int _bcm_en_oam_get_group(int unit, int group_index,
    _bcm_oam_group_t *group_p, bcm_oam_group_info_t *group_info)
{
    maid_reduction_entry_t maid_reduction_entry;
    ma_state_entry_t ma_state_entry;

    group_info->id = group_index;

    memcpy(group_info->name, group_p->name, BCM_OAM_GROUP_NAME_LENGTH);

    /* MAID_REDUCTION entry */

    SOC_IF_ERROR_RETURN(READ_MAID_REDUCTIONm(unit, MEM_BLOCK_ANY, group_index,
        &maid_reduction_entry));

    if (soc_MAID_REDUCTIONm_field32_get(unit, &maid_reduction_entry,
        SW_RDIf) != 0)
    {
        group_info->flags |= BCM_OAM_GROUP_REMOTE_DEFECT_TX;
    }

    /* MA_STATE entry */

    SOC_IF_ERROR_RETURN(READ_MA_STATEm(unit, MEM_BLOCK_ANY, group_index,
        &ma_state_entry));

    return _bcm_en_oam_read_clear_faults(unit, group_index, en_group_faults,
        MA_STATEm, (uint32 *) &ma_state_entry, &group_info->faults,
        &group_info->persistent_faults, group_info->clear_persistent_faults);
}

/*
 * Function:
 *      bcm_en_oam_group_get
 * Purpose:
 *      Get an OAM group object
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The ID of the group object to get
 *      group_info - (OUT) Pointer to an OAM group structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_group_get(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_group_t *group_p;

    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_GROUP_INDEX(group);

    SET_GROUP(group);

    if (!group_p->in_use)
    {
        return BCM_E_NOT_FOUND;
    }

    return _bcm_en_oam_get_group(unit, group, group_p, group_info);
}

static int _bcm_en_oam_destroy_group(int unit, int group_index,
    _bcm_oam_group_t *group_p)
{
    maid_reduction_entry_t maid_reduction_entry;
    ma_state_entry_t ma_state_entry;

    /* Remove all associated endpoints */

    bcm_en_oam_endpoint_destroy_all(unit, group_index);

    /* MAID_REDUCTION entry */

    soc_MAID_REDUCTIONm_field32_set(unit, &maid_reduction_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_MAID_REDUCTIONm(unit, MEM_BLOCK_ALL,
        group_index, &maid_reduction_entry));

    /* MA_STATE entry */

    soc_MA_STATEm_field32_set(unit, &ma_state_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, group_index,
        &ma_state_entry));

    group_p->in_use = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_group_destroy
 * Purpose:
 *      Destroy an OAM group object.  All OAM endpoints associated
 *      with the group will also be destroyed.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The ID of the OAM group object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_group_destroy(
    int unit, 
    bcm_oam_group_t group)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_group_t *group_p;

    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_GROUP_INDEX(group);

    SET_GROUP(group);

    if (!group_p->in_use)
    {
        return BCM_E_NOT_FOUND;
    }

    return _bcm_en_oam_destroy_group(unit, group, group_p);
}

/*
 * Function:
 *      bcm_en_oam_group_destroy_all
 * Purpose:
 *      Destroy all OAM group objects.  All OAM endpoints will also be
 *      destroyed.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_group_destroy_all(
    int unit)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_group_t *group_p;
    int group_index;

    SET_OAM_INFO;

    CHECK_INIT;

    for (group_index = 0; group_index < oam_info_p->group_count;
        ++group_index)
    {
        SET_GROUP(group_index);

        if (group_p->in_use)
        {
            BCM_IF_ERROR_RETURN(_bcm_en_oam_destroy_group(unit, group_index,
                group_p));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_group_traverse
 * Purpose:
 *      Traverse the entire set of OAM groups, calling a specified
 *      callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for each OAM group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_group_traverse(
    int unit, 
    bcm_oam_group_traverse_cb cb, 
    void *user_data)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_group_t *group_p;
    int group_index;
    bcm_oam_group_info_t group_info;

    SET_OAM_INFO;

    CHECK_INIT;

    if (cb == NULL)
    {
        return BCM_E_PARAM;
    }

    for (group_index = 0; group_index < oam_info_p->group_count;
        ++group_index)
    {
        SET_GROUP(group_index);

        if (group_p->in_use)
        {
            bcm_oam_group_info_t_init(&group_info);

            BCM_IF_ERROR_RETURN(_bcm_en_oam_get_group(unit, group_index,
                group_p, &group_info));

            BCM_IF_ERROR_RETURN(cb(unit, &group_info, user_data));
        }
    }

    return BCM_E_NONE;
}

static int _bcm_en_oam_find_free_endpoint(SHR_BITDCL *endpoints,
    int endpoint_count, int increment, int offset)
{
    int endpoint_index;

    for (endpoint_index = 0; endpoint_index < endpoint_count;
        endpoint_index += increment)
    {
        if (!SHR_BITGET(endpoints, endpoint_index + offset))
        {
            break;
        }
    }

    if (endpoint_index >= endpoint_count)
    {
        endpoint_index = -1;
    }

    return endpoint_index + offset;
}

static int _bcm_en_oam_quantize_ccm_period(int period)
{
    int quantized_period;

    if (period == 0)
    {
        quantized_period = 0;
    }
    else
    {
        /* Find closest supported period */

        for (quantized_period = 1;
            en_ccm_periods[quantized_period] !=
                _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED; ++quantized_period)
        {
            if (period < en_ccm_periods[quantized_period])
            {
                break;
            }
        }

        if (quantized_period > 1)
        {
            if (en_ccm_periods[quantized_period] ==
                _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED)
            {
                /* Use the highest defined value */

                --quantized_period;
            }
            else
            {
                if (period - en_ccm_periods[quantized_period - 1] <
                    en_ccm_periods[quantized_period] - period)
                {
                    /* Closer to the lower value */

                    --quantized_period;
                }
            }
        }
    }

    return quantized_period;
}

static void _bcm_en_oam_make_rmep_key(int unit,
    l3_entry_ipv4_unicast_entry_t *l3_key_p, uint16 name, int level, bcm_vlan_t vlan,
    uint32 sglp)
{
    sal_memset(l3_key_p, 0, sizeof(l3_entry_ipv4_unicast_entry_t));

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, RMEP__MEPIDf, name);
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, RMEP__MDLf, level);
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, RMEP__VIDf, vlan);
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, RMEP__SGLPf, sglp);

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, KEY_TYPEf,
        TR_L3_HASH_KEY_TYPE_RMEP);
}

static int _bcm_en_oam_delete_rmep(int unit, _bcm_oam_endpoint_t *endpoint_p)
{
    _bcm_oam_info_t *oam_info_p;
    l3_entry_ipv4_unicast_entry_t l3_key;

    SET_OAM_INFO;

    /* RMEP entry */

    soc_mem_field32_modify(unit, RMEPm, endpoint_p->remote_index, VALIDf, 0);

    /* L3 entry */
    if (endpoint_p->vp) {
        _bcm_en_oam_make_rmep_key(unit, &l3_key, endpoint_p->name,
            endpoint_p->level, 0, endpoint_p->vp);
    } else {
        _bcm_en_oam_make_rmep_key(unit, &l3_key, endpoint_p->name,
            endpoint_p->level, endpoint_p->vlan, endpoint_p->glp);
    }
    
    SOC_IF_ERROR_RETURN(soc_mem_delete(unit, L3_ENTRY_IPV4_UNICASTm,
        MEM_BLOCK_ALL, &l3_key));

    /* Local control block */

    SHR_BITCLR(oam_info_p->remote_endpoints_in_use, endpoint_p->remote_index);

    oam_info_p->remote_endpoints[endpoint_p->remote_index] =
        BCM_OAM_ENDPOINT_INVALID;

    return BCM_E_NONE;
}

static int _bcm_en_oam_find_lmep(int unit, bcm_vlan_t vlan, uint32 sglp,
    int *index_p, l3_entry_ipv4_unicast_entry_t *l3_entry_p)
{
    l3_entry_ipv4_unicast_entry_t l3_key;

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_key, LMEP__VIDf, vlan);

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_key, LMEP__SGLPf, sglp);

    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_key, KEY_TYPEf,
        TR_L3_HASH_KEY_TYPE_LMEP);

    return soc_mem_search(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ANY,
        index_p, &l3_key, l3_entry_p, 0);
}

static int _bcm_en_oam_delete_lmep(int unit,
    _bcm_oam_endpoint_t *endpoint_p)
{
    _bcm_oam_info_t *oam_info_p;
    int l3_index;
    l3_entry_ipv4_unicast_entry_t l3_entry;
    uint32 level_bitmap;
    int result = SOC_E_NONE;

    SET_OAM_INFO;

    if (endpoint_p->local_tx_enabled)
    {
        /* LMEP entry */

        SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
            LMEPm,endpoint_p->local_tx_index, CCMf, 0));

        /* Local control block */

        SHR_BITCLR(oam_info_p->local_tx_endpoints_in_use,
            endpoint_p->local_tx_index);
    }

    if (endpoint_p->local_rx_enabled)
    {
        /* MA_INDEX entry doesn't need to be touched */

        /* L3 entry */

        _bcm_esw_l3_lock(unit);

        if (BCM_SUCCESS(_bcm_en_oam_find_lmep(unit, endpoint_p->vlan,
            endpoint_p->glp, &l3_index, &l3_entry)) || 
            BCM_SUCCESS(_bcm_en_oam_find_lmep(unit, 0,
            endpoint_p->vp, &l3_index, &l3_entry)))
        {
            level_bitmap = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                &l3_entry, LMEP__MDL_BITMAPf);

            level_bitmap &= ~(1 << endpoint_p->level);

            if (level_bitmap != 0)
            {
                /* Still endpoints here at other levels */

                result = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                    l3_index, LMEP__MDL_BITMAPf, level_bitmap);
            }
            else
            {
                /* No endpoints left here at any level */

                result = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                    l3_index, VALIDf, 0);
            }
        }

        _bcm_esw_l3_unlock(unit);

        SOC_IF_ERROR_RETURN(result);

        /* Local control block */

        SHR_BITCLR(oam_info_p->local_rx_endpoints_in_use,
            endpoint_p->local_rx_index);
            
        if (endpoint_p->lm_counter_index != BCM_OAM_LM_COUNTER_INDEX_INVALID)
        {
            SHR_BITCLR(oam_info_p->lm_counter_in_use,
                endpoint_p->lm_counter_index);
            endpoint_p->lm_counter_index = BCM_OAM_LM_COUNTER_INDEX_INVALID;
            result = soc_profile_mem_delete(unit, ing_pri_map_profile[unit],
                                            endpoint_p->pri_map_index);
            endpoint_p->pri_map_index = 0;
        }
        
        _bcm_en_oam_uninstall_vfp(unit, endpoint_p);
        _bcm_en_oam_uninstall_fp(unit, endpoint_p);
    }
    return result;
}

static int _bcm_en_oam_destroy_endpoint(int unit,
    _bcm_oam_endpoint_t *endpoint_p)
{
    if (endpoint_p->is_remote)
    {
        BCM_IF_ERROR_RETURN(_bcm_en_oam_delete_rmep(unit, endpoint_p));
    }
    else
    {
        BCM_IF_ERROR_RETURN(_bcm_en_oam_delete_lmep(unit, endpoint_p));
    }

    endpoint_p->in_use = 0;
    endpoint_p->lm_counter_index = BCM_OAM_LM_COUNTER_INDEX_INVALID;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_endpoint_create
 * Purpose:
 *      Create or replace an OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_endpoint_create(
    int unit, 
    bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_group_t *group_p;
    _bcm_oam_endpoint_t *endpoint_p;
    int i, replace;
    int is_remote, is_local;
    int local_tx_enabled = 0;
    int local_rx_enabled = 0;
    int endpoint_index;
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    int local_id;
    uint32 glp = 0, vp = 0;
    rmep_entry_t rmep_entry;
    lmep_entry_t lmep_entry;
    lmep_1_entry_t lmep1_entry;
    ma_index_entry_t ma_index_entry;
    l3_entry_ipv4_unicast_entry_t l3_entry;
    int quantization_index;
    int l3_index;
    uint32 level_bitmap, mem_entries[8], temp_index;
    int result = SOC_E_NONE;
    void *entries[1];
    
    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_GROUP_INDEX(endpoint_info->group);

    if (endpoint_info->level > MAX_ENDPOINT_LEVEL)
    {
        return BCM_E_PARAM;
    }

    if (endpoint_info->flags & UNSUPPORTED_ENDPOINT_FLAGS)
    {
        return BCM_E_UNAVAIL;
    }

    if (endpoint_info->type != bcmOAMEndpointTypeEthernet)
    {
        /* Other types not supported in en */

        return BCM_E_UNAVAIL;
    }

    replace = ((endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) != 0);

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)
    {
        if (endpoint_info->flags &
            (BCM_OAM_ENDPOINT_CCM_RX | BCM_OAM_ENDPOINT_LOOPBACK |
            BCM_OAM_ENDPOINT_DELAY_MEASUREMENT | BCM_OAM_ENDPOINT_LINKTRACE |
            BCM_OAM_ENDPOINT_LOSS_MEASUREMENT | 
            BCM_OAM_ENDPOINT_PORT_STATE_TX | 
            BCM_OAM_ENDPOINT_INTERFACE_STATE_TX ))
        {
            /* Specified flags aren't valid for RMEPs */

            return BCM_E_PARAM;
        }

        is_remote = 1;
    }
    else
    {
        /* Local */

        is_remote = 0;

        local_tx_enabled = (endpoint_info->ccm_period !=
            BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED);

        local_rx_enabled = ((endpoint_info->flags &
            (BCM_OAM_ENDPOINT_CCM_RX | BCM_OAM_ENDPOINT_LOOPBACK |
            BCM_OAM_ENDPOINT_DELAY_MEASUREMENT |
            BCM_OAM_ENDPOINT_LOSS_MEASUREMENT |
            BCM_OAM_ENDPOINT_LINKTRACE)) != 0);
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)
    {
        endpoint_index = endpoint_info->id;

        VALIDATE_ENDPOINT_INDEX(endpoint_index);

        if (replace && (!oam_info_p->endpoints[endpoint_index].in_use))
        {
            return BCM_E_NOT_FOUND;
        }
        else if (!replace && (oam_info_p->endpoints[endpoint_index].in_use))
        {
            return BCM_E_EXISTS;
        }
    }
    else
    {
        if (replace)
        {
            /* Replace specified with no ID */

            return BCM_E_PARAM;
        }

        for (endpoint_index = 0; endpoint_index < oam_info_p->endpoint_count;
            ++endpoint_index)
        {
            if (!oam_info_p->endpoints[endpoint_index].in_use)
            {
                break;
            }
        }

        if (endpoint_index >= oam_info_p->endpoint_count)
        {
            return BCM_E_FULL;
        }
        
        endpoint_info->id = endpoint_index;
    }

    SET_ENDPOINT(endpoint_index);

    SET_GROUP(endpoint_info->group);

    if (!group_p->in_use)
    {
        /* Associating to a nonexistent group */

        return BCM_E_PARAM;
    }

    if (replace)
    {
        /* Remove anything being replaced from tables */

        BCM_IF_ERROR_RETURN(_bcm_en_oam_destroy_endpoint(unit, endpoint_p));
    }

    quantization_index =
        _bcm_en_oam_quantize_ccm_period(endpoint_info->ccm_period);

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, endpoint_info->gport,
        &module_id, &port_id, &trunk_id, &local_id));

    if (BCM_GPORT_IS_LOCAL(endpoint_info->gport) ||
        BCM_GPORT_IS_MODPORT(endpoint_info->gport))
    {
        glp = (module_id << 6) | port_id;
    }
    else if (BCM_GPORT_IS_TRUNK(endpoint_info->gport))
    {
        glp = (1 << 12) | trunk_id;
    }
    else if (BCM_GPORT_IS_MIM_PORT(endpoint_info->gport) ||
             BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport))
    {
        vp = local_id;
        result = _bcm_esw_modid_is_local(unit, module_id, &is_local);
        if (trunk_id != BCM_TRUNK_INVALID)
        {
            glp = (1 << 12) | trunk_id;
        } 
        else if (is_local)
        {
            glp = (module_id << 6) | port_id;
        }
        else
        {
            return BCM_E_PARAM;
        }
    }
    else
    {
        /* Other gport types not supported */
        return BCM_E_PARAM;
    }

    endpoint_p->is_remote = is_remote;
    endpoint_p->local_tx_enabled = local_tx_enabled;
    endpoint_p->local_rx_enabled = local_rx_enabled;
    endpoint_p->group_index = endpoint_info->group;
    endpoint_p->name = endpoint_info->name;
    endpoint_p->level = endpoint_info->level;
    endpoint_p->vlan = endpoint_info->vlan;
    endpoint_p->vp = vp;
    endpoint_p->glp = glp;

    if (is_remote)
    {
        endpoint_p->remote_index =
            _bcm_en_oam_find_free_endpoint(oam_info_p->
                remote_endpoints_in_use,
            oam_info_p->remote_endpoint_count, 1, 0);

        if (endpoint_p->remote_index < 0)
        {
            return BCM_E_FULL;
        }

        /* RMEP entry */

        sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));

        soc_RMEPm_field32_set(unit, &rmep_entry, MAID_INDEXf,
            endpoint_info->group);

        soc_RMEPm_field32_set(unit, &rmep_entry, VALIDf, 1);

        SOC_IF_ERROR_RETURN(WRITE_RMEPm(unit, MEM_BLOCK_ALL,
            endpoint_p->remote_index, &rmep_entry));

        /* L3 entry */

        sal_memset(&l3_entry, 0, sizeof(l3_entry_ipv4_unicast_entry_t));

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__CCMf,
            quantization_index);

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__RMEP_PTRf,
            endpoint_p->remote_index);

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__MEPIDf,
            endpoint_info->name);

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__MDLf,
            endpoint_info->level);

        if (vp) {
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__VIDf, 0);
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__SGLPf, vp);
        } else {
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__VIDf,
            endpoint_info->vlan);
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__SGLPf, glp);
        }

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, KEY_TYPEf,
            TR_L3_HASH_KEY_TYPE_RMEP);

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, VALIDf, 1);

        SOC_IF_ERROR_RETURN(soc_mem_insert(unit, L3_ENTRY_IPV4_UNICASTm,
            MEM_BLOCK_ALL, &l3_entry));

        /* Local control block */

        SHR_BITSET(oam_info_p->remote_endpoints_in_use,
            endpoint_p->remote_index);

        oam_info_p->remote_endpoints[endpoint_p->remote_index] =
            endpoint_index;
    }
    else
    {
        if (local_tx_enabled)
        {
            uint32 reversed_maid[BCM_OAM_GROUP_NAME_LENGTH / 4];
            int word_index;

            endpoint_p->local_tx_index =
                _bcm_en_oam_find_free_endpoint(oam_info_p->
                    local_tx_endpoints_in_use,
                oam_info_p->local_tx_endpoint_count, 1, 0);

            if (endpoint_p->local_tx_index < 0)
            {
                return BCM_E_FULL;
            }

            /* LMEP entry */

            sal_memset(&lmep_entry, 0, sizeof(lmep_entry_t));

            soc_LMEPm_field32_set(unit, &lmep_entry, MAID_INDEXf,
                endpoint_info->group);

            soc_LMEPm_mac_addr_set(unit, &lmep_entry, SAf,
                endpoint_info->src_mac_address);

            soc_LMEPm_field32_set(unit, &lmep_entry, MDLf, endpoint_info->level);
            soc_LMEPm_field32_set(unit, &lmep_entry, MEPIDf, endpoint_info->name);

            soc_LMEPm_field32_set(unit, &lmep_entry, PRIORITYf,
                endpoint_info->pkt_pri);

            soc_LMEPm_field32_set(unit, &lmep_entry, VLAN_IDf, endpoint_info->vlan);
            soc_LMEPm_field32_set(unit, &lmep_entry, CCMf, quantization_index);
            soc_LMEPm_field32_set(unit, &lmep_entry, DESTf, glp);

            soc_LMEPm_field32_set(unit, &lmep_entry, MH_OPCODEf,
                BCM_HG_OPCODE_UC);
            
            soc_LMEPm_field32_set(unit, &lmep_entry, INT_PRIf,
                endpoint_info->int_pri);

            /* Word-reverse the MAID bytes for the hardware */

            for (word_index = 0;
                word_index < (BCM_OAM_GROUP_NAME_LENGTH / 4);
                ++word_index)
            {
                reversed_maid[word_index] =
                    ((uint32 *) group_p->name)
                        [((BCM_OAM_GROUP_NAME_LENGTH / 4) - 1) - word_index];
            }

            soc_LMEPm_field_set(unit, &lmep_entry, MAIDf, reversed_maid);

            SOC_IF_ERROR_RETURN(WRITE_LMEPm(unit, MEM_BLOCK_ALL,
                endpoint_p->local_tx_index, &lmep_entry));

            /* LMEP_1 entry */

            sal_memset(&lmep1_entry, 0, sizeof(lmep_1_entry_t));

            soc_LMEP_1m_mac_addr_set(unit, &lmep1_entry, MACDAf,
                endpoint_info->dst_mac_address);

            SOC_IF_ERROR_RETURN(WRITE_LMEP_1m(unit, MEM_BLOCK_ALL,
                endpoint_p->local_tx_index, &lmep1_entry));
            
            /* Local control block */

            SHR_BITSET(oam_info_p->local_tx_endpoints_in_use,
                endpoint_p->local_tx_index);
        }

        if (local_rx_enabled)
        {
            endpoint_p->local_rx_index =
                _bcm_en_oam_find_free_endpoint(oam_info_p->
                    local_rx_endpoints_in_use,
                oam_info_p->local_rx_endpoint_count, 8, endpoint_info->level);

            if (endpoint_p->local_rx_index < 0)
            {
                return BCM_E_FULL;
            }

            /* MA_INDEX entry */

            sal_memset(&ma_index_entry, 0, sizeof(ma_index_entry));

            soc_MA_INDEXm_field32_set(unit, &ma_index_entry, MA_PTRf,
                endpoint_info->group);

            SOC_IF_ERROR_RETURN(WRITE_MA_INDEXm(unit, MEM_BLOCK_ALL,
                endpoint_p->local_rx_index, &ma_index_entry));

            /* L3 entry */

            _bcm_esw_l3_lock(unit);

            if (BCM_SUCCESS(_bcm_en_oam_find_lmep(unit, endpoint_info->vlan,
                glp, &l3_index, &l3_entry)) ||
                BCM_SUCCESS(_bcm_en_oam_find_lmep(unit, 0,
                vp, &l3_index, &l3_entry)))
            {
                /* There's already an entry for this vlan+glp or 0+vp */

                soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &level_bitmap,
                    LMEP__MDL_BITMAPf);

                level_bitmap |= (1 << endpoint_info->level);

                result = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm,
                    l3_index, LMEP__MDL_BITMAPf, level_bitmap);

                _bcm_esw_l3_unlock(unit);

                SOC_IF_ERROR_RETURN(result);
            }
            else
            {
                /* This is the first entry at this vlan+glp */

                sal_memset(&l3_entry, 0, sizeof(l3_entry_ipv4_unicast_entry_t));

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                    LMEP__MDL_BITMAPf, 1 << endpoint_info->level);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                    LMEP__MA_BASE_PTRf,
                    endpoint_p->local_rx_index >> LEVEL_BIT_COUNT);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                    LMEP__DM_ENABLEf, (endpoint_info->flags &
                        BCM_OAM_ENDPOINT_DELAY_MEASUREMENT) ? 1 : 0);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                    LMEP__CCM_ENABLEf, (endpoint_info->flags &
                        BCM_OAM_ENDPOINT_CCM_RX) ? 1 : 0);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                    LMEP__LB_ENABLEf, (endpoint_info->flags &
                    BCM_OAM_ENDPOINT_LOOPBACK) ? 1 : 0);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                    LMEP__LT_ENABLEf, (endpoint_info->flags &
                    BCM_OAM_ENDPOINT_LINKTRACE) ? 1 : 0);

                if (vp) {
                    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, LMEP__VIDf,
                       0);

                    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, LMEP__SGLPf,
                        vp);
                } else {
                    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, LMEP__VIDf,
                       endpoint_info->vlan);

                    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, LMEP__SGLPf,
                        glp);
                }
                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, KEY_TYPEf,
                    TR_L3_HASH_KEY_TYPE_LMEP);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, VALIDf, 1);

                SOC_IF_ERROR_RETURN(soc_mem_insert(unit, L3_ENTRY_IPV4_UNICASTm,
                    MEM_BLOCK_ALL, &l3_entry));

                _bcm_esw_l3_unlock(unit);
                
                /* Local control block */

                SHR_BITSET(oam_info_p->local_rx_endpoints_in_use,
                    endpoint_p->local_rx_index);
            }
            
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
                /* Assign LM counter index */
                endpoint_p->lm_counter_index =
                _bcm_en_oam_find_free_endpoint(oam_info_p->lm_counter_in_use,
                oam_info_p->lm_counter_count, 8, 0);

                if (endpoint_p->lm_counter_index < 0) {
                    return BCM_E_FULL;
                }
            
                SHR_BITSET(oam_info_p->lm_counter_in_use, 
                    endpoint_p->lm_counter_index);
                    
                /* Assign Pri Mapping pointer */
                for (i = 0; i < 8 ; i++) {
                    mem_entries[i] = endpoint_info->pri_map[i];
                }
                soc_mem_lock(unit, ING_SERVICE_PRI_MAPm);

                entries[0] = &mem_entries;
                SOC_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, ing_pri_map_profile[unit],
                                 (void *) &entries, 8, &temp_index));
                endpoint_p->pri_map_index = 
                    soc_mem_index_min(unit, ING_SERVICE_PRI_MAPm) + temp_index;
                soc_mem_unlock(unit, ING_SERVICE_PRI_MAPm);
            }
            
            if (endpoint_info->flags & (BCM_OAM_ENDPOINT_DELAY_MEASUREMENT |
                                        BCM_OAM_ENDPOINT_LOSS_MEASUREMENT)) {
                result = _bcm_en_oam_install_fp(unit, endpoint_index, 
                             endpoint_info);
                if (result < 0) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_en_oam_delete_lmep(unit, endpoint_p));
                    return result;
                }
            }
            
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_PBB_TE) {
                /* Install VFP */
    	        result = _bcm_en_oam_install_vfp(unit, endpoint_index, endpoint_info);
                if (result < 0) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_en_oam_delete_lmep(unit, endpoint_p));
                    return result;
                }
            }
        } /* local_rx */
    }
    /* Local control block */
    endpoint_p->in_use = 1;

    return BCM_E_NONE;
}

static int _bcm_en_oam_get_endpoint(int unit, int endpoint_index,
    _bcm_oam_endpoint_t *endpoint_p, bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_info_t *oam_info_p;
    rmep_entry_t rmep_entry;
    lmep_entry_t lmep_entry;
    l3_entry_ipv4_unicast_entry_t l3_key;
    l3_entry_ipv4_unicast_entry_t l3_entry;
    int l3_index;
    int quantization_index = 0;
    bcm_module_t module_id;
    bcm_port_t port_id;

    SET_OAM_INFO;

    endpoint_info->id = endpoint_index;
    endpoint_info->group = endpoint_p->group_index;
    endpoint_info->name = endpoint_p->name;
    endpoint_info->level = endpoint_p->level;
    endpoint_info->vlan = endpoint_p->vlan;

#if defined(INCLUDE_L3)
    if (endpoint_p->vp) {
        if (_bcm_vp_used_get(unit, endpoint_p->vp, _bcmVpTypeMim)) {
            BCM_GPORT_MIM_PORT_ID_SET(endpoint_info->gport, endpoint_p->vp);
        } else {
            BCM_GPORT_MPLS_PORT_ID_SET(endpoint_info->gport, endpoint_p->vp);
        }
    } else 
#endif /* INCLUDE_L3 */
    {
        if (endpoint_p->glp & (1 << 12))
        {
            /* Trunk */

            BCM_GPORT_TRUNK_SET(endpoint_info->gport, endpoint_p->glp & 0x7F);
        }
        else
        {
            module_id = (endpoint_p->glp & 0x0FC0) >> 6;
            port_id = (endpoint_p->glp & 0x3F);

            if (module_id != 0)
            {
                /* Modport */

                BCM_GPORT_MODPORT_SET(endpoint_info->gport, module_id,
                    port_id);
            }
            else
            {
                /* Local port */

                BCM_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, port_id,
                    &(endpoint_info->gport)));
            }
        }
    }

    if (endpoint_p->is_remote)
    {
        endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE;

        /* RMEP entry */

        SOC_IF_ERROR_RETURN(READ_RMEPm(unit, MEM_BLOCK_ANY,
            endpoint_p->remote_index, &rmep_entry));

        BCM_IF_ERROR_RETURN(_bcm_en_oam_read_clear_faults(unit,
            endpoint_index, en_endpoint_faults, RMEPm, (uint32 *) &rmep_entry,
            &endpoint_info->faults, &endpoint_info->persistent_faults,
            endpoint_info->clear_persistent_faults));

        /* L3 entry */

        _bcm_en_oam_make_rmep_key(unit, &l3_key, endpoint_p->name,
            endpoint_p->level, endpoint_p->vlan, endpoint_p->glp);

        if (BCM_FAILURE(soc_mem_search(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ANY,
            &l3_index, &l3_key, &l3_entry, 0)))
        {
            return BCM_E_INTERNAL;
        }

        quantization_index = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
            RMEP__CCMf);
    }
    else
    {
        /* Endpoint is local */

        if (endpoint_p->local_tx_enabled)
        {
            /* LMEP entry */

            SOC_IF_ERROR_RETURN(READ_LMEPm(unit, MEM_BLOCK_ANY,
                endpoint_p->local_tx_index, &lmep_entry));

            soc_LMEPm_mac_addr_get(unit, &lmep_entry, SAf,
                endpoint_info->src_mac_address);

            endpoint_info->pkt_pri = soc_LMEPm_field32_get(unit, &lmep_entry,
                PRIORITYf);

            endpoint_info->int_pri = soc_LMEPm_field32_get(unit, &lmep_entry,
                INT_PRIf);

            quantization_index = soc_LMEPm_field32_get(unit, &lmep_entry,
                CCMf);
        }

        if (endpoint_p->local_rx_enabled)
        {
            /* L3 entry */

            if (BCM_FAILURE(_bcm_en_oam_find_lmep(unit, endpoint_p->vlan,
                endpoint_p->glp, &l3_index, &l3_entry)))
            {
                return BCM_E_INTERNAL;
            }

            if (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                LMEP__DM_ENABLEf))
            {
                endpoint_info->flags |= BCM_OAM_ENDPOINT_DELAY_MEASUREMENT;
            }

            if (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                LMEP__CCM_ENABLEf))
            {
                endpoint_info->flags |= BCM_OAM_ENDPOINT_CCM_RX;
            }

            if (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                LMEP__LB_ENABLEf))
            {
                endpoint_info->flags |= BCM_OAM_ENDPOINT_LOOPBACK;
            }

            if (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                LMEP__LT_ENABLEf))
            {
                endpoint_info->flags |= BCM_OAM_ENDPOINT_LINKTRACE;
            }
        }
    }

    endpoint_info->ccm_period = en_ccm_periods[quantization_index];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_endpoint_get
 * Purpose:
 *      Get an OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_endpoint_get(
    int unit, 
    bcm_oam_endpoint_t endpoint, 
    bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_endpoint_t *endpoint_p;

    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_ENDPOINT_INDEX(endpoint);

    SET_ENDPOINT(endpoint);

    if (!endpoint_p->in_use)
    {
        return BCM_E_NOT_FOUND;
    }

    return _bcm_en_oam_get_endpoint(unit, endpoint, endpoint_p,
        endpoint_info);
}

/*
 * Function:
 *      bcm_en_oam_endpoint_destroy
 * Purpose:
 *      Destroy an OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the OAM endpoint object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_endpoint_destroy(
    int unit, 
    bcm_oam_endpoint_t endpoint)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_endpoint_t *endpoint_p;

    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_ENDPOINT_INDEX(endpoint);

    SET_ENDPOINT(endpoint);

    if (!endpoint_p->in_use)
    {
        return BCM_E_NOT_FOUND;
    }

    return _bcm_en_oam_destroy_endpoint(unit, endpoint_p);
}

/*
 * Function:
 *      bcm_en_oam_endpoint_destroy_all
 * Purpose:
 *      Destroy all OAM endpoint objects associated with a given OAM
 *      group
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The OAM group whose endpoints should be destroyed
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_endpoint_destroy_all(
    int unit, 
    bcm_oam_group_t group)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_endpoint_t *endpoint_p;
    int endpoint_index;

    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_GROUP_INDEX(group);

    for (endpoint_index = 0; endpoint_index < oam_info_p->endpoint_count;
        ++endpoint_index)
    {
        SET_ENDPOINT(endpoint_index);

        if (endpoint_p->in_use && endpoint_p->group_index == group)
        {
            BCM_IF_ERROR_RETURN(_bcm_en_oam_destroy_endpoint(unit,
                endpoint_p));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_endpoint_traverse
 * Purpose:
 *      Traverse the set of OAM endpoints associated with the
 *      specified group, calling a specified callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The OAM group whose endpoints should be traversed
 *      cb - (IN) A pointer to the callback function to call for each OAM endpoint in the specified group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_endpoint_traverse(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_endpoint_traverse_cb cb, 
    void *user_data)
{
    _bcm_oam_info_t *oam_info_p;
    _bcm_oam_endpoint_t *endpoint_p;
    int endpoint_index;
    bcm_oam_endpoint_info_t endpoint_info;

    SET_OAM_INFO;

    CHECK_INIT;

    VALIDATE_GROUP_INDEX(group);

    if (cb == NULL)
    {
        return BCM_E_PARAM;
    }

    for (endpoint_index = 0; endpoint_index < oam_info_p->endpoint_count;
        ++endpoint_index)
    {
        SET_ENDPOINT(endpoint_index);

        if (endpoint_p->in_use && endpoint_p->group_index == group)
        {
            bcm_oam_endpoint_info_t_init(&endpoint_info);
            BCM_IF_ERROR_RETURN(_bcm_en_oam_get_endpoint(unit,
                endpoint_index, endpoint_p, &endpoint_info));
            BCM_IF_ERROR_RETURN(cb(unit, &endpoint_info, user_data));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_event_register
 * Purpose:
 *      Register a callback for handling OAM events
 * Parameters:
 *      unit - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified callback should be called
 *      cb - (IN) A pointer to the callback function to call for the specified OAM events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_event_register(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb, 
    void *user_data)
{
    _bcm_oam_info_t *oam_info_p;
    uint32 result;
    _bcm_oam_event_handler_t *event_handler_p;
    _bcm_oam_event_handler_t *previous_p = NULL;
    bcm_oam_event_type_t event_type;
    uint32 interrupt_control_register_value;
    int update_interrupt_control = 0;

    SET_OAM_INFO;

    CHECK_INIT;

    if (cb == NULL)
    {
        return BCM_E_PARAM;
    }

    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, result);

    if (result == 0)
    {
        return BCM_E_PARAM;
    }

    for (event_handler_p = oam_info_p->event_handler_list_p;
        event_handler_p != NULL;
        event_handler_p = event_handler_p->next_p)
    {
        if (event_handler_p->cb == cb)
        {
            break;
        }

        previous_p = event_handler_p;
    }

    if (event_handler_p == NULL)
    {
        /* This handler hasn't been registered yet */

        event_handler_p = sal_alloc(sizeof(_bcm_oam_event_handler_t),
            "OAM event handler");

        if (event_handler_p == NULL)
        {
            return BCM_E_MEMORY;
        }

        event_handler_p->next_p = NULL;
        event_handler_p->cb = cb;

        SHR_BITCLR_RANGE(event_handler_p->event_types.w, 0, bcmOAMEventCount);

        if (previous_p != NULL)
        {
            previous_p->next_p = event_handler_p;
        }
        else
        {
            oam_info_p->event_handler_list_p = event_handler_p;
        }
    }

    SOC_IF_ERROR_RETURN(READ_CCM_INTERRUPT_CONTROLr(unit,
        &interrupt_control_register_value));

    for (event_type = 0; event_type < bcmOAMEventCount; ++event_type)
    {
        if (SHR_BITGET(event_types.w, event_type))
        {
            if (!SHR_BITGET(event_handler_p->event_types.w, event_type))
            {
                /* This handler isn't handling this event yet */

                SHR_BITSET(event_handler_p->event_types.w, event_type);

                ++(oam_info_p->event_handler_count[event_type]);

                if (oam_info_p->event_handler_count[event_type] == 1)
                {
                    /* This is the first handler for this event */

                    update_interrupt_control = 1;

                    soc_reg_field_set(unit, CCM_INTERRUPT_CONTROLr,
                        &interrupt_control_register_value,
                        en_interrupt_enable_fields[event_type], 1);
                }
            }
        }
    }

    event_handler_p->user_data = user_data;

    /* Enable any needed interrupts not yet enabled */

    if (update_interrupt_control)
    {
        SOC_IF_ERROR_RETURN(WRITE_CCM_INTERRUPT_CONTROLr(unit,
            interrupt_control_register_value));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_en_oam_event_unregister
 * Purpose:
 *      Unregister a callback for handling OAM events
 * Parameters:
 *      unit - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified callback should not be called
 *      cb - (IN) A pointer to the callback function to unregister from the specified OAM events
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_en_oam_event_unregister(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb)
{
     _bcm_oam_info_t *oam_info_p;
    uint32 result;
    _bcm_oam_event_handler_t *event_handler_p;
    _bcm_oam_event_handler_t *previous_p = NULL;
    bcm_oam_event_type_t event_type;
    uint32 interrupt_control_register_value;
    int update_interrupt_control = 0;

    SET_OAM_INFO;

    CHECK_INIT;

    if (cb == NULL)
    {
        return BCM_E_PARAM;
    }

    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, result);

    if (result == 0)
    {
        return BCM_E_PARAM;
    }

    for (event_handler_p = oam_info_p->event_handler_list_p;
        event_handler_p != NULL;
        event_handler_p = event_handler_p->next_p)
    {
        if (event_handler_p->cb == cb)
        {
            break;
        }

        previous_p = event_handler_p;
    }

    if (event_handler_p == NULL)
    {
        return BCM_E_NOT_FOUND;
    }

    SOC_IF_ERROR_RETURN(READ_CCM_INTERRUPT_CONTROLr(unit,
        &interrupt_control_register_value));

    for (event_type = 0; event_type < bcmOAMEventCount; ++event_type)
    {
        if (SHR_BITGET(event_types.w, event_type))
        {
            if (oam_info_p->event_handler_count[event_type] > 0 &&
                SHR_BITGET(event_handler_p->event_types.w, event_type))
            {
                /* This handler has been handling this event */

                SHR_BITCLR(event_handler_p->event_types.w, event_type);

                --(oam_info_p->event_handler_count[event_type]);

                if (oam_info_p->event_handler_count[event_type] == 0)
                {
                    /* No more handlers for this event */

                    update_interrupt_control = 1;

                    soc_reg_field_set(unit, CCM_INTERRUPT_CONTROLr,
                        &interrupt_control_register_value,
                        en_interrupt_enable_fields[event_type], 0);
                }
            }
        }
    }

    /* Disable any interrupts that lost their last handler */

    if (update_interrupt_control)
    {
        SOC_IF_ERROR_RETURN(WRITE_CCM_INTERRUPT_CONTROLr(unit,
            interrupt_control_register_value));
    }

    SHR_BITTEST_RANGE(event_handler_p->event_types.w, 0, bcmOAMEventCount,
        result);

    if (result == 0)
    {
        /* No more events for this handler to handle */

        if (previous_p != NULL)
        {
            previous_p->next_p = event_handler_p->next_p;
        }
        else
        {
            oam_info_p->event_handler_list_p = event_handler_p->next_p;
        }

        sal_free(event_handler_p);
    }

    return BCM_E_NONE;
}

#endif /* defined(BCM_ENDURO) */
