/*
 * $Id: sbZfKaPmLastLineConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfKaPmLastLineConsole.hx"



/* Print members in struct */
void
sbZfKaPmLastLine_Print(sbZfKaPmLastLine_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaPmLastLine:: res=0x%07x%08x"),   COMPILER_64_HI(pFromStruct->m_nReserved), COMPILER_64_LO(pFromStruct->m_nReserved)));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hec=0x%02x"), (unsigned int)  pFromStruct->m_nHec));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" zero=0x%03x"), (unsigned int)  pFromStruct->m_nZero));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("KaPmLastLine:: next_buffer=0x%05x"), (unsigned int)  pFromStruct->m_nNextBuffer));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" timestamp=0x%08x"), (unsigned int)  pFromStruct->m_nTimestamp));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfKaPmLastLine_SPrint(sbZfKaPmLastLine_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaPmLastLine:: res=0x%07x%08x",  COMPILER_64_HI(pFromStruct->m_nReserved), COMPILER_64_LO(pFromStruct->m_nReserved));
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hec=0x%02x", (unsigned int)  pFromStruct->m_nHec);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," zero=0x%03x", (unsigned int)  pFromStruct->m_nZero);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"KaPmLastLine:: next_buffer=0x%05x", (unsigned int)  pFromStruct->m_nNextBuffer);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," timestamp=0x%08x", (unsigned int)  pFromStruct->m_nTimestamp);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfKaPmLastLine_Validate(sbZfKaPmLastLine_t *pZf) {
  uint64 nReservedMax = COMPILER_64_INIT(0xFFFFFFF, 0xFFFFFFFF);
  if (COMPILER_64_GT(pZf->m_nReserved, nReservedMax)) return 0;
  if (pZf->m_nHec > 0xff) return 0;
  if (pZf->m_nZero > 0x7ff) return 0;
  if (pZf->m_nNextBuffer > 0x1ffff) return 0;
  /* pZf->m_nTimestamp implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfKaPmLastLine_SetField(sbZfKaPmLastLine_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_nreserved") == 0) {
    COMPILER_64_SET(s->m_nReserved,0,value);
  } else if (SB_STRCMP(name, "m_nhec") == 0) {
    s->m_nHec = value;
  } else if (SB_STRCMP(name, "m_nzero") == 0) {
    s->m_nZero = value;
  } else if (SB_STRCMP(name, "m_nnextbuffer") == 0) {
    s->m_nNextBuffer = value;
  } else if (SB_STRCMP(name, "m_ntimestamp") == 0) {
    s->m_nTimestamp = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
