/*
 * $Id: field.h$
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
 * File:        field.h
 * Purpose:     Internal Field Processor data structure definitions for the
 *              BCM library.
 *
 */

#ifndef _BCM_INT_FIELD_H
#define _BCM_INT_FIELD_H

#include <bcm/field.h>
#include <bcm_int/common/field.h>

#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/er_tcam.h>

#include <sal/core/sync.h>

#ifdef BCM_FIELD_SUPPORT

/*
 *Macro:
 *     FP_LOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define FP_LOCK(control) \
    sal_mutex_take((control)->fc_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     FP_UNLOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define FP_UNLOCK(control) \
    sal_mutex_give((control)->fc_lock);

#define _FP_PORT_BITWIDTH(_unit_)    \
                  ((SOC_IS_TD_TT(_unit_) || SOC_IS_KATANA(_unit_)) ? 7 :    \
                   (SOC_IS_TOMAHAWK(_unit_) || SOC_IS_KATANA2(_unit_)) ? 8 : 6)

#define _FP_MCAST_D_TYPE_OFFSET(_unit_)  \
                 ((SOC_IS_TRIDENT2(unit) || SOC_IS_TRIUMPH3(unit)) ? (18) : \
                  ((SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit)) ? (16) :    \
                   (14)));


#define _FP_INVALID_INDEX            (-1)

/* 64 bit software counter collection bucket. */
#define _FP_64_COUNTER_BUCKET  512

/* Maximum number of paired slices.*/
#define _FP_PAIR_MAX   (3)

/* Macro to find the PBMP support in a multi-pipe  tcam slice mode */
#if defined(BCM_TOMAHAWK_SUPPORT)
    #define _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(_g_flags)     \
                      (((_g_flags & _FP_GROUP_SPAN_SINGLE_SLICE) && \
                        (_g_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) || \
                       (_g_flags & _FP_GROUP_SPAN_DOUBLE_SLICE) || \
                       (_g_flags & _FP_GROUP_SPAN_TRIPLE_SLICE))
#else
    #define _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(_g_flags)  0
#endif /*BCM_TOMAHAWK_SUPPORT */
/*
 * Meter mode.
 */
#define _BCM_FIELD_METER_BOTH (BCM_FIELD_METER_PEAK | \
                               BCM_FIELD_METER_COMMITTED)

/* Value for control->tcam_ext_numb that indicates there is no external TCAM. */
#define FP_EXT_TCAM_NONE _FP_INVALID_INDEX

/*
 * Initial group IDs and entry IDs.
 */
#define _FP_ID_BASE 1
#define _FP_ID_MAX  (0x1000000)


/*
 * Range of Hint Ids.
 */
#define _FP_HINT_ID_BASE         (0x1)
#define _FP_HINT_ID_MAX          (0x400)
#define _FP_MAX_HINTS_PER_HINTID (0x100)

/*
 * Action flags.
 */
/* Action valid. */
#define _FP_ACTION_VALID              (1 << 0)
/* Reinstall is required. */
#define _FP_ACTION_DIRTY              (1 << 1)
/* Remove action from hw. */
#define _FP_ACTION_RESOURCE_FREE      (1 << 2)
/* Action was updated free previously used hw resources. */
#define _FP_ACTION_OLD_RESOURCE_FREE  (1 << 3)

#define _FP_ACTION_MODIFY             (1 << 4)

#define _FP_ACTION_HW_FREE (_FP_ACTION_RESOURCE_FREE | \
                            _FP_ACTION_OLD_RESOURCE_FREE)

#define _FP_ACTION_POLICER_SW_UPDATE  (1 << 5)

/* Oam Stat Action flags */
#define _FP_OAM_STAT_ACTION_VALID    (1 << 0)
#define _FP_OAM_STAT_ACTION_DIRTY    (1 << 1)

/* Field policy table Cos queue priority hw modes */
#define _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE   0x1
#define _FP_ACTION_UCAST_QUEUE_CHANGE           0x2
#define _FP_ACTION_UCAST_QUEUE_NEW_MODE         0x8
#define _FP_ACTION_MCAST_QUEUE_NEW_MODE         0x9
/* Action set unicast queue value */
#define _FP_ACTION_UCAST_QUEUE_SET(q) ((q) & (0xf))
/* Action set multicast queue value */
#define _FP_ACTION_MCAST_QUEUE_SET(q) (((q) & 0x7) << 4)
/* Action get unicast queue value */
#define _FP_ACTION_UCAST_QUEUE_GET(q) ((q) & (0xf))
/* Action get multicast queue value */
#define _FP_ACTION_MCAST_QUEUE_GET(q) (((q) >> 4) & 0x7)
/* Action set both unicast and multicast queue value */
#define _FP_ACTION_UCAST_MCAST_QUEUE_SET(u,m) ((_FP_ACTION_UCAST_QUEUE_SET(u)) | \
                                                (_FP_ACTION_MCAST_QUEUE_SET(m)))

/*
 * Maximum number of meter pools
 *     Should change when new chips are defined
 */
#define _FIELD_MAX_METER_POOLS 16

#define _FIELD_MAX_CNTR_POOLS  16

/*
 * Internal version of qset add that will allow Data[01] to be set.
 */
#define BCM_FIELD_QSET_ADD_INTERNAL(qset, q)  SHR_BITSET(((qset).w), (q))
#define BCM_FIELD_QSET_REMOVE_INTERNAL(qset, q)  SHR_BITCLR(((qset).w), (q))
#define BCM_FIELD_QSET_TEST_INTERNAL(qset, q) SHR_BITGET(((qset).w), (q))

/* User-Defined Field (UDF) IP Flags. */
#define _BCM_FIELD_USER_THREE_MPLS_LABELS   (18 << 6)
#define _BCM_FIELD_USER_FOUR_MPLS_LABELS    (19 << 6)
#define _BCM_FIELD_USER_FIVE_MPLS_LABELS    (20 << 6)

/*
 * Typedef:
 *     _field_sel_t
 * Purpose:
 */
typedef struct _field_sel_s {
    int8    fpf0;              /* FPF0 field(s) select.              */
    int8    fpf1;              /* FPF1 field(s) select.              */
    int8    fpf2;              /* FPF2 field(s) select.              */
    int8    fpf3;              /* FPF3 field(s) select.              */
    int8    fpf4;              /* FPF4 field(s) selector.            */
    int8    extn;              /* field(s) select external.          */
    int8    src_class_sel;     /* Source lookup class selection.     */
    int8    dst_class_sel;     /* Destination lookup class selection.*/
    int8    intf_class_sel;    /* Interface class selection.         */
    int8    ingress_entity_sel;/* Ingress entity selection.          */
    int8    src_entity_sel;    /* Src port/trunk entity selection.          */
    int8    dst_fwd_entity_sel;/* Destination forwarding  entity.    */
    int8    fwd_field_sel;     /* Forwarding field type select.      */
    int8    loopback_type_sel; /* Loopback/Tunnel type selection.    */
    int8    ip_header_sel;     /* Inner/Outer Ip header selection.   */
    int8    ip6_addr_sel;      /* Ip6 address format selection.      */
    int8    intraslice;        /* Intraslice double wide selection.  */
    int8    secondary;
    int8    inner_vlan_overlay;
    int8    intraslice_vfp_sel;
    int8    aux_tag_1_sel;
    int8    aux_tag_2_sel;
    int8    oam_overlay_sel;   /* Oam Overlay Selection*/
    int8    normalize_ip_sel;
    int8    normalize_mac_sel;
    int8    egr_class_f1_sel;   /* EFP KEY1 Class ID Selector */
    int8    egr_class_f2_sel;   /* EFP KEY2 Class ID Selector */
    int8    egr_class_f3_sel;   /* EFP KEY3 Class ID Selector */
    int8    egr_class_f4_sel;   /* EFP KEY4 Class ID Selector */
    int8    egr_class_f6_sel;   /* EFP KEY6 Class ID Selector */
    int8    egr_class_f7_sel;   /* EFP KEY7 Class ID Selector */
    int8    egr_class_f8_sel;   /* EFP KEY8 Class ID Selector */
    int8    src_dest_class_f1_sel;
    int8    src_dest_class_f3_sel;
    int8    src_type_sel;
    int8    egr_key4_dvp_sel;  /* EFP KEY4 DVP Secondary selector */
    int8    egr_key8_dvp_sel;  /* EFP KEY8 DVP Secondary selector */
    int8    egr_key4_mdl_sel;  /* EFP KEY4 MDL Secondary selector */
    int8    egr_oam_overlay_sel; /* EFP Oam Overlay Selection*/
    int8    oam_intf_class_sel; /* Oam SVP Intf Class Selector */
} _field_sel_t;


/*
 * Count number of bits in a memory field.
 */
#define _FIELD_MEM_FIELD_LENGTH(_u_, _mem_, _fld_) \
             (SOC_MEM_FIELD_VALID((_u_), (_mem_), (_fld_)) ?  \
              (soc_mem_field_length((_u_), (_mem_), (_fld_))) : 0)


/*
 * Field select code macros
 *
 * These are for resolving if the select code has meaning or is really a don't
 * care.
 */

#define _FP_SELCODE_DONT_CARE (-1)
#define _FP_SELCODE_DONT_USE  (-2)

#define _FP_SELCODE_INVALIDATE(selcode) \
     ((selcode) = _FP_SELCODE_DONT_CARE);

#define _FP_SELCODE_IS_VALID(selcode) \
     (((selcode) == _FP_SELCODE_DONT_CARE || \
       (selcode) == _FP_SELCODE_DONT_USE) ? 0 : 1)

#define _FP_SELCODE_IS_INVALID(selcode) \
     (0 == _FP_SELCODE_IS_VALID(selcode))

#define _FP_MAX_FPF_BREAK(_selcode, _stage_fc, _fpf_id) \
      if (_FP_SELCODE_IS_VALID(_selcode))  {           \
         if (NULL == (_stage_fc->_field_table_fpf ## _fpf_id[_selcode])) { \
             break; \
         } \
      }

#define _FP_SELCODE_SET(_selcode_qset, _selcode) \
    sal_memset((_selcode_qset), (int8)_selcode, sizeof(_field_sel_t));


/*
 * Macros for packing and unpacking L3 information for field actions
 * into and out of a uint32 value.
 */

#define MAX_CNT_BITS 10

#define _FP_L3_ACTION_PACK_ECMP(ecmp_ptr, ecmp_cnt) \
    ( 0x80000000 | ((ecmp_ptr) << MAX_CNT_BITS) | (ecmp_cnt) )
#define _FP_L3_ACTION_UNPACK_ECMP_MODE(cookie) \
    ( (cookie) >> 31 )
#define _FP_L3_ACTION_UNPACK_ECMP_PTR(cookie) \
    ( ((cookie) & 0x7fffffff) >> MAX_CNT_BITS )
#define _FP_L3_ACTION_UNPACK_ECMP_CNT(cookie) \
    ((cookie) & ~(0xffffffff << MAX_CNT_BITS))

#define _FP_L3_ACTION_PACK_NEXT_HOP(next_hop) \
    ( (next_hop) & 0x7fffffff )
#define _FP_L3_ACTION_UNPACK_NEXT_HOP(cookie) \
    ( (cookie) & 0x7fffffff )


#define _FP_MAX_ENTRY_WIDTH   4 /* Maximum field entry width. */

/* Both for EFP and External Stage, TCAM Key Generation logic
 * is packet dependent. In EFP, based on the TCAM Slice_Mode and incoming
 * packet type, TCAM Lookup Key is generated.
 * Each entry in EFP TCAM can match on a KEY_MATCH_TYPE field
 * (4 Most Significant Bits) + actual KEY fields from the packet
 * + Pipeline Meta data.
 * KEY_MATCH_TYPE is an encoded value generated by the EFP key generation
 * logic that can be applied to all entries in EFP TCAM to guarantee that
 * each entry matches only on the desired Packet type/s."
 */
#define _FP_MAX_ENTRY_TYPES 2


#define _FP_ENTRY_TYPE_0 0  /* Entries which use Default Slice mode and
                             * KEY_MATCH_TYPE (only EFP)selected by Group
                             * Create logic.
                             */

#define _FP_ENTRY_TYPE_1 1 /* Entries in which alternate KEY_MATCH_TYPE is used
                            * based on the qualifiers applied to the entry.
                            * Typically used to match Not L3 and Not FCoE
                            * Packets.
                            */

#define _FP_ENTRY_TYPE_DEFAULT _FP_ENTRY_TYPE_0

/*  Macros for packing and unpacking Eight Bytes Payload qualifier information */
#define _FP_PACK_L2_EIGHT_BYTES_PAYLOAD(qual_in_1, qual_in_2, qual_out) do {\
        (qual_out)[1] = (uint32) (qual_in_1); \
        (qual_out)[0] = (uint32) (qual_in_2); \
    } while (0)

#define _FP_UNPACK_L2_EIGHT_BYTES_PAYLOAD(qual_in, qual_out_1, qual_out_2) do {\
        (*qual_out_1) = (uint32) ((qual_in)[1]); \
        (*qual_out_2) = (uint32) ((qual_in)[0]); \
    } while (0)

/*
 * Macro: _BCM_FIELD_ENTRIES_IN_SLICE
 *
 * Purpose:
 *     Returns number of entries in a slice, depending on mode
 *
 * Parameters:
 *     fg - pointer to entry's field group struct
 *     fs - pointer to entry's field slice struct
 *     result - return value
 */
#define _BCM_FIELD_ENTRIES_IN_SLICE(_fg_, _fs_, _result_)     \
    if ((_fg_)->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {    \
        (_result_) = ((_fs_)->entry_count >> 1);              \
    } else {                                                  \
        (_result_) = (_fs_)->entry_count;                     \
    }

/*
 * Macro:_FP_QUAL_INFO_SET
 *
 * Purpose:
 *    Set the fields of a _qual_info_t struct.
 */
#define _FP_QUAL_INFO_SET(qual, field, off, wid, fq)  \
    (fq)->qid    = (qual);                                    \
    (fq)->fpf    = (field);                                   \
    (fq)->offset = (off);                                     \
    (fq)->width  = (wid);                                     \
    (fq)->next   = NULL;
/*
 * Macro:
 *     _FIELD_SELCODE_CLEAR (internal)
 * Purpose:
 *     Set all fields of a field select code to invalid
 * Parameters:
 *     selcode - _field_selcode_t struct
 */

#define _FIELD_SELCODE_CLEAR(_selcode_)      \
                _FP_SELCODE_SET(&(_selcode_), _FP_SELCODE_DONT_CARE)

/*
 * Macro:
 *     _FIELD_NEED_I_WRITE
 * Purpose:
 *     Test if it is necessary to write to "I" version of FP_PORT_FIELD_SEL
 *     table.
 * Parameters:
 *     _u_ - BCM device number
 *     _p_ - port
 *     _m_ - Memory (usually IFP_PORT_FIELD_SEL)
 */
#define _FIELD_NEED_I_WRITE(_u_, _p_, _m_) \
     ((SOC_MEM_IS_VALID((_u_), (_m_))) && \
      (IS_XE_PORT((_u_), (_p_)) || IS_ST_PORT((_u_),(_p_)) || \
       IS_CPU_PORT((_u_),(_p_))))


#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)

/* Egress FP slice modes. */
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2      (0x0)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3      (0x1)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE         (0x2)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY  (0x3)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY  (0x4)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6   (0x5)


/* Egress FP slice v6 key  modes. */
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6         (0x0)
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6         (0x1)
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64   (0x2)



/* Egress FP possible keys. */
#define  _BCM_FIELD_EFP_KEY1                    (0x0)  /* IPv4 key.      */
#define  _BCM_FIELD_EFP_KEY2                    (0x1)  /* IPv6 key.      */
#define  _BCM_FIELD_EFP_KEY3                    (0x2)  /* IPv6 dw key.   */
#define  _BCM_FIELD_EFP_KEY4                    (0x3)  /* L2 key.        */
#define  _BCM_FIELD_EFP_KEY5                    (0x4)  /* Ip Any key.    */
#define  _BCM_FIELD_EFP_KEY6                    (0x5)  /* HiGiG Key.     */
#define  _BCM_FIELD_EFP_KEY7                    (0x6)  /* Loopback Key.  */
#define  _BCM_FIELD_EFP_KEY8                    (0x7)  /* Bytes after
                                                          L2 Header key. */

/* KEY_MATCH type field for EFP_TCAM encoding. for every key. */
#define  _BCM_FIELD_EFP_KEY1_MATCH_TYPE       (0x1)
#define  _BCM_FIELD_EFP_KEY2_MATCH_TYPE       (0x2)
#define  _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE  (0x3)
#define  _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE  (0x4)
#define  _BCM_FIELD_EFP_KEY4_MATCH_TYPE       (0x5)
#define  _BCM_FIELD_EFP_KEY2_KEY4_MATCH_TYPE  (0x6)
#define  _EFP_SLICE_MODE_L2_SINGLE_WIDE       (0)
#define  _EFP_SLICE_MODE_L3_SINGLE_WIDE       (0x1)
#define  _EFP_SLICE_MODE_L3_DOUBLE_WIDE       (0x2)
#define  _EFP_SLICE_MODE_L3_ANY_SINGLE_WIDE   (0x3)
#define  _EFP_SLICE_MODE_L3_ANY_DOUBLE_WIDE   (0x4)
#define  _EFP_SLICE_MODE_L3_ALT_DOUBLE_WIDE   (0x5)


/* Key Match Type Values */
#define KEY_TYPE_INVALID            (0x0)
#define KEY_TYPE_IPv4_SINGLE        (0x1)
#define KEY_TYPE_IPv6_SINGLE        (0x2)
#define KEY_TYPE_IPv6_DOUBLE        (0x3)
#define KEY_TYPE_IPv4_L2_L3_DOUBLE  (0x4)
#define KEY_TYPE_L2_SINGLE          (0x5)
#define KEY_TYPE_IPv4_IPv6_DOUBLE   (0x6)
#define KEY_TYPE_HIGIG_DOUBLE       (0x9)
#define KEY_TYPE_LOOPBACK_DOUBLE    (0xa)
#define KEY_TYPE_L2_DOUBLE          (0xb)

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#define FPF_SZ_MAX                           (32)

#define _FP_HASH_SZ(_fc_)   \
           (((_fc_)->flags & _FP_EXTERNAL_PRESENT) ? (0x1000) : (0x100))
#define _FP_HASH_INDEX_MASK(_fc_) (_FP_HASH_SZ(_fc_) - 1)

#define _FP_HASH_INSERT(_hash_ptr_, _inserted_ptr_, _index_)    \
           do {                                                 \
              (_inserted_ptr_)->next = (_hash_ptr_)[(_index_)]; \
              (_hash_ptr_)[(_index_)] = (_inserted_ptr_);       \
           } while (0)

#define _FP_HASH_REMOVE(_hash_ptr_, _entry_type_, _removed_ptr_, _index_)  \
           do {                                                    \
               _entry_type_ *_prev_ent_ = NULL;                    \
               _entry_type_ *_lkup_ent_ = (_hash_ptr_)[(_index_)]; \
               while (NULL != _lkup_ent_) {                        \
                   if (_lkup_ent_ != (_removed_ptr_))  {           \
                       _prev_ent_ = _lkup_ent_;                    \
                       _lkup_ent_ = _lkup_ent_->next;              \
                       continue;                                   \
                   }                                               \
                   if (_prev_ent_!= NULL) {                        \
                       _prev_ent_->next = (_removed_ptr_)->next;   \
                   } else {                                        \
                       (_hash_ptr_)[(_index_)] = (_removed_ptr_)->next; \
                   }                                               \
                   break;                                          \
               }                                                   \
           } while (0)

#define _FP_HINTS_LINKLIST_INSERT(_head_, _node_)     \
    do {                                              \
        if (*_head_ != NULL) {                        \
            (_node_)->next  = (*_head_);        \
        }                                             \
        (*_head_) = (_node_);                         \
    }while (0)

#define _FP_HINTS_LINKLIST_DELETE(_head_, _entry_type_, _node_)        \
    do {                                                               \
        _entry_type_ *prev = NULL;                                     \
        _entry_type_ *curr = NULL;                                     \
        for (prev = curr = *_head_; curr != NULL; curr = curr->next) { \
            if (curr == _node_) {                                      \
                prev->next = curr->next;                               \
                if (curr == *_head_) {                                 \
                    *_head_ = (*_head_)->next;                         \
                }                                                      \
                if (curr->hint != NULL) {                              \
                    sal_free (curr->hint);                             \
                    curr->hint = NULL;                                 \
                }                                                      \
                sal_free (curr);                                       \
                curr = NULL;                                           \
                break;                                                 \
            }                                                          \
            prev = curr;                                               \
        }                                                              \
    }while (0)

#define _FP_HINTS_LINKLIST_DELETE_ALL(_head_, _entry_type_)         \
    do {                                                            \
        _entry_type_ *node = NULL;                                  \
        while (*_head_ != NULL) {                                   \
            node = *_head_;                                         \
            *_head_ = (*_head_)->next;                              \
            if (node->hint != NULL) {                               \
                sal_free (node->hint);                              \
                node->hint = NULL;                                  \
            }                                                       \
            sal_free (node);                                        \
            node = NULL;                                            \
        }                                                           \
    }while (0)

#define _FP_LINKLIST_INSERT(_head_, _node_, _rc_)                   \
    do {                                                            \
        if ((_head_ == NULL) || (_node_ == NULL)) {                 \
            *_rc_ = BCM_E_PARAM;                                    \
        } else {                                                    \
            if (*_head_ != NULL) {                                  \
                (_node_)->next  = (*_head_);                        \
            }                                                       \
            (*_head_) = (_node_);                                   \
            *_rc_ = BCM_E_NONE;                                     \
        }                                                           \
    }while (0)

#define _FP_ACTION_IS_COLOR_BASED(action)               \
     ((bcmFieldActionRpDrop <= (action) &&              \
      (action) <= bcmFieldActionYpPrioIntCancel) ||  \
      (bcmFieldActionGpDrop <= (action)                 \
       && (action) <= bcmFieldActionGpPrioIntCancel))

/* Different types of packet types for External TCAM */
#define _FP_EXT_NUM_PKT_TYPES 3
#define _FP_EXT_L2   0
#define _FP_EXT_IP4  1
#define _FP_EXT_IP6  2

/*
 * Different types of Databases in External TCAM (Triumph)
 * Each type of database is considered as a slice (in S/W)
 * The number of entries in each type of database is configurable
 * The field_slice_t.num_ext_entries denotes the number of entries
 *
 * See include/soc/er_tcam.h for the database types:
 * slice_numb    database type (soc_tcam_partition_type_t)   meaning
 *     0         TCAM_PARTITION_ACL_L2C                      ACL_144_L2
 *     1         TCAM_PARTITION_ACL_L2                       ACL_L2
 *     2         TCAM_PARTITION_ACL_IP4C                     ACL_144_IPV4
 *     3         TCAM_PARTITION_ACL_IP4                      ACL_IPV4
 *     4         TCAM_PARTITION_ACL_L2IP4                    ACL_L2_IPV4
 *     5         TCAM_PARTITION_ACL_IP6C                     ACL_144_IPV6
 *     6         TCAM_PARTITION_ACL_IP6S                     ACL_IPV6_SHORT
 *     7         TCAM_PARTITION_ACL_IP6F                     ACL_IPV6_FULL
 *     8         TCAM_PARTITION_ACL_L2IP6                    ACL_L2_IPV6
 */
#define _FP_EXT_ACL_144_L2     0
#define _FP_EXT_ACL_L2         1
#define _FP_EXT_ACL_144_IPV4   2
#define _FP_EXT_ACL_IPV4       3
#define _FP_EXT_ACL_L2_IPV4    4
#define _FP_EXT_ACL_144_IPV6   5
#define _FP_EXT_ACL_IPV6_SHORT 6
#define _FP_EXT_ACL_IPV6_FULL  7
#define _FP_EXT_ACL_L2_IPV6    8
#define _FP_EXT_NUM_PARTITIONS  9

extern soc_tcam_partition_type_t _bcm_field_fp_tcam_partitions[];
extern soc_mem_t _bcm_field_ext_data_mask_mems[];
extern soc_mem_t _bcm_field_ext_data_mems[];
extern soc_mem_t _bcm_field_ext_mask_mems[];
extern soc_mem_t _bcm_field_ext_policy_mems[];
extern soc_mem_t _bcm_field_ext_counter_mems[];


/*
 * Structure for Group auto-expansion feature
 *     S/W copy of FP_SLICE_MAP
 */

typedef struct _field_virtual_map_s {
    int valid;           /* Map is valid flag.                           */
    int vmap_key;        /* Key = Physical slice number + stage key base */
    int virtual_group;   /* Virtual group id.                            */
    int priority;        /* Virtual group priority.                      */
    int flags;           /* Field group flags.                           */
} _field_virtual_map_t;

/* Number of entries in a single map.*/
#define _FP_VMAP_SIZE       (17)
/* Default virtual map id.           */
#define _FP_VMAP_DEFAULT    (_FP_EXT_L2)
/* Number of virtual maps per stage.*/
#define _FP_VMAP_CNT        (_FP_EXT_NUM_PKT_TYPES)

/* Encode physical slice into vmap key. */
#define _FP_VMAP_IDX_TO_KEY(_slice_, _stage_)                          \
    (((_stage_) == _BCM_FIELD_STAGE_EXTERNAL) ?                        \
     ((_FP_VMAP_SIZE_) - 1 ) :  (_slice_))

/*
 * Typedef:
 *     _bcm_field_device_sel_t
 * Purpose:
 *     Enumerate per device controls used for qualifier selection.
 */
typedef enum _bcm_field_device_sel_e {
    _bcmFieldDevSelDisable,          /* Don't care selection.            */
    _bcmFieldDevSelInnerVlanOverlay, /* Inner vlan overlay enable.       */
    _bcmFieldDevSelIntrasliceVfpKey, /* Intraslice vfp key select.       */
    _bcmFieldDevSelCount             /* Always Last. Not a usable value. */
} _bcm_field_device_sel_t;

/*
 * Typedef:
 *     _bcm_field_fwd_entity_sel_t
 * Purpose:
 *     Enumerate Forwarding Entity Type selection.
 */
typedef enum _bcm_field_fwd_entity_sel_e {
    _bcmFieldFwdEntityAny,           /* Don't care selection.            */
    _bcmFieldFwdEntityMplsGport,     /* MPLS gport selection.            */
    _bcmFieldFwdEntityMimGport,      /* Mim gport selection.             */
    _bcmFieldFwdEntityWlanGport,     /* Wlan gport selection.            */
    _bcmFieldFwdEntityL3Egress,      /* Next hop selection.              */
    _bcmFieldFwdEntityGlp,           /* (MOD/PORT/TRUNK) selection.      */
    _bcmFieldFwdEntityModPortGport,  /* MOD/PORT even if port is part of
                                        the trunk.                       */
    _bcmFieldFwdEntityMulticastGroup,/* Mcast group id selection.        */
    _bcmFieldFwdEntityMultipath,     /* Multpath id selection.           */
    _bcmFieldFwdEntityPortGroupNum,  /* Ingress port group and number */
    _bcmFieldFwdEntityCommonGport,   /* Mod/Port gport or MPLS/MiM/
                                        Wlan/Niv gport selection.        */
    _bcmFieldFwdEntityVxlanGport,    /* vxlan gport selection.           */
    _bcmFieldFwdEntityNivGport,      /* Niv gport selection.            */
    _bcmFieldFwdEntityVlanGport,      /* Vlan gport selection.            */
    _bcmFieldFwdEntityCount          /* Always Last. Not a usable value. */
} _bcm_field_fwd_entity_sel_t;


/*
 * Typedef:
 *     _bcm_field_oam_intf_class_sel_t
 * Purpose:
 *     Enumerate Oam Class Id from SVP table selection.
 */
typedef enum _bcm_field_oam_intf_class_sel_e {
    _bcmFieldOamInterfaceClassSelectAny,
    _bcmFieldOamInterfaceClassSelectNiv,
    _bcmFieldOamInterfaceClassSelectMim,
    _bcmFieldOamInterfaceClassSelectVxlan,
    _bcmFieldOamInterfaceClassSelectVlan,
    _bcmFieldOamInterfaceClassSelectMpls,
    _bcmFieldOamInterfaceClassSelectCount
} _bcm_field_oam_intf_class_sel_t;

/*
 * Typedef:
 *     _bcm_field_fwd_field_sel_t
 * Purpose:
 *     Enumerate Forwarding Field Type selection.
 */
typedef enum _bcm_field_fwd_field_sel_e {
    _bcmFieldFwdFieldAny,           /* Don't care selection.            */
    _bcmFieldFwdFieldVrf,           /* L3 Vrf id selection.             */
    _bcmFieldFwdFieldVpn,           /* VPLS/VPWS VFI selection.         */
    _bcmFieldFwdFieldVlan,          /* Forwarding vlan selection.       */
    _bcmFieldFwdFieldCount          /* Always Last. Not a usable value. */
} _bcm_field_fwd_field_sel_t;

/*
 * Typedef:
 *     _bcm_field_aux_tag_sel_t
 * Purpose:
 *     Enumerate Auxiliary Tag Type selection.
 */
typedef enum _bcm_field_aux_tag_sel_e {
    _bcmFieldAuxTagAny = 0,         /* Don't care selection.            */
    _bcmFieldAuxTagVn = 1,          /* VN Tag selection.                */
    _bcmFieldAuxTagCn = 2,          /* CN Tag selection.                */
    _bcmFieldAuxTagFabricQueue = 3, /* Fabric Queue Tag selection.      */
    _bcmFieldAuxTagMplsFwdingLabel = 4,
    _bcmFieldAuxTagMplsCntlWord = 5,
    _bcmFieldAuxTagRtag7A = 6,
    _bcmFieldAuxTagRtag7B = 7,
    _bcmFieldAuxTagVxlanOrLLTag = 8, /* Vxlan Flags,24 bit VNI/
                                       16 bit Logical Link ID */
    _bcmFieldAuxTagVxlanReserved = 9, /* Vxlan Header Reserved Field */
    _bcmFieldAuxTagCount = 10        /* Always Last. Not a usable value. */
} _bcm_field_aux_tag_sel_t;

/*
 * Typedef:
 *    _bcm_field_nat_oam_sel_t
 * Purpose:
 *     Enumerate Auxiliary Tag Type selection.
 */
typedef enum _bcm_field_nat_oam_sel_e {
    _bcmFieldNatOamInvalid = 0,   /* NAT & OAM Invalid */
    _bcmFieldNatOamValidNat = 1,  /* NAT is valid */
    _bcmFieldNatOamValidOam = 2,  /* OAM is Valid */
    _bcmFieldNatOamReserved = 3,  /* Reserved Field */
    _bcmFieldNatOamCount = 4     /* Always Last. Not a usable value. */
} _bcm_field_nat_oam_sel_t;

/*
 * Typedef:
 *     _bcm_field_oam_overlay_sel_t
 * Purpose:
 *     Enumerate Oam Overlay Enable selector value.
 */
typedef enum _bcm_field_oam_overlay_sel_e {
    _bcmFieldOamOverlayDisable = 0, /* Oam Overlay Disable */
    _bcmFieldOamOverlayEnable  = 1, /* Oam Overlay Enable */
    _bcmFieldOamOverlayCount   = 2  /* Always Last. Not a usable value. */
} _bcm_field_oam_overlay_sel_t;


/*
 * Typedef:
 *     _bcm_field_slice_sel_t
 * Purpose:
 *     Enumerate per slice controls used for qualifier selection.
 */
typedef enum _bcm_field_slice_sel_e {
    _bcmFieldSliceSelDisable,        /* Don't care selection.             */
    _bcmFieldSliceSelFpf1,           /* Field selector 1.                 */
    _bcmFieldSliceSelFpf2,           /* Field selector 2.                 */
    _bcmFieldSliceSelFpf3,           /* Field selector 3.                 */
    _bcmFieldSliceSelFpf4,           /* Field selector 3.                 */
    _bcmFieldSliceSelExternal,       /* Field selector External key.      */
    _bcmFieldSliceSrcClassSelect,    /* Field source class selector.      */
    _bcmFieldSliceDstClassSelect,    /* Field destination class selector. */
    _bcmFieldSliceIntfClassSelect,   /* Field interface class selector.   */
    _bcmFieldSliceIpHeaderSelect,    /* Inner/Outer Ip header selector.   */
    _bcmFieldSliceLoopbackTypeSelect,/* Loopback vs Tunnel type selector. */
    _bcmFieldSliceIngressEntitySelect,/* Ingress entity type selector.    */
    _bcmFieldSliceSrcEntitySelect,   /* Src entity type selector.    */
    _bcmFieldSliceDstFwdEntitySelect, /* Destination entity type selector. */
    _bcmFieldSliceIp6AddrSelect,      /* Sip/Dip/Dip64_Sip64 selector.     */
    _bcmFieldSliceFwdFieldSelect,     /* Vrf/Vfi/Forwarding vlan selector. */
    _bcmFieldSliceAuxTag1Select,      /* VN/CN/FabricQueue tag selector 1. */
    _bcmFieldSliceAuxTag2Select,      /* VN/CN/FabricQueue tag selector 2. */
    _bcmFieldSliceOamOverLayEnable,   /* OAM Overlay enable selector on TD2+*/
    _bcmFieldSliceFpf1SrcDstClassSelect,
    _bcmFieldSliceFpf3SrcDstClassSelect,
    _bcmFieldSliceSelEgrClassF1,
    _bcmFieldSliceSelEgrClassF2,
    _bcmFieldSliceSelEgrClassF3,
    _bcmFieldSliceSelEgrClassF4,
    _bcmFieldSliceSrcTypeSelect,
    _bcmFieldSliceSelEgrDvpKey4,      /* Secondary Selector for 
                                       * EFP_KEY4_DVP_SELECTORr 
                                       */
    _bcmFieldSliceSelEgrMdlKey4,      /* Secondary Selector for 
                                       * EFP_KEY4_MDL_SELECTORr 
                                       */

    _bcmFieldSliceSelEgrOamOverlayKey4, /* Egress OAM Overlay enable selector
                                          on TD2+ */
    _bcmFieldSliceSelEgrClassF6, /* Select CLASS ID from IFP/L3 next Hop/
                                    L3 intf/ Egress DVP for EFP_KEY6 */
    _bcmFieldSliceSelEgrClassF7, /* Select CLASS ID from IFP/L3 next Hop/
                                    L3 intf/ Egress DVP for EFP_KEY7 */
    _bcmFieldSliceSelEgrClassF8, /* Select IFP Class ID/Egress Port/CPU COS 
                                  *  for EFP_KEY8 
                                  */
    _bcmFieldSliceSelEgrDvpKey8, /* Secondary Selector for 
                                  * EFP_KEY8_DVP_SELECTORr 
                                  */
    _bcmFieldSliceOamInterfaceClassSelect, /* OAM SVP class selector on TD2+ */
    _bcmFieldSliceCount               /* Always Last. Not a usable value.  */
} _bcm_field_slice_sel_t;

/*
 * Typedef:
 *     _bcm_field_selector_t
 * Purpose:
 *     Select codes configuration required for qualifer.
 */
typedef struct _bcm_field_selector_s {
    _bcm_field_device_sel_t   dev_sel;         /* Per device Selector.     */
    uint8                     dev_sel_val;     /* Device selector value.   */
    _bcm_field_slice_sel_t    pri_sel;         /* Primary slice selector.  */
    uint8                     pri_sel_val;     /* Primary selector value.  */
    _bcm_field_slice_sel_t    sec_sel;         /* Secondary slice selector.*/
    uint8                     sec_sel_val;     /* Secondary selector value.*/
    _bcm_field_slice_sel_t    ter_sel;         /* Tertiary slice selector. */
    uint8                     ter_sel_val;     /* Tertiary selector value. */
    uint8                     intraslice;      /* Intra-slice double wide. */
    uint8                     secondary;       /* Slice pairing overlay.   */
    uint8                     primary;         /* Primary slice pairing
                                                  overlay.   */
    uint8                     update_count;    /* Number of selcodes updates
                                                  required for selection.*/
} _bcm_field_selector_t;

/* Max Qualifiers Offsets */
#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Current Max offsets value is set as 64.
 * In TH qualifiers can be split across TCAM slices in wide mode.
 * Per-Slice upto 32 extractors are supported in HW.
 */ 
#define _BCM_FIELD_QUAL_OFFSET_MAX  64
#else
#define _BCM_FIELD_QUAL_OFFSET_MAX  3
#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Field Qualifier Type
 */
typedef enum _bcm_field_qual_type_e {
     _bcmFieldQualifierTypeDefault = 0,
     _bcmFieldQualifierTypePresel  = 1,
     _bcmFieldQualifierTypeCount   = 2
} _bcm_field_qual_type_t;


/*
 * Typedef:
 *     _bcm_field_qual_offset_t
 * Purpose:
 *     Qualifier offsets in FP tcam.
 */
typedef struct _bcm_field_qual_offset_s {
    soc_field_t field;                              /* Field name.            */
    uint8       num_offsets;                        /* Number of Offsets.     */
    uint16      offset[_BCM_FIELD_QUAL_OFFSET_MAX]; /* Qualifier offset.      */
    uint8       width[_BCM_FIELD_QUAL_OFFSET_MAX];  /* Qualifier offset width.*/
    uint8       secondary;
    uint8       bit_pos;                            /* Qualifier Bit Position.*/
    uint8       qual_width;                         /* Actual Qualifier Width.*/
} _bcm_field_qual_offset_t;

/*
 * Typedef:
 *     _bcm_field_qual_data_t
 * Purpose:
 *     Qualifier data/mask field buffer.
 */
#define _FP_QUAL_DATA_WORDS  (4)
typedef uint32 _bcm_field_qual_data_t[_FP_QUAL_DATA_WORDS];   /* Qualifier data/mask buffer. */

#define _FP_QUAL_DATA_CLEAR(_data_) \
        sal_memset((_data_), 0, _FP_QUAL_DATA_WORDS * sizeof(uint32))


/*
 * Typedef:
 *     _bcm_field_qual_conf_s
 * Purpose:
 *     Select codes/offsets and other qualifier attributes.
 */
typedef struct _bcm_field_qual_conf_s {
    _bcm_field_selector_t     selector;    /* Per slice/device
                                              configuration required. */
    _bcm_field_qual_offset_t  offset;      /* Qualifier tcam offsets. */
} _bcm_field_qual_conf_t;

/*
 * Typedef:
 *     _bcm_field_entry_part_qual_s
 * Purpose:
 *     Field entry part qualifiers information.
 */
typedef struct _bcm_field_group_qual_s {
    uint16                   *qid_arr;    /* Qualifier id.           */
    _bcm_field_qual_offset_t *offset_arr; /* Qualifier tcam offsets. */
    uint16                   size;        /* Qualifier array size.   */
} _bcm_field_group_qual_t;

/* _FP_QUAL_* macros require the following declaration
   in any function which uses them.
 */
#define _FP_QUAL_DECL \
    int _rv_;         \
    _bcm_field_qual_conf_t _fp_qual_conf_; \
    soc_field_t _key_fld_ = SOC_IS_TD_TT(unit) ? KEYf : DATAf;

/* Add qualifier to stage qualifiers set. */
#define __FP_QUAL_EXT_ADD(_unit_, _stage_fc_, _qual_id_,                      \
                          _dev_sel_, _dev_sel_val_,                           \
                          _pri_sel_, _pri_sel_val_,                           \
                          _sec_sel_, _sec_sel_val_,                           \
                          _ter_sel_, _ter_sel_val_,                           \
                          _intraslice_,                                       \
                          _offset_0_, _width_0_,                              \
                          _offset_1_, _width_1_,                              \
                          _offset_2_, _width_2_,                              \
                          _secondary_, _primary_)                             \
            do {                                                              \
                _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                (_fp_qual_conf_).selector.dev_sel       = (_dev_sel_);        \
                (_fp_qual_conf_).selector.dev_sel_val   = (_dev_sel_val_);    \
                (_fp_qual_conf_).selector.pri_sel       = (_pri_sel_);        \
                (_fp_qual_conf_).selector.pri_sel_val   = (_pri_sel_val_);    \
                (_fp_qual_conf_).selector.sec_sel       = (_sec_sel_);        \
                (_fp_qual_conf_).selector.sec_sel_val   = (_sec_sel_val_);    \
                (_fp_qual_conf_).selector.ter_sel       = (_ter_sel_);        \
                (_fp_qual_conf_).selector.ter_sel_val   = (_ter_sel_val_);    \
                (_fp_qual_conf_).selector.intraslice    = (_intraslice_);     \
                (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                (_fp_qual_conf_).offset.num_offsets     = 3;                  \
                (_fp_qual_conf_).offset.offset[0]       = _offset_0_ ;        \
                (_fp_qual_conf_).offset.width[0]        = _width_0_;          \
                (_fp_qual_conf_).offset.offset[1]       = _offset_1_ ;        \
                (_fp_qual_conf_).offset.width[1]        = _width_1_;          \
                (_fp_qual_conf_).offset.offset[2]       = _offset_2_ ;        \
                (_fp_qual_conf_).offset.width[2]        = _width_2_;          \
                (_fp_qual_conf_).offset.secondary       = (_secondary_);      \
                (_fp_qual_conf_).selector.secondary    = (_secondary_);       \
                (_fp_qual_conf_).selector.primary       = (_primary_);        \
                _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),     \
                                             (_qual_id_), &(_fp_qual_conf_)); \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

