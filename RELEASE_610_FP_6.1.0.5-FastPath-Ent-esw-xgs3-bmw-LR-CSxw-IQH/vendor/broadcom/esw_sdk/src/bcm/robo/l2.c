/*
 * $Id: l2.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * L2 - Broadcom RoboSwitch Layer-2 switch API.
 */
#include <sal/types.h>
#include <sal/appl/io.h>

#include <soc/mem.h>
#include <soc/drv.h>

#include <soc/feature.h>
#include <soc/macipadr.h>
#include <soc/debug.h>
#include <soc/arl.h>
#include <soc/mcm/robo/memregs.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/l2.h>
#include <bcm/trunk.h>
#include <bcm/vlan.h>
#include <bcm/stack.h>
#include <bcm_int/robo/l2.h>
#include <bcm_int/robo/trunk.h>
#include <bcm_int/robo/port.h>
/* 
 * Function:
 *	_bcm_robo_l2_to_arl
 * Purpose:
 *	Translate hardware-independent L2 entry to ROBO-specific ARL entry
 * Parameters:
 *	unit - Unit number
 *	arl_entry - (OUT) ROBO ARL entry
 *	l2addr - Hardware-independent L2 entry
 */
int
_bcm_robo_l2_to_arl(int unit, l2_arl_sw_entry_t *arl_entry, bcm_l2_addr_t *l2addr)
{
    
    uint64 mac_field;
    uint32 fld_value;
    uint32 no_support_flag;

    sal_memset(arl_entry, 0, sizeof (*arl_entry));

    soc_cm_debug(DK_ARL, "in _bcm_robo_l2_to_arl, unit is %d\n", unit);

    if (l2addr->flags & BCM_L2_REPLACE_DYNAMIC) {
       no_support_flag = BCM_L2_DISCARD_SRC | BCM_L2_DISCARD_DST |BCM_L2_COPY_TO_CPU |
           BCM_L2_L3LOOKUP | BCM_L2_TRUNK_MEMBER;
    } else {
    no_support_flag = BCM_L2_DISCARD_SRC | BCM_L2_DISCARD_DST |BCM_L2_COPY_TO_CPU |
        BCM_L2_L3LOOKUP | BCM_L2_TRUNK_MEMBER | BCM_L2_REPLACE_DYNAMIC;
    }

    if (soc_feature(unit, soc_feature_arl_mode_control)) {
        no_support_flag &= ~BCM_L2_DISCARD_SRC;
        no_support_flag &= ~BCM_L2_DISCARD_DST;
        no_support_flag &= ~BCM_L2_COPY_TO_CPU;
    }

    if (l2addr->flags & no_support_flag) {
        return BCM_E_UNAVAIL;
    }

    SAL_MAC_ADDR_TO_UINT64(l2addr->mac, mac_field);
    /* set MAC field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)(unit, DRV_MEM_ARL, 
            DRV_MEM_FIELD_MAC, (uint32 *)arl_entry, (uint32 *)&mac_field));

    fld_value = l2addr->vid; /* vid value */
     /* set VID field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)(unit, DRV_MEM_ARL,
            DRV_MEM_FIELD_VLANID, (uint32 *)arl_entry, &fld_value));

    fld_value = l2addr->cos_dst; /* priority value */
     /* set priority field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)(unit, DRV_MEM_ARL,
            DRV_MEM_FIELD_PRIORITY, (uint32 *)arl_entry, &fld_value));

    /* set static field */
    if (l2addr->flags & BCM_L2_STATIC) {
        fld_value = 0x1; /* static value */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC, 
                (uint32 *)arl_entry, &fld_value));
        soc_cm_debug(DK_ARL, "\t Static flags set in l2addr\n");
    }

    fld_value = 0x1; /* valid value */
    /* set valid field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)(unit, DRV_MEM_ARL,
            DRV_MEM_FIELD_VALID, (uint32 *)arl_entry, &fld_value));

    if (l2addr->flags & BCM_L2_HIT) {
        fld_value = 0x1; /* age value */
        /* set hit/age field */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_AGE, 
                (uint32 *)arl_entry, &fld_value));
        soc_cm_debug(DK_ARL,  "\t Age flags set in l2addr\n");
    }

    if (soc_feature(unit, soc_feature_arl_mode_control)) {

        if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
            fld_value = 0x3; 
        } else if (l2addr->flags & BCM_L2_DISCARD_DST) {
            fld_value = 0x1; 
        }else if (l2addr->flags & BCM_L2_DISCARD_SRC) {
            fld_value = 0x2;
        }else {
            fld_value = 0; 
        }

        /* set arl_control field */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_ARL_CONTROL, 
                (uint32 *)arl_entry, &fld_value));
        soc_cm_debug(DK_ARL,  "\t arl_control flags set in l2addr %x\n",fld_value);

    }

    fld_value = l2addr->port;
    soc_cm_debug(DK_ARL, "in _bcm_robo_l2_to_arl, port is %d\n",
            fld_value);
    /* set Port fields */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)(unit, DRV_MEM_ARL,
            DRV_MEM_FIELD_SRC_PORT,(uint32 *)arl_entry, &fld_value));

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_robo_l2_from_arl
 * Purpose:
 *	Translate ROBO-specific ARL entry to hardware-independent L2 entry
 * Parameters:
 *	unit - Unit number
 *	l2addr - (OUT) hardware-independent L2 entry
 *	arl_entry - ROBO ARL entry
 */
void
_bcm_robo_l2_from_arl(int unit, bcm_l2_addr_t *l2addr, l2_arl_sw_entry_t *arl_entry)
{
    uint64 mac_field;
    uint32 field_value;

    sal_memset(l2addr, 0, sizeof (*l2addr));

    /* Get MAC field */
    (DRV_SERVICES(unit)->mem_field_get)(unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC,
            (uint32 *)arl_entry, (uint32 *)&mac_field);

    SAL_MAC_ADDR_FROM_UINT64(l2addr->mac, mac_field);

    soc_cm_debug(DK_ARL, "in _bcm_robo_l2_from_arl ,\
        mac  is %02x-%02x-%02x-%02x-%02x-%02x \n",
        l2addr->mac[0], l2addr->mac[1],l2addr->mac[2], l2addr->mac[3],
        l2addr->mac[4], l2addr->mac[5]);

    /* Get VID field */
    (DRV_SERVICES(unit)->mem_field_get)(unit, DRV_MEM_ARL,
            DRV_MEM_FIELD_VLANID, (uint32 *)arl_entry, &field_value);
    l2addr->vid = field_value;
    soc_cm_debug(DK_ARL, "in _bcm_robo_l2_from_arl, l2addr->vid is %d\n",
        l2addr->vid);

    /* Get static field */
    (DRV_SERVICES(unit)->mem_field_get)(unit, DRV_MEM_ARL,
            DRV_MEM_FIELD_STATIC, (uint32 *)arl_entry, &field_value);
    if (field_value) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (l2addr->mac[0] & 0x01) { /* mcast address */
        l2addr->flags |= BCM_L2_MCAST;
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_DEST_BITMAP, 
            (uint32 *)arl_entry, &field_value);
        if (field_value) {
            l2addr->l2mc_index = (int)field_value;               
        }
    soc_cm_debug(DK_ARL, "in _bcm_robo_l2_from_arl, l2mc_index is %d\n",
        l2addr->l2mc_index);

    } else {
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, 
            (uint32 *)arl_entry, &field_value);
        l2addr->port = field_value;

        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_PRIORITY, 
            (uint32 *)arl_entry, &field_value);
        l2addr->cos_dst = field_value;
  
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_AGE, 
            (uint32 *)arl_entry, &field_value);
        if (field_value) {
            l2addr->flags |= BCM_L2_HIT;
        }
        if (soc_feature(unit, soc_feature_arl_mode_control)) {
            /* Get arl control field */
            (DRV_SERVICES(unit)->mem_field_get)(unit, DRV_MEM_ARL, 
                    DRV_MEM_FIELD_ARL_CONTROL,
                    (uint32 *)arl_entry, &field_value);

            if (field_value == 0x1) {
                l2addr->flags |= BCM_L2_DISCARD_DST;
            }else if(field_value == 0x2){
                l2addr->flags |= BCM_L2_DISCARD_SRC;        
            }else if(field_value == 0x3){
                l2addr->flags |= BCM_L2_COPY_TO_CPU;
            }
        }

    soc_cm_debug(DK_ARL, "in _bcm_robo_l2_from_arl, port is %d cos_dst %x\n",
        l2addr->port, l2addr->cos_dst);

    }    
    /* Valid bit is ignored here; entry is assumed valid */
}


