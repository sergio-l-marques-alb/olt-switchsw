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

#include "sbTypes.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfC2QmQueueConfigEntryConsole.hx"



/* Print members in struct */
void
sbZfC2QmQueueConfigEntry_Print(sbZfC2QmQueueConfigEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2QmQueueConfigEntry:: enforcepolicemarkings=0x%01x"), (unsigned int)  pFromStruct->m_uEnforcePoliceMarkings));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" dropthresh2=0x%03x"), (unsigned int)  pFromStruct->m_uDropThresh2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2QmQueueConfigEntry:: dropthresh1=0x%03x"), (unsigned int)  pFromStruct->m_uDropThresh1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" flowcontrolthresh2=0x%03x"), (unsigned int)  pFromStruct->m_uFlowControlThresh2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2QmQueueConfigEntry:: flowcontrolthresh1=0x%03x"), (unsigned int)  pFromStruct->m_uFlowControlThresh1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" flowcontrolenable=0x%01x"), (unsigned int)  pFromStruct->m_uFlowControlEnable));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2QmQueueConfigEntry:: flowcontroltarget=0x%01x"), (unsigned int)  pFromStruct->m_uFlowControlTarget));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" flowcontrolport=0x%03x"), (unsigned int)  pFromStruct->m_uFlowControlPort));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2QmQueueConfigEntry:: maxpages=0x%03x"), (unsigned int)  pFromStruct->m_uMaxPages));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" minpages=0x%03x"), (unsigned int)  pFromStruct->m_uMinPages));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" enable=0x%01x"), (unsigned int)  pFromStruct->m_uEnable));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2QmQueueConfigEntry_SPrint(sbZfC2QmQueueConfigEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2QmQueueConfigEntry:: enforcepolicemarkings=0x%01x", (unsigned int)  pFromStruct->m_uEnforcePoliceMarkings);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," dropthresh2=0x%03x", (unsigned int)  pFromStruct->m_uDropThresh2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2QmQueueConfigEntry:: dropthresh1=0x%03x", (unsigned int)  pFromStruct->m_uDropThresh1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," flowcontrolthresh2=0x%03x", (unsigned int)  pFromStruct->m_uFlowControlThresh2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2QmQueueConfigEntry:: flowcontrolthresh1=0x%03x", (unsigned int)  pFromStruct->m_uFlowControlThresh1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," flowcontrolenable=0x%01x", (unsigned int)  pFromStruct->m_uFlowControlEnable);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2QmQueueConfigEntry:: flowcontroltarget=0x%01x", (unsigned int)  pFromStruct->m_uFlowControlTarget);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," flowcontrolport=0x%03x", (unsigned int)  pFromStruct->m_uFlowControlPort);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2QmQueueConfigEntry:: maxpages=0x%03x", (unsigned int)  pFromStruct->m_uMaxPages);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," minpages=0x%03x", (unsigned int)  pFromStruct->m_uMinPages);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," enable=0x%01x", (unsigned int)  pFromStruct->m_uEnable);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2QmQueueConfigEntry_Validate(sbZfC2QmQueueConfigEntry_t *pZf) {

  if (pZf->m_uEnforcePoliceMarkings > 0x1) return 0;
  if (pZf->m_uDropThresh2 > 0xfff) return 0;
  if (pZf->m_uDropThresh1 > 0xfff) return 0;
  if (pZf->m_uFlowControlThresh2 > 0xfff) return 0;
  if (pZf->m_uFlowControlThresh1 > 0xfff) return 0;
  if (pZf->m_uFlowControlEnable > 0x1) return 0;
  if (pZf->m_uFlowControlTarget > 0x1) return 0;
  if (pZf->m_uFlowControlPort > 0x1ff) return 0;
  if (pZf->m_uMaxPages > 0xfff) return 0;
  if (pZf->m_uMinPages > 0xfff) return 0;
  if (pZf->m_uEnable > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2QmQueueConfigEntry_SetField(sbZfC2QmQueueConfigEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_uenforcepolicemarkings") == 0) {
    s->m_uEnforcePoliceMarkings = value;
  } else if (SB_STRCMP(name, "m_udropthresh2") == 0) {
    s->m_uDropThresh2 = value;
  } else if (SB_STRCMP(name, "m_udropthresh1") == 0) {
    s->m_uDropThresh1 = value;
  } else if (SB_STRCMP(name, "m_uflowcontrolthresh2") == 0) {
    s->m_uFlowControlThresh2 = value;
  } else if (SB_STRCMP(name, "m_uflowcontrolthresh1") == 0) {
    s->m_uFlowControlThresh1 = value;
  } else if (SB_STRCMP(name, "m_uflowcontrolenable") == 0) {
    s->m_uFlowControlEnable = value;
  } else if (SB_STRCMP(name, "m_uflowcontroltarget") == 0) {
    s->m_uFlowControlTarget = value;
  } else if (SB_STRCMP(name, "m_uflowcontrolport") == 0) {
    s->m_uFlowControlPort = value;
  } else if (SB_STRCMP(name, "m_umaxpages") == 0) {
    s->m_uMaxPages = value;
  } else if (SB_STRCMP(name, "m_uminpages") == 0) {
    s->m_uMinPages = value;
  } else if (SB_STRCMP(name, "m_uenable") == 0) {
    s->m_uEnable = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
