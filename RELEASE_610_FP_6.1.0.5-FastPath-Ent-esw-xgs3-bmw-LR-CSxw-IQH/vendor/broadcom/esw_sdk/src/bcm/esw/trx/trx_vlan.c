/*
 * $Id: trx_vlan.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:        vlan.c
 * Purpose:     Provide low-level access to Triumph VLAN resources
 */

#include <sal/core/boot.h>

#include <soc/defs.h>
#if defined(BCM_TRX_SUPPORT)
#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/stg.h>
#include <bcm/port.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/xgs3.h>

#include <bcm_int/esw_dispatch.h>

/* Cache of Ingress Vlan Translate Action Profile Table */
static soc_profile_mem_t *ing_action_profile[BCM_MAX_NUM_UNITS] = {NULL};
#define ING_ACTION_PROFILE_DEFAULT  0

/* Cache of Engress Vlan Translate Action Profile Table */
static soc_profile_mem_t *egr_action_profile[BCM_MAX_NUM_UNITS] = {NULL};
#define EGR_ACTION_PROFILE_DEFAULT  0

/* Cache of Ingress Vlan Range Profile Table */
static soc_profile_mem_t *vlan_range_profile[BCM_MAX_NUM_UNITS] = {NULL};
#define VLAN_RANGE_PROFILE_DEFAULT  0




STATIC int _bcm_trx_vlan_range_profile_init(int unit);

/*
 * Function : bcm_trx_vlan_action_profile_detach
 *
 * Purpose  : to initialize hardware ING_VLAN_TAG_ACTION_PROFILE and
 *            EGR_VLAN_TAG_ACTION_PROFILE tables and allocate memory
 *            to cache hardware tables in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
int
_bcm_trx_vlan_action_profile_detach(int unit)
{
    int rv;

    /* De-initialize the ING_VLAN_TAG_ACTION_PROFILE table */
    rv = soc_profile_mem_destroy(unit, ing_action_profile[unit]);
    BCM_IF_ERROR_RETURN(rv);

    sal_free(ing_action_profile[unit]);
    ing_action_profile[unit] = NULL;

    rv = soc_profile_mem_destroy(unit, egr_action_profile[unit]);
    BCM_IF_ERROR_RETURN(rv);

    sal_free(egr_action_profile[unit]);
    egr_action_profile[unit] = NULL;

    rv = soc_profile_mem_destroy(unit, vlan_range_profile[unit]);
    BCM_IF_ERROR_RETURN(rv);

    sal_free(vlan_range_profile[unit]);
    vlan_range_profile[unit] = NULL;    

    return BCM_E_NONE;
}

/*
 * Function : bcm_trx_vlan_action_profile_init
 *
 * Purpose  : to initialize hardware ING_VLAN_TAG_ACTION_PROFILE and
 *            EGR_VLAN_TAG_ACTION_PROFILE tables and allocate memory
 *            to cache hardware tables in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
int
_bcm_trx_vlan_action_profile_init(int unit)
{
    int i, idx, num_ports;
    port_tab_entry_t port_entry;
    vlan_protocol_data_entry_t proto_entry;
    vlan_subnet_entry_t subnet_entry;
    vlan_xlate_entry_t xlate_entry;
    egr_vlan_xlate_entry_t egr_xlate_entry;
    ing_vlan_tag_action_profile_entry_t ing_profile_entry;
    egr_vlan_tag_action_profile_entry_t egr_profile_entry;
    uint32 rval, temp_index;
    soc_mem_t mem;
    void *entries[1];

    /* Initialize the ING_VLAN_TAG_ACTION_PROFILE table */

    if (ing_action_profile[unit] == NULL) {
        ing_action_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Ing Action Profile Mem");
        if (ing_action_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(ing_action_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = ING_VLAN_TAG_ACTION_PROFILEm;
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, 1,
                                               ing_action_profile[unit]));

    if (SOC_WARM_BOOT(unit)) {
        /* Increment the ref count for all ports */
        for (i = 0; i < soc_mem_index_count(unit, PORT_TABm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_PORT_TABm(unit, MEM_BLOCK_ANY, i, &port_entry));
            idx = soc_mem_field32_get(unit, PORT_TABm,
                                      &port_entry, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit], idx, 1);
        }

        /* Increment the ref count for all Protocol-based VLANs */
        for (i = 0; i < soc_mem_index_count(unit, VLAN_PROTOCOL_DATAm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, i, &proto_entry));
            idx = soc_mem_field32_get(unit, VLAN_PROTOCOL_DATAm,
                                      &proto_entry, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit], idx, 1);
        }

        /* Increment the ref count for all Subnet-based VLANs */
        for (i = 0; i < soc_mem_index_count(unit, VLAN_SUBNETm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, &subnet_entry));
            if (!soc_mem_field32_get(unit, VLAN_SUBNETm, 
                                     &subnet_entry, VALIDf)) {
                continue;
            }
            idx = soc_mem_field32_get(unit, VLAN_SUBNETm,
                                      &subnet_entry, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit], idx, 1);
        }

        /* Increment the ref count for all Mac-based VLANs & vlan translations */
        for (i = 0; i < soc_mem_index_count(unit, VLAN_XLATEm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATEm(unit, MEM_BLOCK_ANY, i, &xlate_entry));
            if (!soc_mem_field32_get(unit, VLAN_XLATEm, &xlate_entry, VALIDf)) {
                continue;
            }
            idx = soc_mem_field32_get(unit, VLAN_XLATEm,
                                      &xlate_entry, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit], idx, 1);
        }

        /* One extra increment to preserve location ING_ACTION_PROFILE_DEFAULT */
        SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], 
                              ING_ACTION_PROFILE_DEFAULT, 1);

    } else {
        /* Initialize the ING_ACTION_PROFILE_DEFAULT. For untagged and
         * inner-tagged packets, always add an outer tag.
         */
        sal_memset(&ing_profile_entry, 0, sizeof(ing_vlan_tag_action_profile_entry_t));
        soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &ing_profile_entry,
                            UT_OTAG_ACTIONf, bcmVlanActionAdd);
        soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &ing_profile_entry,
                            SIT_OTAG_ACTIONf, bcmVlanActionAdd);
        soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &ing_profile_entry,
                            SIT_PITAG_ACTIONf, bcmVlanActionDelete);
        soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &ing_profile_entry,
                            SOT_POTAG_ACTIONf, bcmVlanActionReplace);
        soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &ing_profile_entry,
                            DT_POTAG_ACTIONf, bcmVlanActionReplace);
        entries[0] = &ing_profile_entry;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, ing_action_profile[unit],
                                 (void *) &entries, 1, &temp_index));

        /* Increment the ref count for all ports */
        SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], 
                                  ING_ACTION_PROFILE_DEFAULT,
                                  soc_mem_index_count(unit, PORT_TABm));

        /* Increment the ref count for all Protocol-based VLANs */
        SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], 
                                  ING_ACTION_PROFILE_DEFAULT,
                                  soc_mem_index_count(unit,
                                                      VLAN_PROTOCOL_DATAm));
    }

    /* Initialize the EGR_VLAN_TAG_ACTION_PROFILE table */
    if (egr_action_profile[unit] == NULL) {
        egr_action_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Egr Action Profile Mem");
        if (egr_action_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(egr_action_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = EGR_VLAN_TAG_ACTION_PROFILEm;
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, 1,
                                               egr_action_profile[unit]));

    SOC_PBMP_COUNT(PBMP_ALL(unit), num_ports);
    if (SOC_WARM_BOOT(unit)) {
        /* Increment the ref count for all ports */
        PBMP_ALL_ITER(unit, i) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_CONTROL_3r(unit, i, &rval));
            idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                    rval, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, egr_action_profile[unit], idx, 1);
        }

        /* Increment the ref count for all egress vlan translations */
        for (i = 0; i < soc_mem_index_count(unit, EGR_VLAN_XLATEm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATEm(unit, MEM_BLOCK_ANY, i, &egr_xlate_entry));
            if (!soc_mem_field32_get(unit, EGR_VLAN_XLATEm, &egr_xlate_entry, VALIDf)) {
                continue;
            }
            idx = soc_mem_field32_get(unit, EGR_VLAN_XLATEm,
                                      &egr_xlate_entry, TAG_ACTION_PROFILE_PTRf);
            SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, egr_action_profile[unit], idx, 1);
        }

        /* One extra increment to preserve location EGR_ACTION_PROFILE_DEFAULT */
        SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit], 
                              EGR_ACTION_PROFILE_DEFAULT, 1);
    } else {
        /* Initialize the EGR_ACTION_PROFILE_DEFAULT to have all
         * actions as NOP (0).
         */
        sal_memset(&egr_profile_entry, 0, sizeof(egr_vlan_tag_action_profile_entry_t));
        entries[0] = &egr_profile_entry;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, egr_action_profile[unit],
                                 (void *) &entries, 1, &temp_index));

        /* Increment the ref count for all ports */
        SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit], 
                                  EGR_ACTION_PROFILE_DEFAULT, num_ports);
    }

    /* Initialize the VLAN range profile table */
    _bcm_trx_vlan_range_profile_init(unit);

    return BCM_E_NONE;
}

/*
 * Function : _bcm_trx_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to vlan action profile table
 *
 */
int
_bcm_trx_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index)
{   
    ing_vlan_tag_action_profile_entry_t entry;
    void *entries[1];

    sal_memset(&entry, 0, sizeof(ing_vlan_tag_action_profile_entry_t));

    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_OTAG_ACTIONf, action->dt_outer);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_POTAG_ACTIONf, action->dt_outer_prio);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_ITAG_ACTIONf, action->dt_inner);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_PITAG_ACTIONf, action->dt_inner_prio);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_OTAG_ACTIONf, action->ot_outer);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_POTAG_ACTIONf, action->ot_outer_prio);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_ITAG_ACTIONf, action->ot_inner);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SIT_OTAG_ACTIONf, action->it_outer);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SIT_ITAG_ACTIONf, action->it_inner);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SIT_PITAG_ACTIONf, action->it_inner_prio);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        UT_OTAG_ACTIONf, action->ut_outer);
    soc_mem_field32_set(unit, ING_VLAN_TAG_ACTION_PROFILEm, &entry,
                        UT_ITAG_ACTIONf, action->ut_inner);

    entries[0] = &entry;
    return (soc_profile_mem_add(unit, ing_action_profile[unit],
                                (void *) &entries, 1, index));
}

/*
 * Function : _bcm_trx_vlan_action_profile_entry_increment
 *
 * Purpose  : increment the refcount for a vlan action profile table entry
 *
 */
void
_bcm_trx_vlan_action_profile_entry_increment(int unit, uint32 index)
{   
    SOC_PROFILE_MEM_REFERENCE(unit, ing_action_profile[unit], index, 1);
}

/*
 * Function : _bcm_trx_vlan_action_profile_entry_get
 *
 * Purpose  : get a copy of cached vlan action profile table
 *
 */
void
_bcm_trx_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action, 
                                      uint32 index)
{
    ing_vlan_tag_action_profile_entry_t *entry;

    entry = SOC_PROFILE_MEM_ENTRY(unit, ing_action_profile[unit],
                                  ing_vlan_tag_action_profile_entry_t *,
                                  index);

    action->dt_outer = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, DT_OTAG_ACTIONf);
    action->dt_outer_prio = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                                entry, DT_POTAG_ACTIONf);
    action->dt_inner = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, DT_ITAG_ACTIONf);
    action->dt_inner_prio = 
        soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                            entry, DT_PITAG_ACTIONf);
    action->ot_outer = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, SOT_OTAG_ACTIONf);
    action->ot_outer_prio = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                                entry, SOT_POTAG_ACTIONf);
    action->ot_inner = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, SOT_ITAG_ACTIONf);
    action->it_outer = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, SIT_OTAG_ACTIONf);
    action->it_inner = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, SIT_ITAG_ACTIONf);
    action->it_inner_prio = 
        soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                            entry, SIT_PITAG_ACTIONf);
    action->ut_inner = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, UT_ITAG_ACTIONf);
    action->ut_outer = soc_mem_field32_get(unit, ING_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, UT_OTAG_ACTIONf);
}

/*
 * Function : _bcm_trx_vlan_action_profile_entry_delete
 *
 * Purpose  : remove an entry from vlan action profile table
 *
 */
int
_bcm_trx_vlan_action_profile_entry_delete(int unit, uint32 index)
{
    return soc_profile_mem_delete(unit, ing_action_profile[unit], index);
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to egress vlan action profile table
 *
 */
int
_bcm_trx_egr_vlan_action_profile_entry_add(int unit,
                                          bcm_vlan_action_set_t *action,
                                          uint32 *index)
{   
    egr_vlan_tag_action_profile_entry_t entry;
    void *entries[1];

    sal_memset(&entry, 0, sizeof(egr_vlan_tag_action_profile_entry_t));

    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_OTAG_ACTIONf, action->dt_outer);
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_ITAG_ACTIONf, action->dt_inner);
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_PITAG_ACTIONf, action->dt_inner_prio);
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_OTAG_ACTIONf, action->ot_outer);
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_ITAG_ACTIONf, action->ot_inner);

    entries[0] = &entry;
    return (soc_profile_mem_add(unit, egr_action_profile[unit],
                                (void *) &entries, 1, index));
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_increment
 *
 * Purpose  : increment the refcount for an egress vlan action profile table entry
 *
 */
