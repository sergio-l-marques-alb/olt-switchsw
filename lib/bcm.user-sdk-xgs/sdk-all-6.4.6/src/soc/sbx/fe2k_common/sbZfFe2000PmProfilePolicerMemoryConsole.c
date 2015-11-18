/*
 * $Id$
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

#include "sbTypesGlue.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfFe2000PmProfilePolicerMemoryConsole.hx"



/* Print members in struct */
void
sbZfFe2000PmProfilePolicerMemory_Print(sbZfFe2000PmProfilePolicerMemory_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmProfilePolicerMemory:: resv0=0x%01x"), (unsigned int)  pFromStruct->uResv0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" type=0x%01x"), (unsigned int)  pFromStruct->uType));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" dred=0x%01x"), (unsigned int)  pFromStruct->bDropOnRed));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" benodcr=0x%01x"), (unsigned int)  pFromStruct->bBktENoDecrement));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmProfilePolicerMemory:: bcnodcr=0x%01x"), (unsigned int)  pFromStruct->bBktCNoDecrement));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" lenshift=0x%01x"), (unsigned int)  pFromStruct->uLengthShift));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" eir=0x%03x"), (unsigned int)  pFromStruct->uEIR));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" cir=0x%03x"), (unsigned int)  pFromStruct->uCIR));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmProfilePolicerMemory:: cflag=0x%01x"), (unsigned int)  pFromStruct->bCouplingFlag));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" r2698=0x%01x"), (unsigned int)  pFromStruct->bRFC2698Mode));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" blind=0x%01x"), (unsigned int)  pFromStruct->bBlind));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" bcsize=0x%02x"), (unsigned int)  pFromStruct->uBktCSize));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000PmProfilePolicerMemory:: ebs=0x%03x"), (unsigned int)  pFromStruct->uEBS));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" cbs=0x%03x"), (unsigned int)  pFromStruct->uCBS));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfFe2000PmProfilePolicerMemory_SPrint(sbZfFe2000PmProfilePolicerMemory_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmProfilePolicerMemory:: resv0=0x%01x", (unsigned int)  pFromStruct->uResv0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," type=0x%01x", (unsigned int)  pFromStruct->uType);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," dred=0x%01x", (unsigned int)  pFromStruct->bDropOnRed);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," benodcr=0x%01x", (unsigned int)  pFromStruct->bBktENoDecrement);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmProfilePolicerMemory:: bcnodcr=0x%01x", (unsigned int)  pFromStruct->bBktCNoDecrement);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," lenshift=0x%01x", (unsigned int)  pFromStruct->uLengthShift);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," eir=0x%03x", (unsigned int)  pFromStruct->uEIR);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," cir=0x%03x", (unsigned int)  pFromStruct->uCIR);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmProfilePolicerMemory:: cflag=0x%01x", (unsigned int)  pFromStruct->bCouplingFlag);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," r2698=0x%01x", (unsigned int)  pFromStruct->bRFC2698Mode);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," blind=0x%01x", (unsigned int)  pFromStruct->bBlind);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," bcsize=0x%02x", (unsigned int)  pFromStruct->uBktCSize);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000PmProfilePolicerMemory:: ebs=0x%03x", (unsigned int)  pFromStruct->uEBS);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," cbs=0x%03x", (unsigned int)  pFromStruct->uCBS);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfFe2000PmProfilePolicerMemory_Validate(sbZfFe2000PmProfilePolicerMemory_t *pZf) {

  if (pZf->uResv0 > 0x1) return 0;
  if (pZf->uType > 0x1) return 0;
  if (pZf->bDropOnRed > 0x1) return 0;
  if (pZf->bBktENoDecrement > 0x1) return 0;
  if (pZf->bBktCNoDecrement > 0x1) return 0;
  if (pZf->uLengthShift > 0x7) return 0;
  if (pZf->uEIR > 0xfff) return 0;
  if (pZf->uCIR > 0xfff) return 0;
  if (pZf->bCouplingFlag > 0x1) return 0;
  if (pZf->bRFC2698Mode > 0x1) return 0;
  if (pZf->bBlind > 0x1) return 0;
  if (pZf->uBktCSize > 0x1f) return 0;
  if (pZf->uEBS > 0xfff) return 0;
  if (pZf->uCBS > 0xfff) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfFe2000PmProfilePolicerMemory_SetField(sbZfFe2000PmProfilePolicerMemory_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "uresv0") == 0) {
    s->uResv0 = value;
  } else if (SB_STRCMP(name, "utype") == 0) {
    s->uType = value;
  } else if (SB_STRCMP(name, "droponred") == 0) {
    s->bDropOnRed = value;
  } else if (SB_STRCMP(name, "bktenodecrement") == 0) {
    s->bBktENoDecrement = value;
  } else if (SB_STRCMP(name, "bktcnodecrement") == 0) {
    s->bBktCNoDecrement = value;
  } else if (SB_STRCMP(name, "ulengthshift") == 0) {
    s->uLengthShift = value;
  } else if (SB_STRCMP(name, "ueir") == 0) {
    s->uEIR = value;
  } else if (SB_STRCMP(name, "ucir") == 0) {
    s->uCIR = value;
  } else if (SB_STRCMP(name, "couplingflag") == 0) {
    s->bCouplingFlag = value;
  } else if (SB_STRCMP(name, "rfc2698mode") == 0) {
    s->bRFC2698Mode = value;
  } else if (SB_STRCMP(name, "blind") == 0) {
    s->bBlind = value;
  } else if (SB_STRCMP(name, "ubktcsize") == 0) {
    s->uBktCSize = value;
  } else if (SB_STRCMP(name, "uebs") == 0) {
    s->uEBS = value;
  } else if (SB_STRCMP(name, "ucbs") == 0) {
    s->uCBS = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
