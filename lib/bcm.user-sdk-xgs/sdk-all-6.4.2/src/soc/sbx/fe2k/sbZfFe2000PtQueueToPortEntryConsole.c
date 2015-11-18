/*
 * $Id: sbZfFe2000PtQueueToPortEntryConsole.c,v 1.5 Broadcom SDK $
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
#include "sbZfFe2000PtQueueToPortEntryConsole.hx"



/* Print members in struct */
void
sbZfFe2000PtQueueToPortEntry_Print(sbZfFe2000PtQueueToPortEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PtQueueToPortEntry:: pbsel=0x%01x"), (unsigned int)  pFromStruct->m_uPbSel));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" ppebound=0x%01x"), (unsigned int)  pFromStruct->m_uPpeBound));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" ppeenqueue=0x%01x"), (unsigned int)  pFromStruct->m_uPpeEnqueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PtQueueToPortEntry:: destinterface=0x%01x"), (unsigned int)  pFromStruct->m_uDestInterface));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" destport=0x%02x"), (unsigned int)  pFromStruct->m_uDestPort));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" destqueue=0x%02x"), (unsigned int)  pFromStruct->m_uDestQueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfFe2000PtQueueToPortEntry_SPrint(sbZfFe2000PtQueueToPortEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PtQueueToPortEntry:: pbsel=0x%01x", (unsigned int)  pFromStruct->m_uPbSel);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," ppebound=0x%01x", (unsigned int)  pFromStruct->m_uPpeBound);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," ppeenqueue=0x%01x", (unsigned int)  pFromStruct->m_uPpeEnqueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PtQueueToPortEntry:: destinterface=0x%01x", (unsigned int)  pFromStruct->m_uDestInterface);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," destport=0x%02x", (unsigned int)  pFromStruct->m_uDestPort);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," destqueue=0x%02x", (unsigned int)  pFromStruct->m_uDestQueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfFe2000PtQueueToPortEntry_Validate(sbZfFe2000PtQueueToPortEntry_t *pZf) {

  if (pZf->m_uPbSel > 0x1) return 0;
  if (pZf->m_uPpeBound > 0x1) return 0;
  if (pZf->m_uPpeEnqueue > 0x1) return 0;
  if (pZf->m_uDestInterface > 0x7) return 0;
  if (pZf->m_uDestPort > 0x3f) return 0;
  if (pZf->m_uDestQueue > 0xff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfFe2000PtQueueToPortEntry_SetField(sbZfFe2000PtQueueToPortEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_upbsel") == 0) {
    s->m_uPbSel = value;
  } else if (SB_STRCMP(name, "m_uppebound") == 0) {
    s->m_uPpeBound = value;
  } else if (SB_STRCMP(name, "m_uppeenqueue") == 0) {
    s->m_uPpeEnqueue = value;
  } else if (SB_STRCMP(name, "m_udestinterface") == 0) {
    s->m_uDestInterface = value;
  } else if (SB_STRCMP(name, "m_udestport") == 0) {
    s->m_uDestPort = value;
  } else if (SB_STRCMP(name, "m_udestqueue") == 0) {
    s->m_uDestQueue = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
