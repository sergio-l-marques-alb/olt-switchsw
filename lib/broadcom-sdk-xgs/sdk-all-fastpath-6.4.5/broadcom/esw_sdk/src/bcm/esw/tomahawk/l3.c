/*
 * $Id$
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
 * File:    l3.c
 * Purpose: Tomahawk L3 function implementations
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(INCLUDE_L3) && defined(BCM_TOMAHAWK_SUPPORT) 

#include <soc/drv.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#endif
#include <soc/tomahawk.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/trident2.h>

#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/qos.h>

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#define _BCM_TD2_L3_MEM_BANKS_ALL     (-1)
#define _BCM_TD2_HOST_ENTRY_NOT_FOUND (-1)
static soc_profile_mem_t *_bcm_th_macda_oui_profile[BCM_MAX_NUM_UNITS];
static soc_profile_mem_t *_bcm_th_vntag_etag_profile[BCM_MAX_NUM_UNITS];


/* IP Options Handling */
#define L3_INFO(_unit_) (&_bcm_l3_bk_info[_unit_])
#define _BCM_IP_OPTION_PROFILE_CHUNK 256
#define _BCM_IP4_OPTIONS_LEN    \
            (soc_mem_index_count(unit, IP_OPTION_CONTROL_PROFILE_TABLEm)/ \
             _BCM_IP_OPTION_PROFILE_CHUNK)
#ifdef ALPM_ENABLE
#define ALPM_PREFIX_IN_TCAM(u, vrf_id) ((vrf_id == SOC_L3_VRF_OVERRIDE) || \
                  ((soc_th_alpm_mode_get(u) == SOC_ALPM_MODE_TCAM_ALPM) && \
                                        (vrf_id == SOC_L3_VRF_GLOBAL)))

/* Get available bank based on given bank_disable bitmap 
 * - bank_num : a return value for next available bank id 
 * - bank_count: total bank number 
 * - bank_disable: disabled bank bitmap 
 */
#define ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable) \
        do { \
            while ((bank_disable & (1U << bank_num)) && \
                   (bank_num < bank_count)) { \
                bank_num ++; \
            } \
        } while (0)

extern int soc_th_get_alpm_banks(int unit);

/*
 * Function:	
 *     _bcm_th_mem_ip6_alpm_set
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Parameters: 
 *    unit    - (IN) SOC unit number; 
 *    lpm_key - (OUT) Buffer to fill. 
 *    lpm_cfg - (IN) Route information. 
 *    alpm_entry - (OUT) ALPM entry to fill
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_th_mem_ip6_alpm_set(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                         defip_entry_t *lpm_key)
{
    bcm_ip6_t mask;                      /* Subnet mask.        */
    uint8 *ip6;                 /* Ip6 address.       */
    uint32 ip6_word[2];         /* Temp storage.      */
    int idx;                    /* Iteration index .  */

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply subnet mask */
    idx = lpm_cfg->defip_sub_len / 8;   /* Unchanged byte count.    */
    ip6[idx] &= mask[idx];      /* Apply mask on next byte. */
    for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
        ip6[idx] = 0;           /* Reset rest of bytes.     */
    }

    ip6_word[1] = ((ip6[0] << 24) | (ip6[1] << 16) | (ip6[2] << 8) | (ip6[3]));
    ip6_word[0]= ((ip6[4] << 24) | (ip6[5] << 16) | (ip6[6] << 8) | (ip6[7]));

    /* Set IP Addr */
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR1f, (void *)&ip6_word[1]);
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR0f, (void *)&ip6_word[0]);

    /* Set IP MASK */
    ip6_word[0] = ((mask[0] << 24) | (mask[1] << 16) | (mask[2] << 8) | (mask[3]));
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR_MASK1f, (void *)&ip6_word[0]);

    ip6_word[0] = ((mask[4] << 24) | (mask[5] << 16) | (mask[6] << 8) | (mask[7]));
    soc_L3_DEFIPm_field_set(unit, lpm_key, IP_ADDR_MASK0f, (void *)&ip6_word[0]);
}


/*
 * Function:
 *     _bcm_th_mem_ip6_alpm_get
 * Purpose:
 *    Get an IP6 address from field L3_DEFIPm
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
void
_bcm_th_mem_ip6_alpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                         soc_mem_t mem, void *alpm_entry)
{
    uint8 *ip6;                         /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word[2];                 /* Temp storage.      */
    uint32 length;
    uint32 *bufp;

    bufp = (uint32 *)alpm_entry;

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));

    soc_mem_field_get(unit, mem, bufp, KEYf, ip6_word);
    ip6[0] = (uint8) ((ip6_word[1] >> 24) & 0xff);
    ip6[1] = (uint8) ((ip6_word[1] >> 16) & 0xff);
    ip6[2] = (uint8) ((ip6_word[1] >> 8) & 0xff);
    ip6[3] = (uint8) (ip6_word[1] & 0xff);

    ip6[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
    ip6[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
    ip6[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
    ip6[7] = (uint8) (ip6_word[0] & 0xff);

    length = soc_mem_field32_get(unit, mem, bufp, LENGTHf);
    lpm_cfg->defip_sub_len = length;
}

/*
 * Function:
 *     _th_defip_pair128_ip6_mask_get
 * Purpose:
 *     Read IP6 mask field from memory field to ip6_addr_t buffer.
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_th_defip_pair128_ip6_mask_get(int unit, soc_mem_t mem, const void *entry,
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK0_LWRf, (uint32 *)&ip6_field[3]);
    ip6[12] = (uint8) (ip6_field[3] >> 24);
    ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
    ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
    ip6[15] = (uint8) (ip6_field[3] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK1_LWRf, (uint32 *)&ip6_field[2]);
    ip6[8] = (uint8) (ip6_field[2] >> 24);
    ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
    ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
    ip6[11] = (uint8) (ip6_field[2] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK0_UPRf, (uint32 *)&ip6_field[1]);
    ip6[4] = (uint8) (ip6_field[1] >> 24);
    ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
    ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
    ip6[7] =(uint8) (ip6_field[1] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR_MASK1_UPRf, ip6_field);
    ip6[0] =(uint8) (ip6_field[0] >> 24);
    ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
    ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
    ip6[3] =(uint8) (ip6_field[0] & 0xff);
}

/*
 * Function:
 *     _th_defip_pair128_ip6_addr_get
 * Purpose:
 *     Read IP6 address field from memory field to ip6_addr_t buffer.
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (OUT) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_th_defip_pair128_ip6_addr_get(int unit, soc_mem_t mem, const void *entry,
                                ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    soc_mem_field_get(unit, mem, entry, IP_ADDR0_LWRf, (uint32 *)&ip6_field[3]);
    ip6[12] = (uint8) (ip6_field[3] >> 24);
    ip6[13] = (uint8) (ip6_field[3] >> 16 & 0xff);
    ip6[14] = (uint8) (ip6_field[3] >> 8 & 0xff);
    ip6[15] = (uint8) (ip6_field[3] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR1_LWRf, (uint32 *)&ip6_field[2]);
    ip6[8] = (uint8) (ip6_field[2] >> 24);
    ip6[9] = (uint8) (ip6_field[2] >> 16 & 0xff);
    ip6[10] = (uint8) (ip6_field[2] >> 8 & 0xff);
    ip6[11] = (uint8) (ip6_field[2] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR0_UPRf, (uint32 *)&ip6_field[1]);
    ip6[4] = (uint8) (ip6_field[1] >> 24);
    ip6[5] = (uint8) (ip6_field[1] >> 16 & 0xff);
    ip6[6] =(uint8) (ip6_field[1] >> 8 & 0xff);
    ip6[7] =(uint8) (ip6_field[1] & 0xff);
    soc_mem_field_get(unit, mem, entry, IP_ADDR1_UPRf, ip6_field);
    ip6[0] =(uint8) (ip6_field[0] >> 24);
    ip6[1] =(uint8) (ip6_field[0] >> 16 & 0xff);
    ip6[2] =(uint8) (ip6_field[0] >> 8 & 0xff);
    ip6[3] =(uint8) (ip6_field[0] & 0xff);
}

/*
 * Function:    
 *     _th_defip_pair128_ip6_addr_set
 * Purpose:  
 *     Set IP6 address field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_th_defip_pair128_ip6_addr_set(int unit, soc_mem_t mem, uint32 *entry, 
                               const ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                    (ip6[14] << 8) | (ip6[15] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR0_LWRf, &ip6_field[3]);
    ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                    (ip6[10] << 8) | (ip6[11] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR1_LWRf, &ip6_field[2]);
    ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                    (ip6[6] << 8)  | (ip6[7] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR0_UPRf, &ip6_field[1]);
    ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                    (ip6[2] << 8)  | (ip6[3] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR1_UPRf, ip6_field);
}

/*
 * Function:    
 *     _th_defip_pair128_ip6_mask_set
 * Purpose:  
 *     Set IP6 mask field in memory from ip6 addr type. 
 * Parameters: 
 *     unit  - (IN) BCM device number. 
 *     mem   - (IN) Memory id.
 *     entry - (IN) HW entry buffer.
 *     ip6   - (IN) SW ip6 address buffer.
 * Returns:      void
 */
STATIC void
_th_defip_pair128_ip6_mask_set(int unit, soc_mem_t mem, uint32 *entry, 
                               const ip6_addr_t ip6)
{
    uint32              ip6_field[4];
    ip6_field[3] = ((ip6[12] << 24)| (ip6[13] << 16) |
                    (ip6[14] << 8) | (ip6[15] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK0_LWRf, &ip6_field[3]);
    ip6_field[2] = ((ip6[8] << 24) | (ip6[9] << 16) |
                    (ip6[10] << 8) | (ip6[11] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK1_LWRf, &ip6_field[2]);
    ip6_field[1] = ((ip6[4] << 24) | (ip6[5] << 16) |
                    (ip6[6] << 8)  | (ip6[7] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK0_UPRf, &ip6_field[1]);
    ip6_field[0] = ((ip6[0] << 24) | (ip6[1] << 16) |
                    (ip6[2] << 8)  | (ip6[3] << 0));
    soc_mem_field_set(unit, mem, entry, IP_ADDR_MASK1_UPRf, ip6_field);
}

/*
 * Function:
 *      bcm_th_internal_lpm_vrf_calc
 * Purpose:
 *      Service routine used to translate API vrf id to hw specific.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_cfg   - (IN)Prefix info.
 *      vrf_id    - (OUT)Internal vrf id.
 *      vrf_mask  - (OUT)Internal vrf mask.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_internal_lpm_vrf_calc(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                             int *vrf_id, int *vrf_mask)
{
    /* Handle special vrf id cases. */
    switch (lpm_cfg->defip_vrf) {
      case BCM_L3_VRF_OVERRIDE:
      case BCM_L3_VRF_GLOBAL: /* only for td2 */
          *vrf_id = 0;
          *vrf_mask = 0;
          break;
      default:   
          *vrf_id = lpm_cfg->defip_vrf;
          *vrf_mask = SOC_VRF_MAX(unit);
    }

    /* In any case vrf id shouldn't exceed max field mask. */
    if ((*vrf_id < 0) || (*vrf_id > SOC_VRF_MAX(unit))) {
        return (BCM_E_PARAM);
    } 
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th_alpm_ent_init
 * Purpose:
 *      Service routine used to initialize lkup key for lpm entry.
 *      Also initializes the ALPM SRAM entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      lpm_cfg   - (IN)Prefix info.
 *      lpm_entry - (OUT)Hw buffer to fill.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th_alpm_ent_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
                      defip_entry_t *lpm_entry, int nh_ecmp_idx, uint32 *flags)
{
    bcm_ip_t ip4_mask;
    int vrf_id;
    int vrf_mask;
    int ipv6;

    ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (bcm_th_internal_lpm_vrf_calc(unit, lpm_cfg, &vrf_id, &vrf_mask));


    /* Zero buffers. */
    sal_memset(lpm_entry, 0, BCM_XGS3_L3_ENT_SZ(unit, defip));

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, HIT0f, 1);
    }

    /* Set priority override bit. */
    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, RPE0f, 1);
    }

    /* Write priority field. */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, PRI0f, lpm_cfg->defip_prio);

    /* Fill next hop information. */
    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, ECMP0f, 1);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, NEXT_HOP_INDEX0f, 
                                  nh_ecmp_idx);
    } else {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, NEXT_HOP_INDEX0f,
                                  nh_ecmp_idx);
    }


    /* Set destination discard flag. */
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, DST_DISCARD0f, 1);
    }

    /* remember src discard flag */
    if (lpm_cfg->defip_flags & BCM_L3_SRC_DISCARD) {
        *flags |= SOC_ALPM_RPF_SRC_DISCARD;
    }

    /* Set classification group id. */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, CLASS_ID0f, 
                              lpm_cfg->defip_lookup_class);

    /* Set Global route flag. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f, 0x1);
    }

    /* Indicate this is an override entry */
    if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_HIGH0f, 0x1);
        soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, GLOBAL_ROUTE0f, 0x1);
    }

    /* Set VRF */
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_0f, vrf_id);
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_MASK0f, vrf_mask);

    if (ipv6) {
        /* Set prefix ip address & mask. */
        _bcm_th_mem_ip6_alpm_set(unit, lpm_cfg,lpm_entry);

        /* Set second part valid bit. */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 1);

        /* Set mode to ipv6 */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, MODE0f, 1);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, MODE1f, 1);

        /* Set Virtual Router id */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_1f, vrf_id);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, VRF_ID_MASK1f, vrf_mask);
    } else {
        ip4_mask = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
        /* Apply subnet mask. */
        lpm_cfg->defip_ip_addr &= ip4_mask;

        /* Set address to the buffer. */
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, IP_ADDR0f,
                                  lpm_cfg->defip_ip_addr);
        soc_L3_DEFIPm_field32_set(unit, lpm_entry, IP_ADDR_MASK0f, ip4_mask);
    }
    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID0f, 1);

    /* Set Mode Masks */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE_MASK0f, 
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, MODE_MASK0f)) - 1);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, MODE_MASK1f, 
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, MODE_MASK1f)) - 1);

    /* Set Entry Type Masks */
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE0f, 
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, ENTRY_TYPE0f)) - 1);
    soc_mem_field32_set(unit, L3_DEFIPm, lpm_entry, ENTRY_TYPE1f, 
                        (1 << soc_mem_field_length(unit, L3_DEFIPm, ENTRY_TYPE1f)) - 1);
    /* 
     * Note ipv4 entries are expected to reside in part 0 of the entry.
     *      ipv6 entries both parts should be filled.  
     *      Hence if entry is ipv6 copy part 0 to part 1
     */
    return (BCM_E_NONE);
}

