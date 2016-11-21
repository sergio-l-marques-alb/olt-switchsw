/*
 * $Id: l3.c,v 1.892 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:        l3.c
 * Purpose:     trident2plus specific L3 function implementations
 */
#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_RIOT_SUPPORT

#include <assert.h>

#include <sal/core/libc.h>
#include <shared/util.h>
#if defined(BCM_FIREBOLT_SUPPORT)
#include <soc/mem.h>
#include <soc/l3x.h>
#include <bcm/l3.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/virtual.h>

#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#endif /* BCM_TRIDENT_SUPPORT*/
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/nat.h>
#include <bcm_int/esw/qos.h>
#endif /* BCM_TRIDENT_SUPPORT*/
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT*/
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/failover.h>
#endif

/* initialize pointer for useful bokkkeeping information */
#define L3_BK_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])

/* Set ul-ol link params */
#define BCM_L3_NH_NUM_UNDERLAY_BUCKETS    1024     
ul_nh_link_t *ul_nh_assoc_head
    [BCM_MAX_NUM_UNITS][BCM_L3_NH_NUM_UNDERLAY_BUCKETS] = {{NULL}};

/*
 * Function:
 *      bcmi_l3_riot_bank_sel
 * Purpose:
 *      Initialize banks for L3 objects.
 *      The whole l3 table can be divided into 
 *      multiple layers and you can set different banks
 *      for different layers.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int 
bcmi_l3_riot_bank_sel(int unit)
{   
    uint32 bank_sel_reg_val     = 0;
    uint32 num_ol_banks      = 0;
    uint32 l2_tunnel_parse_control = 0;
    _bcm_l3_bookkeeping_t *l3   = L3_BK_INFO(unit);
    uint32 ol_bank_bit_value = 0;
    int iter;
    int num_nh, num_l3_intf;
    int max_nh_banks, max_l3_intf_banks;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    /* The interface entries can only be allocated in terms of 2k
     * banks. if user is allocating in between boundries then we need
     * to throw error as hardware cannot divide overlay and underlay 
     * entries in between bank.
     * If SDK does not return error here then user can start using
     * different entries in same banks for overlay and underlay and may
     * see unextected results.
     */
    if (l3->l3_intf_overlay_entries % BCMI_L3_INTF_ENT_PER_BANK) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "Please allocate entries in the multiple of %d.\n"), 
            BCMI_L3_INTF_ENT_PER_BANK));

        return BCM_E_PARAM;
    }

    if (l3->l3_nh_overlay_entries %  BCMI_L3_NH_ENT_PER_BANK) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "Please allocate entries in the multiple of %d.\n"), 
            BCMI_L3_NH_ENT_PER_BANK));

        return BCM_E_PARAM;
    }
    num_nh = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    max_nh_banks = num_nh / BCMI_L3_NH_ENT_PER_BANK;
    max_l3_intf_banks = num_l3_intf / BCMI_L3_INTF_ENT_PER_BANK;

    if (max_nh_banks == 0) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "No NH banks in system.\n")));
        return BCM_E_RESOURCE;
    }
    if (max_l3_intf_banks == 0) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "No EGR_L3_INTF banks in system.\n")));
        return BCM_E_RESOURCE;
    }
    /* 
     * set banks for EGR_L3_INTF 
     * By default ("alloc mode" is 0):
     * First set of banks are for overlay.
     * Second set of banks are for underlay.
     * If "alloc mode" is 1:
     * First set of banks are for underlay.
     * Second set of banks are for overlay.
     */
    num_ol_banks = (l3->l3_intf_overlay_entries / BCMI_L3_INTF_ENT_PER_BANK) +
        ((l3->l3_intf_overlay_entries % BCMI_L3_INTF_ENT_PER_BANK) ? 1 : 0);

    if (num_ol_banks > max_l3_intf_banks) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "ERROR : Overlay l3 interface(%d) > total interfaces(%d).\n"),
            l3->l3_intf_overlay_entries,
            max_l3_intf_banks * BCMI_L3_INTF_ENT_PER_BANK));

        return BCM_E_CONFIG;
    }

    /* Setup the bits for overlay banks. */
    for (iter = 0; iter < num_ol_banks; iter++) {
        ol_bank_bit_value |= (1 << iter) ;
    }
    if (l3->l3_intf_overlay_alloc_mode > 0) {
        ol_bank_bit_value <<= (max_l3_intf_banks - num_ol_banks);
    }

    soc_reg_field_set(unit, EGR_L3_INTF_BANK_SELr, &bank_sel_reg_val,
        BANK_SELf, ol_bank_bit_value);
    /* set banks in EGR_L3_INTF_BANK_SELr */
    SOC_IF_ERROR_RETURN(WRITE_EGR_L3_INTF_BANK_SELr
                          (unit, bank_sel_reg_val));

    ol_bank_bit_value = 0;
    bank_sel_reg_val     = 0;
    /* 
     * set banks for ING/EGR/INITIAL_ING_L3_NEXT_HOP. 
     * By default ("alloc mode" is 0):
     * First set of banks are for overlay.
     * Second set of banks are for underlay.
     * If "alloc mode" is 1:
     * First set of banks are for underlay.
     * Second set of banks are for overlay.
     */
    num_ol_banks = (l3->l3_nh_overlay_entries / BCMI_L3_NH_ENT_PER_BANK) +
        ((l3->l3_nh_overlay_entries % BCMI_L3_NH_ENT_PER_BANK) ? 1 : 0);
    if (num_ol_banks > max_nh_banks) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "ERROR : Overlay l3 Next hop (%d) > total l3 next hop(%d).\n"),
            l3->l3_nh_overlay_entries,
            max_nh_banks * BCMI_L3_NH_ENT_PER_BANK));

        return BCM_E_CONFIG;
    }
    for (iter=0; iter < num_ol_banks; iter++) {
        ol_bank_bit_value |= (1 << iter) ;
    }
    if (l3->l3_nh_overlay_alloc_mode > 0) {
        ol_bank_bit_value <<= (max_nh_banks - num_ol_banks);
    }

    /* set banks in INITIAL_ING_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, INITIAL_ING_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);

    SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* set banks in ING_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, ING_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);

    SOC_IF_ERROR_RETURN(WRITE_ING_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* set banks in EGR_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, EGR_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
    SOC_IF_ERROR_RETURN(WRITE_EGR_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* Enable the control to look into inner l3 header for RioT */
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, IFP_L2_TUNNEL_PAYLOAD_FIELD_SELf, 0x1);
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, PARSE_IPV4_PAYLOADf, 0x1);
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, PARSE_IPV6_PAYLOADf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_ING_L2_TUNNEL_PARSE_CONTROLr
        (unit, l2_tunnel_parse_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_riot_ecmp_level_sel
 * Purpose:
 *      Initialize banks for L3 objects.
 *      The whole l3 table can be divided into 
 *      multiple layers and you can set different banks
 *      for different layers.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int
bcmi_l3_riot_ecmp_level_sel(int unit)
{   
    uint32 ecmp_level_sel_reg_val = 0;
    uint32 rval = 0;

    /* 
     * If ecmp level is multi level then set ecmp config register to 1.
     * else set the ecmp config register to 0. 0 is default case for
     * legacy single level of ecmp.
     */
    ecmp_level_sel_reg_val = (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit) ? 1 : 0);
    soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_CONFIGURATIONf,
        ecmp_level_sel_reg_val);

    SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_l3_tables_init
 * Purpose:
 *      Initialize internal L3 tables and enable L3.
 *      TD2P has configurable multi domain banks for 
 *      various l3 objects. This routine sets up h/w 
 *      tables for corresponding objects and then calls 
 *      common tables init routine.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td2p_l3_tables_init(int unit)
{

#ifdef BCM_RIOT_SUPPORT
    /* Selects L3 banks based on user inputs */
    BCM_IF_ERROR_RETURN(bcmi_l3_riot_bank_sel(unit));
#endif
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    /* selects ecmp level */
    BCM_IF_ERROR_RETURN(bcmi_l3_riot_ecmp_level_sel(unit));
#endif
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_tables_init(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *       bcmi_l3_nh_assoc_ol_ul_link_hash_dump 
 * Purpose:
 *       dumps the whole hash table
 * Parameters:
 *           IN :  Unit
 * Returns:
 *        void
 */
void bcmi_l3_nh_assoc_ol_ul_link_hash_dump(int unit)
{   

    ol_nh_link_t *temp;
    ul_nh_link_t *hash;
    int i = 0;

    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "printing nh association hash table\n")));

    for (; i< BCM_L3_NH_NUM_UNDERLAY_BUCKETS; i++) {
        hash =  ul_nh_assoc_head[unit][i];
        while (hash != NULL) {
            temp = hash->ol_nh_assoc;
            while (temp != NULL) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "bkt_idx = %d : ol index = %d : ul index = %d \n"),
                    i, hash->u_nh_idx, temp->o_nh_idx));
                temp = temp->next_link;
            }
            hash = hash->ul_nh_link;
        }
    }
}