void
_bcm_trx_egr_vlan_action_profile_entry_increment(int unit, uint32 index)
{   
    SOC_PROFILE_MEM_REFERENCE(unit, egr_action_profile[unit], index, 1);
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_get
 *
 * Purpose  : get a copy of cached egress vlan action profile table
 *
 */
void
_bcm_trx_egr_vlan_action_profile_entry_get(int unit,
                                          bcm_vlan_action_set_t *action, 
                                          uint32 index)
{
    egr_vlan_tag_action_profile_entry_t *entry;

    entry = SOC_PROFILE_MEM_ENTRY(unit, egr_action_profile[unit],
                                  egr_vlan_tag_action_profile_entry_t *,
                                  index);

    action->dt_outer = soc_mem_field32_get(unit, EGR_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, DT_OTAG_ACTIONf);
    action->dt_inner = soc_mem_field32_get(unit, EGR_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, DT_ITAG_ACTIONf);
    action->dt_inner_prio = 
        soc_mem_field32_get(unit, EGR_VLAN_TAG_ACTION_PROFILEm, 
                            entry, DT_PITAG_ACTIONf);
    action->ot_outer = soc_mem_field32_get(unit, EGR_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, SOT_OTAG_ACTIONf);
    action->ot_inner = soc_mem_field32_get(unit, EGR_VLAN_TAG_ACTION_PROFILEm, 
                                           entry, SOT_ITAG_ACTIONf);
    action->it_inner = bcmVlanActionNone;
    action->it_inner_prio = bcmVlanActionNone;
    action->ut_inner = bcmVlanActionNone;
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_delete
 *
 * Purpose  : remove an entry from egress vlan action profile table
 *
 */
int
_bcm_trx_egr_vlan_action_profile_entry_delete(int unit, uint32 index)
{
    return soc_profile_mem_delete(unit, egr_action_profile[unit], index);
}

STATIC soc_field_t _tr_range_min_f[] = {VLAN_MIN_0f, VLAN_MIN_1f, 
                                        VLAN_MIN_2f, VLAN_MIN_3f,
                                        VLAN_MIN_4f, VLAN_MIN_5f, 
                                        VLAN_MIN_6f, VLAN_MIN_7f};
STATIC soc_field_t _tr_range_max_f[] = {VLAN_MAX_0f, VLAN_MAX_1f, 
                                        VLAN_MAX_2f, VLAN_MAX_3f,
                                        VLAN_MAX_4f, VLAN_MAX_5f, 
                                        VLAN_MAX_6f, VLAN_MAX_7f}; 

/*
 * Function : bcm_trx_vlan_range_profile_init
 *
 * Purpose  : to initialize hardware ING_VLAN_RANGE table
 *            allocate memory to cache hardware tables in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
STATIC int
_bcm_trx_vlan_range_profile_init(int unit)
{
    int i, idx;
    source_trunk_map_table_entry_t stm_entry;
    ing_vlan_range_entry_t profile_entry;
    uint32 temp_index;
    soc_mem_t mem;
    void *entries[1];

    /* Initialize the ING_VLAN_RANGE table */

    if (vlan_range_profile[unit] == NULL) {
        vlan_range_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Vlan Range Profile Mem");
        if (vlan_range_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(vlan_range_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = ING_VLAN_RANGEm;
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, 1,
                                               vlan_range_profile[unit]));

    if (SOC_WARM_BOOT(unit)) {
        /* Increment the ref count for all ports */
        for (i = 0; i < soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, i, &stm_entry));
            idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                      &stm_entry, VLAN_RANGE_IDXf);
            SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], idx, 1);
        }

    } else {
        sal_memset(&profile_entry, 0, sizeof(ing_vlan_range_entry_t));
        for (i = 0; i < 8; i++) {
            soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                                &profile_entry, _tr_range_min_f[i], 1);
            soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                                &profile_entry, _tr_range_max_f[i], 0);
        }

        /* Initialize the VLAN_RANGE_PROFILE_DEFAULT to have 
         * all ranges unused (min == 1, max == 0).
         */
        entries[0] = &profile_entry;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, vlan_range_profile[unit],
                                 (void *) &entries, 1, &temp_index));

        /* Increment the ref count for all ports */
        for (i = 0; i < soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm); i++) {
            soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, i, 
                                   VLAN_RANGE_IDXf, temp_index);
            SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], temp_index, 1);
        }
    }

    /* One extra increment to preserve location VLAN_RANGE_PROFILE_DEFAULT */
    SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], 
                              VLAN_RANGE_PROFILE_DEFAULT, 1);
    return BCM_E_NONE;
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_add
 *
 * Purpose  : add a new entry to vlan range profile table
 *
 */
int
_bcm_trx_vlan_range_profile_entry_add(int unit, bcm_vlan_t *min_vlan,
                                     bcm_vlan_t *max_vlan, uint32 *index)
{   
    ing_vlan_range_entry_t profile_entry;
    void *entries[1];
    int i;

    sal_memset(&profile_entry, 0, sizeof(ing_vlan_range_entry_t));

    for (i = 0; i < 8; i++) {
        soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                            &profile_entry, _tr_range_min_f[i], min_vlan[i]);
        soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                            &profile_entry, _tr_range_max_f[i], max_vlan[i]);
    }
    entries[0] = &profile_entry;
    return (soc_profile_mem_add(unit, vlan_range_profile[unit],
                                (void *) &entries, 1, index));
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_increment
 *
 * Purpose  : increment the refcount for a vlan range profile table entry
 *
 */
void
_bcm_trx_vlan_range_profile_entry_increment(int unit, uint32 index)
{   
    SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], index, 1);
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_get
 *
 * Purpose  : get a copy of cached vlan range profile table
 *
 */
void
_bcm_trx_vlan_range_profile_entry_get(int unit, bcm_vlan_t *min_vlan, 
                                     bcm_vlan_t *max_vlan, uint32 index)
{
    int i;
    ing_vlan_range_entry_t *profile_entry;

    profile_entry = SOC_PROFILE_MEM_ENTRY(unit, vlan_range_profile[unit],
                                          ing_vlan_range_entry_t *,
                                          index);

    for (i = 0; i < 8; i++) {
        min_vlan[i] = soc_mem_field32_get(unit, ING_VLAN_RANGEm,
                                          profile_entry, _tr_range_min_f[i]);
        max_vlan[i] = soc_mem_field32_get(unit, ING_VLAN_RANGEm,
                                          profile_entry, _tr_range_max_f[i]);
    }
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_delete
 *
 * Purpose  : remove an entry from vlan range profile table
 *
 */
int
_bcm_trx_vlan_range_profile_entry_delete(int unit, uint32 index)
{
    return soc_profile_mem_delete(unit, vlan_range_profile[unit], index);
}

/*
 * On XGS3, VLAN_SUBNET.IP_ADDR stores only a top half (64 bits) of
 * the IPV6 address. Comparing it against a full length of a parameter
 * will have certainly result in a mismatch. There should be a
 * special type for a stored part of the address; meanwhile, a
 * symbolic constant is used.
 */

#define VLAN_SUBNET_IP_ADDR_LENGTH 8

/*
 * Function:
 *      _trx_vlan_ip_addr_mask_get
 * Purpose:
 *      Get IPV6 address and mask from vlan_ip structure
 * Parameters:
 *      vlan_ip -   structure specifying IP address and other info
 *      ip6addr -   IPv6 address to retrieve
 *      ip6mask -   IPv6 mask to retrieve
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_trx_vlan_ip_addr_mask_get(bcm_vlan_ip_t *vlan_ip, bcm_ip6_t ip6addr, 
                           bcm_ip6_t ip6mask)
{
    if (vlan_ip->flags & BCM_VLAN_SUBNET_IP6) {
        if (vlan_ip->prefix > 64) {
            return BCM_E_PARAM;
        }

        bcm_ip6_mask_create(ip6mask, vlan_ip->prefix); 
        sal_memcpy(ip6addr, vlan_ip->ip6, sizeof(bcm_ip6_t));
    } else { /* IPv4 entry */
        sal_memcpy(ip6addr, "\xff\xff\x00\x00", 4);
        sal_memcpy((ip6addr + 4), &(vlan_ip->ip4), sizeof(bcm_ip_t));

        sal_memcpy(ip6mask, "\xff\xff\xff\xff", 4);
        sal_memcpy((ip6mask + 4), &(vlan_ip->mask), sizeof(bcm_ip_t));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _tr_vlan_subnet_mem_read
 * Purpose:
 *      Helper function to allocated memory and read by range all 
 *      VLAN_SUBNET memory
 * Parameters:
 *      unit    -       (IN) BCM Device unit
 *      vstab   -       (OUT) pointer to memory
 *      nent    -       (OUT) total number of entries in the memory
 * Returns:
 *      BCM_E_XXX
 * Note: 
 *      This routine will allocate a memory, it is up to caller responsibility
 *      to free it 
 */
STATIC int 
_tr_vlan_subnet_mem_read(int unit, vlan_subnet_entry_t **vstab, int *nent)
{
    int imin, imax, vsbytes, rv;

    imin = soc_mem_index_min(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);
    *nent = soc_mem_index_count(unit, VLAN_SUBNETm);
    vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
    vsbytes = WORDS2BYTES(vsbytes);
    *vstab = soc_cm_salloc(unit, *nent * sizeof(vlan_subnet_entry_t), "vlan_subnet");
    if (*vstab == NULL) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                            imin, imax, *vstab);
    return rv;
}

/*
 * Function:
 *      _trx_vlan_subnet_entry_parse
 * Purpose:
 *      Helper function to parse all information fields from 
 *      VLAN_SUBNET memory entry
 * Parameters:
 *      unit    -       (IN) BCM Device unit
 *      ventry   -      (IN) pointer to a vlan subnet entry 
 *      subnet_fields - (OUT) Structure contains all entry fields
 * Returns:
 *      none
 * Note: 
 *      
 */
STATIC void 
_trx_vlan_subnet_entry_parse(int unit, vlan_subnet_entry_t *ventry, 
                           _bcm_vlan_subnet_entry_t *subnet_fields)
{
    bcm_ip6_t   ip6tmp;

    soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, ventry, IP_ADDRf, 
                         subnet_fields->ip, SOC_MEM_IP6_UPPER_ONLY);

    soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, ventry, MASKf, 
                         ip6tmp, SOC_MEM_IP6_UPPER_ONLY);
    subnet_fields->prefix = bcm_ip6_mask_length(ip6tmp);

    subnet_fields->ovid = soc_VLAN_SUBNETm_field32_get(unit, ventry, OVIDf);
    
    subnet_fields->ivid = soc_VLAN_SUBNETm_field32_get(unit, ventry, IVIDf);
    subnet_fields->vid = soc_VLAN_SUBNETm_field32_get(unit, ventry, VLAN_IDf);
    subnet_fields->prio = soc_VLAN_SUBNETm_field32_get(unit, ventry, PRIf);
    subnet_fields->profile_idx = soc_VLAN_SUBNETm_field32_get(unit, ventry, 
                                                    TAG_ACTION_PROFILE_PTRf);
}

/*
 * Function:
 *      _trx_vlan_subnet_entry_set
 * Purpose:
 *      Helper function to write all fields to 
 *      VLAN_SUBNET memory entry
 * Parameters:
 *      unit    -       (IN) BCM Device unit
 *      ventry   -      (IN) pointer to a vlan subnet entry 
 *      subnet_fields - (IN) Structure contains all entry fields
 * Returns:
 *      none
 * Note: 
 *      This routine assumes that all rovided fields has correct values
 */

STATIC void 
_trx_vlan_subnet_entry_set(int unit, vlan_subnet_entry_t *ventry, 
                           _bcm_vlan_subnet_entry_t *subnet_fields)
{
    soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, ventry, IP_ADDRf, 
                         subnet_fields->ip, SOC_MEM_IP6_UPPER_ONLY);
    soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, ventry, MASKf, 
                         subnet_fields->mask, SOC_MEM_IP6_UPPER_ONLY);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, OVIDf, subnet_fields->ovid);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, IVIDf, subnet_fields->ivid);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, VLAN_IDf, subnet_fields->vid);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, PRIf, subnet_fields->prio);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, TAG_ACTION_PROFILE_PTRf, 
                                 subnet_fields->profile_idx);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, VALIDf, 1);
}

/*
 * Function:
 *      _trx_vlan_subnet_match_find
 * Purpose:
 *      Get index into table with settings matching to subnet
 * Parameters:
 *      unit -          (IN) BCM Device unit
 *      vstab   -       (IN) pointer to memory
 *      nent    -       (IN) number of entries in the memory
 *      ip -            (IN) IP address
 *      mask -          (IN) IP mask
 *      match_idx -     (OUT) index to the table entry if match, -1 otherwise
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_vlan_subnet_match_find(int unit, vlan_subnet_entry_t *vstab, int nent,
                           bcm_ip6_t ip, bcm_ip6_t mask, int *match_idx)
{
    int                 i;
    bcm_ip6_t           ip6tmp;
    vlan_subnet_entry_t *vstabp;

    *match_idx = -1;
    for(i = 0; i < nent; i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);

        if (0 == soc_VLAN_SUBNETm_field32_get(unit, vstabp, VALIDf)) {
            continue;
        }
        soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, vstabp, MASKf, ip6tmp, 
                             SOC_MEM_IP6_UPPER_ONLY);
        if (0 != sal_memcmp(ip6tmp, mask, VLAN_SUBNET_IP_ADDR_LENGTH)) {
            continue;
        }
        soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, vstabp, IP_ADDRf, ip6tmp, 
                             SOC_MEM_IP6_UPPER_ONLY);
        if (0 == sal_memcmp(ip6tmp, ip, VLAN_SUBNET_IP_ADDR_LENGTH)) {
            *match_idx = i;
            break;
        }
    }

    if (*match_idx < 0) {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _trx_vlan_subnet_empty_find
 * Purpose:
 *      Get index into first empty entry of the table
 * Parameters:
 *      unit -          (IN) BCM Device unit
 *      vstab   -       (IN) pointer to memory
 *      nent    -       (IN) number of entries in the memory
 *      empty   -       (OUT) index to the first empty entry, -1 if memory full
 * Returns:
 *      BCM_E_XXX 
 */
