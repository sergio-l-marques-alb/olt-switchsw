/*
 * $Id: sbZfKaQsPriLutAddrConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfKaQsPriLutAddrConsole.hx"



/* Print members in struct */
void
sbZfKaQsPriLutAddr_Print(sbZfKaQsPriLutAddr_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaQsPriLutAddr:: res=0x%05x"), (unsigned int)  pFromStruct->m_nReserved));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" shaped=0x%01x"), (unsigned int)  pFromStruct->m_nShaped));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" depth=0x%01x"), (unsigned int)  pFromStruct->m_nDepth));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" anemicaged=0x%01x"), (unsigned int)  pFromStruct->m_nAnemicAged));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" qtype=0x%01x"), (unsigned int)  pFromStruct->m_nQType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaQsPriLutAddr:: efaged=0x%01x"), (unsigned int)  pFromStruct->m_nEfAged));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" creditlevel=0x%01x"), (unsigned int)  pFromStruct->m_nCreditLevel));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" holdts=0x%01x"), (unsigned int)  pFromStruct->m_nHoldTs));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pktlen=0x%01x"), (unsigned int)  pFromStruct->m_nPktLen));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfKaQsPriLutAddr_SPrint(sbZfKaQsPriLutAddr_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaQsPriLutAddr:: res=0x%05x", (unsigned int)  pFromStruct->m_nReserved);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," shaped=0x%01x", (unsigned int)  pFromStruct->m_nShaped);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," depth=0x%01x", (unsigned int)  pFromStruct->m_nDepth);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," anemicaged=0x%01x", (unsigned int)  pFromStruct->m_nAnemicAged);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," qtype=0x%01x", (unsigned int)  pFromStruct->m_nQType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaQsPriLutAddr:: efaged=0x%01x", (unsigned int)  pFromStruct->m_nEfAged);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," creditlevel=0x%01x", (unsigned int)  pFromStruct->m_nCreditLevel);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," holdts=0x%01x", (unsigned int)  pFromStruct->m_nHoldTs);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pktlen=0x%01x", (unsigned int)  pFromStruct->m_nPktLen);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfKaQsPriLutAddr_Validate(sbZfKaQsPriLutAddr_t *pZf) {

  if (pZf->m_nReserved > 0x7ffff) return 0;
  if (pZf->m_nShaped > 0x1) return 0;
  if (pZf->m_nDepth > 0x7) return 0;
  if (pZf->m_nAnemicAged > 0x1) return 0;
  if (pZf->m_nQType > 0xf) return 0;
  if (pZf->m_nEfAged > 0x1) return 0;
  if (pZf->m_nCreditLevel > 0x1) return 0;
  if (pZf->m_nHoldTs > 0x1) return 0;
  if (pZf->m_nPktLen > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfKaQsPriLutAddr_SetField(sbZfKaQsPriLutAddr_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_nreserved") == 0) {
    s->m_nReserved = value;
  } else if (SB_STRCMP(name, "m_nshaped") == 0) {
    s->m_nShaped = value;
  } else if (SB_STRCMP(name, "m_ndepth") == 0) {
    s->m_nDepth = value;
  } else if (SB_STRCMP(name, "m_nanemicaged") == 0) {
    s->m_nAnemicAged = value;
  } else if (SB_STRCMP(name, "m_nqtype") == 0) {
    s->m_nQType = value;
  } else if (SB_STRCMP(name, "m_nefaged") == 0) {
    s->m_nEfAged = value;
  } else if (SB_STRCMP(name, "m_ncreditlevel") == 0) {
    s->m_nCreditLevel = value;
  } else if (SB_STRCMP(name, "m_nholdts") == 0) {
    s->m_nHoldTs = value;
  } else if (SB_STRCMP(name, "m_npktlen") == 0) {
    s->m_nPktLen = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
