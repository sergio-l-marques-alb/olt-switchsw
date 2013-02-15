/*
 * $Id: oam.h 1.21.4.2 Broadcom SDK $
 * 
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
 */

#ifndef __BCM_INT_OAM_H__
#define __BCM_INT_OAM_H__

#include <bcm/oam.h>
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm/field.h>
#include <bcm_int/esw/trunk.h>
#endif

extern int bcm_esw_oam_lock(int unit);
extern int bcm_esw_oam_unlock(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_oam_sync(int unit);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_oam_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#define _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED 0xFFFFFFFF

#define _BCM_OAM_MAC_DA_UPPER_32 0x0180C200
#define _BCM_OAM_MAC_DA_LOWER_13 0x0030 /* To be >> 3 before use */

#define _BCM_OAM_INVALID_INDEX (-1) /* Invalid Index */

/*
 * Typedef:
 *      _bcm_oam_group_t
 * Purpose:
 *      Group information.
 */
struct _bcm_oam_group_s
{
    int     in_use;                          /* Group status.   */
    uint8   name[BCM_OAM_GROUP_NAME_LENGTH]; /* Group name.     */
};
typedef struct _bcm_oam_group_s _bcm_oam_group_t;

/*
 * Typedef:
 *      _bcm_oam_endpoint_t
 * Purpose:
 *      Endpoint information.
 *      One structure for each endpoint created on device.
 */
struct _bcm_oam_endpoint_s
{
    int                 in_use;             /* Endpoint status.             */
    int                 is_remote;          /* Local or Remote MEP.         */
    bcm_oam_group_t     group_index;        /* Group ID.                    */
    uint16              name;               /* Endpoint name.               */
    int                 level;              /* Maintenance domain level.    */
    bcm_vlan_t          vlan;               /* Vlan membership.             */
    uint32              glp;                /* Generic local port number.   */
    int                 local_tx_enabled;   /* CCM Tx enabled.              */
    int                 local_rx_enabled;   /* CCM Rx enabled.              */
    int                 remote_index;       /* RMEP entry hardware index.   */
    int                 local_tx_index;     /* LMEP Tx endpoint Hw index.   */
    int                 local_rx_index;     /* LMEP Rx endpoint Hw index.   */
#if defined(BCM_ENDURO_SUPPORT)
    uint32              vp;                 /* Virtual Port.                */
    uint32              flags;              /* Endpoint flags.              */
    int                 lm_counter_index;   /* LM counter index.            */
    int                 pri_map_index;      /* LM Priority Map Index.       */
    bcm_field_entry_t   vfp_entry;          /* Field Lookup stage entry ID. */
    bcm_field_entry_t   fp_entry_tx;        /* FP LM Tx entry ID.           */
    bcm_field_entry_t   fp_entry_rx;        /* FP LM Rx entry ID.           */
    bcm_field_entry_t   fp_entry_trunk[BCM_SWITCH_TRUNK_MAX_PORTCNT];
                                            /* FP trunk entry ID            */
#endif
#if defined(BCM_KATANA_SUPPORT)
    int                 opcode_profile_index; /* Opcode profile table index.*/
#endif
};
typedef struct _bcm_oam_endpoint_s _bcm_oam_endpoint_t;


/*
 * Typedef:
 *      _bcm_oam_event_handler_t
 * Purpose:
 *      Event handler information.
 */
struct _bcm_oam_event_handler_s
{
    bcm_oam_event_types_t           event_types; /* Type of event detected. */
    bcm_oam_event_cb                cb;          /* Registered callback
                                                    routine. */
    void                            *user_data;  /* Application supplied
                                                    data. */
    struct _bcm_oam_event_handler_s *next_p;     /* Pointer to next event. */
};

typedef struct _bcm_oam_event_handler_s _bcm_oam_event_handler_t;


/*
 * Typedef:
 *      __bcm_oam_info_t
 * Purpose:
 *      One structure for each StrataSwitch Device that holds the global
 *      OAM metadata for one device.
 */
struct _bcm_oam_info_s
{
    int                 initialized;            /* TRUE if oam module has 
                                                   been been initialized.   */
    int                 group_count;            /* No. of oam groups supported
                                                   on this device.          */
    _bcm_oam_group_t    *groups;                /* Pointer to oam group name 
                                                   and group status.        */
    int                 local_rx_endpoint_count; /* Total RX LMEP indices
                                                    supported on device.    */
    int                 local_tx_endpoint_count; /* Total TX LMEP indices
                                                    supported on device.    */
    int                 remote_endpoint_count;   /* Total RMEP indices
                                                    supported on device.    */
    int                 endpoint_count;          /* Total no. of endpoints
                                                    supported on device.
                                                    (local_rx + local_tx
                                                    remote).                */
#if defined(BCM_ENDURO_SUPPORT)
    bcm_field_qset_t    vfp_qs;                  /* Vlan and Mac match qset.*/
    bcm_field_qset_t    fp_vp_qs;                /* Virtual port qset.      */
    bcm_field_qset_t    fp_glp_qs;               /* Physical port qset.     */
    bcm_field_group_t   vfp_group;               /* VFP Group ID            */
    bcm_field_group_t   fp_vp_group;             /* IFP Group ID for Logical
                                                    ports                   */
    bcm_field_group_t   fp_glp_group;            /* IFP Group ID for Physical
                                                    Ports*/
    int                 vfp_entry_count;         /* VFP group entries used. */
    int                 fp_vp_entry_count;       /* IFP group entries used. */
    int                 fp_glp_entry_count;      /* IFP group entries used. */
    int                 lm_counter_count;        /* Total Loss Measurement
                                                    counters supported on
                                                    device                  */
    SHR_BITDCL          *lm_counter_in_use;      /* Loss Measurement counters
                                                    used.                   */
#endif
    SHR_BITDCL          *local_tx_endpoints_in_use; /* TX LMEP indices that
                                                       are in use.          */
    SHR_BITDCL          *local_rx_endpoints_in_use; /* RX LMEP indices that
                                                       are in use.          */
    SHR_BITDCL          *remote_endpoints_in_use;   /* RMEP indices that are
                                                       in use.              */
    bcm_oam_endpoint_t  *remote_endpoints;          /* Endpoint ID/Index.   */
    _bcm_oam_endpoint_t *endpoints;                 /* Endpoint information.*/
    _bcm_oam_event_handler_t *event_handler_list_p; /* Event handlers
                                                       callbacks.           */
    int                 event_handler_count[bcmOAMEventCount]; /* Events
                                                                  occurrance
                                                                  count.    */
};

typedef struct _bcm_oam_info_s _bcm_oam_info_t;


/*
 * Typedef:
 *      _bcm_oam_fault_t
 * Purpose:
 *      OAM group defects information structure.
 */
struct _bcm_oam_fault_s
{
    int     current_field;      /* Defects field.            */
    int     sticky_field;       /* Defects sticky field.     */
    uint32  mask;               /* Defects mask bits.        */
    uint32  clear_sticky_mask;  /* Sticky defects mask bits. */
};

typedef struct _bcm_oam_fault_s _bcm_oam_fault_t;


/*
 * Typedef:
 *      _bcm_oam_interrupt_t
 * Purpose:
 *      OAM group interrupt information structure.
 */
struct _bcm_oam_interrupt_s
{
    soc_reg_t            status_register;      /* Interrupt status register.*/
    soc_field_t          endpoint_index_field; /* Remote endpoint index.    */
    soc_field_t          group_index_field;    /* Group index.              */
    soc_field_t          status_field;         /* Interrupt status field.   */
    bcm_oam_event_type_t event_type;           /* Event type.               */
};

typedef  struct _bcm_oam_interrupt_s _bcm_oam_interrupt_t;


/*
 * Typedef:
 *      oam_hdr_t
 * Purpose:
 *      OAM PDU header information structure.
 *      Used in OAM application diagnostics code.
 */
struct oam_hdr_s {
    uint8 mdl_ver;          /* Maintenance domain level + Version.  */
    uint8 opcode;           /* Identifies OAM packet type.          */
    uint8 flags;            /* Infomation depends on OAM PDU type.  */
    uint8 first_tlvoffset;  /* Offset to first TLV.                 */
};

typedef struct oam_hdr_s oam_hdr_t;


/*
 * Typedef:
 *      oam_lm_pkt_t
 * Purpose:
 *      OAM Loss Measurement counter information.
 */
struct oam_lm_pkt_s {
    uint32 txfcf; /* Value of local counter TxFCl. */
    uint32 rxfcf; /* Value of local counter RxFCl. */
    uint32 txfcb; /* Value of TxFCf in the last received CCM frame from peer
                     MEP. */
};

typedef struct oam_lm_pkt_s oam_lm_pkt_t;


/*
 * Typedef:
 *      oam_dm_pkt_t
 * Purpose:
 *      OAM Delay Measurement time stamp information.
 */
struct oam_dm_pkt_s {
    uint32 txtsf_upper; /* Timestamp at the transmission time of DMM frame.  */
    uint32 txtsf;
    uint32 rxtsf_upper; /* Timestamp at the time of receiving the DMM frame. */
    uint32 rxtsf;
    uint32 txtsb_upper; /* Timestamp at the transmission time of DMR frame.  */
    uint32 txtsb;
    uint32 rxtsb_upper; /* Timestamp at the time of receiving the DMR frame. */
    uint32 rxtsb;
};

typedef struct oam_dm_pkt_s oam_dm_pkt_t;
#endif /* !__BCM_INT_OAM_H__ */
