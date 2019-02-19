/*
 * $Id: sbZfCaPrTagInfoConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaPrTagInfoConsole.hx"



/* Print members in struct */
void
sbZfCaPrTagInfo_Print(sbZfCaPrTagInfo_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPrTagInfo:: active=0x%01x"), (unsigned int)  pFromStruct->m_bActive));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" testpkt=0x%01x"), (unsigned int)  pFromStruct->m_bTestPkt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" port=0x%02x"), (unsigned int)  pFromStruct->m_nPort));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" eop=0x%01x"), (unsigned int)  pFromStruct->m_bEndOfPkt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" enqueues=0x%01x"), (unsigned int)  pFromStruct->m_nEnqueueCount));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPrTagInfo:: lines=0x%01x"), (unsigned int)  pFromStruct->m_nLineCount));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" npages=0x%02x"), (unsigned int)  pFromStruct->m_nNumPages));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" buffer=0x%03x"), (unsigned int)  pFromStruct->m_nBuffer));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" length=0x%04x"), (unsigned int)  pFromStruct->m_nLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" drop=0x%01x"), (unsigned int)  pFromStruct->m_bDropPkt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaPrTagInfo_SPrint(sbZfCaPrTagInfo_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPrTagInfo:: active=0x%01x", (unsigned int)  pFromStruct->m_bActive);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," testpkt=0x%01x", (unsigned int)  pFromStruct->m_bTestPkt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," port=0x%02x", (unsigned int)  pFromStruct->m_nPort);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," eop=0x%01x", (unsigned int)  pFromStruct->m_bEndOfPkt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," enqueues=0x%01x", (unsigned int)  pFromStruct->m_nEnqueueCount);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPrTagInfo:: lines=0x%01x", (unsigned int)  pFromStruct->m_nLineCount);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," npages=0x%02x", (unsigned int)  pFromStruct->m_nNumPages);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," buffer=0x%03x", (unsigned int)  pFromStruct->m_nBuffer);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," length=0x%04x", (unsigned int)  pFromStruct->m_nLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," drop=0x%01x", (unsigned int)  pFromStruct->m_bDropPkt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaPrTagInfo_Validate(sbZfCaPrTagInfo_t *pZf) {

  if (pZf->m_bActive > 0x1) return 0;
  if (pZf->m_bTestPkt > 0x1) return 0;
  if (pZf->m_nPort > 0x3f) return 0;
  if (pZf->m_bEndOfPkt > 0x1) return 0;
  if (pZf->m_nEnqueueCount > 0x3) return 0;
  if (pZf->m_nLineCount > 0x7) return 0;
  if (pZf->m_nNumPages > 0x7f) return 0;
  if (pZf->m_nBuffer > 0xfff) return 0;
  if (pZf->m_nLength > 0x3fff) return 0;
  if (pZf->m_bDropPkt > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaPrTagInfo_SetField(sbZfCaPrTagInfo_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "active") == 0) {
    s->m_bActive = value;
  } else if (SB_STRCMP(name, "testpkt") == 0) {
    s->m_bTestPkt = value;
  } else if (SB_STRCMP(name, "m_nport") == 0) {
    s->m_nPort = value;
  } else if (SB_STRCMP(name, "endofpkt") == 0) {
    s->m_bEndOfPkt = value;
  } else if (SB_STRCMP(name, "m_nenqueuecount") == 0) {
    s->m_nEnqueueCount = value;
  } else if (SB_STRCMP(name, "m_nlinecount") == 0) {
    s->m_nLineCount = value;
  } else if (SB_STRCMP(name, "m_nnumpages") == 0) {
    s->m_nNumPages = value;
  } else if (SB_STRCMP(name, "m_nbuffer") == 0) {
    s->m_nBuffer = value;
  } else if (SB_STRCMP(name, "m_nlength") == 0) {
    s->m_nLength = value;
  } else if (SB_STRCMP(name, "droppkt") == 0) {
    s->m_bDropPkt = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