STATIC int
_bcm_th_alpm_128_ent_init(int unit, _bcm_defip_cfg_t *lpm_cfg,
             defip_pair_128_entry_t *lpm_entry, int nh_ecmp_idx, uint32 *flags)
{
    int vrf_id;
    int vrf_mask;
    bcm_ip6_t mask;                          /* Subnet mask.              */

    /* Extract entry  vrf id  & vrf mask. */
    BCM_IF_ERROR_RETURN
        (bcm_th_internal_lpm_vrf_calc(unit, lpm_cfg, &vrf_id, &vrf_mask));

    /* Zero buffers. */
    sal_memset(lpm_entry, 0, sizeof(*lpm_entry));

    /* Set hit bit. */
    if (lpm_cfg->defip_flags & BCM_L3_HIT) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, HITf, 1);
    }

    /* Set priority override bit. */
    if (lpm_cfg->defip_flags & BCM_L3_RPE) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, RPEf, 1);
    }

    /* Write priority field. */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, PRIf, lpm_cfg->defip_prio);

    /* Fill next hop information. */
    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, ECMPf, 1);
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, NEXT_HOP_INDEXf, 
                                  nh_ecmp_idx);
    } else {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, NEXT_HOP_INDEXf,
                                  nh_ecmp_idx);
    }

    /* Set destination discard flag. */
    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, DST_DISCARDf, 1);
    }

    /* remember src discard flag */
    if (lpm_cfg->defip_flags & BCM_L3_SRC_DISCARD) {
        *flags |= SOC_ALPM_RPF_SRC_DISCARD;
    }

    /* Set classification group id. */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, CLASS_IDf, 
                              lpm_cfg->defip_lookup_class);

    /* Set Global route flag. */
    if (BCM_L3_VRF_GLOBAL == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_ROUTEf, 0x1);
    }

    /* Indicate this is an override entry */
    if (BCM_L3_VRF_OVERRIDE == lpm_cfg->defip_vrf) {
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_HIGHf, 0x1);
        soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, GLOBAL_ROUTEf, 0x1);
    }

    /* Create mask from prefix length. */
    bcm_ip6_mask_create(mask, lpm_cfg->defip_sub_len);

    /* Apply mask on address. */
    bcm_xgs3_l3_mask6_apply(mask, lpm_cfg->defip_ip6_addr);

    /* Set prefix ip address & mask. */
    _th_defip_pair128_ip6_addr_set(unit, L3_DEFIP_PAIR_128m, 
                                   (uint32 *)lpm_entry, lpm_cfg->defip_ip6_addr);

    _th_defip_pair128_ip6_mask_set(unit, L3_DEFIP_PAIR_128m, 
                                   (uint32 *)lpm_entry, mask);

    /* Set Virtual Router id */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_0_LWRf, vrf_id);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_1_LWRf, vrf_id);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_0_UPRf, vrf_id);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_1_UPRf, vrf_id);

    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK0_LWRf, 
                                       vrf_mask);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK1_LWRf, 
                                       vrf_mask);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK0_UPRf, 
                                       vrf_mask);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VRF_ID_MASK1_UPRf, 
                                       vrf_mask);

    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID0_LWRf, 1);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID1_LWRf, 1);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID0_UPRf, 1);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, VALID1_UPRf, 1);

    /* Set mode to ipv6-128 */
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE0_LWRf, 3);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE1_LWRf, 3);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE0_UPRf, 3);
    soc_L3_DEFIP_PAIR_128m_field32_set(unit, lpm_entry, MODE1_UPRf, 3);

    /* Set Mode Masks */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK0_LWRf, 
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK0_LWRf)) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK1_LWRf, 
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK1_LWRf)) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK0_UPRf, 
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK0_UPRf)) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, MODE_MASK1_UPRf, 
    (1 << soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, MODE_MASK1_UPRf)) - 1);

    /* Set Entry Type Masks */
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE0_LWRf, 
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE0_LWRf)) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE1_LWRf, 
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE1_LWRf)) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE0_UPRf, 
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE0_UPRf)) - 1);
    soc_mem_field32_set(unit, L3_DEFIP_PAIR_128m, lpm_entry, ENTRY_TYPE1_UPRf, 
     (1<<soc_mem_field_length(unit, L3_DEFIP_PAIR_128m, ENTRY_TYPE1_UPRf)) - 1);

    return (BCM_E_NONE);
}


STATIC int
_bcm_th_alpm_128_ent_parse(int unit, soc_mem_t mem, uint32 *hw_entry,
                            _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
    if (soc_mem_field32_get(unit, mem, hw_entry, ECMPf)) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = TRUE;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (nh_ecmp_idx != NULL) {
            *nh_ecmp_idx = soc_mem_field32_get(unit, mem, hw_entry, ECMP_PTRf);
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (nh_ecmp_idx != NULL) {
            *nh_ecmp_idx =
                soc_mem_field32_get(unit, mem, hw_entry, NEXT_HOP_INDEXf);
        }
    }

    /* Mark entry as IPv6 */
    lpm_cfg->defip_flags |= BCM_L3_IP6;

    /* Get entry priority. */
    lpm_cfg->defip_prio = soc_mem_field32_get(unit, mem, hw_entry, PRIf);

    /* Get classification group id. */
    lpm_cfg->defip_lookup_class =
        soc_mem_field32_get(unit, mem, hw_entry, CLASS_IDf);
    /* Get hit bit. */
    if (soc_mem_field32_get(unit, mem, hw_entry, HITf)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }

    /* Get priority override bit. */
    if (soc_mem_field32_get(unit, mem, hw_entry, RPEf)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard field. */
    if(soc_mem_field32_get(unit, mem, hw_entry, DST_DISCARDf)) {
        lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th_alpm_ent_parse
 * Purpose:
 *      Parse an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information.
 *      nh_ecmp_idx - (OUT)Next hop index or ecmp group id.
 *      lpm_entry   - (IN) Buffer read from hw.
 * Returns:
 *      void
 */
STATIC void 
_bcm_th_alpm_ent_parse(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx,
                        defip_entry_t *lpm_entry, soc_mem_t alpm_mem,
                        void *alpm_entry)
{
    int ipv6 = soc_L3_DEFIPm_field32_get(unit, lpm_entry, MODE0f);

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
   /*hurricane does not have ecmp flag*/
    if ((!(SOC_IS_HURRICANEX(unit))) &&
        (soc_mem_field32_get(unit, alpm_mem, alpm_entry, ECMPf))) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = 1;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (nh_ecmp_idx) {
            *nh_ecmp_idx =
                soc_mem_field32_get(unit, alpm_mem, alpm_entry, ECMP_PTRf);
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (nh_ecmp_idx) {
            *nh_ecmp_idx =
               soc_mem_field32_get(unit, alpm_mem, alpm_entry, NEXT_HOP_INDEXf);
        }
    }
    /* Get entry priority. */
    lpm_cfg->defip_prio = soc_mem_field32_get(unit, alpm_mem, alpm_entry,
                                             PRIf);

    /* Get hit bit. */
    if (soc_mem_field32_get(unit, alpm_mem, alpm_entry, HITf)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }
    /* Get priority override bit. */
    if (soc_mem_field32_get(unit, alpm_mem, alpm_entry, RPEf)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (SOC_MEM_FIELD_VALID(unit, alpm_mem, DST_DISCARDf)) {
        if(soc_mem_field32_get(unit, alpm_mem, alpm_entry, DST_DISCARDf)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }
    }

    /* Set classification group id. */
    lpm_cfg->defip_lookup_class = soc_mem_field32_get(unit, alpm_mem,
                                                       alpm_entry, CLASS_IDf);

    if (ipv6) {
        lpm_cfg->defip_flags |= BCM_L3_IP6;
    }
    return;
}

/*
 * Function:
 *      _bcm_th_lpm_ent_parse
 * Purpose:
 *      Parse an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information.
 *      nh_ecmp_idx - (OUT)Next hop index or ecmp group id.
 *      lpm_entry   - (IN) Buffer read from hw.
 * Returns:
 *      void
 */
STATIC INLINE void
_bcm_th_lpm_ent_parse(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx,
                      defip_entry_t *lpm_entry)
{
    int ipv6 = soc_L3_DEFIPm_field32_get(unit, lpm_entry, MODE0f);

    /* Reset entry flags first. */
    lpm_cfg->defip_flags = 0;

    /* Check if entry points to ecmp group. */
   /*hurricane does not have ecmp flag*/
    if ((!(SOC_IS_HURRICANE(unit))) &&
        (soc_L3_DEFIPm_field32_get(unit, lpm_entry, ECMP0f))) {
        /* Mark entry as ecmp */
        lpm_cfg->defip_ecmp = 1;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;

        /* Get ecmp group id. */
        if (nh_ecmp_idx) {
            *nh_ecmp_idx =
                soc_L3_DEFIPm_field32_get(unit, lpm_entry, ECMP_PTR0f);
        }
    } else {
        /* Mark entry as non-ecmp. */
        lpm_cfg->defip_ecmp = 0;

        /* Reset ecmp group next hop count. */
        lpm_cfg->defip_ecmp_count = 0;

        /* Get next hop index. */
        if (nh_ecmp_idx) {
            *nh_ecmp_idx =
                soc_L3_DEFIPm_field32_get(unit, lpm_entry, NEXT_HOP_INDEX0f);
        }
    }
    /* Get entry priority. */
    lpm_cfg->defip_prio = soc_L3_DEFIPm_field32_get(unit, lpm_entry, PRI0f);

    /* Get hit bit. */
    if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, HIT0f)) {
        lpm_cfg->defip_flags |= BCM_L3_HIT;
    }

    /* Get priority override bit. */
    if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, RPE0f)) {
        lpm_cfg->defip_flags |= BCM_L3_RPE;
    }

    /* Get destination discard flag. */
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIPm, DST_DISCARD0f)) {
        if(soc_L3_DEFIPm_field32_get(unit, lpm_entry, DST_DISCARD0f)) {
            lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
        }
    }

