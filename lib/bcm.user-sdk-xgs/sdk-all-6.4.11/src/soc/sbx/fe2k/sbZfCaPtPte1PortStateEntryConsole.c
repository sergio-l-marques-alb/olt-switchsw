/*
 * $Id: sbZfCaPtPte1PortStateEntryConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaPtPte1PortStateEntryConsole.hx"



/* Print members in struct */
void
sbZfCaPtPte1PortStateEntry_Print(sbZfCaPtPte1PortStateEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: pktinprogress=0x%01x"), (unsigned int)  pFromStruct->m_uPktInProgress));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mirror=0x%01x"), (unsigned int)  pFromStruct->m_uMirror));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mirrorsrcdrop=0x%01x"), (unsigned int)  pFromStruct->m_uMirrorSrcDrop));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: doubledequeued=0x%01x"), (unsigned int)  pFromStruct->m_uDoubleDequeued));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" eopextraburst=0x%01x"), (unsigned int)  pFromStruct->m_uEopExtraBurst));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: residuebyte=0x%01x"), (unsigned int)  pFromStruct->m_uResidueByte));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pktrequeststate=0x%01x"), (unsigned int)  pFromStruct->m_uPktRequestState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" line=0x%01x"), (unsigned int)  pFromStruct->m_uLine));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: lastpage=0x%01x"), (unsigned int)  pFromStruct->m_uLastPage));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" nextpagestate=0x%01x"), (unsigned int)  pFromStruct->m_uNextPageState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" nextpage=0x%03x"), (unsigned int)  pFromStruct->m_uNextPage));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: nextlength=0x%02x"), (unsigned int)  pFromStruct->m_uNextLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" page=0x%03x"), (unsigned int)  pFromStruct->m_uPage));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" savedbytes=0x%01x"), (unsigned int)  pFromStruct->m_uSavedBytes));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: remainingbytes=0x%02x"), (unsigned int)  pFromStruct->m_uRemainingBytes));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" eop=0x%01x"), (unsigned int)  pFromStruct->m_uEop));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" sop=0x%01x"), (unsigned int)  pFromStruct->m_uSop));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: pktlength=0x%04x"), (unsigned int)  pFromStruct->m_uPktLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" continuebyte=0x%02x"), (unsigned int)  pFromStruct->m_uContinueByte));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPtPte1PortStateEntry:: sourcequeue=0x%02x"), (unsigned int)  pFromStruct->m_uSourceQueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaPtPte1PortStateEntry_SPrint(sbZfCaPtPte1PortStateEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: pktinprogress=0x%01x", (unsigned int)  pFromStruct->m_uPktInProgress);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mirror=0x%01x", (unsigned int)  pFromStruct->m_uMirror);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mirrorsrcdrop=0x%01x", (unsigned int)  pFromStruct->m_uMirrorSrcDrop);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: doubledequeued=0x%01x", (unsigned int)  pFromStruct->m_uDoubleDequeued);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," eopextraburst=0x%01x", (unsigned int)  pFromStruct->m_uEopExtraBurst);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: residuebyte=0x%01x", (unsigned int)  pFromStruct->m_uResidueByte);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pktrequeststate=0x%01x", (unsigned int)  pFromStruct->m_uPktRequestState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," line=0x%01x", (unsigned int)  pFromStruct->m_uLine);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: lastpage=0x%01x", (unsigned int)  pFromStruct->m_uLastPage);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," nextpagestate=0x%01x", (unsigned int)  pFromStruct->m_uNextPageState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," nextpage=0x%03x", (unsigned int)  pFromStruct->m_uNextPage);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: nextlength=0x%02x", (unsigned int)  pFromStruct->m_uNextLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," page=0x%03x", (unsigned int)  pFromStruct->m_uPage);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," savedbytes=0x%01x", (unsigned int)  pFromStruct->m_uSavedBytes);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: remainingbytes=0x%02x", (unsigned int)  pFromStruct->m_uRemainingBytes);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," eop=0x%01x", (unsigned int)  pFromStruct->m_uEop);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," sop=0x%01x", (unsigned int)  pFromStruct->m_uSop);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: pktlength=0x%04x", (unsigned int)  pFromStruct->m_uPktLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," continuebyte=0x%02x", (unsigned int)  pFromStruct->m_uContinueByte);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPtPte1PortStateEntry:: sourcequeue=0x%02x", (unsigned int)  pFromStruct->m_uSourceQueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaPtPte1PortStateEntry_Validate(sbZfCaPtPte1PortStateEntry_t *pZf) {

  if (pZf->m_uPktInProgress > 0x1) return 0;
  if (pZf->m_uMirror > 0x1) return 0;
  if (pZf->m_uMirrorSrcDrop > 0x1) return 0;
  if (pZf->m_uDoubleDequeued > 0x1) return 0;
  if (pZf->m_uEopExtraBurst > 0x1) return 0;
  if (pZf->m_uResidueByte > 0xf) return 0;
  if (pZf->m_uPktRequestState > 0xf) return 0;
  if (pZf->m_uLine > 0x1) return 0;
  if (pZf->m_uLastPage > 0x1) return 0;
  if (pZf->m_uNextPageState > 0x3) return 0;
  if (pZf->m_uNextPage > 0xfff) return 0;
  if (pZf->m_uNextLength > 0xff) return 0;
  if (pZf->m_uPage > 0xfff) return 0;
  if (pZf->m_uSavedBytes > 0xf) return 0;
  if (pZf->m_uRemainingBytes > 0xff) return 0;
  if (pZf->m_uEop > 0x1) return 0;
  if (pZf->m_uSop > 0x1) return 0;
  if (pZf->m_uPktLength > 0x3fff) return 0;
  if (pZf->m_uContinueByte > 0xff) return 0;
  if (pZf->m_uSourceQueue > 0xff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaPtPte1PortStateEntry_SetField(sbZfCaPtPte1PortStateEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_upktinprogress") == 0) {
    s->m_uPktInProgress = value;
  } else if (SB_STRCMP(name, "m_umirror") == 0) {
    s->m_uMirror = value;
  } else if (SB_STRCMP(name, "m_umirrorsrcdrop") == 0) {
    s->m_uMirrorSrcDrop = value;
  } else if (SB_STRCMP(name, "m_udoubledequeued") == 0) {
    s->m_uDoubleDequeued = value;
  } else if (SB_STRCMP(name, "m_ueopextraburst") == 0) {
    s->m_uEopExtraBurst = value;
  } else if (SB_STRCMP(name, "m_uresiduebyte") == 0) {
    s->m_uResidueByte = value;
  } else if (SB_STRCMP(name, "m_upktrequeststate") == 0) {
    s->m_uPktRequestState = value;
  } else if (SB_STRCMP(name, "m_uline") == 0) {
    s->m_uLine = value;
  } else if (SB_STRCMP(name, "m_ulastpage") == 0) {
    s->m_uLastPage = value;
  } else if (SB_STRCMP(name, "m_unextpagestate") == 0) {
    s->m_uNextPageState = value;
  } else if (SB_STRCMP(name, "m_unextpage") == 0) {
    s->m_uNextPage = value;
  } else if (SB_STRCMP(name, "m_unextlength") == 0) {
    s->m_uNextLength = value;
  } else if (SB_STRCMP(name, "m_upage") == 0) {
    s->m_uPage = value;
  } else if (SB_STRCMP(name, "m_usavedbytes") == 0) {
    s->m_uSavedBytes = value;
  } else if (SB_STRCMP(name, "m_uremainingbytes") == 0) {
    s->m_uRemainingBytes = value;
  } else if (SB_STRCMP(name, "m_ueop") == 0) {
    s->m_uEop = value;
  } else if (SB_STRCMP(name, "m_usop") == 0) {
    s->m_uSop = value;
  } else if (SB_STRCMP(name, "m_upktlength") == 0) {
    s->m_uPktLength = value;
  } else if (SB_STRCMP(name, "m_ucontinuebyte") == 0) {
    s->m_uContinueByte = value;
  } else if (SB_STRCMP(name, "m_usourcequeue") == 0) {
    s->m_uSourceQueue = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