/****************************************************************************
 *
 * L2 Message Registration
 *
 ****************************************************************************/

static bcm_l2_addr_callback_t _bcm_l2_cbs[SOC_MAX_NUM_SWITCH_DEVICES];
static void *_bcm_l2_cb_data[SOC_MAX_NUM_SWITCH_DEVICES];

/*
 * Function:
 *	_bcm_robo_l2_register_callback
 * Description:
 *	Call back to handle bcm_l2_addr_register clients. 
 * Parameters:
 *	unit - RoboSwitch unit number.
 *	entry_del - Entry to be deleted or updated, NULL if none.
 *	entry_add - Entry to be inserted or updated, NULL if none.
 *	fn_data - unused
 * Notes:
 *	Only one callback per unit to the bcm layer is supported here. 
 *	Multiple bcm client callbacks per unit are supported in the bcm layer. 
 */

static void
_bcm_robo_l2_register_callback(int unit,
              l2_arl_sw_entry_t *entry_del,
              l2_arl_sw_entry_t *entry_add,
              void *fn_data)
{

     #ifdef LVL7_FIXUP
     static    char flag_inuse=0;
     if(flag_inuse == 1)
     {
        return;
     }
     flag_inuse = 1;
    #endif


    
    if (_bcm_l2_cbs[unit] != NULL) {
        bcm_l2_addr_t l2addr_del, l2addr_add;
            uint32 flags = 0; /* Common flags: Move, From/to native */
        
            /* First, set up the entries:  decode HW entries and set flags */
        if (entry_del != NULL) {
            _bcm_robo_l2_from_arl(unit, &l2addr_del, entry_del);
        }
        if (entry_add != NULL) {
            _bcm_robo_l2_from_arl(unit, &l2addr_add, entry_add);
        }
        
        if ((entry_del != NULL) && (entry_add != NULL)) { /* It's a move */
                flags |= BCM_L2_MOVE;
                if (bcm_l2_port_native(unit, l2addr_del.modid,
                                       l2addr_del.port) > 0) {
                    flags |= BCM_L2_FROM_NATIVE;
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
                if (bcm_l2_port_native(unit, l2addr_add.modid,
                                       l2addr_add.port) > 0) {
                    flags |= BCM_L2_TO_NATIVE;
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
                l2addr_del.flags |= flags;
                l2addr_add.flags |= flags;
        } else if (entry_del != NULL) { /* Age out or simple delete */
                if (bcm_l2_port_native(unit, l2addr_del.modid,
                                       l2addr_del.port) > 0) {
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
        } else if (entry_add != NULL) { /* Insert or learn */
                if (bcm_l2_port_native(unit, l2addr_add.modid,
                                       l2addr_add.port) > 0) {
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
            }
        
        /* The entries are now set up.  Make the callbacks */
        if (entry_del != NULL) {
            _bcm_l2_cbs[unit](unit, &l2addr_del, 0, _bcm_l2_cb_data[unit]);
        }
        if (entry_add != NULL) {
            _bcm_l2_cbs[unit](unit, &l2addr_add, 1, _bcm_l2_cb_data[unit]);
        }
    }
  #ifdef LVL7_FIXUP
  flag_inuse = 0;
  #endif
 }

/*
 * Function:
 *	_bcm_robo_l2_bpdu_init
 * Description:
 *	Initialize all BPDU addresses to recognize the 802.1D
 *      Spanning Tree address on all chips.
 * Parameters:
 *	unit - unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 * Notes:
 */

static int
_bcm_robo_l2_bpdu_init(int unit)
{
    int		i, nbpdu;
    mac_addr_t	mac;
    pbmp_t null_pbmp;

    nbpdu = bcm_l2_bpdu_count(unit);

    /* Spanning Tree addr (01:80:c2:00:00:00) used as default entries */
    mac[0] = 0x01;
    mac[1] = 0x80;
    mac[2] = 0xc2;
    mac[3] = mac[4] = mac[5] = 0x00;

    for (i = 0; i < nbpdu; i++) {
	BCM_PBMP_CLEAR(null_pbmp);
	BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mac_set)
            (unit, null_pbmp, DRV_MAC_CUSTOM_BPDU, mac, i));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_53xx_l2_addr_register
 * Description:
 *	Register a callback routine that will be called whenever
 *	an entry is inserted into or deleted from the L2 address table.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	callback - Callback function of type bcm_l2_addr_callback_t.
 *	userdata - Arbitrary value passed to callback along with messages
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_53xx_l2_addr_register(int unit,
               bcm_l2_addr_callback_t callback,
               void *userdata)
{
    /* this should be managed properly from above */
    assert(_bcm_l2_cbs[unit] == NULL);
    assert(_bcm_l2_cb_data[unit] == NULL);
    
    _bcm_l2_cbs[unit] = callback;
    _bcm_l2_cb_data[unit] = userdata;
    
    /* return SOC_E_NONE; kram 2004-01-06*/
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_53xx_l2_addr_unregister
 * Description:
 *	Unregister a previously registered callback routine.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	callback - Same callback function used to register callback
 *	userdata - Same arbitrary value used to register callback
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Both callback and userdata must match from original register call.
 */

int
bcm_53xx_l2_addr_unregister(int unit,
                 bcm_l2_addr_callback_t callback,
                 void *userdata)
{
    /* this should be managed properly from above */
    assert(_bcm_l2_cbs[unit] == callback);
    assert(_bcm_l2_cb_data[unit] == userdata);
    _bcm_l2_cbs[unit] = _bcm_l2_cb_data[unit] = NULL;
    return BCM_E_NONE;
}



/*
 * Function:
 *	bcm_robo_l2_init
 * Description:
 *	Perform required initializations to L2 table.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_robo_l2_init(int unit)
{

    /* Turn off arl aging */
    BCM_IF_ERROR_RETURN(bcm_l2_age_timer_set(unit, 0));
    
    bcm_l2_detach(unit);

    /*
     * Init BPDU station address registers.
     */
    _bcm_robo_l2_bpdu_init(unit);

    soc_robo_arl_unregister(unit, _bcm_robo_l2_register_callback, NULL);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_clear)
            (unit, DRV_MEM_ARL));

    /* bcm_l2_register clients */
    soc_robo_arl_register(unit, _bcm_robo_l2_register_callback, NULL);

    return (BCM_E_NONE);
}

/*
 * Function:
 *	_bcm_robo_l2_term
 * Description:
 *	Finalize chip-dependent parts of L2 module
 * Parameters:
 *	unit - RoboSwitch unit number.
 */

int
_bcm_robo_l2_term(int unit)
{
    return SOC_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_robo_l2_detach
 * Purpose:
 *      Clean up l2 bcm layer when unit is detached
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_robo_l2_detach(int unit)
{
    return(_bcm_robo_l2_term(unit));
}


/*
 * Initialize a bcm_l2_addr_t to a specified MAC address and VLAN,
 * zeroing all other fields.
 */

/*
 * Function:
 *	bcm_robo_l2_addr_add
 * Description:
 *	Add a MAC address to the Switch Address Resolution Logic (ARL)
 *	port with the given VLAN ID and parameters.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Use CMIC_PORT(unit) to associate the entry with the CPU.
 *	Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 */

int 
bcm_robo_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    l2_arl_sw_entry_t arl_entry;
    uint32 flags = 0;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_add()..\n");
    /*
     * We don't check for valid port number because certain applications
     * require us to add entries with invalid port numbers.
     */

    BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_entry, l2addr));
    soc_cm_debug(DK_ARL, 
                "in bcm_robo_l2_addr_add , arl_entry is 0x%x,0x%x,0x%x\n",
                arl_entry.entry_data[2], 
                arl_entry.entry_data[1], 
                arl_entry.entry_data[0]);

    if (l2addr->flags & BCM_L2_REPLACE_DYNAMIC) {
        soc_cm_debug(DK_ARL, 
                "BCM API : bcm_robo_l2_addr_add()..BCM_L2_REPLACE_DYNAMIC\n");
        
        flags = DRV_MEM_OP_BY_HASH_BY_MAC | DRV_MEM_OP_BY_HASH_BY_VLANID | 
                DRV_MEM_OP_REPLACE;
    } else {
        flags = DRV_MEM_OP_BY_HASH_BY_MAC | DRV_MEM_OP_BY_HASH_BY_VLANID;    
    }
    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_insert)
            (unit, DRV_MEM_ARL, (uint32 *)&arl_entry,
             flags));

    return BCM_E_NONE;

}