#define _FP_QUAL_EXT_ADD(_unit_, _stage_fc_, _qual_id_,         \
                         _dev_sel_, _dev_sel_val_,              \
                         _pri_sel_, _pri_sel_val_,              \
                         _sec_sel_, _sec_sel_val_,              \
                         _intraslice_,                          \
                         _offset_0_, _width_0_,                 \
                         _offset_1_, _width_1_,                 \
                         _offset_2_, _width_2_,                 \
                         _secondary_)                           \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),      \
                      (_dev_sel_), (_dev_sel_val_),             \
                      (_pri_sel_), (_pri_sel_val_),             \
                      (_sec_sel_), (_sec_sel_val_),             \
                      _bcmFieldDevSelDisable, 0,                \
                      (_intraslice_),                           \
                      (_offset_0_), (_width_0_),                \
                      (_offset_1_), (_width_1_),                \
                      (_offset_2_), (_width_2_),                \
                      (_secondary_), 0                          \
                      )

#define _FP_QUAL_PRIMARY_EXT_ADD(_unit_, _stage_fc_, _qual_id_,     \
                         _dev_sel_, _dev_sel_val_,              \
                         _pri_sel_, _pri_sel_val_,              \
                         _sec_sel_, _sec_sel_val_,              \
                         _intraslice_,                          \
                         _offset_0_, _width_0_,                 \
                         _offset_1_, _width_1_,                 \
                         _offset_2_, _width_2_,                 \
                         _secondary_, _primary_)                \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),      \
                      (_dev_sel_), (_dev_sel_val_),             \
                      (_pri_sel_), (_pri_sel_val_),             \
                      (_sec_sel_), (_sec_sel_val_),             \
                      _bcmFieldDevSelDisable, 0,                \
                      (_intraslice_),                           \
                      (_offset_0_), (_width_0_),                \
                      (_offset_1_), (_width_1_),                \
                      (_offset_2_), (_width_2_),                \
                      (_secondary_), (_primary_)                \
                      )

#define _FP_QUAL_PRIMARY_ADD(_unit_, _stage_fc_, _qual_id_,                            \
                     _pri_slice_sel_, _pri_slice_sel_val_,                     \
                     _offset_0_, _width_0_)                                    \
                     _FP_QUAL_PRIMARY_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      _bcmFieldSliceSelDisable, 0, 0,          \
                                      (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 1)

/* Add single offset & two slice selectors qualifier. */
#define _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _pri_slice_sel_, _pri_slice_sel_val_,       \
                                   _sec_slice_sel_, _sec_slice_sel_val_,       \
                                   _offset_0_, _width_0_)                      \
                     _FP_QUAL_PRIMARY_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                      (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      (_sec_slice_sel_), (_sec_slice_sel_val_),\
                                    0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 1)

/* Add single offset & three slice selectors qualifier. */
#define _FP_QUAL_PRIMARY_THREE_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,     \
                                     _pri_slice_sel_, _pri_slice_sel_val_, \
                                     _sec_slice_sel_, _sec_slice_sel_val_, \
                                     _ter_slice_sel_, _ter_slice_sel_val_, \
                                     _offset_0_, _width_0_)             \
    __FP_QUAL_PRIMARY_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),               \
                      _bcmFieldDevSelDisable, 0,                        \
                      (_pri_slice_sel_), (_pri_slice_sel_val_),         \
                      (_sec_slice_sel_), (_sec_slice_sel_val_),         \
                      (_ter_slice_sel_), (_ter_slice_sel_val_),         \
                      0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 0, 1)

/* Add basic (single offset & single per slice selector. */
#define _FP_QUAL_ADD(_unit_, _stage_fc_, _qual_id_,                            \
                     _pri_slice_sel_, _pri_slice_sel_val_,                     \
                     _offset_0_, _width_0_)                                    \
                     _FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      _bcmFieldSliceSelDisable, 0, 0,          \
                                      (_offset_0_), (_width_0_), 0, 0, 0, 0, 0)

/* Add single offset & two slice selectors qualifier. */
#define _FP_QUAL_TWO_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _pri_slice_sel_, _pri_slice_sel_val_,       \
                                   _sec_slice_sel_, _sec_slice_sel_val_,       \
                                   _offset_0_, _width_0_)                      \
                     _FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                      (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      (_sec_slice_sel_), (_sec_slice_sel_val_),\
                                    0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0)

/* Add single offset & three slice selectors qualifier. */
#define _FP_QUAL_THREE_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,     \
                                     _pri_slice_sel_, _pri_slice_sel_val_, \
                                     _sec_slice_sel_, _sec_slice_sel_val_, \
                                     _ter_slice_sel_, _ter_slice_sel_val_, \
                                     _offset_0_, _width_0_)             \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),               \
                      _bcmFieldDevSelDisable, 0,                        \
                      (_pri_slice_sel_), (_pri_slice_sel_val_),         \
                      (_sec_slice_sel_), (_sec_slice_sel_val_),         \
                      (_ter_slice_sel_), (_ter_slice_sel_val_),         \
                      0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 0)

/* Add single offset & two slice selectors qualifier. */
#define _FP_QUAL_INTRASLICE_ADD(_unit_, _stage_fc_, _qual_id_,                 \
                                _pri_slice_sel_, _pri_slice_sel_val_,          \
                                _sec_slice_sel_, _sec_slice_sel_val_,          \
                                _offset_0_, _width_0_)                         \
                     _FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                      (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      (_sec_slice_sel_), (_sec_slice_sel_val_),\
                                  0x1, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0)


/* Add qualifier to stage qualifiers set. */
#define __FP_QUAL_MULTI_EXT_ADD(_unit_, _stage_fc_, _qual_id_,                \
                                _dev_sel_, _dev_sel_val_,                     \
                                _pri_sel_, _pri_sel_val_,                     \
                                _sec_sel_, _sec_sel_val_,                     \
                                _ter_sel_, _ter_sel_val_,                     \
                                _intraslice_, _num_offsets_,                  \
                                _offset_arr_, _width_arr_)                    \
            do {                                                              \
                _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                (_fp_qual_conf_).selector.dev_sel       = (_dev_sel_);        \
                (_fp_qual_conf_).selector.dev_sel_val   = (_dev_sel_val_);    \
                (_fp_qual_conf_).selector.pri_sel       = (_pri_sel_);        \
                (_fp_qual_conf_).selector.pri_sel_val   = (_pri_sel_val_);    \
                (_fp_qual_conf_).selector.sec_sel       = (_sec_sel_);        \
                (_fp_qual_conf_).selector.sec_sel_val   = (_sec_sel_val_);    \
                (_fp_qual_conf_).selector.ter_sel       = (_ter_sel_);        \
                (_fp_qual_conf_).selector.ter_sel_val   = (_ter_sel_val_);    \
                (_fp_qual_conf_).selector.intraslice    = (_intraslice_);     \
                (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                (_fp_qual_conf_).offset.num_offsets     = (_num_offsets_);    \
                sal_memcpy(&(_fp_qual_conf_).offset.offset, _offset_arr_,     \
                     sizeof(_fp_qual_conf_.offset.offset)*_num_offsets_t);    \
                sal_memcpy(&(_fp_qual_conf_).offset.width, _width_arr_,       \
                     sizeof(_fp_qual_conf_.offset.width)*_num_offsets_t);     \
                (_fp_qual_conf_).selector.secondary    = 0;                   \
                _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),     \
                                             (_qual_id_), &(_fp_qual_conf_)); \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

#define _FP_QUAL_MULTI_EXT_ADD(_unit_, _stage_fc_, _qual_id_,         \
                               _dev_sel_, _dev_sel_val_,              \
                               _pri_sel_, _pri_sel_val_,              \
                               _sec_sel_, _sec_sel_val_,              \
                               _intraslice_,                          \
                               _num_offsets_,                         \
                               _offset_arr_, _width_arr_)             \
       __FP_QUAL_MULTI_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),   \
                         (_dev_sel_), (_dev_sel_val_),             \
                         (_pri_sel_), (_pri_sel_val_),             \
                         (_sec_sel_), (_sec_sel_val_),             \
                          _bcmFieldDevSelDisable, 0,               \
                         (_intraslice_),                           \
                          _num_offsets_,                           \
                          _offset_arr_, _width_arr_)
                      

/* Add multiple offsets. */
#define _FP_QUAL_MULTI_ADD(_unit_, _stage_fc_, _qual_id_,                      \
                           _pri_slice_sel_, _pri_slice_sel_val,                \
                           _num_offsets_, offset_arr_, _width_arr_)            \
                  _FP_QUAL_MULTI_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),   \
                                    _bcmFieldDevSelDisable, 0,                 \
                                    (_pri_slice_sel_), (_pri_slice_sel_val_),  \
                                     _bcmFieldSliceSelDisable, 0, 0,           \
                                     _num_offsets_, _offset_arr_, _width_arr_)

/* Add basic (single offset per slice selector with qualifier bit position. */
#define _FP_QUAL_MULTI_INFO_ADD(_unit_, _stage_fc_, _qual_id_,                 \
                            _pri_slice_sel_, _pri_slice_sel_val_,              \
                            _offset_0_, _width_0_, _actual_width_,             \
                            _bit_pos_)                                         \
         do {                                                                  \
             _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                     \
            (_fp_qual_conf_).selector.pri_sel       = (_pri_slice_sel_);       \
            (_fp_qual_conf_).selector.pri_sel_val   = (_pri_slice_sel_val_);   \
            (_fp_qual_conf_).offset.field           = (_key_fld_);             \
            (_fp_qual_conf_).offset.num_offsets     = 1;                       \
            (_fp_qual_conf_).offset.offset[0]       = (_offset_0_);            \
            (_fp_qual_conf_).offset.width[0]        = (_width_0_);             \
            (_fp_qual_conf_).offset.qual_width      = (_actual_width_);        \
            (_fp_qual_conf_).offset.bit_pos         = (_bit_pos_);             \
             _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),         \
                                          (_qual_id_), &(_fp_qual_conf_));     \
             BCM_IF_ERROR_RETURN(_rv_);                                        \
         } while (0)

#define _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(_q_offset_, _width_)    \
       do {                                                        \
            int i;                                                 \
            _width_ = 0;                                           \
            for (i = 0; i < (_q_offset_)->num_offsets; i++)  {     \
                (_width_) = (_width_) + (_q_offset_)->width[i];    \
            }                                                      \
       } while(0)



/*
 * Typedef:
 *     _bcm_field_qual_t
 * Purpose:
 *
 */
typedef struct _bcm_field_qual_info_s {
    uint16                 qid;          /* Qualifer id.               */
    _bcm_field_qual_conf_t *conf_arr;    /* Configurations array.      */
    uint8                  conf_sz;      /* Configurations array size. */
} _bcm_field_qual_info_t;

/*
 * Typedef:
 *     _qual_info_t
 * Purpose:
 *     Holds format info for a particular qualifier's access parameters. These
 *     nodes are stored in qualifier lists for groups and in the FPFx tables
 *     for each chip.
 */
typedef struct _qual_info_s {
    int                    qid;     /* Which Qualifier              */
    soc_field_t            fpf;     /* FPFx field choice            */
    int                    offset;  /* Offset within FPFx field     */
    int                    width;   /* Qual width within FPFx field */
    struct _qual_info_s    *next;
} _qual_info_t;

/* Typedef:
 *     _field_fpf_info_t
 * Purpose:
 *     Hardware memory details of field qualifier
 */
typedef struct _field_fpf_info_s {
    _qual_info_t      **qual_table;
    bcm_field_qset_t  *sel_table;
    soc_field_t       field;
} _field_fpf_info_t;

/*
 * Struct:
 *     _field_counter32_collect_s
 * Purpose:
 *     Holds the accumulated count of FP Counters
 *         Useful for wrap around and movement.
 *     This is used when h/w counter width is <= 32 bits
 */
typedef struct _field_counter32_collect_s {
    uint64 accumulated_counter;
    uint32 last_hw_value;
} _field_counter32_collect_t;

/*
 * Struct:
 *     _field_counter64_collect_s
 * Purpose:
 *     Holds the accumulated count of FP Counters
 *         Useful for wrap around and movement.
 *     This is used when h/w counter width is > 32 bits
 *         e.g. Bradley, Triumph Byte counters
 */
typedef struct _field_counter64_collect_s {
    uint64 accumulated_counter;
    uint64 last_hw_value;
} _field_counter64_collect_t;

/*
 * Typedef:
 *     _field_meter_bmp_t
 * Purpose:
 *     Meter bit map for tracking allocation state of group's meter pairs.
 */
typedef struct _field_meter_bmp_s {
    SHR_BITDCL  *w;
} _field_meter_bmp_t;

#define _FP_METER_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_METER_BMP_ADD(bmp, mtr)     SHR_BITSET(((bmp).w), (mtr))
#define _FP_METER_BMP_REMOVE(bmp, mtr)  SHR_BITCLR(((bmp).w), (mtr))
#define _FP_METER_BMP_TEST(bmp, mtr)    SHR_BITGET(((bmp).w), (mtr))


/*
 * struct:
 *     _field_meter_pool_s
 * Purpose:
 *     Global meter pool
 * Note:
 *     Note that the 2nd parameter is slice_id and not group_id. This
 *     is because more than one group can share the same slice (pbmp based
 *     groups)
 *
 *     Due to this, it needs to be enforced that for an entry in a group
 *     which is not in the group's first slice (due to group auto-expansion/
 *     virtual groups), slice_id denotes the entry's group's first slice.
 */
typedef struct _field_global_meter_pool_s {
    int8               level;       /* Level meter pool attached.            */
    int                slice_id;    /* First slice which uses this meter pool*/
    uint16             size;        /* Number of valid meters in pool.       */
    uint16             pool_size;   /* Total number of meters in pool.       */
    uint16             free_meters; /* Number of free meters                 */
    uint16             num_meter_pairs; /* Number of meter pairs in a pool.  */
    _field_meter_bmp_t meter_bmp;       /* Meter usage bitmap                */
} _field_meter_pool_t;


typedef struct _field_global_counter_pool_s {
    int8               slice_id;   /* First slice which uses this pool */
    uint16             size;       /* Number of counters in pool */
    uint16             free_cntrs; /* Number of free counters */
    _field_meter_bmp_t cntr_bmp;   /* Counter usage bitmap */
} _field_cntr_pool_t;


/*
 * DATA qualifiers section.
 */

/* Instead of the old model of n sets of m 4-consecutive-byte chunks of ingress
   packet (currently: n = 2 and m = 4), consider all user-defined 4-byte chunks
   of ingress packet data presented to the FP as equivalent -- the only proviso
   is that any one data qualifier cannot use chunks that are in different "udf
   numbers" (since those are in different FP selectors).
   So, call the index of a 4-byte chunk in the set of (n*m) of them the "data
   qualifier index".  This is also the "column index" in the FP_UDF_OFFSET
   table. An API-level data qualifier that uses multiple 4-byte
   chunks will be assigned a data qualifier index (see _field_data_qualifier_t)
   based on the lowest numbered chunk index it uses.
*/
#define BCM_FIELD_USER_DQIDX(udf_num, user_num) \
    ((udf_num) * (BCM_FIELD_USER_MAX_USER_NUM + 1) + (user_num))
/* Maximum number of data qualifiers. */
#define BCM_FIELD_USER_MAX_DQS \
    BCM_FIELD_USER_DQIDX((BCM_FIELD_USER_MAX_UDF_NUM + 1), 0)
#define BCM_FIELD_USER_DQIDX_TO_UDF_NUM(dqidx)   ((dqidx) / (BCM_FIELD_USER_MAX_USER_NUM + 1))
#define BCM_FIELD_USER_DQIDX_TO_USER_NUM(dqidx)  ((dqidx) % (BCM_FIELD_USER_MAX_USER_NUM + 1))

/* Maximum packet depth data qualifier can qualify */
#define _FP_DATA_OFFSET_MAX      (128)

/* Maximum packet depth data qualifier can qualify */
#define _FP_DATA_WORDS_COUNT         (8)

/* Number of UDF_OFFSETS allocated for ethertype based qualifiers. */
#define _FP_DATA_ETHERTYPE_MAX              (8)
/* Number of UDF_OFFSETS allocated for IPv4 protocol based qualifiers. */
#define _FP_DATA_IP_PROTOCOL_MAX            (2)
/* Number of UDF_OFFSETS allocated for IPv6 next header based qualifiers. */
#define _FP_DATA_NEXT_HEADER_MAX            (2)

/* Hw specific config.*/
#define _FP_DATA_DATA0_WORD_MIN     (0)
#define _FP_DATA_DATA0_WORD_MAX     (3)
#define _FP_DATA_DATA1_WORD_MIN     (4)
#define _FP_DATA_DATA1_WORD_MAX     (7)

/* Ethertype match L2 format. */
#define  _BCM_FIELD_DATA_FORMAT_ETHERTYPE (0x3)

/* Ip protocol match . */
#define  _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0 (BCM_FIELD_DATA_FORMAT_IP6 + 0x1)
#define  _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1 (BCM_FIELD_DATA_FORMAT_IP6 + 0x2)
#define  _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0 (BCM_FIELD_DATA_FORMAT_IP6 + 0x3)
#define  _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1 (BCM_FIELD_DATA_FORMAT_IP6 + 0x4)

/* Packet content (data) qualification object flags. */
#define _BCM_FIELD_DATA_QUALIFIER_FLAGS                       \
     (BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST |    \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_IP6_EXTENSIONS_ADJUST | \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_GRE_OPTIONS_ADJUST |    \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_FLEX_HASH |             \
      BCM_FIELD_DATA_QUALIFIER_STAGE_LOOKUP)

/* UDF spec - offset is valid flag   */
#define _BCM_FIELD_USER_OFFSET_VALID   (1 << 31)

#define _BCM_FIELD_USER_OFFSET_FLAGS           \
            (_BCM_FIELD_USER_OFFSET_VALID |    \
             _BCM_FIELD_USER_OPTION_ADJUST |   \
             _BCM_FIELD_USER_GRE_OPTION_ADJUST)

/* Data qualifiers tcam priorities. */
 /* Specified packet l2 format. */
#define _FP_DATA_QUALIFIER_PRIO_L2_FORMAT   (0x01)
 /* Specified number of vlan tags. */
#define _FP_DATA_QUALIFIER_PRIO_VLAN_FORMAT (0x01)
 /* Specified l3 format tunnel types/inner/outer ip type. */
#define _FP_DATA_QUALIFIER_PRIO_L3_FORMAT   (0x01)
 /* Specified MPLS format tunnel. */
#define _FP_DATA_QUALIFIER_PRIO_MPLS_FORMAT (0x01)
 /* Specified FCoE format tunnel. */
#define _FP_DATA_QUALIFIER_PRIO_FCOE_FORMAT (0x01)
 /* Specified ip protocol.  */
#define _FP_DATA_QUALIFIER_PRIO_IPPROTO     (0x01)
 /* Specified ip protocol/ethertype.  */
#define _FP_DATA_QUALIFIER_PRIO_MISC        (0x01)
 /* Specified priority for misc flags */
#define _FP_DATA_QUALIFIER_PRIO_FLAGS       (0x01)
 /* Maximum udf tcam entry priority.  */
#define _FP_DATA_QUALIFIER_PRIO_HIGHEST     (0xff)

/* MPLS data qualifier label priority */
#define _FP_DATA_QUALIFIER_PRIO_MPLS_ANY         (0x00)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_ONE_LABEL   (0x01)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_TWO_LABEL   (0x02)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_THREE_LABEL (0x03)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_FOUR_LABEL  (0x04)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_FIVE_LABEL  (0x05)

/*
 * Typedef:
 *     _field_data_qualifier_s
 * Purpose:
 *     Data qualifiers description structure.
 */