STATIC int 
_trx_vlan_subnet_empty_find(int unit, vlan_subnet_entry_t *vstab, int nent, 
                           int *empty) 
{
    int                 i;
    vlan_subnet_entry_t *vstabp, *vsnull;

    *empty = -1;
    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);
    
    for(i = 0; i < nent; i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);
        if (sal_memcmp(vstabp, vsnull,
                       soc_mem_entry_bytes(unit, VLAN_SUBNETm)) == 0) {
            *empty = i;
            break;
        }
    }
    if (*empty < 0) {
        return BCM_E_FULL;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _trx_vlan_subnet_insert_find
 * Purpose:
 *      Get index where the given entry should be added
 * Parameters:
 *      unit -          (IN) BCM Device unit
 *      vstab   -       (IN) pointer to memory
 *      nent    -       (IN) number of entries in the memory
 *      ip -            (IN) IP address
 *      mask -          (IN) IP mask
 *      idx -           (OUT) index to the table entry,-1 if not found
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_vlan_subnet_insert_find(int unit, vlan_subnet_entry_t *vstab, int nent,
                            bcm_ip6_t ip, bcm_ip6_t mask, int *idx)
{
    int                 rv, i, prefix, epx, insert = -1;
    vlan_subnet_entry_t *vstabp;
    bcm_ip6_t           ip6tmp;

    prefix = bcm_ip6_mask_length(mask);
    if (prefix < 0) {
        return BCM_E_PARAM;
    }

    rv = BCM_E_NONE;

    for(i = 0; i < nent; i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);

        if (0 == soc_VLAN_SUBNETm_field32_get(unit, vstabp, VALIDf)) {
            continue;
        }
        soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, vstabp, MASKf, ip6tmp, 
                             SOC_MEM_IP6_UPPER_ONLY);
        epx = bcm_ip6_mask_length(ip6tmp);
        if (epx < 0) {
            rv = BCM_E_PARAM;
            break;
        }
        if (epx < prefix) {
            insert = i;
            break;
        }
    }                          
    *idx = insert;

    return (rv);     
}

/*
 * Function:
 *      _trx_vlan_subnet_entry_add
 * Purpose:
 *      Add vlan settings for given subnet
 * Parameters:
 *      unit            - (IN) BCM Device unit
 *      entry_fields    - (IN) Entry fields to add
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_vlan_subnet_entry_add(int unit, _bcm_vlan_subnet_entry_t *entry_fields)
{
    int                         match = -1, empty = -1, insert = -1;
    int                         rv, nent, i;
    vlan_subnet_entry_t         *vstab, *vstabp, *vsnull;
    uint32                      old_profile_idx;
    
    

    VLAN_CHK_ID(unit, entry_fields->ovid);
    VLAN_CHK_ID(unit, entry_fields->ivid);
    VLAN_CHK_PRIO(unit, entry_fields->prio);

    soc_mem_lock(unit, VLAN_SUBNETm);

    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }
    /* Ignore error code in purpose, match may not be exist */
    _trx_vlan_subnet_match_find(unit, vstab, nent, entry_fields->ip, 
                               entry_fields->mask, &match);
    rv = _trx_vlan_subnet_empty_find(unit, vstab, nent, &empty);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vstab);
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return (rv);
    }
    rv = _trx_vlan_subnet_insert_find(unit, vstab, nent, entry_fields->ip, 
                                     entry_fields->mask, &insert);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vstab);
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return (rv);
    }
    if (insert < 0) {
        insert = empty;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);
    
    if (match >= 0) {   /* found an exact match */
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                            vlan_subnet_entry_t *, vstab, match);

        /* retrieve old vlan action profile pointer */
        old_profile_idx = soc_VLAN_SUBNETm_field32_get(unit, vstabp,
                                                       TAG_ACTION_PROFILE_PTRf);   
        _trx_vlan_subnet_entry_set(unit, vstabp, entry_fields);

        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, match, vstabp);
        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    } else {
        /* Get the vlan action profile pointer from emtpy slot.
         * Should just be ING_ACTION_PROFILE_DEFAULT.
         */
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, empty);
        old_profile_idx = soc_VLAN_SUBNETm_field32_get(unit, vstabp,
                                                       TAG_ACTION_PROFILE_PTRf);       

        for (i = empty; i > insert; i--) {
            vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                            vlan_subnet_entry_t *, vstab, (i - 1));
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
            if (BCM_FAILURE(rv)) {
                break;
            }
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i - 1, &vsnull);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, insert);
        sal_memset(vstabp, 0, 
                   WORDS2BYTES(soc_mem_entry_words(unit, VLAN_SUBNETm)));

        _trx_vlan_subnet_entry_set(unit, vstabp, entry_fields);
        
        if (BCM_SUCCESS(rv)) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL,
                    insert, vstabp);
            if (BCM_SUCCESS(rv)) {
                /* Delete the old vlan action profile entry for the empty */
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit, 
                                                         old_profile_idx);
            }
        }
    }
    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}



/*
 * Function:
 *      _trx_vlan_subnet_entry_get
 * Purpose:
 *      Get vlan settings for given subnet
 * Parameters:
 *      unit -          (IN) BCM Device unit
 *      subnet_fields - (IN/OUT) Structure contains all entry fields
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_vlan_subnet_entry_get(int unit, _bcm_vlan_subnet_entry_t *entry_fields)
{
    vlan_subnet_entry_t         *vstab, *vstabp;
    int                         match_idx, rv, nent;
        ;

    if (NULL == entry_fields) {
        return (BCM_E_PARAM);
    }
    soc_mem_lock(unit, VLAN_SUBNETm);

    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }
    
    rv = _trx_vlan_subnet_match_find(unit, vstab, nent, entry_fields->ip, 
                                    entry_fields->mask, &match_idx);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vstab);
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return (rv);
    }
    /* if found get info */
    vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm, 
                                      vlan_subnet_entry_t *, vstab, match_idx);
    _trx_vlan_subnet_entry_parse(unit, vstabp, entry_fields);
    soc_cm_sfree(unit, vstab);
    soc_mem_unlock(unit, VLAN_SUBNETm);
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _trx_vlan_subnet_entry_delete
 * Purpose:
 *      Delete vlan settings for given subnet
 * Parameters:
 *      unit -        (IN) BCM Device unit
 *      ip -          (IN) IP address
 *      mask -        (IN) IP mask
 * Returns:
 *      BCM_E_XXX
 */
static int
_trx_vlan_subnet_entry_delete(int unit, bcm_ip6_t ip, bcm_ip6_t mask)
{
    int match = -1, empty = -1, rv, i, nent;
    uint32 old_profile_idx;
    vlan_subnet_entry_t *vstab, *vstabp, *vsnull;

    soc_mem_lock(unit, VLAN_SUBNETm);
    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }

    rv = _trx_vlan_subnet_match_find(unit, vstab, nent, ip, mask, &match);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vstab);
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return (rv);
    }

    rv = _trx_vlan_subnet_empty_find(unit, vstab, nent, &empty);
    if (empty < 0) {
        empty = nent;
    }

    /* Get the old vlan action profile pointer */
    vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                    vlan_subnet_entry_t *, vstab, match);
    old_profile_idx = soc_VLAN_SUBNETm_field32_get(unit, vstabp,
                                                   TAG_ACTION_PROFILE_PTRf);
    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    for (i = match; (i < empty - 1) && (rv == BCM_E_NONE); i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, (i + 1));
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
    }

    if (rv >= 0) {
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, empty - 1, vsnull);

        /* Increment the vlan action profile refcount for the empty entry */
        _bcm_trx_vlan_action_profile_entry_increment(unit, ING_ACTION_PROFILE_DEFAULT);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_ip_action_add
 * Purpose:
 *      Add a subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit    -   device number
 *      vlan_ip -   structure specifying IP address and other info
 *      action  -   structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
    int rv;
    _bcm_vlan_subnet_entry_t entry_fields;

    sal_memset(&entry_fields, 0, sizeof(_bcm_vlan_subnet_entry_t));
    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_action_profile_entry_add(unit, action, 
                                               (uint32 *)&entry_fields.profile_idx));

    BCM_IF_ERROR_RETURN(
        _trx_vlan_ip_addr_mask_get(vlan_ip, entry_fields.ip, entry_fields.mask));

    entry_fields.ovid = action->new_outer_vlan;
    entry_fields.ivid = action->new_inner_vlan;
    entry_fields.prio = action->priority;

    rv = _trx_vlan_subnet_entry_add(unit, &entry_fields);
    if (BCM_FAILURE(rv)) {
        /* Add failed, back out the new profile entry */
        (void)_bcm_trx_vlan_action_profile_entry_delete(unit, 
                                                     entry_fields.profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_ip_action_get
 * Purpose:
 *      Helper funtion for the API, implementation for TRX
 * Parameters:
 *      unit    -   (IN) device number
 *      vlan_ip -   (IN) structure specifying IP address and other info
 *      action  -   (OUT) structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_action_get(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
    _bcm_vlan_subnet_entry_t    entry_fields;

    /* Parameter check */
    if (NULL == vlan_ip || NULL == action) {
        return (BCM_E_PARAM);
    }
    sal_memset(&entry_fields, 0, sizeof(_bcm_vlan_subnet_entry_t));
    BCM_IF_ERROR_RETURN(
        _trx_vlan_ip_addr_mask_get(vlan_ip, entry_fields.ip, entry_fields.mask));  
    
    BCM_IF_ERROR_RETURN(
        _trx_vlan_subnet_entry_get(unit, &entry_fields));

    action->new_outer_vlan = entry_fields.ovid; 
    action->new_inner_vlan = entry_fields.ivid;
    action->priority = entry_fields.prio;

    _bcm_trx_vlan_action_profile_entry_get(unit, action, 
                                           entry_fields.profile_idx);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_ip_action_traverse
 * Purpose:
 *      Helper funtion for the API, implementation for TRX
 * Parameters:
 *      unit    -   (IN) device number
 *      cb      -   (IN) user specified call back function
 *      user_data - (IN) pointer to user_data
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_action_traverse(int unit, 
                                 bcm_vlan_ip_action_traverse_cb cb,
                                 void * user_data)
{
    vlan_subnet_entry_t         *vstab, *vstabp;
    int                         nent, rv, i;
    bcm_vlan_action_set_t       action;
    bcm_vlan_ip_t               vlan_ip;
    _bcm_vlan_subnet_entry_t    entry_fields;

    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }
    sal_memset(&entry_fields, 0, sizeof(_bcm_vlan_subnet_entry_t));

    
    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        return rv;
    }
    
    for(i = 0; i < nent; i++) {
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        sal_memset(&vlan_ip, 0, sizeof(bcm_vlan_ip_t));
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *, 
                                              vstab, i);
        if (0 == soc_VLAN_SUBNETm_field32_get(unit, vstabp, VALIDf)){
            continue;
        }
        _trx_vlan_subnet_entry_parse(unit, vstabp, &entry_fields);

        if (*(uint32*)entry_fields.ip == 0xffff0000) {
            /* IPv4 case */
            bcm_ip6_t   ip6tmp;

            vlan_ip.ip4 = *(uint32*)(entry_fields.ip + 4);
            bcm_ip6_mask_create(ip6tmp, entry_fields.prefix);
            vlan_ip.mask = *(uint32*)(ip6tmp + 4);
        } else {
            /* IPv6 case */
            sal_memcpy(vlan_ip.ip6, entry_fields.ip, sizeof(bcm_ip6_t));
            vlan_ip.prefix = entry_fields.prefix;
            vlan_ip.flags |= BCM_VLAN_SUBNET_IP6;
        }

        vlan_ip.prefix = entry_fields.prefix;
        action.new_outer_vlan = entry_fields.ovid;
        action.new_inner_vlan = entry_fields.ivid;
        vlan_ip.vid = entry_fields.vid;
        action.priority = entry_fields.prio; 
         
         /* Read action profile data. */
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, 
                                               entry_fields.profile_idx);
        /* Call traverse callback with the data. */
        rv = cb(unit, &vlan_ip, &action, user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, vstab);
            return rv; 
        }
    }

    soc_cm_sfree(unit, vstab);
    return rv;

}

