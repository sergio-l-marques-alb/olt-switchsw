/*
 * $Id$
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
#include "sbZfC2DiagUcodeCtlConsole.hx"



/* Print members in struct */
void
sbZfC2DiagUcodeCtl_Print(sbZfC2DiagUcodeCtl_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2DiagUcodeCtl:: status=0x%01x"), (unsigned int)  pFromStruct->m_ulStatus));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" errinject0=0x%01x"), (unsigned int)  pFromStruct->m_ulErrInject0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" errinject1=0x%01x"), (unsigned int)  pFromStruct->m_ulErrInject1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" errind=0x%01x"), (unsigned int)  pFromStruct->m_ulErrInd));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2DiagUcodeCtl:: ucloaded=0x%01x"), (unsigned int)  pFromStruct->m_ulUcLoaded));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" lrpstate=0x%01x"), (unsigned int)  pFromStruct->m_ulLrpState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" memexst0=0x%01x"), (unsigned int)  pFromStruct->m_ulMemExstMm0N0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" memexst1=0x%01x"), (unsigned int)  pFromStruct->m_ulMemExstMm0N1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2DiagUcodeCtl:: memexst2=0x%01x"), (unsigned int)  pFromStruct->m_ulMemExstMm0W));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" memexst3=0x%01x"), (unsigned int)  pFromStruct->m_ulMemExstMm1N0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" memexst4=0x%01x"), (unsigned int)  pFromStruct->m_ulMemExstMm1N1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" memexst5=0x%01x"), (unsigned int)  pFromStruct->m_ulMemExstMm1W));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2DiagUcodeCtl:: testoffset=0x%05x"), (unsigned int)  pFromStruct->m_ulTestOffset));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2DiagUcodeCtl_SPrint(sbZfC2DiagUcodeCtl_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2DiagUcodeCtl:: status=0x%01x", (unsigned int)  pFromStruct->m_ulStatus);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," errinject0=0x%01x", (unsigned int)  pFromStruct->m_ulErrInject0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," errinject1=0x%01x", (unsigned int)  pFromStruct->m_ulErrInject1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," errind=0x%01x", (unsigned int)  pFromStruct->m_ulErrInd);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2DiagUcodeCtl:: ucloaded=0x%01x", (unsigned int)  pFromStruct->m_ulUcLoaded);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," lrpstate=0x%01x", (unsigned int)  pFromStruct->m_ulLrpState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," memexst0=0x%01x", (unsigned int)  pFromStruct->m_ulMemExstMm0N0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," memexst1=0x%01x", (unsigned int)  pFromStruct->m_ulMemExstMm0N1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2DiagUcodeCtl:: memexst2=0x%01x", (unsigned int)  pFromStruct->m_ulMemExstMm0W);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," memexst3=0x%01x", (unsigned int)  pFromStruct->m_ulMemExstMm1N0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," memexst4=0x%01x", (unsigned int)  pFromStruct->m_ulMemExstMm1N1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," memexst5=0x%01x", (unsigned int)  pFromStruct->m_ulMemExstMm1W);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2DiagUcodeCtl:: testoffset=0x%05x", (unsigned int)  pFromStruct->m_ulTestOffset);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2DiagUcodeCtl_Validate(sbZfC2DiagUcodeCtl_t *pZf) {

  if (pZf->m_ulStatus > 0x1) return 0;
  if (pZf->m_ulErrInject0 > 0x1) return 0;
  if (pZf->m_ulErrInject1 > 0x1) return 0;
  if (pZf->m_ulErrInd > 0x1) return 0;
  if (pZf->m_ulUcLoaded > 0x1) return 0;
  if (pZf->m_ulLrpState > 0x3) return 0;
  if (pZf->m_ulMemExstMm0N0 > 0x1) return 0;
  if (pZf->m_ulMemExstMm0N1 > 0x1) return 0;
  if (pZf->m_ulMemExstMm0W > 0x1) return 0;
  if (pZf->m_ulMemExstMm1N0 > 0x1) return 0;
  if (pZf->m_ulMemExstMm1N1 > 0x1) return 0;
  if (pZf->m_ulMemExstMm1W > 0x1) return 0;
  if (pZf->m_ulTestOffset > 0x7ffff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2DiagUcodeCtl_SetField(sbZfC2DiagUcodeCtl_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "status") == 0) {
    s->m_ulStatus = value;
  } else if (SB_STRCMP(name, "errinject0") == 0) {
    s->m_ulErrInject0 = value;
  } else if (SB_STRCMP(name, "errinject1") == 0) {
    s->m_ulErrInject1 = value;
  } else if (SB_STRCMP(name, "errind") == 0) {
    s->m_ulErrInd = value;
  } else if (SB_STRCMP(name, "ucloaded") == 0) {
    s->m_ulUcLoaded = value;
  } else if (SB_STRCMP(name, "lrpstate") == 0) {
    s->m_ulLrpState = value;
  } else if (SB_STRCMP(name, "memexstmm0n0") == 0) {
    s->m_ulMemExstMm0N0 = value;
  } else if (SB_STRCMP(name, "memexstmm0n1") == 0) {
    s->m_ulMemExstMm0N1 = value;
  } else if (SB_STRCMP(name, "memexstmm0w") == 0) {
    s->m_ulMemExstMm0W = value;
  } else if (SB_STRCMP(name, "memexstmm1n0") == 0) {
    s->m_ulMemExstMm1N0 = value;
  } else if (SB_STRCMP(name, "memexstmm1n1") == 0) {
    s->m_ulMemExstMm1N1 = value;
  } else if (SB_STRCMP(name, "memexstmm1w") == 0) {
    s->m_ulMemExstMm1W = value;
  } else if (SB_STRCMP(name, "testoffset") == 0) {
    s->m_ulTestOffset = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