/*
 * Function:
 *      bcmi_nh_assoc_alloc_ol_link 
 * Purpose:
 *      Allocates link memory and sets overlay nh.
 * Parameters:
 *      ol_nh - overlay nh number
 * Returns:
 *      pointer to allocated link memory              
 */

ol_nh_link_t *
bcmi_nh_assoc_alloc_ol_link(int ol_nh)
{
    ol_nh_link_t *ptr ;

    ptr = (ol_nh_link_t *) sal_alloc(sizeof(ol_nh_link_t), "ol_nh_link" );

    sal_memset(ptr, 0, sizeof(ol_nh_link_t));
    ptr->o_nh_idx = ol_nh;
    return ptr;
}

/*
 * Function:
 *      bcmi_nh_assoc_alloc_ul_link 
 * Purpose:
 *      Allocates link memory and sets underlay nh.
 * Parameters:
 *      ul_nh - underlay nh number
 * Returns:
 *      pointer to allocated link memory              
 */
ul_nh_link_t *
bcmi_nh_assoc_alloc_ul_link(int ul_nh)
{   
    ul_nh_link_t *ptr;

    ptr = (ul_nh_link_t *) sal_alloc(sizeof(ul_nh_link_t), "ul_nh_link");
    sal_memset(ptr, 0, sizeof(ul_nh_link_t));

    ptr->u_nh_idx = ul_nh;
    return ptr;
}


