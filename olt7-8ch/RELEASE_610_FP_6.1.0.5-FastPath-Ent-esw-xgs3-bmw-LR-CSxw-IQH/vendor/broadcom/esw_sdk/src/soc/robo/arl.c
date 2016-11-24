/*
 * $Id: arl.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
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
 * File:    arlmsg.c
 * Purpose: Keep a synchronized ARL shadow table.
 *      Provide a reliable stream of ARL insert/delete messages.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/time.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/robo.h>

#include <soc/mcm/robo/driver.h>
#include <soc/arl.h>

#define ARL_ENTRY_EQL(unit, e1, e2) \
    ((e1)->entry_data[0] == (e2)->entry_data[0] && \
     (e1)->entry_data[1] == (e2)->entry_data[1] && \
     (e1)->entry_data[2] == (e2)->entry_data[2])

/*
 * While the ARL is frozen, the ARLm lock is held.
 *
 * All tasks must obtain the ARLm lock before modifying the CML bits or
 * age timer registers.
 */

typedef struct freeze_s {
    int                 frozen;
    int                 save_cml[SOC_MAX_NUM_PORTS];
    int                 save_age_sec;
    int                 save_age_ena;
} freeze_t;

STATIC freeze_t arl_freeze_state[SOC_MAX_NUM_DEVICES];

int _drv_bcm5396_arl_hw_to_sw_entry (int unit, uint32 *key, l2_arl_entry_t *hw_arl, l2_arl_sw_entry_t  *sw_arl)
{
    int rv = SOC_E_NONE;
    uint32        temp;

    (DRV_SERVICES(unit)->mem_read)
        (unit, DRV_MEM_ARL, *key, 1, (uint32 *)hw_arl);
    /* Read VALID bit */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VALID, 
            (uint32 *)hw_arl, &temp);

    if (!temp) {
        return rv;
    }
    sal_memcpy(sw_arl, hw_arl, sizeof(l2_arl_entry_t));
    return rv;
}
int _drv_bcm5324_arl_hw_to_sw_entry (int unit, uint32 *key, l2_arl_entry_t *hw_arl, l2_arl_sw_entry_t  *sw_arl)
{
    uint32        temp;
    uint32      vid_rw, mcast_index;
    uint32        reg_addr, reg_value;
    uint64      rw_mac_field, temp_mac_field;
    int              reg_len;
    uint8          hash_value[6];
    uint8       mac_addr_rw[6], temp_mac_addr[6];
    uint16        hash_result;
    int         rv = SOC_E_NONE;

    (DRV_SERVICES(unit)->mem_read)
        (unit, DRV_MEM_ARL, *key, 1, (uint32 *)hw_arl);
    /* Read VALID bit */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VALID, 
            (uint32 *)hw_arl, &temp);

    if (!temp) {
        return rv;
    }

    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, (uint32 *)sw_arl, &temp);


    /* Read VLAN ID value */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VLANID, 
            (uint32 *)hw_arl, &vid_rw);
    
    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, (uint32 *)sw_arl, &vid_rw);
        
    /* Read MAC address bit 12~47 */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_MAC, 
            (uint32 *)hw_arl, (uint32 *)&rw_mac_field);
    SAL_MAC_ADDR_FROM_UINT64(mac_addr_rw, rw_mac_field);

    /* check HASH enabled ? */
    /* check 802.1q enable */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, VLAN_CTRL0r);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, VLAN_CTRL0r, 0, 0);
 
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    SOC_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->reg_field_get)
        (unit, VLAN_CTRL0r, &reg_value, VLAN_ENf, &temp));

    sal_memset(hash_value, 0, sizeof(hash_value));
    if (temp) {
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
                (unit, VLAN_CTRL0r, &reg_value, VLAN_LEARN_MODEf, &temp));
        if (temp == 3) {
            /* hash value = VID + MAC */
            hash_value[0] = (vid_rw >> 4) & 0xff;
            hash_value[1] = ((vid_rw & 0xf) << 4) + (mac_addr_rw[1] & 0xf);
            hash_value[2] = mac_addr_rw[2];
            hash_value[3] = mac_addr_rw[3];
            hash_value[4] = mac_addr_rw[4];
            hash_value[5] = mac_addr_rw[5];
        } else {
            /* hash value = MAC */
            hash_value[0] = ((mac_addr_rw[1] & 0xf) << 4) 
                + ((mac_addr_rw[2] & 0xf0) >> 4);
            hash_value[1] = ((mac_addr_rw[2] & 0xf) << 4) 
                + ((mac_addr_rw[3] & 0xf0) >> 4);
            hash_value[2] = ((mac_addr_rw[3] & 0xf) << 4) 
                + ((mac_addr_rw[4] & 0xf0) >> 4);
            hash_value[3] = ((mac_addr_rw[4] & 0xf) << 4) 
                + ((mac_addr_rw[5] & 0xf0) >> 4);
            hash_value[4] = ((mac_addr_rw[5] & 0xf) << 4);
        }
    } else {
        /* hash value = MAC value */
        hash_value[0] = ((mac_addr_rw[1] & 0xf) << 4) 
            + ((mac_addr_rw[2] & 0xf0) >> 4);
        hash_value[1] = ((mac_addr_rw[2] & 0xf) << 4) 
            + ((mac_addr_rw[3] & 0xf0) >> 4);
        hash_value[2] = ((mac_addr_rw[3] & 0xf) << 4) 
            + ((mac_addr_rw[4] & 0xf0) >> 4);
        hash_value[3] = ((mac_addr_rw[4] & 0xf) << 4) 
            + ((mac_addr_rw[5] & 0xf0) >> 4);
        hash_value[4] = ((mac_addr_rw[5] & 0xf) << 4);
    }

    /* Get the hash revalue */
    _drv_arl_hash(hash_value, 48, &hash_result);
    /* Recover the MAC bit 0~11 */
    temp = *key;
    temp = temp >> 1; 
    hash_result = (hash_result ^ temp) & 0xfff;

    temp_mac_addr[0] = ((mac_addr_rw[1] & 0xf) << 4) 
        + ((mac_addr_rw[2] & 0xf0) >> 4);
    temp_mac_addr[1] = ((mac_addr_rw[2] & 0xf) << 4) 
        + ((mac_addr_rw[3] & 0xf0) >> 4);
    temp_mac_addr[2] = ((mac_addr_rw[3] & 0xf) << 4) 
        + ((mac_addr_rw[4] & 0xf0) >> 4);
    temp_mac_addr[3] = ((mac_addr_rw[4] & 0xf) << 4) 
        + ((mac_addr_rw[5] & 0xf0) >> 4);
    temp_mac_addr[4] = ((mac_addr_rw[5] & 0xf) << 4) 
        + (hash_result >> 8);
    temp_mac_addr[5] = hash_result & 0xff;
    SAL_MAC_ADDR_TO_UINT64(temp_mac_addr, temp_mac_field);
    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC, 
            (uint32 *)sw_arl, (uint32 *)&temp_mac_field);
    if (temp_mac_addr[0] & 0x01) { /* multicast entry */
        /* Multicast index */
       mcast_index = 0;
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_SRC_PORT, 
                (uint32 *)hw_arl, &temp);
        mcast_index = temp;
         (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_PRIORITY, 
                (uint32 *)hw_arl, &temp);
        mcast_index += (temp << 5);
         (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_AGE, 
                (uint32 *)hw_arl, &temp);
        mcast_index += (temp << 7);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_DEST_BITMAP, 
            (uint32 *)sw_arl, &mcast_index);
    } else { /* unicast entry */
        /* Source port number */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_SRC_PORT, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, (uint32 *)sw_arl, &temp);

        /* Priority queue value */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_PRIORITY, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_PRIORITY, (uint32 *)sw_arl, &temp);

        /* Age bit */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_AGE, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_AGE, (uint32 *)sw_arl, &temp);
    }

    /* Static bit */
    (DRV_SERVICES(unit)->mem_field_get)(
        unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_STATIC, 
           (uint32 *)hw_arl, &temp);
    (DRV_SERVICES(unit)->mem_field_set)(
        unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC, (uint32 *)sw_arl, &temp);

    return rv;

}

