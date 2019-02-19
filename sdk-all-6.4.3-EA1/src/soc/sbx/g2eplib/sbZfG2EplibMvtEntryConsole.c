/*
 * $Id: sbZfG2EplibMvtEntryConsole.c,v 1.8 Broadcom SDK $
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
#include "sbZfG2EplibMvtEntryConsole.hx"



/* Print members in struct */
void
sbZfG2EplibMvtEntry_Print(sbZfG2EplibMvtEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("G2EplibMvtEntry:: portmask=0x%01x"), COMPILER_64_LO(pFromStruct->ullPortMask)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" type=0x%01x"), (unsigned int)  pFromStruct->nType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mvtda=0x%01x"), (unsigned int)  pFromStruct->ulMvtdA));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mvtdb=0x%01x"), (unsigned int)  pFromStruct->ulMvtdB));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("G2EplibMvtEntry:: sourceknockout=0x%01x"), (unsigned int)  pFromStruct->bSourceKnockout));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" enablechaining=0x%01x"), (unsigned int)  pFromStruct->bEnableChaining));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" nextmcgroup=0x%01x"), (unsigned int)  pFromStruct->ulNextMcGroup));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfG2EplibMvtEntry_SPrint(sbZfG2EplibMvtEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"G2EplibMvtEntry:: portmask=0x%01x", COMPILER_64_LO(pFromStruct->ullPortMask));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," type=0x%01x", (unsigned int)  pFromStruct->nType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mvtda=0x%01x", (unsigned int)  pFromStruct->ulMvtdA);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mvtdb=0x%01x", (unsigned int)  pFromStruct->ulMvtdB);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"G2EplibMvtEntry:: sourceknockout=0x%01x", (unsigned int)  pFromStruct->bSourceKnockout);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," enablechaining=0x%01x", (unsigned int)  pFromStruct->bEnableChaining);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," nextmcgroup=0x%01x", (unsigned int)  pFromStruct->ulNextMcGroup);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfG2EplibMvtEntry_Validate(sbZfG2EplibMvtEntry_t *pZf) {
  uint64 ullPortMask;
  COMPILER_64_SET(ullPortMask,0,1);
  if (COMPILER_64_GT(pZf->ullPortMask, ullPortMask)) return 0;
  if (pZf->nType > 0x1) return 0;
  if (pZf->ulMvtdA > 0x1) return 0;
  if (pZf->ulMvtdB > 0x1) return 0;
  if (pZf->bSourceKnockout > 0x1) return 0;
  if (pZf->bEnableChaining > 0x1) return 0;
  if (pZf->ulNextMcGroup > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfG2EplibMvtEntry_SetField(sbZfG2EplibMvtEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "lportmask") == 0) {
    COMPILER_64_SET(s->ullPortMask,0,value);
  } else if (SB_STRCMP(name, "ntype") == 0) {
    s->nType = value;
  } else if (SB_STRCMP(name, "mvtda") == 0) {
    s->ulMvtdA = value;
  } else if (SB_STRCMP(name, "mvtdb") == 0) {
    s->ulMvtdB = value;
  } else if (SB_STRCMP(name, "sourceknockout") == 0) {
    s->bSourceKnockout = value;
  } else if (SB_STRCMP(name, "enablechaining") == 0) {
    s->bEnableChaining = value;
  } else if (SB_STRCMP(name, "nextmcgroup") == 0) {
    s->ulNextMcGroup = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