#if defined(BCM_TRX_SUPPORT)
    /* Set classification group id. */
    if (SOC_MEM_FIELD_VALID(unit, L3_DEFIPm, CLASS_ID0f)) {
        lpm_cfg->defip_lookup_class =
            soc_L3_DEFIPm_field32_get(unit, lpm_entry, CLASS_ID0f);
    }
#endif /* BCM_TRX_SUPPORT */


    if (ipv6) {
        lpm_cfg->defip_flags |= BCM_L3_IP6;
        /* Get hit bit from the second part of the entry. */
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, HIT1f)) {
            lpm_cfg->defip_flags |= BCM_L3_HIT;
        }

        /* Get priority override bit from the second part of the entry. */
        if (soc_L3_DEFIPm_field32_get(unit, lpm_entry, RPE1f)) {
            lpm_cfg->defip_flags |= BCM_L3_RPE;
        }
    }
    return;
}

/*
 * Function:
 *     _bcm_fb_mem_ip6_defip_get
 * Purpose:
 *    Set an IP6 address field in an ALPM memory
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_th_alpm_mem_ip6_defip_get(int unit, const void *lpm_key, soc_mem_t mem,
                                _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word[2];            /* Temp storage.      */

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));

    soc_mem_field_get(unit, mem, lpm_key, KEYf, &ip6_word[0]);
    ip6[0] = (uint8) ((ip6_word[1] >> 24) & 0xff);
    ip6[1] = (uint8) ((ip6_word[1] >> 16) & 0xff);
    ip6[2] = (uint8) ((ip6_word[1] >> 8) & 0xff);
    ip6[3] = (uint8) (ip6_word[1] & 0xff);

    ip6[4] = (uint8) ((ip6_word[0] >> 24) & 0xff);
    ip6[5] = (uint8) ((ip6_word[0] >> 16) & 0xff);
    ip6[6] = (uint8) ((ip6_word[0] >> 8) & 0xff);
    ip6[7] = (uint8) (ip6_word[0] & 0xff);

    lpm_cfg->defip_sub_len = soc_mem_field32_get(unit, mem, lpm_key, LENGTHf);
}

/*
 * Function:
 *      _bcm_th_alpm_ent_get_key
 * Purpose:
 *      Parse entry key from ALPM table.
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information.
 *      *lpm_entry  - (IN) Pointer to lpm buffer read from hw.
 *      alpm_mem    - (IN) ALPM memory type.
 *      *alpm_entry - (IN) Pointer to alpm entry read from SRAM bank.
 * Returns:
 *      void
 */
STATIC void
_bcm_th_alpm_ent_get_key(int unit, _bcm_defip_cfg_t *lpm_cfg,
                        defip_entry_t *lpm_entry, soc_mem_t alpm_mem,
                        void *alpm_entry)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6;

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_th_alpm_mem_ip6_defip_get(unit, alpm_entry, alpm_mem, lpm_cfg);
    } else {
        /* Get ipv4 address. */
        lpm_cfg->defip_ip_addr = soc_mem_field32_get(unit, alpm_mem,
                                                     alpm_entry, KEYf);

        /* Get subnet mask. */
        v4_mask = soc_mem_field32_get(unit, alpm_mem, alpm_entry, LENGTHf);
        if (v4_mask) {
            v4_mask = ~((1 << (32 - v4_mask)) - 1);
        }

        /* Fill mask length. */
        lpm_cfg->defip_sub_len = bcm_ip_mask_length(v4_mask);
    }

    /* Get Virtual Router id */
    (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, &ipv6);

    return;
}

STATIC int
_bcm_th_alpm_128_get_addr(int unit, soc_mem_t mem, uint32 *alpm_entry,
                           _bcm_defip_cfg_t *lpm_cfg)
{
    uint32 ip6_word[4];

    uint8 *ip6;

    ip6 = lpm_cfg->defip_ip6_addr;

    soc_mem_field_get(unit, mem, alpm_entry, KEYf, ip6_word);
    ip6[0] = (ip6_word[3] >> 24);
    ip6[1] = (ip6_word[3] >> 16) & 0xff;
    ip6[2] = (ip6_word[3] >> 8) & 0xff;
    ip6[3] = ip6_word[3];

    ip6[4] = ip6_word[2] >> 24;
    ip6[5] = (ip6_word[2] >> 16) & 0xff;
    ip6[6] = (ip6_word[2] >> 8) & 0xff;
    ip6[7] = ip6_word[2];

    ip6[8] = ip6_word[1] >> 24;
    ip6[9] = (ip6_word[1] >> 16) & 0xff;
    ip6[10] = (ip6_word[1] >> 8) & 0xff;
    ip6[11] = ip6_word[1];

    ip6[12] = ip6_word[0] >> 24;
    ip6[13] = (ip6_word[0] >> 16) & 0xff;
    ip6[14] = (ip6_word[0] >> 8) & 0xff;
    ip6[15] = ip6_word[0];

    lpm_cfg->defip_sub_len =
                    soc_mem_field32_get(unit, mem, alpm_entry, LENGTHf);

    return SOC_E_NONE;
}

/*
 * Function:
 *     _bcm_th_mem_ip6_defip_get
 * Purpose:
 *    Set an IP6 address field in L3_DEFIPm
 * Note:
 *    See soc_mem_ip6_addr_set()
 */
STATIC void
_bcm_th_mem_ip6_defip_get(int unit, const void *lpm_key,
                          _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;                 /* Ip6 address.       */
    uint8 mask[BCM_IP6_ADDRLEN];        /* Subnet mask.       */
    uint32 ip6_word;            /* Temp storage.      */

    sal_memset(mask, 0, sizeof (bcm_ip6_t));

    /* Just to keep variable name short. */
    ip6 = lpm_cfg->defip_ip6_addr;
    sal_memset(ip6, 0, sizeof (bcm_ip6_t));

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR1f, &ip6_word);
    ip6[0] = (uint8) ((ip6_word >> 24) & 0xff);
    ip6[1] = (uint8) ((ip6_word >> 16) & 0xff);
    ip6[2] = (uint8) ((ip6_word >> 8) & 0xff);
    ip6[3] = (uint8) (ip6_word & 0xff);

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR0f, &ip6_word);
    ip6[4] = (uint8) ((ip6_word >> 24) & 0xff);
    ip6[5] = (uint8) ((ip6_word >> 16) & 0xff);
    ip6[6] = (uint8) ((ip6_word >> 8) & 0xff);
    ip6[7] = (uint8) (ip6_word & 0xff);

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR_MASK1f, &ip6_word);
    mask[0] = (uint8) ((ip6_word >> 24) & 0xff);
    mask[1] = (uint8) ((ip6_word >> 16) & 0xff);
    mask[2] = (uint8) ((ip6_word >> 8) & 0xff);
    mask[3] = (uint8) (ip6_word & 0xff);

    soc_L3_DEFIPm_field_get(unit, lpm_key, IP_ADDR_MASK0f, &ip6_word);
    mask[4] = (uint8) ((ip6_word >> 24) & 0xff);
    mask[5] = (uint8) ((ip6_word >> 16) & 0xff);
    mask[6] = (uint8) ((ip6_word >> 8) & 0xff);
    mask[7] = (uint8) (ip6_word & 0xff);

    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);
}

/*
 * Function:
 *      _bcm_th_lpm_ent_get_key
 * Purpose:
 *      Parse entry key from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (OUT)Buffer to fill defip information.
 *      lpm_entry   - (IN) Buffer read from hw.
 * Returns:
 *      void
 */
STATIC void
_bcm_th_lpm_ent_get_key(int unit, _bcm_defip_cfg_t *lpm_cfg,
                        defip_entry_t *lpm_entry)
{
    bcm_ip_t v4_mask;
    int ipv6 = lpm_cfg->defip_flags & BCM_L3_IP6, tmp;

    /* Set prefix ip address & mask. */
    if (ipv6) {
        _bcm_th_mem_ip6_defip_get(unit, lpm_entry, lpm_cfg);
    } else {
        /* Get ipv4 address. */
        lpm_cfg->defip_ip_addr =
            soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR0f);

        /* Get subnet mask. */
        v4_mask = soc_L3_DEFIPm_field32_get(unit, lpm_entry, IP_ADDR_MASK0f);

        /* Fill mask length. */
        lpm_cfg->defip_sub_len = bcm_ip_mask_length(v4_mask);
    }

    /* Get Virtual Router id */
    (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, &tmp);

    return;
}