int _drv_bcm5398_arl_hw_to_sw_entry (int unit, uint32 *key, l2_arl_entry_t *hw_arl, l2_arl_sw_entry_t  *sw_arl)
{
    uint32        temp;
    uint32      vid_rw, mcast_pbmp;
    uint32        reg_addr, reg_value;
    uint64      rw_mac_field, temp_mac_field;
    int              reg_len;
    uint8          hash_value[6];
    uint8       mac_addr_rw[6], temp_mac_addr[6];
    uint16        hash_result;
    int         rv = SOC_E_NONE;

    (DRV_SERVICES(unit)->mem_read)
        (unit, DRV_MEM_ARL, *key, 1, (uint32 *)hw_arl);
    /* Read VALID bit */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VALID, 
            (uint32 *)hw_arl, &temp);

    if (!temp) {
        return rv;
    }

    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, (uint32 *)sw_arl, &temp);


    /* Read VLAN ID value */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VLANID, 
            (uint32 *)hw_arl, &vid_rw);
    
    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, (uint32 *)sw_arl, &vid_rw);
        
    /* Read MAC address bit 12~47 */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_MAC, 
            (uint32 *)hw_arl, (uint32 *)&rw_mac_field);
    SAL_MAC_ADDR_FROM_UINT64(mac_addr_rw, rw_mac_field);

    /* check HASH enabled ? */
    /* check 802.1q enable */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, VLAN_CTRL0r);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, VLAN_CTRL0r, 0, 0);
 
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    SOC_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->reg_field_get)
        (unit, VLAN_CTRL0r, &reg_value, VLAN_ENf, &temp));

    sal_memset(hash_value, 0, sizeof(hash_value));

    if (temp) {
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
                (unit, VLAN_CTRL0r, &reg_value, VLAN_LEARN_MODEf, &temp));
        if (temp == 3) {
            /* hash value = VID + MAC */
            hash_value[0] = (vid_rw >> 4) & 0xff;
            hash_value[1] = ((vid_rw & 0xf) << 4) + (mac_addr_rw[1] & 0xf);
            hash_value[2] = mac_addr_rw[2];
            hash_value[3] = mac_addr_rw[3];
            hash_value[4] = mac_addr_rw[4];
            hash_value[5] = mac_addr_rw[5];
        } else if (temp == 0){
            /* hash value = MAC */
            hash_value[0] = ((mac_addr_rw[1] & 0xf) << 4) 
                + ((mac_addr_rw[2] & 0xf0) >> 4);
            hash_value[1] = ((mac_addr_rw[2] & 0xf) << 4) 
                + ((mac_addr_rw[3] & 0xf0) >> 4);
            hash_value[2] = ((mac_addr_rw[3] & 0xf) << 4) 
                + ((mac_addr_rw[4] & 0xf0) >> 4);
            hash_value[3] = ((mac_addr_rw[4] & 0xf) << 4) 
                + ((mac_addr_rw[5] & 0xf0) >> 4);
            hash_value[4] = ((mac_addr_rw[5] & 0xf) << 4);
        } else {
            return SOC_E_CONFIG;
        }
    } else {
        /* hash value = MAC value */
        hash_value[0] = ((mac_addr_rw[1] & 0xf) << 4) 
            + ((mac_addr_rw[2] & 0xf0) >> 4);
        hash_value[1] = ((mac_addr_rw[2] & 0xf) << 4) 
            + ((mac_addr_rw[3] & 0xf0) >> 4);
        hash_value[2] = ((mac_addr_rw[3] & 0xf) << 4) 
            + ((mac_addr_rw[4] & 0xf0) >> 4);
        hash_value[3] = ((mac_addr_rw[4] & 0xf) << 4) 
            + ((mac_addr_rw[5] & 0xf0) >> 4);
        hash_value[4] = ((mac_addr_rw[5] & 0xf) << 4);
    }

    /* Get the hash revalue */
    _drv_arl_hash(hash_value, 6, &hash_result);
    /* Recover the MAC bit 0~11 */
    temp = *key;
    temp = temp >> 1; 
    hash_result = (hash_result ^ temp) & 0xfff;

    temp_mac_addr[0] = ((mac_addr_rw[1] & 0xf) << 4) 
        + ((mac_addr_rw[2] & 0xf0) >> 4);
    temp_mac_addr[1] = ((mac_addr_rw[2] & 0xf) << 4) 
        + ((mac_addr_rw[3] & 0xf0) >> 4);
    temp_mac_addr[2] = ((mac_addr_rw[3] & 0xf) << 4) 
        + ((mac_addr_rw[4] & 0xf0) >> 4);
    temp_mac_addr[3] = ((mac_addr_rw[4] & 0xf) << 4) 
        + ((mac_addr_rw[5] & 0xf0) >> 4);
    temp_mac_addr[4] = ((mac_addr_rw[5] & 0xf) << 4) 
        + (hash_result >> 8);
    temp_mac_addr[5] = hash_result & 0xff;
    SAL_MAC_ADDR_TO_UINT64(temp_mac_addr, temp_mac_field);
    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC, 
            (uint32 *)sw_arl, (uint32 *)&temp_mac_field);

    if (temp_mac_addr[0] & 0x01) { /* multicast entry */
        /* Multicast index */
        mcast_pbmp = 0;
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_SRC_PORT, 
                (uint32 *)hw_arl, &temp);
        mcast_pbmp = temp;
         (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_DEST_BITMAP1, 
                (uint32 *)hw_arl, &temp);
        mcast_pbmp += (temp << 4);
         (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_AGE, 
                (uint32 *)hw_arl, &temp);
        mcast_pbmp += (temp << 8);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_MARL, DRV_MEM_FIELD_DEST_BITMAP, 
            (uint32 *)sw_arl, &mcast_pbmp);
    } else { /* unicast entry */
        /* Source port number */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_SRC_PORT, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, (uint32 *)sw_arl, &temp);

        /* Priority queue value */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_PRIORITY, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_PRIORITY, (uint32 *)sw_arl, &temp);

        /* Age bit */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_AGE, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_AGE, (uint32 *)sw_arl, &temp);
    }

    /* Static bit */
    (DRV_SERVICES(unit)->mem_field_get)(
        unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_STATIC, 
           (uint32 *)hw_arl, &temp);
    (DRV_SERVICES(unit)->mem_field_set)(
        unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC, (uint32 *)sw_arl, &temp);

    return rv;
}

