/*
 * $Id: loopback.h,v 1.0 Broadcom SDK $
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
 * File:        loopback.h
 */

#ifndef CMICM_EMBEDDED
#include <soc/defs.h>
#endif

#ifndef _SOC_LOOPBACK_H
#define _SOC_LOOPBACK_H

#ifdef BCM_XGS3_SWITCH_SUPPORT
#define START_LBMH_HDR  0xFB
#define COMMON_LBMH_HDR   3
#define VIS_LBMH_HDR      1
#define SRC_TYPE_GPP_LBMH_HDR 1
#define SRC_TYPE_VP_LBMH_HDR  0

typedef struct soc_loopback_hdr_s {
#if defined(LE_HOST)
                                  /* byte # */
    uint32 start:8;             /* 0      */

    uint32 common_hdr0:4;       /* 1      */
    uint32 input_pri:4;         /* 1      */

    uint32 source0:6;           /* 2      */
    uint32 source_type:1;       /* 2      */
    uint32 common_hdr1:1;       /* 2      */

    uint32 source1:8;           /* 3      */

    uint32 reserved0_0:2;       /* 4      */
    uint32 pkt_profile:3;       /* 4      */
    uint32 visibility:1;        /* 4      */
    uint32 source2:2;           /* 4      */

    uint32 reserved0_1:8;       /* 5      */       
    uint32 reserved1_0:8;       /* 6      */
    uint32 reserved_1:5;        /* 7      */
    uint32 zero:1;              /* 7      */
    uint32 reserved1_1:2;       /* 7      */

    uint32 reserved_2:8;        /* 8      */
    uint32 reserved_3:8;        /* 9      */
    uint32 reserved_4:8;        /* 10     */
    uint32 reserved_5:8;        /* 11     */
    uint32 reserved_6:8;        /* 12     */
    uint32 reserved_7:8;        /* 13     */
    uint32 reserved_8:8;        /* 14     */
    uint32 pp_port:8;           /* 15      */
   
#else /* !LE_HOST*/
                                 /* byte # */
    uint32 start:8;             /* 0      */

    uint32 input_pri:4;         /* 1      */
    uint32 common_hdr0:4;       /* 1      */

    uint32 common_hdr1:1;       /* 2      */
    uint32 source_type:1;       /* 2      */
    uint32 source0:6;           /* 2      */

    uint32 source1:8;           /* 3      */

    uint32 source2:2;           /* 4      */
    uint32 visibility:1;        /* 4      */
    uint32 pkt_profile:3;       /* 4      */
    uint32 reserved0_0:2;       /* 4      */

    uint32 reserved0_1:8;       /* 5      */       
    uint32 reserved1_0:8;       /* 6      */
    uint32 reserved1_1:2;       /* 7      */
    uint32 zero:1;              /* 7      */
    uint32 reserved_1:5;        /* 7      */

    uint32 reserved_2:8;        /* 8      */
    uint32 reserved_3:8;        /* 9      */
    uint32 reserved_4:8;        /* 10     */
    uint32 reserved_5:8;        /* 11     */
    uint32 reserved_6:8;        /* 12     */
    uint32 reserved_7:8;        /* 13     */
    uint32 reserved_8:8;        /* 14     */
    uint32 pp_port:8;           /* 15      */
 #endif   
} soc_loopback_hdr_t;


typedef enum {
        LBMH_invalid = -1,
        LBMH_start   =  0,
        LBMH_input_pri,
        LBMH_visibility,
        LBMH_pkt_profile,
        LBMH_src,
        LBMH_src_type,
        LBMH_common_hdr,
        LBMH_pp_port,
        LBMH_cng,
        LBMH_tc,
        LBMH_zero,
        LBMH_COUNT
} soc_loopback_hdr_field_t;


#ifndef CMICM_EMBEDDED

extern soc_loopback_hdr_field_t soc_loopback_hdr_name_to_field(int unit, char *name);
extern char  *soc_loopback_hdr_field_to_name(int unit, soc_loopback_hdr_field_t f);
extern uint32 soc_loopback_hdr_field_get(int unit, soc_loopback_hdr_t *lh,
                                         soc_loopback_hdr_field_t field);
extern void soc_loopback_hdr_field_set(int unit, soc_loopback_hdr_t *lh,
                                       soc_loopback_hdr_field_t field, uint32 val);
extern void soc_loopback_hdr_dump(int unit, char *pfx, soc_loopback_hdr_t *lh);
extern int  soc_loopback_lbport_num_get(int unit);
#define soc_looopback_hdr_field_init(unit, mh) \
        sal_memset((mh), 0, sizeof (soc_loopback_hdr_t))

#define LOOPBACK_HDR_SIZE sizeof(soc_loopback_hdr_t)

/* PACKET TRACE SUPPORT */
/* used until SDK-59835 is approved */
#define VISIBILITY_PKT_PROFILE_NUM 0

#endif


#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif /* _SOC_LOOPBACK_H */

