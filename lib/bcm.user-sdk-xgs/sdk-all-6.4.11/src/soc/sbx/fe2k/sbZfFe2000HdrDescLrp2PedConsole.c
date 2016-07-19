/*
 * $Id: sbZfFe2000HdrDescLrp2PedConsole.c,v 1.5 Broadcom SDK $
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
#include "sbZfFe2000HdrDescLrp2PedConsole.hx"



/* Print members in struct */
void
sbZfFe2000HdrDescLrp2Ped_Print(sbZfFe2000HdrDescLrp2Ped_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: continuebyte=0x%02x"), (unsigned int)  pFromStruct->ulContinueByte));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr0=0x%01x"), (unsigned int)  pFromStruct->ulHdr0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr1=0x%01x"), (unsigned int)  pFromStruct->ulHdr1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr2=0x%01x"), (unsigned int)  pFromStruct->ulHdr2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: hdr3=0x%01x"), (unsigned int)  pFromStruct->ulHdr3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr4=0x%01x"), (unsigned int)  pFromStruct->ulHdr4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr5=0x%01x"), (unsigned int)  pFromStruct->ulHdr5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr6=0x%01x"), (unsigned int)  pFromStruct->ulHdr6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr7=0x%01x"), (unsigned int)  pFromStruct->ulHdr7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: hdr8=0x%01x"), (unsigned int)  pFromStruct->ulHdr8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr9=0x%01x"), (unsigned int)  pFromStruct->ulHdr9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn0=0x%02x"), (unsigned int)  pFromStruct->ulLocn0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn1=0x%02x"), (unsigned int)  pFromStruct->ulLocn1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn2=0x%02x"), (unsigned int)  pFromStruct->ulLocn2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: locn3=0x%02x"), (unsigned int)  pFromStruct->ulLocn3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn4=0x%02x"), (unsigned int)  pFromStruct->ulLocn4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn5=0x%02x"), (unsigned int)  pFromStruct->ulLocn5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn6=0x%02x"), (unsigned int)  pFromStruct->ulLocn6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: locn7=0x%02x"), (unsigned int)  pFromStruct->ulLocn7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn8=0x%02x"), (unsigned int)  pFromStruct->ulLocn8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn9=0x%02x"), (unsigned int)  pFromStruct->ulLocn9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" ulstr=0x%01x"), (unsigned int)  pFromStruct->ulStr));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: ulbuffer=0x%04x"), (unsigned int)  pFromStruct->ulBuffer));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" exceptindex=0x%02x"), (unsigned int)  pFromStruct->ulExceptIndex));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" squeue=0x%02x"), (unsigned int)  pFromStruct->ulSQueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" tc=0x%01x"), (unsigned int)  pFromStruct->ulTc));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: hc=0x%01x"), (unsigned int)  pFromStruct->ulHc));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mirrorindex=0x%01x"), (unsigned int)  pFromStruct->ulMirrorIndex));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" copycnt=0x%03x"), (unsigned int)  pFromStruct->ulCopyCnt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" cm=0x%01x"), (unsigned int)  pFromStruct->ulCM));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" d=0x%01x"), (unsigned int)  pFromStruct->ulD));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: enq=0x%01x"), (unsigned int)  pFromStruct->ulEnq));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" dqueue=0x%02x"), (unsigned int)  pFromStruct->ulDQueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" xferlen=0x%04x"), (unsigned int)  pFromStruct->ulXferLen));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pm=0x%01x"), (unsigned int)  pFromStruct->ulPm));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: padding0=0x%01x"), (unsigned int)  pFromStruct->ulPadding0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" framelen=0x%04x"), (unsigned int)  pFromStruct->ulFrameLen));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" padding1=0x%08x"), (unsigned int)  pFromStruct->ulPadding1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("Fe2000HdrDescLrp2Ped:: padding2=0x%08x"), (unsigned int)  pFromStruct->ulPadding2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfFe2000HdrDescLrp2Ped_SPrint(sbZfFe2000HdrDescLrp2Ped_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: continuebyte=0x%02x", (unsigned int)  pFromStruct->ulContinueByte);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr0=0x%01x", (unsigned int)  pFromStruct->ulHdr0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr1=0x%01x", (unsigned int)  pFromStruct->ulHdr1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr2=0x%01x", (unsigned int)  pFromStruct->ulHdr2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: hdr3=0x%01x", (unsigned int)  pFromStruct->ulHdr3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr4=0x%01x", (unsigned int)  pFromStruct->ulHdr4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr5=0x%01x", (unsigned int)  pFromStruct->ulHdr5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr6=0x%01x", (unsigned int)  pFromStruct->ulHdr6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr7=0x%01x", (unsigned int)  pFromStruct->ulHdr7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: hdr8=0x%01x", (unsigned int)  pFromStruct->ulHdr8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr9=0x%01x", (unsigned int)  pFromStruct->ulHdr9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn0=0x%02x", (unsigned int)  pFromStruct->ulLocn0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn1=0x%02x", (unsigned int)  pFromStruct->ulLocn1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn2=0x%02x", (unsigned int)  pFromStruct->ulLocn2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: locn3=0x%02x", (unsigned int)  pFromStruct->ulLocn3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn4=0x%02x", (unsigned int)  pFromStruct->ulLocn4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn5=0x%02x", (unsigned int)  pFromStruct->ulLocn5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn6=0x%02x", (unsigned int)  pFromStruct->ulLocn6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: locn7=0x%02x", (unsigned int)  pFromStruct->ulLocn7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn8=0x%02x", (unsigned int)  pFromStruct->ulLocn8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn9=0x%02x", (unsigned int)  pFromStruct->ulLocn9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," ulstr=0x%01x", (unsigned int)  pFromStruct->ulStr);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: ulbuffer=0x%04x", (unsigned int)  pFromStruct->ulBuffer);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," exceptindex=0x%02x", (unsigned int)  pFromStruct->ulExceptIndex);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," squeue=0x%02x", (unsigned int)  pFromStruct->ulSQueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," tc=0x%01x", (unsigned int)  pFromStruct->ulTc);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: hc=0x%01x", (unsigned int)  pFromStruct->ulHc);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mirrorindex=0x%01x", (unsigned int)  pFromStruct->ulMirrorIndex);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," copycnt=0x%03x", (unsigned int)  pFromStruct->ulCopyCnt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," cm=0x%01x", (unsigned int)  pFromStruct->ulCM);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," d=0x%01x", (unsigned int)  pFromStruct->ulD);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: enq=0x%01x", (unsigned int)  pFromStruct->ulEnq);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," dqueue=0x%02x", (unsigned int)  pFromStruct->ulDQueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," xferlen=0x%04x", (unsigned int)  pFromStruct->ulXferLen);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pm=0x%01x", (unsigned int)  pFromStruct->ulPm);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: padding0=0x%01x", (unsigned int)  pFromStruct->ulPadding0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," framelen=0x%04x", (unsigned int)  pFromStruct->ulFrameLen);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," padding1=0x%08x", (unsigned int)  pFromStruct->ulPadding1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"Fe2000HdrDescLrp2Ped:: padding2=0x%08x", (unsigned int)  pFromStruct->ulPadding2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfFe2000HdrDescLrp2Ped_Validate(sbZfFe2000HdrDescLrp2Ped_t *pZf) {

  if (pZf->ulContinueByte > 0xff) return 0;
  if (pZf->ulHdr0 > 0xf) return 0;
  if (pZf->ulHdr1 > 0xf) return 0;
  if (pZf->ulHdr2 > 0xf) return 0;
  if (pZf->ulHdr3 > 0xf) return 0;
  if (pZf->ulHdr4 > 0xf) return 0;
  if (pZf->ulHdr5 > 0xf) return 0;
  if (pZf->ulHdr6 > 0xf) return 0;
  if (pZf->ulHdr7 > 0xf) return 0;
  if (pZf->ulHdr8 > 0xf) return 0;
  if (pZf->ulHdr9 > 0xf) return 0;
  if (pZf->ulLocn0 > 0xff) return 0;
  if (pZf->ulLocn1 > 0xff) return 0;
  if (pZf->ulLocn2 > 0xff) return 0;
  if (pZf->ulLocn3 > 0xff) return 0;
  if (pZf->ulLocn4 > 0xff) return 0;
  if (pZf->ulLocn5 > 0xff) return 0;
  if (pZf->ulLocn6 > 0xff) return 0;
  if (pZf->ulLocn7 > 0xff) return 0;
  if (pZf->ulLocn8 > 0xff) return 0;
  if (pZf->ulLocn9 > 0xff) return 0;
  if (pZf->ulStr > 0x7) return 0;
  if (pZf->ulBuffer > 0x1fff) return 0;
  if (pZf->ulExceptIndex > 0xff) return 0;
  if (pZf->ulSQueue > 0xff) return 0;
  if (pZf->ulTc > 0x7) return 0;
  if (pZf->ulHc > 0x1) return 0;
  if (pZf->ulMirrorIndex > 0xf) return 0;
  if (pZf->ulCopyCnt > 0xfff) return 0;
  if (pZf->ulCM > 0x3) return 0;
  if (pZf->ulD > 0x1) return 0;
  if (pZf->ulEnq > 0x1) return 0;
  if (pZf->ulDQueue > 0xff) return 0;
  if (pZf->ulXferLen > 0x3fff) return 0;
  if (pZf->ulPm > 0x3) return 0;
  if (pZf->ulPadding0 > 0x3) return 0;
  if (pZf->ulFrameLen > 0x3fff) return 0;
  /* pZf->ulPadding1 implicitly masked by data type */
  /* pZf->ulPadding2 implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfFe2000HdrDescLrp2Ped_SetField(sbZfFe2000HdrDescLrp2Ped_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "continuebyte") == 0) {
    s->ulContinueByte = value;
  } else if (SB_STRCMP(name, "hdr0") == 0) {
    s->ulHdr0 = value;
  } else if (SB_STRCMP(name, "hdr1") == 0) {
    s->ulHdr1 = value;
  } else if (SB_STRCMP(name, "hdr2") == 0) {
    s->ulHdr2 = value;
  } else if (SB_STRCMP(name, "hdr3") == 0) {
    s->ulHdr3 = value;
  } else if (SB_STRCMP(name, "hdr4") == 0) {
    s->ulHdr4 = value;
  } else if (SB_STRCMP(name, "hdr5") == 0) {
    s->ulHdr5 = value;
  } else if (SB_STRCMP(name, "hdr6") == 0) {
    s->ulHdr6 = value;
  } else if (SB_STRCMP(name, "hdr7") == 0) {
    s->ulHdr7 = value;
  } else if (SB_STRCMP(name, "hdr8") == 0) {
    s->ulHdr8 = value;
  } else if (SB_STRCMP(name, "hdr9") == 0) {
    s->ulHdr9 = value;
  } else if (SB_STRCMP(name, "locn0") == 0) {
    s->ulLocn0 = value;
  } else if (SB_STRCMP(name, "locn1") == 0) {
    s->ulLocn1 = value;
  } else if (SB_STRCMP(name, "locn2") == 0) {
    s->ulLocn2 = value;
  } else if (SB_STRCMP(name, "locn3") == 0) {
    s->ulLocn3 = value;
  } else if (SB_STRCMP(name, "locn4") == 0) {
    s->ulLocn4 = value;
  } else if (SB_STRCMP(name, "locn5") == 0) {
    s->ulLocn5 = value;
  } else if (SB_STRCMP(name, "locn6") == 0) {
    s->ulLocn6 = value;
  } else if (SB_STRCMP(name, "locn7") == 0) {
    s->ulLocn7 = value;
  } else if (SB_STRCMP(name, "locn8") == 0) {
    s->ulLocn8 = value;
  } else if (SB_STRCMP(name, "locn9") == 0) {
    s->ulLocn9 = value;
  } else if (SB_STRCMP(name, "str") == 0) {
    s->ulStr = value;
  } else if (SB_STRCMP(name, "buffer") == 0) {
    s->ulBuffer = value;
  } else if (SB_STRCMP(name, "exceptindex") == 0) {
    s->ulExceptIndex = value;
  } else if (SB_STRCMP(name, "squeue") == 0) {
    s->ulSQueue = value;
  } else if (SB_STRCMP(name, "tc") == 0) {
    s->ulTc = value;
  } else if (SB_STRCMP(name, "hc") == 0) {
    s->ulHc = value;
  } else if (SB_STRCMP(name, "mirrorindex") == 0) {
    s->ulMirrorIndex = value;
  } else if (SB_STRCMP(name, "copycnt") == 0) {
    s->ulCopyCnt = value;
  } else if (SB_STRCMP(name, "cm") == 0) {
    s->ulCM = value;
  } else if (SB_STRCMP(name, "d") == 0) {
    s->ulD = value;
  } else if (SB_STRCMP(name, "enq") == 0) {
    s->ulEnq = value;
  } else if (SB_STRCMP(name, "dqueue") == 0) {
    s->ulDQueue = value;
  } else if (SB_STRCMP(name, "xferlen") == 0) {
    s->ulXferLen = value;
  } else if (SB_STRCMP(name, "pm") == 0) {
    s->ulPm = value;
  } else if (SB_STRCMP(name, "padding0") == 0) {
    s->ulPadding0 = value;
  } else if (SB_STRCMP(name, "framelen") == 0) {
    s->ulFrameLen = value;
  } else if (SB_STRCMP(name, "padding1") == 0) {
    s->ulPadding1 = value;
  } else if (SB_STRCMP(name, "padding2") == 0) {
    s->ulPadding2 = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