int _drv_bcm53242_arl_hw_to_sw_entry (int unit, uint32 *key, l2_arl_entry_t *hw_arl, l2_arl_sw_entry_t  *sw_arl)
{
    uint32        temp;
    uint32      vid_rw, mcast_index;
    uint32        reg_addr, reg_value;
    uint64      rw_mac_field, temp_mac_field;
    int              reg_len;
    uint8          hash_value[6];
    uint8       mac_addr_rw[6], temp_mac_addr[6];
    uint16        hash_result;
    int         rv = SOC_E_NONE;

    (DRV_SERVICES(unit)->mem_read)
        (unit, DRV_MEM_ARL, *key, 1, (uint32 *)hw_arl);
    /* Read VALID bit */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VALID, 
            (uint32 *)hw_arl, &temp);
    if (!temp) {
        return rv;
    }

    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, (uint32 *)sw_arl, &temp);


    /* Read VLAN ID value */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_VLANID, 
            (uint32 *)hw_arl, &vid_rw);

    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, (uint32 *)sw_arl, &vid_rw);
        
    /* Read MAC address bit 12~47 */
    (DRV_SERVICES(unit)->mem_field_get)
        (unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_MAC, 
            (uint32 *)hw_arl, (uint32 *)&rw_mac_field);
    SAL_MAC_ADDR_FROM_UINT64(mac_addr_rw, rw_mac_field);

    /* check HASH enabled ? */
    /* check 802.1q enable */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, VLAN_CTRL0r);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, VLAN_CTRL0r, 0, 0);
 
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    SOC_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->reg_field_get)
        (unit, VLAN_CTRL0r, &reg_value, VLAN_ENf, &temp));

    sal_memset(hash_value, 0, sizeof(hash_value));
    if (temp) {
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
                (unit, VLAN_CTRL0r, &reg_value, VLAN_LEARN_MODEf, &temp));
        if (temp == 3) {
            /* hash value = VID + MAC */
            hash_value[0] = (vid_rw >> 4) & 0xff;
            hash_value[1] = ((vid_rw & 0xf) << 4) + (mac_addr_rw[1] & 0xf);
            hash_value[2] = mac_addr_rw[2];
            hash_value[3] = mac_addr_rw[3];
            hash_value[4] = mac_addr_rw[4];
            hash_value[5] = mac_addr_rw[5];
        } else {
            /* hash value = MAC */
            hash_value[0] = ((mac_addr_rw[1] & 0xf) << 4) 
                + ((mac_addr_rw[2] & 0xf0) >> 4);
            hash_value[1] = ((mac_addr_rw[2] & 0xf) << 4) 
                + ((mac_addr_rw[3] & 0xf0) >> 4);
            hash_value[2] = ((mac_addr_rw[3] & 0xf) << 4) 
                + ((mac_addr_rw[4] & 0xf0) >> 4);
            hash_value[3] = ((mac_addr_rw[4] & 0xf) << 4) 
                + ((mac_addr_rw[5] & 0xf0) >> 4);
            hash_value[4] = ((mac_addr_rw[5] & 0xf) << 4);
        }
    } else {
        /* hash value = MAC value */
        hash_value[0] = ((mac_addr_rw[1] & 0xf) << 4) 
            + ((mac_addr_rw[2] & 0xf0) >> 4);
        hash_value[1] = ((mac_addr_rw[2] & 0xf) << 4) 
            + ((mac_addr_rw[3] & 0xf0) >> 4);
        hash_value[2] = ((mac_addr_rw[3] & 0xf) << 4) 
            + ((mac_addr_rw[4] & 0xf0) >> 4);
        hash_value[3] = ((mac_addr_rw[4] & 0xf) << 4) 
            + ((mac_addr_rw[5] & 0xf0) >> 4);
        hash_value[4] = ((mac_addr_rw[5] & 0xf) << 4);
    }

    /* Get the hash revalue */
    _drv_arl_hash(hash_value, 47, &hash_result);

    /* Recover the MAC bit 0~11 */
    temp = *key;
    temp = temp >> 2; 
    hash_result = (hash_result ^ temp) & 0xfff;

    temp_mac_addr[0] = ((mac_addr_rw[1] & 0xf) << 4) 
        + ((mac_addr_rw[2] & 0xf0) >> 4);
    temp_mac_addr[1] = ((mac_addr_rw[2] & 0xf) << 4) 
        + ((mac_addr_rw[3] & 0xf0) >> 4);
    temp_mac_addr[2] = ((mac_addr_rw[3] & 0xf) << 4) 
        + ((mac_addr_rw[4] & 0xf0) >> 4);
    temp_mac_addr[3] = ((mac_addr_rw[4] & 0xf) << 4) 
        + ((mac_addr_rw[5] & 0xf0) >> 4);
    temp_mac_addr[4] = ((mac_addr_rw[5] & 0xf) << 4) 
        + (hash_result >> 8);
    temp_mac_addr[5] = hash_result & 0xff;
    SAL_MAC_ADDR_TO_UINT64(temp_mac_addr, temp_mac_field);

    (DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC, 
            (uint32 *)sw_arl, (uint32 *)&temp_mac_field);
    if (temp_mac_addr[0] & 0x01) { /* multicast entry */
        /* Multicast index */
       mcast_index = 0;
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_SRC_PORT, 
                (uint32 *)hw_arl, &temp);
        mcast_index = temp;
         (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_PRIORITY, 
                (uint32 *)hw_arl, &temp);
        mcast_index += (temp << 6);
         (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_AGE, 
                (uint32 *)hw_arl, &temp);
        mcast_index += (temp << 11);

        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_DEST_BITMAP, 
            (uint32 *)sw_arl, &mcast_index);
    } else { /* unicast entry */
        /* Source port number */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_SRC_PORT, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT, (uint32 *)sw_arl, &temp);

        /* Age bit */
        (DRV_SERVICES(unit)->mem_field_get)(
            unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_AGE, 
                (uint32 *)hw_arl, &temp);
        (DRV_SERVICES(unit)->mem_field_set)(
            unit, DRV_MEM_ARL, DRV_MEM_FIELD_AGE, (uint32 *)sw_arl, &temp);
    }

    /* Static bit */
    (DRV_SERVICES(unit)->mem_field_get)(
        unit, DRV_MEM_ARL_HW, DRV_MEM_FIELD_STATIC, 
           (uint32 *)hw_arl, &temp);
    (DRV_SERVICES(unit)->mem_field_set)(
        unit, DRV_MEM_ARL, DRV_MEM_FIELD_STATIC, (uint32 *)sw_arl, &temp);

    return rv;
}

