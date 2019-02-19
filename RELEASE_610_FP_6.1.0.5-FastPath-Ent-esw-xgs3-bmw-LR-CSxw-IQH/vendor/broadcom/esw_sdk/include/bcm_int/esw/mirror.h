/*
 * $Id: mirror.h,v 1.1 2011/04/18 17:10:56 mruas Exp $
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
 * This file contains Mirroring definitions internal to the BCM library.
 */

#ifndef _BCM_INT_MIRROR_H
#define _BCM_INT_MIRROR_H

#define _BCM_MIRROR_INVALID_MTP    (-1)


/* Internal mirror destination flags. */
#define _BCM_MIRROR_DESTINATION_LOCAL (1 << 31)

/* Mirroring destination managment structure. */
typedef struct _bcm_mirror_dest_config_s {
    bcm_mirror_destination_t mirror_dest; /* Mirroring destination.  */
    int                      ref_count;   /* Reference count.        */
} _bcm_mirror_dest_config_t, *_bcm_mirror_dest_config_p;

/* MTP managment structure. */
typedef struct _bcm_mtp_config_s {
    bcm_gport_t dest_id;      /* Mirroring destination id. */
    int         ref_count;     /* Reference count.          */
} _bcm_mtp_config_t, *_bcm_mtp_config_p;


#define BCM_MIRROR_MTP_ONE                    (1 << 0)
#define BCM_MIRROR_MTP_TWO                    (1 << 1)
#define BCM_MIRROR_MTP_THREE                  (1 << 2)
#define BCM_MIRROR_MTP_FOUR                   (1 << 3)
#define BCM_MIRROR_MTP_MAX                    4

#define BCM_XGS3_MIRROR_MTP_MASK              (BCM_MIRROR_MTP_ONE)
#define BCM_TRX_MIRROR_MTP_MASK (BCM_MIRROR_MTP_ONE | BCM_MIRROR_MTP_TWO)
#define BCM_TR2_MIRROR_MTP_MASK (BCM_MIRROR_MTP_ONE | BCM_MIRROR_MTP_TWO | \
                                 BCM_MIRROR_MTP_THREE | BCM_MIRROR_MTP_FOUR)

#define BCM_MIRROR_MTP_FLEX_SLOT_SHIFT        16
#define BCM_MIRROR_MTP_FLEX_SLOT_MASK         0x3

#define BCM_MTP_SLOT_TYPE_FP                  0
#define BCM_MTP_SLOT_TYPE_IPFIX               1
#define BCM_MTP_SLOT_TYPES                    2

/* Module control structure. */
typedef struct _bcm_mirror_config_s {
    int                       mode;           /* L2/L2_L3/DISABLED.        */
    _bcm_mirror_dest_config_p dest_arr;       /* Mirror destination array  */
    uint8                     dest_count;     /* Mirror destinations size. */
    _bcm_mtp_config_p         ing_mtp;        /* Ingress mtp array.        */
    uint8                     ing_mtp_count;  /* Ingress mtp array size.   */
    _bcm_mtp_config_p         egr_mtp;        /* Egress mtp array.         */
    uint8                     egr_mtp_count;  /* Egress mtp array size.    */
    uint8                     mtp_dev_mask;   /* MTPs allowed on device    */
    uint8                     mtp_mode_bmp;   /* MTP ingress/egress bitmap */
    int                       mtp_mode_ref_count[BCM_MIRROR_MTP_MAX];
                                              /* MTP container references  */
    _bcm_mtp_config_p         egr_true_mtp;   /* True egress mtp array.    */
    uint8                     egr_true_mtp_count;  /* True egress mtp array size. */
    _bcm_mtp_config_p         mtp_slot[BCM_MTP_SLOT_TYPES];
                                              /* FP mtp slot arrays. */
    uint8                     mtp_slot_count[BCM_MTP_SLOT_TYPES];
                                              /* FP mtp slot array size.   */
    sal_mutex_t               mutex;          /* Protection mutex.         */
} _bcm_mirror_config_t, *_bcm_mirror_config_p;


#define MIRROR_INIT(_unit_) (NULL != _bcm_mirror_config[(_unit_)])
#define MIRROR_CONFIG(_unit_) (_bcm_mirror_config[(_unit_)])
#define MIRROR_CONFIG_MODE(_unit_)  ((MIRROR_CONFIG(_unit_))->mode)

#define MIRROR_CONFIG_ING_MTP(_unit_, _idx_) \
    ((MIRROR_CONFIG(_unit_))->ing_mtp[(_idx_)])

#define MIRROR_CONFIG_EGR_MTP(_unit_, _idx_) \
    ((MIRROR_CONFIG(_unit_))->egr_mtp[(_idx_)])

