/*
 * $Id: sbZfCaQmDequeueState0EntryConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaQmDequeueState0EntryConsole.hx"



/* Print members in struct */
void
sbZfCaQmDequeueState0Entry_Print(sbZfCaQmDequeueState0Entry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaQmDequeueState0Entry:: drop=0x%01x"), (unsigned int)  pFromStruct->m_uDrop));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" remainingpageshi=0x%01x"), (unsigned int)  pFromStruct->m_uRemainingPagesHi));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaQmDequeueState0Entry:: remainingpageslo=0x%02x"), (unsigned int)  pFromStruct->m_uRemainingPagesLo));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" replicantnumber=0x%03x"), (unsigned int)  pFromStruct->m_uReplicantNumber));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaQmDequeueState0Entry:: replicantlength=0x%04x"), (unsigned int)  pFromStruct->m_uReplicantLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaQmDequeueState0Entry_SPrint(sbZfCaQmDequeueState0Entry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaQmDequeueState0Entry:: drop=0x%01x", (unsigned int)  pFromStruct->m_uDrop);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," remainingpageshi=0x%01x", (unsigned int)  pFromStruct->m_uRemainingPagesHi);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaQmDequeueState0Entry:: remainingpageslo=0x%02x", (unsigned int)  pFromStruct->m_uRemainingPagesLo);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," replicantnumber=0x%03x", (unsigned int)  pFromStruct->m_uReplicantNumber);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaQmDequeueState0Entry:: replicantlength=0x%04x", (unsigned int)  pFromStruct->m_uReplicantLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaQmDequeueState0Entry_Validate(sbZfCaQmDequeueState0Entry_t *pZf) {

  if (pZf->m_uDrop > 0x1) return 0;
  if (pZf->m_uRemainingPagesHi > 0x1) return 0;
  if (pZf->m_uRemainingPagesLo > 0x3f) return 0;
  if (pZf->m_uReplicantNumber > 0xfff) return 0;
  if (pZf->m_uReplicantLength > 0x3fff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaQmDequeueState0Entry_SetField(sbZfCaQmDequeueState0Entry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_udrop") == 0) {
    s->m_uDrop = value;
  } else if (SB_STRCMP(name, "m_uremainingpageshi") == 0) {
    s->m_uRemainingPagesHi = value;
  } else if (SB_STRCMP(name, "m_uremainingpageslo") == 0) {
    s->m_uRemainingPagesLo = value;
  } else if (SB_STRCMP(name, "m_ureplicantnumber") == 0) {
    s->m_uReplicantNumber = value;
  } else if (SB_STRCMP(name, "m_ureplicantlength") == 0) {
    s->m_uReplicantLength = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