int
drv_arl_table_process(int unit, uint32 *key, void *hw_arl, void  *sw_arl)
{
    int rv = SOC_E_NONE;

    if (SOC_IS_ROBO5324(unit)) {
        rv = _drv_bcm5324_arl_hw_to_sw_entry
            (unit, key, (l2_arl_entry_t * )hw_arl, (l2_arl_sw_entry_t *) sw_arl);
    }
    
    if (SOC_IS_ROBO5396(unit)) {
        rv = _drv_bcm5396_arl_hw_to_sw_entry
            (unit, key, (l2_arl_entry_t * )hw_arl, (l2_arl_sw_entry_t *) sw_arl);
    }

    if (SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit)) {
        rv = _drv_bcm5398_arl_hw_to_sw_entry
            (unit, key, (l2_arl_entry_t * )hw_arl, (l2_arl_sw_entry_t *) sw_arl);
    }

    if (SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)) {
        rv = _drv_bcm53242_arl_hw_to_sw_entry
            (unit, key, (l2_arl_entry_t * )hw_arl, (l2_arl_sw_entry_t *) sw_arl);
    }

    if (SOC_IS_ROBO5348(unit) ||SOC_IS_ROBO5347(unit)) {
        /* Temporary unavailable since it's unable to get ARL by memory address now */
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}