/*
 * Function:
 *      bcm_vlan_ip_delete
 * Purpose:
 *      Delete a subnet lookup entry.
 * Parameters:
 *      unit -          device number
 *      vlan_ip -       structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_delete(int unit, bcm_vlan_ip_t *vlan_ip)
{
    bcm_ip6_t   ip6addr, ip6mask;

    if (NULL == vlan_ip) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(
        _trx_vlan_ip_addr_mask_get(vlan_ip, ip6addr, ip6mask));
    return _trx_vlan_subnet_entry_delete(unit, ip6addr, ip6mask); 
}

/*
 * Function:
 *      bcm_vlan_ip_delete_all
 * Purpose:
 *      Delete all subnet lookup entries.
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_delete_all(int unit)
{
    int i, nent,  rv;
    uint32 old_profile_idx;
    vlan_subnet_entry_t *vstab, *vsnull,  *vstabp;

    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        return rv;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    for(i = 0; i < nent; i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);

        if (!soc_VLAN_SUBNETm_field32_get(unit, vstabp, VALIDf)) {
            continue;
        }
        old_profile_idx = soc_VLAN_SUBNETm_field32_get(unit, vstabp,
                                                       TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vsnull);

        /* Increment the vlan action profile refcount for the cleared entry */
        _bcm_trx_vlan_action_profile_entry_increment(unit, ING_ACTION_PROFILE_DEFAULT);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

/*
 * Function:
 *      bcm_vlan_mac_action_add
 * Description   :
 *      Add association from MAC address to VLAN.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (IN) Action for outer and inner tag
 * Note:
 *   Program VLAN_XLATEm.
 */
int _bcm_trx_vlan_mac_action_add(int                   unit,
                                 bcm_mac_t              mac,
                                 bcm_vlan_action_set_t *action) 
{
    int rv;
    uint32 profile_idx;
    vlan_mac_entry_t vment;

    if (action->priority > 7) {
        return BCM_E_PARAM;
    }

    rv = _bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx);

    sal_memset(&vment, 0, sizeof(vment));
    soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
    soc_VLAN_MACm_field32_set(unit, &vment, KEY_TYPEf,
                              TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_VLAN_MACm_field32_set(unit, &vment, OVIDf, action->new_outer_vlan);
    soc_VLAN_MACm_field32_set(unit, &vment, IVIDf, action->new_inner_vlan);
    if (action->priority >= 0) {
        soc_VLAN_MACm_field32_set(unit, &vment, PRIf, action->priority);
    }
    soc_VLAN_MACm_field32_set(unit, &vment, VALIDf, 1);
    soc_VLAN_MACm_field32_set(unit, &vment, TAG_ACTION_PROFILE_PTRf, 
                              profile_idx);

    rv = soc_mem_insert_return_old(unit, VLAN_MACm, MEM_BLOCK_ALL, &vment, &vment);

    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan action profile entry */
        profile_idx = soc_VLAN_MACm_field32_get(unit, &vment,
                                                TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_mac_action_get
 * Description   :
 *      Get association from MAC address to VLAN tag actions.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (OUT) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_mac_action_get(int unit, bcm_mac_t  mac,
                             bcm_vlan_action_set_t *action) 
{
    vlan_mac_entry_t vment;             /* Lookup key hw buffer.      */
    vlan_mac_entry_t res_vment;         /* Lookup result buffer.      */
    uint32 profile_idx;                 /* Vlan action profile index. */
    int rv;                             /* Operation return status.   */
    int idx = 0;                        /* Lookup result entry index. */

    /* Input parameters check. */
    if (NULL == action) {
        return (BCM_E_PARAM);
    }

    /* Reset lookup key and result destination buffer. */
    sal_memset(&vment, 0, sizeof(vlan_mac_entry_t));
    sal_memset(&res_vment, 0, sizeof(vlan_mac_entry_t));

    /* Initialize lookup key. */
    soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
    soc_VLAN_MACm_field32_set(unit, &vment, KEY_TYPEf,
                              TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);

    /* Perform VLAN_MAC table search by mac address. */
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_search(unit, VLAN_MACm, MEM_BLOCK_ALL, &idx, 
                        &vment, &res_vment, 0);
    soc_mem_unlock(unit, VLAN_MACm);
    BCM_IF_ERROR_RETURN(rv);

    /* Extract action profile index if mac entry was found. */
    profile_idx = soc_VLAN_MACm_field32_get(unit, &res_vment,
                                            TAG_ACTION_PROFILE_PTRf);       
    action->new_outer_vlan = soc_VLAN_MACm_field32_get(unit, &res_vment, OVIDf);
    action->new_inner_vlan = soc_VLAN_MACm_field32_get(unit, &res_vment, IVIDf);
    action->priority = soc_VLAN_MACm_field32_get(unit, &res_vment, PRIf);

    /* Read action profile data. */
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_vlan_mac_delete
 * Purpose:
 *      Delete a vlan mac lookup entry.
 * Parameters:
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_mac_delete(int unit, bcm_mac_t mac) 
{
    vlan_mac_entry_t  vment;
    int rv; 
    uint32 profile_idx;

    sal_memset(&vment, 0, sizeof(vment));
    soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
    soc_VLAN_MACm_field32_set(unit, &vment, KEY_TYPEf,
                              TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_delete_return_old(unit, VLAN_MACm, MEM_BLOCK_ALL,
                                   &vment, &vment);
    soc_mem_unlock(unit, VLAN_MACm);
    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    } else if ((rv == SOC_E_NONE) && soc_VLAN_MACm_field32_get(unit, &vment, VALIDf)) {
        profile_idx = soc_VLAN_MACm_field32_get(unit, &vment,
                                                TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      bcm_vlan_mac_delete_all
 * Purpose:
 *      Delete all vlan mac lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_mac_delete_all(int unit) 
{
    int i, imin, imax, nent, vmbytes, rv;
    uint32 old_profile_idx;
    vlan_mac_entry_t *vmtab, *vmnull, *vmtabp;
    
    imin = soc_mem_index_min(unit, VLAN_MACm);
    imax = soc_mem_index_max(unit, VLAN_MACm);
    nent = soc_mem_index_count(unit, VLAN_MACm);
    vmbytes = soc_mem_entry_words(unit, VLAN_MACm);
    vmbytes = WORDS2BYTES(vmbytes);

    vmtab = soc_cm_salloc(unit, nent * vmbytes, "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vmnull = soc_mem_entry_null(unit, VLAN_MACm);

    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_read_range(unit, VLAN_MACm, MEM_BLOCK_ANY,
                            imin, imax, vmtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_MACm);
        soc_cm_sfree(unit, vmtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {
        vmtabp = soc_mem_table_idx_to_pointer(unit, VLAN_MACm,
                        vlan_mac_entry_t *, vmtab, i);

        if (!soc_VLAN_MACm_field32_get(unit, vmtabp, VALIDf) ||
            (soc_VLAN_MACm_field32_get(unit, vmtabp, KEY_TYPEf) !=
             TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC)) {
            continue;
        }
        old_profile_idx = soc_VLAN_MACm_field32_get(unit, vmtabp,
                                                    TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_VLAN_MACm(unit, MEM_BLOCK_ANY, i, vmnull);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }
    
    soc_mem_unlock(unit, VLAN_MACm);
    soc_cm_sfree(unit, vmtab);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_mac_action_traverse
 * Description   :
 *      Traverse over vlan mac actions, which are used for VLAN
 *      tag/s assignment to untagged packets.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_mac_action_traverse(int unit, 
                                  bcm_vlan_mac_action_traverse_cb cb, 
                                  void *user_data)
{
    int idx, imin, imax, nent, vmbytes, rv;
    uint32 profile_idx;
    bcm_mac_t  mac;
    bcm_vlan_action_set_t action;
    vlan_mac_entry_t * vmtab, *vmtabp;
    
    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    imin = soc_mem_index_min(unit, VLAN_MACm);
    imax = soc_mem_index_max(unit, VLAN_MACm);
    nent = soc_mem_index_count(unit, VLAN_MACm);
    vmbytes = soc_mem_entry_words(unit, VLAN_MACm);
    vmbytes = WORDS2BYTES(vmbytes);
    vmtab = soc_cm_salloc(unit, nent * sizeof(*vmtab), "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_read_range(unit, VLAN_MACm, MEM_BLOCK_ANY,
                            imin, imax, vmtab);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_MACm);
        soc_cm_sfree(unit, vmtab);
        return rv; 
    }
    
    for(idx = 0; idx < nent; idx++) {
        sal_memset(mac, 0, sizeof(bcm_mac_t));
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        vmtabp = soc_mem_table_idx_to_pointer(unit, VLAN_MACm,
                                              vlan_mac_entry_t *, 
                                              vmtab, idx);

        if ((0 == soc_VLAN_MACm_field32_get(unit, vmtabp, VALIDf)) ||
            (TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC !=
             soc_VLAN_MACm_field32_get(unit, vmtabp, KEY_TYPEf))) {
            continue;
        }

        /* Get entry mac address. */
        soc_VLAN_MACm_mac_addr_get(unit, vmtabp, MAC_ADDRf, mac);

        /* Get vlan action profile index. */
        profile_idx = soc_VLAN_MACm_field32_get(unit, vmtabp,
                                                TAG_ACTION_PROFILE_PTRf);

        /* Read action profile data. */
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);

        /* Call traverse callback with the data. */
        rv = cb(unit, mac, &action, user_data);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_MACm);
            soc_cm_sfree(unit, vmtab);
            return rv; 
        }
    }
    
    soc_mem_unlock(unit, VLAN_MACm);
    soc_cm_sfree(unit, vmtab);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_entry_parse
 * Purpose:
 *      Parses vlan translate entry by given key_type, for inner and outer vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) vlan translate entry to parse
 *      action          (OUT) Action to fill
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_trx_vlan_translate_entry_parse(int unit, soc_mem_t mem, uint32 *vent,
                                   bcm_vlan_action_set_t *action)
{
    if ((NULL == vent) || (NULL == action) || (INVALIDm == mem)) {
        return BCM_E_PARAM;
    }

    action->new_outer_vlan = soc_mem_field32_get(unit, mem, vent, NEW_OVIDf);
    action->new_inner_vlan = soc_mem_field32_get (unit, mem, vent, NEW_IVIDf);
    action->priority = soc_mem_field32_get(unit, mem, vent, PRIf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_trx_vlan_translate_entry_assemble
 * Purpose:
 *      Constructs vlan translate entry from given key_type, inner and outer vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      vent            (IN/OUT) vlan translate entry to construct
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      inner_vlan      (IN) inner VLAN ID
 *      outer_vlan      (IN) outer VLAN ID
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_translate_entry_assemble(int unit, vlan_xlate_entry_t *vent,
                                      bcm_gport_t port,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t inner_vlan, 
                                      bcm_vlan_t outer_vlan)
{
    int             use_glp = 1;    /* By default incoming port used in lookup key */
    int             tmp_id;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    bcm_trunk_t     trunk_out;

    switch (key_type) {
        case bcmVlanTranslateKeyDouble:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortDouble:
            soc_VLAN_XLATEm_field32_set(unit, vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_VLAN_XLATEm_field32_set(unit, vent, OVIDf, outer_vlan);
            soc_VLAN_XLATEm_field32_set(unit, vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyOuterTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuterTag:
            soc_VLAN_XLATEm_field32_set(unit, vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OTAG);
            soc_VLAN_XLATEm_field32_set(unit, vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyInnerTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInnerTag:
            soc_VLAN_XLATEm_field32_set(unit, vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_ITAG);
            soc_VLAN_XLATEm_field32_set(unit, vent, ITAGf, inner_vlan);
            break;
        case bcmVlanTranslateKeyOuter:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuter:
            soc_VLAN_XLATEm_field32_set(unit, vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_VLAN_XLATEm_field32_set(unit, vent, OVIDf, outer_vlan);
            break;
        case bcmVlanTranslateKeyInner:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInner:
            soc_VLAN_XLATEm_field32_set(unit, vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID);
            soc_VLAN_XLATEm_field32_set(unit, vent, IVIDf, inner_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_out,
                               &tmp_id));

    if (use_glp) {
        if (BCM_GPORT_IS_TRUNK(port)) {
            soc_VLAN_XLATEm_field32_set(unit, vent, Tf, 1);
            soc_VLAN_XLATEm_field32_set(unit, vent, TGIDf, trunk_out);
        } else {
            soc_VLAN_XLATEm_field32_set(unit, vent, MODULE_IDf, mod_out);
            soc_VLAN_XLATEm_field32_set(unit, vent, PORT_NUMf, port_out);
        }
    } else {
        /* incoming port not used in lookup key, set field to all 1's */
        soc_VLAN_XLATEm_field32_set(unit, vent, GLPf, 
                                    BCM_VLAN_XLATE_GLP_WILDCARD);
    }
    return BCM_E_NONE;
}


/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_add
 * Description   :
 *      Add an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int 
_bcm_trx_vlan_translate_action_add(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
    uint32 profile_idx; 
    int rv, gport_id, use_glp = 1;
    vlan_xlate_entry_t vent;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;

    if (action->priority > 7) {
        return BCM_E_PARAM;
    }

    sal_memset(&vent, 0, sizeof(vent));
    switch (key_type) {
        case bcmVlanTranslateKeyDouble:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortDouble:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
            soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyOuterTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuterTag:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OTAG);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyInnerTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInnerTag:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_ITAG);
            soc_VLAN_XLATEm_field32_set(unit, &vent, ITAGf, inner_vlan);
            break;
        case bcmVlanTranslateKeyOuter:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuter:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
            break;
        case bcmVlanTranslateKeyInner:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInner:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (use_glp) {
        if (BCM_GPORT_IS_TRUNK(port)) {
            soc_VLAN_XLATEm_field32_set(unit, &vent, Tf, 1);
            soc_VLAN_XLATEm_field32_set(unit, &vent, TGIDf, trunk_id);
        } else {
            soc_VLAN_XLATEm_field32_set(unit, &vent, MODULE_IDf, mod_out);
            soc_VLAN_XLATEm_field32_set(unit, &vent, PORT_NUMf, port_out);
        }
    } else {
        /* incoming port not used in lookup key, set field to all 1's */
        soc_VLAN_XLATEm_field32_set(unit, &vent, GLPf, 
                                    BCM_VLAN_XLATE_GLP_WILDCARD);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));

    if (action->priority > 0) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, RPEf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, PRIf, action->priority);
    }
    else
    {
        soc_VLAN_XLATEm_field32_set(unit, &vent, RPEf, 0);
        soc_VLAN_XLATEm_field32_set(unit, &vent, PRIf, action->priority);
    }
    soc_VLAN_XLATEm_field32_set(unit, &vent, TAG_ACTION_PROFILE_PTRf,
                                profile_idx);
    soc_VLAN_XLATEm_field32_set(unit, &vent, NEW_IVIDf, 
                                action->new_inner_vlan);
    soc_VLAN_XLATEm_field32_set(unit, &vent, NEW_OVIDf, 
                                action->new_outer_vlan);

    if (SOC_IS_TR_VL(unit) ) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, MPLS_ACTIONf, 0);
    }
    soc_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);

    rv = soc_mem_insert_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vent, &vent);

    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_get
 * Purpose:
 *      Gets a vlan translate entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      old_vlan        (IN) Packet old VLAN ID
 *      new_vlan        (OUT) Packet new VLAN ID
 *      prio            (OUT) Packet prio
 */
int 
_bcm_trx_vlan_translate_action_get (int unit, 
                                   bcm_gport_t port, 
                                   bcm_vlan_translate_key_t key_type, 
                                   bcm_vlan_t outer_vid,
                                   bcm_vlan_t inner_vid,
                                   bcm_vlan_action_set_t *action)
{
    vlan_xlate_entry_t  vent, res_vent;
    int                 rv;
    int                 idx = 0;
    uint32              profile_idx = 0;

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
    sal_memset(&res_vent, 0, sizeof(vlan_xlate_entry_t));
    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_translate_entry_assemble(unit, &vent, port, key_type, 
                                              inner_vid, outer_vid));
    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &idx, 
                         &vent, &res_vent, 0);
    soc_mem_unlock(unit, VLAN_XLATEm);
    BCM_IF_ERROR_RETURN(rv);
    profile_idx = soc_VLAN_XLATEm_field32_get(unit, &res_vent,
                                              TAG_ACTION_PROFILE_PTRf);       
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

    return _bcm_trx_vlan_translate_entry_parse(unit, VLAN_XLATEm,  
                                               (uint32 *)&res_vent, action);
}


/*
 * Function:
 *      bcm_vlan_translate_action_delete
 * Purpose:
 *      Delete a vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int 
_bcm_trx_vlan_translate_action_delete(int unit,
                                     bcm_gport_t port,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan)
{
    vlan_xlate_entry_t vent;
    uint32 profile_idx;
    int gport_id, rv, use_glp = 1;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;

    sal_memset(&vent, 0, sizeof(vent));
    switch (key_type) {
        case bcmVlanTranslateKeyDouble:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortDouble:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
            soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyOuterTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuterTag:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OTAG);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyInnerTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInnerTag:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_ITAG);
            soc_VLAN_XLATEm_field32_set(unit, &vent, ITAGf, inner_vlan);
            break;
        case bcmVlanTranslateKeyOuter:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuter:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
            break;
        case bcmVlanTranslateKeyInner:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInner:
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (use_glp) {
        if (BCM_GPORT_IS_TRUNK(port)) {
            soc_VLAN_XLATEm_field32_set(unit, &vent, Tf, 1);
            soc_VLAN_XLATEm_field32_set(unit, &vent, TGIDf, trunk_id);
        } else {
            soc_VLAN_XLATEm_field32_set(unit, &vent, MODULE_IDf, mod_out);
            soc_VLAN_XLATEm_field32_set(unit, &vent, PORT_NUMf, port_out);
        }
    } else {
        /* incoming port not used in lookup key, set field to all 1's */
        soc_VLAN_XLATEm_field32_set(unit, &vent, GLPf, 
                                    BCM_VLAN_XLATE_GLP_WILDCARD);
    }

    rv = soc_mem_delete_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vent, &vent);
    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    } else if ((rv == SOC_E_NONE) && soc_VLAN_XLATEm_field32_get(unit, &vent, VALIDf)) {
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_action_delete_all
 * Purpose:
 *      Delete all vlan translate lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_translate_action_delete_all(int unit) 
{
    int i, imin, imax, nent, vbytes, rv, mpls;
    uint32 old_profile_idx;
    vlan_xlate_entry_t * vtab, * vnull,  *vtabp;
    
    imin = soc_mem_index_min(unit, VLAN_XLATEm);
    imax = soc_mem_index_max(unit, VLAN_XLATEm);
    nent = soc_mem_index_count(unit, VLAN_XLATEm);
    vbytes = soc_mem_entry_words(unit, VLAN_XLATEm);
    vbytes = WORDS2BYTES(vbytes);
    vtab = soc_cm_salloc(unit, nent * sizeof(*vtab), "vlan_xlate");

    if (vtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vnull = soc_mem_entry_null(unit, VLAN_XLATEm);

    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_read_range(unit, VLAN_XLATEm, MEM_BLOCK_ANY,
                            imin, imax, vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_XLATEm);
        soc_cm_sfree(unit, vtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {
        vtabp = soc_mem_table_idx_to_pointer(unit, VLAN_XLATEm,
                                             vlan_xlate_entry_t *, vtab, i);

        if (SOC_IS_TR_VL(unit)) {
            mpls = soc_VLAN_XLATEm_field32_get(unit, vtabp, MPLS_ACTIONf);
        } else {
            mpls = 0;
        }

        if (!soc_VLAN_XLATEm_field32_get(unit, vtabp, VALIDf) ||
            (mpls != 0) ||
            (soc_VLAN_XLATEm_field32_get(unit, vtabp, KEY_TYPEf) == 
             TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC)) {
            continue;
        }

        old_profile_idx = soc_VLAN_XLATEm_field32_get(unit, vtabp,
                                                      TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_VLAN_XLATEm(unit, MEM_BLOCK_ANY, i, vnull);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }
    
    soc_mem_unlock(unit, VLAN_XLATEm);
    soc_cm_sfree(unit, vtab);
    return rv;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_add
 * Description   :
 *      Add a range of VLANs and an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 *      action          (IN) Action for outer and inner tag
 */
int
_bcm_trx_vlan_translate_action_range_add(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
    int i, key_type = 0, rv, gport_id, old_idx, stm_idx = 0;
    uint32 new_idx;
    source_trunk_map_table_entry_t stm_entry;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    bcm_trunk_t  trunk_id;
    bcm_vlan_t vlan_low, vlan_high, min_vlan[8], max_vlan[8];

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        key_type = bcmVlanTranslateKeyPortDouble;
        vlan_low = outer_vlan_low;
        vlan_high = outer_vlan_high;
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        vlan_low = outer_vlan_low;
        vlan_high = outer_vlan_high;
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        vlan_low = inner_vlan_low;
        vlan_high = inner_vlan_high;
    } else {
        return BCM_E_PARAM;
    }
    if (action->priority > 7) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        return BCM_E_PARAM;
    } else {
        stm_idx = (mod_out * (SOC_PORT_ADDR_MAX(unit) + 1)) + port_out;
    }

    soc_mem_lock(unit, ING_VLAN_RANGEm);

    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }
    old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                  &stm_entry, VLAN_RANGE_IDXf);

    /* Get the profile table entry for this port/trunk */
    _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, old_idx);

    /* Find the first unused min/max range. Unused ranges are
     * identified by { min == 1, max == 0 }
     */
    for (i = 0 ; i < 8 ; i++) {
        if ((min_vlan[i] == 1) && (max_vlan[i] == 0)) {
            break;
        } else if (min_vlan[i] == vlan_low) {
            /* Can't have multiple ranges with the same min */
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            return BCM_E_EXISTS;
        }
    }
    if (i == 8) {
        /* All ranges are taken */
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return BCM_E_FULL;
    }

    /* Insert the new range into the table entry sorted by min VID */
    for ( ; i > 0 ; i--) {
        if (min_vlan[i - 1] > vlan_low) {
            /* Move existing min/max down */
            min_vlan[i] = min_vlan[i - 1];
            max_vlan[i] = max_vlan[i - 1];
        } else {
            break;
        }
    }
    min_vlan[i] = vlan_low;
    max_vlan[i] = vlan_high;

    /* Try adding the new profile table entry */
    rv = _bcm_trx_vlan_range_profile_entry_add(unit, min_vlan, max_vlan, &new_idx);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }

    /* Add an entry in the vlan translate table for the low VID */
    rv = _bcm_trx_vlan_translate_action_add(unit, port, key_type,
                                           outer_vlan_low, inner_vlan_low,
                                           action);
    if (rv != BCM_E_NONE) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }

    /* Update the profile pointer */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm,
                        &stm_entry, VLAN_RANGE_IDXf, new_idx);
    rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv != BCM_E_NONE) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }

    /* Delete the old profile entry */
    _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
    soc_mem_unlock(unit, ING_VLAN_RANGEm);

    return rv;
}

