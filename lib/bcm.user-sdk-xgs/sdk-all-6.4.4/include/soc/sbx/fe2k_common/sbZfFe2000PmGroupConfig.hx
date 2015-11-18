/*  -*-  Mode:C; c-basic-offset:4 -*- */
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
 */


#ifndef SB_ZF_FE2000PMGROUPCONFIG_H
#define SB_ZF_FE2000PMGROUPCONFIG_H

#define SB_ZF_FE2000PMGROUPCONFIG_SIZE_IN_BYTES 16
#define SB_ZF_FE2000PMGROUPCONFIG_SIZE 16
#define SB_ZF_FE2000PMGROUPCONFIG_UENABLE_BITS "127:127"
#define SB_ZF_FE2000PMGROUPCONFIG_USPARE2_BITS "126:119"
#define SB_ZF_FE2000PMGROUPCONFIG_UMINPOLICERID_BITS "118:96"
#define SB_ZF_FE2000PMGROUPCONFIG_USPARE1_BITS "95:87"
#define SB_ZF_FE2000PMGROUPCONFIG_UMAXPOLICERID_BITS "86:64"
#define SB_ZF_FE2000PMGROUPCONFIG_UREFRESHCOUNT_BITS "63:60"
#define SB_ZF_FE2000PMGROUPCONFIG_UREFRESHTHRESH_BITS "59:48"
#define SB_ZF_FE2000PMGROUPCONFIG_UREFRESHPERIOD_BITS "47:32"
#define SB_ZF_FE2000PMGROUPCONFIG_USPARE0_BITS "31:31"
#define SB_ZF_FE2000PMGROUPCONFIG_UTIMERENABLE_BITS "30:30"
#define SB_ZF_FE2000PMGROUPCONFIG_UTIMESTAMPOFFSET_BITS "29:24"
#define SB_ZF_FE2000PMGROUPCONFIG_UTIMERTICKPERIOD_BITS "23:0"


typedef struct _sbZfFe2000PmGroupConfig {
  uint32 uEnable;
  uint32 uSpare2;
  uint32 uMinPolicerId;
  uint32 uSpare1;
  uint32 uMaxPolicerId;
  uint32 uRefreshCount;
  uint32 uRefreshThresh;
  uint32 uRefreshPeriod;
  uint32 uSpare0;
  uint32 uTimerEnable;
  uint32 uTimestampOffset;
  uint32 uTimerTickPeriod;
} sbZfFe2000PmGroupConfig_t;

uint32
sbZfFe2000PmGroupConfig_Pack(sbZfFe2000PmGroupConfig_t *pFrom,
                             uint8 *pToData,
                             uint32 nMaxToDataIndex);
void
sbZfFe2000PmGroupConfig_Unpack(sbZfFe2000PmGroupConfig_t *pToStruct,
                               uint8 *pFromData,
                               uint32 nMaxToDataIndex);
void
sbZfFe2000PmGroupConfig_InitInstance(sbZfFe2000PmGroupConfig_t *pFrame);

#define SB_ZF_FE2000PMGROUPCONFIG_SET_ENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[15] = ((pToData)[15] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_SPARE2(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[14] = ((pToData)[14] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[15] = ((pToData)[15] & ~ 0x7f) | (((nFromData) >> 1) & 0x7f); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_PMIN(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[12] = ((nFromData)) & 0xFF; \
           (pToData)[13] = ((pToData)[13] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[14] = ((pToData)[14] & ~ 0x7f) | (((nFromData) >> 16) & 0x7f); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_SPARE1(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[10] = ((pToData)[10] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
           (pToData)[11] = ((pToData)[11] & ~0xFF) | (((nFromData) >> 1) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_PMAX(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[8] = ((nFromData)) & 0xFF; \
           (pToData)[9] = ((pToData)[9] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[10] = ((pToData)[10] & ~ 0x7f) | (((nFromData) >> 16) & 0x7f); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_RCOUNT(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[7] = ((pToData)[7] & ~(0x0f << 4)) | (((nFromData) & 0x0f) << 4); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_RTHRSH(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[6] = ((nFromData)) & 0xFF; \
           (pToData)[7] = ((pToData)[7] & ~ 0x0f) | (((nFromData) >> 8) & 0x0f); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_RPRD(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[4] = ((nFromData)) & 0xFF; \
           (pToData)[5] = ((pToData)[5] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_SPARE0(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_TENA(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 6)) | (((nFromData) & 0x01) << 6); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_TSOFF(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~0x3f) | ((nFromData) & 0x3f); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_SET_TPRD(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((nFromData)) & 0xFF; \
           (pToData)[1] = ((pToData)[1] & ~0xFF) | (((nFromData) >> 8) & 0xFF); \
           (pToData)[2] = ((pToData)[2] & ~0xFF) | (((nFromData) >> 16) & 0xFF); \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_ENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[15] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_SPARE2(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[14] >> 7) & 0x01; \
           (nToData) |= (uint32) ((pFromData)[15] & 0x7f) << 1; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_PMIN(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[12] ; \
           (nToData) |= (uint32) (pFromData)[13] << 8; \
           (nToData) |= (uint32) ((pFromData)[14] & 0x7f) << 16; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_SPARE1(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[10] >> 7) & 0x01; \
           (nToData) |= (uint32) (pFromData)[11] << 1; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_PMAX(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[8] ; \
           (nToData) |= (uint32) (pFromData)[9] << 8; \
           (nToData) |= (uint32) ((pFromData)[10] & 0x7f) << 16; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_RCOUNT(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[7] >> 4) & 0x0f; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_RTHRSH(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[6] ; \
           (nToData) |= (uint32) ((pFromData)[7] & 0x0f) << 8; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_RPRD(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[4] ; \
           (nToData) |= (uint32) (pFromData)[5] << 8; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_SPARE0(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_TENA(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3] >> 6) & 0x01; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_TSOFF(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3]) & 0x3f; \
          } while(0)

#define SB_ZF_FE2000PMGROUPCONFIG_GET_TPRD(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) (pFromData)[0] ; \
           (nToData) |= (uint32) (pFromData)[1] << 8; \
           (nToData) |= (uint32) (pFromData)[2] << 16; \
          } while(0)

#endif