/*
 * Function:
 *  drv_arl_sync (internal)
 * Purpose:
 *  Compare old ARL contents to new and synchronize shadow table.
 * Parameters:
 *  unit    - RoboSwitch unit #
 *  old_arl - Results of previous ARL entry
 *  new_arl - Results of current ARL entry
 */
int
drv_arl_sync(int unit, uint32 *key, void *old_arl, void *new_arl)
{
    int rv = SOC_E_NONE;
    sal_usecs_t     stime, etime;
    l2_arl_sw_entry_t   *old, *new;
    uint64  old_mac_field, new_mac_field;
    uint8   old_mac_addr[6], new_mac_addr[6];
    int old_valid = 0, new_valid = 0;
    uint32  old_vid = 0, new_vid = 0;
    uint32  old_port = 0, new_port = 0;
    soc_cm_debug(DK_ARL+DK_VERBOSE, "soc_arl_sync: start\n");

    stime = sal_time_usecs();

    /*
     * Do fast compare to skip entries that have not changed at all;
     * normally this is the case for most entries.
     */
        
    old = (l2_arl_sw_entry_t *)old_arl;
    new = (l2_arl_sw_entry_t *)new_arl;
              
    if (!ARL_ENTRY_EQL(unit, old, new)) {
                
        soc_cm_debug(DK_ARL, "arl_sync : old = %x, %x, %x new = %x, %x, %x\n",
            old->entry_data[2],old->entry_data[1], old->entry_data[0],
                new->entry_data[2],new->entry_data[1], new->entry_data[0]);
       
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
            (uint32 *)old, (uint32 *) &old_valid);       
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID, 
            (uint32 *)new, (uint32 *) &new_valid); 
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC, 
            (uint32 *)old, (uint32 *)&old_mac_field); 
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_MAC, 
            (uint32 *)new, (uint32 *)&new_mac_field); 
        SAL_MAC_ADDR_FROM_UINT64(old_mac_addr, old_mac_field);
        SAL_MAC_ADDR_FROM_UINT64(new_mac_addr, new_mac_field);
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, 
            (uint32 *)old, &old_vid);
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VLANID, 
            (uint32 *)new, &new_vid);
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT,
            (uint32 *)old, &old_port);
        (DRV_SERVICES(unit)->mem_field_get)
            (unit, DRV_MEM_ARL, DRV_MEM_FIELD_SRC_PORT,
            (uint32 *)new, &new_port);

       
        soc_cm_debug(DK_ARL,"old_valid %x old_mac_addr %02x:%02x:%02x:%02x:%02x:%02x \
           old_vid %x old_port %d\n",
            old_valid,old_mac_addr[0],old_mac_addr[1],
            old_mac_addr[2],old_mac_addr[3],
            old_mac_addr[4],old_mac_addr[5],old_vid, old_port);
        soc_cm_debug(DK_ARL,"new_valid %x new_mac_addr %02x:%02x:%02x:%02x:%02x:%02x \
            new_vid %x new_port %d\n",
            new_valid,new_mac_addr[0],new_mac_addr[1],
            new_mac_addr[2],new_mac_addr[3],
            new_mac_addr[4],new_mac_addr[5],new_vid, new_port);

        if (old_valid && new_valid) {
            if (!((old_mac_addr[0] & 0x01) && (new_mac_addr[0] & 0x01))) {
                soc_cm_debug(DK_ARL,"OLD NEW ALL VALID\n");
                if (!(sal_memcmp(old_mac_addr, new_mac_addr, 6) == 0) || 
                    (old_vid != new_vid) || (old_port != new_port)) {
                    soc_cm_debug(DK_ARL, "MAC or VID different\n");
                    soc_robo_arl_callback(unit, (l2_arl_sw_entry_t *)old_arl, 
                        (l2_arl_sw_entry_t *)new_arl);
                }
            }
        } else if (old_valid) {
            if (!(old_mac_addr[0] & 0x01)) {
                soc_cm_debug(DK_ARL, "OLD ENTRY DELETE\n");
                soc_robo_arl_callback(unit, (l2_arl_sw_entry_t *)old_arl, NULL);
            }
        } else if (new_valid) {
            if (!(new_mac_addr[0] & 0x01)) {
                soc_cm_debug(DK_ARL, "NEW ENTRY ADD\n");
                soc_robo_arl_callback(unit, NULL, (l2_arl_sw_entry_t *)new_arl);
            }
        }
        if (!(new_mac_addr[0] & 0x01)) {
            sal_memcpy(old,new,sizeof(l2_arl_sw_entry_t));
        }
    }

    etime = sal_time_usecs();

    soc_cm_debug(DK_ARL+DK_VERBOSE,
        "soc_arl_sync: done in %d usec\n",
        SAL_USECS_SUB(etime, stime));
    return rv;
}