/*
 * Function:
 *      _bcm_th_alpm_warmboot_walk
 * Purpose:
 *      Recover LPM and ALPM entries
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_alpm_warmboot_walk(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6;                   /* IPv6 flag.                   */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* IPv4 entries iterator.       */
    int vrf, vrf_id;            /*                              */
    int idx_end;                /*                              */
    int bkt_addr;               /*                              */
    int bkt_count;              /*                              */
    int bkti = 0;               /*                              */
    int bkt_ptr2, bkt_ptr = 0;  /* Bucket pointer               */
    int sub_bkt_idx = 0;        /* Sub bucket pointer           */
    int bank_num = 0;           /* Number of active SRAM banks  */
    int pivot_idx = 0;          /* Pivot Index                  */
    int entry_num = 0;          /* ALPM entry number in bucket  */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int entry_count;            /* ALPM entry count in bucket   */
    int bank_count;             /* SRAM bank count              */
    int step_count;             /*                              */
    int cmp_result;             /* Test routine result.         */
    int rv = BCM_E_FAIL;        /* Operation return status.     */
    int defip_table_size = 0;   /* Defip table size.            */
    char *lpm_tbl_ptr = NULL;   /* DMA table pointer.           */
    void *alpm_entry = NULL;    /*                              */
    soc_mem_t alpm_mem;         /*                              */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    defip_entry_t *lpm_entry;   /* Hw entry buffer.             */
    int bank_bits;              /* 2 bank mode, or 4 bank mode  */
    uint32 db_type;             /* for temp use                 */
    uint32 bank_disable;        /* Bank disable mask            */
    
    defip_alpm_ipv4_entry_t    alpm_entry_v4;
    defip_alpm_ipv6_64_entry_t alpm_entry_v6_64;
        
    /* DMA LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, defip),
                             BCM_XGS3_L3_ENT_SZ(unit, defip), "lpm_tbl",
                             &lpm_tbl_ptr, &defip_table_size));

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end    = defip_table_size;
    bank_count = soc_th_get_alpm_banks(unit);
    bank_bits  = (bank_count + 1) / 2;
    
    /* Walk all lpm entries */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                         defip_entry_t *, lpm_tbl_ptr, idx);
       
        ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0f);

        /* Calculate LPM table traverse step count */
        step_count = ipv6 ? 1 : 2;
      
        /* Insert LPM entry into HASH table and init LPM trackers */ 
        if (soc_th_alpm_warmboot_lpm_reinit(unit, ipv6, idx, lpm_entry) < 0) {
            goto free_lpm_table;
        }
        
        /*
         * This loop is used for two purposes
         *  1. IPv4, DEFIP entry has two IPv4 entries.
         *  2. IPv6 double wide mode, walk next bucket.
         */
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx) {  /* If index == 1 */
                /* Copy upper half of lpm entry to lower half */
                soc_th_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, lpm_entry, TRUE);
               
                /* Invalidate upper half */
                soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 0);
            }
    
            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            /* Extract bucket pointer from LPM entry */
            bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                          ALG_BKT_PTR0f);
            sub_bkt_idx = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                      ALG_SUB_BKT_PTR0f);

            /* Extract VRF from LPM entry*/
            if (soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf) < 0) {
                goto free_lpm_table;
            }
            soc_th_alpm_bank_db_type_get(unit, vrf, &bank_disable, &db_type);

            if (ipv6) {
                /* IPv6 */
                alpm_mem    = L3_DEFIP_ALPM_IPV6_64m;
                alpm_entry  = &alpm_entry_v6_64;
                entry_count = 4;
            } else {
                /* IPv4 */
                alpm_mem    = L3_DEFIP_ALPM_IPV4m;
                alpm_entry  = &alpm_entry_v4;
                entry_count = 6;
            }
            
            /* VRF_OVERRIDE (Global High) entries, prefix resides in TCAM */
            if (ALPM_PREFIX_IN_TCAM(unit, vrf_id)) {
                if (vrf_id == SOC_L3_VRF_OVERRIDE) {
                    VRF_PIVOT_REF_INC(unit, MAX_VRF_ID, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, MAX_VRF_ID, ipv6);
                } else {
                    VRF_PIVOT_REF_INC(unit, vrf, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, vrf, ipv6);
                }
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                _bcm_th_lpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry);
                _bcm_th_lpm_ent_get_key(unit, &lpm_cfg, lpm_entry);
                lpm_cfg.defip_index = idx;

                if (ipv6 == (lpm_cfg.defip_flags & BCM_L3_IP6)) {
                    if (trv_data->op_cb) {
                        (void) (*trv_data->op_cb)(unit, (void *)trv_data,
                                                  (void *)&lpm_cfg,
                                                  (void *)&nh_ecmp_idx,
                                                  &cmp_result);
                    }
                }
                continue;
            }

            pivot_idx = (idx << 1) + tmp_idx;

            /* TCAM pivot recovery */
            if (soc_th_alpm_warmboot_pivot_add(unit, ipv6, lpm_entry, 
                                               pivot_idx, bkt_ptr) < 0) {
                goto free_lpm_table;
            }

            /* Set bucket bitmap */ 
            if (soc_th_alpm_warmboot_bucket_bitmap_set(unit, vrf, ipv6, bkt_ptr) < 0) {
                goto free_lpm_table;
            }

            bkt_count = _soc_th_alpm_bkt_entry_cnt(unit, ipv6);

            entry_num = 0;
            bank_num = 0;
            ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);

            /* Get the bucket pointer from lpm entry */
            bkt_ptr2 = bkt_ptr;
            for (bkti = 0; bkti < bkt_count; bkti++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + SOC_TH_ALPM_BUCKET_BITS)) | 
                           (bkt_ptr2 << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num ++;
                    ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);
                    if (bank_num == bank_count) {
                        bank_num = 0;
                        ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr2 ++;
                    }
                }

                /* Read entry from bucket memory */
                if (soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY, bkt_addr, alpm_entry) < 0) {
                    goto free_lpm_table;
                }

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }
                
                if (sub_bkt_idx != 
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }                

                _soc_tomahawk_alpm_bkt_view_set(unit, bkt_addr, alpm_mem);

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry,
                                        alpm_mem, alpm_entry);
               
                /* Execute operation routine if any. */
                if (trv_data->op_cb) {
                    rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                        continue;
                    }
#endif
                }

                if (soc_th_alpm_warmboot_prefix_insert(unit, ipv6, lpm_entry,
                                                       alpm_entry, pivot_idx, 
                                                       ALPM_BKTID(bkt_ptr, sub_bkt_idx), 
                                                       bkt_addr) < 0) {
                    continue;
                }

            } /* End of bucket walk loop*/
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    if (soc_th_alpm_warmboot_lpm_reinit_done(unit) < 0) {
        goto free_lpm_table;
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;

free_lpm_table:
    soc_cm_sfree(unit, lpm_tbl_ptr);

    return (rv);
}

/*
 * Function:
 *      _th_defip_pair128_get_key
 * Purpose:
 *      Parse route entry key from L3_DEFIP_PAIR_128 table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      entry       - (IN)Hw entry buffer.
 *      lpm_cfg     - (IN/OUT)Buffer to fill defip information.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_th_defip_pair128_get_key(int unit, uint32 *hw_entry,
                           _bcm_defip_cfg_t *lpm_cfg)
{
    soc_mem_t mem = L3_DEFIP_PAIR_128m;  /* Route table memory. */
    bcm_ip6_t mask;                      /* Subnet mask.        */

    /* Input parameters check. */
    if ((lpm_cfg == NULL) || (hw_entry == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Extract ip6 address. */
    _th_defip_pair128_ip6_addr_get(unit,mem, hw_entry,
                                    lpm_cfg->defip_ip6_addr);

    /* Extract subnet mask. */
    _th_defip_pair128_ip6_mask_get(unit, mem, hw_entry, mask);
    lpm_cfg->defip_sub_len = bcm_ip6_mask_length(mask);

    /* Extract vrf id & vrf mask. */
    if(!soc_mem_field32_get(unit, mem, hw_entry, VRF_ID_MASK0_LWRf)) {
        lpm_cfg->defip_vrf = BCM_L3_VRF_OVERRIDE;
    } else {
        lpm_cfg->defip_vrf = soc_mem_field32_get(unit, mem, hw_entry,
                                                 VRF_ID_0_LWRf);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th_alpm_128_warmboot_walk
 * Purpose:
 *      Recover IPv6-128Bit LPM and ALPM entries
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_alpm_128_warmboot_walk(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    int ipv6 = 2;               /* IPv6 flag.                   */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* IPv4 entries iterator.       */
    int vrf, vrf_id;            /*                              */
    int idx_end;                /*                              */
    int bkt_addr;               /*                              */
    int bkt_count;              /*                              */
    int bkti = 0;               /*                              */
    int bkt_ptr2, bkt_ptr = 0;  /* Bucket pointer               */
    int sub_bkt_idx = 0;        /* Sub bucket pointer           */
    int bank_num = 0;           /* Number of active SRAM banks  */
    int entry_num = 0;          /* ALPM entry number in bucket  */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int entry_count;            /* ALPM entry count in bucket   */
    int bank_count;             /* SRAM bank count              */
    int step_count = 1;         /*                              */
    int cmp_result;             /* Test routine result.         */
    int rv = BCM_E_FAIL;        /* Operation return status.     */
    int defip_table_size = 0;   /* Defip table size.            */
    char *lpm_tbl_ptr = NULL;   /* DMA table pointer.           */
    void *alpm_entry = NULL;    /*                              */
    uint32 rval;                /*                              */
    soc_mem_t alpm_mem;         /*                              */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int bank_bits;              /* 2 bank mode, or 4 bank mode  */
    uint32 db_type;             /* for temp use                 */
    uint32 bank_disable;        /* Bank disable mask            */
    
    defip_pair_128_entry_t *lpm_entry; /* Hw entry buffer.      */
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128; /* ALPM 128  */
        
    /* DMA LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, L3_DEFIP_PAIR_128m,
                    WORDS2BYTES(soc_mem_entry_words(unit, L3_DEFIP_PAIR_128m)), 
                    "lpm_128_warmboot_tbl",
                    &lpm_tbl_ptr, &defip_table_size));

    if(BCM_FAILURE(READ_L3_DEFIP_RPF_CONTROLr(unit, &rval))) {
        goto free_lpm_table;
    }

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    alpm_mem    = L3_DEFIP_ALPM_IPV6_128m;
    alpm_entry  = &alpm_entry_v6_128;
    entry_count = 2;
    bank_count  = soc_th_get_alpm_banks(unit);
    bank_bits   = (bank_count + 1) / 2;
    bkt_count   = _soc_th_alpm_bkt_entry_cnt(unit, L3_DEFIP_MODE_128);
    
    /* Walk all lpm entries */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                         defip_pair_128_entry_t *, lpm_tbl_ptr, idx);
    
        /* Verify if read LPM entry is IPv6-128 entry */   
        if (0x03 != soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                        MODE1_UPRf)) {
            continue;    
        };
     
        /* Verify if LPM entry is valid */
        if (!soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    VALID1_LWRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    VALID0_LWRf) ||
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                    VALID1_UPRf) || 
            !soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry,
                                    VALID0_UPRf)) {
            continue;
        };

        /* Insert LPM entry into HASH table and init LPM trackers */ 
        if (soc_th_alpm_128_warmboot_lpm_reinit(unit, ipv6, idx, lpm_entry) < 0) {
            goto free_lpm_table;
        }
        
        /*  If IPv6 double wide mode, walk next bucket.*/
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            /* Extract bucket pointer from LPM entry */
            bkt_ptr = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                          ALG_BKT_PTRf);
            sub_bkt_idx = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, 
                                              lpm_entry, ALG_SUB_BKT_PTRf);

            /* Extract VRF from LPM entry*/
            if (soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &vrf_id, &vrf) < 0) {
                goto free_lpm_table;
            }

            /* VRF_OVERRIDE (Global High) entries, prefix resides in TCAM */
            if (ALPM_PREFIX_IN_TCAM(unit, vrf_id)) {
                if (vrf_id == SOC_L3_VRF_OVERRIDE) {
                    VRF_PIVOT_REF_INC(unit, MAX_VRF_ID, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, MAX_VRF_ID, ipv6);
                } else {
                    VRF_PIVOT_REF_INC(unit, vrf, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, vrf, ipv6);
                }
                /* TCAM Routes Processing */ 
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                (void) _th_defip_pair128_get_key(unit, (uint32 *) lpm_entry, 
                                            &lpm_cfg);
                (void) soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, 
                                            &lpm_cfg.defip_vrf, &nh_ecmp_idx);
                _bcm_th_alpm_128_ent_parse(unit, L3_DEFIP_PAIR_128m, 
                                           (uint32 *) lpm_entry, 
                                           &lpm_cfg, &nh_ecmp_idx);
                lpm_cfg.defip_index = idx;
                if (trv_data->op_cb) {
                    (void) (*trv_data->op_cb)(unit, (void *)trv_data,
                                              (void *)&lpm_cfg,
                                              (void *)&nh_ecmp_idx, &cmp_result);
                }
                continue;
            }
            soc_th_alpm_bank_db_type_get(unit, vrf, &bank_disable, &db_type);
                
            /* TCAM pivot recovery */
            if (soc_th_alpm_128_warmboot_pivot_add(unit, ipv6, lpm_entry, 
                                                   idx, bkt_ptr) < 0) {
                goto free_lpm_table;
            }

            /* Set bucket bitmap */ 
            if (soc_th_alpm_128_warmboot_bucket_bitmap_set(unit, vrf, ipv6, 
                                                           bkt_ptr) < 0) {
                goto free_lpm_table;
            }

            bank_num = 0;
            ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);                
            entry_num = 0;
            bkt_ptr2 = bkt_ptr;
            /* Get the bucket pointer from lpm entry */
            for (bkti = 0; bkti < bkt_count; bkti++) {
                /* calculate bucket memory address */
                /* also increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + SOC_TH_ALPM_BUCKET_BITS)) | 
                           (bkt_ptr2 << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);
                    if (bank_num == bank_count) {
                        bank_num = 0;
                        ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr2 ++;
                    }                    
                }

                /* Read entry from bucket memory */
                if (soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY, bkt_addr, 
                                 alpm_entry) < 0) {
                    goto free_lpm_table;
                }

                /*
                 * In Tomahawk URPF mode, valid bits will make sure
                 * SIP entries in buckets are included in this walk
                 */

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }
                if (sub_bkt_idx != 
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }                

                _soc_tomahawk_alpm_bkt_view_set(unit, bkt_addr, alpm_mem);

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_128_ent_parse(unit, L3_DEFIP_PAIR_128m, 
                                            (uint32 *) lpm_entry, 
                                            &lpm_cfg, &nh_ecmp_idx);
               
                /* Execute operation routine if any. */
                if (trv_data->op_cb) {
                    rv = (*trv_data->op_cb)(unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                    if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                        continue;
                    }
#endif
                }

                if (soc_th_alpm_128_warmboot_prefix_insert(unit, ipv6, lpm_entry,
                       alpm_entry, idx, ALPM_BKTID(bkt_ptr, sub_bkt_idx), 
                       bkt_addr) < 0) {
                    continue;
                }

            } /* End of bucket walk loop*/
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    if (soc_th_alpm_128_warmboot_lpm_reinit_done(unit) < 0) {
        goto free_lpm_table;
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */

    rv = BCM_E_NONE;
free_lpm_table:
    soc_cm_sfree(unit, lpm_tbl_ptr);

    return (rv);
}

