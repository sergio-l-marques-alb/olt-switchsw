/*
 * $Id: wb_db_gport.h,v 1.8 Broadcom SDK $
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
 * File:        wb_db_gport.h
 * Purpose:     WarmBoot - Level 2 support (GPORT Module)
 */

#ifndef _BCM_DPP_WB_DB_GPORT_H_
#define _BCM_DPP_WB_DB_GPORT_H_

#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <bcm_int/dpp/gport_mgmt.h>

#if defined(BCM_WARM_BOOT_SUPPORT)

#define _SHR_WB_HTB_OFFSET_ALIGNMENT               (1)

#define SHR_WB_HL_LOCK(unit, hl)               sal_mutex_take((hl)->lock, sal_mutex_FOREVER)
#define SHR_WB_HL_UNLOCK(unit, hl)             sal_mutex_give((hl)->lock)

#define SHR_WB_NULL_OFFSET                      (0xFFFFFFFF)
#define SHR_WB_MAX_ITER                         (0xFFFFF)

#define BCM_DPP_WB_GPORT_VERSION_1_0           SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_GPORT_CURRENT_VERSION       BCM_DPP_WB_GPORT_VERSION_1_0

#define BCM_DPP_WB_GPORT_INFO(unit)            (_dpp_wb_gport_info_p[unit])
#define BCM_DPP_WB_GPORT_INFO_INIT(unit)       ( (BCM_DPP_WB_GPORT_INFO(unit) != NULL) &&         \
                                                 (BCM_DPP_WB_GPORT_INFO(unit)->init_done == TRUE) )

#define BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                        \
                        SOC_CONTROL(unit)->scache_dirty = 1;                           \
                        BCM_DPP_WB_GPORT_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_GPORT_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                        \
                        BCM_DPP_WB_GPORT_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_GPORT_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_GPORT_INFO(unit)->is_dirty == 1)

#ifdef _SHR_WB_HTB_OFFSET_ALIGNMENT

#define SHR_WB_OFFSET_SET(addr, off, is_aligned)    _bcm_dpp_wb_store32(off, (uint8 *)addr);
#define SHR_WB_OFFSET_GET(addr, is_aligned)         _bcm_dpp_wb_load32((uint8 *)addr)

#else /* _SHR_WB_HTB_OFFSET_ALIGNMENT */

#define SHR_WB_OFFSET_SET(addr, off, is_aligned)    (*((shr_wb_htb_off_t *)(addr))) = off
#define SHR_WB_OFFSET_GET(addr, is_aligned)         (*((shr_wb_htb_off_t *)(addr)))

#endif /* !(_SHR_WB_HTB_OFFSET_ALIGNMENT) */


/* macros for converting cache memory pointers to cache offsets and vice versa */
#define SHR_WB_HTB_PTR2OFF(hl_info, ptr)            (((uint8 *)ptr) - hl_info->scache_ptr)
#define SHR_WB_HTB_OFF2PTR(hl_info, off)            (hl_info->scache_ptr + off)


#define SHR_WB_HTB_COMMIT_OFF(hl_info, off)         (hl_info->scache_off + off)

#define BCM_DPP_WB_GPORT_HL_DESC_INIT(hl_ds, _type, _size_hdr_list, _max_elements, _key_size,    \
                 _data_size, _scache_handle, _scache_ptr, _scache_off, _is_offset_aligned)       \
{                                                                                                \
        (hl_ds)->type = _type;                                                                   \
        (hl_ds)->size_hdr_list = _size_hdr_list;                                                 \
        (hl_ds)->max_elements = _max_elements;                                                   \
        (hl_ds)->key_size = _key_size;                                                           \
        (hl_ds)->data_size = _data_size;                                                         \
        (hl_ds)->scache_handle = _scache_handle;                                                 \
        (hl_ds)->scache_ptr = _scache_ptr;                                                       \
        (hl_ds)->scache_off = _scache_off;                                                       \
        (hl_ds)->is_offset_aligned = _is_offset_aligned;                                         \
}

/*
 * hash list for persistence storage also maintains the data space for element. elememnt
 * data space is allocated upfront and also contains the 'key' and 'offset' to next data
 * element.
 *
 * Hash list headers maintains the head 'offset' for the data elements. There is no
 * seperate offset for data. This has the potential of saving a data 'offset' field.
 * The free list header maintains the head 'offset' off the data elements (allocated
 * upfront). Data elements are allocated from this list and freed to this list.
 *
 * There is a compile time option for aligning the data element to a "offset" boundary.
 * This will make traversal of linked list fast. 'data' itself will be packed.
 *
 * Layout of data element is
 *     - "offset". indicates next data element
 *        aligned at natural boundary via compile time flag.
 *     - "key". size depends on the data type
 *     - "data". Actual data asociated with key.
 */

typedef enum shr_wb_hash_list_type_e {
    shr_wb_hl_port_info,
    shr_wb_hl_mpls_info,
    shr_wb_hl_trill_info,
    shr_wb_hl_mc_trill_info,
    shr_wb_hl_trill_route_info,
    shr_wb_hl_trill_src_info,
    shr_wb_hl_trill_vpn_info,
    shr_wb_hl_last
} shr_wb_hash_list_type_t;