/*
 * Function:
 *	bcm_robo_l2_addr_delete
 * Description:
 *	Remove an L2 address from the device's ARL
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	mac - MAC address to remove
 *	vid - associated VLAN ID
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */
int 
bcm_robo_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t   l2addr;
    l2_arl_sw_entry_t		arl_delete;
    int             rv;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete()..\n");
    bcm_l2_addr_init(&l2addr, mac, vid);

    BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_delete, &l2addr));

    rv =((DRV_SERVICES(unit)->mem_delete)
                    (unit, DRV_MEM_ARL,
                    (uint32 *)&arl_delete,
                    (DRV_MEM_OP_BY_HASH_BY_MAC | 
                            DRV_MEM_OP_BY_HASH_BY_VLANID|
                            DRV_MEM_OP_DELETE_BY_STATIC)));

    return rv;
}


/*
 * Function:
 *	bcm_robo_l2_addr_delete_by_port
 * Description:
 *	Remove all L2 (MAC) addresses associated with the port.
 * Parameters:
 *	unit  - RoboSwitch PCI device unit number (driver internal).
 *	mod   - module id (or -1 for local unit)
 *	pbmp  - bitmap of ports to affect
 *	flags - BCM_L2_REMOVE_XXX
 * Returns:
 *	BCM_E_NONE		Success.
 *	BCM_E_INTERNAL		Chip access failure.
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *
 *	ARL aging and learning on all ports is disabled during this
 *	operation.   If these weren't disabled, the hardware could
 *	shift around the contents of the ARL table during the remove
 *	operation, causing some entries that should be removed to remain
 *	in the table.
 */
