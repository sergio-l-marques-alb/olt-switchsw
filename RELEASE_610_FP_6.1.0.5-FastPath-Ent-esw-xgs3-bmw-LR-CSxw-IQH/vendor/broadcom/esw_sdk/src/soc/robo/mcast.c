/*
 * $Id: mcast.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
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
#include <soc/error.h>
#include <soc/types.h>
#include <soc/mcm/robo/memregs.h>
#include <soc/drv_if.h>
#include <assert.h>
#include <soc/register.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/mcast.h>
#include <bcm/mcast.h>

_bcm_robo_mcast_t    robo_l2mc_info[BCM_MAX_NUM_UNITS];
/*
 *  Function : drv_mcast_init
 *
 *  Purpose :
 *      Initialize multicast
 *
 *  Parameters :
 *      unit        :   unit id
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 */

int
drv_mcast_init(int unit)
{
    int rv = SOC_E_NONE;   

    if(SOC_IS_ROBO5324(unit)){
        /* clear the mcast table(mcast pbmap table) */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_clear)
                    (unit, DRV_MEM_MCAST));
    }
    /* clear the mcast arl table */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_clear)
                    (unit, DRV_MEM_MARL));    
    return rv;
}


/*
 * Internal functions to convert between marl_entry_t and bcm_mcast_addr_t
 */

int
drv_mcast_to_marl(int unit, uint32 *marl_entry,
			  uint32 *addr)
{
    uint32 fld_value;
    uint64 mac_field;
    bcm_mcast_addr_t *mcaddr = (bcm_mcast_addr_t *)addr;

    sal_memset(marl_entry, 0, sizeof (l2_marl_sw_entry_t));

    SAL_MAC_ADDR_TO_UINT64(mcaddr->mac, mac_field);
    (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_MAC,
                    (uint32 *)marl_entry, (uint32 *)&mac_field);

    /* set VID field */
    fld_value = mcaddr->vid;
    (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_VLANID,
                    (uint32 *)marl_entry, &fld_value);

    /* set static field - multicast entry should be static entry */
    /* exclude ROBO5396, no such field */
    fld_value = 1;
    if(!SOC_IS_ROBO5396(unit)) {
    (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_STATIC,
                    (uint32 *)marl_entry, &fld_value);
    }

    /* multicast entry should be valid entry */
    fld_value = 1;
    (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_VALID,
                    (uint32 *)marl_entry, &fld_value);
            
    if (SOC_IS_ROBO5324(unit)||SOC_IS_ROBO5348(unit) ||SOC_IS_ROBO5347(unit) ||
        SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* set mc_index */
	if(mcaddr->l2mc_index == -1) {
	  fld_value = 0;
        } else {
           fld_value = mcaddr->l2mc_index;
	}	   
     } else {
        fld_value = SOC_PBMP_WORD_GET(mcaddr->pbmp, 0);
     }
     (DRV_SERVICES(unit)->mem_field_set)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_DEST_BITMAP,
                    (uint32 *)marl_entry, &fld_value);
    
    return SOC_E_NONE;
}


