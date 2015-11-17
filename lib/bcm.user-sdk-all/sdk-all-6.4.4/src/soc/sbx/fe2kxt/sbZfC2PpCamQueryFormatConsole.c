/*
 * $Id$
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
#include "sbZfC2PpCamQueryFormatConsole.hx"



/* Print members in struct */
void
sbZfC2PpCamQueryFormat_Print(sbZfC2PpCamQueryFormat_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamQueryFormat:: state=0x%06x"), (unsigned int)  pFromStruct->m_uState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data5=0x%08x"), (unsigned int)  pFromStruct->m_uQueryData5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data4=0x%08x"), (unsigned int)  pFromStruct->m_uQueryData4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamQueryFormat:: data3=0x%08x"), (unsigned int)  pFromStruct->m_uQueryData3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data2=0x%08x"), (unsigned int)  pFromStruct->m_uQueryData2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data1=0x%08x"), (unsigned int)  pFromStruct->m_uQueryData1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamQueryFormat:: data0=0x%08x"), (unsigned int)  pFromStruct->m_uQueryData0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" resv=0x%01x"), (unsigned int)  pFromStruct->m_uResv));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2PpCamQueryFormat_SPrint(sbZfC2PpCamQueryFormat_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamQueryFormat:: state=0x%06x", (unsigned int)  pFromStruct->m_uState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data5=0x%08x", (unsigned int)  pFromStruct->m_uQueryData5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data4=0x%08x", (unsigned int)  pFromStruct->m_uQueryData4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamQueryFormat:: data3=0x%08x", (unsigned int)  pFromStruct->m_uQueryData3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data2=0x%08x", (unsigned int)  pFromStruct->m_uQueryData2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data1=0x%08x", (unsigned int)  pFromStruct->m_uQueryData1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamQueryFormat:: data0=0x%08x", (unsigned int)  pFromStruct->m_uQueryData0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," resv=0x%01x", (unsigned int)  pFromStruct->m_uResv);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2PpCamQueryFormat_Validate(sbZfC2PpCamQueryFormat_t *pZf) {

  if (pZf->m_uState > 0xffffff) return 0;
  /* pZf->m_uQueryData5 implicitly masked by data type */
  /* pZf->m_uQueryData4 implicitly masked by data type */
  /* pZf->m_uQueryData3 implicitly masked by data type */
  /* pZf->m_uQueryData2 implicitly masked by data type */
  /* pZf->m_uQueryData1 implicitly masked by data type */
  if (pZf->m_uQueryData0 > 0x3fffffff) return 0;
  if (pZf->m_uResv > 0x3) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2PpCamQueryFormat_SetField(sbZfC2PpCamQueryFormat_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_ustate") == 0) {
    s->m_uState = value;
  } else if (SB_STRCMP(name, "m_uquerydata5") == 0) {
    s->m_uQueryData5 = value;
  } else if (SB_STRCMP(name, "m_uquerydata4") == 0) {
    s->m_uQueryData4 = value;
  } else if (SB_STRCMP(name, "m_uquerydata3") == 0) {
    s->m_uQueryData3 = value;
  } else if (SB_STRCMP(name, "m_uquerydata2") == 0) {
    s->m_uQueryData2 = value;
  } else if (SB_STRCMP(name, "m_uquerydata1") == 0) {
    s->m_uQueryData1 = value;
  } else if (SB_STRCMP(name, "m_uquerydata0") == 0) {
    s->m_uQueryData0 = value;
  } else if (SB_STRCMP(name, "m_uresv") == 0) {
    s->m_uResv = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