int
bcm_robo_l2_addr_delete_by_port(int unit, bcm_module_t mod, bcm_port_t port,
            	uint32 flags)
{
    int                 rv = BCM_E_NONE;
    l2_arl_sw_entry_t         arl_entry;
    uint32              fld_value;
    uint32              action_flag = 0;

    sal_memset(&arl_entry, 0, sizeof (arl_entry));
    fld_value = port;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete_by_port()..\n");
    /* set port field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
              (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, 
              (uint32 *)&arl_entry, &fld_value));
    action_flag = DRV_MEM_OP_DELETE_BY_PORT ;
    action_flag |= (flags & BCM_L2_DELETE_STATIC) ? 
                    DRV_MEM_OP_DELETE_BY_STATIC : 0;
    rv =((DRV_SERVICES(unit)->mem_delete)
                    (unit, DRV_MEM_ARL,
                    (uint32 *)&arl_entry,
                    action_flag));
    return rv;
}


/*
 * Function:
 *	bcm_l2_addr_delete_by_mac
 * Description:
 *	Delete L2 entries associated with a MAC address.
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_robo_l2_addr_delete_by_mac(
	int	unit,
	bcm_mac_t	mac,
	uint32	flags)
{
    bcm_l2_addr_t   l2addr;      
    l2_arl_sw_entry_t		arl_search;     
    bcm_vlan_t      t_vid = VLAN_ID_INVALID;
    int         	rv = BCM_E_NONE;
    int         	i;
    uint32         action_flag = 0, vlan_count;        

    (DRV_SERVICES(unit)->dev_prop_get)
        (unit,DRV_DEV_PROP_VLAN_ENTRY_NUM, &vlan_count);

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete_by_mac()..\n vlan_count=%d\n",
            vlan_count);
    for (i = 1; i < vlan_count; i++) {
        t_vid = i;

        soc_cm_debug(DK_ARL, 
                "ARL_DEL: MAC=%02x:%02x:%02x:%02x:%02x:%02x, VID=0x%x\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                t_vid);
        /* prepare the l2 entry with mac+vid */   
        bcm_l2_addr_init(&l2addr, mac, t_vid);
        l2addr.flags |= (flags == BCM_L2_DELETE_STATIC) ? 
                        BCM_L2_STATIC : 0;
        BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_search, &l2addr));
    
        /* delete this l2 entry*/
        action_flag = DRV_MEM_OP_BY_HASH_BY_MAC | 
                    DRV_MEM_OP_BY_HASH_BY_VLANID;
        action_flag |= (flags == BCM_L2_DELETE_STATIC) ? 
                        DRV_MEM_OP_DELETE_BY_STATIC : 0;
        rv =((DRV_SERVICES(unit)->mem_delete)
                        (unit, DRV_MEM_ARL,
                        (uint32 *)&arl_search,
                        action_flag));
        soc_cm_debug(DK_ARL, 
                "bcm_robo_l2_addr_delete_by_mac(): mem_delete()=%d\n",rv);

        if (rv == BCM_E_NOT_FOUND){
            continue;
        } else if(rv != 0){
            soc_cm_debug(DK_WARN, 
                    "Warnning: bcm_l2_addr_delete_by_mac(),\n\t >>%s\n", 
                    bcm_errmsg(rv));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *	bcm_robo_l2_addr_delete_by_vlan
 * Description:
 *	Remove all L2 (MAC) addresses associated with vid.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	vlan - vid to check
 *	flags - BCM_L2_REMOVE_XXX
 * Returns:
 *	BCM_E_NONE		Success.
 *	BCM_E_INTERNAL		Chip access failure.
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *
 *	ARL aging and learning on all ports is disabled during this
 *	operation.   If these weren't disabled, the hardware could
 *	shift around the contents of the ARL table during the remove
 *	operation, causing some entries that should be removed to remain
 *	in the table.
 */
int
bcm_robo_l2_addr_delete_by_vlan(int unit, bcm_vlan_t vid, uint32 flags)
{
    int                 rv = BCM_E_NONE;
    l2_arl_sw_entry_t         arl_entry;
    uint32              fld_value;
    uint32              action_flag = 0;
    
    sal_memset(&arl_entry, 0, sizeof (arl_entry));
    fld_value = vid; /* vid value */

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete_by_vlan()..\n");
    /* set VID field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
              (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, 
              (uint32 *)&arl_entry, &fld_value));
    action_flag = DRV_MEM_OP_DELETE_BY_VLANID ;
    action_flag |= (flags & BCM_L2_DELETE_STATIC) ? 
                    DRV_MEM_OP_DELETE_BY_STATIC : 0;
    rv =((DRV_SERVICES(unit)->mem_delete)
                    (unit, DRV_MEM_ARL,
                    (uint32 *)&arl_entry,
                    action_flag));
    return rv;
}


/*
 * Function:
 *	bcm_robo_l2_addr_delete_by_trunk
 * Description:
 *	Remove all L2 (MAC) addresses associated with tid.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	tid - tid to check
 *	flags - BCM_L2_REMOVE_XXX
 * Returns:
 *	BCM_E_NONE		Success.
 *	BCM_E_INTERNAL		Chip access failure.
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *
 *	ARL aging and learning on all ports is disabled during this
 *	operation.   If these weren't disabled, the hardware could
 *	shift around the contents of the ARL table during the remove
 *	operation, causing some entries that should be removed to remain
 *	in the table.
 */
int
bcm_robo_l2_addr_delete_by_trunk(int unit, bcm_trunk_t tid, uint32 flags)
{
    bcm_trunk_add_info_t t_add_info;
    int         i, rv = BCM_E_NONE;
    
    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete_by_trunk()..\n");

    if (tid == BCM_TRUNK_INVALID) {
    	return BCM_E_PARAM;
    }
    
    rv = bcm_trunk_get(unit, tid, &t_add_info);
    
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    } else if (rv < 0){
        return BCM_E_INTERNAL;
    }
    
    /* 
     * If no any port assigned to the current trunk,
     * there will be no further processes.
     * Since no tgid information recorded in the arl entry of robo chip.
     */
    if (!t_add_info.num_ports) {
    	return BCM_E_UNAVAIL;
    }
    	
    for (i = 0 ; i < t_add_info.num_ports ; i++){
        BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_port
                        (unit, -1, t_add_info.tp[i], flags));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *	bcm_l2_addr_delete_by_mac_port
 * Description:
 *	Delete L2 entries associated with a MAC address and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	1. Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	2. Robo L2 learning will prevent that the same MAC+VID learned at 
 *      different ports.
 */
int
bcm_robo_l2_addr_delete_by_mac_port(
	int	unit,
	bcm_mac_t	mac,
	bcm_module_t	mod,
	bcm_port_t	port,
	uint32	flags)
{
    bcm_l2_addr_t   l2addr;
    l2_arl_sw_entry_t		arl_search, arl_result;
    bcm_vlan_t      t_vid = VLAN_ID_INVALID;
    int         	rv = BCM_E_NONE;
    int         	i;
    uint32          field_value = 0,vlan_count;
    uint32          action_flag = 0;        

            
    /* Get the current valid VLAN entry count */
     (DRV_SERVICES(unit)->dev_prop_get)
        (unit,DRV_DEV_PROP_VLAN_ENTRY_NUM, &vlan_count);
    soc_cm_debug(DK_ARL, 
        "BCM API : bcm_robo_l2_addr_delete_by_mac_port()..\n vlan_count=%d\n",
        vlan_count);

    for (i = 1; i < vlan_count; i++) {
        t_vid = i;

        /* look up the l2 entry with mac+vid */   
        bcm_l2_addr_init(&l2addr, mac, t_vid);
        BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_search, &l2addr));
    
        action_flag = DRV_MEM_OP_BY_HASH_BY_MAC | 
                    DRV_MEM_OP_BY_HASH_BY_VLANID;
        
        rv =((DRV_SERVICES(unit)->mem_search)
                        (unit, DRV_MEM_ARL, (uint32 *)&arl_search,
                        (uint32 *)&arl_result, NULL,
                        action_flag));
    
        if (rv == BCM_E_EXISTS) {
            if (flags == BCM_L2_DELETE_STATIC){
                action_flag |= DRV_MEM_OP_DELETE_BY_STATIC;
            }
            /* get the port field */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT,
                            (uint32 *)&arl_result, &field_value));
                            
            if (field_value != port){
                continue;
            }
        
            /* delete this l2 entry */
            rv =((DRV_SERVICES(unit)->mem_delete)
                            (unit, DRV_MEM_ARL,
                            (uint32 *)&arl_search,
                            action_flag));
            soc_cm_debug(DK_ARL, 
                    "bcm_robo_l2_addr_delete_by_mac(): mem_delete()=%d\n",rv);
            soc_cm_debug(DK_ARL, 
                "ARL_DEL: MAC=%02x:%02x:%02x:%02x:%02x:%02x, "
                "VID=0x%x, port=0x%x\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                t_vid, port);
                
        } else if (rv == BCM_E_NOT_FOUND || rv == BCM_E_FULL){
            continue;
        } else if(rv != 0){
            soc_cm_debug(DK_WARN, 
                    "Warnning: bcm_l2_addr_delete_by_mac(),\n\t >>%s\n", 
                    bcm_errmsg(rv));
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_l2_addr_delete_by_vlan_port
 * Description:
 *	Delete L2 entries associated with a VLAN and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int 
bcm_robo_l2_addr_delete_by_vlan_port(int unit, bcm_vlan_t vid,
                       bcm_module_t mod, bcm_port_t port,
                       uint32 flags)
{
    int                 rv = BCM_E_NONE;
    l2_arl_sw_entry_t         arl_entry;
    uint32              fld_value;
    uint32              action_flag = 0;
    
    sal_memset(&arl_entry, 0, sizeof (arl_entry));
    fld_value = vid; /* vid value */

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete_by_vlan_port()..\n");
    /* set VID field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
              (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, 
              (uint32 *)&arl_entry, &fld_value));
            
    fld_value = port; /* vid value */
    /* set Port field */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
              (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, 
              (uint32 *)&arl_entry, &fld_value));
    action_flag = DRV_MEM_OP_DELETE_BY_VLANID | 
                    DRV_MEM_OP_DELETE_BY_PORT ;
    action_flag |= (flags & BCM_L2_DELETE_STATIC) ? 
                    DRV_MEM_OP_DELETE_BY_STATIC : 0;
    rv =((DRV_SERVICES(unit)->mem_delete)
                    (unit, DRV_MEM_ARL,
                    (uint32 *)&arl_entry,
                    action_flag));
    return rv;
}                       

/*
 * Function:
 *	bcm_l2_addr_delete_by_vlan_trunk
 * Description:
 *	Delete L2 entries associated with a VLAN and a
 *      destination trunk.
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	tid   - trunk group id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_robo_l2_addr_delete_by_vlan_trunk(int unit, bcm_vlan_t vid,
                                 bcm_trunk_t tid, uint32 flags)
{
    bcm_trunk_add_info_t t_add_info;
    int         i, rv = BCM_E_NONE;
    
    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_delete_by_vlan_trunk()..\n");

    if (tid == BCM_TRUNK_INVALID) {
    	return BCM_E_PARAM;
    }
    
    rv = bcm_trunk_get(unit, tid, &t_add_info);
    
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    } else if (rv < 0){
        return BCM_E_INTERNAL;
    }
    
    /* 
     * If no any port assigned to the current trunk,
     * there will be no further processes.
     * Since no tgid information recorded in the arl entry of robo chip.
     */
    if (!t_add_info.num_ports) {
    	return BCM_E_UNAVAIL;
    }
    	
    for (i = 0 ; i < t_add_info.num_ports ; i++){
        BCM_IF_ERROR_RETURN(bcm_l2_addr_delete_by_vlan_port
                        (unit, vid,-1, t_add_info.tp[i], flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_robo_l2_addr_get
 * Description:
 *	Given a MAC address and VLAN ID, check if the entry is present
 *	in the L2 table, and if so, return all associated information.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	mac - input MAC address to search
 *	vid - input VLAN ID to search
 *	l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *	BCM_E_NONE		Success (l2addr filled in)
 *	BCM_E_PARAM		Illegal parameter (NULL pointer)
 *	BCM_E_INTERNAL		Chip access failure
 *	BCM_E_NOT_FOUND	Address not found (l2addr not filled in)
 */
int 
bcm_robo_l2_addr_get(int unit, bcm_mac_t mac, bcm_vlan_t vid, 
                    bcm_l2_addr_t *l2addr)
{
    bcm_l2_addr_t	l2_search;
    l2_arl_sw_entry_t		arl_entry, arl_result;
    int	    	rv;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_get()..\n");
    bcm_l2_addr_init(&l2_search, mac, vid);

    BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_entry, &l2_search));

    rv =((DRV_SERVICES(unit)->mem_search)
                    (unit, DRV_MEM_ARL, (uint32 *)&arl_entry,
                    (uint32 *)&arl_result, NULL,
                    (DRV_MEM_OP_BY_HASH_BY_MAC | 
                            DRV_MEM_OP_BY_HASH_BY_VLANID)));

    if (rv == BCM_E_EXISTS) {
        _bcm_robo_l2_from_arl(unit, l2addr, &arl_result);
    } else {
        if (rv == BCM_E_FULL){
            rv = BCM_E_NOT_FOUND;
        }
        return rv;
    }

    return BCM_E_NONE;

}



/*
 * Function:
 *	bcm_l2_key_dump
 * Purpose:
 *	Dump the key (VLAN+MAC) portion of a hardware-independent
 *	L2 address for debugging
 * Parameters:
 *	unit - Unit number
 *	pfx - String to print before output
 *	entry - Hardware-independent L2 entry to dump
 *	sfx - String to print after output
 */

int
bcm_robo_l2_key_dump(int unit, char *pfx, bcm_l2_addr_t *entry, char *sfx)
{
    /*
     * In VxWorks, in interrupt context, soc_cm_print uses logMsg, which only
     * allows up to 6 args.  That's why the MAC address is formatted as
     * two hex numbers here.
     */
    soc_cm_print("l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
                 "%02x%02x%02x%s", pfx, entry->vid,
         entry->mac[0], entry->mac[1], entry->mac[2],
         entry->mac[3], entry->mac[4], entry->mac[5], sfx);
    return BCM_E_NONE;
}

/****************************************************************************
 *
 * ARL Message Registration
 *
 ****************************************************************************/

#define L2_CB_MAX		3

typedef struct arl_cb_entry_s {
    bcm_l2_addr_callback_t	fn;
    void	        *fn_data;
} l2_cb_entry_t;

typedef struct l2_data_s {
    l2_cb_entry_t		cb[L2_CB_MAX];
    int	        	cb_count;
} l2_data_t;

static l2_data_t l2_data[SOC_MAX_NUM_SWITCH_DEVICES];

/*
 * Function:
 * 	_bcm_l2_addr_callback
 * Description:
 *	Callback used with chip addr registration functions.
 *	This callback calls all the top level client callbacks.
 * Parameters:
 *	unit - RoboSwitch unit number (driver internal).
 *	l2addr
 *	insert
 *	userdata
 * Returns:
 *
 */
static void
_bcm_l2_addr_callback(int unit,
              bcm_l2_addr_t *l2addr,
              int insert,
              void *userdata)
{
    l2_data_t	    *ad = &l2_data[unit];
    int i;

    for(i = 0; i < L2_CB_MAX; i++) {
        if(ad->cb[i].fn) {
            ad->cb[i].fn(unit, l2addr, insert, ad->cb[i].fn_data);
        }
    }
}

/*
 * Function:
 *	bcm_robo_l2_addr_register
 * Description:
 *	Register a callback routine that will be called whenever
 *	an entry is inserted into or deleted from the L2 address table.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	fn - Callback function of type bcm_l2_addr_callback_t.
 *	fn_data - Arbitrary value passed to callback along with messages
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 */
int
bcm_robo_l2_addr_register(int unit,
             bcm_l2_addr_callback_t fn, 
             void *fn_data)
{
    l2_data_t   *ad = &l2_data[unit];
    int         i;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_register()..\n");
    if (ad->cb_count == 0) {
        BCM_IF_ERROR_RETURN
            (bcm_53xx_l2_addr_register(unit, _bcm_l2_addr_callback, NULL));
    }

    if (ad->cb_count >= L2_CB_MAX) {
        return BCM_E_MEMORY;
    }

#ifdef LVL7_FIXUP
    /* Avoid duplicate registrations */
    for (i = 0; i < L2_CB_MAX; i++)
    {
      if ( (ad->cb[i].fn != NULL) &&
            (ad->cb[i].fn == fn)
         )
      {
        return BCM_E_NONE;
      }
    }
#endif

    for (i = 0; i < L2_CB_MAX; i++) {
        if (ad->cb[i].fn == NULL) {
            ad->cb[i].fn = fn;
            ad->cb[i].fn_data = fn_data;
            ad->cb_count++;
            break;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_robo_l2_addr_unregister
 * Description:
 *	Unregister a previously registered callback routine.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	fn - Same callback function used to register callback
 *	fn_data - Same arbitrary value used to register callback
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Both callback and userdata must match from original register call.
 */
int
bcm_robo_l2_addr_unregister(int unit,
               bcm_l2_addr_callback_t fn, 
               void *fn_data)
{
    l2_data_t   *ad = &l2_data[unit];
    int         i, rv = BCM_E_NONE;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_unregister()..\n");
    for (i = 0; (i < ad->cb_count) && (rv == BCM_E_NONE); i++) {
        if((ad->cb[i].fn == fn) && (ad->cb[i].fn_data == fn_data)) {
            ad->cb[i].fn = ad->cb[i].fn_data = NULL;
            ad->cb_count--;
            if (ad->cb_count == 0) {
                rv = bcm_53xx_l2_addr_unregister(unit,
                            _bcm_l2_addr_callback,
                            NULL);
            }
        }
    }
    return rv;
}
                  

/*
 * Set L2 table aging time
 */

/*
 * Function:
 *	bcm_robo_l2_age_timer_set
 * Description:
 *	Set the age timer for all blocks.
 *	Setting the value to 0 disables the age timer.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	age_seconds - Age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */
int
bcm_robo_l2_age_timer_set(int unit, int age_seconds)
{
    uint32  max_value;
    uint32  enabled;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_age_timer_set()..\n");
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->dev_prop_get)
                        (unit, DRV_DEV_PROP_AGE_TIMER_MAX_S, &max_value));

    if (age_seconds > max_value) {
        return BCM_E_PARAM;
    }

    enabled = age_seconds ? TRUE : FALSE;

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->age_timer_set)
            (unit, enabled, (uint32)age_seconds));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_robo_l2_age_timer_get
 * Description:
 *	Returns the current age timer value.
 *	The value is 0 if aging is not enabled.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	age_seconds - Place to store returned age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */
int
bcm_robo_l2_age_timer_get(int unit, int *age_seconds)
{
    uint32 enabled;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_age_timer_get()..\n");
    if (age_seconds == NULL){
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->age_timer_get)
            (unit, &enabled, (uint32 *)age_seconds));

    if (!enabled) {
        *age_seconds = 0;
    }

    return BCM_E_NONE;
}
    

/*
 * Manage BPDU addresses
 */

/*
 * Function:
 *	bcm_robo_l2_bpdu_count
 * Description:
 *	Retrieve the number of BPDU addresses available on the
 *      specified unit.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *	number of BPDU address entries
 */
int
bcm_robo_l2_bpdu_count(int unit)
{
    int count;
    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_bpdu_count()..\n");
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->dev_prop_get)
        (unit, DRV_DEV_PROP_BPDU_NUM, (uint32 *) &count));
    return count;
}