/*
 * Function:
 *  soc_robo_arl_freeze
 * Purpose:
 *  Temporarily quiesce ARL from all activity (learning, aging)
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *  SOC_E_NONE      Success
 *  SOC_E_XXX       Error (tables not locked)
 * Notes:
 *  Leaves ARLm locked until corresponding thaw.
 *  PTABLE is locked in order to lockout bcm_port calls
 *  bcm_port calls will callout to soc_arl_frozen_cml_set/get
 */

int
soc_robo_arl_freeze(int unit)
{
    freeze_t        *f = &arl_freeze_state[unit];
    soc_port_t      port;
    soc_pbmp_t       pbmp;
    int         rv;

    /*
     * First time through: lock arl, disable learning and aging
     */
    soc_mem_lock(unit, L2_ARLm);
    if (f->frozen++) {
        soc_mem_unlock(unit, L2_ARLm);
        return SOC_E_NONE;      /* Recursive freeze OK */
    }
    /* disable learning */
    PBMP_E_ITER(unit, port) {
        rv = (DRV_SERVICES(unit)->arl_learn_enable_get)(unit, 
                port, (uint32 *) &f->save_cml[port]);
        if (rv < 0) {
            goto fail;
        }
    }
    pbmp = PBMP_E_ALL(unit);

    rv = (DRV_SERVICES(unit)->arl_learn_enable_set)(unit,
            pbmp, DRV_PORT_DISABLE_LEARN);
    if (rv < 0) {
        goto fail;
    }
    /* disable aging */
    rv = (DRV_SERVICES(unit)->age_timer_get)(unit,
                        (uint32 *) &f->save_age_ena,
                        (uint32 *) &f->save_age_sec);
    if (rv < 0) {
    goto fail;
    }
    if (f->save_age_ena) {
        rv = (DRV_SERVICES(unit)->age_timer_set)(unit,
                        0,
                        f->save_age_sec);
    if (rv < 0) {
            goto fail;
        }
    }

    return SOC_E_NONE;

 fail:
    f->frozen--;
    soc_mem_unlock(unit, L2_ARLm);
    return rv;
}

