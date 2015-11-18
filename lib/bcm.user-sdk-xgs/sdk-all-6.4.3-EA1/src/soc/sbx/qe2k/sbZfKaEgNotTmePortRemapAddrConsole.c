/*
 * $Id: sbZfKaEgNotTmePortRemapAddrConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfKaEgNotTmePortRemapAddrConsole.hx"



/* Print members in struct */
void
sbZfKaEgNotTmePortRemapAddr_Print(sbZfKaEgNotTmePortRemapAddr_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaEgNotTmePortRemapAddr:: txdma=0x%01x"), (unsigned int)  pFromStruct->m_nTxdma));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" ef=0x%01x"), (unsigned int)  pFromStruct->m_nEf));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" qe1k=0x%01x"), (unsigned int)  pFromStruct->m_nQe1k));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" dport=0x%02x"), (unsigned int)  pFromStruct->m_nPort));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfKaEgNotTmePortRemapAddr_SPrint(sbZfKaEgNotTmePortRemapAddr_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaEgNotTmePortRemapAddr:: txdma=0x%01x", (unsigned int)  pFromStruct->m_nTxdma);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," ef=0x%01x", (unsigned int)  pFromStruct->m_nEf);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," qe1k=0x%01x", (unsigned int)  pFromStruct->m_nQe1k);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," dport=0x%02x", (unsigned int)  pFromStruct->m_nPort);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfKaEgNotTmePortRemapAddr_Validate(sbZfKaEgNotTmePortRemapAddr_t *pZf) {

  if (pZf->m_nTxdma > 0x1) return 0;
  if (pZf->m_nEf > 0x1) return 0;
  if (pZf->m_nQe1k > 0x1) return 0;
  if (pZf->m_nPort > 0x3f) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfKaEgNotTmePortRemapAddr_SetField(sbZfKaEgNotTmePortRemapAddr_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_ntxdma") == 0) {
    s->m_nTxdma = value;
  } else if (SB_STRCMP(name, "m_nef") == 0) {
    s->m_nEf = value;
  } else if (SB_STRCMP(name, "m_nqe1k") == 0) {
    s->m_nQe1k = value;
  } else if (SB_STRCMP(name, "m_nport") == 0) {
    s->m_nPort = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