/*
 * Function:
 *	bcm_robo_l2_bpdu_set
 * Description:
 *	Set BPDU address
 * Parameters:
 *	unit - switch unit
 *	index - BPDU index
 *	addr - 6 byte mac address
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_*        	Failure
 */
int
bcm_robo_l2_bpdu_set(int unit, int index, mac_addr_t addr)
{
    int	nbpdu;
    pbmp_t null_pbmp;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_bpdu_set()..\n");

    nbpdu = bcm_l2_bpdu_count(unit);

    if (nbpdu == 0) {
	    return BCM_E_UNAVAIL;
    }
    if (index < 0 || index >= nbpdu) {
	    return BCM_E_PARAM;
    }
    BCM_PBMP_CLEAR(null_pbmp);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mac_set)
            (unit, null_pbmp, DRV_MAC_CUSTOM_BPDU, addr, index));
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_robo_l2_bpdu_get
 * Description:
 *	Return BPDU address
 * Parameters:
 *	unit - switch unit
 *	index - BPDU index
 *	addr - (OUT) 6 byte mac address
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_*        	Failure
 */
int
bcm_robo_l2_bpdu_get(int unit, int index, mac_addr_t *addr)
{
    int	nbpdu;
    pbmp_t null_parm;

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_bpdu_get()..\n");

    nbpdu = bcm_l2_bpdu_count(unit);

    if (nbpdu == 0) {
	    return BCM_E_UNAVAIL;
    }
    if (index < 0 || index >= nbpdu) {
	    return BCM_E_PARAM;
    }
   
    /* null_parm is a null parameter to suit soc driver prototype*/
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mac_get)
            (unit, index, DRV_MAC_CUSTOM_BPDU, &null_parm, *addr));
    
    return BCM_E_NONE;
}

