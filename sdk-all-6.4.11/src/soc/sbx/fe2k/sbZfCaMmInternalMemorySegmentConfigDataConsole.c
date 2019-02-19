/*
 * $Id: $
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
 */
#include <shared/bsl.h>

#include "sbTypes.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfCaMmInternalMemorySegmentConfigDataConsole.hx"



/* Print members in struct */
void
sbZfCaMmInternalMemorySegmentConfigData_Print(sbZfCaMmInternalMemorySegmentConfigData_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaMmInternalMemorySegmentConfigData:: resv0=0x%07x"), (unsigned int)  pFromStruct->m_uResv0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" wacc=0x%01x"), (unsigned int)  pFromStruct->m_uWriteAccess));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" racc=0x%01x"), (unsigned int)  pFromStruct->m_uReadAccess));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaMmInternalMemorySegmentConfigData:: ding=0x%01x"), (unsigned int)  pFromStruct->m_uDataIntegrity));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" trap=0x%01x"), (unsigned int)  pFromStruct->m_uTrapNullPointer));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" wrap=0x%01x"), (unsigned int)  pFromStruct->m_uWrap));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaMmInternalMemorySegmentConfigData:: lmask=0x%04x"), (unsigned int)  pFromStruct->m_uLimitMask));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" baddr=0x%04x"), (unsigned int)  pFromStruct->m_uBaseAddress));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaMmInternalMemorySegmentConfigData_SPrint(sbZfCaMmInternalMemorySegmentConfigData_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaMmInternalMemorySegmentConfigData:: resv0=0x%07x", (unsigned int)  pFromStruct->m_uResv0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," wacc=0x%01x", (unsigned int)  pFromStruct->m_uWriteAccess);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," racc=0x%01x", (unsigned int)  pFromStruct->m_uReadAccess);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaMmInternalMemorySegmentConfigData:: ding=0x%01x", (unsigned int)  pFromStruct->m_uDataIntegrity);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," trap=0x%01x", (unsigned int)  pFromStruct->m_uTrapNullPointer);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," wrap=0x%01x", (unsigned int)  pFromStruct->m_uWrap);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaMmInternalMemorySegmentConfigData:: lmask=0x%04x", (unsigned int)  pFromStruct->m_uLimitMask);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," baddr=0x%04x", (unsigned int)  pFromStruct->m_uBaseAddress);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaMmInternalMemorySegmentConfigData_Validate(sbZfCaMmInternalMemorySegmentConfigData_t *pZf) {

  if (pZf->m_uResv0 > 0x1ffffff) return 0;
  if (pZf->m_uWriteAccess > 0x1) return 0;
  if (pZf->m_uReadAccess > 0x1) return 0;
  if (pZf->m_uDataIntegrity > 0x7) return 0;
  if (pZf->m_uTrapNullPointer > 0x1) return 0;
  if (pZf->m_uWrap > 0x1) return 0;
  if (pZf->m_uLimitMask > 0xffff) return 0;
  if (pZf->m_uBaseAddress > 0xffff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaMmInternalMemorySegmentConfigData_SetField(sbZfCaMmInternalMemorySegmentConfigData_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_uresv0") == 0) {
    s->m_uResv0 = value;
  } else if (SB_STRCMP(name, "m_uwriteaccess") == 0) {
    s->m_uWriteAccess = value;
  } else if (SB_STRCMP(name, "m_ureadaccess") == 0) {
    s->m_uReadAccess = value;
  } else if (SB_STRCMP(name, "m_udataintegrity") == 0) {
    s->m_uDataIntegrity = value;
  } else if (SB_STRCMP(name, "m_utrapnullpointer") == 0) {
    s->m_uTrapNullPointer = value;
  } else if (SB_STRCMP(name, "m_uwrap") == 0) {
    s->m_uWrap = value;
  } else if (SB_STRCMP(name, "m_ulimitmask") == 0) {
    s->m_uLimitMask = value;
  } else if (SB_STRCMP(name, "m_ubaseaddress") == 0) {
    s->m_uBaseAddress = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
