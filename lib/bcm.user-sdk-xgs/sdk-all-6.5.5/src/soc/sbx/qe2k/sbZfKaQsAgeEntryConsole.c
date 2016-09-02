/*
 * $Id: sbZfKaQsAgeEntryConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfKaQsAgeEntryConsole.hx"



/* Print members in struct */
void
sbZfKaQsAgeEntry_Print(sbZfKaQsAgeEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaQsAgeEntry:: res=0x%06x"), (unsigned int)  pFromStruct->m_nReserved));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" noempty=0x%01x"), (unsigned int)  pFromStruct->m_nNoEmpty));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" anemicevent=0x%01x"), (unsigned int)  pFromStruct->m_nAnemicEvent));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" efevent=0x%01x"), (unsigned int)  pFromStruct->m_nEfEvent));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaQsAgeEntry:: cnt=0x%02x"), (unsigned int)  pFromStruct->m_nCnt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfKaQsAgeEntry_SPrint(sbZfKaQsAgeEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaQsAgeEntry:: res=0x%06x", (unsigned int)  pFromStruct->m_nReserved);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," noempty=0x%01x", (unsigned int)  pFromStruct->m_nNoEmpty);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," anemicevent=0x%01x", (unsigned int)  pFromStruct->m_nAnemicEvent);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," efevent=0x%01x", (unsigned int)  pFromStruct->m_nEfEvent);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaQsAgeEntry:: cnt=0x%02x", (unsigned int)  pFromStruct->m_nCnt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfKaQsAgeEntry_Validate(sbZfKaQsAgeEntry_t *pZf) {

  if (pZf->m_nReserved > 0x1fffff) return 0;
  if (pZf->m_nNoEmpty > 0x1) return 0;
  if (pZf->m_nAnemicEvent > 0x1) return 0;
  if (pZf->m_nEfEvent > 0x1) return 0;
  if (pZf->m_nCnt > 0xff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfKaQsAgeEntry_SetField(sbZfKaQsAgeEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_nreserved") == 0) {
    s->m_nReserved = value;
  } else if (SB_STRCMP(name, "m_nnoempty") == 0) {
    s->m_nNoEmpty = value;
  } else if (SB_STRCMP(name, "m_nanemicevent") == 0) {
    s->m_nAnemicEvent = value;
  } else if (SB_STRCMP(name, "m_nefevent") == 0) {
    s->m_nEfEvent = value;
  } else if (SB_STRCMP(name, "m_ncnt") == 0) {
    s->m_nCnt = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
