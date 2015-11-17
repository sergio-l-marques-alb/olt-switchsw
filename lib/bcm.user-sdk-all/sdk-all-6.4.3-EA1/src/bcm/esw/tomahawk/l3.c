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
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/trident2.h>

#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/virtual.h>

#if defined(BCM_TRX_SUPPORT) 
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#define _BCM_TD2_L3_MEM_BANKS_ALL     (-1)
#define _BCM_TD2_HOST_ENTRY_NOT_FOUND (-1)

#define TH_IPMC_NO_SRC_CHECK_PORT      0xFF

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
            if (SOC_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
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

                _soc_trident2_alpm_bkt_view_set(unit, bkt_addr, alpm_mem);

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
    
    if (SOC_ALPM_V6_SCALE_CHECK(unit, ipv6)) {
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

                _soc_trident2_alpm_bkt_view_set(unit, bkt_addr, alpm_mem);

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
    int entry_num, bank_num, entry_count, bank_count;
    defip_alpm_ipv6_128_entry_t alpm_entry_v6_128;
    void *alpm_entry;
    soc_mem_t alpm_mem, pivot_mem = L3_DEFIP_PAIR_128m;
    int step_count;
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

    if (SOC_ALPM_V6_SCALE_CHECK(unit, 1)) {
        step_count = 2;
    } else {
        step_count = 1;
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
            if (tmp_idx) {
                bkt_ptr++;
            } else {
                bkt_ptr = soc_mem_field32_get(unit, pivot_mem, lpm_entry, 
                                          ALG_BKT_PTRf);
            }

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

    if (ipv6) {
        if (SOC_ALPM_V6_SCALE_CHECK(unit, 1)) {
            step_count = 2;
        } else {
            step_count = 1;
        }
    } else {
        step_count = 2;
    }

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
            if (tmp_idx) {
                if (!ipv6) {
                    /* Check second part of the entry. */
                    soc_th_alpm_lpm_ip4entry1_to_0(unit, lpm_entry, lpm_entry, TRUE);
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

            soc_th_alpm_lpm_vrf_get(unit, lpm_entry, &lpm_cfg.defip_vrf,
                                &cmp_result);
       
            if (ALPM_PREFIX_IN_TCAM(unit,lpm_cfg.defip_vrf)) {

                if (ipv6 && tmp_idx) {
                    /* traverse lpm_entry only once for v6 OVERRIDE */
                    break;
                }

                sal_memset(&lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
                _bcm_th_lpm_ent_parse(unit, &lpm_cfg, &nh_ecmp_idx, lpm_entry);
                _bcm_th_lpm_ent_get_key(unit, &lpm_cfg, lpm_entry);

                lpm_cfg.defip_index = idx;

                if (ipv6 == (lpm_cfg.defip_flags & BCM_L3_IP6)) {
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
                continue;
            }

            if (ipv6 && tmp_idx) {
                bkt_ptr++;
            } else {
                bkt_ptr = soc_mem_field32_get(unit, L3_DEFIPm, lpm_entry, 
                                          ALG_BKT_PTR0f);
            }
            
            /* Get the bucket pointer from lpm mem entry */
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
#endif