typedef uint8*  shr_wb_htb_key_t;
typedef uint8*  shr_wb_htb_data_t;
typedef uint32  shr_wb_htb_off_t;
                  /* for fixed size data elements it coul dbe optimized to be "uint16". */
                  /* then instead of storing the offset it could be used to store the   */
                  /* element number. To align the key at natural boundary it may then   */
                  /* be necessary to have the "key" field before "next_offet" field for */
                  /* "shr_wb_hash_entry_t" structure.                                   */

typedef uint32 (*shr_wb_htb_hash_f)(uint8 *key_bytes, uint32 length);
typedef int    (*shr_wb_htb_key_cmp_f)(shr_wb_htb_key_t a, shr_wb_htb_key_t b, uint32 size);


typedef struct shr_wb_hash_entry_s {
    shr_wb_htb_off_t   next_offset;
    shr_wb_htb_key_t   key;   
    shr_wb_htb_data_t  data;
} shr_wb_hash_entry_t;

typedef struct shr_wb_hash_list_info_s {
    sal_mutex_t                      lock;
    shr_wb_hash_list_type_t          type;
    int                              size_hdr_list; /* should be power of 2 */
    int                              max_elements;
    int                              key_size;
    int                              data_size;
    int                              total_size;
    int                              element_size;

    soc_scache_handle_t              scache_handle;
    uint8                           *scache_ptr;
    uint32                           scache_off;

    int                              is_offset_aligned;

    shr_wb_htb_off_t                 free_list_off;
    shr_wb_htb_off_t                 hd_lists_off;

    shr_wb_htb_off_t                *free_list_off_p; /* shr_wb_hash_entry_t */
    shr_wb_htb_off_t                *hd_lists_off_p;   /* shr_wb_hash_entry_t */

    shr_wb_htb_hash_f                hash_f;
    shr_wb_htb_key_cmp_f             key_cmp_f;
} shr_wb_hash_list_info_t;

typedef struct shr_wb_hast_list_desc_s {
    shr_wb_hash_list_type_t          type;
    int                              size_hdr_list;
    int                              max_elements;
    int                              key_size;
    int                              data_size;

    soc_scache_handle_t              scache_handle;
    uint8                           *scache_ptr;
    uint32                           scache_off;

    int                              is_offset_aligned;
} shr_wb_hash_list_desc_t;


typedef struct shr_wb_hast_list_rinfo_s {
    int                              size;
    int                              element_size;

    shr_wb_htb_off_t                 free_list_off;
    shr_wb_htb_off_t                 hd_lists_off;
    shr_wb_htb_off_t                 data_off;
} shr_wb_hast_list_rinfo_t;

/*
 * runtime information
 */
typedef struct bcm_dpp_wb_gport_info_s {
    int                          init_done;

    int                          is_dirty;
    int                          is_offset_aligned;

    uint16                       version;
    soc_scache_handle_t          scache_handle;
    uint8                       *scache_ptr;
    int                          size;

    int                         inlif_match_off;
    shr_wb_hash_list_info_t     *port_info;
    shr_wb_hash_list_info_t     *trill_info;
    shr_wb_hash_list_info_t     *mc_trill_info;
    shr_wb_hash_list_info_t     *trill_route_info;
    shr_wb_hash_list_info_t     *trill_src_info;
    shr_wb_hash_list_info_t     *trill_vpn_info;

    uint32                       max_voqs;
    uint32                       voq_off;
} bcm_dpp_wb_gport_info_t;

typedef int (*shr_wb_cb_t)(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data);


#define BCM_DPP_WB_GPORT_HL_PORT_INFO_HL_SZ(unit)         (BCM_DPP_WB_GPORT_HL_PORT_INFO_NBR_ENTRIES(unit))
#define BCM_DPP_WB_GPORT_HL_PORT_INFO_NBR_ENTRIES(unit)   (_BCM_DPP_NOF_GPORTS(unit))
#define BCM_DPP_WB_GPORT_HL_PORT_INFO_KEY_SZ(unit)        (sizeof(bcm_gport_t))