typedef struct _field_data_qualifier_s {
    int    qid;                     /* Qualifier id.                     */
    bcm_field_udf_spec_t *spec;     /* Hw adjusted offsets array.        */
    bcm_field_data_offset_base_t offset_base; /* Offset base adjustment. */
    int    offset;                  /* Master word offset.               */
    uint8  byte_offset;             /* Data offset inside the word.      */
    uint32 hw_bmap;                 /* Allocated hw words.               */
    uint32 flags;                   /* Offset adjustment flags.          */
    uint8  elem_count;              /* Number of hardware elements required. */
    int    length;                  /* Matched data byte length.         */
    struct _field_data_qualifier_s *next;/* Next installed  qualifier.   */
} _field_data_qualifier_t, *_field_data_qualifier_p;

/*
 * Typedef:
 *     _field_data_ethertype_s
 * Purpose:
 *     Ethertype based data qualifiers description structure.
 */
typedef struct _field_data_ethertype_s {
    int ref_count;                  /* Reference count.        */
    uint16 l2;                      /* Packet l2 format.       */
    uint16 vlan_tag;                /* Packet vlan tag format. */
    bcm_port_ethertype_t ethertype; /* Ether type value.       */
    int relative_offset;            /* Packet byte offset      */
                                    /* relative to qualifier   */
                                    /* byte offset.            */
} _field_data_ethertype_t;

/*
 * Typedef:
 *     _field_data_protocol_s
 * Purpose:
 *     Protocol based data qualifiers description structure.
 */
typedef struct _field_data_protocol_s {
    int ip4_ref_count;    /* Ip4 Reference count.    */
    int ip6_ref_count;    /* Ip6 Reference count.    */
    uint32 flags;         /* Ip4/Ip6 flags.          */
    uint8 ip;             /* Ip protocol id.         */
    uint16 l2;            /* Packet l2 format.       */
    uint16 vlan_tag;      /* Packet vlan tag format. */
    int relative_offset;  /* Packet byte offset      */
                          /* relative to qualifier   */
                          /* byte offset.            */
} _field_data_protocol_t;

/*
 * Typedef:
 *     _field_data_tcam_entry_s
 * Purpose:
 *     Field data tcam entry structucture. Used for
 *     tcam entry allocation and organization by relative priority.
 */
typedef struct _field_data_tcam_entry_s {
   uint8 ref_count;           /* udf tcam entry reference count.  */
   uint8 priority;            /* udf tcam entry priority.         */
} _field_data_tcam_entry_t;

/*
 * Typedef:
 *     _field_data_control_s
 * Purpose:
 *     Field data qualifiers control structucture.
 *
 */
typedef struct _field_data_control_s {
   uint32 usage_bmap;                 /* Offset usage bitmap.          */
   _field_data_qualifier_p data_qual; /* List of data qualifiers.      */
                                      /* Ethertype based qualifiers.   */
   _field_data_ethertype_t etype[_FP_DATA_ETHERTYPE_MAX];
                                      /* IP protocol based qualifiers. */
   _field_data_protocol_t  ip[_FP_DATA_IP_PROTOCOL_MAX];
   _field_data_tcam_entry_t *tcam_entry_arr;/* Tcam entries/keys array.*/
   int                     elem_size; /* Number of bytes per element. */
   int                     num_elems; /* Number of elements per UDF. */
} _field_data_control_t;

/* Katana2 can support 170 pp_ports */
#define _FP_ACTION_PARAM_SZ             (6)

/*
 * Typedef:
 *     _bcm_field_action_offset_t
 * Purpose:
 *     Action offsets in FP policy table.
 */
typedef struct _bcm_field_action_offset_s {
    uint32      flags;     /* Action flags _BCM_FIELD_ACTION_XXX. */
    uint16      offset[_FP_ACTION_PARAM_SZ]; /* Action offset.    */
    uint8       width[_FP_ACTION_PARAM_SZ];  /* Action width.     */
    int         value[_FP_ACTION_PARAM_SZ];  /* Action Value.     */
    struct _bcm_field_action_offset_s *next;
} _bcm_field_action_offset_t;

typedef struct _bcm_field_action_conf_s {
    bcm_field_action_t         action;  /* Field Action bcmFieldActionXXX. */
    _bcm_field_action_offset_t *offset; /* Action offset information.      */
} _bcm_field_action_conf_t;


#ifdef BCM_TOMAHAWK_SUPPORT
typedef struct _field_action_params_s {
    int    cosq_new;
    int    nh_ecmp_id;
    uint8  is_dvp;
    uint8  is_trunk;
    uint8  fabric_tag_type;
    uint8  ucast_redirect_control;
    uint8  mcast_redirect_control;
    uint32 flags;
    uint32 i2e_cl_sel;
    uint32 i2e_hg_cl_sel;
    uint32 fabric_tag;
    uint32 redirect_value;
#ifdef INCLUDE_L3
    int32  hw_idx;
    int32  hw_half;
#endif
    int mtp_index;
    int mtp_type_index;
} _field_action_params_t;

#define _BCM_FIELD_ACTION_REDIRECT_DGLP     (1 << 0)
#define _BCM_FIELD_ACTION_REDIRECT_DVP      (1 << 1)
#define _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP (1 << 2)
#define _BCM_FIELD_ACTION_REDIRECT_ECMP     (1 << 3)
#define _BCM_FIELD_ACTION_REDIRECT_TRUNK    (1 << 4)
#define _BCM_FIELD_ACTION_NO_IFP_SUPPORT    (1 << 5)
#define _BCM_FIELD_ACTION_NO_EM_SUPPORT     (1 << 6)

#define _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP (1 << 0)
#define _BCM_FIELD_ACTION_L3SWITCH_ECMP     (1 << 1)

#define _FP_INVALID_ACTION_VAL (-1)

/* _FP_ACTION_* macros require the following declaration
 *    in any function which uses them.
 *     */
#define _FP_ACTION_DECL \
    int _rv_;         \
    _bcm_field_action_offset_t _fp_action_offset_;

/* Add Action to stage actions set. */
#define _FP_ACTION_ADD(_unit_, _stage_fc_, _action_, _flags_,                 \
                               _offset1_, _width1_, _value1_,                 \
                               _offset2_, _width2_, _value2_,                 \
                               _offset3_, _width3_, _value3_,                 \
                               _offset4_, _width4_, _value4_)                 \
            do {                                                              \
                sal_memset(&(_fp_action_offset_), 0,                          \
                           sizeof(_fp_action_offset_));                       \
                (_fp_action_offset_).flags            |= (_flags_);           \
                (_fp_action_offset_).offset[0]   = (_offset1_);               \
                (_fp_action_offset_).width[0]    = (_width1_);                \
                (_fp_action_offset_).value[0]    = (_value1_);                \
                (_fp_action_offset_).offset[1]   = (_offset2_);               \
                (_fp_action_offset_).width[1]    = (_width2_);                \
                (_fp_action_offset_).value[1]    = (_value2_);                \
                (_fp_action_offset_).offset[2]   = (_offset3_);               \
                (_fp_action_offset_).width[2]    = (_width3_);                \
                (_fp_action_offset_).value[2]    = (_value3_);                \
                (_fp_action_offset_).offset[3]   = (_offset4_);               \
                (_fp_action_offset_).width[3]    = (_width4_);                \
                (_fp_action_offset_).value[3]    = (_value4_);                \
                (_fp_action_offset_).next      = NULL;                        \
                _rv_ =_bcm_field_action_insert((_unit_), (_stage_fc_),        \
                                       (_action_), &(_fp_action_offset_));    \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

/* Add single offset, width, value for action. */
#define _FP_ACTION_ADD_ONE(_unit_, _stage_fc_, _action_, _flags_,             \
                                   _offset1_, _width1_, _value1_)             \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),        \
                                      (_flags_), (_offset1_), (_width1_),     \
                                      (_value1_), 0, 0, 0, 0, 0, 0, 0, 0, 0)   

#define _FP_ACTION_ADD_TWO(_unit_, _stage_fc_, _action_, _flags_,              \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), (_offset1_), (_width1_),      \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), 0, 0, 0, 0, 0, 0)      

#define _FP_ACTION_ADD_THREE(_unit_, _stage_fc_, _action_, _flags_,            \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), (_offset1_), (_width1_),      \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                                      (_value3_), 0, 0, 0)  

#define _FP_ACTION_ADD_FOUR(_unit_, _stage_fc_, _action_, _flags_,             \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_,         \
                                        _offset4_, _width4_, _value4_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), (_offset1_), (_width1_),      \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                      (_value3_), (_offset4_), (_width4_),     \
                                                               (_value4_))  

#define ACTION_CHECK(width, param)                          \
                         if (((1 << width) - 1) < param) {  \
                             return BCM_E_PARAM;            \
                         }

#define ACTION_GET(_unit_, _f_ent_, _buf_, _action_, _flags_,  _a_offset_)   \
            do {                                                             \
               int _rv_;                                                     \
               _rv_ = _bcm_field_action_val_get((_unit_), (_f_ent_),         \
                                                   (_buf_), (_action_),      \
                                                   (_flags_), (_a_offset_)); \
               BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)                       

#define ACTION_OFFSET_GET(_unit_, _stagefc_, _action_, _a_offset_, _flags_)  \
            do {                                                             \
               int _rv_;                                                     \
               _rv_ = _bcm_field_action_offset_get((_unit_), (_stagefc_),    \
                                                (_action_), (_a_offset_),    \
                                                              (_flags_));    \
               BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)  

#define ACTION_SET(_unit_, _f_ent_, _buf_, _a_offset_)                         \
            do {                                                               \
               int _rv_;                                                       \
               _rv_ = _bcm_field_action_val_set((_unit_), (_f_ent_),           \
                                                   (_buf_),  (_a_offset_)); \
               BCM_IF_ERROR_RETURN(_rv_);                                      \
            } while (0)

#define ACTION_RESOLVE(_unit_, _fent_, _fa_, _params_)                \
            do {                                                      \
                int _rv_;                                             \
                _rv_ = _bcm_field_action_resolve((_unit_), (_fent_),  \
                                                 (_fa_), (_params_)); \
                BCM_IF_ERROR_RETURN(_rv_);                            \
            } while (0)

#endif /* BCM_TOMAHAWK_SUPPORT */
/*
 * Stage flags.
 */

/* Separate packet byte counters. */
#define _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS (1 << 0)

/* Global meter pools . */
#define _FP_STAGE_GLOBAL_METER_POOLS            (1 << 1)

/* Global counters. */
#define _FP_STAGE_GLOBAL_COUNTERS               (1 << 2)

/* Auto expansion support. */
#define _FP_STAGE_AUTO_EXPANSION                (1 << 3)

/* Slice enable/disable support. */
#define _FP_STAGE_SLICE_ENABLE                  (1 << 4)

/* Only first half of slice is valid.. */
#define _FP_STAGE_HALF_SLICE                    (1 << 5)

/* Global counter pools . */
#define _FP_STAGE_GLOBAL_CNTR_POOLS             (1 << 6)

/* Only first quarter of slice is valid.. */
#define _FP_STAGE_QUARTER_SLICE                 (1 << 7)

#define _FP_STAGE_MULTI_PIPE_COUNTERS           (1 << 8)

/* Maximum number of FP instances. */
#define _FP_MAX_NUM_PIPES (SOC_MAX_NUM_PIPES)

/* Default FP instance. */
#define _FP_DEF_INST (0)


#if defined(BCM_TOMAHAWK_SUPPORT)
/* Maximum number of logical table partitions supported. */
#define _FP_MAX_LT_PARTS (12)

/* Total number of logical tables supported. */
#define _FP_MAX_NUM_LT (32)

/* Maximum value supported for a LT partition in hardware. */
#define _FP_MAX_LT_PART_PRIO (15)

/* Total number of extractor levels. */
#define _FP_EXT_LEVEL_COUNT (4)

/* 
 * Field Extractors selector macros
 *
 * These are for resolving if the select code has meaning or is really
 * a don't care.
 */
#define _FP_EXT_SELCODE_DONT_CARE (-1)
#define _FP_EXT_SELCODE_DONT_USE (-2)

#define _FP_EXT_SELCODE_SET(_selcode_qset, _selcode) \
    sal_memset((_selcode_qset), (int8)_selcode, sizeof(_field_ext_sel_t));

#define _FP_EXT_SELCODE_CLEAR(_selcode_) \
    _FP_EXT_SELCODE_SET(&(_selcode_), _FP_EXT_SELCODE_DONT_CARE)

/* Maximum number of bus chunks used by a qualifier. */
#define _FP_QUAL_MAX_CHUNKS (128)

/*
 * Typedef:
 *     _field_keygen_profiles_t
 * Purpose:
 *     TCAM Key generation program profile tables information structure.
 */
typedef struct _field_keygen_profiles_s {
    soc_profile_mem_t profile;      /* keygen profile.                  */
} _field_keygen_profiles_t;

/*
 * Typedef:
 *     _field_lt_slice_t
 * Purpose:
 *     Logical Table Selection TCAM slice information structure.
 */
typedef struct _field_lt_slice_s {
    uint8 slice_number;             /* Slice number.                        */
    int start_tcam_idx;             /* Slice first entry TCAM index.        */
    int entry_count;                /* Number of entries in the slice.      */
    int free_count;                 /* Number of free entries.              */
    _field_stage_id_t stage_id;     /* Pipeline Stage ID.                   */
    struct _field_lt_entry_s **entries; /* List of entries pointer.         */
    struct _field_presel_entry_s **p_entries; /* List of Presel entries pointer.*/
    struct _field_lt_slice_s *next; /* Linked list of auto expanded groups. */
    struct _field_lt_slice_s *prev; /* Linked list of auto expanded groups. */
    uint16 slice_flags;             /* Slice flags.                         */
    uint16 group_flags;             /* Group flags.                         */
} _field_lt_slice_t;

/*
 * Typedef:
 *     _field_lt_tcam_t
 * Purpose:
 *     Logical Table Selection TCAM Entry Key, Mask and Data information
 *     structure.
 */
typedef struct _field_lt_tcam_s {
    uint32 *key;        /* HW entry key replica.    */
    uint32 *mask;       /* HW entry mask replica.   */
    uint16 key_size;    /* HW entry Key size.       */
    uint32 *data;       /* HW entry data replica.   */
    uint16 data_size;   /* HW entry Data size.      */
} _field_lt_tcam_t;

/*
 * Typedef:
 *     _field_lt_entry_t
 * Purpose:
 *     Logical Table Selection TCAM Entry information structure.
 */
typedef struct _field_lt_entry_s {
    uint32 flags;                    /* Entry flags _FP_ENTRY_XXX.            */
    bcm_field_entry_t eid;           /* Unique identifier.                    */
    int index;                       /* Entry slice index.                    */
    int prio;                        /* Entry priority.                       */
    _field_lt_slice_t *lt_fs;        /* LT slice where entry lives.           */
    struct _field_lt_tcam_s tcam;    /* Fields to be written in logical table
                                        select TCAM.                          */
    struct _field_action_s *actions; /* Lined list of actions per entry.      */
    struct _field_group_s *group;    /* Group with which this entry 
                                        associated.                           */
    struct _field_lt_entry_s *next;  /* LT entry lookup linked list           */
} _field_lt_entry_t;

/*
 * Typedef:
 *     _field_lt_config_t
 * Purpose:
 *     Logical Tables configuration information structure.
 */
typedef struct _field_lt_config_s {
    int valid;                          /* Map is valid.                       */
    int lt_id;                          /* Logical Table ID.                   */
    int lt_part_pri[_FP_MAX_LT_PARTS];  /* Logical table partition priority.   */
    uint32 lt_part_map;                 /* Logical Table to Physical slice
                                           mapping.                            */
    int lt_action_pri;                  /* Logical Table action priority.      */
    int priority;                       /* Group priority.                     */
    int flags;                          /* Group flags.                        */
    bcm_field_entry_t lt_entry;         /* LT selection entry ID.              */
} _field_lt_config_t;

/*
 * Typedef:
 *     _field_group_lt_status_t
 * Purpose:
 *     Field Group Logical Table Status structure.
 */
typedef struct _field_group_lt_status_s {
    int entries_total;  /* Total possible entries in group.     */
    int entries_free;   /* Unused entries in LT slice.          */
    int entry_count;    /* Number of entries in the group.      */
} _field_group_lt_status_t;

/*
 * Typedef:
 *     _field_ext_sel_t
 * Purpose:
 *     Logical table key generation program control selectors.
 */
typedef struct _field_ext_sel_s {
    int8 l1_e32_sel[4];         /* Level 1 32b extractors.              */
    int8 l1_e16_sel[7];         /* Level 1 16b extractors.              */
    int8 l1_e8_sel[7];          /* Level 1 8b extractors.               */
    int8 l1_e4_sel[8];          /* Level 1 4b extractors.               */
    int8 l1_e2_sel[8];          /* Level 1 2b extractors.               */
    int8 l2_e16_sel[10];        /* Level 2 16b extractors.              */
    int16 l3_e1_sel[2];         /* Level 3 1b extractors.               */
    int8 l3_e2_sel[5];          /* Level 3 2b extractors.               */
    int8 l3_e4_sel[21];         /* Level 3 4b extractors.               */
    int8 pmux_sel[15];          /* Post muxing extractors.              */
    int8 intraslice;            /* Intraslice double wide selection.    */
    int8 secondary;             /* Secondary selector.                  */
    int8 ipbm_present;          /* Post mux IPBM in final key.          */
    int8 normalize_l3_l4_sel;   /* Normalize L3 and L4 address.         */
    int8 normalize_mac_sel;     /* Normalize L2 SA & DA.                */
    int8 aux_tag_a_sel;         /* Auxiliary Tag A Selector.            */
    int8 aux_tag_b_sel;         /* Auxiliary Tag B Selector.            */
    int8 aux_tag_c_sel;         /* Auxiliary Tag C Selector.            */
    int8 aux_tag_d_sel;         /* Auxiliary Tag d Selector.            */
    int8 tcp_fn_sel;            /* TCP function selector.               */
    int8 tos_fn_sel;            /* TOS function selector.               */
    int8 ttl_fn_sel;            /* TTL function selector.               */
    int8 class_id_cont_a_sel;   /* Class ID container A selector.       */
    int8 class_id_cont_b_sel;   /* Class ID container B selector.       */
    int8 class_id_cont_c_sel;   /* Class ID container C selector.       */
    int8 class_id_cont_d_sel;   /* Class ID container D selector.       */
    int8 src_cont_a_sel;        /* SRC container A selector.            */
    int8 src_cont_b_sel;        /* SRC container B selector.            */
    int8 src_dest_cont_0_sel;   /* SRC or DST container 0 selector.     */
    int8 src_dest_cont_1_sel;   /* SRC or DST container 1 selector.     */
    uint32 keygen_index;        /* LT keygen program profile index.     */
} _field_ext_sel_t;

/*
 * Typedef:
 *     _field_keygen_ext_sel_t
 * Purpose:
 *     Key Generation extractor selector encodings.
 */
typedef enum _field_keygen_ext_sel_e {
    _FieldKeygenExtSelDisable   = 0,  /* Selector disabled.               */
    _FieldKeygenExtSelL1E32     = 1,  /* Level1 32bit extractor.          */
    _FieldKeygenExtSelL1E16     = 2,  /* Level1 16bit extractor.          */
    _FieldKeygenExtSelL1E8      = 3,  /* Level1 8bit extractor.           */
    _FieldKeygenExtSelL1E4      = 4,  /* Level1 4bit extractor.           */
    _FieldKeygenExtSelL1E2      = 5,  /* Level1 2bit extractor.           */
    _FieldKeygenExtSelL2E16     = 6,  /* Level2 16bit extractor.          */
    _FieldKeygenExtSelL2E4      = 7,  /* 104 Passthru bits.               */
    _FieldKeygenExtSelL3E16     = 8,  /* Level3 16bit extractor.          */
    _FieldKeygenExtSelL3E4      = 9,  /* Level3 4bit extractor.           */
    _FieldKeygenExtSelL3E2      = 10, /* Level3 2bit extractor.           */
    _FieldKeygenExtSelL3E1      = 11, /* Level3 16bit extractor.          */
    _FieldKeygenExtSelL4        = 12, /* Level4 output.                   */
    _FieldKeygenExtSelL4A       = 13, /* Level4 Slice A output.           */
    _FieldKeygenExtSelL4B       = 14, /* Level4 Slice B output.           */
    _FieldKeygenExtSelL4C       = 15, /* Level4 Slice C output.           */
    _FieldKeygenExtSelL2AE16    = 16, /* Level2 Slice A 16bit extractor.  */
    _FieldKeygenExtSelL2BE16    = 17, /* Level2 Slice B 16 bit extractor. */
    _FieldKeygenExtSelL2CE16    = 18, /* Level2 Slice C 16 bit extractor. */
    _FieldKeygenExtSelL2AE4     = 19, /* Level2 Slice A 4bit extractor.   */
    _FieldKeygenExtSelL2BE4     = 20, /* Level2 Slice B 4bit extractor.   */
    _FieldKeygenExtSelL2CE4     = 21, /* Level2 Slice C 4bit extractor.   */
    _FieldKeygenExtSelL3AE16    = 22, /* Level3 Slice A 16bit extractor.  */
    _FieldKeygenExtSelL3BE16    = 23, /* Level3 Slice B 16bit extractor.  */
    _FieldKeygenExtSelL3CE16    = 24, /* Level3 Slice C 16bit extractor.  */
    _FieldKeygenExtSelL3AE4     = 25, /* Level3 Slice A 4bit extractor.   */
    _FieldKeygenExtSelL3BE4     = 26, /* Level3 Slice B 4bit extractor.   */
    _FieldKeygenExtSelL3CE4     = 27, /* Level3 Slice C 4bit extractor.   */
    _FieldKeygenExtSelCount     = 28  /* Always Last. Not a usable value. */
} _field_keygen_ext_sel_t;

/*
 * Typedef:
 *     _field_ext_ctrl_sel_t
 * Purpose:
 *     Pre-Mux extractor input bus controls.
 */
typedef enum _field_ext_ctrl_sel_e {
    _FieldExtCtrlSelDisable         = 0, /* Selector disabled.                */
    _FieldExtCtrlAuxTagASel         = 1, /* Auxiliary Tag A selector.         */
    _FieldExtCtrlAuxTagBSel         = 2, /* Auxiliary Tag B selector.         */
    _FieldExtCtrlAuxTagCSel         = 3, /* Auxiliary Tag C selector.         */
    _FieldExtCtrlAuxTagDSel         = 4, /* Auxiliary Tag D selector.         */
    _FieldExtCtrlTcpFnSel           = 5, /* TCP function selector.            */
    _FieldExtCtrlTosFnSel           = 6, /* ToS function selector.            */
    _FieldExtCtrlTtlFnSel           = 7, /* TTL function selector.            */
    _FieldExtCtrlClassIdContASel    = 8, /* Class ID container A selector.    */
    _FieldExtCtrlClassIdContBSel    = 9, /* Class ID container B selector.    */
    _FieldExtCtrlClassIdContCSel    = 10, /* Class ID container C selector.   */
    _FieldExtCtrlClassIdContDSel    = 11, /* Class ID container D selector.   */
    _FieldExtCtrlSrcContASel        = 12, /* Source Container A selector.     */
    _FieldExtCtrlSrcContBSel        = 13, /* Source Container B selector.     */
    _FieldExtCtrlSrcDestCont0Sel    = 14, /* Src/Dst Container 0 selector.    */
    _FieldExtCtrlSrcDestCont1Sel    = 15, /* Src/Dst Container 1 selector.    */
    _FieldExtCtrlSelCount           = 16  /* Always Last. Not a usable value. */
} _field_ext_ctrl_sel_t;

/*
 * Typedef:
 *     _field_ext_conf_mode_t
 * Purpose:
 *     Key Generation extractors slice configuration modes.
 */
typedef enum _field_ext_conf_mode_e {
    _FieldExtConfMode160Bits = 0,       /* Extractor config for 160b mode.  */
    _FieldExtConfMode320Bits = 1,       /* Extractor config for 320b mode.  */
    _FieldExtConfMode480Bits = 2,       /* Extractor config for 480b mode.  */
    _FieldExtConfModeCount = 3          /* Always Last. Not a usable value. */
} _field_ext_conf_mode_t;

/*
 * Typedef:
 *     _field_ext_params_t
 * Purpose:
 *     Extractors additional configuration parameters.
 */
typedef struct _field_ext_params_s {
    _field_keygen_ext_sel_t section;    /* Extractor section.               */
    uint8 sec_val;                      /* Extractor selector value.        */
    uint16 bus_offset;                  /* Field offset in input bus.       */
    uint8 width;                        /* Chunk widths in input bus.       */
    uint8 extracted;                    /* Bits extracted count.            */
} _field_ext_params_t;

/*
 * Typedef:
 *     _field_qual_sec_info_t
 * Purpose:
 *     Field qualifiers to extractor sections mapping information structure.
 */
typedef struct _field_qual_sec_info_s {
    bcm_field_qualify_t qid;             /* Nameslist                        */
    _field_ext_ctrl_sel_t ctrl_sel;      /* Control selector.                */
    uint8 ctrl_sel_val;                  /* Control selector value.          */
    _field_ext_params_t e_params[_FP_QUAL_MAX_CHUNKS]; /* Extractor params. */
    int size;                            /* Size of qualifier.               */
    uint16 attrib;                       /* Attribute.                       */
    uint8 bits_used;                     /* No. bits used from this section. */
    uint8 num_chunks;                    /* No. of valid qualifier chunks.   */
    uint8 in_use;                        /* Section inuse flag.              */
    struct _field_qual_sec_info_s *next; /* Next section info of qualifier.  */
} _field_qual_sec_info_t;

/*
 * Typedef:
 *     _field_bus_info_t
 * Purpose:
 *     Field Stage Ingress slice input bus information structure.
 */
typedef struct _field_bus_info_s {
   int size;                            /* Total input bus width.             */
   int num_fields;                      /* Number of qualifiers in input bus. */
   _field_qual_sec_info_t **qual_list;  /* Qualifiers to section information. */
} _field_bus_info_t;

/*
 * Typedef:
 *     _field_ace_info_t
 * Purpose:
 *     Field group acess control list information structure.
 */
typedef struct _field_ace_info_s {
    bcm_field_group_t gid;              /* Field group identifier.           */
    int size;                           /* Key size.                         */
    uint32 flags;                       /* Post muxed qualifiers flags.      */
    _field_qual_sec_info_t **qual_list; /* Qualifier extractor section info. */
} _field_ace_info_t;

/*
 * Typedef:
 *     _field_ext_cfg_t
 * Purpose:
 *     Field extractors configuration structure.
 */
typedef struct _field_ext_cfg_s {
    uint32 ext_id;                          /* Extractor identifier.        */
    int gran;                               /* Extractor ganularity.        */
    int ext_num;                            /* Extractor number.            */
    _field_keygen_ext_sel_t in_sec;         /* IN section.                  */
    _field_keygen_ext_sel_t out_sec;        /* OUT section.                 */
    int bus_offset;                         /* Output bus offset.           */
    uint32 flags;                           /* Attribtues _FP_EXT_ATTR_XXX. */
    uint8  in_use;                          /* Extractor inuse.             */
    int    chunk_extracted;                 /* Which chunk of attached qual 
                                               is extracted using this 
                                               extractor. */
    _field_qual_sec_info_t *attached_qual;  /* Attached qualifer.           */
} _field_ext_cfg_t;

/*
 * Typedef:
 *     _field_ext_sections_t
 * Purpose:
 *     Field sections information structure.
 */
typedef struct _field_ext_sections_s {
    _field_keygen_ext_sel_t sec;    /* Extractor section information.        */
    int fill_bits;                  /* No. of bits used from this extractor. */
    int drain_bits;                 /* Total bits in this extractor.         */
} _field_ext_sections_t;

/*
 * Typedef:
 *     _field_ext_info_t
 * Purpose:
 *     Field extractors configuration and extractor sections information
 *     structure.
 */
typedef struct _field_ext_info_s {
    _field_ext_conf_mode_t mode;                    /* Extractor
                                                       80b/160b/320b/480b
                                                       modes. */
    _field_ext_cfg_t *ext_cfg[_FP_EXT_LEVEL_COUNT]; /* Extractor config.     */
    uint16 conf_size[_FP_EXT_LEVEL_COUNT];          /* Configuration size.   */
    _field_ext_sections_t **sections;               /* Sections capacity.    */
    uint16 num_sec;                                 /* Number of sections.   */
} _field_ext_info_t;

/*
 * Typedef:
 *    _FP_EXT_ATTR_XXX
 * Purpose:
 *    Field extractor sections attribute flags.
 */
#define _FP_EXT_ATTR_PASS_THRU                  (1 << 0)
#define _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT          (1 << 1)
#define _FP_EXT_ATTR_NOT_WITH_IPBM              (1 << 2)
#define _FP_EXT_ATTR_DUPLICATE                  (1 << 3)
#define _FP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID  (1 << 4)
#define _FP_EXT_ATTR_NOT_WITH_NAT_NEEDED        (1 << 5)
#define _FP_EXT_ATTR_NOT_WITH_DROP              (1 << 6)
#define _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0    (1 << 7)
#define _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1    (1 << 8)
#define _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_80 (1 << 9)
#define _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_80 (1 << 10)

/*
 * Typedef:
 *    _FP_POST_MUX_XXX
 * Purpose:
 *    Post muxed field qualifiers flags.
 */
#define _FP_POST_MUX_IPBM               (1 << 0)
#define _FP_POST_MUX_NAT_DST_REALM_ID   (1 << 1)
#define _FP_POST_MUX_NAT_NEEDED         (1 << 2)
#define _FP_POST_MUX_DROP               (1 << 3)
#define _FP_POST_MUX_SRC_DST_CONT_0     (1 << 4)
#define _FP_POST_MUX_SRC_DST_CONT_1     (1 << 5)

/*
 * Typedef:
 *    _FP_QUAL_SEC_INFO_DECL
 * Purpose:
 *    _FP_QUAL_SEC_INFO_XXX macros require the following declaration in any
 *    function which uses them.
 */
#define _FP_QUAL_SEC_INFO_DECL                       \
    int _rv_;                                        \
    _field_qual_sec_info_t _fp_qual_sec_info_
/*
 * Typedef:
 *    __FP_QUAL_SEC_INFO_ADD
 * Purpose:
 *    Extractors section information add macros.
 */