/*
 * Temporarily stop L2 table from changing (learning, aging, CPU, etc)
 */

/*
 * Function:
 *	bcm_robo_l2_addr_freeze
 * Description:
 *	Temporarily quiesce ARL from all activity (learning, aging)
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */
int
bcm_robo_l2_addr_freeze(int unit)
{
    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_freeze().\n");
    BCM_IF_ERROR_RETURN(soc_robo_arl_freeze(unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_robo_l2_addr_thaw
 * Description:
 *	Restore normal ARL activity.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */
int
bcm_robo_l2_addr_thaw(int unit)
{
    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_addr_thaw().\n");
    BCM_IF_ERROR_RETURN( soc_robo_arl_thaw(unit));
    return BCM_E_NONE;
}
    
/*
 * Function:
 *      bcm_l2_clear
 * Purpose:
 *      Clear the L2 layer
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_l2_clear(int unit)
{
    bcm_l2_detach(unit);

    soc_cm_debug(DK_ARL, 
            "BCM API : bcm_robo_l2_clear()..\n");
    /*
     * Call chip-dependent initialization
     */
    soc_robo_arl_unregister(unit, _bcm_robo_l2_register_callback, NULL);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_clear)
            (unit, DRV_MEM_ARL));
    
    /* bcm_l2_register clients */
    soc_robo_arl_register(unit, _bcm_robo_l2_register_callback, NULL);


    /* Clear l2_data structure */
    l2_data[unit].cb_count = 0;
    sal_memset(&l2_data[unit].cb, 0, sizeof(l2_data[unit].cb));

    /* BCM shadow table will go away soon */

    return BCM_E_NONE;
}


/*
 * Given an L2 or L2 multicast address, return any existing L2 or L2
 * multicast addresses which might prevent it from being inserted
 * because a chip resource (like a hash bucket) is full.
 */

/*
 * Function:
 *	bcm_robo_l2_conflict_get
 * Purpose:
 *	Given an L2 address, return existing addresses which could conflict.
 * Parameters:
 *	unit	    - switch device
 *	addr	    - l2 address to search for conflicts
 *	cf_array	- (OUT) list of l2 addresses conflicting with addr
 *	cf_max	    - number of entries allocated to cf_array
 *	cf_count	- (OUT) actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 * Note :
 *      1. Robo's hash algorism presented the bucket size is two.
 *          - means 8K l2 entries -> 4k l2 buckets(per bucket 2 entries)
 *      2. BCM5348 has only one bucket 8k l2 entries.
 */

int
bcm_robo_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
            bcm_l2_addr_t *cf_array, int cf_max,
            int *cf_count)
{
    l2_arl_sw_entry_t arl_entry, output;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_entry, addr));

    *cf_count = 0;
    rv = (DRV_SERVICES(unit)->mem_search)
            (unit, DRV_MEM_ARL, (uint32 *)&arl_entry,
            (uint32 *)&output, NULL,
             (DRV_MEM_OP_BY_HASH_BY_MAC | DRV_MEM_OP_BY_HASH_BY_VLANID));

    if ((rv == SOC_E_EXISTS) && (*cf_count < cf_max)) {
        _bcm_robo_l2_from_arl(unit, &cf_array[*cf_count], &output);
        *cf_count += 1;
        soc_cm_debug(DK_ARL, 
                "bcm_robo_l2_conflict_get ,conflict entry is 0x%x,0x%x,0x%x\n",
                output.entry_data[2], 
                output.entry_data[1], 
                output.entry_data[0]);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *      bcm_l2_port_native
 * Purpose:
 *      Determine if the given port is "native" from the point
 *      of view of L2.
 * Parameters:
 *      unit       - The unit
 *      modid      - Module ID of device
 *      port       - Physical port on the unit
 * Returns:
 *      TRUE (> 0) if (modid, port) is front panel/CPU port for unit.
 *      FALSE (0) otherwise.
 *      < 0 on error.
 *
 *      Native means front panel, but also includes the CPU port.
 *      HG ports are always excluded as are ports marked as internal or
 *      external stacking
 */
int
bcm_robo_l2_port_native(int	unit, int modid, int port)
{
    /* 
     *  no stk API been defined currently for robo, this API is not suitable
     *  for Robo
     */
     if(modid<0)
        return FALSE;

    return TRUE;

}
int
bcm_robo_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_l2_tunnel_delete_all(int unit)
{
    return BCM_E_UNAVAIL;
}
int bcm_robo_l2_learn_limit_set(int unit, uint32 flags, int limit)
{
   return BCM_E_UNAVAIL;
}
int bcm_robo_l2_learn_limit_get(int unit, uint32 *flags, int *limit)
{
   return BCM_E_UNAVAIL;
}
int bcm_robo_l2_learn_limit_port_set(int unit, bcm_port_t port, uint32 flags,
                                int limit)
{
   return BCM_E_UNAVAIL;
}
int bcm_robo_l2_learn_limit_port_get(int unit, bcm_port_t port, uint32 *flags,
                                int *limit)
{
   return BCM_E_UNAVAIL;
}
int bcm_robo_l2_learn_limit_trunk_set(int unit, bcm_trunk_t trunk, 
                                uint32 flags, int limit)
{
   return BCM_E_UNAVAIL;
}
 int bcm_robo_l2_learn_limit_trunk_get(int unit, bcm_trunk_t tid,
                                     uint32 *flags, int *limit)
{   
    return BCM_E_UNAVAIL;
}

int
bcm_robo_l2_learn_class_set(int unit, int class, int class_prio, uint32 flags)
{   
    return BCM_E_UNAVAIL;
}

int
bcm_robo_l2_learn_class_get(int unit, int class, int *class_prio,
                            uint32 *flags)
{   
    return BCM_E_UNAVAIL;
}

int
bcm_robo_l2_learn_port_class_set(int unit,  bcm_gport_t port, int class)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_l2_learn_port_class_get(int unit,  bcm_gport_t port, int *class)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_l2_addr_delete_by_mac_vpn
 * Description:
 *      Delete L2 entry with matching MAC address and VPN.
 * Parameters:
 *      unit  - device unit
 *      mac   - MAC address
 *      vpn   - L2 VPN identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_l2_addr_delete_by_mac_vpn(int unit, bcm_mac_t mac, bcm_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_l2_addr_delete_by_vpn
 * Description:
 *      Delete L2 entries associated with an L2 VPN.
 * Parameters:
 *      unit  - device unit
 *      vpn   - L2 VPN identifier
 *      flags - BCM_L2_DELETE_XXX
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *      L2 aging and learning are disabled during this operation.
 */
int
bcm_robo_l2_addr_delete_by_vpn(int unit, bcm_vpn_t vpn, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_robo_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_robo_l2_traverse to itterate over given memory 
 *      and actually read the table and parse entries.
 * Parameters:
 *     unit         device number
 *      mem         L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_robo_l2_traverse_mem(int unit, soc_mem_t mem, _bcm_robo_l2_traverse_t *trav_st)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, rv, chunksize, copysize;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *l2_tbl_chnk, valid;
    soc_control_t   *soc = SOC_CONTROL(unit);
    l2_arl_sw_entry_t *l2x_entry;

    if (!soc->arl_table) {
        return BCM_E_UNAVAIL;
    }

    if (!soc_robo_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_ROBO_MEM_CHUNKS_DEFAULT);

    buf_size = sizeof(l2_arl_sw_entry_t) * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 traverse");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;;
    }

    mem_idx_max = soc_robo_mem_index_max(unit, mem);
    for (chnk_idx = soc_robo_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            chnk_idx + chunksize : mem_idx_max;

        copysize = chnk_idx_max - chnk_idx + 
            ((chnk_idx_max == mem_idx_max) ? 1 : 0);
        ARL_SW_TABLE_LOCK(soc);
        sal_memcpy((void *)l2_tbl_chnk, &soc->arl_table[chnk_idx], 
            sizeof(l2_arl_sw_entry_t) * copysize);
        ARL_SW_TABLE_UNLOCK(soc);
        
        l2x_entry = (l2_arl_sw_entry_t *)l2_tbl_chnk;
        for (ent_idx = 0 ; ent_idx < copysize; ent_idx ++) {
            (DRV_SERVICES(unit)->mem_field_get)
     		(unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
    		(uint32 *)l2x_entry, &valid);
            if (valid){
                _bcm_robo_l2_from_arl(unit, trav_st->data, l2x_entry);          	  
                rv = trav_st->user_cb(unit, trav_st->data, trav_st->user_data);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, l2_tbl_chnk);
                    return rv;
                }
            }
            l2x_entry++;
        }
    }           
    soc_cm_sfree(unit, l2_tbl_chnk);
    return BCM_E_NONE;        
}