STATIC int
_bcm_th_alpm_128_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 ipv6;                /* Iterate over ipv6 only flag. */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* ipv4 entries iterator.       */
    char *lpm_tbl_ptr;          /* Dma table pointer.           */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int cmp_result;             /* Test routine result.         */
    defip_pair_128_entry_t *lpm_entry = NULL;   /* Hw entry buffer.    */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int defip_table_size;       /* Defip table size.            */
    int rv = BCM_E_NONE;        /* Operation return status.     */
    int idx_start = 0;
    int idx_end = 0, bkt_idx, bkt_count, bkt_ptr = 0, bkt_addr;
    int entry_num, bank_num, entry_count, bank_count, bank_bits;
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128;
    void *alpm_entry;
    soc_mem_t alpm_mem, pivot_mem = L3_DEFIP_PAIR_128m;
    int step_count = 1;
    int def_arr_idx = 0;
    int def_rte_arr_sz;
    typedef struct _alpm_def_route_info_s {
        int idx;
        int bkt_addr;
    } _alpm_def_route_info_t;
    _alpm_def_route_info_t *def_rte_arr = NULL;
    int sub_bkt_idx;
    uint32 db_type, bank_disable;

    ipv6 = (trv_data->flags & BCM_L3_IP6);
    if (!ipv6) {
        return SOC_E_NONE;
    }

    /* LPM table DMA to software copy if callback is not for 'delete all' */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, L3_DEFIP_PAIR_128m, 
                    WORDS2BYTES(soc_mem_entry_words(unit, L3_DEFIP_PAIR_128m)), 
                             "lpm_128_tbl", &lpm_tbl_ptr, &defip_table_size));

    /* Allocate memory to store default route meta data*/
    def_rte_arr_sz = SOC_VRF_MAX(unit) * sizeof(_alpm_def_route_info_t);
    def_rte_arr = sal_alloc(def_rte_arr_sz, "alpm_def_rte_arry"); 
    if (NULL == def_rte_arr) {
        return BCM_E_MEMORY;
    }
    sal_memset(def_rte_arr, 0, def_rte_arr_sz);

    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    alpm_mem    = L3_DEFIP_ALPM_IPV6_128m;
    idx_start   = 0;
    bank_num    = 0;
    entry_num   = 0;
    bank_count  = soc_th_get_alpm_banks(unit);
    entry_count = 2;
    alpm_entry  = &alpm_entry_v6_128;
    bank_bits   = (bank_count + 1) / 2;
    bkt_count   = _soc_th_alpm_bkt_entry_cnt(unit, L3_DEFIP_MODE_128);

    /* if v6 loop twice once with with defip and once with defip_pair */
    for (idx = (idx_end - 1); idx >= idx_start; idx--) {    

        /* Calculate entry ofset. */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                    defip_pair_128_entry_t *, lpm_tbl_ptr, idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get(unit, pivot_mem, lpm_entry, VALID0_LWRf)) {
            continue;
        }
        
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            bkt_ptr = soc_mem_field32_get(unit, pivot_mem, lpm_entry, ALG_BKT_PTRf);
            sub_bkt_idx = soc_mem_field32_get(unit, pivot_mem, lpm_entry, ALG_SUB_BKT_PTRf);
            (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                              &cmp_result);
            soc_th_alpm_bank_db_type_get(unit, cmp_result, &bank_disable, &db_type);
            if (ALPM_PREFIX_IN_TCAM(unit, lpm_cfg.defip_vrf)) {
                if (lpm_cfg.defip_vrf == SOC_L3_VRF_OVERRIDE) {
                    VRF_PIVOT_REF_INC(unit, MAX_VRF_ID, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, MAX_VRF_ID, ipv6);
                } else {
                    VRF_PIVOT_REF_INC(unit, cmp_result, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, cmp_result, ipv6);
                }

                /* OVERRIDE VRF Processing */ 
                (void) _th_defip_pair128_get_key(unit, (uint32 *) lpm_entry, 
                                                 &lpm_cfg);
                /* defip_vrf adjust */
                (void) soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, 
                                          &lpm_cfg.defip_vrf, &nh_ecmp_idx);
                _bcm_th_alpm_128_ent_parse(unit, pivot_mem, 
                                            (uint32 *) lpm_entry, 
                                            &lpm_cfg, &nh_ecmp_idx);
                lpm_cfg.defip_index = idx;
                
                if (trv_data->op_cb) {
                    (void) (*trv_data->op_cb) (unit, (void *)trv_data,
                                               (void *)&lpm_cfg,
                                               (void *)&nh_ecmp_idx, &cmp_result);
                }

                continue;
            }

            entry_num = 0;
            bank_num = 0;
            ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);

            /* get the bucket pointer from lpm mem  entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* calculate bucket memory address */
                /* also increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + SOC_TH_ALPM_BUCKET_BITS)) | 
                           (bkt_ptr << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);                   
                    if (bank_num == bank_count) {
                        bank_num = 0;
                        ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr ++;
                    }                    
                }

                /* read entry from bucket memory */
                rv = soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                                    bkt_addr, alpm_entry);
                if (SOC_FAILURE(rv)) {
                    break;
                }

                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                if (sub_bkt_idx != 
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse  the entry. */
                /* Fill entry ip address &  subnet mask. */
                _bcm_th_alpm_128_get_addr(unit, alpm_mem, 
                                           (uint32 *) alpm_entry, &lpm_cfg);
                
                (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                                  &cmp_result);
                
                /* get associated data */
                _bcm_th_alpm_128_ent_parse(unit, alpm_mem, alpm_entry, 
                                            &lpm_cfg, &nh_ecmp_idx);
                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
                    continue;
                }

                lpm_cfg.defip_index = bkt_addr;

                /* Check if this is default route */
                /* Subnet length zero will indicate default route */
                if (0 == lpm_cfg.defip_sub_len) {
                    if (def_arr_idx < SOC_VRF_MAX(unit)) {
                        def_rte_arr[def_arr_idx].bkt_addr = bkt_addr;
                        def_rte_arr[def_arr_idx].idx = idx;
                        def_arr_idx ++;
                    }
                } else {
                    /* Execute operation routine if any. */
                    if (trv_data->op_cb) {
                        rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                                (void *)&lpm_cfg,
                                                (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            break;
                        }
#endif
                    }
                }
            }
        }
    }
   
    /* Process Default routes */
    for (idx = 0; idx < def_arr_idx; idx++) {

        /* Calculate entry offset. */
        lpm_entry = soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                    defip_pair_128_entry_t *, lpm_tbl_ptr,
                                    def_rte_arr[idx].idx);

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get(unit, pivot_mem, lpm_entry, VALID0_LWRf)) {
            continue;
        }

        bkt_addr = def_rte_arr[idx].bkt_addr;
        
        /* read entry from bucket memory */
        rv = soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                            bkt_addr, alpm_entry);
        if (SOC_FAILURE(rv)) {
            break;
        }

        if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        /* Parse  the entry. */
        (void)soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                          &cmp_result);
        /* Fill entry ip address & subnet mask. */
        _bcm_th_alpm_128_get_addr(unit, alpm_mem, alpm_entry, &lpm_cfg);

        _bcm_th_alpm_128_ent_parse(unit, alpm_mem, alpm_entry, &lpm_cfg, 
                                    &nh_ecmp_idx);

        /* If protocol doesn't match skip the entry. */
        if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
            continue;
        }

        /* Execute operation routine if any. */
        if (trv_data->op_cb) {
            rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                    (void *)&lpm_cfg,
                                    (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                break;
            }
#endif
        }
    }

    sal_free(def_rte_arr);
    soc_cm_sfree(unit, lpm_tbl_ptr);
    
    return (rv);
}

/*
 * Function:
 *      _bcm_th_alpm_update_match
 * Purpose:
 *      Update/Delete all entries in defip table matching a certain rule.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      trv_data - (IN)Delete pattern + compare,act,notify routines.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_alpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data)
{
    uint32 ipv6 = 0;            /* Iterate over ipv6 only flag. */
    int idx;                    /* Iteration index.             */
    int tmp_idx;                /* ipv4 entries iterator.       */
    int tmp_start;              /* Iteration start.             */
    int tmp_end;                /* Iteration end.               */
    int tmp_delta;              /* Iteration delta.             */
    char *lpm_tbl_ptr;          /* Dma table pointer.           */
    int nh_ecmp_idx;            /* Next hop/Ecmp group index.   */
    int cmp_result;             /* Test routine result.         */
    defip_entry_t *lpm_entry;   /* Hw entry buffer.             */
    defip_entry_t lpm_entry_data; /* Hw entry buffer.           */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
    int defip_table_size;       /* Defip table size.            */
    int rv = BCM_E_NONE;        /* Operation return status.     */
    int idx_start = 0;
    int idx_end = 0, bkt_idx, bkt_count, bkt_ptr = 0, bkt_addr;
    int entry_num, bank_num, entry_count, bank_count;
    defip_alpm_ipv4_entry_t alpm_entry_v4;
    defip_alpm_ipv4_1_entry_t alpm_entry_v4_1;
    defip_alpm_ipv6_64_entry_t alpm_entry_v6_64;
    defip_alpm_ipv6_64_1_entry_t alpm_entry_v6_64_1;
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128;
    void *alpm_entry;
    soc_mem_t alpm_mem;
    int step_count;
    int def_arr_idx = 0;
    int def_rte_arr_sz;
    int bank_bits;
    int sub_bkt_idx;
    uint32 db_type, bank_disable;
    
    typedef struct _alpm_def_route_info_s {
        int idx;
        int bkt_addr;
        defip_entry_t lpm_entry;
    } _alpm_def_route_info_t;
    
    _alpm_def_route_info_t *def_rte_arr = NULL;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv =_bcm_th_alpm_warmboot_walk(unit, trv_data);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_ALPM,
                      (BSL_META_U(unit,
                      "ERROR!  ALPM Warmboot recovery failed")));
            return (rv);
        }

        if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) != 0) {
            /* v6-128 entries */
            rv = _bcm_th_alpm_128_warmboot_walk(unit, trv_data);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_ALPM,
                         (BSL_META_U(unit,
                         "ERROR!  ALPM Warmboot V6-128 recovery failed")));
            }
        }
        return (rv);
    }