/*
 * Function   :
 *      _bcm_trx_vlan_range_profile_index_get
 * Description   :
 *      Get the index for profile entry in the ING_VLAN_RANGE memory.
 * Parameters   :
 *      unit        (IN) BCM unit number
 *      gport       (IN) Ingress generic port
 *      profile_idx (OUT) index to profile entry
 */
int 
_bcm_trx_vlan_range_profile_index_get(int unit, bcm_gport_t gport, 
                                     int *profile_idx)
{
    int                             idx = 0, gport_id = 0;
    bcm_port_t                      port_out;
    bcm_module_t                    mod_out;
    bcm_trunk_t                     trunk_id;
    uint32                          stm_idx;     /* Source Trunk Map index */
    source_trunk_map_table_entry_t  stm_entry;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(gport)) {
        return BCM_E_PARAM;
    } else {
        if(SOC_IS_ENDURO(unit)) {
            stm_idx = (mod_out * 64) + port_out;
        }
        else {
            stm_idx = (mod_out * (SOC_PORT_ADDR_MAX(unit) + 1)) + port_out;
        }
    }

    BCM_IF_ERROR_RETURN(
       READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry));
    idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                              &stm_entry, VLAN_RANGE_IDXf);
    *profile_idx = idx;
    return BCM_E_NONE;
}


/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_get
 * Description   :
 *      Get a range of VLANs and an entry to ingress VLAN translation table.
 *      for Triumph
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 *      action          (OUT) Action for outer and inner tag
 */
int
_bcm_trx_vlan_translate_action_range_get(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
    int         key_type = 0, profile_idx, i;
    bcm_vlan_t  vlan_low = BCM_VLAN_INVALID, vlan_high = BCM_VLAN_INVALID;
    bcm_vlan_t  min_vlan[8], max_vlan[8];
    int rv;

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        key_type = bcmVlanTranslateKeyPortDouble;
        vlan_low = outer_vlan_low;
        vlan_high = outer_vlan_high;
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        vlan_low = outer_vlan_low;
        vlan_high = outer_vlan_high;
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        vlan_low = inner_vlan_low;
        vlan_high = inner_vlan_high;
    } else {
        return BCM_E_PARAM;
    }


    rv = _bcm_trx_vlan_range_profile_index_get(unit, port, &profile_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the profile table entry for this port/trunk */
    _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, 
                                          max_vlan, profile_idx);
    /* Find range match */
    for (i = 0 ; i < 8 ; i++) {
        if ((min_vlan[i] == vlan_low) && (max_vlan[i] == vlan_high)) {
            break;
        }
    }
    if (i == 8) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_trx_vlan_translate_action_get(unit,port, key_type,
                                            outer_vlan_low, 
                                            inner_vlan_low, action);  
    return (rv);
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_delete
 * Description   :
 *      Delete a range of VLANs and an entry from ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 */