#define __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_, _ctrl_sel_,      \
                               _ctrl_sel_val_, _sec0_, _sec0_val_, _sec1_,     \
                               _sec1_val_, _sec2_, _sec2_val_, _sec3_,         \
                               _sec3_val_, _sec4_, _sec4_val_, _width0_,       \
                               _width1_, _width2_, _width3_, _width4_,         \
                               _offset0_, _offset1_, _offset2_, _offset3_,     \
                               _offset4_, _num_chunks_)                        \
            do {                                                               \
                _field_qual_sec_info_t_init(&_fp_qual_sec_info_);              \
                (_fp_qual_sec_info_).qid                    = (_qual_id_);     \
                (_fp_qual_sec_info_).ctrl_sel               = (_ctrl_sel_);    \
                (_fp_qual_sec_info_).ctrl_sel_val          = (_ctrl_sel_val_); \
                (_fp_qual_sec_info_).e_params[0].section    = (_sec0_);        \
                (_fp_qual_sec_info_).e_params[0].sec_val    = (_sec0_val_);    \
                (_fp_qual_sec_info_).e_params[0].width      = (_width0_);      \
                (_fp_qual_sec_info_).e_params[0].bus_offset = (_offset0_);     \
                (_fp_qual_sec_info_).e_params[1].section    = (_sec1_);        \
                (_fp_qual_sec_info_).e_params[1].sec_val    = (_sec1_val_);    \
                (_fp_qual_sec_info_).e_params[1].width      = (_width1_);      \
                (_fp_qual_sec_info_).e_params[1].bus_offset = (_offset1_);     \
                (_fp_qual_sec_info_).e_params[2].section    = (_sec2_);        \
                (_fp_qual_sec_info_).e_params[2].sec_val    = (_sec2_val_);    \
                (_fp_qual_sec_info_).e_params[2].width      = (_width2_);      \
                (_fp_qual_sec_info_).e_params[2].bus_offset = (_offset2_);     \
                (_fp_qual_sec_info_).e_params[3].section    = (_sec3_);        \
                (_fp_qual_sec_info_).e_params[3].sec_val    = (_sec3_val_);    \
                (_fp_qual_sec_info_).e_params[3].width      = (_width3_);      \
                (_fp_qual_sec_info_).e_params[3].bus_offset = (_offset3_);     \
                (_fp_qual_sec_info_).e_params[4].section    = (_sec4_);        \
                (_fp_qual_sec_info_).e_params[4].sec_val    = (_sec4_val_);    \
                (_fp_qual_sec_info_).e_params[4].width      = (_width4_);      \
                (_fp_qual_sec_info_).e_params[4].bus_offset = (_offset4_);     \
                (_fp_qual_sec_info_).size = ((_width0_) + (_width1_)           \
                                                + (_width2_) + (_width3_)      \
                                                + (_width4_));                 \
                (_fp_qual_sec_info_).num_chunks = (_num_chunks_);              \
                (_fp_qual_sec_info_).next = NULL;                              \
                (_fp_qual_sec_info_).in_use = 1;                               \
                _rv_ = _bcm_field_th_qual_sec_info_insert((_unit_),            \
                            (_stage_fc_), &(_fp_qual_sec_info_));              \
                BCM_IF_ERROR_RETURN(_rv_);                                     \
            } while(0)

#define _FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_, _sec0_,           \
                              _sec0_val_, _width0_, _offset0_)                 \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _FieldExtCtrlSelDisable, 0, _sec0_,         \
                                   _sec0_val_, 0, 0, 0, 0, 0, 0, 0, 0,         \
                                   _width0_, 0, 0, 0, 0, _offset0_, 0, 0, 0,   \
                                   0, 1)

#define _FP_QUAL_TWO_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_, _sec0_,       \
                                  _sec0_val_, _sec1_, _sec1_val_, _width0_,    \
                                  _width1_, _offset0_, _offset1_)              \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _FieldExtCtrlSelDisable, 0, _sec0_,         \
                                   _sec0_val_, _sec1_, _sec1_val_, 0, 0,       \
                                   0, 0, 0, 0, _width0_, _width1_, 0, 0, 0,    \
                                   _offset0_, _offset1_, 0, 0, 0, 2)

#define _FP_QUAL_THREE_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_, _sec0_,     \
                                    _sec0_val_, _sec1_, _sec1_val_, _sec2_,    \
                                    _sec2_val_, _width0_, _width1_, _width2_,  \
                                    _offset0_, _offset1_,  _offset2_)          \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _FieldExtCtrlSelDisable, 0, _sec0_,         \
                                   _sec0_val_, _sec1_, _sec1_val_, _sec2_,     \
                                   _sec2_val_, 0, 0, 0, 0, _width0_,           \
                                   _width1_, _width2_, 0, 0, _offset0_,        \
                                   _offset1_, _offset2_, 0, 0, 3)
#define _FP_QUAL_FOUR_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_, _sec0_,      \
                                    _sec0_val_, _sec1_, _sec1_val_, _sec2_,    \
                                    _sec2_val_, _sec3_, _sec3_val_, _width0_,  \
                                    _width1_, _width2_, _width3_, _offset0_,   \
                                    _offset1_, _offset2_, _offset3_)           \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _FieldExtCtrlSelDisable, 0, _sec0_,         \
                                   _sec0_val_, _sec1_, _sec1_val_, _sec2_,     \
                                   _sec2_val_, _sec3_, _sec3_val_, 0, 0,       \
                                   _width0_, _width1_, _width2_, _width3_, 0,  \
                                   _offset0_, _offset1_, _offset2_,            \
                                   _offset3_, 0, 4)
#define _FP_QUAL_FIVE_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_, _sec0_,      \
                                   _sec0_val_, _sec1_, _sec1_val_, _sec2_,     \
                                   _sec2_val_, _sec3_, _sec3_val_, _sec4_,     \
                                   _sec4_val_, _width0_, _width1_, _width2_,   \
                                   _width3_, _width4_, _offset0_, _offset1_,   \
                                   _offset2_, _offset3_, _offset4_)            \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _FieldExtCtrlSelDisable, 0, _sec0_,         \
                                   _sec0_val_, _sec1_, _sec1_val_, _sec2_,     \
                                   _sec2_val_, _sec3_, _sec3_val_, _sec4_,     \
                                   _sec4_val_, _width0_, _width1_, _width2_,   \
                                   _width3_, _width4_, _offset0_, _offset1_,   \
                                   _offset2_, _offset3_, _offset4_, 5)

#define _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(_unit_, _stage_fc_, _qual_id_,          \
                                       _ctrl_sel_, _ctrl_sel_val_, _sec0_,     \
                                       _sec0_val_, _width0_, _offset0_)        \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _ctrl_sel_, _ctrl_sel_val_, _sec0_,         \
                                   _sec0_val_, 0, 0, 0, 0, 0, 0, 0, 0,         \
                                   _width0_, 0, 0, 0, 0, _offset0_, 0, 0, 0,   \
                                   0, 1)

#define _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(_unit_, _stage_fc_, _qual_id_,      \
                                           _ctrl_sel_, _ctrl_sel_val_, _sec0_, \
                                           _sec0_val_, _sec1_, _sec1_val_,     \
                                           _width0_,  _width1_, _offset0_,     \
                                           _offset1_)                          \
            __FP_QUAL_SEC_INFO_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _ctrl_sel_, _ctrl_sel_val_, _sec0_,         \
                                   _sec0_val_, _sec1_, _sec1_val_, 0, 0,       \
                                   0, 0, 0, 0, _width0_, _width1_, 0, 0, 0,    \
                                   _offset0_, _offset1_, 0, 0, 0, 2)

/*
 * Typedef:
 *    _FP_EXT_ID_CREATE
 * Purpose:
 *    Extractor ID construction macro.
 */
#define _FP_EXT_ID_CREATE(_part_, _level_, _gran_, _ext_num_, _ext_id_)     \
            (_ext_id_) = (((_part_) << 28) | ((_level_) << 18)              \
                            | ((_gran_) << 10) | (_ext_num_))

/*
 * Typedef:
 *    _FP_EXT_ID_PARSE
 * Purpose:
 *    Parse an Extractor ID and get the extractor level, granularity and
 *    extractor number information.
 */
#define _FP_EXT_ID_PARSE(_ext_id_, _part_, _level_, _gran_, _ext_num_)      \
            (_ext_num_) = ((_ext_id_) & 0x3ff);                             \
            (_gran_) = (((_ext_id_) >> 10) & 0xff);                         \
            (_level_) = (((_ext_id_) >> 18) & 0xf);                         \
            (_part_) = (((_ext_id_) >> 28) & 0x3)

/*
 * Typedef:
 *    _FP_EXT_CONFIG_DECL
 * Purpose:
 *    __FP_EXT_CONFIG_ADD macro needs the following declaration in any function
 *    that uses it.
 */
#define _FP_EXT_CONFIG_DECL         \
    int _rv_;                       \
    _field_ext_cfg_t _fp_ext_cfg_

/*
 * Typedef:
 *    _FP_EXT_CONFIG_ADD
 * Purpose:
 *    Extractor hierarcy construction utility macro.
 */
#define _FP_EXT_CONFIG_ADD(_unit_, _stage_fc_, _emode_, _part_, _level_,       \
                            _gran_, _ext_num_, _in_sec_, _out_sec_, _offset_,  \
                            _qid_, _in_use_, _flags_)                          \
            do {                                                               \
                int _ext_id_ = 0;                                              \
                sal_memset((&_fp_ext_cfg_), 0, sizeof(_field_ext_cfg_t));      \
                _FP_EXT_ID_CREATE(_part_, _level_, _gran_, _ext_num_,          \
                    _ext_id_);                                                 \
                (_fp_ext_cfg_).ext_id = (_ext_id_);                            \
                (_fp_ext_cfg_).gran = (_gran_);                                \
                (_fp_ext_cfg_).ext_num = (_ext_num_);                          \
                (_fp_ext_cfg_).in_sec = (_in_sec_);                            \
                (_fp_ext_cfg_).out_sec = (_out_sec_);                          \
                (_fp_ext_cfg_).bus_offset = (_offset_);                        \
                (_fp_ext_cfg_).flags = (_flags_);                              \
                (_fp_ext_cfg_).in_use = (_in_use_);                            \
                (_fp_ext_cfg_).chunk_extracted = -1;                           \
                _rv_ = _bcm_field_th_ext_config_insert((_unit_), (_stage_fc_), \
                            _emode_, _level_, &(_fp_ext_cfg_));                \
                BCM_IF_ERROR_RETURN(_rv_);                                     \
            } while(0)

/* Max Preselector Entries Per Group */
#define _FP_PRESEL_ENTRIES_MAX_PER_GROUP  4

/*
 * Typedef:
 *      _field_class_table_bmp_t
 * Purpose:
 *      Class table bitmap for tracking allocation.
 *
 */
typedef struct _field_class_table_bmp_s {
    SHR_BITDCL *w;
} _field_class_table_bmp_t;

#define _FP_CLASS_TABLE_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_CLASS_TABLE_BMP_ADD(bmp, cl)     SHR_BITSET(((bmp).w), (cl))
#define _FP_CLASS_TABLE_BMP_REMOVE(bmp, cl)  SHR_BITCLR(((bmp).w), (cl))
#define _FP_CLASS_TABLE_BMP_TEST(bmp, cl)    SHR_BITGET(((bmp).w), (cl))

/*
 * Typedef:
 *     _field_class_type_t
 * Purpose:
 *      Field Class Types.
 */
typedef enum _field_class_type_e {
    _FieldClassEtherType = 0,
    _FieldClassTtl = 1,
    _FieldClassToS = 2,
    _FieldClassIpProto = 3,
    _FieldClassL4SrcPort = 4,
    _FieldClassL4DstPort = 5,
    _FieldClassTcp = 6,
    _FieldClassSrcCompression = 7,
    _FieldClassDstCompression = 8,
    _FieldClassCount = 9
} _field_class_type_t;

#define _BCM_FIELD_CLASS_TYPE_STRINGS \
{"EtherType",       \
 "TTL",             \
 "TOS",             \
 "IPProtocol",      \
 "L4SrcPort",       \
 "L4DstPort",       \
 "TCP",             \
 "SrcCompression",  \
 "DstCompression"}

/*
 * Typedef:
 *    _field_class_info_s {
 * Purpose:
 *    Field Class Table information.
 */
typedef struct _field_class_info_s {
    uint32 flags;
    _field_class_type_t classtype;
    uint32 total_entries_used;
    uint32 total_entries_available;
    _field_class_table_bmp_t class_bmp;
} _field_class_info_t;

/* Maximum number of FP Class tables. */
#define _FP_MAX_NUM_CLASS_TYPES                  _FieldClassCount

/* Total number of entries in class table */
#define _FP_MAX_NUM_CLASS_TTL                    soc_mem_index_count(unit,TTL_FNm)
#define _FP_MAX_NUM_CLASS_TOS                    soc_mem_index_count(unit,TOS_FNm)
#define _FP_MAX_NUM_CLASS_IP_PROTO               soc_mem_index_count(unit,IP_PROTO_MAPm)
#define _FP_MAX_NUM_CLASS_TCP                    soc_mem_index_count(unit,TCP_FNm)
#define _FP_MAX_NUM_CLASS_SRC_COMPRESSION        soc_mem_index_count(unit,SRC_COMPRESSIONm)
#define _FP_MAX_NUM_CLASS_DST_COMPRESSION        soc_mem_index_count(unit,DST_COMPRESSIONm)
#define _FP_MAX_NUM_CLASS_ETHERTYPE              16
#define _FP_MAX_NUM_CLASS_L4_SRC_PORT            16
#define _FP_MAX_NUM_CLASS_L4_DST_PORT            16

/* Class Entry Size */
#define _FP_SIZE_CLASS_TTL                  4
#define _FP_SIZE_CLASS_TOS                  4
#define _FP_SIZE_CLASS_IP_PROTO             4
#define _FP_SIZE_CLASS_TCP                  4
#define _FP_SIZE_CLASS_SRC_COMPRESSION      44
#define _FP_SIZE_CLASS_DST_COMPRESSION      44
#define _FP_SIZE_CLASS_ETHERTYPE            4
#define _FP_SIZE_CLASS_L4_SRC_PORT          4
#define _FP_SIZE_CLASS_L4_DST_PORT          4

/* Class Size in Bits*/
#define _FP_QUAL_CLASS_SIZE_TTL                  8
#define _FP_QUAL_CLASS_SIZE_TOS                  8
#define _FP_QUAL_CLASS_SIZE_IP_PROTO             4
#define _FP_QUAL_CLASS_SIZE_TCP                  8
#define _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION      36
#define _FP_QUAL_CLASS_SIZE_DST_COMPRESSION      36
#define _FP_QUAL_CLASS_SIZE_ETHERTYPE            4
#define _FP_QUAL_CLASS_SIZE_L4_SRC_PORT          4
#define _FP_QUAL_CLASS_SIZE_L4_DST_PORT          4

/* L4 Src/Dst Port Match Types */
#define _FP_MATCH_L4_PORT_TYPE              0
#define _FP_MATCH_EXCHANGE_ID_TYPE          1

/* Src/Dst Compression Key Type */
#define _FP_KEY_RESERVED_TYPE               0
#define _FP_KEY_IPv4_TYPE                   1
#define _FP_KEY_IPv6_TYPE                   2
#define _FP_KEY_FCoE_TYPE                   3

/* Src/Dst Compression Key Type Mask */
#define _FP_KEY_TYPE_MASK                   3

#endif /* !BCM_TOMAHAWK_SUPPORT */

/*
 * Typedef:
 *     _field_stage_t
 * Purpose:
 *     Pipeline stage field processor information.
 */
typedef struct _field_stage_s {
    _field_stage_id_t      stage_id;        /* Pipeline stage id.           */
    uint32                 flags;           /* Stage flags.                 */
    int                    tcam_sz;         /* Number of entries in TCAM.   */
    int                    tcam_slices;     /* Number of internal slices.   */
    int                    num_instances;   /* Number of active FP instances. */
    int                    num_pipes;       /* Number of pipelines in a stage */
    struct _field_slice_s  *slices[_FP_MAX_NUM_PIPES]; /* Array of slices.*/
    struct _field_range_s  *ranges;         /* List of all ranges allocated.*/
    uint32                 range_id;        /* Seed ID for range creation.  */
                                        /* Virtual map for slice extension */
                                        /* and group priority management.  */
    _field_virtual_map_t   vmap[_FP_VMAP_CNT][_FP_VMAP_SIZE];

    /* FPF tables */
    _qual_info_t *_field_table_fpf0[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf1[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf2[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf3[FPF_SZ_MAX];
    _qual_info_t *_field_table_fpf4[FPF_SZ_MAX];
    _qual_info_t *_field_table_extn[FPF_SZ_MAX];
    _qual_info_t *_field_table_doublewide_fpf[FPF_SZ_MAX];
    _qual_info_t *_field_table_fixed[1];

    bcm_field_qset_t *_field_sel_f0;
    bcm_field_qset_t *_field_sel_f1;
    bcm_field_qset_t *_field_sel_f2;
    bcm_field_qset_t *_field_sel_f3;
    bcm_field_qset_t *_field_sel_f4;
    bcm_field_qset_t *_field_sel_extn;
    bcm_field_qset_t *_field_sel_doublewide;
    bcm_field_qset_t *_field_sel_fixed;
    bcm_field_qset_t _field_supported_qset;
    _bcm_field_qual_info_t  **f_qual_arr;  /* Stage qualifiers config array. */
    _bcm_field_qual_info_t  **f_presel_qual_arr; /* Presel qualifiers config array. */
    _bcm_field_action_conf_t **f_action_arr;

    int counter_collect_table; /* Used for counter collection in chunks */
    int counter_collect_index; /* Used for counter collection in chunks */

    int num_meter_pools;
    _field_meter_pool_t *meter_pool[_FP_MAX_NUM_PIPES][_FIELD_MAX_METER_POOLS];
#ifdef BCM_TRIUMPH3_SUPPORT
    int num_logical_meter_pools;
    _field_meter_pool_t *logical_meter_pool[_FIELD_MAX_METER_POOLS];
#endif
    unsigned num_cntr_pools;
    _field_cntr_pool_t *cntr_pool[_FIELD_MAX_CNTR_POOLS];

    _field_counter32_collect_t *_field_x32_counters;
                              /* X pipeline 32 bit counter collect      */
#if defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
    _field_counter32_collect_t *_field_ext_counters;
                              /* External counter collect */
#endif /* BCM_EASYRIDER_SUPPORT || BCM_TRIUMPH_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    _field_counter64_collect_t *_field_x64_counters;
                              /* X pipeline 64 bit counter collect */
    _field_counter64_collect_t *_field_y64_counters;
                              /* Y pipeline 64 bit counter collect  */
#endif /* BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
    _field_counter32_collect_t *_field_y32_counters;
                              /* Y pipeline packet counter collect */
#endif /* BCM_SCORPION_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
    _field_counter32_collect_t  *_field_32_counters[_FP_MAX_NUM_PIPES];
    _field_counter64_collect_t  *_field_64_counters[_FP_MAX_NUM_PIPES];
    int                          lt_tcam_sz;     /* Logical table TCAM size.  */
    int                          num_logical_tables;
                                                 /* Number of logical tables. */
    bcm_field_qset_t             presel_qset;
                                                 /* Preselector Qualifiers Set*/
    _field_lt_slice_t            *lt_slices[_FP_MAX_NUM_PIPES];
                                                 /* Array of LT slices.       */
    _field_keygen_profiles_t     keygen_profile[_FP_MAX_NUM_PIPES];
                                                 /* Keygen Profiles.          */
    _field_lt_config_t           *lt_info[_FP_MAX_NUM_PIPES][_FP_MAX_NUM_LT];
                                                 /* Logical table info.       */
    uint32                       lt_action_pri;  /* Logical Action Priority.  */ 
    int                          num_ext_levels; /* No. of extractor leves.   */
    _field_ext_info_t            **ext_cfg_arr;  /* Extractor config info.    */
    _field_class_info_t          **class_info_arr[_FP_MAX_NUM_PIPES];
                                                 /* Class Status Array. */
    _field_bus_info_t            input_bus;      /* Input bus.                */
#endif /* BCM_TOMAHAWK_SUPPORT */

    soc_memacc_t               *_field_memacc_counters;
                              /* Memory access info for FP counter fields */

    soc_profile_mem_t redirect_profile;     /* Redirect action memory profile.*/
#if defined(BCM_TRIDENT2_SUPPORT)
    soc_profile_mem_t hash_select[2];    /* Hash select action memory profile */
#endif
    soc_profile_mem_t ext_act_profile;      /* Action profile for external. */
    _field_data_control_t *data_ctrl;       /* Data qualifiers control. */
    bcm_field_group_oper_mode_t  oper_mode; /* Group Operational Mode */
    struct _field_stage_s *next;            /* Next pipeline FP stage. */
} _field_stage_t;

/* Indexes into the memory access list for FP counter acceleration */
typedef enum _field_counters_memacc_type_e {
    _FIELD_COUNTER_MEMACC_BYTE,
    _FIELD_COUNTER_MEMACC_PACKET,
    _FIELD_COUNTER_MEMACC_BYTE_Y,
    _FIELD_COUNTER_MEMACC_PACKET_Y,
    _FIELD_COUNTER_MEMACC_NUM,       /* The max size of the memacc list */
    /* If the device doesn't have packet and byte counters in a single
     * counter mem entry, then these aliases are used. */
    _FIELD_COUNTER_MEMACC_COUNTER = _FIELD_COUNTER_MEMACC_BYTE,
    _FIELD_COUNTER_MEMACC_COUNTER_Y = _FIELD_COUNTER_MEMACC_PACKET
} _field_counters_memacc_type_t;

/* Indexes into the memory access list for FP counter acceleration */
typedef enum _field_counters_multi_pipe_memacc_type_e {
    _FIELD_COUNTER_MEMACC_PIPE0_BYTE   = 0,
    _FIELD_COUNTER_MEMACC_PIPE0_PACKET = 1,
    _FIELD_COUNTER_MEMACC_PIPE1_BYTE   = 2,
    _FIELD_COUNTER_MEMACC_PIPE1_PACKET = 3,
    _FIELD_COUNTER_MEMACC_PIPE2_BYTE   = 4,
    _FIELD_COUNTER_MEMACC_PIPE2_PACKET = 5,
    _FIELD_COUNTER_MEMACC_PIPE3_BYTE   = 6,
    _FIELD_COUNTER_MEMACC_PIPE3_PACKET = 7,
    _FIELD_COUNTER_MEMACC_PIPE_NUM     = 8 /* The max size of the
                                               memacc list */
} _field_counters_multi_pipe_memacc_type_t;

#define _FIELD_FIRST_MEMORY_COUNTERS                 (1 << 0)
#define _FIELD_SECOND_MEMORY_COUNTERS                (1 << 1)
#define _FIELD_MULTI_PIPE_MEMORY_COUNTERS            (1 << 3)

#define FILL_FPF_TABLE(_fpf_info, _qid, _offset, _width, _code)        \
    BCM_IF_ERROR_RETURN                                                \
        (_field_qual_add((_fpf_info), (_qid), (_offset), (_width), (_code)))

/* Generic memory allocation routine. */
#define _FP_XGS3_ALLOC(_ptr_,_size_,_descr_)                 \
            do {                                             \
                if (NULL == (_ptr_)) {                       \
                   (_ptr_) = sal_alloc((_size_), (_descr_)); \
                }                                            \
                if((_ptr_) != NULL) {                        \
                    sal_memset((_ptr_), 0, (_size_));        \
                }  else {                                    \
                    LOG_ERROR(BSL_LS_BCM_FP, \
                              (BSL_META("FP Error: Allocation failure %s\n"), (_descr_))); \
                }                                          \
            } while (0)

/*
 * Typedef:
 *     _field_udf_t
 * Purpose:
 *     Holds user-defined field (UDF) hardware metadata.
 */
typedef struct _field_udf_s {
    uint8                  valid;     /* Indicates valid UDF             */
    int                    use_count; /* Number of groups using this UDF */
    bcm_field_qualify_t    udf_num;   /* UDFn (UDF0 or UDF1)             */
    uint8                  user_num;  /* Which user field in UDFn        */
} _field_udf_t;


/*
 * Typedef:
 *     _field_tcam_int_t
 * Purpose:
 *     These are the fields that are written into or read from FP_TCAM_xxx.
 */
typedef struct _field_tcam_s {
    uint32                 *key;
    uint32                 *key_hw;  /* Hardware replica */
    uint32                 *mask;
    uint32                 *mask_hw; /* Hardware replica */
    uint16                 key_size;
    uint32                 f4;
    uint32                 f4_mask;
    uint8                  higig;         /* 0/1 non-HiGig/HiGig    */
    uint8                  higig_mask;    /* 0/1 non-HiGig/HiGig    */
     uint8                 ip_type;
#if defined(BCM_FIREBOLT2_SUPPORT)
    uint8                  drop;         /* 0/1 don't Drop/Drop     */
    uint8                  drop_mask;    /* 0/1 don't Drop/Drop     */
#endif /* BCM_FIREBOLT2_SUPPORT */
} _field_tcam_t;

/*
 * Typedef:
 *     _field_tcam_mem_info_t
 * Purpose:
 *     TCAM memory name and chip specific field names.
 */
typedef struct _field_tcam_mem_info_s{
    soc_mem_t      memory;     /* Tcam memory name.    */
    soc_field_t    key_field;  /* Tcam key field name. */
    soc_field_t    mask_field; /* Tcam mask field name.*/
} _field_tcam_mem_info_t;

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
typedef struct _field_pbmp_s {
    bcm_pbmp_t data;
    bcm_pbmp_t mask;
} _field_pbmp_t;
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

/*
 * Typedef:
 *     _field_counter_t
 * Purpose:
 *     Holds the counter parameters to be written into FP_POLICY_TABLE
 *     (Firebolt) or FP_INTERNAL (Easyrider).
 */
typedef struct _field_counter_s {
    int                    index;
    uint16                 entries;    /* Number of entries using counter */
} _field_counter_t;

/*
 * Typedef:
 *     _field_counter_bmp_t
 * Purpose:
 *     Counter bit map for tracking allocation state of slice's counter pairs.
 */
typedef struct _field_counter_bmp_s {
    SHR_BITDCL  *w;
} _field_counter_bmp_t;

#define _FP_COUNTER_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_COUNTER_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_COUNTER_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_COUNTER_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))

/* Max number of counter pools. Tomahawk has 20 Flex counter pools which is 
 * maximum among all devices. But only 16 are exposed for now.
 */
#define _FIELD_MAX_COUNTER_POOLS 16 

/* per Group Counter Pool Bitmap */
typedef struct _field_counter_pool_bmp_s {
    SHR_BITDCL w[_FIELD_MAX_COUNTER_POOLS];
} _field_counter_pool_bmp_t;

#define _FP_COUNTER_POOL_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_COUNTER_POOL_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_COUNTER_POOL_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))


/*
 * Structure for priority management
 * Currently used only on External TCAM
 */
typedef struct _field_prio_mgmt_s {
    int prio;
    uint32 start_index;
    uint32 end_index;
    uint32 num_free_entries;
    struct _field_prio_mgmt_s *prev;
    struct _field_prio_mgmt_s *next;
} _field_prio_mgmt_t;

/* Slice specific flags. */
#define _BCM_FIELD_SLICE_EXTERNAL              (1 << 0)
#define _BCM_FIELD_SLICE_INTRASLICE_CAPABLE    (1 << 1)
#define _BCM_FIELD_SLICE_SIZE_SMALL            (1 << 2)
#define _BCM_FIELD_SLICE_SIZE_LARGE            (1 << 3)
#define _BCM_FIELD_SLICE_IPBM_CAPABLE          (1 << 4)
/* Indicates whether the slice is operationally Enabled */
#define _BCM_FIELD_SLICE_HW_ENABLE             (1 << 7)
/* configure the flag to provide the SW atomicity for a slice. */
#define _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT  (1 << 8)  
                                                            

#define _FP_INTRA_SLICE_PART_0        (0)
#define _FP_INTRA_SLICE_PART_1        (1)
#define _FP_INTRA_SLICE_PART_2        (2)
#define _FP_INTRA_SLICE_PART_3        (3)

#define _FP_INTER_SLICE_PART_0        (0)
#define _FP_INTER_SLICE_PART_1        (1)
#define _FP_INTER_SLICE_PART_2        (2)

/*
 * Typedef:
 *     _field_slice_t
 * Purpose:
 *     This has the fields specific to a hardware slice.
 * Notes:
 */
typedef struct _field_slice_s {
    uint8                  slice_number;  /* Hardware slice number.         */
    int                    start_tcam_idx;/* Slice first entry tcam index.  */
    int                    entry_count;   /* Number of entries in the slice.*/
    int                    free_count;    /* Number of free entries.        */
    int                    counters_count;/* Number of counters accessible. */
    int                    meters_count;  /* Number of meters accessible.   */
    int                    hw_ent_count;  /* Number of entries installed
                                             in the slice.                  */
    _field_counter_bmp_t   counter_bmp;   /* Bitmap for counter allocation. */
    _field_meter_bmp_t     meter_bmp;     /* Bitmap for meter allocation.   */
    _field_stage_id_t      stage_id;      /* Pipeline stage slice belongs.  */
    bcm_pbmp_t             pbmp;          /* Ports in use by groups.        */

    struct _field_entry_s  **entries;     /* List of entries pointers.      */
    _field_prio_mgmt_t     *prio_mgmt;    /* Priority management of entries.*/

    uint8 pkt_type[_FP_EXT_NUM_PKT_TYPES];/* Packet types supported
                                             by this slice (aka database).  */

    bcm_pbmp_t ext_pbmp[_FP_EXT_NUM_PKT_TYPES];/* Bmap for each packet type.*/

    struct _field_slice_s  *next;  /* Linked list for auto-expand of groups.*/
    struct _field_slice_s  *prev;  /* Linked list for auto-expand of groups.*/
    uint8                  slice_flags;   /* _BCM_FIELD_SLICE_XXX flags.    */
    uint8                  group_flags;   /* Intalled groups _FP_XXX_GROUP. */
    int8                   doublewide_key_select;
                                          /* Key selection for the          */
                                          /* intraslice double wide mode.   */
    int8                   src_class_sel; /* Source lookup class selection.*/
    int8                   dst_class_sel; /* Destination lookup class.     */
    int8                   intf_class_sel;/* Interface class selection.    */
    int8                   loopback_type_sel;/* Loopback/Tunnel selection.  */
    int8                   ingress_entity_sel;/* Ingress entity selection.  */
    int8                   src_entity_sel;    /* Src port/trunk entity selection.          */
    int8                   dst_fwd_entity_sel;/* Destination forwarding     */
    int8                   fwd_field_sel; /* Forwarding field vrf/vpn/vid   */
                                              /* entity selection.          */
    int8                   aux_tag_1_sel;
    int8                   aux_tag_2_sel;
    int8                   oam_overlay_sel;   /* Oam Overlay Selection*/
    int8                   egr_class_f1_sel;
    int8                   egr_class_f2_sel;
    int8                   egr_class_f3_sel;
    int8                   egr_class_f4_sel;
    int8                   src_dest_class_f1_sel;
    int8                   src_dest_class_f3_sel;
    int8                   egr_key4_dvp_sel;
    int8                   egr_key4_mdl_sel;
    int8                   egr_oam_overlay_sel;   /* Egress Oam Overlay Selection*/
    int8                   oam_intf_class_sel;   /* Oam SVP Class Id Selection*/
#if defined(BCM_TOMAHAWK_SUPPORT)
    uint32                 lt_map;        /* Bitmap of LT IDs on this slice. */
    uint8                  lt_partition_pri;  /* LT Partition Priority. */ 
    _field_ext_sel_t       ext_sel[_FP_MAX_NUM_LT];
#endif
} _field_slice_t;


/* Macro: _BCM_FIELD_SLICE_SIZE
 * Purpose:
 *        Given stage, slice get number of entries in the slice.
 */
#define _BCM_FIELD_SLICE_SIZE(_stage_fc_, instance, _slice_)     \
       (((_stage_fc_)->slices[instance] + (_slice_))->entry_count)


#define _FP_GROUP_ENTRY_ARR_BLOCK (0xff)
typedef struct _field_entry_s _field_entry_t;
/*
 * Typedef:
 *     _field_group_t
 * Purpose:
 *     This is the logical group's internal storage structure. It has 1, 2 or
 *     3 slices, each with physical entry structures.
 * Notes:
 *   'ent_qset' should always be a subset of 'qset'.
 */
typedef struct _field_group_s {
    bcm_field_group_t      gid;            /* Opaque handle.                */
    int                    priority;       /* Field group priority.         */
    bcm_field_qset_t       qset;           /* This group's Qualifier Set.   */
    uint16                 flags;          /* Group configuration flags.    */
    _field_slice_t         *slices;        /* Pointer into slice array.     */
    bcm_pbmp_t             pbmp;           /* Ports in use this group.      */
    _field_sel_t           sel_codes[_FP_MAX_ENTRY_WIDTH]; /* Select codes for
                                                              slice(s).     */
    _bcm_field_group_qual_t qual_arr[_FP_MAX_ENTRY_TYPES][_FP_MAX_ENTRY_WIDTH];
                                           /* Qualifiers available in each
                                              individual entry part.        */
    _field_stage_id_t      stage_id;       /* FP pipeline stage id.         */

    _field_entry_t         **entry_arr;    /* FP group entry array.         */
    uint16                 ent_block_count;/* FP group entry array size  .  */
    /*
     * Public data for each group: The number of used and available entries,
     * counters, and meters for a field group.
     */
    bcm_field_group_status_t group_status;
    bcm_field_aset_t         aset;
    _field_counter_pool_bmp_t counter_pool_bmp; /* counter pools in use for
                                                 * this group.
                                                 */
    int                    instance;       /* FP Instance.                   */
#if defined(BCM_TOMAHAWK_SUPPORT)
    _field_ext_sel_t       ext_codes[_FP_MAX_ENTRY_WIDTH]; /* KeyGen select
                                                              codes. */
    _field_lt_slice_t      *lt_slices;     /* Pointer into LT slices arrary. */
    _field_lt_config_t     *lt_info;       /* Pointer into LT ID config.     */
    _field_lt_entry_t      **lt_entry_arr; /* FP Group LT entry array.       */
    uint16                 lt_ent_blk_cnt; /* FP group LT entry array size.  */
    _field_group_lt_status_t lt_grp_status;/* FP Group LT status.            */
    int                     qset_size;     /* Qset key size.                 */
    _bcm_field_group_qual_t presel_qual_arr[_FP_MAX_ENTRY_TYPES][_FP_MAX_ENTRY_WIDTH];
                                           /* Preselector Qualifiers available
                                            * in each individual presel entry part. */
    struct _field_presel_entry_s *presel_ent_arr[_FP_PRESEL_ENTRIES_MAX_PER_GROUP];
                                     /* Preselector entries associated with
                                        the group arranged on priority basis. */
#endif /* BCM_TOMAHAWK_SUPPORT */
    bcm_field_hintid_t       hintid;         /* Hintid linked to a group */
    int                      action_res_id;  /* Action resolution id */
    int                      vmap_group[_FP_PAIR_MAX];
    uint32                   max_group_size; /* Max Size of a group */
    struct _field_group_s   *next;         /* For storing in a linked-list  */
} _field_group_t;

/*
 * Typedef:
 *     _field_action_t
 * Purpose:
 *     This is the real action storage structure that is hidden behind
 *     the opaque handle bcm_field_action_t.
 */
typedef struct _field_action_s {
    bcm_field_action_t     action;       /* Action type                  */
    uint32                 param[_FP_ACTION_PARAM_SZ];
                                         /* Action specific parameters   */
    int                    hw_index;     /* Allocated hw resource index. */
    int                    old_index;    /* Hw resource to be freed, in  */
                                         /* case action parameters were  */
                                         /* modified.                    */
    uint8                  flags;        /* Field action flags.          */
    struct _field_action_s *next;
} _field_action_t;

#define _FP_RANGE_STYLE_FIREBOLT    1


#define PolicyMax(_unit_, _mem_, _field_)                                 \
    ((soc_mem_field_length((_unit_), (_mem_) , (_field_)) < 32) ?         \
        ((1 << soc_mem_field_length((_unit_), (_mem_), (_field_))) - 1) : \
        (0xFFFFFFFFUL))

#define PolicyGet(_unit_, _mem_, _entry_, _field_) \
    soc_mem_field32_get((_unit_), (_mem_), (_entry_), (_field_))

#define PolicySet(_unit_, _mem_, _entry_, _field_, _value_)    \
    soc_mem_field32_set((_unit_), (_mem_), (_entry_), (_field_), (_value_))

#define PolicyCheck(_unit_, _mem_, _field_, _value_)                      \
    if (0 == ((uint32)(_value_) <=                                        \
              (uint32)PolicyMax((_unit_), (_mem_), (_field_)))) {         \
        LOG_ERROR(BSL_LS_BCM_FP,                                        \
                  (BSL_META("FP(unit %d) Error: Policy _value_ %d > %d (max) mem (%d)" \
                        "field (%d).\n"), _unit_, (_value_),            \
                (uint32)PolicyMax((_unit_), (_mem_), (_field_)), (_mem_), \
                (_field_)));                                              \
        return (BCM_E_PARAM);                                             \
    }

/*
 * Typedef:
 *     _field_range_t
 * Purpose:
 *     Internal management of Range Checkers. There are two styles or range
 *     checkers, the Firebolt style that only chooses source or destination
 *     port, without any sense of TCP vs. UDP or inverting. This style writes
 *     into the FP_RANGE_CHECK table. The Easyrider style is able to specify
 *     TCP vs. UDP.
 *     The choice of styles is based on the user supplied flags.
 *     If a Firebolt style checker is sufficient, that will be used. If an
 *     Easyrider style checker is required then that will be used.
 *
 */
typedef struct _field_range_s {
    uint32                 flags;
    bcm_field_range_t      rid;
    bcm_l4_port_t          min, max;
    int                    hw_index;
    uint8                  style;        /* Simple (FB) or more complex (ER) */
    struct _field_range_s *next;
} _field_range_t;

/*
 * Entry status flags.
 */

/* Software entry differs from one in hardware. */
#define _FP_ENTRY_DIRTY                      (1 << 0)

/* Entry is in primary slice. */
#define _FP_ENTRY_PRIMARY                    (1 << 1)

/* Entry is in secondary slice of wide-mode group. */
#define _FP_ENTRY_SECONDARY                  (1 << 2)

/* Entry is in tertiary slice of wide-mode group. */
#define _FP_ENTRY_TERTIARY                   (1 << 3)

/* Entry has an ingress Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_ING0                   (1 << 4)

/* Entry has an ingress 1 Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_ING1                   (1 << 5)

/* Entry has an egress Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_EGR0                   (1 << 6)

/* Entry has an egress 1 Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_EGR1                   (1 << 7)

/* Second part of double wide intraslice entry. */
#define _FP_ENTRY_SECOND_HALF                (1 << 8)

/* Field entry installed in hw. */
#define _FP_ENTRY_INSTALLED                  (1 << 9)

/* Treat all packets as green. */
#define _FP_ENTRY_COLOR_INDEPENDENT          (1 << 10)

/* Meter installed in secondary slice . */
#define _FP_ENTRY_POLICER_IN_SECONDARY_SLICE   (1 << 11)

/* Counter installed in secondary slice . */
#define _FP_ENTRY_STAT_IN_SECONDARY_SLICE    (1 << 12)

/* Allocate meters/counters from secondary slice. */
#define _FP_ENTRY_ALLOC_FROM_SECONDARY_SLICE (1 << 13)

/* Entry uses secondary overlay */
#define _FP_ENTRY_USES_IPBM_OVERLAY (1 << 14)

/* Entry action dirty */
#define _FP_ENTRY_ACTION_ONLY_DIRTY (1 << 15)

/* Field entry enabled in hw. */
#define _FP_ENTRY_ENABLED                    (1 << 16)


/* Entry slice identification flags. */
#define _FP_ENTRY_SLICE_FLAGS (_FP_ENTRY_PRIMARY |  _FP_ENTRY_SECONDARY | \
                               _FP_ENTRY_TERTIARY)

/* Entry slice identification flags. */
#define _FP_ENTRY_MIRROR_ON  (_FP_ENTRY_MTP_ING0 |  _FP_ENTRY_MTP_ING1 | \
                               _FP_ENTRY_MTP_EGR0 | _FP_ENTRY_MTP_EGR1)

/* There is no action enum for Oam Stat Actions.
 * Oam Stat Actions conflict with NAT actions.
 * Using Flags to check conflict of actions.
 */
#define _FP_ENTRY_OAM_STAT_VALID            (1 << 17)
#define _FP_ENTRY_OAM_STAT_NOT_ALLOWED      (1 << 18)

/*
 * Group status flags.
 */

/* Group resides in a single slice. */
#define _FP_GROUP_SPAN_SINGLE_SLICE          (1 << 0)

/* Group resides in a two paired slices. */
#define _FP_GROUP_SPAN_DOUBLE_SLICE          (1 << 1)

/* Group resides in three paired slices. */
#define _FP_GROUP_SPAN_TRIPLE_SLICE          (1 << 2)

/* Group entries are double wide in each slice. */
#define _FP_GROUP_INTRASLICE_DOUBLEWIDE      (1 << 3)

/* Group span flags mask. */
#define _FP_GROUP_SPAN_MASK                  (0xF)

/*
 * Group has slice lookup enabled
 *     This is default, unless it is disabled by call to
 *     bcm_field_group_enable_set with enable=0
 */
#define _FP_GROUP_LOOKUP_ENABLED             (1 << 4)

/* Group for WLAN tunnel terminated packets. */
#define _FP_GROUP_WLAN                       (1 << 5)

/* Group resides on the smaller slice */
#define _FP_GROUP_SELECT_SMALL_SLICE         (1 << 6)

/* Group resides on the larger slice */
#define _FP_GROUP_SELECT_LARGE_SLICE         (1 << 7)

/* Group supports auto expansion */
#define _FP_GROUP_SELECT_AUTO_EXPANSION      (1 << 8)

/* Flags to set auto expansion slice preference */
#define _FP_GROUP_AUTO_EXPAND_SMALL_SLICE    (1 << 9)
#define _FP_GROUP_AUTO_EXPAND_LARGE_SLICE    (1 << 10)

/* Max Group Size Hard Limit */
#define _FP_GROUP_MAX_SIZE_HARD_LIMIT        (1 << 11)

/* Group supports Preselector */
#define _FP_GROUP_PRESELECTOR_SUPPORT        (1 << 12)

#define _FP_GROUP_STATUS_MASK        (_FP_GROUP_SPAN_SINGLE_SLICE | \
                                      _FP_GROUP_SPAN_DOUBLE_SLICE | \
                                      _FP_GROUP_SPAN_TRIPLE_SLICE | \
                                      _FP_GROUP_INTRASLICE_DOUBLEWIDE | \
                                      _FP_GROUP_WLAN)