#endif

    ipv6 = trv_data->flags & BCM_L3_IP6;

    if (ipv6 && soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) != 0) {
        /* first get v6-128 entries */
        rv = _bcm_th_alpm_128_update_match(unit, trv_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#endif
    }

    if (soc_mem_index_count(unit, BCM_XGS3_L3_MEM(unit, defip)) == 0) {
        return BCM_E_NONE;
    }

    /* Table DMA the LPM table to software copy */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_tbl_dma(unit, BCM_XGS3_L3_MEM(unit, defip),
                             BCM_XGS3_L3_ENT_SZ(unit, defip), "lpm_tbl",
                             &lpm_tbl_ptr, &defip_table_size));

    /* Allocate memory to store default route meta data*/
    def_rte_arr_sz = SOC_VRF_MAX(unit) * sizeof(_alpm_def_route_info_t);
    def_rte_arr = sal_alloc(def_rte_arr_sz, "alpm_def_rte_arry"); 
    if (NULL == def_rte_arr) {
        return BCM_E_MEMORY;
    }
    sal_memset(def_rte_arr, 0, def_rte_arr_sz);

    alpm_mem = ipv6 ? L3_DEFIP_ALPM_IPV6_64m : L3_DEFIP_ALPM_IPV4m;
    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end     = defip_table_size;
    bank_num    = 0;
    entry_num   = 0;
    bank_count  = soc_th_get_alpm_banks(unit);
    bank_bits   = (bank_count + 1) / 2;
    bkt_count   = _soc_th_alpm_bkt_entry_cnt(unit, ipv6 ? L3_DEFIP_MODE_64 : 
                                                   L3_DEFIP_MODE_V4);
    
    switch (alpm_mem) {
    case L3_DEFIP_ALPM_IPV4m:
        entry_count = 6;
        alpm_entry = &alpm_entry_v4;
        break;
    case L3_DEFIP_ALPM_IPV4_1m:
        entry_count = 4;
        alpm_entry = &alpm_entry_v4_1;
        break;
    case L3_DEFIP_ALPM_IPV6_64m:
        entry_count = 4;
        alpm_entry = &alpm_entry_v6_64;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m:
        entry_count = 3;
        alpm_entry = &alpm_entry_v6_64_1;
        break;
    case L3_DEFIP_ALPM_IPV6_128m:
        entry_count = 2;
        alpm_entry = &alpm_entry_v6_128;
        break;
    default:
        return SOC_E_PARAM;
    }

    step_count = ipv6 ? 1 : 2;

    /* Walk all lpm entries */
    for (idx = (idx_end - 1); idx >= idx_start; idx--) {
            /* Calculate entry ofset. */
        lpm_entry =
                soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                             defip_entry_t *, lpm_tbl_ptr, idx);
        sal_memcpy (&lpm_entry_data, lpm_entry, sizeof (lpm_entry_data));

        if (ipv6 && (soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0f) == 0)) {
           /*
            * Function called for IPv6 LPM/ALPM walk and LPM entry is IPv4; Continue;
            */
           continue;
        }
        
        /* Each LPM index has two IPv4 entries*/
        if (ipv6) {
           tmp_start = 0;
           tmp_end   = step_count;
           tmp_delta = 1;
        } else {
           tmp_start = step_count - 1;
           tmp_end   = -1;
           tmp_delta = -1;
        }

        for (tmp_idx = tmp_start; tmp_idx != tmp_end; tmp_idx += tmp_delta) {
            if (!ipv6) {
                if (tmp_idx) {
                    /* Check second part of the entry. */
                    soc_th_alpm_lpm_ip4entry1_to_0(unit, &lpm_entry_data, lpm_entry, TRUE);
                } else {
                    soc_th_alpm_lpm_ip4entry0_to_0(unit, &lpm_entry_data, lpm_entry, TRUE);            
                }
            }

            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            cmp_result = ipv6 ? 1 : 0;
            if (cmp_result != (soc_mem_field32_get(unit, L3_DEFIPm, 
                                                lpm_entry, MODE0f))) {
                continue;
            }

            (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                          &cmp_result);
            soc_th_alpm_bank_db_type_get(unit, cmp_result, &bank_disable, &db_type);
       
            if (ALPM_PREFIX_IN_TCAM(unit, lpm_cfg.defip_vrf)) {
                if (lpm_cfg.defip_vrf == SOC_L3_VRF_OVERRIDE) {
                    VRF_PIVOT_REF_INC(unit, MAX_VRF_ID, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, MAX_VRF_ID, ipv6);
                } else {
                    VRF_PIVOT_REF_INC(unit, cmp_result, ipv6);
                    VRF_TRIE_ROUTES_INC(unit, cmp_result, ipv6);
                }
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                _bcm_th_lpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry);
                _bcm_th_lpm_ent_get_key(unit, &lpm_cfg, lpm_entry);

                lpm_cfg.defip_index = idx;

                if (ipv6 == (lpm_cfg.defip_flags & BCM_L3_IP6)) {
                    if (trv_data->op_cb) {
                        rv = (*trv_data->op_cb)(unit, (void *)trv_data,
                                            (void *)&lpm_cfg,
                                            (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            break;
                        }
#endif
                    }
                }
                continue;
            }

            bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                      ALG_BKT_PTR0f);
            sub_bkt_idx = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                      ALG_SUB_BKT_PTR0f);
            entry_num = 0;
            bank_num = 0;
            ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);

            /* Get the bucket pointer from lpm mem entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + SOC_TH_ALPM_BUCKET_BITS)) | 
                           (bkt_ptr << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num ++;
                    ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);                  
                    if (bank_num == bank_count) {
                        bank_num = 0;
                        ALPM_GET_AVAIL_BANK(bank_num, bank_count, bank_disable);
                        bkt_ptr ++;
                    }
                }

                /* Read entry from bucket memory */
                rv = soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                                    bkt_addr, alpm_entry);
                if (SOC_FAILURE(rv)) {
                    break;
                }

                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
                    continue;
                }

                if (sub_bkt_idx != 
                    soc_mem_field32_get(unit, alpm_mem, alpm_entry, SUB_BKT_PTRf)) {
                    continue;
                }

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry,
                                        alpm_mem, alpm_entry);
        
                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
                    continue;
                }
                 
                /* Fill entry IP address and subnet mask. */
                _bcm_th_alpm_ent_get_key(unit, &lpm_cfg, lpm_entry, 
                                          alpm_mem, alpm_entry);

                lpm_cfg.defip_index = bkt_addr;

                /* Check if this is default route */
                /* Subnet length zero will indicate default route */
                if (0 == lpm_cfg.defip_sub_len) {
                    if (def_arr_idx < SOC_VRF_MAX(unit)) {
                        def_rte_arr[def_arr_idx].bkt_addr = bkt_addr;
                        def_rte_arr[def_arr_idx].idx = idx;
                        sal_memcpy(&def_rte_arr[def_arr_idx].lpm_entry,
                                   lpm_entry, sizeof(*lpm_entry));
                        def_arr_idx ++;
                    }
                } else {
                    /* Execute operation routine if any. */
                    if (trv_data->op_cb) {
                        rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                                (void *)&lpm_cfg,
                                                (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            break;
                        }
#endif
                    }
                }
            }
        }
    }

    /* Process Default routes */
    for (idx = 0; idx < def_arr_idx; idx++) {

        /* Calculate entry ofset. */
        lpm_entry = &def_rte_arr[idx].lpm_entry;

        cmp_result = ipv6 ? 1 : 0;
        if (cmp_result != (soc_mem_field32_get(unit, L3_DEFIPm, 
                                            lpm_entry, MODE0f))) {
            /*
             * Function called for IPv6 LPM/ALPM walk and LPM entry is IPv4; 
             * Continue;
             */
            continue;
        }
        bkt_addr = def_rte_arr[idx].bkt_addr;
        
        /* read entry from bucket memory */
        rv = soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                            bkt_addr, alpm_entry);
        if (SOC_FAILURE(rv)) {
            break;
        }

        if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
            continue;
        }

        /* Zero destination buffer first. */
        sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

        (void)soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                      &cmp_result);
        /* Parse  the entry. */
        _bcm_th_alpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry,
                                alpm_mem, alpm_entry);

        /* If protocol doesn't match skip the entry. */
        if ((lpm_cfg.defip_flags & BCM_L3_IP6) != ipv6) {
            continue;
        }

        /* Fill entry ip address & subnet mask. */
        _bcm_th_alpm_ent_get_key(unit, &lpm_cfg, lpm_entry, 
                                  alpm_mem, alpm_entry);

        /* Execute operation routine if any. */
        if (trv_data->op_cb) {
            rv = (*trv_data->op_cb) (unit, (void *)trv_data,
                                    (void *)&lpm_cfg,
                                    (void *)&nh_ecmp_idx, &cmp_result);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                break;
            }
#endif
        }
    }

#ifdef ALPM_WARM_BOOT_DEBUG
    soc_alpm_lpm_sw_dump(unit);
#endif /* ALPM_WARM_BOOT_DEBUG  */
    sal_free(def_rte_arr);
    soc_cm_sfree(unit, lpm_tbl_ptr);

    return (rv);
}

/*
 * Function:
 *      _bcm_th_alpm_find
 * Purpose:
 *      Get an entry from DEFIP table.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      lpm_cfg     - (IN)Buffer to fill defip information. 
 *      nh_ecmp_idx - (IN)Next hop or ecmp group index
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_alpm_find(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx)
{
    defip_entry_t lpm_key;      /* Route lookup key.        */
    defip_entry_t lpm_entry;    /* Search result buffer.    */
    defip_pair_128_entry_t lpm_128_key;
    defip_pair_128_entry_t lpm_128_entry;
    int do_urpf = 0;            /* Find uRPF region */
    int rv;                     /* Operation return status. */
    uint32 flags = 0;
    soc_mem_t mem;
    int lpm_mode, tmp;
    uint32 rval;

    /* Input parameters check */
    if (NULL == lpm_cfg) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(unit, &rval));
    lpm_mode  = soc_reg_field_get(unit, L3_DEFIP_RPF_CONTROLr, rval, LPM_MODEf);
    /* Not supported in Legacy LPM mode */
    if (lpm_mode == 0) {
        return (BCM_E_UNAVAIL);
    }
    
    /* Zero buffers. */
    sal_memset(&lpm_entry, 0, sizeof(defip_entry_t));
    sal_memset(&lpm_key, 0, sizeof(defip_entry_t));
    sal_memset(&lpm_128_entry, 0, sizeof(defip_pair_128_entry_t));
    sal_memset(&lpm_128_key, 0, sizeof(defip_pair_128_entry_t));

    /* Default value */
    mem = L3_DEFIPm;
    if ((lpm_cfg->defip_flags & BCM_L3_IP6)) {
        if (soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) > 0) {
            mem = L3_DEFIP_PAIR_128m;
        }
    }
    
    switch (mem) {
    case L3_DEFIP_PAIR_128m:
        /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_th_alpm_128_ent_init(unit, lpm_cfg, 
                                            &lpm_128_key, 0, &flags));
        /* Perform hw lookup. */
        rv = soc_th_alpm_128_find_best_match(unit, &lpm_128_key, &lpm_128_entry, 
                                             &lpm_cfg->defip_index, do_urpf);
        BCM_IF_ERROR_RETURN(rv);

        /* Parse hw buffer to defip entry. */
        _bcm_th_alpm_128_ent_parse(unit, L3_DEFIP_PAIR_128m, 
                                   (uint32 *)&lpm_128_entry, lpm_cfg, 
                                   nh_ecmp_idx);
        /* OVERRIDE VRF Processing */ 
        _th_defip_pair128_get_key(unit, (uint32 *)&lpm_128_entry, lpm_cfg);
        soc_th_alpm_128_lpm_vrf_get(unit, &lpm_128_entry, &lpm_cfg->defip_vrf, &tmp);

        break;
    default:
        /* Initialize lkup key. */
        BCM_IF_ERROR_RETURN(_bcm_th_alpm_ent_init(unit, lpm_cfg, &lpm_key, 0, 
                                                  &flags));
        /* Perform hw lookup. */
        rv = soc_th_alpm_find_best_match(unit, &lpm_key, &lpm_entry, 
                                      &lpm_cfg->defip_index, do_urpf);
        BCM_IF_ERROR_RETURN(rv);
    
        /* Parse hw buffer to defip entry. */
        _bcm_th_lpm_ent_parse(unit, lpm_cfg, nh_ecmp_idx, &lpm_entry);
        _bcm_th_lpm_ent_get_key(unit, lpm_cfg, &lpm_entry);
        
        break;
    }
    
    return (BCM_E_NONE);
}