int
_bcm_trx_vlan_translate_action_range_delete(int unit,
                                           bcm_gport_t port,
                                           bcm_vlan_t outer_vlan_low,
                                           bcm_vlan_t outer_vlan_high,
                                           bcm_vlan_t inner_vlan_low,
                                           bcm_vlan_t inner_vlan_high)
{
    int i, key_type = 0, rv, gport_id, old_idx, stm_idx = 0;
    uint32 new_idx;
    source_trunk_map_table_entry_t stm_entry;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    bcm_trunk_t  trunk_id;
    bcm_vlan_t vlan_low, vlan_high, min_vlan[8], max_vlan[8];

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        key_type = bcmVlanTranslateKeyPortDouble;
        vlan_low = outer_vlan_low;
        vlan_high = outer_vlan_high;
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        vlan_low = outer_vlan_low;
        vlan_high = outer_vlan_high;
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        vlan_low = inner_vlan_low;
        vlan_high = inner_vlan_high;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        return BCM_E_PARAM;
    } else {
        if(SOC_IS_ENDURO(unit)) {
            stm_idx = (mod_out * 64) + port_out;
        }
        else {
            stm_idx = (mod_out * (SOC_PORT_ADDR_MAX(unit) + 1)) + port_out;
        }
    }

    soc_mem_lock(unit, ING_VLAN_RANGEm);
    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }
    old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                  &stm_entry, VLAN_RANGE_IDXf);

    /* Get the profile table entry for this port/trunk */
    _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, old_idx);

    /* Find the min/max range. */
    for (i = 0 ; i < 8 ; i++) {
        if ((min_vlan[i] == vlan_low) && (max_vlan[i] == vlan_high)) {
            break;
        }
    }
    if (i == 8) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return BCM_E_NOT_FOUND;
    }

    /* Remove the range from the table entry and fill in the gap */
    for ( ; i < 7 ; i++) {
        /* Move existing min/max UP */
        min_vlan[i] = min_vlan[i + 1];
        max_vlan[i] = max_vlan[i + 1];
    }
    /* Mark last min/max range as unused. Unused ranges are
     * identified by { min == 1, max == 0 }
     */
    min_vlan[i] = 1;
    max_vlan[i] = 0;

    /* Try adding the new profile table entry */
    rv = _bcm_trx_vlan_range_profile_entry_add(unit, min_vlan, max_vlan, &new_idx);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }

    /* Delete the entry from the vlan translate table for the low VID */
    rv = _bcm_trx_vlan_translate_action_delete(unit, port, key_type,
                                              outer_vlan_low, inner_vlan_low);
    if (rv != BCM_E_NONE) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }

    /* Update the profile pointer */
    soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm,
                        &stm_entry, VLAN_RANGE_IDXf, new_idx);
    rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv != BCM_E_NONE) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        return rv;
    }

    /* Delete the old profile entry */
    _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
    soc_mem_unlock(unit, ING_VLAN_RANGEm);

    return rv;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_range_delete_all
 * Description   :
 *      Delete all ranges of VLANs and entries from ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 */
int
_bcm_trx_vlan_translate_action_range_delete_all(int unit)
{
    int rv;

    rv = _bcm_trx_vlan_range_profile_init(unit);
    if (rv == BCM_E_NONE) {
        rv = _bcm_trx_vlan_translate_action_delete_all(unit);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_trx_vlan_translate_stm_index_to_gport
 * Description:
 *      Translate index of SOURCE_TRUNK_MAP_TABLE into gport
 * Parameters:
 *      unit         device number
 *      stm_index    Index to SOURCE_TRUNK_MAP_TABLE
 *      gport       (OUT) gport
 * Return:
 *     BCM_E_XXX
 */
STATIC int  
_bcm_trx_vlan_translate_stm_index_to_gport(int unit, int stm_index, 
                                          bcm_gport_t *gport)
{
    bcm_module_t    modid;
    bcm_port_t      port;

    if (NULL == gport) {
        return BCM_E_PARAM;
    }

    modid = stm_index / (SOC_PORT_ADDR_MAX(unit) + 1) ; 
    port = stm_index - modid * (SOC_PORT_ADDR_MAX(unit) + 1);

    if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return BCM_E_PARAM;
    }

    BCM_GPORT_MODPORT_SET(*gport, modid, port);

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_traverse
 * Description   :
 *      Traverses over range of ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN/OUT) Traverse Structure to opertae on
 */
int
_bcm_trx_vlan_translate_action_range_traverse(int unit, 
                                    _bcm_vlan_translate_traverse_t *trvs_info)
{
        /* Indexes to iterate over memories, chunks and entries */
    int                           chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int                             buf_size, chunksize, chnk_end;
    /* Buffer to store chunks of memory table we currently work on */
    uint32                          *stm_tbl_chnk;
    source_trunk_map_table_entry_t  *stm_ent;
    soc_mem_t                       mem;
    int                             stop, rv = BCM_E_NONE;
    /* Index to point to table of vlan ranges. */
    int                             range_idx, stm_index, i;
    bcm_gport_t                     gport;
    bcm_vlan_t                      min_vlan[8], max_vlan[8], new_outer_vlan;
    

    mem = SOURCE_TRUNK_MAP_TABLEm;
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    stm_tbl_chnk = soc_cm_salloc(unit, buf_size, "vlan translate range traverse");
    if (NULL == stm_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)stm_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) <= mem_idx_max) ? 
            chnk_idx + chunksize : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, stm_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx < chnk_end; ent_idx ++) {
            stm_ent = 
                soc_mem_table_idx_to_pointer(unit, mem, 
                                             source_trunk_map_table_entry_t *, 
                                             stm_tbl_chnk, ent_idx);
            range_idx = soc_mem_field32_get(unit, mem, &stm_ent, 
                                            VLAN_RANGE_IDXf);
            stm_index = chnk_idx * chunksize + ent_idx;
            rv = _bcm_trx_vlan_translate_stm_index_to_gport(unit, stm_index, 
                                                           &gport);
            if (BCM_FAILURE(rv)) {
                break;
            }
            trvs_info->gport = gport;
            new_outer_vlan = BCM_VLAN_INVALID;
            _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, range_idx);
            if ((1 == min_vlan[0]) && (0 == max_vlan[0]) ){
                continue;
            }
            for (i = 0; i < 8; i++) {
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_vlan_translate_action_range_get(unit, gport, min_vlan[i], 
                                                            max_vlan[i], 
                                                            BCM_VLAN_INVALID,
                                                            BCM_VLAN_INVALID, 
                                                            trvs_info->action));
                if (BCM_VLAN_INVALID != trvs_info->action->new_outer_vlan) {
                    trvs_info->outer_vlan = min_vlan[i];
                    trvs_info->outer_vlan_high = max_vlan[i];
                    new_outer_vlan = trvs_info->action->new_outer_vlan;
                }
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_vlan_translate_action_range_get(unit, gport, 
                                                            BCM_VLAN_INVALID,
                                                            BCM_VLAN_INVALID,
                                                            min_vlan[i], 
                                                            max_vlan[i], 
                                                            trvs_info->action));
                if (BCM_VLAN_INVALID != trvs_info->action->new_inner_vlan) {
                    trvs_info->inner_vlan = min_vlan[i];
                    trvs_info->inner_vlan_high = max_vlan[i];
                }
                trvs_info->action->new_outer_vlan = new_outer_vlan;

                rv = trvs_info->int_cb(unit, trvs_info, &stop);
                if (BCM_FAILURE(rv)) {
                    break;
                }
                if (TRUE == stop) {
                    break;
                }
            }
        }
    }
    soc_cm_sfree(unit, stm_tbl_chnk);
    return rv;        

}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_add
 * Description   :
 *      Add an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int 
_bcm_trx_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 profile_idx;
    egr_vlan_xlate_entry_t vent;

    if ((action->priority > 7)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_add(unit, action, &profile_idx));

    sal_memset(&vent, 0, sizeof(vent));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, PORT_GROUP_IDf, port_class);
#ifndef LVL7_FIXUP
    if (action->priority >= 0) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, RPEf, 1);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, PRIf, action->priority);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IRPEf, 1);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IPRIf, action->priority);
    }
#else
    if (action->priority > 0) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, RPEf, 1);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, PRIf, action->priority);
        /* Internal priority should be zero */
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IRPEf, 1);
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IPRIf, 0);
    }
#endif
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, TAG_ACTION_PROFILE_PTRf,
                                    profile_idx);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_IVIDf, 
                                    action->new_inner_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OVIDf, 
                                    action->new_outer_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OTAG_VPTAG_SELf, 0);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);

    rv = soc_mem_insert_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &vent, &vent);

    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &vent,
                                                      TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_egress_action_get
 * Purpose:
 *      Get an egress vlan translate entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      old_vid         (IN) Packet old VLAN ID to match
 *      new_vid         (OUT) Packet new VLAN ID
 *      prio            (OUT) Translation priority
 */
extern int 
_bcm_trx_vlan_translate_egress_action_get(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action)
{
    egr_vlan_xlate_entry_t  vent;
    egr_vlan_xlate_entry_t  res_vent;
    int                     rv;
    int                     idx = 0, profile_idx;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));
    sal_memset(&res_vent, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, PORT_GROUP_IDf, port_class);
    soc_mem_lock(unit, EGR_VLAN_XLATEm);
    rv = soc_mem_search(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &idx, 
                        &vent, &res_vent, 0);
    soc_mem_unlock(unit, EGR_VLAN_XLATEm);
    BCM_IF_ERROR_RETURN(rv);
    profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &res_vent,
                                              TAG_ACTION_PROFILE_PTRf);       
    _bcm_trx_egr_vlan_action_profile_entry_get(unit, action, profile_idx);
   
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_translate_egress_action_delete
 * Purpose:
 *      Delete an egress vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int 
_bcm_trx_vlan_translate_egress_action_delete(int unit, int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan)
{
    egr_vlan_xlate_entry_t vent;
    int rv; 
    uint32 profile_idx;

    sal_memset(&vent, 0, sizeof(vent));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, outer_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, IVIDf, inner_vlan);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, PORT_GROUP_IDf, port_class);

    rv = soc_mem_delete_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &vent, &vent);
    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    } else if ((rv == SOC_E_NONE) && soc_EGR_VLAN_XLATEm_field32_get(unit, &vent, VALIDf)) {
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &vent,
                                                      TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}

/*
 * Function:
 *      bcm_vlan_translate_egress_action_delete_all
 * Purpose:
 *      Delete all egress vlan translate lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_translate_egress_action_delete_all(int unit) 
{
    int i, imin, imax, nent, vbytes, rv;
    uint32 old_profile_idx;
    egr_vlan_xlate_entry_t * vtab, * vnull,  *vtabp;
    
    imin = soc_mem_index_min(unit, EGR_VLAN_XLATEm);
    imax = soc_mem_index_max(unit, EGR_VLAN_XLATEm);
    nent = soc_mem_index_count(unit, EGR_VLAN_XLATEm);
    vbytes = soc_mem_entry_words(unit, EGR_VLAN_XLATEm);
    vbytes = WORDS2BYTES(vbytes);
    vtab = soc_cm_salloc(unit, nent * sizeof(*vtab), "egr_vlan_xlate");

    if (vtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vnull = soc_mem_entry_null(unit, EGR_VLAN_XLATEm);

    soc_mem_lock(unit, EGR_VLAN_XLATEm);
    rv = soc_mem_read_range(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ANY,
                            imin, imax, vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, EGR_VLAN_XLATEm);
        soc_cm_sfree(unit, vtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {
        vtabp = soc_mem_table_idx_to_pointer(unit, EGR_VLAN_XLATEm,
                                             egr_vlan_xlate_entry_t *, vtab, i);

        if (!soc_EGR_VLAN_XLATEm_field32_get(unit, vtabp, VALIDf)) {
            continue;
        }
        old_profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, vtabp,
                                                          TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_EGR_VLAN_XLATEm(unit, MEM_BLOCK_ANY, i, vnull);

        /* Delete the old vlan action profile entry */
        if (rv == SOC_E_NONE) {
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, 
                                                              old_profile_idx);
        }
    }
    
    soc_mem_unlock(unit, EGR_VLAN_XLATEm);
    soc_cm_sfree(unit, vtab);
    return rv;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_old_vlan_get
 * Description   :
 *      Helper function to get an old vid from vlan translate entry 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) Vlan translate entry
 *      key_type        (IN) Key type to determine which old vid to get
 *      outer_vlan      (OUT) outer vlan to retrieve
 *      inner_vlan      (OUT) inner vlan to retrieve
 */

STATIC int 
_bcm_trx_vlan_translate_old_vlan_get(int unit, soc_mem_t mem, 
                                     uint32 *vent, uint32 key_type,
                                     bcm_vlan_t *outer_vlan, 
                                     bcm_vlan_t *inner_vlan)
{
    bcm_vlan_t  tmp_o_vlan = BCM_VLAN_INVALID;
    bcm_vlan_t  tmp_i_vlan = BCM_VLAN_INVALID;

    /* Input parameters check. */ 
    if ((NULL  == vent) || (NULL == outer_vlan) || 
        (INVALIDm == mem) || (NULL == inner_vlan)) {
        return (BCM_E_PARAM);
    }

    switch (key_type) {
      case bcmVlanTranslateKeyDouble:
      case bcmVlanTranslateKeyPortDouble:
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, OVIDf);
          tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, IVIDf);
          break;
      case bcmVlanTranslateKeyOuterTag:
      case bcmVlanTranslateKeyPortOuterTag:
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, OTAGf);
          break;
      case bcmVlanTranslateKeyInnerTag:
      case bcmVlanTranslateKeyPortInnerTag:
          tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, ITAGf);
          break;
      case bcmVlanTranslateKeyOuter:
      case bcmVlanTranslateKeyPortOuter:
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, OVIDf);
          break;
      case bcmVlanTranslateKeyInner:
      case bcmVlanTranslateKeyPortInner:
          tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, IVIDf);
          break;
      default:
          return BCM_E_PARAM;
    }

    *outer_vlan = tmp_o_vlan;
    *inner_vlan = tmp_i_vlan;

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_gport_get
 * Description   :
 *      Helper function to get a gport from vlan translate entry 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) Vlan translate entry
 *      gport           (OUT) gport to retrieve
 */

