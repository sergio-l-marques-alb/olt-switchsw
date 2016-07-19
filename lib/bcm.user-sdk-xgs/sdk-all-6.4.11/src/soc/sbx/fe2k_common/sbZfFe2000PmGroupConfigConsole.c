/*
 * $Id$
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

#include "sbTypesGlue.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfFe2000PmGroupConfigConsole.hx"



/* Print members in struct */
void
sbZfFe2000PmGroupConfig_Print(sbZfFe2000PmGroupConfig_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmGroupConfig:: enable=0x%01x"), (unsigned int)  pFromStruct->uEnable));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" spare2=0x%02x"), (unsigned int)  pFromStruct->uSpare2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pmin=0x%06x"), (unsigned int)  pFromStruct->uMinPolicerId));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" spare1=0x%03x"), (unsigned int)  pFromStruct->uSpare1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmGroupConfig:: pmax=0x%06x"), (unsigned int)  pFromStruct->uMaxPolicerId));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" rcount=0x%01x"), (unsigned int)  pFromStruct->uRefreshCount));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" rthrsh=0x%03x"), (unsigned int)  pFromStruct->uRefreshThresh));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" rprd=0x%04x"), (unsigned int)  pFromStruct->uRefreshPeriod));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmGroupConfig:: spare0=0x%01x"), (unsigned int)  pFromStruct->uSpare0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" tena=0x%01x"), (unsigned int)  pFromStruct->uTimerEnable));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" tsoff=0x%02x"), (unsigned int)  pFromStruct->uTimestampOffset));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" tprd=0x%06x"), (unsigned int)  pFromStruct->uTimerTickPeriod));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfFe2000PmGroupConfig_SPrint(sbZfFe2000PmGroupConfig_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmGroupConfig:: enable=0x%01x", (unsigned int)  pFromStruct->uEnable);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," spare2=0x%02x", (unsigned int)  pFromStruct->uSpare2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pmin=0x%06x", (unsigned int)  pFromStruct->uMinPolicerId);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," spare1=0x%03x", (unsigned int)  pFromStruct->uSpare1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmGroupConfig:: pmax=0x%06x", (unsigned int)  pFromStruct->uMaxPolicerId);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," rcount=0x%01x", (unsigned int)  pFromStruct->uRefreshCount);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," rthrsh=0x%03x", (unsigned int)  pFromStruct->uRefreshThresh);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," rprd=0x%04x", (unsigned int)  pFromStruct->uRefreshPeriod);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmGroupConfig:: spare0=0x%01x", (unsigned int)  pFromStruct->uSpare0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," tena=0x%01x", (unsigned int)  pFromStruct->uTimerEnable);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," tsoff=0x%02x", (unsigned int)  pFromStruct->uTimestampOffset);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," tprd=0x%06x", (unsigned int)  pFromStruct->uTimerTickPeriod);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfFe2000PmGroupConfig_Validate(sbZfFe2000PmGroupConfig_t *pZf) {

  if (pZf->uEnable > 0x1) return 0;
  if (pZf->uSpare2 > 0xff) return 0;
  if (pZf->uMinPolicerId > 0x7fffff) return 0;
  if (pZf->uSpare1 > 0x1ff) return 0;
  if (pZf->uMaxPolicerId > 0x7fffff) return 0;
  if (pZf->uRefreshCount > 0xf) return 0;
  if (pZf->uRefreshThresh > 0xfff) return 0;
  if (pZf->uRefreshPeriod > 0xffff) return 0;
  if (pZf->uSpare0 > 0x1) return 0;
  if (pZf->uTimerEnable > 0x1) return 0;
  if (pZf->uTimestampOffset > 0x3f) return 0;
  if (pZf->uTimerTickPeriod > 0xffffff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfFe2000PmGroupConfig_SetField(sbZfFe2000PmGroupConfig_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "uenable") == 0) {
    s->uEnable = value;
  } else if (SB_STRCMP(name, "uspare2") == 0) {
    s->uSpare2 = value;
  } else if (SB_STRCMP(name, "uminpolicerid") == 0) {
    s->uMinPolicerId = value;
  } else if (SB_STRCMP(name, "uspare1") == 0) {
    s->uSpare1 = value;
  } else if (SB_STRCMP(name, "umaxpolicerid") == 0) {
    s->uMaxPolicerId = value;
  } else if (SB_STRCMP(name, "urefreshcount") == 0) {
    s->uRefreshCount = value;
  } else if (SB_STRCMP(name, "urefreshthresh") == 0) {
    s->uRefreshThresh = value;
  } else if (SB_STRCMP(name, "urefreshperiod") == 0) {
    s->uRefreshPeriod = value;
  } else if (SB_STRCMP(name, "uspare0") == 0) {
    s->uSpare0 = value;
  } else if (SB_STRCMP(name, "utimerenable") == 0) {
    s->uTimerEnable = value;
  } else if (SB_STRCMP(name, "utimestampoffset") == 0) {
    s->uTimestampOffset = value;
  } else if (SB_STRCMP(name, "utimertickperiod") == 0) {
    s->uTimerTickPeriod = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