#endif /* ALPM_ENABLE */

/*
 * Function:
 *      _bcm_th_macda_oui_profile_init
 * Purpose:
 *      Allocate and initialize _bcm_th_macda_oui_profile_init
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_macda_oui_profile_init(int unit)
{
    soc_mem_t mem_profile;
    int entry_words[1];
    
    /* Create profile for MACDA_OUI_PROFILE table*/
    if (_bcm_th_macda_oui_profile[unit] == NULL) {
        _bcm_th_macda_oui_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "MACDA OUI Profile Mem");
        if (_bcm_th_macda_oui_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_destroy(unit, _bcm_th_macda_oui_profile[unit]));
    }
    soc_profile_mem_t_init(_bcm_th_macda_oui_profile[unit]);
    mem_profile = EGR_MACDA_OUI_PROFILEm;
    entry_words[0] = BYTES2WORDS(sizeof(egr_macda_oui_profile_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem_profile, entry_words, 1,
                                _bcm_th_macda_oui_profile[unit]));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_macda_oui_profile_deinit
 * Purpose:
 *      Deallocate _bcm_th_macda_oui_profile
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_th_macda_oui_profile_deinit(int unit)
{
    if (_bcm_th_macda_oui_profile[unit]) {
        (void)soc_profile_mem_destroy(unit, _bcm_th_macda_oui_profile[unit]);
        sal_free(_bcm_th_macda_oui_profile[unit]);
        _bcm_th_macda_oui_profile[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th_vntag_etag_profile_init
 * Purpose:
 *      Allocate and initialize _bcm_th_vntag_etag_profile
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_vntag_etag_profile_init(int unit)
{
    soc_mem_t mem_profile;
    int entry_words[1];
    void *entries[1];
    uint32 profile_idx;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;

    /* Create profile for EGR_VNTAG_ETAG_PROFILE table */
    if (_bcm_th_vntag_etag_profile[unit] == NULL) {
        _bcm_th_vntag_etag_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "EGR VNTAG ETAG Profile Mem");
        if (_bcm_th_vntag_etag_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_destroy(unit, _bcm_th_vntag_etag_profile[unit]));
    }
    soc_profile_mem_t_init(_bcm_th_vntag_etag_profile[unit]);
    mem_profile = EGR_VNTAG_ETAG_PROFILEm;
    entry_words[0] = BYTES2WORDS(sizeof(egr_vntag_etag_profile_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem_profile, entry_words, 1,
                                _bcm_th_vntag_etag_profile[unit]));

    /* Index 0 in EGR_VNTAG_ETAG_PROFILE table is reserved for non NIV/PE flow
     * hence adding this dummy entry at index 0 during init time
     */
    sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
    entries[0] = &vntag_etag_profile_entry;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_th_vntag_etag_profile[unit],
                             entries, 1, &profile_idx));
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th_vntag_etag_profile_deinit
 * Purpose:
 *      Deallocate _bcm_th_vntag_etag_profile
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_th_vntag_etag_profile_deinit(int unit)
{
    if (_bcm_th_vntag_etag_profile[unit]) {
        (void)soc_profile_mem_destroy(unit, _bcm_th_vntag_etag_profile[unit]);
        sal_free(_bcm_th_vntag_etag_profile[unit]);
        _bcm_th_vntag_etag_profile[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th_l3_vp_entry_del
 * Purpose:
 *      Delete entry from profile table
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      ipv6     - (IN) IPV4 or IPv6 flag
 *      l3cfg    - (IN) L3 entry info.
 *      bufentry - (IN) hw buffer
 * Returns:
 */
bcm_error_t
_bcm_th_l3_vp_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                        int macda_oui_profile_index ,int vntag_etag_profile_index)
{

    int rv = BCM_E_NONE;

    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        
        /* delete an entry or decrement ref count from EGR_MACDA_OUI_PROFILE table */
        soc_mem_lock(unit, EGR_MACDA_OUI_PROFILEm);
        if (macda_oui_profile_index != -1) {
            rv = soc_profile_mem_delete(unit, _bcm_th_macda_oui_profile[unit],
                                        macda_oui_profile_index);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
        
        /* delete an entry or decrement ref count from EGR_VNTAG_ETAG_PROFILE table */
        soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
        /* profile 0 is reserved for NON NIV/PE entries */
        if (vntag_etag_profile_index != 0) {
            rv = soc_profile_mem_delete(unit, _bcm_th_vntag_etag_profile[unit],
                                        vntag_etag_profile_index);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);

    }
    
    return rv;
}

/*
 * Function:
 *      _bcm_th_l3_vp_ent_parse
 * Purpose:
 *      TH helper routine used to parse hw l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 */
bcm_error_t
_bcm_th_l3_vp_ent_parse(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{

    uint32 profile_index;
    uint32 dst_vif;
    uint32 mac_field;
    bcm_gport_t vp_gport;
    uint32 vntag_actions;
    int rv = BCM_E_NONE;
    uint32 glp;                   /* Global port.                */
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *buf_p;                /* HW buffer address.          */
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;
    int is_trunk = -1;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    buf_p = (uint32 *)l3x_entry;

    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    soc_mem_mac_address_get(unit, mem, buf_p, fld->macda_low, 
                            l3cfg->l3c_mac_addr,
                            SOC_MEM_MAC_LOWER_ONLY);
    /* Read upper 3 bytes of MAC address from profile table */
    profile_index = soc_mem_field32_get(unit, mem, buf_p,
                                              fld->oui_profile_id);
    SOC_IF_ERROR_RETURN
        (READ_EGR_MACDA_OUI_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                     &macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_get(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    l3cfg->l3c_mac_addr[0] = (uint8) (mac_field >> 16 & 0xff);
    l3cfg->l3c_mac_addr[1] = (uint8) (mac_field >> 8 & 0xff);
    l3cfg->l3c_mac_addr[2] = (uint8) (mac_field & 0xff);
    glp = soc_mem_field32_get(unit, mem, buf_p, fld->glp);
    _bcm_th_glp_resolve_embedded_nh(unit, glp, &l3cfg->l3c_modid,
                                    &l3cfg->l3c_port_tgid, &is_trunk);
    if (is_trunk == 1) {
        l3cfg->l3c_flags |= BCM_L3_TGID;
    }
    
    dst_vif = soc_mem_field32_get(unit, mem, buf_p,
                                  fld->dst_vif);
    profile_index = soc_mem_field32_get(unit, mem, buf_p,
                                        fld->vntag_etag_profile_id);

    if (profile_index != L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE) {
        SOC_IF_ERROR_RETURN
            (READ_EGR_VNTAG_ETAG_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                         &vntag_etag_profile_entry));
        soc_EGR_VNTAG_ETAG_PROFILEm_field_get(unit, &vntag_etag_profile_entry,
                                              VNTAG_ACTIONSf, &vntag_actions);
        switch (vntag_actions) {
            case VNTAG_ACTIONS_NIV: /* NIV encap */
                if (_bcm_niv_gport_get(unit, is_trunk, l3cfg->l3c_modid,
                                       l3cfg->l3c_port_tgid, dst_vif,
                                       &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            case VNTAG_ACTIONS_ETAG: /* PE encap */
                if (_bcm_extender_gport_get(unit, is_trunk, l3cfg->l3c_modid,
                                            l3cfg->l3c_port_tgid, dst_vif,
                                            &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            default:
                break;
        }
    }
    
    return rv;
}


/*
 * Function:
 *      _bcm_th_l3_vp_entry_add
 * Purpose:
 *      TH helper routine used to parse hw l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      buf_p     - (IN) hw buffer.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_l3_vp_entry_add(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                        uint32 *buf_p, int *macda_oui_profile_index,
                        int *vntag_etag_profile_index)
{

    int rv = BCM_E_NONE;
    int rv1 = BCM_E_NONE;
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *bufp;                 /* HW buffer address.          */
    uint32 profile_idx;
    uint32 mac_field;
    uint32 vntag_actions;
    void *entries[1];
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    /* Set table fields */
    BCM_TD2_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    bufp = buf_p;
    
    if (soc_feature(unit, soc_feature_virtual_port_routing) &&
        (l3cfg->l3c_encap_id > 0 &&
         l3cfg->l3c_encap_id < BCM_XGS3_EGRESS_IDX_MIN)) {
        int virtual_interface_id;
        uint32 hg_vntag_modify = 1;
        if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id,
                             _bcmVpTypeNiv)) { /* NIV */
            bcm_niv_port_t niv_port;
            bcm_niv_egress_t niv_egress;
            int count;
            BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id,
                                      l3cfg->l3c_encap_id);
            BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit, &niv_port));
            if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
                BCM_IF_ERROR_RETURN(bcm_esw_niv_egress_get(unit,
                                    niv_port.niv_port_id, 1,
                                    &niv_egress, &count));
                if (count == 0) {
                    return BCM_E_CONFIG;
                }
                if (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) {
                    return BCM_E_PARAM;
                }
                virtual_interface_id = niv_egress.virtual_interface_id;
            } else {
                if (niv_port.flags & BCM_NIV_PORT_MULTICAST) {
                    return BCM_E_PARAM;
                }
                virtual_interface_id = niv_port.virtual_interface_id;
            }
            soc_mem_field32_set(unit, mem, bufp, fld->dst_vif,
                                virtual_interface_id);
            
            sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
            vntag_actions = 1;
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  VNTAG_ACTIONSf, &vntag_actions);
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_NIV_ENCAP_LOCAL) {
                soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                      HG_VNTAG_MODIFY_ENABLEf, &hg_vntag_modify);
            }
            entries[0] = &vntag_etag_profile_entry;
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_th_vntag_etag_profile[unit],
                                                    entries, 1, &profile_idx));
            *vntag_etag_profile_index = profile_idx;
            soc_mem_field32_set(unit, mem, bufp, fld->vntag_etag_profile_id,
                                profile_idx);
        } else if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id,
                                    _bcmVpTypeExtender)) { /* PE */
            bcm_extender_port_t ep;
            int etag_dot1p_mapping_ptr = 0;
            BCM_GPORT_EXTENDER_PORT_ID_SET(ep.extender_port_id,
                                           l3cfg->l3c_encap_id);
            BCM_IF_ERROR_RETURN(bcm_esw_extender_port_get(unit, &ep));
            if (ep.flags & BCM_EXTENDER_PORT_MULTICAST) {
                return BCM_E_PARAM;
            }
            sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
            vntag_actions = 2;
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  VNTAG_ACTIONSf, &vntag_actions);
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_NIV_ENCAP_LOCAL) {
                soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                      HG_VNTAG_MODIFY_ENABLEf, &hg_vntag_modify);
            }                    
            bcm_td2_qos_egr_etag_id2profile(unit, ep.qos_map_id,
                                            &etag_dot1p_mapping_ptr);
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  ETAG_DOT1P_MAPPING_PTRf,
                                                  (uint32 *)&etag_dot1p_mapping_ptr);
            
            entries[0] = &vntag_etag_profile_entry;
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_th_vntag_etag_profile[unit],
                                                    entries, 1, &profile_idx));
            *vntag_etag_profile_index = profile_idx;
            soc_mem_field32_set(unit, mem, bufp, fld->vntag_etag_profile_id,
                                profile_idx);
            virtual_interface_id = ep.extended_port_vid;
            soc_mem_field32_set(unit, mem, bufp, fld->dst_vif, virtual_interface_id);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        soc_mem_field32_set(unit, mem, bufp, fld->vntag_etag_profile_id,
                            L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE);
    }
    soc_mem_mac_address_set(unit, mem, bufp, fld->macda_low,
                            l3cfg->l3c_mac_addr,
                            SOC_MEM_MAC_LOWER_ONLY);
    mac_field = ((l3cfg->l3c_mac_addr[0] << 16) |
                 (l3cfg->l3c_mac_addr[1] << 8)  |
                 (l3cfg->l3c_mac_addr[2] << 0));
    sal_memset(&macda_oui_profile_entry, 0, sizeof(macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_set(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    entries[0] = &macda_oui_profile_entry;
    rv = soc_profile_mem_add(unit, _bcm_th_macda_oui_profile[unit],
                             entries, 1, &profile_idx);
    if (BCM_FAILURE(rv)) {
        /* delete previously added entry or decrement ref count
         * from EGR_VNTAG_ETAG_PROFILE table
         */
        soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
        /* profile 0 is reserved for NON NIV/PE entries */
        if (*vntag_etag_profile_index != 0) {
            rv1 = soc_profile_mem_delete(unit, _bcm_th_vntag_etag_profile[unit],
                                         *vntag_etag_profile_index);
        }
        if (BCM_FAILURE(rv1)) {
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
            return rv1;
        }
        soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
        return rv;
    }
  
    *macda_oui_profile_index = profile_idx;
    soc_mem_field32_set(unit, mem, bufp, fld->oui_profile_id,
                        profile_idx);
    
    return rv;
}

/*
 * Function:
 *      th_glp_resolve
 * Purpose:
 *      TH helper routine  function to get modid, port, and trunk_id
 *      from a bcm_gport_t
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  gport     - Global port identifier
 *               (OUT) modid     - Module ID
 *               (OUT) port      - Port number
 *               (OUT) is_trunk  - Trunk

 * Returns:
 *      BCM_X_XXX
 */

void _bcm_th_glp_resolve_embedded_nh(int unit, uint32 gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     int *is_trunk)
{

    uint8 port_mask_len = 0, modid_mask_len = 0;
    uint32 temp = 0;    
               
    /* Determine length of port mask */
    temp = SOC_PORT_ADDR_MAX(unit);
    while (0 != temp) {
        port_mask_len++;
        temp >>= 1;
    }
    
    /* Determine length of modid mask */
    temp = SOC_MODID_MAX(unit);
    while (0 != temp) {
        modid_mask_len++;
        temp >>= 1;
    }
    *modid = (gport >> port_mask_len) & SOC_MODID_MAX(unit);   
                  
    if (gport & (1 << (port_mask_len + modid_mask_len))) {
        *is_trunk = 1;
    }
    *port = gport & SOC_PORT_ADDR_MAX(unit);
}

/*
 * Function:
 *      _bcm_th_l3_ext_ent_init
 * Purpose:
 *      TH helper routine used to init extended l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 extended host memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_th_l3_ext_ent_init(int unit, soc_mem_t mem_ext, 
                        _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 


    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Zero destination buffer. */
    buf_p = (uint32 *)l3x_entry;
    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem_ext)); 

    if (ipv6) { /* ipv6 entry */
        if (BCM_XGS3_L3_MEM(unit, v6_4) == mem_ext) { 
            soc_mem_ip6_addr_set(unit, mem_ext, buf_p, IPV6UC_EXT__IP_ADDR_LWR_64f,
                                 l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
            soc_mem_ip6_addr_set(unit, mem_ext, buf_p, IPV6UC_EXT__IP_ADDR_UPR_64f,
                                l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
            soc_mem_field32_set(unit, mem_ext, buf_p, IPV6UC_EXT__VRF_IDf,
                                l3cfg->l3c_vrf);
            /* ipv6 extended host entry */
            soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_0f, 3);
            soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_1f, 3);
            soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_2f, 3);
            soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_3f, 3);

            soc_mem_field32_set(unit, mem_ext, buf_p, VALID_0f, 1); 
            soc_mem_field32_set(unit, mem_ext, buf_p, VALID_1f, 1); 
            soc_mem_field32_set(unit, mem_ext, buf_p, VALID_2f, 1); 
            soc_mem_field32_set(unit, mem_ext, buf_p, VALID_3f, 1); 
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else { /* ipv4 entry */
        if (BCM_XGS3_L3_MEM(unit, v4_2) == mem_ext) { 
            soc_mem_field32_set(unit, mem_ext, buf_p, IPV4UC_EXT__IP_ADDRf,
                                l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem_ext, buf_p, IPV4UC_EXT__VRF_IDf,
                                l3cfg->l3c_vrf);
            /* ipv4 extended host entry */
            soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_0f, 1);
            soc_mem_field32_set(unit, mem_ext, buf_p, KEY_TYPE_1f, 1);
            soc_mem_field32_set(unit, mem_ext, buf_p, VALID_1f, 1); 
            soc_mem_field32_set(unit, mem_ext, buf_p, VALID_0f, 1); 
        } else {
            return BCM_E_NOT_FOUND;
        }
    }
    return BCM_E_NONE;
    
}