#define MIRROR_CONFIG_EGR_TRUE_MTP(_unit_, _idx_) \
    ((MIRROR_CONFIG(_unit_))->egr_true_mtp[(_idx_)])

#define MIRROR_CONFIG_TYPE_MTP(_unit_, _idx_, _type_) \
    (((MIRROR_CONFIG(_unit_))->mtp_slot[(_type_)])[(_idx_)])

#define MIRROR_CONFIG_MTP(_unit_, _idx_, _flags_) \
        ((_flags_ & BCM_MIRROR_PORT_INGRESS) ? \
          &MIRROR_CONFIG_ING_MTP(unit, index) : \
          ((_flags_ & BCM_MIRROR_PORT_EGRESS) ? \
          &MIRROR_CONFIG_EGR_MTP(unit, index) : \
          &MIRROR_CONFIG_EGR_TRUE_MTP(unit, index)))

#define MIRROR_CONFIG_MTP_DEV_MASK(_unit_) \
    ((MIRROR_CONFIG(_unit_))->mtp_dev_mask)

#define MIRROR_CONFIG_MTP_MODE_BMP(_unit_) \
    ((MIRROR_CONFIG(_unit_))->mtp_mode_bmp)

#define MIRROR_CONFIG_MTP_MODE_REF_COUNT(_unit_, _idx_) \
    ((MIRROR_CONFIG(_unit_))->mtp_mode_ref_count[(_idx_)])

#define MIRROR_CONFIG_ING_MTP_DEST(_unit_, _idx_) \
    MIRROR_CONFIG_ING_MTP((_unit_), (_idx_)).dest_id

#define MIRROR_CONFIG_EGR_MTP_DEST(_unit_, _idx_) \
    MIRROR_CONFIG_EGR_MTP((_unit_),(_idx_)).dest_id

#define MIRROR_CONFIG_EGR_TRUE_MTP_DEST(_unit_, _idx_) \
    MIRROR_CONFIG_EGR_TRUE_MTP((_unit_),(_idx_)).dest_id

#define MIRROR_CONFIG_ING_MTP_REF_COUNT(_unit_, _idx_) \
    MIRROR_CONFIG_ING_MTP((_unit_), (_idx_)).ref_count

#define MIRROR_CONFIG_EGR_MTP_REF_COUNT(_unit_, _idx_) \
    MIRROR_CONFIG_EGR_MTP((_unit_),(_idx_)).ref_count

#define MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(_unit_, _idx_) \
    MIRROR_CONFIG_EGR_TRUE_MTP((_unit_),(_idx_)).ref_count

#define MIRROR_CONFIG_TYPE_MTP_SLOT(_unit_, _idx_, _type_) \
    MIRROR_CONFIG_TYPE_MTP((_unit_),(_idx_),(_type_)).dest_id

#define MIRROR_CONFIG_TYPE_MTP_REF_COUNT(_unit_, _idx_, _type_) \
    MIRROR_CONFIG_TYPE_MTP((_unit_),(_idx_),(_type_)).ref_count

#define MIRROR_DEST_CONFIG(_unit_, _idx_) \
    ((MIRROR_CONFIG(_unit_))->dest_arr[BCM_GPORT_MIRROR_GET(_idx_)])

#define MIRROR_DEST(_unit_, _idx_) \
    (&MIRROR_DEST_CONFIG((_unit_),(_idx_)).mirror_dest)

#define MIRROR_DEST_GPORT(_unit_, _idx_) \
    (MIRROR_DEST((_unit_), (_idx_))->gport)

#define MIRROR_DEST_REF_COUNT(_unit_, _idx_) \
    (MIRROR_DEST_CONFIG((_unit_), (_idx_)).ref_count)

#define MIRROR_LOCK(_unit_)  \
            sal_mutex_take(MIRROR_CONFIG(_unit_)->mutex, sal_mutex_FOREVER)
#define MIRROR_UNLOCK(_unit_)   sal_mutex_give(MIRROR_CONFIG(_unit_)->mutex)
extern int _bcm_esw_mirror_fp_dest_add(int unit, int modid, int port, 
                                       uint32 flags, int *mtp_index);

extern int _bcm_esw_mirror_fp_dest_delete(int unit, int mtp_index, uint32 flags);

extern int _bcm_esw_mirror_stk_update(int unit, bcm_module_t modid, bcm_port_t port,
                                      bcm_pbmp_t pbmp);
extern int bcm_esw_mirror_deinit(int unit);

#if defined(BCM_TRX_SUPPORT)
/* 18 byte L2 header + 20 byte IPv4 header + 4 byte GRE header = 11 words */
#define _BCM_TRX_MIRROR_TUNNEL_BUFFER_SZ  (0x11) 
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
extern int _bcm_xgs3_mirror_trunk_update(int unit, bcm_trunk_t tid);
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_mirror_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif	/* !_BCM_INT_MIRROR_H */
