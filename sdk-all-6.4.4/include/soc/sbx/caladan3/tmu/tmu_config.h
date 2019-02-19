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
 * $Id: tmu_config.h,v $
 *
 * TMU configuration defines
 *
 *-----------------------------------------------------------------------------*/
#ifndef _SBX_CALADN3_TMU_CONFIG_H_
#define _SBX_CALADN3_TMU_CONFIG_H_

#include <sal/appl/sal.h>

#define SOC_SBX_CALADAN3_TMU_TABLE_1X_REPLICATION (1)
#define SOC_SBX_CALADAN3_TMU_TABLE_2X_REPLICATION (2)
#define SOC_SBX_CALADAN3_TMU_TABLE_4X_REPLICATION (4)
#define SOC_SBX_CALADAN3_TMU_TABLE_DEF_REPLICATION (SOC_SBX_CALADAN3_TMU_TABLE_4X_REPLICATION)

#define SOC_SBX_CALADAN3_TMU_MAX_TABLE (64)
#define SOC_SBX_CALADAN3_TMU_MAX_CHAIN_TABLE (32) /* 0 - 31 */
#define SOC_SBX_CALADAN3_TMU_MIN_TABLE_NUM_ENTRIES (4096) /* 4K entries */
#define SOC_SBX_CALADAN3_TMU_TABLE_ENTRY_SIZE_UNITS (64) /* entires has to be multiple of 64bits */
#define SOC_SBX_CALADAN3_TMU_TABLE_ENTRY_SIZE_MIN (64) /* min entry size 64 bits */
#define SOC_SBX_CALADAN3_TMU_TABLE_ENTRY_SIZE_MAX (8*1024) /* max entry size 8K bits */

#define SOC_SBX_TMU_TABLE_FLAG_NONE         (0)
#define SOC_SBX_TMU_TABLE_FLAG_WITH_ID      (1)
#define SOC_SBX_TMU_TABLE_FLAG_CHAIN        (2)

#define SOC_SBX_CALADAN3_TMU_MAX_BMP        (512)
#define SOC_SBX_CALADAN3_TMU_MAX_BMP_WORDS  (SOC_SBX_CALADAN3_TMU_MAX_BMP/32)


typedef enum _soc_sbx_caladan3_tmu_lookup_e {
    SOC_SBX_TMU_LKUP_EML_64 = 0,
    SOC_SBX_TMU_LKUP_EML_144 = 1, /* EML144 share same lookup value as EML176 */
    SOC_SBX_TMU_LKUP_EML_176 = 1,
    SOC_SBX_TMU_LKUP_EML_304 = 2,
    SOC_SBX_TMU_LKUP_EML_424 = 3,
    SOC_SBX_TMU_LKUP_EML2ND_64 = 4,
    SOC_SBX_TMU_LKUP_EML2ND_144 = 5, /* EML144 share same lookup value as EML176 */
    SOC_SBX_TMU_LKUP_EML2ND_176 = 5,
    SOC_SBX_TMU_LKUP_EML2ND_304 = 6,
    SOC_SBX_TMU_LKUP_EML2ND_424 = 7,
    SOC_SBX_TMU_LKUP_EMC_64 = 8,
    SOC_SBX_TMU_LKUP_EMC2ND_64 = 9,
    SOC_SBX_TMU_LKUP_TAPS_IPV4_SUB_KEY = 12,
    SOC_SBX_TMU_LKUP_TAPS_IPV6_SUB_KEY = 13,
    SOC_SBX_TMU_LKUP_TAPS_IPV4_UNIFIED_KEY = 14,
    SOC_SBX_TMU_LKUP_TAPS_IPV6_UNIFIED_KEY = 15,
    SOC_SBX_TMU_LKUP_TAPS_IPV4_BUCKET = 16,
    SOC_SBX_TMU_LKUP_TAPS_IPV6_BUCKET = 17,
    SOC_SBX_TMU_LKUP_TAPS_IPV4_DATA = 18,
    SOC_SBX_TMU_LKUP_TAPS_IPV6_DATA = 19,
    SOC_SBX_TMU_LKUP_EML_INSERT_DELETE = 28,
    SOC_SBX_TMU_LKUP_DM_119 = 32,
    SOC_SBX_TMU_LKUP_DM_247 = 33,
    SOC_SBX_TMU_LKUP_DM_366 = 34,
    SOC_SBX_TMU_LKUP_DM_494 = 35,
    SOC_SBX_TMU_LKUP_NONE = 63 /* nothing */
} soc_sbx_caladan3_tmu_lookup_t; 


#endif /* _SBX_CALADN3_TMU_CONFIG_H_ */
