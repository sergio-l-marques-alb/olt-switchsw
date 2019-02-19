/*
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
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: ped.h,v 1.7 Broadcom SDK $
 *
 * ped.h : PED defines
 *
 *-----------------------------------------------------------------------------*/
#ifndef _SBX_CALADN3_PED_H_
#define _SBX_CALADN3_PED_H_

#define SOC_SBX_CALADAN3_PD_NUM_HEADER_CONFIGS    15
#define SOC_SBX_CALADAN3_PD_MAX_CAPTURE_HEADERS   16

#define SOC_SBX_CALADAN3_PD_MAX_HEADER_TYPE   64
#define SOC_SBX_CALADAN3_PD_LAST_HEADER_TYPE  63

/* PD control flags */
#define SOC_SBX_CALADAN3_PD_DQ_DROP (1)
#define SOC_SBX_CALADAN3_PD_SQ_DROP (2)
#define SOC_SBX_CALADAN3_PD_ZERO_DROP (4)
#define SOC_SBX_CALADAN3_PD_OVERLAP_DROP (8)
#define SOC_SBX_CALADAN3_PD_LEN_DROP (0x10)
#define SOC_SBX_CALADAN3_PD_OVERRUN_DROP (0x20)
#define SOC_SBX_CALADAN3_PD_DQ_COPY (0x40)
#define SOC_SBX_CALADAN3_PD_SQ_COPY (0x80)
#define SOC_SBX_CALADAN3_PD_ZERO_COPY (0x100)
#define SOC_SBX_CALADAN3_PD_OVERLAP_COPY (0x200)
#define SOC_SBX_CALADAN3_PD_LEN_COPY (0x200)
#define SOC_SBX_CALADAN3_PD_OVERRUN_COPY (0x200)

typedef struct soc_sbx_caladan3_pd_hdr_info_s {
    int length;
    int type;
    int valid;
    uint8 elen_type;
    uint8 ipv4_type;
    uint8 xferlen_type;
    uint8 xferlen_offset;
    uint8 xferlen_size;
    uint8 varlen_mod;
    uint8 varlen_units;
    uint8 varlen_size;
    uint16 varlen_posn;
} soc_sbx_caladan3_pd_hdr_info_t;

typedef struct soc_sbx_caladan3_pd_config_s {
    uint8  debug;
    uint8  route_hdr_len_update;
    uint8  ipv4_chksum_update;
    uint8  ipv4_chksum_conditional_update;
    uint32 truncation_remove;
    uint32 truncation_value;
    uint8  route_header_len_update;
    uint32 flags;
    uint8 xferlen_enable;
    uint8 xferlen_offset;
    uint8 xferlen_size;
    soc_sbx_caladan3_pd_hdr_info_t hdr_attr[SOC_SBX_CALADAN3_PD_MAX_HEADER_TYPE];

} soc_sbx_caladan3_pd_config_t;

extern int soc_sbx_caladan3_ped_driver_init(int unit);
int soc_sbx_caladan3_ped_hd(int unit, int pedin, int pedout, int mode, uint32 *headerin, uint32 *headerout);
extern int soc_sbx_caladan3_ped_hdr_init(int unit,
                                         soc_sbx_caladan3_pd_hdr_info_t *info,
                                         uint8 clear);
int soc_sbx_caladan3_ped_hdr_config(int unit, int index, int clear);


#endif /* _SBX_CALADN3_PED_H_ */