int
drv_mcast_from_marl(int unit, uint32 *addr,
			    uint32 *marl_entry)
{
    int rv = SOC_E_NONE;
    uint64 mac_field;
    vlan_1q_entry_t vEntry;
    uint32 fld_value[((MAX_PORT(unit)+1)/32) + 1] ;
    bcm_mcast_addr_t *mcaddr = (bcm_mcast_addr_t *)addr;
    int i;

    sal_memset(mcaddr, 0, sizeof (*mcaddr));

   /* get MAC addr */
    (DRV_SERVICES(unit)->mem_field_get)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_MAC,
                    (uint32 *)marl_entry, (uint32 *)&mac_field);

    SAL_MAC_ADDR_FROM_UINT64(mcaddr->mac, mac_field);

    /* get VID field */
    (DRV_SERVICES(unit)->mem_field_get)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_VLANID,
                    (uint32 *)marl_entry, fld_value);
    mcaddr->vid = fld_value[0];

    /* Need to fill in the untagged port bitmap from the VLAN table */
    if ((rv = (DRV_SERVICES(unit)->mem_read)
        (unit, DRV_MEM_VLAN, mcaddr->vid, 1, (uint32 *)&vEntry))< 0) {
        return rv;
    }

    /* get pbmp field */
    (DRV_SERVICES(unit)->mcast_bmp_get)
                    (unit, (uint32 *)&marl_entry, &mcaddr->pbmp);

    /* get ubmp field */
    (DRV_SERVICES(unit)->mem_field_get)(unit, DRV_MEM_VLAN,
            DRV_MEM_FIELD_OUTPUT_UNTAG, (uint32 *)&vEntry, fld_value);

    BCM_PBMP_CLEAR(mcaddr->ubmp);
    for (i=0; i<(sizeof(fld_value)/sizeof(*fld_value)) ; i++){
        SOC_PBMP_WORD_SET(mcaddr->ubmp, 0, fld_value[i]);
    }
    
    if (SOC_IS_ROBO5324(unit)||SOC_IS_ROBO5348(unit) ||SOC_IS_ROBO5347(unit) ||
        SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* get l2mc_index field */
        (DRV_SERVICES(unit)->mem_field_get)
                    (unit, DRV_MEM_MARL, DRV_MEM_FIELD_DEST_BITMAP,
                    (uint32 *)marl_entry, fld_value);
        mcaddr->l2mc_index = *fld_value;
    }
    return SOC_E_NONE;   
}