/*
 * Function:
 *  soc_arl_thaw
 * Purpose:
 *  Restore normal ARL activity.
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *  Unlocks ARLm.
 */

int
soc_robo_arl_thaw(int unit)
{
    freeze_t        *f = &arl_freeze_state[unit];
    soc_port_t      port;
    int         rv,cml;
    soc_pbmp_t       pbm;

    assert(f->frozen > 0);

    if (--f->frozen) {
    return SOC_E_NONE;      /* Thaw a recursive freeze */
    }

    /*
     * Last thaw enables learning and aging, and unlocks arl
     */
    rv = SOC_E_NONE;

    
    PBMP_E_ITER(unit, port) {
        cml = f->save_cml[port];
        if (cml) {
            SOC_PBMP_CLEAR(pbm);
            SOC_PBMP_PORT_SET(pbm,port);
            rv = (DRV_SERVICES(unit)->arl_learn_enable_set)(unit,
                pbm, cml);

            if (rv < 0) {
                goto fail;
            }
        }
    }

    if (f->save_age_ena) {
        rv = (DRV_SERVICES(unit)->age_timer_set)(unit,
                        f->save_age_ena,
                        f->save_age_sec);
    if (rv < 0) {
        goto fail;
    }
    }
 fail:
    soc_mem_unlock(unit, L2_ARLm);
    return rv;
}

/*
 * Function:
 *  drv_arl_learn_enable_set
 * Purpose:
 *  Setting per port SA learning process.
 * Parameters:
 *  unit    - RoboSwitch unit #
 *  pbmp    - port bitmap
 *  mode   - DRV_PORT_HW_LEARN
 *               DRV_PORT_DISABLE_LEARN
 *               DRV_PORT_SW_LEARN
 */
int
drv_arl_learn_enable_set(int unit, soc_pbmp_t pbmp, uint32 mode)
{
    int     rv = SOC_E_NONE;
    uint32  reg_addr, reg_v32, fld_v32;
    int     reg_len;
    uint64  reg_v64, fld_v64;
    soc_pbmp_t current_pbmp, temp_pbmp;
    soc_port_t port;

    switch (mode ) {
        case DRV_PORT_HW_LEARN:
            /* bcm5324 can support HW_LEARN only 
             *  - return SOC_E_NONE to avoid the port info restoring failure.
             */
            if (SOC_IS_ROBO5324(unit)){
                return SOC_E_NONE;
            }
        case DRV_PORT_DISABLE_LEARN:
    if(SOC_IS_ROBO5348(unit)|| SOC_IS_ROBO5347(unit) ||
        SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit)) {
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, DIS_LEARNr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, DIS_LEARNr, 0, 0);
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, PORT_SEC_CONr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, PORT_SEC_CONr, 0, 0);
    }else{
        return SOC_E_UNAVAIL;
    }

    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, (uint32 *)&reg_v64, reg_len)) < 0) {
            return rv;
        }
    
        if (SOC_IS_ROBO5348(unit)) {
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, DIS_LEARNr, (uint32 *)&reg_v64, DIS_LEARNf, (uint32 *)&fld_v64));
        soc_robo_64_val_to_pbmp(unit, &current_pbmp, fld_v64);
        } else { /* 5347 */
            SOC_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->reg_field_get)
                (unit, DIS_LEARNr, (uint32 *)&reg_v64, DIS_LEARNf, &fld_v32));
            SOC_PBMP_WORD_SET(current_pbmp, 0, fld_v32);
        }
    
        if (mode == DRV_PORT_HW_LEARN) { /* enable */
            SOC_PBMP_CLEAR(temp_pbmp);
            SOC_PBMP_NEGATE(temp_pbmp, pbmp);
            SOC_PBMP_AND(current_pbmp, temp_pbmp);
        } else { /* disable */
            SOC_PBMP_OR(current_pbmp, pbmp);
        }
    
        if (SOC_IS_ROBO5348(unit)) {
        soc_robo_64_pbmp_to_val(unit, &current_pbmp, &fld_v64);
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, DIS_LEARNr, (uint32 *)&reg_v64, DIS_LEARNf, (uint32 *)&fld_v64));
        } else { /* 5347 */
            fld_v32 = SOC_PBMP_WORD_GET(current_pbmp, 0);
            SOC_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->reg_field_set)
                (unit, DIS_LEARNr, (uint32 *)&reg_v64, DIS_LEARNf, &fld_v32));
        }
    
        if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, (uint32 *)&reg_v64, reg_len)) < 0) {
            return rv;
        }
    } else if (SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit)) {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_v32, reg_len)) < 0) {
            return rv;
        }
    
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, DIS_LEARNr, &reg_v32, DIS_LEARNf, &fld_v32));
        SOC_PBMP_WORD_SET(current_pbmp, 0, fld_v32);
    
        if (mode == DRV_PORT_HW_LEARN) { /* enable */
            SOC_PBMP_CLEAR(temp_pbmp);
            SOC_PBMP_NEGATE(temp_pbmp, pbmp);
            SOC_PBMP_AND(current_pbmp, temp_pbmp);
        } else { /* disable */
            SOC_PBMP_OR(current_pbmp, pbmp);
        }

        fld_v32 = SOC_PBMP_WORD_GET(current_pbmp, 0);
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, DIS_LEARNr, &reg_v32, DIS_LEARNf, &fld_v32));
    
        if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_v32, reg_len)) < 0) {
            return rv;
        }
     } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        /* per port */
        PBMP_ITER(pbmp, port) {
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, PORT_SEC_CONr, port, 0);
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_v32, reg_len)) < 0) {
                return rv;
            }

            if (mode == DRV_PORT_HW_LEARN) { /* enable */
                fld_v32 = 0;
            } else { /* disable */
                fld_v32 = 1;
            }
            SOC_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->reg_field_set)
                (unit, PORT_SEC_CONr, &reg_v32, DIS_LEARNf, &fld_v32));
        
            if ((rv = (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_v32, reg_len)) < 0) {
                return rv;
            }
        }
    } else {
        rv = SOC_E_UNAVAIL;
    }
    break;

   /* no support section */
   case DRV_PORT_SW_LEARN:
   case DRV_PORT_HW_SW_LEARN:
   case DRV_PORT_DROP:
   case DRV_PORT_SWLRN_DROP:
   case DRV_PORT_HWLRN_DROP:
   case DRV_PORT_SWHWLRN_DROP:
       rv = SOC_E_UNAVAIL;
       break;
   default:
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *  drv_arl_learn_enable_get
 * Purpose:
 *  Setting per port SA learning process.
 * Parameters:
 *  unit    - RoboSwitch unit #
 *  port    - port
 *  mode   - Port learn mode
 */