/* Internal DATA qualifiers. */
typedef enum _bcm_field_internal_qualify_e {
    _bcmFieldQualifyData0 = bcmFieldQualifyCount,/* [0x00] Data qualifier 0.  */
    _bcmFieldQualifyData1,                    /* [0x01] Data qualifier 1.     */
    _bcmFieldQualifyData2,                    /* [0x02] Data qualifier 2.     */
    _bcmFieldQualifyData3,                    /* [0x03] Data qualifier 3.     */
    _bcmFieldQualifySvpValid,                 /* [0x04] SVP valid             */
    _bcmFieldQualifyPreLogicalTableId,        /* [0x05] Preselector LT ID.    */
    _bcmFieldQualifyData4,                    /* [0x06] Data qualifier 4.     */
    _bcmFieldQualifyData5,                    /* [0x07] Data qualifier 5.     */
    _bcmFieldQualifyData6,                    /* [0x08] Data qualifier 6.     */
    _bcmFieldQualifyData7,                    /* [0x09] Data qualifier 7.     */
    _bcmFieldQualifyData8,                    /* [0x0a] Data qualifier 8.     */
    _bcmFieldQualifyData9,                    /* [0x0b] Data qualifier 9.     */
    _bcmFieldQualifyRangeCheckBits24_31,      /* [0x0c] Upper 8 bits of the Range Check qualifier  */
    _bcmFieldQualifyCount                     /* [0x0d] Always last not used. */
} _bcm_field_internal_qualify_t;


/* Committed portion in sw doesn't match hw. */
#define _FP_POLICER_COMMITTED_DIRTY     (0x80000000)

/* Peak portion in sw doesn't match hw. */
#define _FP_POLICER_PEAK_DIRTY          (0x40000000)

/* Policer created through meter APIs.  */
#define _FP_POLICER_INTERNAL            (0x20000000)

/* Policer using excess meter. */
#define _FP_POLICER_EXCESS_METER        (0x10000000)

#define _FP_POLICER_DIRTY             (_FP_POLICER_COMMITTED_DIRTY | \
                                       _FP_POLICER_PEAK_DIRTY)

#define _FP_POLICER_LEVEL_COUNT       (2)

/* Flow mode policer using committed meter in hardware. */
#define _FP_POLICER_COMMITTED_HW_METER(f_pl)                \
        (bcmPolicerModeCommitted == (f_pl)->cfg.mode        \
         && !((f_pl)->hw_flags & _FP_POLICER_EXCESS_METER))

/* Flow mode policer using excess meter in hardware. */
#define _FP_POLICER_EXCESS_HW_METER(f_pl)                   \
        (bcmPolicerModeCommitted == (f_pl)->cfg.mode        \
         && ((f_pl)->hw_flags & _FP_POLICER_EXCESS_METER))

/* Set excess meter bit. */
#define _FP_POLICER_EXCESS_HW_METER_SET(f_pl)               \
        ((f_pl)->hw_flags |= _FP_POLICER_EXCESS_METER)

/* Clear excess meter bit. */
#define _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl)             \
        ((f_pl)->hw_flags &= ~_FP_POLICER_EXCESS_METER)

/* Check for Flow mode policer. */
#define _FP_POLICER_IS_FLOW_MODE(f_pl)                      \
        (bcmPolicerModeCommitted == (f_pl)->cfg.mode)

/*
 * Typedef:
 *     _field_policer_t
 * Purpose:
 *     This is the policer description structure.
 *     Indexed by bcm_policer_t handle.
 */
typedef struct _field_policer_s {
    bcm_policer_t        pid;         /* Unique policer identifier.       */
    bcm_policer_config_t cfg;         /* API level policer configuration. */
    uint16               sw_ref_count;/* SW object use reference count.   */
    uint16               hw_ref_count;/* HW object use reference count.   */
    uint8                level;       /* Policer attachment level.        */
    int8                 pool_index;
                             /* Physical Meter pool/slice policer resides.*/
#ifdef    BCM_TRIUMPH3_SUPPORT
    int8                 logical_pool_index;
                              /* Logical Meter pool/slice policer resides.*/
#endif
    int                  hw_index;    /* HW index policer resides.        */
    uint32               hw_flags;    /* HW installation status flags.    */
    _field_stage_id_t    stage_id;    /* Attached entry stage id.         */
    struct _field_policer_s *next;    /* Policer lookup linked list.      */
}_field_policer_t;


#define _FP_POLICER_VALID                (1 << 0)
#define _FP_POLICER_INSTALLED            (1 << 1)
#define _FP_POLICER_DETACH               (1 << 2)

/*
 * Typedef:
 *     _field_entry_policer_t
 * Purpose:
 *     This is field entry policers description structure.
 *     Used to form an array for currently attached policers.
 */
typedef struct _field_entry_policer_s {
    bcm_policer_t  pid;         /* Unique policer identifier. */
    uint8          flags;       /* Policer/entry flags.       */
}_field_entry_policer_t;

/* _bcm_field_stat_e  - Internal counter types. */
typedef enum _bcm_field_stat_e {
    _bcmFieldStatCount = bcmFieldStatAcceptedBytes /* Internal STAT count
                                                      for XGS devices. Not
                                                      a usable value. */
} _bcm_field_stat_t;

#define _BCM_FIELD_STAT \
{ \
    "BytesEven", \
    "BytesOdd", \
    "PacketsEven", \
    "PacketsOdd" \
}

#define _FP_STAT_HW_MODE_MAX  (0xf)
#define _FP_TRIDENT_STAT_HW_MODE_MAX  (0x3F)

/* Statistics entity was  created through counter APIs.  */
#define _FP_STAT_INTERNAL          (1 << 0)
/* UpdateCounter action was used with NO_YES/YES_NO. */
#define _FP_STAT_COUNTER_PAIR      (1 << 1)
/* Arithmetic operations. */
#define _FP_STAT_ADD               (1 << 2)
#define _FP_STAT_SUBSTRACT         (1 << 3)
/* Packet/bytes selector. */
#define _FP_STAT_BYTES             (1 << 4)
/* Sw entry doesn't match hw. */
#define _FP_STAT_DIRTY             (1 << 5)
/* Stat Create with ID */
#define _FP_STAT_CREATE_ID         (1 << 6)
/* Stat uses Flex Stat resources. */
#define _FP_STAT_FLEX_CNTR         (1 << 7)
/* Stat Create with Internal Advanced Flex Counter feature  */
#define _FP_STAT_INTERNAL_FLEX_COUNTER (1 << 8)




/* Action conflict check macro. */
#define _FP_ACTIONS_CONFLICT(_val_)    \
    if (action == _val_) {             \
        return (BCM_E_CONFIG);         \
    }


/*
 * Typedef:
 *     _field_stat_t
 * Purpose:
 *     This is the statistics collection entity description structure.
 *     Indexed by int sid (statistics id) handle.
 *
 */
typedef struct _field_stat_s {
    uint32               sid;           /* Unique stat entity identifier.  */

    /* Reference counters  information.*/
    uint16               sw_ref_count;  /* SW object use reference count.   */
    uint16               hw_ref_count;  /* HW object use reference count.   */
    /* Allocated hw resource information.*/
    int8                 offset_mode;   /* Mode to compute final counter idx*/
    int8                 pool_index;    /* Counter pool/slice stat resides. */
    int                  hw_index;      /* HW index stat resides.           */
    /* Reinstall flags. */
    uint32               hw_flags;      /* HW installation status flags.    */
    /* Application requested statistics. */
    uint8                nstat;         /* User requested stats array size. */
    bcm_field_stat_t     *stat_arr;     /* User requested stats array.      */
    /* HW supported statistics. */
    uint32               hw_stat;       /* Statistics supported by HW.      */
    uint16               hw_mode;       /* Hw configuration mode.           */
    uint8                hw_entry_count;/* Number of counters needed.       */
    bcm_field_group_t    gid;           /* Group statistics entity attached.*/
    _field_stage_id_t    stage_id;      /* Attached entry stage id.         */
    struct _field_stat_s *next;         /* Stat lookup linked list.         */
    /* Values after last detach. */
    uint64               *stat_values;  /* Stat value after it was detached */
                                        /* from a last entry.               */
    uint32              flex_mode;      /* Flex stat entity identifier.     */
} _field_stat_t;

typedef enum _field_stat_color_s {
    _bcmFieldStatColorNoColor = 0,
    _bcmFieldStatColorGreen,
    _bcmFieldStatColorYellow,
    _bcmFieldStatColorRed,
    _bcmFieldStatColorNotGreen,
    _bcmFieldStatColorNotYellow,
    _bcmFieldStatColorNotRed,
    _bcmFieldStatColorGreenYellow,
    _bcmFieldStatColorGreenRed,
    _bcmFieldStatColorYellowRed,
    _bcmFieldStatColorGreenNotGreen,
    _bcmFieldStatColorYellowNotYellow,
    _bcmFieldStatColorRedNotRed,
    _bcmFieldStatColorGreenYellowRed,
    _bcmFieldStatColorCount
} _field_stat_color_t;

/*
 * Typedef:
 *     _field_entry_stat_t
 * Purpose:
 *     This is field entry statistics collector descriptor structure.
 */
typedef struct _field_entry_stat_s {
    int            sid;          /* Unique statistics entity id. */
    int            extended_sid; /* Statistics entity id for Flex counter
                                  * in IFP */
    uint16         flags;        /* Statistics entity/entry flags.*/
    bcm_field_stat_action_t
                    stat_action; /* Stat Action. */
}_field_entry_stat_t;
/* Statistics entity attached to fp entry flag. */
#define _FP_ENTRY_STAT_VALID                (1 << 0)
/* Statistics entity installed in HW. */
#define _FP_ENTRY_STAT_INSTALLED            (1 << 1)
/* Statistics entity doesn't have any counters attached. */
#define _FP_ENTRY_STAT_EMPTY                (1 << 2)
/* Statistics entity use even counter. */
#define _FP_ENTRY_STAT_USE_EVEN             (1 << 3)
/* Statistics entity use odd counter. */
#define _FP_ENTRY_STAT_USE_ODD              (1 << 4)
/* In Trident2 EFP , Statistics action conflicts with
 * Vxlan Actions. Below flag will be used in case of
 * conflicts */
#define _FP_ENTRY_STAT_NOT_ALLOWED          (1 << 5)
#define _FP_ENTRY_EXTENDED_STAT_VALID       (1 << 6)
#define _FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED (1 << 7)
#define _FP_ENTRY_EXTENDED_STAT_INSTALLED   (1 << 8)
#define _FP_ENTRY_EXTENDED_STAT_RESERVED    (1 << 9)



typedef struct _field_oam_stat_action_s {
    bcm_field_oam_stat_action_t *oam_stat_action; /* Oam Stat Action */
    uint8  flex_pool_id;                          /* Flex Stat Pool Id */
    int8   pool_instance;                         /* Flex Pool Instance in 
                                                     FP_POLICY_TABLE */
    uint16 flags;                                 /* Action Flags */
    struct _field_oam_stat_action_s *next;        /* Next Pointer */
} _field_oam_stat_action_t;

typedef enum _field_oam_stat_pool_config_e {
    _bcmFieldOamStatPoolInstanceFree = 0,
    _bcmFieldOamStatPoolInstanceReserve = 1,
    _bcmFieldOamStatPoolInstanceGet = 2,
    _bcmFieldOamStatPoolInstanceFreeAll = 3,
    _bcmFieldOamStatPoolInstanceCount = 4
} _field_oam_stat_pool_config_t;
/*
 * Typedef:
 *     _field_entry_t
 * Purpose:
 *     This is the physical entry structure, hidden behind the logical
 *     bcm_field_entry_t handle.
 * Notes:
 *     Entries are stored in linked list in the under a slice's _field_slice_t
 *     structure.
 *
 *     Each entry can use 0 or 1 counters. Multiple entries may use the
 *     same counter. The only requirement is that the counter be within
 *     the same slice as the entry.
 *
 *     Similarly each entry can use 0 or 1 meter. Multiple entries may use
 *     the same meter. The only requirement is that the meter be within
 *     the same slice as the entry.
 */
struct _field_entry_s {
    bcm_field_entry_t      eid;        /* BCM unit unique entry identifier   */
    int                    prio;       /* Entry priority                     */
    uint32                 slice_idx;  /* Field entry tcam index.            */
    uint32                 ext_act_profile_idx; /* External field entry action profile index. */
    uint32                 flags;      /* _FP_ENTRY_xxx flags                */
    int8                   efp_key_match_type; /* Holds info(0/1) of which
                                                  tcam (tcam/key_match_tcam) has
                                                  to be written into EFP_TCAM
                                                  during entry install */
    _field_tcam_t          tcam;       /* Fields to be written into FP_TCAM  */
    _field_tcam_t          extra_tcam;
    _field_tcam_t          key_match_tcam; /* Fields to be written
                                              into FP_TCAM */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
    _field_pbmp_t          pbmp;       /* Port bitmap                        */
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
    _field_action_t        *actions;   /* linked list of actions for entry   */
    _field_slice_t         *fs;        /* Slice where entry lives            */
    _field_group_t         *group;     /* Group where entry lives            */
    _field_entry_stat_t    statistic;  /* Statistics collection entity.      */
                                       /* Policers attached to the entry.    */
    _field_oam_stat_action_t *field_oam_stat_action; /* Oam Stat Actions     */
    uint8                   oam_pool_instance; /* Oam Stat Pool Instance */
    _field_entry_policer_t policer[_FP_POLICER_LEVEL_COUNT];
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    _field_entry_policer_t global_meter_policer;
#endif
    uint8                  ing_mtp_slot_map;  /* Ingress Mirror
                                                 Slot Container Allocated */
    uint8                  egr_mtp_slot_map;  /* Egress  Mirror
                                                 Slot Container Allocated */
    struct _field_entry_s  *ent_copy;
    uint8                 dvp_type;   /* Type of DVP used in qualification  */
    struct _field_entry_s  *next;      /* Entry lookup linked list.          */
};

typedef struct _field_control_s _field_control_t;

typedef struct _field_entry_recover_egr_ports_s {
    bcm_field_entry_t eid;
    struct _field_entry_recover_egr_ports_s *next;
}_field_entry_recover_egr_ports_t;

typedef struct _field_egr_ports_entry_ids_list_s {
    bcm_field_entry_t eid;
    bcm_pbmp_t egr_ports_pbmp;
    struct _field_egr_ports_entry_ids_list_s *next;
}_field_egr_ports_entry_ids_list_t;

typedef struct _field_egr_ports_recovery_s {
   _field_entry_recover_egr_ports_t **entry_recover_egr_ports;
}_field_egr_ports_recovery_t;


/* Sw WorkAround for EgressPortsAdd Action */
extern _field_egr_ports_recovery_t
                   *_field_egr_ports_recovery[BCM_MAX_NUM_UNITS];

/*
 * Typedef:
 *     _field_funct_t
 * Purpose:
 *     Function pointers to device specific Field functions
 */
typedef struct _field_funct_s {
    int(*fp_detach)(int, _field_control_t *fc);  /* destructor function */
    int(*fp_data_qualifier_ethertype_add)(int, int,
                                          bcm_field_data_ethertype_t *);
    int(*fp_data_qualifier_ethertype_delete)(int, int,
                                          bcm_field_data_ethertype_t *);
    int(*fp_data_qualifier_ip_protocol_add)(int, int,
                                          bcm_field_data_ip_protocol_t *);
    int(*fp_data_qualifier_ip_protocol_delete)(int, int,
                                          bcm_field_data_ip_protocol_t *);
    int(*fp_data_qualifier_packet_format_add)(int, int,
                                          bcm_field_data_packet_format_t *);
    int(*fp_data_qualifier_packet_format_delete)(int, int,
                                          bcm_field_data_packet_format_t *);
    int(*fp_group_install)(int, _field_group_t *fg);
    int(*fp_selcodes_install)(int unit, _field_group_t *fg,
                              uint8 slice_numb, bcm_pbmp_t pbmp,
                              int selcode_index);
    int(*fp_slice_clear)(int unit, _field_group_t *fg, _field_slice_t *fs);
    int(*fp_entry_remove)(int unit, _field_entry_t *f_ent, int tcam_idx);
    int(*fp_entry_move) (int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new);
    int(*fp_selcode_get)(int unit, _field_stage_t*, bcm_field_qset_t*,
                         _field_group_t*);
    int(*fp_selcode_to_qset)(int unit, _field_stage_t *stage_fc,
                             _field_group_t *fg,
                             int code_id,
                             bcm_field_qset_t *qset);
    int(*fp_qual_list_get)(int unit, _field_stage_t *, _field_group_t*);
    int(*fp_tcam_policy_clear)(int unit, _field_stage_id_t stage_id, int idx);
    int(*fp_tcam_policy_install)(int unit, _field_entry_t *f_ent, int idx);
    int(*fp_tcam_policy_reinstall)(int unit, _field_entry_t *f_ent, int idx);
    int(*fp_policer_install)(int unit, _field_entry_t *f_ent,
                             _field_policer_t *f_pl);
    int(*fp_write_slice_map)(int unit, _field_stage_t *stage_fc);
    int(*fp_qualify_ip_type)(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t type, bcm_field_qualify_t qual);
    int(*fp_qualify_ip_type_get)(int unit, bcm_field_entry_t entry,
                                 bcm_field_IpType_t *type, bcm_field_qualify_t qual);
    int(*fp_action_support_check)(int unit, _field_entry_t *f_ent,
                                  bcm_field_action_t action, int *result);
    int(*fp_action_conflict_check)(int unit, _field_entry_t *f_ent,
                                   bcm_field_action_t action,
                                   bcm_field_action_t action1);
    int(*fp_action_params_check)(int unit, _field_entry_t *f_ent,
                                 _field_action_t *fa);
    int(*fp_action_depends_check)(int unit, _field_entry_t *f_ent,
                                  _field_action_t *fa);
    int (*fp_egress_key_match_type_set)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_install)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_reinstall)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_remove)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_prio_set)(int unit, _field_entry_t *f_ent,
                                      int prio);
    int (*fp_counter_get)(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count,
                          uint64 *byte_count);
    int (*fp_counter_set)(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count,
                          uint64 *byte_count);
    int (*fp_stat_index_get)(int unit, _field_stat_t *f_st,
                             bcm_field_stat_t stat, int *idx1,
                             int *idx2, int *idx3, uint32 *out_flags);
    int (*fp_control_set)(int unit,  _field_control_t *fc, 
                          bcm_field_control_t control, uint32 state);
    int (*fp_control_get)(int unit,  _field_control_t *fc,
                          bcm_field_control_t control, uint32 *state);
    int (*fp_stat_value_get)(int unit, int sync_mode, _field_stat_t *f_st,
                              bcm_field_stat_t stat, uint64 *value);
    int (*fp_stat_value_set)(int unit, _field_stat_t *f_st,
                              bcm_field_stat_t stat, uint64 value);
    int (*fp_stat_hw_mode_get)(int unit, _field_stat_t *f_st,
                                         _field_stage_id_t stage_id);
    int (*fp_stat_hw_alloc)(int unit, _field_entry_t *f_ent);
    int (*fp_stat_hw_free)(int unit, _field_entry_t *f_ent);
    int (*fp_group_add)(int unit, bcm_field_group_config_t *grp_conf);
    int (*fp_entry_enable)(int unit, _field_entry_t *f_ent, int enable_flag);
    int (*fp_qualify_svp)(int unit, bcm_field_entry_t entry, 
                          bcm_field_qualify_t qual, uint32 data,
                          uint32 mask, int svp_valid); 
    int (*fp_qualify_dvp)(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual, uint32 data,
                          uint32 mask, int dvp_valid);
    int (*fp_qualify_trunk)(int unit, 
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual, 
                            bcm_trunk_t data, bcm_trunk_t mask);
    int (*fp_qualify_trunk_get)(int unit,
                                bcm_field_entry_t entry,
                                bcm_field_qualify_t qual,
                                bcm_trunk_t *data, bcm_trunk_t *mask);
    int (*fp_qualify_inports)(int unit,
                              bcm_field_entry_t entry,
                              bcm_field_qualify_t qual,
                              bcm_pbmp_t data, 
                              bcm_pbmp_t mask,
                              bcm_pbmp_t valid_pbmp);
    int (*fp_entry_stat_extended_attach)(int unit, _field_entry_t *f_ent,
            int stat_id, bcm_field_stat_action_t stat_action);
    int (*fp_entry_stat_extended_get)(int unit, _field_entry_t *f_ent,
            int *stat_id, bcm_field_stat_action_t *stat_action);
    int (*fp_entry_stat_detach)(int unit, _field_entry_t *f_ent, int stat_id);
    int (*fp_class_size_get)(int unit, bcm_field_qualify_t qual,
                                            uint16 *class_size);
    int(*fp_policer_packet_counter_get32)(int unit,
                          _field_policer_t *f_pl, uint32 *count);
    int(*fp_policer_packet_reset_counter_get32)(int unit,
                          _field_policer_t *f_pl, uint32 *count);

} _field_funct_t;