/*
 *  Function : drv_mcast_bmp_get
 *
 *  Purpose :
 *      Get the multicast member ports from multicast entry
 *
 *  Parameters :
 *      unit        :   unit id
 *      entry   :   entry data pointer 
 *      bmp     :   group port member
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 */
int  
drv_mcast_bmp_get(int unit, uint32 *entry, soc_pbmp_t *bmp)
{
    int rv = SOC_E_NONE;
    uint32      index, entry_index, fld_v32;
    gen_memory_entry_t  mcast_entry;

    assert(entry);
    assert(bmp);

    /* get the multicast id */
    if ((rv = (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_MARL, DRV_MEM_FIELD_DEST_BITMAP, 
            entry, &index)) < 0) {
        return rv;
    }

    /* calculate entry index */
    entry_index = (index / 2);

    /* get multicast group entry */
    if ((rv = (DRV_SERVICES(unit)->mem_read)
            (unit, DRV_MEM_MCAST, entry_index, 1, 
            (uint32 *)&mcast_entry)) < 0) {
        return rv;
    }

    if (index % 2) { /* odd index */
        if ((rv = (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_MCAST, DRV_MEM_FIELD_DEST_BITMAP1, 
            (uint32 *)&mcast_entry, &fld_v32)) < 0) {
            return rv;
        }
    }else { /* even index */
        if ((rv = (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_MCAST, DRV_MEM_FIELD_DEST_BITMAP, 
            (uint32 *)&mcast_entry, &fld_v32)) < 0) {
            return rv;
        }
    }
    SOC_PBMP_WORD_SET(*bmp, 0, fld_v32);
    soc_cm_debug(DK_L2TABLE, 
        "drv_mcast_bmp_get: unit %d, bmp = %x\n",
        unit, SOC_PBMP_WORD_GET(*bmp, 0));
    return rv;
}

 /*
 *  Function : drv_mcast_bmp_set
 *
 *  Purpose :
 *      Set the multicast member ports from multicast entry
 *
 *  Parameters :
 *      unit        :   unit id
 *      entry   :   entry data pointer 
 *      bmp     :   group port member
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 */
int  
drv_mcast_bmp_set(int unit, uint32 *entry, soc_pbmp_t bmp, uint32 flag)
{
    int rv, loop, mcindex = -1;
    uint32      index, entry_index, temp;
    gen_memory_entry_t  mcast_entry;
    uint32  reg_addr, reg_value, reg_len;
    uint32  fld_value = 0;
    
    assert(entry);
    soc_cm_debug(DK_L2TABLE, 
        "drv_mcast_bmp_set: unit %d, bmp = %x flag %x\n",
        unit, SOC_PBMP_WORD_GET(bmp, 0), flag);
    
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_get)
                        (unit, DRV_MEM_MARL, DRV_MEM_FIELD_DEST_BITMAP,
                        entry, &index)); 
    if( flag & DRV_MCAST_INDEX_ADD) {
               
        if (index){
            mcindex = index;
        } else {
            /* get a free index for creating */
            for (loop = 0; loop < L2MC_SIZE(unit); loop++) {
                if (!L2MC_USED_ISSET(unit, loop)) {
                    mcindex = loop;
                    break;
                }
            }
    
            if (mcindex == -1){
                return SOC_E_FULL;
            }
        }
        /* set mc-index field */
        fld_value = (uint32)mcindex;
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
            (unit, DRV_MEM_MARL, DRV_MEM_FIELD_DEST_BITMAP, 
            entry, &fld_value));

        /* Insert this address into arl table. */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_insert)
            (unit, DRV_MEM_ARL, entry,
             (DRV_MEM_OP_BY_HASH_BY_MAC | DRV_MEM_OP_BY_HASH_BY_VLANID
               | DRV_MEM_OP_REPLACE)));

        L2MC_USED_SET(unit, mcindex);
        index = mcindex;

    }

    /* enable IP_Multicast scheme (specific for Robo5324)
     *   - here we enable IP_Multicast scheme each time when set mcast_pbm.
     */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, NEW_CONTROLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, NEW_CONTROLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    temp = 1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, NEW_CONTROLr, &reg_value, IP_MULTICASTf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));
    
    /* calculate entry index */
    entry_index = (index / 2);

    /* get multicast group entry */
    if ((rv = (DRV_SERVICES(unit)->mem_read)
        (unit, DRV_MEM_MCAST, entry_index, 1, 
        (uint32 *)&mcast_entry)) < 0) {
        return rv;
    }

    /* set valid bit depend on bmp */
    temp = (SOC_PBMP_IS_NULL(bmp)) ? 0 : 1;  
    if (index % 2) { /* odd index */
        if ((rv = (DRV_SERVICES(unit)->mem_field_set)
            (unit, DRV_MEM_MCAST, DRV_MEM_FIELD_VALID1, 
            (uint32 *)&mcast_entry, &temp)) < 0) {
            return rv;
        }
        temp = SOC_PBMP_WORD_GET(bmp, 0);
        if ((rv = (DRV_SERVICES(unit)->mem_field_set)
            (unit, DRV_MEM_MCAST, DRV_MEM_FIELD_DEST_BITMAP1, 
            (uint32 *)&mcast_entry, &temp)) < 0) {
            return rv;
        }
    }else { /* even index */
        if ((rv = (DRV_SERVICES(unit)->mem_field_set)
            (unit, DRV_MEM_MCAST, DRV_MEM_FIELD_VALID, 
            (uint32 *)&mcast_entry, &temp)) < 0) {
            return rv;
        }
        temp = SOC_PBMP_WORD_GET(bmp, 0);
        if ((rv = (DRV_SERVICES(unit)->mem_field_set)
            (unit, DRV_MEM_MCAST, DRV_MEM_FIELD_DEST_BITMAP, 
            (uint32 *)&mcast_entry, &temp)) < 0) {
            return rv;
        }
    }

    if ((rv = (DRV_SERVICES(unit)->mem_write)
        (unit, DRV_MEM_MCAST, entry_index, 1, 
        (uint32 *)&mcast_entry)) < 0) {
        return rv;
    }

    if( flag & DRV_MCAST_INDEX_REMOVE) {
       
        /* clear MC used info-base */
        L2MC_USED_CLR(unit, index);

    }

    return rv;
}
