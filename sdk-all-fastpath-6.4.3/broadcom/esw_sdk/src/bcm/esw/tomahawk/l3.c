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
extern int soc_th_get_alpm_banks(int unit);

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
    soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, &ipv6);

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
    soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg->defip_vrf, &tmp);

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
    int bkt_idx = 0;            /*                              */
    int bkt_ptr = 0;            /* Bucket pointer               */
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
    
    /* Walk all lpm entries */
    for (idx = 0; idx < idx_end; idx++) {
        /* Calculate entry ofset */
        lpm_entry =
            soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                         defip_entry_t *, lpm_tbl_ptr, idx);
       
        ipv6 = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0f);

        /* Calculate LPM table traverse step count */
        if (ipv6) {
            if (SOC_TH_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
                step_count = 2;
            } else {
                step_count = 1;
            }
        } else {
            step_count = 2;
        }
      
        /* Insert LPM entry into HASH table and init LPM trackers */ 
        if (BCM_FAILURE(soc_alpm_warmboot_lpm_reinit(unit, ipv6, 
                                                     idx, lpm_entry))) {
            goto free_lpm_table;
        }
        
        /*
         * This loop is used for two purposes
         *  1. IPv4, DEFIP entry has two IPv4 entries.
         *  2. IPv6 double wide mode, walk next bucket.
         */
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx) {  /* If index == 1*/
                if (!ipv6) {
                    /* Copy upper half of lpm entry to lower half */
                    soc_th_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, lpm_entry, TRUE);
               
                    /* Invalidate upper half */
                    soc_L3_DEFIPm_field32_set(unit, lpm_entry, VALID1f, 0);
                }
            }
    
            /* Make sure entry is valid. */
            if (!soc_L3_DEFIPm_field32_get(unit, lpm_entry, VALID0f)) {
                continue;
            }

            if (ipv6 && tmp_idx) {
                /* 
                 * IPv6 double wide mode,  Walk bucket entries at next bucket.
                 */
                bkt_ptr++;
            } else {
                /* Extract bucket pointer from LPM entry */
                bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                              ALG_BKT_PTR0f);

                /* Extract VRF from LPM entry*/
                if (BCM_FAILURE(soc_th_alpm_lpm_vrf_get
                                (unit, lpm_entry, &vrf_id, &vrf))) {
                    goto free_lpm_table;
                }

                /* VRF_OVERRIDE (Global High) entries, prefix resides in TCAM */
                if (ALPM_PREFIX_IN_TCAM(unit,vrf_id)) {
                    continue;
                }
                    
                pivot_idx = (idx << 1) + tmp_idx;

                /* TCAM pivot recovery */
                if(BCM_FAILURE(soc_alpm_warmboot_pivot_add(unit, ipv6, lpm_entry, pivot_idx,
                                                           bkt_ptr))) {
                    goto free_lpm_table;
                }
            }

            /* Set bucket bitmap */ 
            if(BCM_FAILURE(soc_alpm_warmboot_bucket_bitmap_set(unit, ipv6, bkt_ptr))) {
                goto free_lpm_table;
            }

            if (ipv6) {
                /* IPv6 */
                alpm_mem    = L3_DEFIP_ALPM_IPV6_64m;
                alpm_entry  = &alpm_entry_v6_64;
                bkt_count   = ALPM_IPV6_64_BKT_COUNT;
                entry_count = 4;
            } else {
                /* IPv4 */
                alpm_mem    = L3_DEFIP_ALPM_IPV4m;
                bkt_count   = ALPM_IPV4_BKT_COUNT;
                alpm_entry  = &alpm_entry_v4;
                entry_count = 6;
            }

            if (SOC_URPF_STATUS_GET(unit)) {
                defip_table_size >>= 1;
            }

            entry_num = 0;
            bank_num = 0;

            /* Get the bucket pointer from lpm entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << 16) | (bkt_ptr << 2) | 
                           (bank_num & 0x3);
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
                    }
                }

                /* Read entry from bucket memory */
                if (BCM_FAILURE(soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                                  bkt_addr, alpm_entry))) {
                    goto free_lpm_table;
                }

                /* Check if ALPM entry is valid */
                if (!soc_mem_field32_get(unit, alpm_mem, alpm_entry, VALIDf)) {
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

                if(BCM_FAILURE(soc_alpm_warmboot_prefix_insert(unit, ipv6, lpm_entry,
                                         alpm_entry, pivot_idx, bkt_ptr, bkt_addr))) {
                    continue;
                }

            } /* End of bucket walk loop*/
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    if(BCM_FAILURE(soc_alpm_warmboot_lpm_reinit_done(unit))) {
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
    int bkt_idx = 0;            /*                              */
    int bkt_ptr = 0;            /* Bucket pointer               */
    int bank_num = 0;           /* Number of active SRAM banks  */
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
    uint32 rval;                /*                              */
    soc_mem_t alpm_mem;         /*                              */
    _bcm_defip_cfg_t lpm_cfg;   /* Buffer to fill route info.   */
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
    bkt_count   = ALPM_IPV6_128_BKT_COUNT;
    entry_count = 2;
    bank_count  = soc_th_get_alpm_banks(unit);
    
    if (SOC_TH_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
        step_count = 2;
    } else {
        step_count = 1;
    }
    
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
        if (BCM_FAILURE(soc_alpm_128_warmboot_lpm_reinit(unit, ipv6, 
                                                     idx, lpm_entry))) {
            goto free_lpm_table;
        }
        
        /*  If IPv6 double wide mode, walk next bucket.*/
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
    
            if (tmp_idx) {
                /* IPv6 double wide mode, Walk bucket entries at next bucket.*/
                bkt_ptr++;
            } else {
                /* Extract bucket pointer from LPM entry */
                bkt_ptr = soc_mem_field32_get(unit, L3_DEFIP_PAIR_128m, lpm_entry, 
                                              ALG_BKT_PTRf);

                /* Extract VRF from LPM entry*/
                if (BCM_FAILURE(soc_th_alpm_128_lpm_vrf_get
                                (unit, lpm_entry, &vrf_id, &vrf))) {
                    goto free_lpm_table;
                }

                /* VRF_OVERRIDE (Global High) entries, prefix resides in TCAM */
                if (ALPM_PREFIX_IN_TCAM(unit,vrf_id)) {
                    continue;
                }
                    
                /* TCAM pivot recovery */
                if(BCM_FAILURE(soc_alpm_128_warmboot_pivot_add(unit, ipv6,
                               lpm_entry, idx, bkt_ptr))) {
                    goto free_lpm_table;
                }
            }

            /* Set bucket bitmap */ 
            if(BCM_FAILURE(soc_alpm_128_warmboot_bucket_bitmap_set(unit, ipv6, 
                           bkt_ptr))) {
                goto free_lpm_table;
            }

            /* Get the bucket pointer from lpm entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                bkt_addr = (entry_num << 16) | (bkt_ptr << 2) | 
                           (bank_num & 0x3);
                entry_num++; 
                
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
                    }
                }

                /* Read entry from bucket memory */
                if (BCM_FAILURE(soc_mem_read(unit, alpm_mem, MEM_BLOCK_ANY,
                                  bkt_addr, alpm_entry))) {
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

                _soc_tomahawk_alpm_bkt_view_set(unit, bkt_addr, alpm_mem);

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_128_ent_parse(unit, L3_DEFIP_PAIR_128m, 
                                            (uint32 *) lpm_entry, 
                                            &lpm_cfg, &nh_ecmp_idx);
               
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

                if(BCM_FAILURE(soc_alpm_128_warmboot_prefix_insert(unit, ipv6, lpm_entry,
                                         alpm_entry, idx, bkt_ptr, bkt_addr))) {
                    continue;
                }

            } /* End of bucket walk loop*/
        } /* End of lpm entry upper/lower half traversal */
    } /* End of lpm table traversal */

    if(BCM_FAILURE(soc_alpm_128_warmboot_lpm_reinit_done(unit))) {
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
    defip_pair_128_entry_t lpm_entry_data;   /* Hw entry buffer.    */
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
    bkt_count   = ALPM_IPV6_128_BKT_COUNT;
    idx_start   = 0;
    bank_num    = 0;
    entry_num   = 0;
    bank_count  = soc_th_get_alpm_banks(unit);
    entry_count = 2;
    alpm_entry  = &alpm_entry_v6_128;
    bank_bits  = (bank_count + 1) / 2;
    if (bank_bits == 1) {
        bkt_count >>= 1;
    }    

    if (SOC_TH_ALPM_V6_SCALE_CHECK(unit, 1)) {
        bkt_count <<= 1;
    }

    /* if v6 loop twice once with with defip and once with defip_pair */
    for (idx = (idx_end - 1); idx >= idx_start; idx--) {    

        if (!(trv_data->flags & BCM_L3_D_HIT)) {
            /* Calculate entry ofset. */
            lpm_entry =
                soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                        defip_pair_128_entry_t *, lpm_tbl_ptr, idx);
        } else {       
            /* For 'delete-all' call back, read entry from hardware table */
            rv = soc_mem_read(unit, L3_DEFIP_PAIR_128m, MEM_BLOCK_ANY,
                                       idx, &lpm_entry_data);
            if (SOC_FAILURE(rv)) {
                break;
            }
            lpm_entry = &lpm_entry_data;
        }

        /* Make sure entry is valid. */
        if (!soc_mem_field32_get(unit, pivot_mem, lpm_entry, VALID0_LWRf)) {
            continue;
        }
        
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            bkt_ptr = soc_mem_field32_get(unit, pivot_mem, lpm_entry, 
                                      ALG_BKT_PTRf);
            if (bkt_ptr == 0) {
                /* OVERRIDE VRF Processing */ 
                _th_defip_pair128_get_key(unit, (uint32 *) lpm_entry, &lpm_cfg);

                _bcm_th_alpm_128_ent_parse(unit, pivot_mem, 
                                            (uint32 *) lpm_entry, 
                                            &lpm_cfg, &nh_ecmp_idx);
                lpm_cfg.defip_index = idx;
                
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
            
                /* Skip second iteration for VRF OVERRIDE routes */
                tmp_idx++;

                continue;
            }

            /* get the bucket pointer from lpm mem  entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* calculate bucket memory address */
                /* also increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + 13)) | (bkt_ptr << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
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

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse  the entry. */
                /* Fill entry ip address &  subnet mask. */
                _bcm_th_alpm_128_get_addr(unit, alpm_mem, 
                                           (uint32 *) alpm_entry, &lpm_cfg);
                
                soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
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

        if (!(trv_data->flags & BCM_L3_D_HIT)) {
            /* Calculate entry ofset. */
            lpm_entry =
                        soc_mem_table_idx_to_pointer(unit, L3_DEFIP_PAIR_128m,
                                        defip_pair_128_entry_t *, lpm_tbl_ptr,
                                        def_rte_arr[idx].idx);
        } else {       
            /* For 'delete-all' call back, read entry from hardware table */
            rv = soc_mem_read(unit, L3_DEFIP_PAIR_128m, 
                                MEM_BLOCK_ANY,
                                def_rte_arr[idx].idx, &lpm_entry_data);
            if (SOC_FAILURE(rv)) {
                break;
            }
            lpm_entry = &lpm_entry_data;
        }

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
        soc_th_alpm_128_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
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

    if (!ipv6) {
        alpm_mem = L3_DEFIP_ALPM_IPV4m;
        bkt_count = ALPM_IPV4_BKT_COUNT;
    } else {
        alpm_mem = L3_DEFIP_ALPM_IPV6_64m;
        bkt_count = ALPM_IPV6_64_BKT_COUNT;
    }
    if (SOC_URPF_STATUS_GET(unit)) {
        defip_table_size >>= 1;
    }

    idx_end    = defip_table_size;
    bank_num   = 0;
    entry_num  = 0;
    bank_count = soc_th_get_alpm_banks(unit);
    bank_bits  = (bank_count + 1) / 2;
    if (bank_bits == 1) {
        bkt_count >>= 1;
    }
    
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

    if (SOC_TH_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
        bkt_count <<= 1;
    }

    step_count = ipv6 ? 1 : 2;

    /* Walk all lpm entries */
    for (idx = (idx_end - 1); idx >= idx_start; idx--) {
    
        if (!(trv_data->flags & BCM_L3_D_HIT)) {
            /* Calculate entry ofset. */
            lpm_entry =
                soc_mem_table_idx_to_pointer(unit, BCM_XGS3_L3_MEM(unit, defip),
                                             defip_entry_t *, lpm_tbl_ptr, idx);
        } else {       
            /* For 'delete-all' callback, read entry from hardware table */
            rv = soc_mem_read(unit, BCM_XGS3_L3_MEM(unit, defip), 
                                      MEM_BLOCK_ANY, idx, &lpm_entry_data);
            if (SOC_FAILURE(rv)) {
                break;
            }
            lpm_entry = &lpm_entry_data;
        }
       
        if (ipv6 && (soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, MODE0f) == 0)) {
           /*
            * Function called for IPv6 LPM/ALPM walk and LPM entry is IPv4; Continue;
            */
           continue;
        }
        
        /* Each LPM index has two IPv4 entries*/
        for (tmp_idx = 0; tmp_idx < step_count; tmp_idx++) {
            if (tmp_idx & 0x1) {
                /* Check second part of the entry. */
                soc_th_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, lpm_entry, TRUE);
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

            soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                &cmp_result);
       
            if (ALPM_PREFIX_IN_TCAM(unit, lpm_cfg.defip_vrf)) {
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
            
            /* Get the bucket pointer from lpm mem entry */
            for (bkt_idx = 0; bkt_idx < bkt_count; bkt_idx++) {
                /* Calculate bucket memory address */
                /* Increment so next bucket address can be calculated */
                bkt_addr = (entry_num << (bank_bits + 13)) | (bkt_ptr << bank_bits) | 
                           (bank_num & ((1U << bank_bits) - 1));
                entry_num++; 
                if (entry_num == entry_count) {
                    entry_num = 0;
                    bank_num ++;
                    if (bank_num == bank_count) {
                        bank_num = 0;
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

                /* Zero destination buffer first. */
                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));

                /* Parse the entry. */
                _bcm_th_alpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry,
                                        alpm_mem, alpm_entry);
        
                /* If protocol doesn't match skip the entry. */
                if ((lpm_cfg.defip_flags & BCM_L3_IP6) !=  ipv6) {
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

        soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
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
#endif

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
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_l3_vp_entry_del(int unit, int ipv6, _bcm_l3_cfg_t *l3cfg, uint32 *bufentry) {

    soc_field_t macda_oui_id;
    soc_field_t vntag_etag_id;
    int rv = BCM_E_NONE;
    soc_mem_t mem_ext;            /* L3 table memory.     */
    uint32 *buf_entry;            /* Entry buffer ptrs    */

    
    buf_entry = bufentry;
    if (ipv6) {
        mem_ext = BCM_XGS3_L3_MEM(unit, v6_4);
        macda_oui_id = IPV6UC_EXT__MAC_DA_OUI_PROFILE_IDf;
        vntag_etag_id = IPV6UC_EXT__VNTAG_ETAG_PROFILE_IDf;
    } else {
        mem_ext = BCM_XGS3_L3_MEM(unit, v4_2);
        macda_oui_id = IPV4UC_EXT__MAC_DA_OUI_PROFILE_IDf;
        vntag_etag_id = IPV4UC_EXT__VNTAG_ETAG_PROFILE_IDf;
    }
    
    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        int profile_index;
        /* delete an entry or decrement ref count from EGR_MACDA_OUI_PROFILE table */
        profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                            macda_oui_id);
        soc_mem_lock(unit, EGR_MACDA_OUI_PROFILEm);
        rv = soc_profile_mem_delete(unit, _bcm_th_macda_oui_profile[unit],
                                    profile_index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
        
        /* delete an entry or decrement ref count from EGR_VNTAG_ETAG_PROFILE table */
        if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id, _bcmVpTypeNiv) ||
            _bcm_vp_used_get(unit, l3cfg->l3c_encap_id, _bcmVpTypeExtender)) {
            profile_index = soc_mem_field32_get(unit, mem_ext, buf_entry,
                                                vntag_etag_id);
            soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
            rv = soc_profile_mem_delete(unit, _bcm_th_vntag_etag_profile[unit],
                                        profile_index);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
                return rv;
            }
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
        }
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
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_th_l3_vp_ent_parse(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg, void *l3x_entry) {

    uint32 profile_index;
    uint32 dst_vif;
    uint32 mac_field;
    bcm_gport_t vp_gport;
    uint32 vntag_actions;
    int rv = BCM_E_NONE;
    uint8 port_mask_len = 0, modid_mask_len = 0;
    uint32 temp = 0;
    uint32 glp;                   /* Global port.                */
    int ipv6;                     /* Entry is IPv6 flag.         */
    _bcm_l3_fields_t *fld;        /* L3 table common fields.     */
    uint32 *buf_p;                /* HW buffer address.          */
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;

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
    l3cfg->l3c_modid = (glp >> port_mask_len)
                        & SOC_MODID_MAX(unit);
    if (glp & (1 << (port_mask_len + modid_mask_len))) {
        l3cfg->l3c_flags |= BCM_L3_TGID;
    }
    dst_vif = soc_mem_field32_get(unit, mem, buf_p,
                                  fld->dst_vif);
    profile_index = soc_mem_field32_get(unit, mem, buf_p,
                                        fld->vntag_etag_profile_id);
    if (profile_index == L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE) {
        l3cfg->l3c_port_tgid = glp & SOC_PORT_ADDR_MAX(unit);
    } else {
        SOC_IF_ERROR_RETURN
            (READ_EGR_VNTAG_ETAG_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                         &vntag_etag_profile_entry));
        soc_EGR_VNTAG_ETAG_PROFILEm_field_get(unit, &vntag_etag_profile_entry,
                                              VNTAG_ACTIONSf, &vntag_actions);
        switch (vntag_actions) {
            case VNTAG_ACTIONS_NIV: /* NIV encap */
                if (_bcm_niv_gport_get(unit, glp, dst_vif,
                                       &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            case VNTAG_ACTIONS_ETAG: /* PE encap */
                if (_bcm_extender_gport_get(unit, glp, dst_vif,
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
_bcm_th_l3_vp_entry_add(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg, uint32 *buf_p) {

    int rv = BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_th_macda_oui_profile[unit],
                                            entries, 1, &profile_idx));
    soc_mem_field32_set(unit, mem, bufp, fld->oui_profile_id,
                        profile_idx);
    
    return rv;
}

#endif