typedef struct bcm_dpp_wb_gport_port_info_s {
    uint32 phy_gport;
    uint32 encap_id;
    uint8  type;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_port_info_t;




#define BCM_DPP_WB_GPORT_HL_TRILL_INFO_HL_SZ(unit)         (BCM_DPP_WB_GPORT_HL_TRILL_INFO_NBR_ENTRIES(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_INFO_NBR_ENTRIES(unit)   (_BCM_DPP_NOF_GPORTS(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_INFO_KEY_SZ(unit)        (sizeof(bcm_gport_t))

typedef struct bcm_dpp_wb_gport_trill_info_s {
    int type;
    int fec_id;
    int use_cnt;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_trill_info_t;


#define BCM_DPP_WB_GPORT_HL_MC_TRILL_HL_SZ(unit)         (BCM_DPP_WB_GPORT_HL_MC_TRILL_NBR_ENTRIES(unit))
#define BCM_DPP_WB_GPORT_HL_MC_TRILL_NBR_ENTRIES(unit)   (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids)
#define BCM_DPP_WB_GPORT_HL_MC_TRILL_KEY_SZ(unit)        (sizeof(bcm_multicast_t))

typedef struct bcm_dpp_wb_gport_mc_trill_s {
    bcm_gport_t gport;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_mc_trill_t;


#define BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_HL_SZ(unit)         (BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_NBR_ENTRIES(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_NBR_ENTRIES(unit)   (_BCM_DPP_NOF_GPORTS(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_KEY_SZ(unit)        (sizeof(bcm_gport_t))

/* The route info */
typedef struct bcm_dpp_wb_gport_trill_route_info_s {
    uint32 nof_routes;
    _bcm_petra_trill_mc_route_info_t route[_BCM_PETRA_TRILL_MC_ROOT_NOF_VLAN];
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_trill_route_info_t;



#define BCM_DPP_WB_GPORT_HL_TRILL_SRC_HL_SZ(unit)         (BCM_DPP_WB_GPORT_HL_TRILL_SRC_NBR_ENTRIES(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_SRC_NBR_ENTRIES(unit)   (_BCM_DPP_NOF_GPORTS(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_SRC_KEY_SZ(unit)        (sizeof(bcm_gport_t))

typedef struct bcm_dpp_wb_gport_trill_src_info_s {
    uint32  nof_src_rbridge;
    _bcm_petra_trill_mc_src_info_t src_rbridge[_BCM_PETRA_TRILL_MC_ROOT_NOF_SRC_RBRIDGE];
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_trill_src_info_t;

#define BCM_DPP_WB_GPORT_HL_TRILL_VPN_HL_SZ(unit)         (BCM_DPP_WB_GPORT_HL_TRILL_VPN_NBR_ENTRIES(unit))
#define BCM_DPP_WB_GPORT_HL_TRILL_VPN_NBR_ENTRIES(unit)   (_BCM_DPP_NUM_OF_TRILL_VSI)
#define BCM_DPP_WB_GPORT_HL_TRILL_VPN_KEY_SZ(unit)        (sizeof(SOC_PPD_SYS_VSI_ID))

typedef struct bcm_dpp_wb_gport_trill_vpn_info_s {
    uint32     flags; 
    bcm_vlan_t high_vid;                
    bcm_vlan_t low_vid;                 
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_trill_vpn_info_t;

#define BCM_DPP_WB_GPORT_HL_LIF_MATCH_NBR_ENTRIES(unit)   _BCM_GPORT_NOF_LIFS 

typedef struct bcm_dpp_wb_gport_lif_match_s {
    _bcm_dpp_inlif_match_info_t lif_match_info;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_gport_lif_match_t;


typedef enum bcm_dpp_wb_gport_oper_type_e {
    bcm_dpp_wb_gport_oper_add,
    bcm_dpp_wb_gport_oper_delete,
    bcm_dpp_wb_gport_oper_update,
    bcm_dpp_wb_gport_oper_save,
    bcm_dpp_wb_gport_oper_last
} bcm_dpp_wb_gport_oper_type_t;

extern bcm_dpp_wb_gport_info_t *_dpp_wb_gport_info_p[BCM_MAX_NUM_UNITS];

/*
 * Functions
 */
extern int
_bcm_dpp_wb_gport_state_init(int unit);

extern int
_bcm_dpp_wb_gport_state_deinit(int unit);

extern int
_bcm_dpp_wb_gport_sync(int unit);

extern int
_bcm_dpp_wb_gport_update_port_state(int unit, bcm_dpp_wb_gport_oper_type_t oper, shr_htb_key_t key, shr_htb_data_t data);

extern int
_bcm_dpp_wb_gport_update_trill_info_state(int unit, bcm_dpp_wb_gport_oper_type_t oper, shr_htb_key_t key, shr_htb_data_t data);

extern int
_bcm_dpp_wb_gport_update_mc_trill_state(int unit, bcm_dpp_wb_gport_oper_type_t oper, shr_htb_key_t key, shr_htb_data_t data);

extern int
_bcm_dpp_wb_gport_update_trill_vpn_state(int unit, bcm_dpp_wb_gport_oper_type_t oper, shr_htb_key_t key, shr_htb_data_t data);

extern int
_bcm_dpp_wb_gport_update_trill_route_state(int unit, bcm_dpp_wb_gport_oper_type_t oper, shr_htb_key_t key, shr_htb_data_t data);

extern int
_bcm_dpp_wb_gport_update_trill_src_state(int unit, bcm_dpp_wb_gport_oper_type_t oper, shr_htb_key_t key, shr_htb_data_t data);

extern int
_bcm_dpp_wb_gport_update_inlif_match_state(int unit, int lif, _bcm_dpp_inlif_match_info_t *match);

#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_COSQ_H_ */