/*
 * Function:
 *     _bcm_robo_l2_traverse
 * Description:
 *      Helper function to bcm_robo_l2_traverse to itterate over table 
 *      and actually read the momery
 * Parameters:
 *     unit         device number
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_robo_l2_traverse(int unit, _bcm_robo_l2_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL; 

    rv = _bcm_robo_l2_traverse_mem(unit, L2_ARLm, trav_st);

    return rv;
}


/*
 * Function:
 *     bcm_robo_l2_traverse
 * Description:
 *     To traverse the L2 table and call provided callback function with matched entry
 * Parameters:
 *     unit         device number
 *     trav_fn      User specified callback function 
 *     user_data    User specified cookie
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_robo_l2_traverse(int unit, bcm_l2_traverse_cb trav_fn, void *user_data)
{
    _bcm_robo_l2_traverse_t  trav_st;
    bcm_l2_addr_t       l2_entry;
       
    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(_bcm_robo_l2_traverse_t));
    sal_memset(&l2_entry, 0, sizeof(bcm_l2_addr_t));

    trav_st.pattern = NULL;
    trav_st.data = &l2_entry;
    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;

    return (_bcm_robo_l2_traverse(unit, &trav_st));
}

#define _BCM_L2_REPLACE_ALL_LEGAL            (BCM_L2_REPLACE_MATCH_MAC | \
                                              BCM_L2_REPLACE_MATCH_VLAN | \
                                              BCM_L2_REPLACE_MATCH_DEST | \
                                              BCM_L2_REPLACE_MATCH_STATIC   | \
                                              BCM_L2_REPLACE_DELETE | \
                                              BCM_L2_REPLACE_NEW_TRUNK | \
                                              BCM_L2_REPLACE_PENDING)
/*
 * Function:
 *     _bcm_robo_l2_replace_flags_validate
 * Description:
 *     THelper function to bcm_l2_replace API to validate given flags
 * Parameters:
 *     flags        flags BCM_L2_REPLACE_* 
 * Return:
 *     BCM_E_NONE - OK 
 *     BCM_E_PARAM - Failure
 */
