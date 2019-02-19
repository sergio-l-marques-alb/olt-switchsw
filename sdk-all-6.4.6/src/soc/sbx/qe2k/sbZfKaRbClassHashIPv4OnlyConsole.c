/*
 * $Id: sbZfKaRbClassHashIPv4OnlyConsole.c,v 1.3 Broadcom SDK $
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
#include <shared/bsl.h>

#include "sbTypes.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfKaRbClassHashIPv4OnlyConsole.hx"



/* Print members in struct */
void
sbZfKaRbClassHashIPv4Only_Print(sbZfKaRbClassHashIPv4Only_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaRbClassHashIPv4Only:: protocol=0x%02x"), (unsigned int)   pFromStruct->m_nProtocol));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pad1=0x%02x"),  COMPILER_64_LO(pFromStruct->m_nPadWord1)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pad0=0x%08x%08x"),  COMPILER_64_HI(pFromStruct->m_nPadWord0), COMPILER_64_LO(pFromStruct->m_nPadWord0)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaRbClassHashIPv4Only:: ipsa=0x%08x"), (unsigned int)  pFromStruct->m_nIpSa));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" ipda=0x%08x"), (unsigned int)  pFromStruct->m_nIpDa));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" skt=0x%08x"), (unsigned int)  pFromStruct->m_nSocket));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaRbClassHashIPv4Only:: pad2=0x%07x"),   COMPILER_64_LO(pFromStruct->m_nPadWord2)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pad3=0x%06x%08x"),  COMPILER_64_HI(pFromStruct->m_nPadWord3), COMPILER_64_LO(pFromStruct->m_nPadWord3)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaRbClassHashIPv4Only:: word1=0x%08x%08x"),  COMPILER_64_HI(pFromStruct->m_nSpareWord1), COMPILER_64_LO(pFromStruct->m_nSpareWord1)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" word0=0x%08x%08x"),  COMPILER_64_HI(pFromStruct->m_nSpareWord0), COMPILER_64_LO(pFromStruct->m_nSpareWord0)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfKaRbClassHashIPv4Only_SPrint(sbZfKaRbClassHashIPv4Only_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaRbClassHashIPv4Only:: protocol=0x%02x", (unsigned int)  pFromStruct->m_nProtocol);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pad1=0x%02x",   COMPILER_64_LO(pFromStruct->m_nPadWord1));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pad0=0x%08x%08x",  COMPILER_64_HI(pFromStruct->m_nPadWord0), COMPILER_64_LO(pFromStruct->m_nPadWord0));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaRbClassHashIPv4Only:: ipsa=0x%08x", (unsigned int)  pFromStruct->m_nIpSa);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," ipda=0x%08x", (unsigned int)  pFromStruct->m_nIpDa);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," skt=0x%08x", (unsigned int)  pFromStruct->m_nSocket);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaRbClassHashIPv4Only:: pad2=0x%07x",   COMPILER_64_LO(pFromStruct->m_nPadWord2));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pad3=0x%06x%08x",  COMPILER_64_HI(pFromStruct->m_nPadWord3), COMPILER_64_LO(pFromStruct->m_nPadWord3));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaRbClassHashIPv4Only:: word1=0x%08x%08x",  COMPILER_64_HI(pFromStruct->m_nSpareWord1), COMPILER_64_LO(pFromStruct->m_nSpareWord1));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," word0=0x%08x%08x",  COMPILER_64_HI(pFromStruct->m_nSpareWord0), COMPILER_64_LO(pFromStruct->m_nSpareWord0));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfKaRbClassHashIPv4Only_Validate(sbZfKaRbClassHashIPv4Only_t *pZf) {
  uint64 nPadWord1Max = COMPILER_64_INIT(0x00000000, 0x000000ff),
         nPadWord2Max = COMPILER_64_INIT(0x00000000, 0x07ffffff), 
         nPadWord3Max = COMPILER_64_INIT(0x001FFFFF, 0xFFFFFFFF);
  if (pZf->m_nProtocol > 0xff) return 0;
  if (COMPILER_64_GT(pZf->m_nPadWord1,nPadWord1Max)) return 0;
  /* pZf->m_nPadWord0 implicitly masked by data type */
  /* pZf->m_nIpSa implicitly masked by data type */
  /* pZf->m_nIpDa implicitly masked by data type */
  /* pZf->m_nSocket implicitly masked by data type */
  if (COMPILER_64_GT(pZf->m_nPadWord2, nPadWord2Max)) return 0;
  if (COMPILER_64_GT(pZf->m_nPadWord3, nPadWord3Max)) return 0;
  /* pZf->m_nSpareWord1 implicitly masked by data type */
  /* pZf->m_nSpareWord0 implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfKaRbClassHashIPv4Only_SetField(sbZfKaRbClassHashIPv4Only_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_nprotocol") == 0) {
    s->m_nProtocol = value;
  } else if (SB_STRCMP(name, "m_npadword1") == 0) {
    COMPILER_64_SET(s->m_nPadWord1,0,value);
  } else if (SB_STRCMP(name, "m_npadword0") == 0) {
    COMPILER_64_SET(s->m_nPadWord0,0,value);
  } else if (SB_STRCMP(name, "m_nipsa") == 0) {
    s->m_nIpSa = value;
  } else if (SB_STRCMP(name, "m_nipda") == 0) {
    s->m_nIpDa = value;
  } else if (SB_STRCMP(name, "m_nsocket") == 0) {
    s->m_nSocket = value;
  } else if (SB_STRCMP(name, "m_npadword2") == 0) {
    COMPILER_64_SET(s->m_nPadWord2,0,value);
  } else if (SB_STRCMP(name, "m_npadword3") == 0) {
    COMPILER_64_SET(s->m_nPadWord3,0,value);
  } else if (SB_STRCMP(name, "m_nspareword1") == 0) {
    COMPILER_64_SET(s->m_nSpareWord1,0,value);
  } else if (SB_STRCMP(name, "m_nspareword0") == 0) {
    COMPILER_64_SET(s->m_nSpareWord0,0,value);
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
