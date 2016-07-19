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

#include "sbTypes.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfC2PpCamRamConfigurationInstance0EntryConsole.hx"



/* Print members in struct */
void
sbZfC2PpCamRamConfigurationInstance0Entry_Print(sbZfC2PpCamRamConfigurationInstance0Entry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: lengthunits=0x%01x"), (unsigned int)  pFromStruct->m_uLengthUnits));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" lengthmask=0x%01x"), (unsigned int)  pFromStruct->m_uLengthMask));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: lengthptr=0x%02x"), (unsigned int)  pFromStruct->m_uLengthPtr));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" statemask=0x%06x"), (unsigned int)  pFromStruct->m_uStateMask));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: state=0x%06x"), (unsigned int)  pFromStruct->m_uState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hashstart=0x%01x"), (unsigned int)  pFromStruct->m_uHashStart));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: hashtemplate=0x%02x"), (unsigned int)  pFromStruct->m_uHashTemplate));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: checkeroffset=0x%02x"), (unsigned int)  pFromStruct->m_uCheckerOffset));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" checkertype=0x%01x"), (unsigned int)  pFromStruct->m_uCheckerType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: headeravalid=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderAValid));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" headeratype=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderAType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: headerabaselength=0x%02x"), (unsigned int)  pFromStruct->m_uHeaderABaseLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: headerbvalid=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderBValid));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" headerbtype=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderBType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: headerbbaselength=0x%02x"), (unsigned int)  pFromStruct->m_uHeaderBBaseLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" shift=0x%02x"), (unsigned int)  pFromStruct->m_uShift));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: variablemask=0x%08x"), (unsigned int)  pFromStruct->m_uVariableMask));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PpCamRamConfigurationInstance0Entry:: variabledata=0x%08x"), (unsigned int)  pFromStruct->m_uVariableData));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2PpCamRamConfigurationInstance0Entry_SPrint(sbZfC2PpCamRamConfigurationInstance0Entry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: lengthunits=0x%01x", (unsigned int)  pFromStruct->m_uLengthUnits);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," lengthmask=0x%01x", (unsigned int)  pFromStruct->m_uLengthMask);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: lengthptr=0x%02x", (unsigned int)  pFromStruct->m_uLengthPtr);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," statemask=0x%06x", (unsigned int)  pFromStruct->m_uStateMask);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: state=0x%06x", (unsigned int)  pFromStruct->m_uState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hashstart=0x%01x", (unsigned int)  pFromStruct->m_uHashStart);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: hashtemplate=0x%02x", (unsigned int)  pFromStruct->m_uHashTemplate);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: checkeroffset=0x%02x", (unsigned int)  pFromStruct->m_uCheckerOffset);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," checkertype=0x%01x", (unsigned int)  pFromStruct->m_uCheckerType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: headeravalid=0x%01x", (unsigned int)  pFromStruct->m_uHeaderAValid);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," headeratype=0x%01x", (unsigned int)  pFromStruct->m_uHeaderAType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: headerabaselength=0x%02x", (unsigned int)  pFromStruct->m_uHeaderABaseLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: headerbvalid=0x%01x", (unsigned int)  pFromStruct->m_uHeaderBValid);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," headerbtype=0x%01x", (unsigned int)  pFromStruct->m_uHeaderBType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: headerbbaselength=0x%02x", (unsigned int)  pFromStruct->m_uHeaderBBaseLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," shift=0x%02x", (unsigned int)  pFromStruct->m_uShift);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: variablemask=0x%08x", (unsigned int)  pFromStruct->m_uVariableMask);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PpCamRamConfigurationInstance0Entry:: variabledata=0x%08x", (unsigned int)  pFromStruct->m_uVariableData);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2PpCamRamConfigurationInstance0Entry_Validate(sbZfC2PpCamRamConfigurationInstance0Entry_t *pZf) {

  if (pZf->m_uLengthUnits > 0x3) return 0;
  if (pZf->m_uLengthMask > 0x7) return 0;
  if (pZf->m_uLengthPtr > 0x1f) return 0;
  if (pZf->m_uStateMask > 0xffffff) return 0;
  if (pZf->m_uState > 0xffffff) return 0;
  if (pZf->m_uHashStart > 0x1) return 0;
  if (pZf->m_uHashTemplate > 0x1f) return 0;
  if (pZf->m_uCheckerOffset > 0x3f) return 0;
  if (pZf->m_uCheckerType > 0xf) return 0;
  if (pZf->m_uHeaderAValid > 0x1) return 0;
  if (pZf->m_uHeaderAType > 0xf) return 0;
  if (pZf->m_uHeaderABaseLength > 0x3f) return 0;
  if (pZf->m_uHeaderBValid > 0x1) return 0;
  if (pZf->m_uHeaderBType > 0xf) return 0;
  if (pZf->m_uHeaderBBaseLength > 0x3f) return 0;
  if (pZf->m_uShift > 0x3f) return 0;
  /* pZf->m_uVariableMask implicitly masked by data type */
  /* pZf->m_uVariableData implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2PpCamRamConfigurationInstance0Entry_SetField(sbZfC2PpCamRamConfigurationInstance0Entry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_ulengthunits") == 0) {
    s->m_uLengthUnits = value;
  } else if (SB_STRCMP(name, "m_ulengthmask") == 0) {
    s->m_uLengthMask = value;
  } else if (SB_STRCMP(name, "m_ulengthptr") == 0) {
    s->m_uLengthPtr = value;
  } else if (SB_STRCMP(name, "m_ustatemask") == 0) {
    s->m_uStateMask = value;
  } else if (SB_STRCMP(name, "m_ustate") == 0) {
    s->m_uState = value;
  } else if (SB_STRCMP(name, "m_uhashstart") == 0) {
    s->m_uHashStart = value;
  } else if (SB_STRCMP(name, "m_uhashtemplate") == 0) {
    s->m_uHashTemplate = value;
  } else if (SB_STRCMP(name, "m_ucheckeroffset") == 0) {
    s->m_uCheckerOffset = value;
  } else if (SB_STRCMP(name, "m_ucheckertype") == 0) {
    s->m_uCheckerType = value;
  } else if (SB_STRCMP(name, "m_uheaderavalid") == 0) {
    s->m_uHeaderAValid = value;
  } else if (SB_STRCMP(name, "m_uheaderatype") == 0) {
    s->m_uHeaderAType = value;
  } else if (SB_STRCMP(name, "m_uheaderabaselength") == 0) {
    s->m_uHeaderABaseLength = value;
  } else if (SB_STRCMP(name, "m_uheaderbvalid") == 0) {
    s->m_uHeaderBValid = value;
  } else if (SB_STRCMP(name, "m_uheaderbtype") == 0) {
    s->m_uHeaderBType = value;
  } else if (SB_STRCMP(name, "m_uheaderbbaselength") == 0) {
    s->m_uHeaderBBaseLength = value;
  } else if (SB_STRCMP(name, "m_ushift") == 0) {
    s->m_uShift = value;
  } else if (SB_STRCMP(name, "m_uvariablemask") == 0) {
    s->m_uVariableMask = value;
  } else if (SB_STRCMP(name, "m_uvariabledata") == 0) {
    s->m_uVariableData = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
