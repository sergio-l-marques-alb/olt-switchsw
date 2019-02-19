/*
 * $Id: sbZfCaPeDiagPV2EtcConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaPeDiagPV2EtcConsole.hx"



/* Print members in struct */
void
sbZfCaPeDiagPV2Etc_Print(sbZfCaPeDiagPV2Etc_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPeDiagPV2Etc:: qid=0x%04x"), (unsigned int)  pFromStruct->m_uQid));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" test=0x%01x"), (unsigned int)  pFromStruct->m_uTest));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" dummy=0x%02x"), (unsigned int)  pFromStruct->m_uDummy));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" vid=0x%03x"), (unsigned int)  pFromStruct->m_uVid));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaPeDiagPV2Etc_SPrint(sbZfCaPeDiagPV2Etc_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPeDiagPV2Etc:: qid=0x%04x", (unsigned int)  pFromStruct->m_uQid);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," test=0x%01x", (unsigned int)  pFromStruct->m_uTest);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," dummy=0x%02x", (unsigned int)  pFromStruct->m_uDummy);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," vid=0x%03x", (unsigned int)  pFromStruct->m_uVid);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaPeDiagPV2Etc_Validate(sbZfCaPeDiagPV2Etc_t *pZf) {

  if (pZf->m_uQid > 0x3fff) return 0;
  if (pZf->m_uTest > 0x1) return 0;
  if (pZf->m_uDummy > 0x1f) return 0;
  if (pZf->m_uVid > 0xfff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaPeDiagPV2Etc_SetField(sbZfCaPeDiagPV2Etc_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_uqid") == 0) {
    s->m_uQid = value;
  } else if (SB_STRCMP(name, "m_utest") == 0) {
    s->m_uTest = value;
  } else if (SB_STRCMP(name, "m_udummy") == 0) {
    s->m_uDummy = value;
  } else if (SB_STRCMP(name, "m_uvid") == 0) {
    s->m_uVid = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