/*
 * Function:
 *      bcmi_nh_assoc_bkt_get
 * Purpose:
 *      Adds a overlay/underlay association.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_nh_assoc_bkt_get(int nh_idx)
{


    return( _shr_crc16b(0, (uint8 *)(&nh_idx),
        BYTES2BITS(sizeof(nh_idx))) %
        BCM_L3_NH_NUM_UNDERLAY_BUCKETS);

}
/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_sw_add
 * Purpose:
 *      Adds a overlay/underlay association.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_sw_add(int unit, int ol_nh, int ul_nh)
{   

    ul_nh_link_t *ul_assoc, *prev_assoc, *ul_alloc;
    ol_nh_link_t *ol_link, *prev_link, *ol_alloc;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_nh_assoc_bkt_get(ul_nh);

    prev_assoc = ul_assoc = ul_nh_assoc_head[unit][bkt_idx];
    while (ul_assoc && (ul_assoc->u_nh_idx != ul_nh)) {
        prev_assoc = ul_assoc;
        ul_assoc = ul_assoc->ul_nh_link;
    }

    if (ul_assoc) {
        prev_link = ol_link = ul_assoc->ol_nh_assoc;
        while (ol_link && (ol_link->o_nh_idx != ol_nh)) {
            prev_link = ol_link;
            ol_link = ol_link->next_link;
        }
        if (!ol_link) {
            ol_alloc =  bcmi_nh_assoc_alloc_ol_link(ol_nh);
            if (ol_alloc == NULL) {
                return BCM_E_MEMORY;
            }
            prev_link->next_link = ol_alloc;
        }
    } else {

        ul_alloc = bcmi_nh_assoc_alloc_ul_link(ul_nh);
        if (ul_alloc == NULL) {
            return BCM_E_MEMORY;
        }
        ol_alloc = bcmi_nh_assoc_alloc_ol_link(ol_nh);
        if (ol_alloc == NULL) {
            sal_free(ul_alloc);
            return BCM_E_MEMORY;
        }

        if (prev_assoc) {
            prev_assoc->ul_nh_link = ul_alloc;
            prev_assoc->ul_nh_link->ol_nh_assoc = ol_alloc;
        } else {
            ul_nh_assoc_head[unit][bkt_idx] = ul_alloc;
            ul_nh_assoc_head[unit][bkt_idx]->ol_nh_assoc = ol_alloc;
        }
    }
    /* 
     * increase the ref count of second next hop so that it
     * not deleted in case someone tries to delete it. 
     */
    BCM_XGS3_L3_ENT_REF_CNT_INC 
        (BCM_XGS3_L3_TBL_PTR(unit, next_hop), ul_nh,
        _BCM_SINGLE_WIDE); 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_del_nh_nh_assoc 
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 *      nh_idx - next hop index
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int bcmi_l3_nh_assoc_ol_ul_link_delete(int unit, int nh_idx)
{
    soc_mem_t mem;                        /* Table location memory. */
    egr_l3_next_hop_entry_t  egr_entry;   /* Egress next hop entry. */
    int ent_type, next_ptr_type, next_nh_idx;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    mem = EGR_L3_NEXT_HOPm;
    sal_memset(&egr_entry, 0, sizeof(egr_l3_next_hop_entry_t));

    SOC_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, EGR_L3_NEXT_HOPm,
                                          nh_idx, &egr_entry));

    ent_type = soc_mem_field32_get(unit, mem, &egr_entry, ENTRY_TYPEf);

    /* chk if the entry is L3MC */
    if (ent_type == BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE) {
        next_ptr_type = soc_mem_field32_get
            (unit, mem, &egr_entry, L3MC__NEXT_PTR_TYPEf);

        if (next_ptr_type  == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
            next_nh_idx = soc_mem_field32_get(unit, mem, &egr_entry,
                              L3MC__NEXT_PTRf);

            BCM_IF_ERROR_RETURN(
                bcmi_l3_nh_assoc_ol_ul_link_sw_delete(unit, nh_idx, 
                next_nh_idx));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_sw_delete
 * Purpose:
 *      Deletes a overlay/underlay association.
 *      If there is only one underlay nh link, 
 *      then remove the overlay link also.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_sw_delete(int unit, int ol_nh, int ul_nh)
{
    ul_nh_link_t *ul_assoc, *prev_assoc;
    ol_nh_link_t *link_nh, *prev_link;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_nh_assoc_bkt_get(ul_nh);

    prev_assoc = ul_assoc = ul_nh_assoc_head[unit][bkt_idx];

    while (ul_assoc && (ul_assoc->u_nh_idx != ul_nh)) {
        prev_assoc = ul_assoc;
        ul_assoc = ul_assoc->ul_nh_link;
    }

    if (ul_assoc) {
        prev_link = link_nh = ul_assoc->ol_nh_assoc;
        while (link_nh && (link_nh->o_nh_idx != ol_nh)) {
            prev_link = link_nh;
            link_nh = link_nh->next_link;
        }

        if (link_nh) {
            if (link_nh == ul_assoc->ol_nh_assoc) {
                ul_assoc->ol_nh_assoc = link_nh->next_link;
            } else {
                prev_link->next_link = link_nh->next_link;
            }
            sal_free(link_nh);
        } else {
            /* send an error here */
            return BCM_E_NOT_FOUND;
        }
    }

    if (ul_assoc && (ul_assoc->ol_nh_assoc == NULL)) {
        if (ul_assoc == ul_nh_assoc_head[unit][bkt_idx]) {
            ul_nh_assoc_head[unit][bkt_idx] = ul_assoc->ul_nh_link;
        } else {
            prev_assoc->ul_nh_link = ul_assoc->ul_nh_link;
        }
        sal_free(ul_assoc);
    } else if (ul_assoc == NULL) {
        return BCM_E_NOT_FOUND;
    }
    /* 
     * decrease the ref count of second next hop so that it
     * not deleted in case someone tries to delete it. 
     */
    BCM_XGS3_L3_ENT_REF_CNT_DEC 
        (BCM_XGS3_L3_TBL_PTR(unit, next_hop), ul_nh,
        _BCM_SINGLE_WIDE); 

 return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_replace
 * Purpose:
 *      Replace overlay/underlay association.
 *
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_replace(int unit, int old_ul, int new_ul)
{   

    int entry_type, next_ptr_type;
    egr_l3_next_hop_entry_t egr_nh;
    ul_nh_link_t *ul_assoc;
    ol_nh_link_t *ol_link;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_nh_assoc_bkt_get(old_ul);

    if (ul_nh_assoc_head[unit][bkt_idx] == NULL) {
        /* There is nothing to replace */
        return BCM_E_NONE;
    }
    ul_assoc = ul_nh_assoc_head[unit][bkt_idx];
    while (ul_assoc && (ul_assoc->u_nh_idx != old_ul)) {
        ul_assoc = ul_assoc->ul_nh_link;
    }
    /* replace underlay nh index */
    if (ul_assoc) {
        ol_link = ul_assoc->ol_nh_assoc;
        while (ol_link) {

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                ol_link->o_nh_idx, &egr_nh));
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, ENTRY_TYPEf);

            if (entry_type == BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE) {
                next_ptr_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, L3MC__NEXT_PTR_TYPEf);
                if (next_ptr_type == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH) {

                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf, BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTRf, new_ul);

                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ALL, ol_link->o_nh_idx, &egr_nh));
                }
            }
        /* For all overlay NHs, replace the underlay NH. */
        bcmi_l3_nh_assoc_ol_ul_link_sw_delete(unit, ol_link->o_nh_idx, 
            old_ul);
        bcmi_l3_nh_assoc_ol_ul_link_sw_add(unit, ol_link->o_nh_idx, 
            new_ul);
        ol_link = ol_link->next_link;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_is_required 
 * Purpose:
 *      check if nh-nh association is needed.
 * Parameters:
 *      unit        - unit number
 *      vp nh index - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_is_required(int unit, int vp_nh_index)
{ 
    int entry_type = 0;
    egr_l3_next_hop_entry_t egr_nh;
    int action_present=0, action_not_present=0, vntag_actions = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                  vp_nh_index, &egr_nh));
    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);

    if (entry_type == BCMI_L3_EGR_NH_SD_TAG_ENTRY_TYPE) {
        /* check if sdtag egr nh has some tag actions */
        action_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
        vntag_actions =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__VNTAG_ACTIONSf);

        if (vntag_actions || action_present || action_not_present) {
            /* 
             * this next hop can do some modifications and therefore
             * we should set nh-nh association for this NH.
             */
            return 1;
        } else {
            return 0;
        }
    }

    return 1;
}