int 
_bcm_robo_l2_replace_flags_validate(uint32 flags)
{
    if (!flags) {
        return BCM_E_PARAM;
    }
    if ((flags & BCM_L2_REPLACE_DELETE) && (flags & BCM_L2_REPLACE_NEW_TRUNK)) {
        return BCM_E_PARAM;
    }
    if ((flags > (BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_VLAN |
        BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_STATIC)) && 
        (flags < BCM_L2_REPLACE_DELETE)) {
        return BCM_E_PARAM;
    }
    if (flags > _BCM_L2_REPLACE_ALL_LEGAL) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_robo_l2_addr_replace_by_mac
 * Description:
 *     Helper function to bcm_l2_replace API to replace l2 entries by mac
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_robo_l2_addr_replace_by_mac(int unit, uint32 flags, _bcm_robo_l2_replace_t *rep_st)
{
    uint32 l2_flags, field_value=0;
    bcm_l2_addr_t	l2_search;
    l2_arl_sw_entry_t		arl_entry, arl_result, arl_search;
    int rv, i, vlan_count, t_vid;
    
    l2_flags = 0;

    if (rep_st->isDel){
        if (rep_st->isStatic) {
            l2_flags = BCM_L2_DELETE_STATIC;
        }
        return bcm_l2_addr_delete_by_mac(unit, rep_st->match_mac, l2_flags);
    } 

    /* Get the current valid VLAN entry count */
     (DRV_SERVICES(unit)->dev_prop_get)
        (unit, DRV_DEV_PROP_VLAN_ENTRY_NUM, (uint32 *) &vlan_count);

    for (i = 1; i < vlan_count; i++) {
        t_vid = i;

        bcm_l2_addr_init(&l2_search, rep_st->match_mac, t_vid);

        BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_entry, &l2_search));
        l2_flags =  DRV_MEM_OP_BY_HASH_BY_MAC|DRV_MEM_OP_BY_HASH_BY_VLANID;
        rv =((DRV_SERVICES(unit)->mem_search)
                    (unit, DRV_MEM_ARL, (uint32 *)&arl_entry,
                    (uint32 *)&arl_result, NULL,
                    l2_flags));                    

        if (rv == BCM_E_EXISTS) {
            if (rep_st->isStatic){
                l2_flags |= DRV_MEM_OP_DELETE_BY_STATIC;
            }
        

            /* get the port field */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT,
                            (uint32 *)&arl_result, &field_value));

            if (field_value == rep_st->new_port) {
                continue;
            }
                
            /* delete this l2 entry */
            rv =((DRV_SERVICES(unit)->mem_delete)
                            (unit, DRV_MEM_ARL,
                            (uint32 *)&arl_search,
                            l2_flags));
            soc_cm_debug(DK_ARL, 
                    "_bcm_robo_l2_addr_replace_by_mac(): mem_delete()=%d\n",rv);
            soc_cm_debug(DK_ARL, 
                "ARL_DEL: MAC=%02x:%02x:%02x:%02x:%02x:%02x, "
                "VID=0x%x\n",
                rep_st->match_mac[0], rep_st->match_mac[1], rep_st->match_mac[2], 
                rep_st->match_mac[3], rep_st->match_mac[4], rep_st->match_mac[5],
                t_vid);
        
            l2_flags = DRV_MEM_OP_BY_HASH_BY_MAC | DRV_MEM_OP_BY_HASH_BY_VLANID | DRV_MEM_OP_REPLACE;
    
            /* set the port field */
            field_value = rep_st->new_port;
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT,
                            (uint32 *)&arl_result, &field_value));
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_insert)
                (unit, DRV_MEM_ARL, (uint32 *)&arl_search,
                 l2_flags));

        } else if (rv == BCM_E_NOT_FOUND || rv == BCM_E_FULL){
            continue;
        } else if(rv != 0){
            soc_cm_debug(DK_WARN, 
                    "Warnning: _bcm_robo_l2_addr_replace_by_mac(),\n\t >>%s\n", 
                    bcm_errmsg(rv));
        }
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_robo_l2_addr_replace_by_mac_vlan
 * Description:
 *     Helper function to bcm_l2_replace API to replace l2 entries by mac
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_robo_l2_addr_replace_by_mac_vlan(int unit, _bcm_robo_l2_replace_t *rep_st) {

    uint32 action_flag, port, temp;
    int rv;
    bcm_l2_addr_t	 l2addr;
    bcm_trunk_add_info_t t_add_info;
    l2_arl_sw_entry_t   arl_search, arl_result;
    bcm_trunk_t     tid;
    
    action_flag = 0;

    bcm_l2_addr_init(&l2addr, rep_st->match_mac, rep_st->match_vid);

    /* look up the l2 entry with mac+vid */   
    BCM_IF_ERROR_RETURN(_bcm_robo_l2_to_arl(unit, &arl_search, &l2addr));
    
    action_flag = DRV_MEM_OP_BY_HASH_BY_MAC | 
                 DRV_MEM_OP_BY_HASH_BY_VLANID;
        
    rv =((DRV_SERVICES(unit)->mem_search)
                    (unit, DRV_MEM_ARL, (uint32 *)&arl_search,
                    (uint32 *)&arl_result, NULL,
                    action_flag));
    
    if (rv != BCM_E_EXISTS) {
        return BCM_E_NOT_FOUND;
    }

    if (rep_st->isStatic) {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC,
                (uint32 *)&arl_result, &temp));
        if (!temp) {
            return BCM_E_NOT_FOUND;
        }
    }

    if (rep_st->isTrunk) {
        /* get the port field */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT,
                        (uint32 *)&arl_result, &port));
                            
        bcm_trunk_find(unit, 0, port, &tid);
        /* check the port existed in the match_trunk ? */
        if (tid != rep_st->match_trunk) {
            return BCM_E_PARAM;
        }

        /* check the new_trunk valid ?*/
        if (rep_st->new_trunk== BCM_TRUNK_INVALID) {
        	return BCM_E_PARAM;
        }    
        rv = bcm_trunk_get(unit, rep_st->new_trunk, &t_add_info);
    
        if (rv < 0){
            return BCM_E_PARAM;
        }
    
        /* 
         * If no any port assigned to the current trunk,
         * there will be no further processes.
         * Since no tgid information recorded in the arl entry of robo chip.
         */
        if (!t_add_info.num_ports) {
            return BCM_E_UNAVAIL;
        }
        /* pick any port in the new_trunk */
        l2addr.port = t_add_info.tp[0];
    }else {

        if (rep_st->isDel){            
            return bcm_l2_addr_delete(unit, rep_st->match_mac, rep_st->match_vid);
        } 

        l2addr.port = rep_st->new_port;
    }
    bcm_l2_addr_add(unit, &l2addr);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_robo_l2_replace
 * Description:
 *     To replace destination (or delete) multiple L2 entries
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     match_addr   L2 parameters to match on delete/replace
 *     new_module   new module ID for a replace 
 *     new_port     new port for a replace
 *     new_trunk    new trunk for a replace  
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_robo_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                   bcm_module_t new_module, bcm_port_t new_port, 
                   bcm_trunk_t new_trunk)
{
    uint32              cmp_flags, l2_flags;
    _bcm_robo_l2_replace_t   rep_st;
    int                 rv = BCM_E_UNAVAIL;


    if (NULL == match_addr) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_robo_l2_replace_flags_validate(flags));
    sal_memset(&rep_st, 0, sizeof(_bcm_robo_l2_replace_t));

    if (0 == (flags & BCM_L2_REPLACE_DELETE)) {
        if (BCM_GPORT_IS_SET(new_port)) {
            bcm_port_t  tmp_port;
            int         tmp_id;

            BCM_IF_ERROR_RETURN(
                _bcm_robo_gport_resolve(unit, new_port, &new_module, &tmp_port,
                                       &new_trunk, &tmp_id));
            new_port = tmp_port;
        } 
        if (flags & BCM_L2_REPLACE_NEW_TRUNK) { 
            BCM_IF_ERROR_RETURN(
                _bcm_robo_trunk_id_validate(unit, new_trunk));

            BCM_IF_ERROR_RETURN(
                _bcm_robo_trunk_id_validate(unit, match_addr->tgid));

              rep_st.isTrunk= 1;

              rep_st.match_trunk = match_addr->tgid;
        } else {
            if (new_module != 0) {
                return BCM_E_PARAM;
            }
            if (!SOC_PORT_VALID(unit, new_port)) {
                return BCM_E_PORT;
            }
        }
        rep_st.new_module = new_module;
        rep_st.new_port = new_port;
        rep_st.new_trunk = new_trunk;
    } else {
        rep_st.isDel = 1;
    }
    cmp_flags = flags &  ( BCM_L2_REPLACE_MATCH_MAC | 
                                        BCM_L2_REPLACE_MATCH_VLAN |
                                        BCM_L2_REPLACE_MATCH_DEST );

    if (flags & BCM_L2_REPLACE_MATCH_STATIC) {
        rep_st.isStatic = 1;
    }

    BCM_IF_ERROR_RETURN(bcm_l2_addr_freeze(unit));
    l2_flags = 0;

    switch (cmp_flags) {
        case BCM_L2_REPLACE_MATCH_MAC: 
        {        
            if (rep_st.isDel) {
                if (rep_st.isStatic) {
                    l2_flags = BCM_L2_DELETE_STATIC;
                }
                rv = bcm_l2_addr_delete_by_mac(unit, match_addr->mac, l2_flags);
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        }
        case BCM_L2_REPLACE_MATCH_VLAN: 
        {
            if (rep_st.isDel) {
                if (rep_st.isStatic) {
                    l2_flags = BCM_L2_DELETE_STATIC;
                }
                rv = bcm_l2_addr_delete_by_vlan(unit, match_addr->vid, l2_flags);
            } else {
                rv = BCM_E_UNAVAIL;
            }
            break;
        }
        case (BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_VLAN):
        {
            sal_memcpy(&(rep_st.match_mac), match_addr->mac, sizeof(bcm_mac_t));
            rep_st.match_vid = match_addr->vid;
            rv =_bcm_robo_l2_addr_replace_by_mac_vlan(unit, &rep_st);
            break;
        }
        case BCM_L2_REPLACE_MATCH_DEST: 
        case (BCM_L2_REPLACE_MATCH_MAC |BCM_L2_REPLACE_MATCH_DEST):
        case (BCM_L2_REPLACE_MATCH_MAC |BCM_L2_REPLACE_MATCH_VLAN |
                    BCM_L2_REPLACE_MATCH_DEST ):
        case (BCM_L2_REPLACE_MATCH_VLAN |
                    BCM_L2_REPLACE_MATCH_DEST ):
        {
            rv = BCM_E_UNAVAIL;
            break;
        }
        default:
            break;

    }


    BCM_IF_ERROR_RETURN(bcm_l2_addr_thaw(unit));

    return rv;
}