#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef BCM_TOMAHAWK_SUPPORT

struct _field_dynamic_map_entry_s {
    int entry_id;
    _field_entry_t *entry;
    struct _field_dynamic_map_entry_s *next;
};
typedef struct _field_dynamic_map_entry_s _field_dynamic_map_entry_t;

struct _field_dynamic_map_lt_entry_s {
    int lt_entry_id;
    _field_lt_entry_t *lt_entry;
    struct _field_dynamic_map_lt_entry_s *next;
};
typedef struct _field_dynamic_map_lt_entry_s _field_dynamic_map_lt_entry_t;


struct _field_dynamic_map_s {
    _field_dynamic_map_entry_t *entry_map;
    _field_dynamic_map_lt_entry_t *lt_entry_map;
};

typedef struct _field_dynamic_map_s _field_dynamic_map_t;

/*
 * Typedef:
 *     _field_action_bmp_t
 * Purpose:
 *     Action bit map for tracking configured actions for each entry,
 *     used only in WB.
 */
typedef struct _field_action_bmp_s {
    SHR_BITDCL  *w;
} _field_action_bmp_t;

#define _FP_ACTION_BMP_FREE(bmp)   sal_free((bmp).w)
#define _FP_ACTION_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_ACTION_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_ACTION_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))

/* Flags */
#define _FP_WB_TLV_LEN_PRESENT      0x00000001
#define _FP_WB_TLV_LEN_ENCODED      0x00000002
#define _FP_WB_TLV_NO_VALUE         0x00000004
#define _FP_WB_TLV_NO_TYPE          0x00000008

/* TLV Processing */
#define _FP_WB_TLV_TYPES_MASK	        0xffc00000
#define _FP_WB_TLV_TYPE_SHIFT		    22
#define _FP_WB_TLV_LENGTH_MASK	        0x003fffff
#define _FP_WB_TLV_BASIC_TYPE_SHIFT 	28
#define _FP_WB_TLV_BASIC_TYPE_MASK	    0xf0000000
#define _FP_WB_TLV_ELEM_TYPE_MASK	    0x0fffffff

/* end markers */
#define _FIELD_WB_EM_CONTROL           0xceab1122
#define _FIELD_WB_EM_STAT              0xceab3344
#define _FIELD_WB_EM_POLICER           0xceab5566
#define _FIELD_WB_EM_HINT              0xceab7788
#define _FIELD_WB_EM_STAGE             0xceab9900
#define _FIELD_WB_EM_RANGE             0xcead1122
#define _FIELD_WB_EM_METER             0xcead3344
#define _FIELD_WB_EM_CNTR              0xcead5566
#define _FIELD_WB_EM_LTCONF            0xcead7788
#define _FIELD_WB_EM_DATACONTROL       0xcead9900
#define _FIELD_WB_EM_DATAQUAL          0xceab1234
#define _FIELD_WB_EM_DATAETYPE         0xceab5678
#define _FIELD_WB_EM_DATAPROT          0xceab4321
#define _FIELD_WB_EM_DATATCAM          0xceab8765
#define _FIELD_WB_EM_QUAL              0xcead4321
#define _FIELD_WB_EM_GROUP             0xcead8765
#define _FIELD_WB_EM_QUALOFFSET        0xcead1234
#define _FIELD_WB_EM_UDF               0xcead5678
#define _FIELD_WB_EM_LTENTRY           0xcead9012
#define _FIELD_WB_EM_ENTRY             0xcead3456
#define _FIELD_WB_EM_SLICE             0xcead7890
#define _FIELD_WB_EM_LTSLICE           0xcead0987
#define _FIELD_WB_EM_EXTRACTOR         0xcead6543

/* 
 *  Contains a type for every element that needs to be stored 
 *  in field structures 
 */
typedef enum _bcm_field_internal_element_e {
    _bcmFieldInternalType = 0,
    _bcmFieldInternalExtractor = 1,
    _bcmFieldInternalGroup = 2,
    _bcmFieldInternalGroupId = 3,
    _bcmFieldInternalGroupPri = 4,
    _bcmFieldInternalGroupQset = 5,
    _bcmFieldInternalQsetW = 6,
    _bcmFieldInternalQsetUdfMap = 7,
    _bcmFieldInternalGroupFlags = 8,
    _bcmFieldInternalGroupPbmp = 9,
    _bcmFieldInternalGroupSlice = 10,
    _bcmFieldInternalGroupQual = 11,
    _bcmFieldInternalQualQid = 12,
    _bcmFieldInternalQualOffset = 13,
    _bcmFieldInternalQualSize = 14,
    _bcmFieldInternalGroupStage = 15,
    _bcmFieldInternalGroupEntry = 16,
    _bcmFieldInternalGroupBlockCount = 17,
    _bcmFieldInternalGroupGroupStatus = 18,
    _bcmFieldInternalGroupGroupAset = 19,
    _bcmFieldInternalGroupCounterBmp = 20,
    _bcmFieldInternalGroupInstance = 21,
    _bcmFieldInternalGroupExtCodes = 22,
    _bcmFieldInternalGroupLtSlice = 23,
    _bcmFieldInternalGroupLtConfig = 24,
    _bcmFieldInternalGroupLtEntry = 25,
    _bcmFieldInternalGroupLtEntrySize = 26,
    _bcmFieldInternalGroupLtEntryStatus = 27,
    _bcmFieldInternalLtStatusEntriesFree = 28,
    _bcmFieldInternalLtStatusEntriesTotal = 29,
    _bcmFieldInternalLtStatusEntriesCnt = 30,
    _bcmFieldInternalGroupQsetSize = 31,
    _bcmFieldInternalGroupHintId = 32,
    _bcmFieldInternalGroupMaxSize = 33,
    _bcmFieldInternalEndStructGroup = 34,
    _bcmFieldInternalQualOffsetField = 35,
    _bcmFieldInternalQualOffsetNumOffset = 36,
    _bcmFieldInternalQualOffsetOffsetArr = 37,
    _bcmFieldInternalQualOffsetWidth = 38,
    _bcmFieldInternalQualOffsetSec = 39,
    _bcmFieldInternalQualOffsetBitPos = 40,
    _bcmFieldInternalQualOffsetQualWidth = 41,
    _bcmFieldInternalGroupPartCount = 42,
    _bcmFieldInternalEntryEid = 43,
    _bcmFieldInternalEntryPrio = 44,
    _bcmFieldInternalEntrySliceId = 45,
    _bcmFieldInternalEntryFlagsPart1 = 46,
    _bcmFieldInternalEntryFlagsPart2 = 47,
    _bcmFieldInternalEntryFlagsPart3 = 48,
    _bcmFieldInternalEntryPbmp = 49,
    _bcmFieldInternalEntryAction = 50,
    _bcmFieldInternalEntrySlice = 51,
    _bcmFieldInternalEntryGroup = 52,
    _bcmFieldInternalEntryStat = 53,
    _bcmFieldInternalEntryPolicer = 54,
    _bcmFieldInternalEntryIngMtp = 55,
    _bcmFieldInternalEntryEgrMtp = 56,
    _bcmFieldInternalEntryDvp = 57,
    _bcmFieldInternalEntryCopy = 58,
    _bcmFieldInternalEntryCopyTypePointer = 59,
    _bcmFieldInternalPbmpData = 60,
    _bcmFieldInternalPbmpMask = 61,
    _bcmFieldInternalEntryActionsPbmp = 62,
    _bcmFieldInternalActionParam = 63,
    _bcmFieldInternalGroupClassAct = 64,
    _bcmFieldInternalActionHwIdx = 65,
    _bcmFieldInternalActionFlags = 66,
    _bcmFieldInternalStatFlags = 67,
    _bcmFieldInternalPolicerFlags = 68,
    _bcmFieldInternalExtl1e32Sel = 69,
    _bcmFieldInternalExtl1e16Sel = 70,
    _bcmFieldInternalExtl1e8Sel = 71,
    _bcmFieldInternalExtl1e4Sel = 72,
    _bcmFieldInternalExtl1e2Sel = 73,
    _bcmFieldInternalExtl2e16Sel = 74,
    _bcmFieldInternalExtl3e1Sel = 75,
    _bcmFieldInternalExtl3e2Sel = 76,
    _bcmFieldInternalExtl3e4Sel = 77,
    _bcmFieldInternalExtPmuxSel = 78,
    _bcmFieldInternalExtIntraSlice = 79,
    _bcmFieldInternalExtSecondary = 80,
    _bcmFieldInternalExtIpbmpRes = 81,
    _bcmFieldInternalExtNorml3l4 = 82,
    _bcmFieldInternalExtNormmac = 83,
    _bcmFieldInternalExtAuxTagaSel = 84,
    _bcmFieldInternalExtAuxTagbSel = 85,
    _bcmFieldInternalExtAuxTagcSel = 86,
    _bcmFieldInternalExtAuxTagdSel = 87,
    _bcmFieldInternalExtTcpFnSel = 88,
    _bcmFieldInternalExtTosFnSel = 89,
    _bcmFieldInternalExtTtlFnSel = 90,
    _bcmFieldInternalExtClassIdaSel = 91,
    _bcmFieldInternalExtClassIdbSel = 92,
    _bcmFieldInternalExtClassIdcSel = 93,
    _bcmFieldInternalExtClassIddSel = 94,
    _bcmFieldInternalExtSrcContaSel = 95,
    _bcmFieldInternalExtSrcContbSel = 96,
    _bcmFieldInternalExtSrcDestCont0Sel = 97,
    _bcmFieldInternalExtSrcDestCont1Sel = 98,
    _bcmFieldInternalExtKeygenIndex = 99,
    _bcmFieldInternalLtEntrySlice = 100,
    _bcmFieldInternalLtEntryIndex = 101,
    _bcmFieldInternalGroupCount = 102,
    _bcmFieldInternalSliceCount = 103,
    _bcmFieldInternalLtSliceCount = 104,
    _bcmFieldInternalEndStructGroupQual = 105,
    _bcmFieldInternalEndStructQualOffset = 106,
    _bcmFieldInternalEndStructEntryDetails = 107,
    _bcmFieldInternalEndStructEntry = 108,
    _bcmFieldInternalEntryPbmpData = 109,
    _bcmFieldInternalEntryPbmpMask = 110,
    _bcmFieldInternalEndStructAction = 111,
    _bcmFieldInternalGlobalEntryPolicer = 112,
    _bcmFieldInternalGlobalEntryPolicerPid = 113,
    _bcmFieldInternalGlobalEntryPolicerFlags = 114,
    _bcmFieldInternalEndGroupExtractor = 115,
    _bcmFieldInternalControl = 116,
    _bcmFieldInternalControlFlags = 117,
    _bcmFieldInternalStage = 118,
    _bcmFieldInternalControlGroups = 119,
    _bcmFieldInternalControlStages = 120,
    _bcmFieldInternalControlPolicerHash = 121,
    _bcmFieldInternalControlPolicerCount = 122,
    _bcmFieldInternalControlStathash = 123,
    _bcmFieldInternalControlStatCount = 124,
    _bcmFieldInternalControlHintBmp = 125,
    _bcmFieldInternalControlHintHash = 126,
    _bcmFieldInternalControlLastAllocatedLtId = 127,
    _bcmFieldInternalEndStructControl = 128,
    _bcmFieldInternalPolicerPid = 129,
    _bcmFieldInternalPolicerSwRefCount = 130,
    _bcmFieldInternalPolicerHwRefCount = 131,
    _bcmFieldInternalPolicerLevel = 132,
    _bcmFieldInternalPolicerPoolIndex = 133,
    _bcmFieldInternalPolicerHwIndex = 134,
    _bcmFieldInternalPolicerHwFlags = 135,
    _bcmFieldInternalPolicerStageId = 136,
    _bcmFieldInternalEndStructPolicer = 137,
    _bcmFieldInternalStatsId = 138,
    _bcmFieldInternalStatSwRefCount = 139,
    _bcmFieldInternalStatHwRefCount = 140,
    _bcmFieldInternalStatOffsetMode = 141,
    _bcmFieldInternalStatPoolIndex = 142,
    _bcmFieldInternalStatHwIndex = 143,
    _bcmFieldInternalStatHwFlags = 144,
    _bcmFieldInternalEndStructStage = 145,
    _bcmFieldInternalStatnStat = 146,
    _bcmFieldInternalStatArr = 147,
    _bcmFieldInternalStatHwStat = 148,
    _bcmFieldInternalStatHwMode = 149,
    _bcmFieldInternalStatHwEntryCount = 150,
    _bcmFieldInternalStatGid = 151,
    _bcmFieldInternalStatStageId = 152,
    _bcmFieldInternalStatStatValues = 153,
    _bcmFieldInternalStatFlexMode = 154,
    _bcmFieldInternalEndStructStat = 155,
    _bcmFieldInternalHintHintid = 156,
    _bcmFieldInternalHintHints = 157,
    _bcmFieldInternalHintsHinttype = 158,
    _bcmFieldInternalHintsQual = 159,
    _bcmFieldInternalHintsMaxValues = 160,
    _bcmFieldInternalHintsStartbit = 161,
    _bcmFieldInternalHintsEndbit = 162,
    _bcmFieldInternalHintsFlags = 163,
    _bcmFieldInternalHintsMaxGrpSize = 164,
    _bcmFieldInternalEndStructHints = 165,
    _bcmFieldInternalHintGrpRefCount = 166,
    _bcmFieldInternalHintCount = 167,
    _bcmFieldInternalEndStructHint = 168,
    _bcmFieldInternalPolicerCfgFlags = 169,
    _bcmFieldInternalPolicerCfgMode = 170,
    _bcmFieldInternalPolicerCfgCkbitsSec = 171,
    _bcmFieldInternalPolicerCfgMaxCkbitsSec = 172,
    _bcmFieldInternalPolicerCfgCkbitsBurst = 173,
    _bcmFieldInternalPolicerCfgPkbitsSec = 174,
    _bcmFieldInternalPolicerCfgMaxPkbitsSec = 175,
    _bcmFieldInternalPolicerCfgPkbitsBurst  = 176,
    _bcmFieldInternalPolicerCfgKbitsCurrent = 177,
    _bcmFieldInternalPolicerCfgActionId = 178,
    _bcmFieldInternalPolicerCfgSharingMode = 179,
    _bcmFieldInternalPolicerCfgEntropyId = 180,
    _bcmFieldInternalPolicerCfgPoolId = 181,
    _bcmFieldInternalControlEndStructUdf = 182,
    _bcmFieldInternalHintHintsHintType = 183,
    _bcmFieldInternalHintHintsQual = 184,
    _bcmFieldInternalHintHintsMaxValues = 185,
    _bcmFieldInternalHintHintsStartBit = 186,
    _bcmFieldInternalHintHintsEndBit = 187,
    _bcmFieldInternalHintHintsFlags = 188,
    _bcmFieldInternalHintHintsMaxGrpSize = 189,
    _bcmFieldInternalEndStructHintHints = 190,
    _bcmFieldInternalStageStageid = 191,
    _bcmFieldInternalStageFlags = 192,
    _bcmFieldInternalStageTcamSz = 193,
    _bcmFieldInternalStageTcamSlices = 194,
    _bcmFieldInternalStageNumInstances = 195,
    _bcmFieldInternalStageNumPipes = 196,
    _bcmFieldInternalStageRanges = 197,
    _bcmFieldInternalStageRangeId = 198,
    _bcmFieldInternalStageNumMeterPools = 199,
    _bcmFieldInternalStageMeterPool = 200,
    _bcmFieldInternalStageNumCntrPools = 201,
    _bcmFieldInternalStageCntrPools = 202,
    _bcmFieldInternalStageLtTcamSz = 203,
    _bcmFieldInternalStageNumLogicalTables = 204,
    _bcmFieldInternalStageLtInfo = 205,
    _bcmFieldInternalStageExtLevels = 206,
    _bcmFieldInternalStageOperMode = 207,
    _bcmFieldInternalRangeFlags = 208,
    _bcmFieldInternalRangeRid = 209,
    _bcmFieldInternalRangeMin = 210,
    _bcmFieldInternalRangeMax = 211,
    _bcmFieldInternalRangeHwIndex = 212,
    _bcmFieldInternalRangeStyle = 213,
    _bcmFieldInternalMeterLevel = 214,
    _bcmFieldInternalMeterSliceId = 215,
    _bcmFieldInternalMeterSize = 216,
    _bcmFieldInternalMeterPoolSize = 217,
    _bcmFieldInternalMeterFreeMeters = 218,
    _bcmFieldInternalMeterNumMeterPairs = 219,
    _bcmFieldInternalMeterBmp = 220,
    _bcmFieldInternalCntrSliceId = 221,
    _bcmFieldInternalCntrSize = 222,
    _bcmFieldInternalCntrFreeCntrs = 223,
    _bcmFieldInternalCntrBmp = 224,
    _bcmFieldInternalLtConfigValid = 225,
    _bcmFieldInternalLtConfigLtId = 226,
    _bcmFieldInternalLtConfigLtPartPri = 227,
    _bcmFieldInternalLtConfigLtPartMap = 228,
    _bcmFieldInternalLtConfigLtActionPri = 229,
    _bcmFieldInternalLtConfigPri = 230,
    _bcmFieldInternalLtConfigFlags = 231,
    _bcmFieldInternalLtConfigEntry = 232,
    _bcmFieldInternalEndStructRanges = 233,
    _bcmFieldInternalEndStructMeter = 234,
    _bcmFieldInternalEndStructCntr = 235,
    _bcmFieldInternalEndStructLtConfig = 236,
    _bcmFieldInternalSlice = 237,
    _bcmFieldInternalSliceStartTcamIdx = 238,
    _bcmFieldInternalSliceNumber = 239,
    _bcmFieldInternalSliceEntryCount = 240,
    _bcmFieldInternalSliceFreeCount = 241,
    _bcmFieldInternalSliceCountersCount = 242,
    _bcmFieldInternalSliceMetersCount = 243,
    _bcmFieldInternalSliceInstalledEntriesCount = 244,
    _bcmFieldInternalSliceCounterBmp = 245,
    _bcmFieldInternalSliceMeterBmp = 246,
    _bcmFieldInternalSliceStageId = 247,
    _bcmFieldInternalSlicePortPbmp = 248,
    _bcmFieldInternalSliceEntriesInfo = 249,
    _bcmFieldInternalSliceNextSlice = 250,
    _bcmFieldInternalSlicePrevSlice = 251,
    _bcmFieldInternalSliceFlags = 252,
    _bcmFieldInternalSliceGroupFlags = 253,
    _bcmFieldInternalSliceLtMap = 254,
    _bcmFieldInternalEndStructSlice = 255,
    _bcmFieldInternalEndStructExtractor = 256,
    _bcmFieldInternalLtSliceSliceNum = 257,
    _bcmFieldInternalLtSliceStartTcamIdx = 258,
    _bcmFieldInternalLtSliceEntryCount = 259,
    _bcmFieldInternalLtSliceFreeCount = 260,
    _bcmFieldInternalLtSliceStageid = 261,
    _bcmFieldInternalLtSliceEntryinfo = 262,
    _bcmFieldInternalLtSliceNextSlice = 263,
    _bcmFieldInternalLtSlicePrevSlice = 264,
    _bcmFieldInternalLtSliceFlags = 265,
    _bcmFieldInternalLtSliceGroupFlags = 266,
    _bcmFieldInternalDataControlStart = 267,
    _bcmFieldInternalDataControlUsageBmp = 268,
    _bcmFieldInternalDataControlDataQualStruct = 269,
    _bcmFieldInternalDataControlDataQualQid = 270,
    _bcmFieldInternalDataControlDataQualUdfSpec = 271,
    _bcmFieldInternalDataControlDataQualOffsetBase = 272,
    _bcmFieldInternalDataControlDataQualOffset = 273,
    _bcmFieldInternalDataControlDataQualByteOffset = 274,
    _bcmFieldInternalDataControlDataQualHwBmp = 275,
    _bcmFieldInternalDataControlDataQualFlags = 276,
    _bcmFieldInternalDataControlDataQualElemCount = 277,
    _bcmFieldInternalDataControlDataQualLength = 278,
    _bcmFieldInternalDataControlEndStructDataQual = 279,
    _bcmFieldInternalDataControlEthertypeStruct = 280,
    _bcmFieldInternalDataControlEthertypeRefCount = 281,
    _bcmFieldInternalDataControlEthertypeL2 = 282,
    _bcmFieldInternalDataControlEthertypeVlanTag = 283,
    _bcmFieldInternalDataControlEthertypePortEt = 284,
    _bcmFieldInternalDataControlEthertypeRelOffset = 285,
    _bcmFieldInternalDataControlProtStart = 286,
    _bcmFieldInternalDataControlProtIp4RefCount = 287,
    _bcmFieldInternalDataControlProtIp6RefCount = 288,
    _bcmFieldInternalDataControlProtFlags = 289,
    _bcmFieldInternalDataControlProtIp = 290,
    _bcmFieldInternalDataControlProtL2 = 291,
    _bcmFieldInternalDataControlProtVlanTag = 292,
    _bcmFieldInternalDataControlProtRelOffset = 293,
    _bcmFieldInternalDataControlTcamStruct = 294,
    _bcmFieldInternalDataControlTcamRefCount = 295,
    _bcmFieldInternalDataControlTcamPriority = 296,
    _bcmFieldInternalDataControlElemSize = 297,
    _bcmFieldInternalDataControlNumElem = 298,
    _bcmFieldInternalEndStructDataControl = 299,
    _bcmFieldInternalControlUdfValid = 300,
    _bcmFieldInternalControlUdfUseCount = 301,
    _bcmFieldInternalControlUdfNum = 302,
    _bcmFieldInternalControlUdfUserNum = 303,
    _bcmFieldInternalControlUdfDetails = 304,
    _bcmFieldInternalDataControlEndStructEtype = 305,
    _bcmFieldInternalDataControlEndStructProt = 306,
    _bcmFieldInternalDataControlEndStructTcam = 307,
    _bcmFieldInternalEntryStatSid = 308,
    _bcmFieldInternalEntryStatExtendedSid = 309,
    _bcmFieldInternalEntryStatFlags = 310,
    _bcmFieldInternalEntryStatAction = 311,
    _bcmFieldInternalEndStructLtEntryDetails = 312,
    _bcmFieldInternalSliceLtPartitionPri = 313,
    _bcmFieldInternalEntryPolicerPid = 314,
    _bcmFieldInternalEntryPolicerFlags = 315,
    _bcmFieldInternalEndStructEntPolicer = 316,
    _bcmFieldInternalEndStructIFP = 317,
    _bcmFieldInternalElementCount = 318
} _bcm_field_internal_element_t;

#endif /* BCM_TOMAHAWK_SUPPORT */
typedef struct _field_slice_group_info_s {
    bcm_field_group_t                  gid; /* Temp recovered GID       */
    int                           priority; /* Temp recovered group priority */
    bcm_field_qset_t                  qset; /* Temp QSET retrieved      */
    bcm_pbmp_t                        pbmp; /* Temp recovered rep. port */
    int                              found; /* Group has been found     */
    int                              flags; /* Group flags */
    int                      action_res_id; /* Group Action Resolution id */
    struct _field_slice_group_info_s *next;
} _field_slice_group_info_t;

typedef struct _field_hw_qual_info_s {
    _field_sel_t pri_slice[2]; /* Potentially intraslice */
    _field_sel_t sec_slice[2]; /* Potentially intraslice */
} _field_hw_qual_info_t;

typedef struct _meter_config_s {
    uint8 meter_mode;
    uint8 meter_mode_modifier;
    uint16 meter_idx;
    uint8 meter_update_odd;
    uint8 meter_test_odd;
    uint8 meter_update_even;
    uint8 meter_test_even;
} _meter_config_t;
typedef struct _field_table_pointers_s {
    char *fp_global_mask_tcam_buf;
    char *fp_gm_tcam_x_buf;
    char *fp_gm_tcam_y_buf;
    uint32 *fp_tcam_buf;
    uint32 *fp_tcam_x_buf;
    uint32 *fp_tcam_y_buf;
} _field_table_pointers_t;


/* Flags for is_present_bmp in _field_entry_wb_t */
#define _FP_OAM_STAT_ACTION_WB_SYNC       (1 << 0)
#define _FP_EXTENDED_STATS_WB_SYNC        (1 << 1)

typedef struct _field_oam_stat_action_wb_s {
    bcm_stat_object_t stat_object;
    uint32 stat_mode_id;
} _field_oam_stat_action_wb_t;

typedef struct _field_oam_action_recover_s {
    int8 flex_pool_id;
    bcm_field_stat_action_t action;
}_field_oam_action_recover_t;

typedef struct _field_extended_stat_wb_s {
    uint32 flex_mode;
    int hw_flags;
    uint8 flex_stat_map;
    bcm_field_stat_t stat_id;
} _field_extended_stat_wb_t;

/* Per entry data that can be synced and recovered */
typedef struct _field_entry_wb_s {
    uint32 is_present_bmp; /* Bitmap to notify if data present */
    /* Oam Lm Stat Actions */
    uint8 oam_stat_action_count;     /* Number of oam stat actions */
    _field_oam_stat_action_wb_t *oam_stat_action_wb;
                                     /* Oam Stat Action */
    /* Extended Stats */
    _field_extended_stat_wb_t *extended_stat_wb;
                                    /* Extended Stats */
} _field_entry_wb_t;

#ifdef BCM_TOMAHAWK_SUPPORT
/* Type of elements that are being stored */
typedef enum _bcm_field_internal_type_e {
    _bcmFieldInternalVariable = 0,
    _bcmFieldInternalArray = 1,
    _bcmFieldInternalTypeCount = 2
} _bcm_field_internal_type_t;


typedef struct _field_tlv_s {
    _bcm_field_internal_element_t type;
    _bcm_field_internal_type_t basic_type;
    uint32 length;
    void *value;
} _field_tlv_t;


typedef struct _field_type_map_s {
    _bcm_field_internal_element_t element;
    int size;
    uint32 flags;
} _field_type_map_t;
#endif /* BCM_TOMAHAWK_SUPPORT */

#endif

/* This reflects the 64,000bps granularity */
#define _FP_POLICER_REFRESH_RATE 64

typedef struct _field_stage_funct_s {
    int (*fp_stages_add)(int unit, _field_control_t *fc);
    int (*fp_stages_delete)(int unit, _field_control_t *fc);
} _field_stage_funct_t;

/*
 * Typedef:
 *     _field_hintid_bmp_t
 * Purpose:
 *     Hint bit map for tracking allocation of hintids.
 */
typedef struct _field_hintid_bmp_s{
    SHR_BITDCL  *w;
} _field_hintid_bmp_t;

#define _FP_HINTID_BMP_ADD(bmp, hintid)     SHR_BITSET(((bmp).w), (hintid))
#define _FP_HINTID_BMP_REMOVE(bmp, hintid)  SHR_BITCLR(((bmp).w), (hintid))
#define _FP_HINTID_BMP_TEST(bmp, hintid)    SHR_BITGET(((bmp).w), (hintid))

typedef struct _field_hint_s {
    bcm_field_hint_t     *hint;  /* hint datastructure */
    struct _field_hint_s *next;  /* Next pointer to hint structure */
}_field_hint_t;

typedef struct _field_hints_s {
    bcm_field_hintid_t  hintid;         /* hint id */
    _field_hint_t      *hints;          /* List of hints linked to hintid */
    uint16              grp_ref_count;  /* Count of groups linked to a hintid */
    uint16              hint_count;     /* Count of hints linked to a hintid */
    struct _field_hints_s *next;        /* Pointer to next hint_id present
                                           in the hash_index */
}_field_hints_t;

/*
 * Typedef:
 *     _field_control_t
 * Purpose:
 *     One structure for each StrataSwitch Device that holds the global
 *     field processor metadata for one device.
 */
