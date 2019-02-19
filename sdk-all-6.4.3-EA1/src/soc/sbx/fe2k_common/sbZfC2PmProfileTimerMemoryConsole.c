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
#include <shared/bsl.h>

#include "sbTypesGlue.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfC2PmProfileTimerMemoryConsole.hx"



/* Print members in struct */
void
sbZfC2PmProfileTimerMemory_Print(sbZfC2PmProfileTimerMemory_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PmProfileTimerMemory:: resv0=0x%06x"), (unsigned int)  pFromStruct->uResv0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" type=0x%01x"), (unsigned int)  pFromStruct->uType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" profmode=0x%01x"), (unsigned int)  pFromStruct->uProfMode));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" resv1=0x%02x"), (unsigned int)  pFromStruct->uResv1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PmProfileTimerMemory:: resv2=0x%08x"), (unsigned int)  pFromStruct->uResv2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" resv3=0x%02x"), (unsigned int)  pFromStruct->uResv3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" binterrupt=0x%01x"), (unsigned int)  pFromStruct->bInterrupt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PmProfileTimerMemory:: breset=0x%01x"), (unsigned int)  pFromStruct->bReset));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" bstrict=0x%01x"), (unsigned int)  pFromStruct->bStrict));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" bstarted=0x%01x"), (unsigned int)  pFromStruct->bStarted));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PmProfileTimerMemory:: deadline=0x%05x"), (unsigned int)  pFromStruct->uDeadline));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2PmProfileTimerMemory_SPrint(sbZfC2PmProfileTimerMemory_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PmProfileTimerMemory:: resv0=0x%06x", (unsigned int)  pFromStruct->uResv0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," type=0x%01x", (unsigned int)  pFromStruct->uType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," profmode=0x%01x", (unsigned int)  pFromStruct->uProfMode);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," resv1=0x%02x", (unsigned int)  pFromStruct->uResv1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PmProfileTimerMemory:: resv2=0x%08x", (unsigned int)  pFromStruct->uResv2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," resv3=0x%02x", (unsigned int)  pFromStruct->uResv3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," binterrupt=0x%01x", (unsigned int)  pFromStruct->bInterrupt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PmProfileTimerMemory:: breset=0x%01x", (unsigned int)  pFromStruct->bReset);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," bstrict=0x%01x", (unsigned int)  pFromStruct->bStrict);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," bstarted=0x%01x", (unsigned int)  pFromStruct->bStarted);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PmProfileTimerMemory:: deadline=0x%05x", (unsigned int)  pFromStruct->uDeadline);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2PmProfileTimerMemory_Validate(sbZfC2PmProfileTimerMemory_t *pZf) {

  if (pZf->uResv0 > 0x1fffff) return 0;
  if (pZf->uType > 0x1) return 0;
  if (pZf->uProfMode > 0x3) return 0;
  if (pZf->uResv1 > 0xff) return 0;
  /* pZf->uResv2 implicitly masked by data type */
  if (pZf->uResv3 > 0xff) return 0;
  if (pZf->bInterrupt > 0x1) return 0;
  if (pZf->bReset > 0x1) return 0;
  if (pZf->bStrict > 0x1) return 0;
  if (pZf->bStarted > 0x1) return 0;
  if (pZf->uDeadline > 0xfffff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2PmProfileTimerMemory_SetField(sbZfC2PmProfileTimerMemory_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "uresv0") == 0) {
    s->uResv0 = value;
  } else if (SB_STRCMP(name, "utype") == 0) {
    s->uType = value;
  } else if (SB_STRCMP(name, "uprofmode") == 0) {
    s->uProfMode = value;
  } else if (SB_STRCMP(name, "uresv1") == 0) {
    s->uResv1 = value;
  } else if (SB_STRCMP(name, "uresv2") == 0) {
    s->uResv2 = value;
  } else if (SB_STRCMP(name, "uresv3") == 0) {
    s->uResv3 = value;
  } else if (SB_STRCMP(name, "interrupt") == 0) {
    s->bInterrupt = value;
  } else if (SB_STRCMP(name, "reset") == 0) {
    s->bReset = value;
  } else if (SB_STRCMP(name, "strict") == 0) {
    s->bStrict = value;
  } else if (SB_STRCMP(name, "started") == 0) {
    s->bStarted = value;
  } else if (SB_STRCMP(name, "udeadline") == 0) {
    s->uDeadline = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
