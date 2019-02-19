/*
 * $Id: sbZfFe2000RcSbPatternConsole.c,v 1.5 Broadcom SDK $
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
#include "sbZfFe2000RcSbPatternConsole.hx"



/* Print members in struct */
void
sbZfFe2000RcSbPattern_Print(sbZfFe2000RcSbPattern_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000RcSbPattern:: pat3=0x%08x"), (unsigned int)  pFromStruct->uPattern3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pat2=0x%08x"), (unsigned int)  pFromStruct->uPattern2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pat1=0x%08x"), (unsigned int)  pFromStruct->uPattern1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000RcSbPattern:: pat0=0x%08x"), (unsigned int)  pFromStruct->uPattern0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfFe2000RcSbPattern_SPrint(sbZfFe2000RcSbPattern_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000RcSbPattern:: pat3=0x%08x", (unsigned int)  pFromStruct->uPattern3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pat2=0x%08x", (unsigned int)  pFromStruct->uPattern2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pat1=0x%08x", (unsigned int)  pFromStruct->uPattern1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000RcSbPattern:: pat0=0x%08x", (unsigned int)  pFromStruct->uPattern0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfFe2000RcSbPattern_Validate(sbZfFe2000RcSbPattern_t *pZf) {

  /* pZf->uPattern3 implicitly masked by data type */
  /* pZf->uPattern2 implicitly masked by data type */
  /* pZf->uPattern1 implicitly masked by data type */
  /* pZf->uPattern0 implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfFe2000RcSbPattern_SetField(sbZfFe2000RcSbPattern_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "upattern3") == 0) {
    s->uPattern3 = value;
  } else if (SB_STRCMP(name, "upattern2") == 0) {
    s->uPattern2 = value;
  } else if (SB_STRCMP(name, "upattern1") == 0) {
    s->uPattern1 = value;
  } else if (SB_STRCMP(name, "upattern0") == 0) {
    s->uPattern0 = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
