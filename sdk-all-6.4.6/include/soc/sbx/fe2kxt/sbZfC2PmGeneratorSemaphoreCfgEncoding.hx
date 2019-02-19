/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


#ifndef SB_ZF_C2PMPGENERATORSEMAPHORECFGENCODING_H
#define SB_ZF_C2PMPGENERATORSEMAPHORECFGENCODING_H

#define SB_ZF_C2PMPGENERATORSEMAPHORECFGENCODING_SIZE_IN_BYTES 10
#define SB_ZF_C2PMPGENERATORSEMAPHORECFGENCODING_SIZE 10
#define SB_ZF_C2PMPGENERATORSEMAPHORECFGENCODING_M_UTYPE_BITS "74:72"


typedef struct _sbZfC2PmGeneratorSemaphoreCfgEncoding {
  uint32 m_uType;
} sbZfC2PmGeneratorSemaphoreCfgEncoding_t;

uint32
sbZfC2PmGeneratorSemaphoreCfgEncoding_Pack(sbZfC2PmGeneratorSemaphoreCfgEncoding_t *pFrom,
                                           uint8 *pToData,
                                           uint32 nMaxToDataIndex);
void
sbZfC2PmGeneratorSemaphoreCfgEncoding_Unpack(sbZfC2PmGeneratorSemaphoreCfgEncoding_t *pToStruct,
                                             uint8 *pFromData,
                                             uint32 nMaxToDataIndex);
void
sbZfC2PmGeneratorSemaphoreCfgEncoding_InitInstance(sbZfC2PmGeneratorSemaphoreCfgEncoding_t *pFrame);

#define SB_ZF_C2PMGENERATORSEMAPHORECFGENCODING_SET_TYPE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[9] = ((pToData)[9] & ~0x07) | ((nFromData) & 0x07); \
          } while(0)

#define SB_ZF_C2PMGENERATORSEMAPHORECFGENCODING_GET_TYPE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[9]) & 0x07; \
          } while(0)

#endif