struct _field_control_s {
    int                    init;          /* TRUE if field module has been */
                                          /* initialized                   */
    sal_mutex_t            fc_lock;       /* Protection mutex.             */
    uint8                  flags;         /* Module specific flags, as follows */
#define _FP_COLOR_INDEPENDENT          (1 << 0)
#define _FP_INTRASLICE_ENABLE          (1 << 1)
#define _FP_EXTERNAL_PRESENT           (1 << 2)
#ifdef BCM_WARM_BOOT_SUPPORT
#define _FP_STABLE_SAVE_LONG_IDS       (1 << 3)
#endif
#define _FP_STAT_SYNC_ENABLE           (1 << 4)
#define _FP_POLICER_GROUP_SHARE_ENABLE (1 << 5)

    bcm_field_stage_t      stage;         /* Default FP pipeline stage.    */
    int                    tcam_ext_numb; /* Slice number for external.    */
                                          /* TCAM (-1 if not present).     */
    _field_udf_t           *udf;          /* field_status->group_total     */
                                          /* elements indexed by priority. */
    struct _field_group_s  *groups;       /* List of groups in unit.       */
    struct _field_stage_s  *stages;       /* Pipeline stage FP info.       */
    _field_funct_t         functions;     /* Device specific functions.    */
    _field_policer_t       **policer_hash;/* Policer lookup hash.          */
    uint32                 policer_count; /* Number of active policers.    */
#ifdef BCM_TRIUMPH3_SUPPORT
    uint8                  ingress_logical_policer_pools_mode;
#endif
    _field_stat_t          **stat_hash;   /* Counter lookup hash.          */
    uint32                 stat_count;    /* Number of active counters.    */
    _field_hintid_bmp_t    hintid_bmp;    /* Bitmap of Created Hint Ids    */
    _field_hints_t         **hints_hash;  /* Hints lookup hash             */
#ifdef BCM_WARM_BOOT_SUPPORT
#define _FIELD_SCACHE_PART_COUNT 0x2
    /* Size of Level2 warm boot cache */
    uint32                 scache_size[_FIELD_SCACHE_PART_COUNT];
    uint32                 scache_usage;  /* Actual scache usage            */
    uint32                 scache_pos;   /* Current position of scache
                                            pointer in _FIELD_ACACHE_PART_0 */
    uint32                 scache_pos1;  /* Current position of scache
                                            pointer in _FIELD_ACACHE_PART_1 */
    /* Pointer to scache section */
    uint8                  *scache_ptr[_FIELD_SCACHE_PART_COUNT];
    uint8                  l2warm;        /* Use the stored scache data     */
    _field_slice_group_info_t *group_arr; /* Temp recovered group info      */
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    uint32                 last_allocated_lt_eid; /* Last allocated LT ID.  */
    struct _field_presel_entry_s **presel_hash; /* Presel Entry lookup hash.*/
    struct _field_presel_info_s  *presel_info;  /* Preselector information. */ 
#endif /* !BCM_TOMAHAWK_SUPPORT */
    _field_stage_funct_t   stage_func;   /* Stage Init and Destroy functions. */
};


/*
 *  _field_group_add_fsm struct.
 *  Field group creation state machine.
 *  Handles sanity checks, resources selection, allocation.
 */
typedef struct _field_group_add_fsm_s {
    /* State machine data. */
    uint8                 fsm_state;     /* FSM state.                     */
    uint8                 fsm_state_prev;/* Previous FSM state.            */
    uint32                flags;         /* State specific flags.          */
    int                   rv;            /* Error code.                    */

    /* Field control info. */
    _field_control_t      *fc;           /* Field control structure.       */
    _field_stage_t        *stage_fc;     /* Field stage control structure. */


    /* Group information. */
    int                    priority;     /* New group priority.            */
    bcm_field_group_t      group_id;     /* SW Group id.                   */
    bcm_pbmp_t             pbmp;         /* Input port bitmap.             */
    bcm_field_qset_t       qset;         /* Qualifiers set.                */
    bcm_field_group_mode_t mode;         /* Group mode.                    */
    bcm_field_presel_set_t preselset;    /* set of preselector Ids.        */
    /* Allocated data structures. */
    _field_group_t         *fg;          /* Allocated group structure.     */
    bcm_field_hintid_t     hintid;       /* HintId to be linked to a group */
    int                      action_res_id;  /* Action resolution id */
} _field_group_add_fsm_t;

#define _BCM_FP_GROUP_ADD_STATE_START                  (0x1)
#define _BCM_FP_GROUP_ADD_STATE_ALLOC                  (0x2)
#define _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE            (0x3)
#define _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET          (0x4)
#define _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE         (0x5)
#define _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE         (0x6)
#define _BCM_FP_GROUP_ADD_STATE_HW_QUAL_LIST_GET       (0x7)
#define _BCM_FP_GROUP_ADD_STATE_UDF_UPDATE             (0x8)
#define _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP     (0x9)
#define _BCM_FP_GROUP_ADD_STATE_EXTERNAL_GROUP_CREATE  (0xa)
#define _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS           (0xb)
#define _BCM_FP_GROUP_ADD_STATE_END                    (0xc)

#define _BCM_FIELD_PIPE0_GROUP              (0x0)
#define _BCM_FIELD_PIPE1_GROUP              (0x1)
#define _BCM_FIELD_PIPE2_GROUP              (0x2)
#define _BCM_FIELD_PIPE3_GROUP              (0x3)
#define _BCM_FIELD_GLOBAL_GROUP             (0xf)

#define _BCM_FP_GROUP_ADD_STATE_STRINGS \
{                                  \
    "None", \
    "GroupAddStart",               \
    "GroupAddAlloc",               \
    "GroupAddStateQsetUpdate",     \
    "GroupAddSelCodesGet",         \
    "GroupAddQsetAlternate",       \
    "GroupAddSliceAllocate",       \
    "GroupAddHwQualListGet",       \
    "GroupAddUdfUpdate",           \
    "GroupAddAdjustVirtualMap",    \
    "GroupAddExternalGroupCreate", \
    "GroupAddCamCompress",         \
    "GroupAddEnd"                  \
}

#define _BCM_FP_GROUP_ADD_INTRA_SLICE          (1 << 0)
#define _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY     (1 << 1)
#define _BCM_FP_GROUP_ADD_WLAN                 (1 << 2)
#define _BCM_FP_GROUP_ADD_SMALL_SLICE          (1 << 3)
#define _BCM_FP_GROUP_ADD_LARGE_SLICE          (1 << 4)

/* _bcm_field_DestType_e  - Destination types. */
typedef enum _bcm_field_DestType_e {
    _bcmFieldDestTypeNhi,   /* Next Hop Interface */
    _bcmFieldDestTypeL3mc,  /* IPMC group         */
    _bcmFieldDestTypeL2mc,  /* L2 Multicast group */
    _bcmFieldDestTypeDvp,   /* Virtual Port       */
    _bcmFieldDestTypeDglp,  /* Mod/Port/Trunk     */
    _bcmFieldDestTypeNone,  /* None               */
    _bcmFieldDestTypeEcmp,  /* ECMP group         */
    _bcmFieldDestTypeCount  /* Always Last. Not a usable value. */
} _bcm_field_DestType_t;

#define _FIELD_D_TYPE_MASK  (0x7)
#define _FIELD_D_TYPE_POS   \
       ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIUMPH3(unit)) ? (18) : \
       ((SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit)) ? (16) : (14)))
#define _FIELD_D_TYPE_INSERT(_d_, _dtype_)     \
       ((_d_) |= ((uint32) (_dtype_) << _FIELD_D_TYPE_POS))
#define _FIELD_D_TYPE_RESET(_d_)                \
       ((_d_) &= ~(_FIELD_D_TYPE_MASK << _FIELD_D_TYPE_POS))
#define _FIELD_D_TYPE_MASK_INSERT(_d_)          \
       ((_d_) |= (_FIELD_D_TYPE_MASK << _FIELD_D_TYPE_POS))

/* Insert destination type value at specific bit offset */
#define _FIELD_D_TYPE_INSERT_BITPOS(_d_, _dtype_, _bitpos_)   \
       ((_d_) |= ((uint32) (_dtype_) << (_bitpos_)))
#define _FIELD_D_TYPE_RESET_BITPOS(_d_, _bitpos_)             \
       ((_d_) &= ~(_FIELD_D_TYPE_MASK << (_bitpos_)))
#define _FIELD_D_TYPE_MASK_INSERT_BITPOS(_d_, _bitpos_)              \
       ((_d_) |= (_FIELD_D_TYPE_MASK << (_bitpos_)))

extern soc_field_t _trx_src_class_id_sel[], _trx_dst_class_id_sel[],
    _trx_interface_class_id_sel[];

#define COPY_TO_CPU_CANCEL                 0x2
#define SWITCH_TO_CPU_CANCEL               0x3
#define COPY_AND_SWITCH_TO_CPU_CANCEL      0x6

/*
 * Prototypes of Field Processor utility funtions
 */
extern int _bcm_field_meter_free_get(const _field_stage_t *stage_fc,
                                     int instance, const _field_slice_t *fs);
extern int _bcm_field_meters_total_get(const _field_stage_t *stage_fc,
                                       int instance, const _field_slice_t *fs);
extern int _bcm_field_counter_free_get(const _field_stage_t *stage_fc,
                                       const _field_slice_t *fs);
extern int _bcm_field_counters_total_get(const _field_stage_t *stage_fc,
                                         const _field_slice_t *fs);
extern int _bcm_field_entries_free_get(int unit, _field_slice_t *fs,
                                       _field_group_t *fg, int *free_cnt);
extern char * _field_action_name(bcm_field_action_t action);

extern int _bcm_field_qualifiers_info_get(int unit, _field_stage_t *stage_fc,
                                          bcm_field_qset_t *qset_req,
                                          _bcm_field_qual_info_t ***f_qual_arr,
                                          uint16 *qual_arr_size);
extern int _bcm_field_group_linked_list_insert(int unit,
                                               _field_group_add_fsm_t *fsm_ptr);
extern int _bcm_field_group_linked_list_remove(int unit, _field_group_t *fg);
extern int _bcm_field_group_add(int unit, _field_group_add_fsm_t *fsm_ptr);
extern int _bcm_field_group_stage_get(int unit, bcm_field_qset_t *qset_p,
                                      _field_stage_id_t *stage);
extern int _bcm_field_control_set(int unit,  _field_control_t *fc,
                                  bcm_field_control_t control, uint32 state);
extern int _bcm_field_control_get(int unit,  _field_control_t *fc,
                                  bcm_field_control_t control, uint32 *state);
extern int _bcm_field_stage_fpf_destroy(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_stage_fpf_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_data_qualifier_destroy(int unit, int qid);
extern int _bcm_field_group_slice_count_get(uint32 flags, int *count);
extern int _field_egr_ports_recovery_control_get(int unit,
                 _field_egr_ports_recovery_t **f_egr_recovery);
extern int _bcm_trx_field_egr_ports_recovery_add (int unit,
                                             bcm_field_entry_t entry,
                                             bcm_pbmp_t pbmp);
extern int _bcm_trx_field_egr_ports_recovery_search (int unit, int port,
                                                bcm_field_entry_t entry);
extern int _bcm_trx_field_egr_ports_recovery_print (int unit);
extern void _bcm_trx_field_action_egr_ports_recovery_dump (int unit,
              bcm_field_entry_t entry, const _field_action_t *fa);
extern int _bcm_trx_field_egr_ports_recovery_entry_ids_list_get (int unit,
                                   _field_egr_ports_entry_ids_list_t **entries,
                                   int *entry_count);
extern int _bcm_trx_field_egr_ports_recovery_entry_list_destroy (int unit,
                                 _field_egr_ports_entry_ids_list_t **entries);
extern int _bcm_trx_field_egr_ports_recovery_entry_remove (int unit,
                                                     bcm_field_entry_t entry);
extern int _bcm_trx_field_egr_ports_recovery_entry_pbmp_get (int unit,
                                                   bcm_field_entry_t entry,
                                                   bcm_pbmp_t *redirectPbmp);
extern int _field_egr_ports_recovery_scache_recover (int unit,
                          _field_control_t *fc, _field_stage_t   *stage_fc,
                                                            uint8 *scache_ptr);
extern int _field_egr_ports_recovery_scache_sync(int unit, _field_control_t *fc,
                                _field_stage_t   *stage_fc, uint8 *scache_ptr);
extern int _field_egr_ports_recovery_wb_upgrade_add (int unit,
                                                  _field_group_t *fg);
extern int _field_hints_control_get (int unit, bcm_field_hintid_t hint_id,
                                     _field_hints_t **f_ht);
extern int _bcm_field_hints_group_info_update (int unit,_field_group_t *fg);
extern int _bcm_field_hints_group_count_update (int unit,
                                                bcm_field_hintid_t hintid,
                                                uint8 action);
extern int _bcm_field_hints_add (int unit, bcm_field_hintid_t hint_id,
                                  bcm_field_hint_t *hint);
extern int _bcm_field_hints_node_get (int unit, _field_hint_t *hint_list,
                             bcm_field_hint_t *hint, _field_hint_t **hint_node);
extern int _bcm_field_hints_get(int unit, bcm_field_hintid_t hint_id,
                                bcm_field_hint_t *hint);
extern int _bcm_field_hints_delete (int unit, bcm_field_hintid_t hint_id,
                                    bcm_field_hint_t *hint);
extern int _bcm_field_hints_delete_all (int unit, bcm_field_hintid_t hint_id);
extern int _bcm_field_hints_dump (int unit);
extern int _bcm_field_hints_display (int unit, bcm_field_hintid_t hint_id);
extern int _bcm_field_hints_scache_sync (int unit, _field_control_t *fc,
                                  _field_stage_t *stage_fc, uint8 *scache_ptr);
extern int _bcm_field_hints_scache_recover (int unit, _field_control_t *fc,
                                  _field_stage_t *stage_fc, uint8 *scache_ptr);
extern int _bcm_hints_scache_hintid_sync (int unit, uint8 *scache_ptr);
extern int _bcm_hints_scache_hintid_recover (int unit, _field_control_t *fc,
                                   uint8 *scache_ptr);
extern int _field_control_get(int unit, _field_control_t **fc);
extern int _field_stage_control_get(int unit, _field_stage_id_t stage,
                                    _field_stage_t **stage_fc);
extern int _bcm_field_stat_hw_alloc(int unit, _field_entry_t *f_ent);
extern int _bcm_field_stat_array_init(int unit, _field_control_t *fc,
                                      _field_stat_t *f_st, int nstat,
                                      bcm_field_stat_t *stat_arr);
extern int _field_stat_destroy(int unit, int sid);
extern int _field_group_get(int unit, bcm_field_group_t gid,
                            _field_group_t **group_p);
extern int _bcm_field_group_id_generate(int unit, bcm_field_group_t *group);
extern int _bcm_field_group_default_aset_set(int unit, _field_group_t *fg);
extern int _field_entry_get(int unit, bcm_field_entry_t eid, uint32 flags,
                            _field_entry_t **enty_p);
extern int _field_group_entry_add (int unit, _field_group_t *fg,
                                   _field_entry_t *f_ent);
extern int _field_group_entry_delete (int unit, _field_group_t *fg,
                                      _field_entry_t *f_ent);
extern int _bcm_field_entry_tcam_parts_count (int unit,  
                                              _field_stage_id_t stage_id,
                                              uint32 group_flags, 
                                              int *part_count);
extern int _bcm_field_entry_flags_to_tcam_part (int unit, uint32 entry_flags,
                                                _field_group_t *fg,
                                                uint8 *entry_part);
extern int _bcm_field_tcam_part_to_entry_flags(int unit, int entry_part,
                                               _field_group_t *fg,
                                               uint32 *entry_flags);

extern int _field_port_filter_enable_set(int unit, _field_control_t *fc,
                                         uint32 state);
extern int _field_meter_refresh_enable_set(int unit, _field_control_t *fc,
                                           uint32 state);
extern int _field_qual_info_create(bcm_field_qualify_t qid, soc_mem_t mem,
                                   soc_field_t fpf, int offset, int width,
                                    _qual_info_t **fq_p);
extern int _field_qualify32(int unit, bcm_field_entry_t entry,
                            int qual, uint32 data, uint32 mask);
extern int _field_qual_offset_get(int unit, _field_entry_t *f_ent, int qid, 
                                  _bcm_field_qual_offset_t **offset);
extern int _bcm_field_qualify_trunk(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_qualify_t qual,
                                    bcm_trunk_t data,
                                    bcm_trunk_t mask);
extern int _bcm_field_qualify_trunk_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t qual,
                                        bcm_trunk_t *data,
                                        bcm_trunk_t *mask);
extern int _bcm_field_qualify_InPorts(int unit,
                                      bcm_field_entry_t entry,
                                      bcm_field_qualify_t qual,
                                      bcm_pbmp_t data,
                                      bcm_pbmp_t mask,
                                      bcm_pbmp_t valid_pbm);
extern int _field_qset_union(const bcm_field_qset_t *qset1,
                             const bcm_field_qset_t *qset2,
                             bcm_field_qset_t *qset_union);
extern int _field_qset_is_subset(const bcm_field_qset_t *qset1,
                                 const bcm_field_qset_t *qset2);
extern bcm_field_qset_t _field_qset_diff(const bcm_field_qset_t qset_1,
                                         const bcm_field_qset_t qset_2);
extern int _field_qset_is_empty(const bcm_field_qset_t qset);
extern int
_field_selector_diff(int unit, _field_sel_t *sel_arr, int part_idx,
                     _bcm_field_selector_t *selector,
                     uint8 *diff_count
                     );

extern int _field_trans_flags_to_index(int unit, int flags, uint8 *tbl_idx);
extern int _field_qual_add (_field_fpf_info_t *fpf_info, int qid, int offset,
                            int width, int code);
extern int _bcm_field_tpid_hw_encode(int unit, uint16 tpid, uint32 *hw_code);
extern int _field_tpid_hw_decode(int unit, uint32 hw_code, uint16 *tpid);
extern int _field_entry_prio_cmp(int prio_first, int prio_second);
extern int _bcm_field_prio_mgmt_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_prio_mgmt_deinit(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_entry_prio_mgmt_update(int unit, _field_entry_t *f_ent,
                                              int flag, uint32 old_location);
extern int
_bcm_field_prio_mgmt_slice_reinit(int            unit,
                                  _field_stage_t *stage_fc,
                                  _field_slice_t *fs
                                  );
extern int _bcm_field_entry_target_location(int unit, _field_stage_t *stage_fc,
                   _field_entry_t *f_ent, int new_prio, uint32 *new_location);
extern int _bcm_field_entry_counters_move(int unit, _field_stage_t *stage_fc,
                                          int old_index, int new_index);
extern int _bcm_field_sw_counter_update(int unit, _field_stage_t *stage_fc,
                                        soc_mem_t mem, int idx_min, int idx_max,
                                        char *buf, int flags);
extern int _bcm_field_counter_mem_get(int            unit,
                                      _field_stage_t *stage_fc,
                                      soc_mem_t      *counter_x_mem,
                                      soc_mem_t      *counter_y_mem
                                      );
extern int _bcm_field_counter_hw_alloc(int            unit,
                                       _field_entry_t *f_ent,
                                       _field_slice_t *fs
                                       );
extern int _bcm_field_stat_hw_free(int unit, _field_entry_t *f_ent);
extern int _bcm_field_entry_stat_detach(int unit, _field_entry_t *f_ent, int stat_id);
#if defined BCM_TRX_SUPPORT
extern int _bcm_field_action_stat_flag_update (int unit, _field_entry_t *f_ent,
                                                _field_action_t *fa,
                                                uint8 is_action_added);
extern int _bcm_field_action_stat_conflict_check (int unit, _field_entry_t *f_ent,
                                                         _field_action_t *fa);
#endif
extern int _bcm_field_stat_destroy(int unit, int sid);
extern int _bcm_field_stat_hw_mode_get(int unit, _field_stat_t *f_st,
                                          _field_stage_id_t stage_id);
extern int _bcm_field_stat_array_to_bmap(int unit,
                                         _field_stat_t *f_st,
                                         uint32 *bmap);
extern int _bcm_field_stages_supported_qset_init(int unit, 
                                                 _field_control_t *fc);
extern int _bcm_field_policer_get(int unit, bcm_policer_t pid,
                                  _field_policer_t **policer_p);
extern int _bcm_field_stat_get(int unit, int sid, _field_stat_t **stat_p);
extern int _bcm_field_stat_id_alloc(int unit, int *sid);
extern int _field_stat_array_init(int unit, _field_control_t *fc, _field_stat_t *f_st,
                               int nstat, bcm_field_stat_t *stat_arr);
extern int _bcm_field_meter_pair_mode_get(int unit, _field_policer_t *f_pl,
                                          uint32 *mode);
extern int _field_policer_id_alloc(int unit, bcm_policer_t *pid);
extern int _field_meter_pool_free(int unit, _field_stage_t *stage_fc,
                                  int instance, uint8 pool_idx);
extern int _bcm_field_29bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter32_collect_t *result);

#if defined BCM_TRIDENT2PLUS_SUPPORT
extern int _bcm_field_oam_stat_action_add (int unit, bcm_field_entry_t entry,
                                         bcm_field_oam_stat_action_t *oam_stat_action);
extern int _bcm_field_oam_stat_action_delete (int unit, bcm_field_entry_t entry,
                                         bcm_field_oam_stat_action_t *oam_stat_action);
extern int _bcm_field_oam_stat_action_delete_all (int unit,
                                                      bcm_field_entry_t entry);
extern int _bcm_field_oam_stat_action_get (int unit, bcm_field_entry_t entry,
                                         bcm_field_oam_stat_action_t *oam_stat_action);
extern int _bcm_field_oam_stat_action_get_all(int unit,
                                         bcm_field_entry_t entry, int oam_stat_max,
                                         bcm_field_oam_stat_action_t *oam_stat_action,
                                         int *oam_stat_count);
extern int _bcm_field_oam_stat_action_set (int unit, _field_entry_t *f_ent,
                                             soc_mem_t policy_mem, uint32 *policy_buf);
extern int _bcm_field_oam_stat_invalid_actions_remove (int unit,
                                                     _field_entry_t *f_ent);
extern int _field_oam_stat_action_dump (int unit, _field_entry_t *f_ent);
extern int _field_oam_stat_action_display (int unit);
extern int _bcm_field_oam_stat_actions_destroy (int unit, _field_entry_t *f_ent);
extern int _bcm_field_td2plus_entry_stat_extended_attach (int unit,
                                               _field_entry_t *f_ent, int stat_id,
                                               bcm_field_stat_action_t stat_action);
extern int _bcm_field_td2plus_entry_stat_extended_get (int unit,
                                               _field_entry_t *f_ent, int *stat_id,
                                               bcm_field_stat_action_t *stat_action);
extern void _field_td2plus_functions_init (_field_funct_t *functions);
extern int _bcm_field_td2plus_entry_stat_detach (int unit,
                                              _field_entry_t *f_ent, int stat_id);
extern int _bcm_field_td2plus_extended_stat_action_set (int unit,
                                               _field_entry_t *f_ent, soc_mem_t mem,
                                              int tcam_idx, uint32 *buf);
extern int _bcm_field_td2plus_extended_stat_hw_free (int unit, _field_entry_t *f_ent);
extern int _bcm_field_td2plus_stat_hw_alloc (int unit, _field_entry_t *f_ent);
extern int _bcm_field_td2plus_stat_hw_free (int unit, _field_entry_t *f_ent);
#if defined BCM_WARM_BOOT_SUPPORT
extern int _field_oam_stat_action_count_get (int unit,
                uint8 oam_pool_instance, int8 *oam_stat_count);
extern int _field_oam_stat_action_wb_add (int unit, _field_entry_t *f_ent,
                                   _field_oam_action_recover_t *oam_action);
extern int _field_oam_stat_action_scache_sync (int unit, _field_stage_t *stage_fc,
                                  _field_entry_t *f_ent,_field_entry_wb_t *f_ent_wb);
extern int _field_oam_stat_action_scache_recover (int unit, _field_stage_t   *stage_fc,
                                  _field_entry_t *f_ent, _field_entry_wb_t *f_ent_wb);
extern int _field_extended_stat_scache_sync (int unit, _field_stage_t   *stage_fc,
                _field_entry_t   *f_ent,  _field_entry_wb_t *f_ent_wb);
extern int _field_extended_flex_counter_recover(int unit, _field_entry_t *f_ent,
                                       int part, bcm_field_stat_t sid,
                                       _field_entry_wb_t *f_ent_wb,
                                       bcm_field_stat_action_t action);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_field_30bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter32_collect_t *result);
#endif /* BCM_TRIDENT_SUPPORT || BCM_TRIUMPH3_SUPPORT */

extern int _bcm_field_32bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter32_collect_t *result);
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_field_36bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter64_collect_t *result);
#endif /* BCM_BRADLEY_SUPPORT||BCM_TRIUMPH3_SUPPORT*/
#ifdef BCM_TRIDENT_SUPPORT
extern int _bcm_field_37bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter64_collect_t *result);
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
extern int _bcm_field_26bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter32_collect_t *result);
extern int _bcm_field_34bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter64_collect_t *result);
#if defined (BCM_WARM_BOOT_SUPPORT)
extern int _bcm_field_th_stage_ingress_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_egress_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_th_ingress_scache_sync(int unit,
                                             _field_stage_t   *stage_fc);
extern int _bcm_field_scache_th_pointer_init(int unit, _field_control_t  *fc);
extern int _bcm_field_scache_th_pointer_realloc(int unit, 
                                                _field_control_t  *fc);
extern int _bcm_field_action_val_get(int unit,
                                     _field_entry_t *f_ent,
                                     uint32 *entbuf,
                                     bcm_field_action_t action,
                                     uint32 flags,
                                     _bcm_field_action_offset_t *a_offset);
#endif
#endif
extern int _bcm_field_entry_counter_move(int unit, _field_stage_t *stage_fc,
                                         uint8 old_slice, int old_hw_index,
                                         _field_stat_t *f_st_old,
                                         _field_stat_t *f_st);
extern int _bcm_field_qset_test(bcm_field_qualify_t qid,
                                bcm_field_qset_t *qset, uint8 *result);
extern int _bcm_field_entry_get_by_id(int unit, bcm_field_entry_t eid,
                                      _field_entry_t **f_ent);
extern int _field_qual_tcam_key_mask_get(int unit, _field_entry_t *f_ent,
                                         _field_tcam_t *tcam, int ipbm_overlay);
extern int _bcm_field_qual_tcam_key_mask_get(int unit, _field_entry_t *f_ent);
extern int _bcm_field_qual_list_append(_field_group_t *fg, uint8 entry_part,
                                     _qual_info_t *fq_src, int offset);
extern int _bcm_field_qual_value_set(int unit, _bcm_field_qual_offset_t *q_offset,
                                     _field_entry_t *f_ent,
                                     uint32 *p_data, uint32 *p_mask);
extern int _bcm_field_qual_value_set_by_entry_type(int unit,
                                             _bcm_field_qual_offset_t *q_offset,
                                             _field_entry_t *f_ent,
                                             uint32 *p_data,
                                             uint32 *p_mask,
                                             uint8 idx);
extern int _bcm_field_qual_value_get(int unit,
                          _bcm_field_qual_offset_t *q_offset, 
                          _field_entry_t *f_ent, _bcm_field_qual_data_t p_data, 
                          _bcm_field_qual_data_t p_mask);
extern int _bcm_field_qual_partial_data_get(uint32 *in_data, uint8 start_pos,
                                            uint8 len, uint32 *out_data);
extern int _bcm_field_qual_partial_data_set(uint32 *in_data, uint8 start_pos,
                                            uint8 len, uint32 *out_data);
extern int _bcm_field_stages_supported_qset_init(int unit, _field_control_t *fc);
extern int _bcm_field_group_qualifiers_free(_field_group_t *fg, int idx);
extern int _bcm_field_qual_insert (int unit, _field_stage_t *stage_fc,
                                   int qual_type, int qual_id,
                                   _bcm_field_qual_conf_t *ptr);
extern int _bcm_field_tcam_part_to_slice_number(int unit, int entry_part,
                                                _field_group_t *fg,
                                                uint8 *slice_number);
extern int _bcm_field_group_status_init(int unit,
                                        bcm_field_group_status_t *entry);
extern int _bcm_field_group_status_calc(int unit, _field_group_t *fg);
extern int _bcm_field_stage_entries_free(int unit,
                                         _field_stage_t *stage_fc);
extern int _bcm_field_selcode_get(int unit, _field_stage_t *stage_fc,
                                      bcm_field_qset_t *qset_req,
                                      _field_group_t *fg);
extern void _bcm_field_qual_conf_t_init(_bcm_field_qual_conf_t *ptr);
extern void _bcm_field_qual_info_t_init(_bcm_field_qual_info_t *ptr);
extern int _bcm_field_selcode_to_qset(int unit, _field_stage_t *stage_fc,
                                          _field_group_t *fg, int code_id,
                                          bcm_field_qset_t *qset);
extern int _bcm_field_qual_lists_get(int unit, _field_stage_t *stage_fc,
                                         _field_group_t *fg);
extern int _bcm_field_entry_part_tcam_idx_get(int unit, _field_entry_t *f_ent,
                                              uint32 idx_pri, uint8 ent_part,
                                              int *idx_out);
extern int _bcm_field_data_qualifier_get(int unit, _field_stage_t *stage_fc,
                                         int qid,
                                         _field_data_qualifier_t **data_qual);
extern int _bcm_field_data_qualifier_alloc(int unit,
                                           _field_data_qualifier_t **qual);
extern int _bcm_field_data_qualifier_free(int unit,
                                          _field_data_qualifier_t *qual);
extern int _bcm_field_entry_qualifier_uint32_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 int qual_id, uint32 *data,
                                                 uint32 *mask);
extern int _bcm_field_entry_qualifier_uint64_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 int qual_id, uint64 *data,
                                                 uint64 *mask);
extern int _bcm_field_entry_qualifier_uint16_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 int qual_id, uint16 *data,
                                                 uint16 *mask);
extern int _bcm_field_entry_qualifier_uint8_get(int unit,
                                                bcm_field_entry_t entry,
                                                int qual_id, uint8 *data,
                                                uint8 *mask);
extern int _bcm_field_qual_tcam_key_mask_free(int unit, _field_entry_t *f_ent);
extern int _bcm_field_action_dest_check(int unit, _field_action_t *fa);
extern int _bcm_field_policy_set_l3_nh_resolve(int unit,  int value,
                                               uint32 *flags, int *nh_ecmp_id);
extern int _bcm_field_virtual_map_size_get(int unit, _field_stage_t *stage_fc,
                                           int *size);
extern int _bcm_field_entry_tcam_idx_get(int unit,_field_entry_t *f_ent,
                                         int *tcam_idx);