int
drv_arl_learn_enable_get(int unit, soc_port_t port, uint32 *mode)
{
    int     rv = SOC_E_NONE;
    uint32  reg_addr, reg_v32, fld_v32 = 0;
    int     reg_len;
    uint64  reg_v64, fld_v64;
    soc_pbmp_t current_pbmp;

    if(SOC_IS_ROBO5348(unit) ||SOC_IS_ROBO5347(unit) || 
        SOC_IS_ROBO5398(unit) || SOC_IS_ROBO5397(unit)){
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, DIS_LEARNr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, DIS_LEARNr, 0, 0);
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, PORT_SEC_CONr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, PORT_SEC_CONr, port, 0);
    } else {
        return SOC_E_UNAVAIL;
    }

    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, (uint32 *)&reg_v64, reg_len)) < 0) {
            return rv;
        }
    
        if (SOC_IS_ROBO5348(unit)) {
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, DIS_LEARNr, (uint32 *)&reg_v64, DIS_LEARNf, (uint32 *)&fld_v64));
        soc_robo_64_val_to_pbmp(unit, &current_pbmp, fld_v64);
        } else { /* 5347 */
            SOC_IF_ERROR_RETURN(
                (DRV_SERVICES(unit)->reg_field_get)
                (unit, DIS_LEARNr, (uint32 *)&reg_v64, DIS_LEARNf, &fld_v32));
            SOC_PBMP_WORD_SET(current_pbmp, 0, fld_v32);
        }
    } else if (SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit)) {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_v32, reg_len)) < 0) {
            return rv;
        }
    
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, DIS_LEARNr, &reg_v32, DIS_LEARNf, &fld_v32));
        SOC_PBMP_WORD_SET(current_pbmp, 0, fld_v32);
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_v32, reg_len)) < 0) {
            return rv;
        }

        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, PORT_SEC_CONr, &reg_v32, DIS_LEARNf, &fld_v32));
    } else {
        return SOC_E_UNAVAIL;
    }

    if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        if (fld_v32) {
            *mode = DRV_PORT_DISABLE_LEARN; /* This port is in DISABLE SA learn state */
        } else {
            *mode = DRV_PORT_HW_LEARN;
        }
    } else {
    if (SOC_PBMP_MEMBER(current_pbmp, port)) {
        *mode = DRV_PORT_DISABLE_LEARN; /* This port is in DISABLE SA learn state */
    } else {
        *mode = DRV_PORT_HW_LEARN;
    }
    }
    return rv;
}