STATIC int
_bcm_trx_vlan_translate_gport_get(int unit, soc_mem_t mem,
                                  uint32 *vent, bcm_gport_t *gport)
{
    bcm_gport_t     tmp_gport = 0;
    uint32          modid;
    uint32          port;
    int             glp_wildcard;


    /* Input parameters check. */
    if ((NULL == vent) || (NULL == gport) || (INVALIDm == mem)) {
        return (BCM_E_PARAM);
    }

    glp_wildcard = (soc_mem_field32_get(unit, mem, vent, GLPf) == 
                    BCM_VLAN_XLATE_GLP_WILDCARD);
    if (glp_wildcard) {
        tmp_gport = BCM_GPORT_INVALID;
    } else {
        if (soc_mem_field32_get(unit, mem, vent, Tf)) {
            port = soc_mem_field32_get(unit, mem, vent, TGIDf); 
            BCM_GPORT_TRUNK_SET(tmp_gport, port); 
        } else {
            port = soc_mem_field32_get(unit, mem, vent, PORT_NUMf); 
            modid = soc_mem_field32_get(unit, mem, vent, MODULE_IDf); 
            BCM_GPORT_MODPORT_SET(tmp_gport, modid, port); 
        }
    }

    *gport = tmp_gport;
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_key_type_get
 * Description   :
 *      Helper function to get a key_type from vlan translate entry 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) Vlan translate entry
 *      key_type        (OUT) Key type to retrieve
 */

STATIC int 
_bcm_trx_vlan_translate_key_type_get(int unit, soc_mem_t mem, 
                                     uint32 *vent, uint32 *key_type)
{
    uint32 key_val, glp_wildcard;

    /* Input parameters check. */
    if ((NULL == vent) || (NULL == key_type) || (INVALIDm == mem)) {
        return (BCM_E_PARAM);
    }

    key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf); 
    glp_wildcard = (soc_mem_field32_get(unit, mem, vent, GLPf) == 
                    BCM_VLAN_XLATE_GLP_WILDCARD);

    switch (key_val) {
      case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyDouble;
              } else {

              }   *key_type = bcmVlanTranslateKeyPortDouble;
              break;
          }
      case TR_VLXLT_HASH_KEY_TYPE_OTAG:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyOuterTag;
              } else {
                  *key_type = bcmVlanTranslateKeyPortOuterTag;
              }
              break;
          }
      case TR_VLXLT_HASH_KEY_TYPE_ITAG:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyInnerTag;
              } else {
                  *key_type = bcmVlanTranslateKeyPortInnerTag;
              }
              break;
          }
      case TR_VLXLT_HASH_KEY_TYPE_OVID:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyOuter;
              } else {
                  *key_type = bcmVlanTranslateKeyPortOuter;
              }
              break;
          }
      case TR_VLXLT_HASH_KEY_TYPE_IVID: 
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyInner;
              } else {
                  *key_type = bcmVlanTranslateKeyPortInner;
              }
              break;
          }
    }

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_parse
 * Description   :
 *      Helper function for an API to parse a vlan translate 
 *      entry for Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translation memory id.
 *      vent            (IN) HW entry.
 *      trvs_info       (IN/OUT) Traverse structure that contain all relevant info
 */
int 
_bcm_trx_vlan_translate_parse(int unit, soc_mem_t mem, uint32 *vent,
                             _bcm_vlan_translate_traverse_t *trvs_info)
{
    uint32   profile_idx = 0;     /* Vlan profile index.      */
    int rv;                       /* Operation return status. */

    profile_idx = soc_mem_field32_get(unit, mem, vent,
                                      TAG_ACTION_PROFILE_PTRf); 
    if (mem == EGR_VLAN_XLATEm) {
        _bcm_trx_egr_vlan_action_profile_entry_get(unit, 
                                                   trvs_info->action, 
                                                   profile_idx);
        trvs_info->port_class = soc_mem_field32_get(unit, mem, vent, PORT_GROUP_IDf);
        trvs_info->key_type = bcmVlanTranslateKeyPortDouble;
    } else {
        _bcm_trx_vlan_action_profile_entry_get(unit, trvs_info->action, 
                                               profile_idx);

        rv =  _bcm_trx_vlan_translate_key_type_get(unit, mem, vent,
                                                   &(trvs_info->key_type));
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_trx_vlan_translate_gport_get(unit, mem, vent, 
                                               &(trvs_info->gport));
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = _bcm_trx_vlan_translate_entry_parse(unit, mem, vent,
                                             trvs_info->action);
    BCM_IF_ERROR_RETURN(rv);


    rv = _bcm_trx_vlan_translate_old_vlan_get(unit, mem, vent, 
                                              trvs_info->key_type,
                                              &(trvs_info->outer_vlan), 
                                              &(trvs_info->inner_vlan));

    return BCM_E_NONE;
}

/*
*       VLAN PORT PROTOCOL 
*/

/*
 * Function:
 *      _bcm_trx_vlan_port_protocol_entry_parse
 * Purpose:
 *      Parses VLAN protocol entry and extracts frame type and ethertype
 * Parameters:
 *      unit        -     (IN) BCM Device unit
 *      vpe         -     (IN) VLAN protocol entry to parse
 *      frame       -     (OUT) Frame type from the entry
 *      ether       -     (IN) Ethertype from the entry
 * Returns:
 *      None
 */
STATIC void
_bcm_trx_vlan_port_protocol_entry_parse(int unit, vlan_protocol_entry_t *vpe,
                                        bcm_port_frametype_t *frame,
                                        bcm_port_ethertype_t *ether)
{
    *frame = 0;
    if (soc_VLAN_PROTOCOLm_field32_get(unit, vpe, ETHERIIf)) {
        *frame |= BCM_PORT_FRAMETYPE_ETHER2;
    }
    if (soc_VLAN_PROTOCOLm_field32_get(unit, vpe, SNAPf)) {
        *frame |= BCM_PORT_FRAMETYPE_8023;
    }
    if (soc_VLAN_PROTOCOLm_field32_get(unit, vpe, LLCf)) {
        *frame |= BCM_PORT_FRAMETYPE_LLC;
    }
    *ether = soc_VLAN_PROTOCOLm_field32_get(unit, vpe, ETHERTYPEf);
}

/*
 * Function:
 *      _bcm_trx_vlan_port_prot_match_get
 * Purpose:
 *      Get index into table with settings matching to port protocol
 * Parameters:
 *      unit        -     (IN) BCM Device unit
 *      frame       -     (IN) Frmae to match
 *      ether       -     (IN) Ethertype to match
 *      match_idx   -     (OUT) index to the table entry if match, -1 otherwise
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_port_prot_match_get(int unit, bcm_port_frametype_t frame, 
                                  bcm_port_ethertype_t ether, int *match_idx)
{
    int                         i, idxmin, idxmax;
    vlan_protocol_entry_t       vpe;
    bcm_port_frametype_t        ft;
    bcm_port_ethertype_t        et;

    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
    
    *match_idx = -1;

    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN
            (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        _bcm_trx_vlan_port_protocol_entry_parse(unit, &vpe, &ft, &et);
        if (ft == frame && et == ether) {
            *match_idx = i;
            break;
        }
    }

    return (*match_idx < 0) ?  BCM_E_NOT_FOUND : BCM_E_NONE ;
}

/*
 * Function:
 *      _bcm_trx_vlan_port_prot_match_get
 * Purpose:
 *      Get index into first empty entry in the table 
 * Parameters:
 *      unit        -     (IN) BCM Device unit
 *      empty_idx   -     (OUT) index to the table entry if match, -1 otherwise
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_port_prot_empty_get(int unit, int *empty_idx)
{
    int                         i, idxmin, idxmax;
    vlan_protocol_entry_t       vpe;
    bcm_port_frametype_t        ft;
    
    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
    
    *empty_idx = -1;

    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN
            (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        ft = 0;
        if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, ETHERIIf)) {
            ft |= BCM_PORT_FRAMETYPE_ETHER2;
        }
        if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, SNAPf)) {
            ft |= BCM_PORT_FRAMETYPE_8023;
        }
        if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, LLCf)) {
            ft |= BCM_PORT_FRAMETYPE_LLC;
        }
        if (0 == ft) {
            *empty_idx = i;
            break;
        }
    }

    return (*empty_idx < 0) ?  BCM_E_FULL : BCM_E_NONE ;
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_entry_set
 * Description   :
 *      Helper function to set vlan_protocol_data entry
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      ventry          (IN / OUT) Entry to setup
 *      action          (IN) vlan action to be used
 *      profile_idx     (IN) profile index to set
 */
STATIC void 
_bcm_trx_vlan_protocol_data_entry_set(int unit, 
                                      vlan_protocol_data_entry_t *ventry, 
                                      bcm_vlan_action_set_t *action,
                                      int profile_idx)
{
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, OVIDf, 
                                        action->new_outer_vlan);
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, IVIDf, 
                                        action->new_inner_vlan);
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, PRIf, action->priority);
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, TAG_ACTION_PROFILE_PTRf,
                                        profile_idx);
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_entry_parse
 * Description   :
 *      Helper function to parse vlan_protocol_data entry into action
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      ventry          (IN) Entry to setup
 *      action          (OUT) vlan action to be used
 *      profile_idx     (OUT) profile index 
 */
STATIC void 
_bcm_trx_vlan_protocol_data_entry_parse(int unit, 
                                        vlan_protocol_data_entry_t *ventry, 
                                        bcm_vlan_action_set_t *action,
                                        int *profile_idx)
{
    action->new_outer_vlan = 
        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, OVIDf);

    action->new_inner_vlan = 
        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, IVIDf); 

    action->priority = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, PRIf);
    *profile_idx = 
        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, 
                                            TAG_ACTION_PROFILE_PTRf);
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_update
 * Description   :
 *      Helper function for an API to update lan port protocol data memory
 *      for specific ports.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      update_pbm      (IN) BCM port bitmap to update
 *      prot_idx        (IN) Index to Port Protocol table
 *      action          (IN) VLAN action to be used to update all ports in pbmp
 */
STATIC int
_bcm_trx_vlan_protocol_data_update(int unit, bcm_pbmp_t update_pbm, int prot_idx,
                                  bcm_vlan_action_set_t *action)
{
    bcm_port_t                  p;
    vlan_protocol_data_entry_t  vde;
    int                         data_idx, profile_idx;
    int                         c_profile_idx, use_default = 0;
    bcm_vlan_action_set_t       def_action, *action_p;
    _bcm_port_info_t            *pinfo;
    

    /* if action is NULL then default action should be used for each port */
    if (NULL == action) {
        use_default = 1;
        action_p = &def_action;
    } else {
        action_p = action;
    }

    BCM_PBMP_ITER(update_pbm, p) {
        BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
        data_idx = pinfo->vlan_prot_ptr + prot_idx;

        if (use_default) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_vlan_port_default_action_get(unit, p, action_p));
        }
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_profile_entry_add(unit, action_p, 
                                                   (uint32 *)&profile_idx));
        BCM_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, data_idx, &vde));

        c_profile_idx =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                                                TAG_ACTION_PROFILE_PTRf);
        sal_memset(&vde, 0, sizeof(vde));
        _bcm_trx_vlan_protocol_data_entry_set(unit, &vde, action_p, 
                                              profile_idx);
        BCM_IF_ERROR_RETURN(
            WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL, data_idx, &vde));
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_profile_entry_delete(unit, c_profile_idx));
    }
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_entry_delete_by_idx
 * Description   :
 *      Helper function to delete a vlan_protocol_data entry from memory by 
 *      protocol index
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Port
 *      vp_idx          (IN) Index to protcol id
 */
STATIC int 
_bcm_trx_vlan_protocol_data_entry_delete_by_idx(int unit, bcm_port_t port, 
                                                int vp_idx) 
{
    bcm_pbmp_t                  switched_pbm, update_pbm;
    bcm_port_config_t           pconf;
    _bcm_port_info_t            *pinfo;
    bcm_port_t                  p;
    int                         in_use = 0;
    vlan_protocol_entry_t       *vpnull;

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pconf));

    BCM_PBMP_CLEAR(update_pbm);
    BCM_PBMP_PORT_ADD(update_pbm, port);
    
    switched_pbm = pconf.e; 
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(switched_pbm, pconf.cpu);
    }

    BCM_PBMP_ITER(switched_pbm, p) {
        if (p == port) {    /* skip the port we just added */
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
        /* Check for explicit VID entry */
        if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, vp_idx)) {
            BCM_PBMP_PORT_ADD(update_pbm, p);
        } else {
            in_use = 1;
        }
    }

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_protocol_data_update(unit, update_pbm, vp_idx, NULL));
    if (!in_use) {
        vpnull = soc_mem_entry_null(unit, VLAN_PROTOCOLm); 
        WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, vp_idx, vpnull);
    }

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_action_add
 * Description   :
 *      Helper function for an API to add vlan port protocol action
 *      For TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 *      frame           (IN) Frame type
 *      ethr            (IN) Ethertype 
 *      action          (IN) vlan action to be used
 */