extern int _bcm_field_tcam_idx_to_slice_offset(int unit,
                                             _field_stage_t *stage_fc, 
                                             int instance, int tcam_idx,
                                             int *slice, int *offset);
extern int _bcm_field_slice_offset_to_tcam_idx(int unit,
                                               _field_stage_t *stage_fc,
                                               int instance, int slice,
                                               int offset, int *tcam_idx);
extern int _field_action_alloc(int unit, bcm_field_action_t action,
                               uint32 param0, uint32 param1,
                               uint32 param2, uint32 param3,
                               uint32 param4, uint32 param5,
                               _field_action_t **fa);

extern void _bcm_field_action_conf_t_init(_bcm_field_action_conf_t *ptr);
extern int _bcm_field_action_insert(int unit, _field_stage_t *stage_fc,
                              int action, _bcm_field_action_offset_t *ptr);

extern int _bcm_field_th_action_set(int unit, soc_mem_t mem, 
                                    _field_entry_t *f_ent, int tcam_idx, 
                                    _field_action_t *fa, uint32 *entbuf);
extern int _field_th_ingress_actions_init(int unit, _field_stage_t *stage_fc);

extern int
_field_qualify_VlanFormat_get(int                 unit,
                              bcm_field_entry_t   entry,
                              bcm_field_qualify_t qual_id,
                              uint8               *data,
                              uint8               *mask
                              );
extern int
_field_data_qualifier_get(int unit, int qual_id,
                           _field_data_qualifier_t *data_qual);
extern int
_field_data_qualifier_id_alloc(int unit, _field_stage_t *stage_fc,
                               bcm_field_data_qualifier_t *data_qualifier);
extern int
_field_data_qualifier_init(int                        unit,
                           _field_stage_t             *stage_fc,
                           _field_data_qualifier_t    *f_dq,
                           bcm_field_data_qualifier_t *data_qualifier
                           );
extern int
_field_data_qualifier_init2(int                        unit,
                            _field_stage_t             *stage_fc,
                            _field_data_qualifier_t    *f_dq
                            );
extern int
_field_data_qual_recover(int              unit,
                         _field_control_t *fc,
                         _field_stage_t   *stage_fc
                         );
extern void
_field_qset_udf_bmap_reinit(int              unit,
                            _field_stage_t   *stage_fc,
                            bcm_field_qset_t *qset,
                            int              qual_id
                            );
extern int
_field_trx2_udf_tcam_entry_l3_parse(int unit, uint32 *hw_buf,
                                    bcm_field_data_packet_format_t *pkt_fmt);

extern int
_field_trx2_udf_tcam_entry_misc_parse(int unit, uint32 *hw_buf,
                                    bcm_field_data_packet_format_t *pkt_fmt);
extern int
_field_trx2_udf_tcam_entry_vlanformat_parse(int unit, uint32 *hw_buf,
                                    uint16 *vlanformat);
extern int
_field_trx2_udf_tcam_entry_l2format_parse(int unit, uint32 *hw_buf,
                                    uint16 *l2format);

extern void
_field_scache_sync_data_quals_write(_field_control_t *fc, _field_data_control_t *data_ctrl);

extern int
_field_counter_mem_get(int unit, _field_stage_t *stage_fc,
                       soc_mem_t *counter_x_mem, soc_mem_t *counter_y_mem);

extern int
_bcm_field_counter32_collect_alloc(int unit, soc_mem_t mem, char *descr,
                                   _field_counter32_collect_t **ptr);
extern int
_bcm_field_counter_collect_deinit(int unit, _field_stage_t *stage_fc);

extern int _bcm_field_policer_mode_support(int              unit,
                                           _field_entry_t   *f_ent,
                                           int              level,
                                           _field_policer_t *f_pl
                                           );
extern int _field_stat_value_set(int unit,
                                 _field_stat_t *f_st,
                                 bcm_field_stat_t stat,
                                 uint64 value);

extern int _field_stat_value_get(int unit,
                                 int sync_mode,
                                 _field_stat_t *f_st,
                                 bcm_field_stat_t stat,
                                 uint64 *value);
extern int _field_flex_counter_free(int unit, _field_entry_t *f_ent,
                                                    _field_stat_t *f_st);
extern int _field_group_default_aset_set(int unit,
                                         _field_group_t *fg);
extern int _bcm_field_trx_egress_mode_get(int unit, _field_group_t *fg,
                                          uint8 *slice_mode);
extern int _field_group_slice_vmap_recovery(int unit,
                                        _field_control_t *fc,
                                        _field_stage_t *stage_fc);

extern int _field_slice_map_write(int unit,
                                        _field_control_t *fc,
                                        _field_stage_t *stage_fc);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
extern int _bcm_field_egress_key_attempt(int unit, _field_stage_t *stage_fc,
                              bcm_field_qset_t *qset_req,
                              uint8 key_pri, uint8 key_sec,
                              _field_group_t *fg);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
extern int _bcm_field_35bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter64_collect_t *result);
extern int _field_trx_action_copy_to_cpu(int unit, soc_mem_t mem,
                                         _field_entry_t *f_ent,
                                         _field_action_t *fa, uint32 *buf);
extern int
_bcm_field_counter64_collect_alloc(int unit, soc_mem_t mem, char *descr,
                                   _field_counter64_collect_t **ptr);

#endif /* BCM_TRX_SUPPORT */

extern int _bcm_esw_field_qset_udf_offsets_alloc(int unit,
    _field_stage_id_t stage, bcm_field_qset_t qset,
    int req_offsets, int offset_array[], int *max_chunks);

#if defined BCM_TRIDENT2_SUPPORT
extern int _field_td2_group_install(int unit, _field_group_t *fg);
extern int _field_td2_selcodes_install(int unit, _field_group_t *fg,
                           uint8 slice_num, bcm_pbmp_t pbmp, int selcode_idx);
extern int _field_td2_slice_clear(int unit, _field_group_t *fg,
                                              _field_slice_t *fs);
extern int _field_td2_entry_move(int unit, _field_entry_t *f_ent,
                      int parts_count, int *tcam_idx_old, int *tcam_idx_new);
extern int _field_td2_action_support_check(int unit, _field_entry_t *f_ent,
                                    bcm_field_action_t action, int *result);
extern int _bcm_field_td2_action_conflict_check (int unit, _field_entry_t *f_ent,
                             bcm_field_action_t action1, bcm_field_action_t action);
extern int _field_td2_action_params_check (int unit, _field_entry_t *f_ent,
                                                  _field_action_t *fa);
extern int _field_td2_selcode_get(int unit, _field_stage_t *stage_fc,
                                     bcm_field_qset_t *qset_req,
                                     _field_group_t *fg);

#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#define _FIELD_IFP_DATA_START 0xDEADBEE0
#define _FIELD_IFP_DATA_END   0XDEADBEEF
#define _FIELD_EFP_DATA_START 0xDEADBEE1
#define _FIELD_EFP_DATA_END   0XDEADBEEE
#define _FIELD_VFP_DATA_START 0xDEADBEE2
#define _FIELD_VFP_DATA_END   0XDEADBEED
#define _FIELD_EXTFP_DATA_START 0xDEADBEE3
#define _FIELD_EXTFP_DATA_END   0XDEADBEEC
#define _FIELD_SCACHE_PART_0    0x0
#define _FIELD_SCACHE_PART_1    0x1

#define _FIELD_GROUP_NEXT_GROUP_VALID (0x1 << 0) /* Next group is valid
                                                  * in scache
                                                  */
#define _FIELD_GROUP_AUTO_EXPANSION   (0x1 << 7) /* Indicates group can be
                                                  * expanded to multiple slices
                                                  */

#define _FIELD_QSET_ITER(qset, q) \
        for ((q) = 0; (q) < (int)bcmFieldQualifyCount; (q)++) \
            if (BCM_FIELD_QSET_TEST((qset), (q)))
#define _FIELD_QSET_INCL_INTERNAL_ITER(qset, q) \
        for ((q) = 0; (q) < (int)_bcmFieldQualifyCount; (q)++) \
            if (BCM_FIELD_QSET_TEST((qset), (q)))

extern int _field_table_read(int unit, soc_mem_t mem, char **buffer_p,
    const char *buffer_name_p);
extern void _bcm_field_last_alloc_eid_incr(void);
extern int _bcm_field_last_alloc_eid_get(void);
extern int _field_slice_expanded_status_get(int unit, _field_control_t *fc, _field_stage_t *stage_fc,
                                 int *expanded, int *slice_master_idx);
extern int _mask_is_set(int unit, _bcm_field_qual_offset_t *offset, uint32 *buf,
             _field_stage_id_t stage_id);
extern int _field_physical_to_virtual(int unit, int slice_idx, _field_stage_t *stage_fc);
extern int _bcm_esw_field_scache_sync(int unit);
extern int _bcm_esw_field_tr2_ext_scache_size(int       unit,
                                              unsigned  part_idx
                                              );
extern int _field_trx_redirect_profile_get(int unit, soc_profile_mem_t
                                                           **redirect_profile);
extern unsigned _field_trx_ext_scache_size(int       unit,
                                                  unsigned  part_idx,
                                                  soc_mem_t *mems
                                                  );
extern int _field_group_info_retrieve(int               unit,
                                      bcm_port_t        port,
                                      bcm_field_group_t *gid,
                                      int               *priority,
                                      int               *action_res_id,
                                      int               *group_flags,
                                      bcm_field_qset_t  *qset,
                                      _field_control_t  *fc
                                      );
extern int _field_scache_slice_group_recover(int unit, _field_control_t *fc,
    int slice_num, int *multigroup);
extern int _field_entry_info_retrieve(int unit, bcm_field_entry_t *eid,
    int *prio, _field_control_t *fc, int multigroup, int *prev_prio,
    bcm_field_stat_t *sid, bcm_policer_t *pid, uint8 *dvp_type,
    _field_entry_wb_t *f_ent_wb);
extern void _field_scache_slice_group_free(int unit, _field_control_t *fc,
    int slice_num);
extern void _field_scache_stage_hdr_save(_field_control_t *fc,
                                         uint32           header
                                         );
extern int _field_scache_stage_hdr_chk(_field_control_t *fc,
                                       uint32           header
                                       );
extern int _field_range_check_reinit(int unit, _field_stage_t *stage_fc,
                                     _field_control_t *fc);

extern int _field_table_pointers_init(int unit,
                                      _field_table_pointers_t *field_tables);

#ifdef BCM_FIREBOLT_SUPPORT
extern int
_field_fb_meter_recover(int             unit,
                        _field_entry_t  *f_ent,
                        _meter_config_t *meter_conf,
                        int             part,
                        bcm_policer_t   pid
                        );
extern int _field_fb_stage_reinit(int unit, _field_control_t *fc,
    _field_stage_t *stage_fc);
extern int _field_scache_sync(int unit, _field_control_t *fc,
        _field_stage_t *stage_fc);
extern int _field_fb_slice_is_primary(int unit, int slice_index,
    int *is_primary_p, int *slice_mode_p);
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT)
extern int _field_fb2_stage_ingress_reinit(int unit, _field_control_t *fc,
                                           _field_stage_t *stage_fc);
extern int _field_fb2_stage_egress_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _field_fb2_stage_lookup_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
#endif
#if defined(BCM_RAVEN_SUPPORT)
extern int _field_raven_stage_reinit(int unit, _field_control_t *fc,
                                     _field_stage_t *stage_fc);
#endif


#if defined(BCM_TRX_SUPPORT)
extern int _field_tr2_stage_ingress_reinit(int unit, _field_control_t *fc,
                                           _field_stage_t *stage_fc);
extern int _field_tr2_stage_egress_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _field_tr2_stage_lookup_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _field_tr2_stage_external_reinit(int unit, _field_control_t *fc,
                                            _field_stage_t *stage_fc);
extern int _field_tr2_scache_sync(int unit, _field_control_t *fc,
                                  _field_stage_t *stage_fc);
extern int _field_tr2_ifp_slice_expanded_status_get(int unit, _field_control_t *fc,
                                                    _field_stage_t *stage_fc,
                                                    int *expanded, int *slice_master_idx);
extern int _field_trx_scache_slice_group_recover(int unit,
                                                 _field_control_t *fc,
                                                 int slice_num,
                                                 int *multigroup,
                                                 _field_stage_t *stage_fc,
                                                 int recovered_count);
extern int _field_tr2_slice_key_control_entry_recover(int unit,
                                                      unsigned slice_num,
                                                      _field_sel_t *sel);
extern int _field_tr2_group_construct_alloc(int unit, _field_group_t **pfg);
extern void _field_tr2_ingress_entity_get(int unit,
                                          int slice_idx,
                                          uint32 *fp_tcam_buf,
                                          int slice_ent_cnt,
                                          _field_stage_t *stage_fc,
                                          int8 *ingress_entity_sel);
extern int _field_tr2_group_construct(int unit,
                                      _field_hw_qual_info_t *hw_sel,
                                      int intraslice,
                                      int paired,
                                      _field_control_t *fc,
                                      bcm_port_t port,
                                     _field_stage_id_t stage_id,
                                     int slice_idx);
extern int _field_trx_entry_info_retrieve(int unit,
                               bcm_field_entry_t *eid,
                               int               *prio,
                               _field_control_t  *fc,
                               int               multigroup,
                               int               *prev_prio,
                               bcm_field_stat_t  *sid,
                               bcm_policer_t     *pid,
                               _field_stage_t    *stage_fc,
                               uint8             *dvp_type,
                               _field_entry_wb_t *f_ent_wb);
extern int _field_tr2_actions_recover(int unit,
                                      soc_mem_t policy_mem,
                                      uint32 *policy_entry,
                                      _field_entry_t *f_ent,
                                      int part,
                                      bcm_field_stat_t sid,
                                      bcm_policer_t pid,
                                      _field_entry_wb_t *f_ent_wb);
extern int
_field_trx_actions_recover_action_add(int                unit,
                                      _field_entry_t     *f_ent,
                                      bcm_field_action_t action,
                                      uint32 param0, uint32 param1,
                                      uint32 param2, uint32 param3,
                                      uint32 param4, uint32 param5,
                                      uint32 hw_index);
extern int _field_tr2_stage_reinit_all_groups_cleanup(int unit,
                                                      _field_control_t *fc,
                                                      unsigned stage_id,
                                _field_table_pointers_t *fp_table_pointers);
extern int _field_tr2_group_entry_write(int unit,
                                        int slice_idx,
                                        _field_slice_t *fs,
                                        _field_control_t *fc,
                                        _field_stage_t *stage_fc);

extern int _bcm_field_trx_meter_rate_burst_recover(uint32 unit,
                                    uint32 meter_table,
                                    uint32 mem_idx,
                                    uint32   *prate,
                                    uint32   *pburst
                                    );
extern int _field_adv_flex_stat_info_retrieve(int unit, int stat_id);
extern int _field_free_wb_entry (int unit, _field_entry_wb_t **f_ent_wb);
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
extern void _field_extract(uint32 *buf, int offset, int width, uint32 *value);
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_field_tr3_stage_lookup_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_tr3_stage_ingress_reinit(int unit,
                                               _field_control_t *fc,
                                               _field_stage_t   *stage_fc);
extern int _bcm_field_tr3_stage_egress_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_tr3_stage_external_reinit(int unit,
                                                _field_control_t *fc,
                                                _field_stage_t *stage_fc);
extern int _bcm_field_tr3_counter_recover(int unit,
                                          _field_entry_t *f_ent,
                                          uint32 *policy_entry,
                                          int part,
                                          bcm_field_stat_t sid);
extern int _bcm_field_tr3_scache_sync(int unit,
                                      _field_control_t *fc,
                                      _field_stage_t *stage_fc);
extern int _bcm_field_tr3_meter_recover(int unit,
                                        _field_entry_t *f_ent,
                                        int part,
                                        bcm_policer_t pid,
                                        uint32 level,
                                        soc_mem_t policy_mem,
                                        uint32 *policy_buf
                                        );
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
extern int _bcm_field_td2_scache_sync(int unit,
                                      _field_control_t *fc,
                                      _field_stage_t *stage_fc);
extern int _bcm_field_td2_stage_lookup_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
extern int _field_kt2_stage_lookup_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
#endif /* BCM_KATANA2_SUPPORT */

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
extern int _bcm_field_fb_slice_enable_set(int            unit,
                                          _field_group_t *fg,
                                          uint8          slice,
                                          int enable
                                          );
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_field_cleanup(int unit);
#else
#define _bcm_field_cleanup(u)        (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BROADCOM_DEBUG
extern void _field_qset_debug(bcm_field_qset_t qset);
extern void _field_qset_dump(char *prefix, bcm_field_qset_t qset, char* suffix);
extern void _field_aset_dump(char *prefix, bcm_field_aset_t aset, char* suffix);
extern int _field_group_dump_brief(int unit, bcm_field_group_t group);
extern int _field_dump_brief(int unit, const char *pfx);
extern int _field_dump_class(int unit, const char *pfx);
extern int _bcm_field_presel_dump(int unit, bcm_field_presel_t presel_id);
extern int
 _bcm_field_presel_qualifiers_dump(int unit, bcm_field_presel_t presel_id);
#endif /* BROADCOM_DEBUG */
extern char *_field_qual_name(bcm_field_qualify_t qid);


/* User-Defined Field (UDF) L2 Flags. (0 to 2 bits) */
#define _BCM_FIELD_USER_L2_MASK 0x00000003
#define _BCM_FIELD_USER_L2_ETHERNET2 (0 << 0)
#define _BCM_FIELD_USER_L2_SNAP (1 << 0)
#define _BCM_FIELD_USER_L2_LLC (2 << 0)
#define _BCM_FIELD_USER_L2_OTHER (3 << 0)

/* User-Defined Field (UDF) VLAN Flags. (3 to 4 bits) */
#define _BCM_FIELD_USER_VLAN_MASK 0x00000018
#define _BCM_FIELD_USER_VLAN_NOTAG (0 << 3)
#define _BCM_FIELD_USER_VLAN_ONETAG (1 << 3)
#define _BCM_FIELD_USER_VLAN_TWOTAG (2 << 3)
#define _BCM_FIELD_USER_VLAN_NOTUSED (3 << 3)

/* User-Defined Field (UDF) IP Flags. (6 to 10 bits) */
#define _BCM_FIELD_USER_IP_MASK 0x000007c0
#define _BCM_FIELD_USER_IP4_HDR_ONLY (0 << 6)
#define _BCM_FIELD_USER_IP6_HDR_ONLY (1 << 6)
#define _BCM_FIELD_USER_IP6_FRAGMENT (2 << 6)
#define _BCM_FIELD_USER_IP4_OVER_IP4 (3 << 6)
#define _BCM_FIELD_USER_IP6_OVER_IP4 (4 << 6)
#define _BCM_FIELD_USER_IP4_OVER_IP6 (5 << 6)
#define _BCM_FIELD_USER_IP6_OVER_IP6 (6 << 6)
#define _BCM_FIELD_USER_GRE_IP4_OVER_IP4 (7 << 6)
#define _BCM_FIELD_USER_GRE_IP6_OVER_IP4 (8 << 6)
#define _BCM_FIELD_USER_GRE_IP4_OVER_IP6 (9 << 6)
#define _BCM_FIELD_USER_GRE_IP6_OVER_IP6 (10 << 6)
#define _BCM_FIELD_USER_ONE_MPLS_LABEL (11 << 6)
#define _BCM_FIELD_USER_TWO_MPLS_LABELS (12 << 6)
#define _BCM_FIELD_USER_IP6_FRAGMENT_OVER_IP4 (13 << 6)
#define _BCM_FIELD_USER_IP_NOTUSED (14 << 6)
#define _BCM_FIELD_USER_ANY_MPLS_LABELS (15 << 6)
#define _BCM_FIELD_USER_ONE_FCOE_HEADER (16 << 6)
#define _BCM_FIELD_USER_TWO_FCOE_HEADER (17 << 6)

/* Flags for Auto Adjust Offset with IP options length. */
#define _BCM_FIELD_USER_OPTION_ADJUST (1 << 30)

/* Flags for Auto Adjust Offset with GRE header options length. */
#define _BCM_FIELD_USER_GRE_OPTION_ADJUST (1 << 29)

/* User-Defined Field (UDF) Miscellaneous Flags (13 to 21 bits) */
#define _BCM_FIELD_USER_F_ENCAP_STACK_ANY (0 << 13)
#define _BCM_FIELD_USER_F_NO_ENCAP_STACK  (1 << 13)
#define _BCM_FIELD_USER_F_ENCAP_STACK     (2 << 13)

#define _BCM_FIELD_USER_F_VNTAG_ANY      (0 << 15)
#define _BCM_FIELD_USER_F_NO_VNTAG       (1 << 15)
#define _BCM_FIELD_USER_F_VNTAG          (2 << 15)

#define _BCM_FIELD_USER_F_CNTAG_ANY      (0 << 17)
#define _BCM_FIELD_USER_F_NO_CNTAG       (1 << 17)
#define _BCM_FIELD_USER_F_CNTAG          (2 << 17)

#define _BCM_FIELD_USER_F_ETAG_ANY       (0 << 19)
#define _BCM_FIELD_USER_F_NO_ETAG        (1 << 19)
#define _BCM_FIELD_USER_F_ETAG           (2 << 19)

#define _BCM_FIELD_USER_F_ICNM_ANY       (0 << 21)
#define _BCM_FIELD_USER_F_NO_ICNM        (1 << 21)
#define _BCM_FIELD_USER_F_ICNM           (2 << 21)

#if defined(BCM_TOMAHAWK_SUPPORT)
/* Memory Types */
#define _BCM_FIELD_MEM_TYPE_IFP_LT    1
#define _BCM_FIELD_MEM_TYPE_EM_LT     2

/* Memory View Type flags */
#define _BCM_FIELD_MEM_VIEW_TYPE_TCAM           1  /* TCAM view */
#define _BCM_FIELD_MEM_VIEW_TYPE_DATA           2  /* DATA view */
#define _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB 4  /* TCAM_DATA combined view */


/* ---- Macros/Enums/Structures related to Preselector Module ---- */

/* Max Preselector Entries Per Pipe */
#define _FP_FIELD_PRESEL_ENTRIES_PER_PIPE  31

/* Max Presel IDs possible value. */
#define _BCM_FIELD_PRESEL_ID_MAX BCM_FIELD_PRESEL_SEL_MAX

/* Validate the Presel ID range */
#define _BCM_FIELD_IS_PRESEL_VALID(_id)          \
            ((_id < 0 || _id >= _BCM_FIELD_PRESEL_ID_MAX) ? FALSE:TRUE)

/* Validate the Presel ID */
#define _BCM_FIELD_IS_PRESEL_ENTRY(_e)           \
            ((_e & BCM_FIELD_QUALIFY_PRESEL) ? TRUE:FALSE)

/* Retrieve the Presel ID from the Entry ID */
#define _BCM_FIELD_PRESEL_FROM_ENTRY(_e)    \
            (_e & ~(BCM_FIELD_QUALIFY_PRESEL))

/* Enums related to Preselector Flags */
typedef enum _bcm_field_presel_flags_e {
   _bcmFieldPreselCreateWithId = 1,    /* Indicates Presel creation with id. */
   _bcmFieldPreselCount                /* Indicates Last entry - not in use. */
} _bcm_field_presel_flags_t;

/* Structure related to Preselector */
typedef struct _field_presel_info_s {
   uint32                  presel_limit;       /* max preselectors */
   uint32                  last_allocated_id;  /* Last allocated id */
   bcm_field_presel_set_t  presel_set;         /* set of preselectors created */
   bcm_field_presel_set_t  operational_set;    /* Indicates Operational status
                                                  of a presel IDs */
} _field_presel_info_t;

/* Add qualifier to Preselector stage qualifiers set. */
#define _FP_PRESEL_QUAL_ADD(_unit_, _stage_fc_, _qual_id_,                    \
                     _pri_slice_sel_, _pri_slice_sel_val_,                    \
                     _qual_type,                                              \
                     _offset_0_, _width_0_)                                   \
            do {                                                              \
                _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                (_fp_qual_conf_).offset.num_offsets     = 1;                  \
                (_fp_qual_conf_).offset.offset[0]       = _offset_0_;         \
                (_fp_qual_conf_).offset.width[0]        = _width_0_;          \
                _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_),          \
                                             (_qual_type), (_qual_id_),       \
                                             &(_fp_qual_conf_));              \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

/* Preselector TCAM Info */
typedef struct _field_presel_tcam_s {
    uint32 *key;        /* HW entry key replica.    */
    uint32 *mask;       /* HW entry mask replica.   */
    uint16 key_size;    /* HW entry Key size.       */
} _field_presel_tcam_t;


/* Preselector HW Data */
typedef struct _field_presel_data_s {
    uint32 *data;       /* HW entry data replica.   */
    uint16 data_size;   /* HW entry Data size.      */
} _field_presel_data_t;

/* Preselector Entry Structure */
typedef struct _field_presel_entry_s {
   bcm_field_presel_t     presel_id;    /* Unique Identifier */
   uint32                 flags;        /* Entry flags _FP_ENTRY_XXX */
   int                    priority;     /* Entry Priority */
   int                    hw_index;     /* Allocated hw resource index. */    
   int                    slice_index;  /* Entry Presel slice index. */
   _field_presel_tcam_t   lt_tcam;      /* Fields to be written in LT TCAM */
   _field_presel_data_t   lt_data;      /* Fields to be written in LT DATA */
   bcm_field_qset_t       p_qset;       /* Contains Preselector Qualifiers
                                           associated with the presel entry */
   _field_action_t        *actions;     /* Reference to lined list of Actions   
                                           associate with entry. */
   _field_group_t         *group;       /* Reference to the Group with this
                                           the entry is associated */
   _field_lt_slice_t      *lt_fs;       /* LT slice where entry lives. */
   
   struct _field_presel_entry_s *next;  /* Reference to next presel entry */
} _field_presel_entry_t;

extern int
_bcm_field_th_stage_preselector_init(int unit,
                                     _field_control_t *fc,
                                     _field_stage_t *stage_fc);
extern int
_bcm_field_th_preselector_deinit(int unit,
                                 _field_control_t *fc,
                                 _field_stage_t *stage_fc);
extern int
_bcm_field_th_presel_create(int unit,
                            _bcm_field_presel_flags_t flags,
                            bcm_field_presel_t *presel);
extern int
_bcm_field_th_presel_destroy(int unit,
                             bcm_field_presel_t presel_id);
extern int
_bcm_field_presel_entry_get(int unit,
                            bcm_field_presel_t presel_id,
                            _field_presel_entry_t **presel);
extern int
_bcm_field_presel_qualify_set(int unit,
                              bcm_field_entry_t eid,
                              bcm_field_qualify_t qual,
                              uint32 *data,
                              uint32 *mask);
extern int
_bcm_field_th_lt_tcam_policy_mem_get(int unit, _field_stage_t *stage_fc,
                                     int instance, int mem_type, int view_type,
                                     soc_mem_t *tcam_mem, soc_mem_t *data_mem);
extern int
_bcm_field_presel_qualifier_get(int unit,
                                bcm_field_entry_t entry,
                                int qual_id,
                                _bcm_field_qual_data_t q_data,
                                _bcm_field_qual_data_t q_mask);
extern int
_bcm_field_presel_action_add(int unit, _field_control_t *fc, 
                             bcm_field_entry_t entry, _field_action_t *fa);
extern int 
_bcm_field_presel_action_delete(int unit, bcm_field_entry_t entry, 
                                bcm_field_action_t action, 
                                uint32 param0, uint32 param1);
extern int
_bcm_field_presel_action_get(int unit,
                             bcm_field_entry_t entry,
                             bcm_field_action_t action,
                             uint32 *param0,
                             uint32 *param1);
extern int
_bcm_field_presel_group_add(int unit, _field_group_t *fg,
                            bcm_field_presel_t presel);
extern int
_bcm_field_presel_entry_prio_set(int unit, bcm_field_entry_t entry, int prio);
extern int
_bcm_field_presel_entry_tcam_idx_get(int unit, _field_presel_entry_t *f_presel,
                                     _field_lt_slice_t *lt_fs, int *tcam_idx);
extern int
_bcm_field_th_presel_mem_data_get(int unit,
                                  _field_stage_t *stage_fc,
                                  int inst,
                                  _field_presel_entry_t *f_presel,
                                  _field_presel_data_t *p_data);
extern int
_bcm_field_presel_entry_actions_set(int unit, soc_mem_t data_mem,
                                    _field_presel_entry_t *f_presel,
                                    uint32 *buf);
extern int
_bcm_field_th_lt_entry_config_set(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, _field_lt_slice_t *lt_fs,
                                  _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_lt_entry_hw_install(int unit, soc_mem_t lt_tcam_mem, int tcam_idx,
                                  uint32 *key, uint32 *mask, uint32 *data,
                                  uint8 valid);
extern int
_bcm_field_th_lt_entry_parts_install(int unit, _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_lt_entry_install(int unit, _field_presel_entry_t *f_presel,
                               _field_lt_slice_t *lt_fs);
extern int
_bcm_field_th_group_keygen_profiles_index_alloc(int unit,
                                           _field_stage_t *stage_fc,
                                           _field_group_t *fg, int part_index);
extern int
_bcm_field_th_lt_entry_data_value_set(int unit, _field_stage_t *stage_fc,
                                      _field_group_t *fg, int index,
                                      soc_mem_t lt_data_mem, uint32 *data);
extern int
_bcm_field_th_lt_entry_part_tcam_idx_get(int unit, _field_group_t *fg,
                                         uint32 idx_pri, uint8 ent_part,
                                         int *idx_out);
extern int
_bcm_field_th_lt_entry_phys_destroy(int unit, _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_group_presel_set(int unit, bcm_field_group_t group,
                               bcm_field_presel_set_t *presel);
extern int
_bcm_field_th_group_presel_get(int unit, bcm_field_group_t group,
                               bcm_field_presel_set_t *presel);
extern int
_bcm_field_th_ingress_logical_table_map_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg);
extern int
_bcm_field_th_ingress_lt_partition_prio_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg, int lt_id,
                                              int slice);
extern int
 _bcm_field_th_lt_part_prio_value_get(int unit, _field_group_t *fg,
                                      int *lt_part_prio);
extern int
 _bcm_field_th_lt_part_prio_reset(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, _field_slice_t *fs);
extern int
_bcm_field_th_presel_entry_hw_remove(int unit, _field_presel_entry_t *f_presel);


#endif  /* BCM_TOMAHAWK_SUPPORT */
#endif  /* BCM_FIELD_SUPPORT */
#endif  /* !_BCM_INT_FIELD_H */
