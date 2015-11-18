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
#include "sbZfC2PpFilterDataOverlayFormatConsole.hx"



/* Print members in struct */
void
sbZfC2PpFilterDataOverlayFormat_Print(sbZfC2PpFilterDataOverlayFormat_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpFilterDataOverlayFormat:: frag=0x%01x"), (unsigned int)  pFromStruct->m_uFragment));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" lstaid=0x%02x"), (unsigned int)  pFromStruct->m_uLocalStationId));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" fm0=0x%01x"), (unsigned int)  pFromStruct->m_uFilterMatch0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" fm1=0x%01x"), (unsigned int)  pFromStruct->m_uFilterMatch1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" fid0=0x%01x"), (unsigned int)  pFromStruct->m_uFilterId0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpFilterDataOverlayFormat:: fid1=0x%01x"), (unsigned int)  pFromStruct->m_uFilterId1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2PpFilterDataOverlayFormat_SPrint(sbZfC2PpFilterDataOverlayFormat_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpFilterDataOverlayFormat:: frag=0x%01x", (unsigned int)  pFromStruct->m_uFragment);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," lstaid=0x%02x", (unsigned int)  pFromStruct->m_uLocalStationId);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," fm0=0x%01x", (unsigned int)  pFromStruct->m_uFilterMatch0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," fm1=0x%01x", (unsigned int)  pFromStruct->m_uFilterMatch1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," fid0=0x%01x", (unsigned int)  pFromStruct->m_uFilterId0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpFilterDataOverlayFormat:: fid1=0x%01x", (unsigned int)  pFromStruct->m_uFilterId1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2PpFilterDataOverlayFormat_Validate(sbZfC2PpFilterDataOverlayFormat_t *pZf) {

  if (pZf->m_uFragment > 0x1) return 0;
  if (pZf->m_uLocalStationId > 0x1f) return 0;
  if (pZf->m_uFilterMatch0 > 0x1) return 0;
  if (pZf->m_uFilterMatch1 > 0x1) return 0;
  if (pZf->m_uFilterId0 > 0xf) return 0;
  if (pZf->m_uFilterId1 > 0xf) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2PpFilterDataOverlayFormat_SetField(sbZfC2PpFilterDataOverlayFormat_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_ufragment") == 0) {
    s->m_uFragment = value;
  } else if (SB_STRCMP(name, "m_ulocalstationid") == 0) {
    s->m_uLocalStationId = value;
  } else if (SB_STRCMP(name, "m_ufiltermatch0") == 0) {
    s->m_uFilterMatch0 = value;
  } else if (SB_STRCMP(name, "m_ufiltermatch1") == 0) {
    s->m_uFilterMatch1 = value;
  } else if (SB_STRCMP(name, "m_ufilterid0") == 0) {
    s->m_uFilterId0 = value;
  } else if (SB_STRCMP(name, "m_ufilterid1") == 0) {
    s->m_uFilterId1 = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