/*
 * Function:
 *      _bcm_th_l3_ent_init
 * Purpose:
 *      TH helper routine used to init l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      mem       - (IN) L3 host memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_th_l3_ent_init(int unit, soc_mem_t mem, 
                    _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 *buf_p;                /* HW buffer address.          */ 


    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Zero destination buffer. */
    buf_p = (uint32 *)l3x_entry;
    sal_memset(buf_p, 0, BCM_L3_MEM_ENT_SIZE(unit, mem)); 

    if (ipv6) { /* ipv6 entry */
        if (BCM_XGS3_L3_MEM(unit, v6) == mem) {
            soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDR_LWR_64f,
                                 l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
            soc_mem_ip6_addr_set(unit, mem, buf_p, IP_ADDR_UPR_64f,
                                l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
            soc_mem_field32_set(unit, mem, buf_p, VRF_IDf,
                                l3cfg->l3c_vrf);
            soc_mem_field32_set(unit, mem, buf_p, VALID_0f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, VALID_1f, 1); 
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_0f, 2);
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPE_1f, 2);
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else { /* ipv4 entry */
        if (BCM_XGS3_L3_MEM(unit, v4) == mem) {
            soc_mem_field32_set(unit, mem, buf_p, IP_ADDRf,
                                l3cfg->l3c_ip_addr);
            soc_mem_field32_set(unit, mem, buf_p, VRF_IDf,
                                l3cfg->l3c_vrf);
            /* ipv4 unicast */
            soc_mem_field32_set(unit, mem, buf_p, KEY_TYPEf, 0); 
            soc_mem_field32_set(unit, mem, buf_p, VALIDf, 1); 
        } else {
            return BCM_E_NOT_FOUND;
        }
    }
    return BCM_E_NONE;
    
}

/*
 * Function:
 *      _bcm_th_get_extended_profile_index
 * Purpose:
 *      TH helper routine used to get profile index for EGR_MACDA_OUI_PROFILE
 *      and EGR_VNTAG_ETAG_PROFILE
 * Parameters:
 *      unit                      - (IN) SOC unit number. 
 *      mem                       - (IN) L3 Extended table memory.
 *      l3cfg                     - (IN) L3 entry info.
 *      macda_oui_profile_index   - (out) EGR_MACDA_OUI profile index.
 *      vntag_etag_profile_index  - (out) EGR_VNTAG_ETAG profile index.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t _bcm_th_get_extended_profile_index(int unit, soc_mem_t mem_ext,
                                               _bcm_l3_cfg_t *l3cfg,
                                               int *macda_oui_profile_index,
                                               int *vntag_etag_profile_index,
                                               int *ref_count) {
                                        
    int ipv6;                          /* IPv6 entry indicator.*/
    int rv = BCM_E_NONE;               /* Operation return status. */
    uint32 *buf_key, *buf_entry;       /* Key and entry buffer ptrs*/
    l3_entry_ipv4_unicast_entry_t l3v4_key, l3v4_entry;            /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_key, l3v4_ext_entry; /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_key, l3v6_entry;           /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_key, l3v6_ext_entry; /* IPv6-Embedded */
    soc_field_t macda_oui_id, vntag_etag_id;
    soc_mem_t mem;
   

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    
    if (ipv6) {
        macda_oui_id  = IPV6UC_EXT__MAC_DA_OUI_PROFILE_IDf;
        vntag_etag_id = IPV6UC_EXT__VNTAG_ETAG_PROFILE_IDf;
        mem = BCM_XGS3_L3_MEM(unit, v6);
    } else {
        macda_oui_id  = IPV4UC_EXT__MAC_DA_OUI_PROFILE_IDf;
        vntag_etag_id = IPV4UC_EXT__VNTAG_ETAG_PROFILE_IDf;
        mem = BCM_XGS3_L3_MEM(unit, v4);
    }

    /* Assign entry-key buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_key,
                              l3v4_key,
                              l3v4_ext_key,
                              l3v6_key,
                              l3v6_ext_key);

    /* Assign entry buf based on table being used */
    BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem_ext, buf_entry,
                              l3v4_entry,
                              l3v4_ext_entry,
                              l3v6_entry,
                              l3v6_ext_entry);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN
    (_bcm_th_l3_ext_ent_init(unit, mem_ext, l3cfg, buf_key));

    /* Perform lookup hw. */
    rv = soc_mem_generic_lookup(unit, mem_ext, MEM_BLOCK_ANY,
                                _BCM_TD2_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (BCM_SUCCESS(rv)) {
        *macda_oui_profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                       macda_oui_id);
        *vntag_etag_profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                        vntag_etag_id);
        rv = soc_profile_mem_ref_count_get(unit, 
                                           _bcm_th_macda_oui_profile[unit],
                                           *macda_oui_profile_index,
                                           ref_count);
        return rv;
    } else if (BCM_E_NOT_FOUND == rv) {
        /* Check if entry was added in regular L3_ENTRY table view */

        /* Assign entry-key buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_key,
                                  l3v4_key,
                                  l3v4_ext_key,
                                  l3v6_key,
                                  l3v6_ext_key);

        /* Assign entry buf based on table being used */
        BCM_TD2_L3_HOST_ENTRY_BUF(ipv6, mem, buf_entry,
                                  l3v4_entry,
                                  l3v4_ext_entry,
                                  l3v6_entry,
                                  l3v6_ext_entry);
        
        /* Prepare lookup key. */
        BCM_IF_ERROR_RETURN(_bcm_th_l3_ent_init(unit, mem, l3cfg, buf_key));

        /* Perform lookup */
        rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                    _BCM_TD2_L3_MEM_BANKS_ALL,
                                    buf_key, buf_entry, &l3cfg->l3c_hw_index);
        return rv;
    }

    return rv;
   
}

#endif