int
_bcm_trx_vlan_port_protocol_action_add(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    
    bcm_port_t                  p;
    vlan_protocol_entry_t       vpe;
    vlan_protocol_data_entry_t  vde;
    int                         vpentry, vdentry, empty, rv_m, rv_e;
    bcm_pbmp_t                  switched_pbm, update_pbm, clear_pbm;
    _bcm_port_info_t            *pinfo;
    bcm_port_config_t           pconf;
    bcm_vlan_t                  c_ovid, c_ivid;

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    rv_m = _bcm_trx_vlan_port_prot_match_get(unit, frame, ether, &vpentry);
    rv_e = _bcm_trx_vlan_port_prot_empty_get(unit, &empty);

    if ((BCM_E_NOT_FOUND == rv_m)  && (BCM_E_FULL == rv_e)) {
        return BCM_E_FULL;
    }
    if (BCM_E_NOT_FOUND == rv_m) {
        sal_memset(&vpe, 0, sizeof(vpe));
        soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, ETHERTYPEf, ether);
        if (frame & BCM_PORT_FRAMETYPE_ETHER2) {
            soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, ETHERIIf, 1);
        }
        if (frame & BCM_PORT_FRAMETYPE_8023) {
            soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, SNAPf, 1);
        }
        if (frame & BCM_PORT_FRAMETYPE_LLC) {
            soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, LLCf, 1);
        }
        soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, MATCHUPPERf, 1);
        soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, MATCHLOWERf, 1);
        /* Actual writing into the memory will be last in this routine */
        vpentry = empty;

#ifdef LVL7_FIXUP
	BCM_IF_ERROR_RETURN(
           WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, empty, &vpe));
#endif
    }

    /*
     * Set VLAN ID for target port. For all other ethernet ports,
     * make sure entries indexed by the matched entry in VLAN_PROTOCOL
     * have initialized values (either default or explicit VID).
     */
    BCM_PBMP_CLEAR(update_pbm);
    BCM_PBMP_CLEAR(clear_pbm);
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_config_get(unit, &pconf));
    switched_pbm = pconf.e;
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(switched_pbm, pconf.cpu);
    }

    BCM_PBMP_ITER(switched_pbm, p) {
        BCM_IF_ERROR_RETURN(
            _bcm_port_info_get(unit, p, &pinfo));
        vdentry = pinfo->vlan_prot_ptr + vpentry;
        BCM_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, vdentry, &vde));
        c_ovid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, OVIDf);
        c_ivid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, IVIDf);
        if (p == port) {
            if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, vpentry)) {
                return BCM_E_EXISTS;
            }
            /* Set as explicit VID */
            _BCM_PORT_VD_PBVL_SET(pinfo, vpentry);
            if ((c_ovid == action->new_outer_vlan) &&
                (c_ivid == action->new_inner_vlan)) {
                /* Current (default) VID same as explicit VID */
                continue;
            }
            BCM_PBMP_PORT_ADD(update_pbm, p);
        } else {
            /*
             * For all other ports, if no non-zero VID already set,
             * set to default VLAN ID.
             */
            if (c_ovid != 0) {
                continue;
            }
            BCM_PBMP_PORT_ADD(clear_pbm, p);
            _BCM_PORT_VD_PBVL_CLEAR(pinfo, vpentry);

        }
    }

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_protocol_data_update(unit, clear_pbm, vpentry, NULL));
    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_protocol_data_update(unit, update_pbm, vpentry, action));

#ifdef LVL7_FIXUP
    return BCM_E_NONE;
#else
    return WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, empty, &vpe); 
#endif
}

/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_action_get
 * Description   :
 *      Helper function for an API to get vlan port protocol action
 *      For TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 *      frame           (IN) Frame type
 *      ethr            (IN) Ethertype 
 *      action          (OUT) vlan action to get
 */
int
_bcm_trx_vlan_port_protocol_action_get(int unit, bcm_port_t port, 
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    int                             match_prot_idx, match_data_idx, profile_idx; 
    vlan_protocol_data_entry_t      vde;
    _bcm_port_info_t                *pinfo;

    if (NULL == action) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_port_prot_match_get(unit, frame, ether, &match_prot_idx));

    BCM_IF_ERROR_RETURN(
        _bcm_port_info_get(unit, port, &pinfo));

    match_data_idx = pinfo->vlan_prot_ptr + match_prot_idx;
    
    BCM_IF_ERROR_RETURN(
        READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, match_data_idx, &vde));

    _bcm_trx_vlan_protocol_data_entry_parse(unit, &vde, action, &profile_idx);
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx); 

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_delete
 * Description   :
 *      Helper function for an API to delete vlan port protocol action
 *      For TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 *      frame           (IN) Frame type
 *      ethr            (IN) Ethertype 
 */

int
_bcm_trx_vlan_port_protocol_delete(int unit, bcm_port_t port,
                                  bcm_port_frametype_t frame,
                                  bcm_port_ethertype_t ether)
{
    int                 vpentry, rv;
    _bcm_port_info_t    *pinfo;

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_port_prot_match_get(unit, frame, ether, &vpentry));

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));

    if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, vpentry)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_trx_vlan_protocol_data_entry_delete_by_idx(unit, port, vpentry);
    if (BCM_SUCCESS(rv)) {
        _BCM_PORT_VD_PBVL_CLEAR(pinfo, vpentry);
    }

    return rv;
}


/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_delete_all
 * Description   :
 *      Helper function for an API to delete all actions for all 
 *      vlan port protocols for TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 */
int
_bcm_trx_vlan_port_protocol_delete_all(unit, port)
{
    int      i, idxmin, idxmax, rv;
    _bcm_port_info_t    *pinfo;

    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    
    rv = BCM_E_NONE;
    
    for (i = idxmin; i <= idxmax; i++) {
        if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) {
            rv = _bcm_trx_vlan_protocol_data_entry_delete_by_idx(unit, port, i);
            if (BCM_FAILURE(rv)) {
                break;
            } else {
                _BCM_PORT_VD_PBVL_CLEAR(pinfo, i);
            }
        }
    }

    return rv;
}


/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_action_traverse
 * Description   :
 *      Helper function for an API to traverse over all  
 *      vlan port protocols and call given callback routine for TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) user callback function
 *      user_data       (IN) pointer to user data
 */
int 
_bcm_trx_vlan_port_protocol_action_traverse(int unit,
                                   bcm_vlan_port_protocol_action_traverse_cb cb,
                                           void *user_data)
{
    int                         idxmin, idxmax, i, data_idx, profile_idx;
    vlan_protocol_entry_t       vpe;
    vlan_protocol_data_entry_t  vde;
    bcm_port_frametype_t        ft;
    bcm_port_ethertype_t        et;
    bcm_pbmp_t                  pbmp;
    bcm_port_t                  port;
    bcm_vlan_action_set_t       def_action, action;
    bcm_vlan_t                  def_ovid, def_ivid;
    bcm_port_config_t           pconf;
    _bcm_port_info_t            *pinfo;

    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);

    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pconf));
    pbmp = pconf.e; 
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(pbmp, pconf.cpu);
    }
    
    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        _bcm_trx_vlan_port_protocol_entry_parse(unit, &vpe, &ft, &et);
        if (0 == ft) {
            continue;
        }
        PBMP_ITER(pbmp, port) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_vlan_port_default_action_get(unit, port, &def_action));
            BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
            data_idx = pinfo->vlan_prot_ptr + i;
            SOC_IF_ERROR_RETURN(
                READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, data_idx, &vde));
            def_ovid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, OVIDf);
            def_ivid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, IVIDf);
            if (def_ivid == def_action.new_inner_vlan &&
                def_ovid == def_action.new_outer_vlan) {
                continue;
            }
            profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                                                TAG_ACTION_PROFILE_PTRf);
            action.new_inner_vlan = def_ivid;
            action.new_outer_vlan = def_ovid;
            action.priority = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                                                                  PRIf);
            /* Read action profile data. */
            _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);
            BCM_IF_ERROR_RETURN(cb(unit, port, ft, et, &action, user_data));
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_port_default_action_set
 * Purpose:
 *      Set the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_default_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    uint32 profile_idx, old_profile_idx, vp_profile_idx;
    bcm_vlan_t old_outer_vlan, old_inner_vlan;
    bcm_vlan_t vp_outer_vlan, vp_inner_vlan;
    bcm_port_cfg_t pcfg;
    vlan_protocol_data_entry_t vde;
    int vlan_prot_entries, vlan_data_prot_start, i;
    _bcm_port_info_t *pinfo;

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    
    old_outer_vlan = pcfg.pc_vlan;
    old_inner_vlan = pcfg.pc_ivlan;
    old_profile_idx = pcfg.pc_vlan_action;
    pcfg.pc_vlan = action->new_outer_vlan;
    pcfg.pc_ivlan = action->new_inner_vlan;
    pcfg.pc_vlan_action = profile_idx;
    pcfg.pc_new_pri = action->priority;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

    SOC_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx));

    /*
     * Update default VLAN ID in VLAN_PROTOCOL_DATA
     */
    vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    vlan_data_prot_start = pinfo->vlan_prot_ptr;

    
    for (i = 0; i < vlan_prot_entries; i++) {

        BCM_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                                     vlan_data_prot_start + i, &vde));
        /*
         * Update entry with new default VLAN ID if not a programmed
         * VLAN ID and entry has previous default VLAN ID
         */
        vp_outer_vlan = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                             OVIDf);
        vp_inner_vlan = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                             IVIDf);
        vp_profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                             TAG_ACTION_PROFILE_PTRf);

        if ( (!(_BCM_PORT_VD_PBVL_IS_SET(pinfo, i))) &&
             (vp_outer_vlan == old_outer_vlan) &&
             (vp_inner_vlan == old_inner_vlan)) {

            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_action_profile_entry_add(unit, action, 
                                                       &profile_idx));
            _bcm_trx_vlan_protocol_data_entry_set(unit, &vde, action, 
                                                  profile_idx);

            BCM_IF_ERROR_RETURN(
                WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL,
                                           vlan_data_prot_start + i, &vde));
            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_action_profile_entry_delete(unit, 
                                                          vp_profile_idx));
        }
    }
    
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_port_default_action_get
 * Purpose:
 *      Get the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */ 
int 
_bcm_trx_vlan_port_default_action_get(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    uint32 profile_idx;
    bcm_port_cfg_t pcfg;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    profile_idx = pcfg.pc_vlan_action;

    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);
    
    action->new_outer_vlan = pcfg.pc_vlan;
    action->new_inner_vlan = pcfg.pc_ivlan;
    action->priority = pcfg.pc_new_pri;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tr_vlan_port_default_action_delete
 * Purpose:
 *      Deletes the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */ 
int 
_bcm_trx_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    uint32                      profile_idx;
    bcm_port_cfg_t              pcfg;
    _bcm_port_info_t            *pinfo;
    int                         num_ent, start, i;
    vlan_protocol_data_entry_t  vde;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    profile_idx = pcfg.pc_vlan_action;

    _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    
    BCM_IF_ERROR_RETURN(
        _bcm_port_info_get(unit,port, &pinfo));

    num_ent = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    start = pinfo->vlan_prot_ptr;
    for (i = 0; i < num_ent; i++) {
        /* Update entry with all 0 */
        sal_memset(&vde, 0, sizeof(vlan_protocol_data_entry_t));
        BCM_IF_ERROR_RETURN(
            WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL, start + i, &vde));
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_tr_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 rval, profile_idx, old_profile_idx;

    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_add(unit, action, &profile_idx));

    rv = READ_EGR_VLAN_CONTROL_2r(unit, port, &rval);
    if (rv < 0) {
        goto error;
    }
    if (action->priority < 0) {
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, ORPEf, 0);
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRIf, 0);
    } else if (action->priority < 8){
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, ORPEf, 1);
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, 
                          OPRIf, action->priority);
    } else {
        rv = BCM_E_PARAM;
        goto error;
    }
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval,
                      OVIDf, action->new_outer_vlan);
    rv = WRITE_EGR_VLAN_CONTROL_2r(unit, port, rval);
    if (rv < 0) {
        goto error;
    }

    rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
    if (rv < 0) {
        goto error;
    }   
    old_profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                        rval, TAG_ACTION_PROFILE_PTRf);
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                      TAG_ACTION_PROFILE_PTRf, profile_idx);
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                      IVIDf, action->new_inner_vlan);
    if (action->priority < 0) {
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IRPEf, 0);
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRIf, 0);
    } else {
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IRPEf, 1);
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, 
                          IPRIf, action->priority);
    }
    rv = WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval);
    if (rv < 0) {
        goto error;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_delete(unit, old_profile_idx));

    return BCM_E_NONE;

error:
    /* Undo action profile entry addition */
    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx));
    return rv;
}

/*
 * Function:
 *      _bcm_tr_vlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    uint32 profile_idx, rval;

    bcm_vlan_action_set_t_init(action);

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
    profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                    rval, TAG_ACTION_PROFILE_PTRf);
    _bcm_trx_egr_vlan_action_profile_entry_get(unit, action, profile_idx);
    action->new_inner_vlan = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                               rval, IVIDf);

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_2r(unit, port, &rval));
    action->new_outer_vlan = soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r,
                                               rval, OVIDf);
    if (soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, ORPEf)) {
        action->priority = soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r,
                                             rval, OPRIf);
    } else {
        action->priority = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_vlan_port_egress_default_action_delete
 * Purpose:
 *      Deletes the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    uint32                  profile_idx, rval;

    BCM_IF_ERROR_RETURN(
        READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
    profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                    rval, TAG_ACTION_PROFILE_PTRf);
    
    BCM_IF_ERROR_RETURN(
        _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx));
    sal_memset(&rval, 0, sizeof(uint32));
    BCM_IF_ERROR_RETURN(
        WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval));
    BCM_IF_ERROR_RETURN(
        WRITE_EGR_VLAN_CONTROL_2r(unit, port, rval));


    return BCM_E_NONE;
}


#else /* BCM_TRX_SUPPORT */
int _triumph_vlan_not_empty;
#endif  /* BCM_TRX_SUPPORT */