/*
 * Function:
 *      bcmi_l3_nh_assoc_ol_ul_link_reinit
 * Purpose:
 *      re-initializes overlay/underlay association.
 * Parameters:
 *      unit     - unit number
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_l3_nh_assoc_ol_ul_link_reinit(int unit, int ol_nh_idx)
{
    int entry_type, next_ptr_type, ul_nh_idx;
    egr_l3_next_hop_entry_t egr_nh;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, ol_nh_idx, &egr_nh));

    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);


    if (entry_type == BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE) { 
        next_ptr_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh, L3MC__NEXT_PTR_TYPEf);
        if (next_ptr_type == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
        
            ul_nh_idx =  soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, L3MC__NEXT_PTRf);

            BCM_IF_ERROR_RETURN(
                bcmi_l3_nh_assoc_ol_ul_link_sw_add(unit, ol_nh_idx, 
                ul_nh_idx));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_nh_destination_set
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */

int bcmi_l3_nh_dest_set(int unit, bcm_l3_egress_t *nh_entry, uint32 *nh_dest)
{
    int gport_type = 0, port = -1, gport = -1;

    gport = nh_entry->port;
    gport_type = BCMI_GPORT_TYPE_GET(gport);

    if (BCMI_RIOT_IS_ENABLED(unit)) {

        switch(gport_type) {
        case BCM_GPORT_VXLAN_PORT:
            port = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        case BCM_GPORT_L2GRE_PORT:
            port = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        case BCM_GPORT_MPLS_PORT:
            port = BCM_GPORT_MPLS_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        case BCM_GPORT_MIM_PORT:
            port = BCM_GPORT_MIM_PORT_ID_GET(gport);
            *nh_dest =  BCMI_L3_NH_DEST_DVP_GET(port);
        break;
        default:
            if (nh_entry->flags & BCM_L3_TGID) {
                int tid_is_vp_lag = 0;
                if (soc_feature(unit, soc_feature_vp_lag)) {
                    (void)_bcm_esw_trunk_id_is_vp_lag(unit, gport, &tid_is_vp_lag);
                }

                if (tid_is_vp_lag) {
                    int vp_lag_vp;
                    /* Get the VP value representing VP LAG */
                    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_tid_to_vp_lag_vp(unit,
                        gport, &vp_lag_vp));

                    *nh_dest = BCMI_L3_NH_DEST_DVP_GET(vp_lag_vp);
                } else {
                    *nh_dest = BCMI_L3_NH_DEST_LAG_GET(nh_entry->port);
                }
            } else {
                *nh_dest =  BCMI_L3_NH_DEST_DGLP_GET
                    (BCM_L3_DGLP_GET(nh_entry->module, nh_entry->port));
            }
        }
    } else {
        if (nh_entry->flags & BCM_L3_TGID) {
            *nh_dest = BCMI_L3_NH_DEST_LAG_GET(nh_entry->port);
        } else {
            *nh_dest =  BCMI_L3_NH_DEST_DGLP_GET
                (BCM_L3_DGLP_GET(nh_entry->module, nh_entry->port));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_get_gport_from_destination
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int bcmi_get_port_from_destination(int unit, uint32 dest,
                                        bcm_l3_egress_t *nh)
{
    int vp;

    if (BCMI_L3_NH_DEST_IS_LAG(dest)) {
        nh->port = BCMI_L3_NH_PORT_LAG_GET(dest);
        nh->flags |= BCM_L3_TGID;
    } else if (BCMI_L3_NH_DEST_IS_DGLP(dest)) {
        nh->port = BCMI_L3_NH_PORT_DGLP_GET(dest);
    } else if (BCMI_L3_NH_DEST_IS_DVP(dest)) {
        vp = BCMI_L3_NH_PORT_DVP_GET(dest);
        _bcm_vp_encode_gport(unit, vp, &(nh->port));
    } else {
        nh->port = dest;
    }

    return BCM_E_NONE;

}

#endif /* BCM_RIOT_SUPPORT */
