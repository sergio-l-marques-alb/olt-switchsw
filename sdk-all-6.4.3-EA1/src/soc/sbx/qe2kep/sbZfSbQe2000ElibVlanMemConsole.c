/*
 * $Id: sbZfSbQe2000ElibVlanMemConsole.c,v 1.5 Broadcom SDK $
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
#include "sbZfSbQe2000ElibVlanMemConsole.hx"



/* Print members in struct */
void
sbZfSbQe2000ElibVlanMem_Print(sbZfSbQe2000ElibVlanMem_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("SbQe2000ElibVlanMem:: patches=0x%01x"), (unsigned int)  pFromStruct->m_Patches));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" edges=0x%01x"), (unsigned int)  pFromStruct->m_Edges));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" numfree=0x%01x"), (unsigned int)  pFromStruct->m_NumFree));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" numused=0x%01x"), (unsigned int)  pFromStruct->m_NumUsed));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfSbQe2000ElibVlanMem_SPrint(sbZfSbQe2000ElibVlanMem_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"SbQe2000ElibVlanMem:: patches=0x%01x", (unsigned int)  pFromStruct->m_Patches);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," edges=0x%01x", (unsigned int)  pFromStruct->m_Edges);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," numfree=0x%01x", (unsigned int)  pFromStruct->m_NumFree);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," numused=0x%01x", (unsigned int)  pFromStruct->m_NumUsed);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfSbQe2000ElibVlanMem_Validate(sbZfSbQe2000ElibVlanMem_t *pZf) {

  if (pZf->m_Patches > 0x1) return 0;
  if (pZf->m_Edges > 0x1) return 0;
  if (pZf->m_NumFree > 0x1) return 0;
  if (pZf->m_NumUsed > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfSbQe2000ElibVlanMem_SetField(sbZfSbQe2000ElibVlanMem_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_patches") == 0) {
    s->m_Patches = value;
  } else if (SB_STRCMP(name, "m_edges") == 0) {
    s->m_Edges = value;
  } else if (SB_STRCMP(name, "m_numfree") == 0) {
    s->m_NumFree = value;
  } else if (SB_STRCMP(name, "m_numused") == 0) {
    s->m_NumUsed = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
