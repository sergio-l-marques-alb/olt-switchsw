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
#include "sbZfC2PrCcCamRamLastEntryConsole.hx"



/* Print members in struct */
void
sbZfC2PrCcCamRamLastEntry_Print(sbZfC2PrCcCamRamLastEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport31=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort31));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport30=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort30));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport29=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort29));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport28=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort28));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport27=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort27));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport26=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort26));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport25=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort25));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport24=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort24));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport23=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort23));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport22=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort22));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport21=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort21));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport20=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort20));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport19=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort19));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport18=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort18));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport17=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort17));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport16=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort16));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport15=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort15));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport14=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort14));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport13=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort13));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport12=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort12));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport11=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort11));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport10=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort10));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport9=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport8=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport7=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport6=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport5=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport4=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: deport3=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport2=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport1=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" deport0=0x%01x"), (unsigned int)  pFromStruct->m_uDEPort0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: drop=0x%01x"), (unsigned int)  pFromStruct->m_uDrop));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" queueaction=0x%01x"), (unsigned int)  pFromStruct->m_uQueueAction));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" queue=0x%02x"), (unsigned int)  pFromStruct->m_uQueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" e2ecc=0x%01x"), (unsigned int)  pFromStruct->m_uE2ECC));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcCamRamLastEntry:: last=0x%01x"), (unsigned int)  pFromStruct->m_uLast));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2PrCcCamRamLastEntry_SPrint(sbZfC2PrCcCamRamLastEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport31=0x%01x", (unsigned int)  pFromStruct->m_uDEPort31);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport30=0x%01x", (unsigned int)  pFromStruct->m_uDEPort30);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport29=0x%01x", (unsigned int)  pFromStruct->m_uDEPort29);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport28=0x%01x", (unsigned int)  pFromStruct->m_uDEPort28);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport27=0x%01x", (unsigned int)  pFromStruct->m_uDEPort27);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport26=0x%01x", (unsigned int)  pFromStruct->m_uDEPort26);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport25=0x%01x", (unsigned int)  pFromStruct->m_uDEPort25);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport24=0x%01x", (unsigned int)  pFromStruct->m_uDEPort24);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport23=0x%01x", (unsigned int)  pFromStruct->m_uDEPort23);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport22=0x%01x", (unsigned int)  pFromStruct->m_uDEPort22);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport21=0x%01x", (unsigned int)  pFromStruct->m_uDEPort21);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport20=0x%01x", (unsigned int)  pFromStruct->m_uDEPort20);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport19=0x%01x", (unsigned int)  pFromStruct->m_uDEPort19);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport18=0x%01x", (unsigned int)  pFromStruct->m_uDEPort18);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport17=0x%01x", (unsigned int)  pFromStruct->m_uDEPort17);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport16=0x%01x", (unsigned int)  pFromStruct->m_uDEPort16);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport15=0x%01x", (unsigned int)  pFromStruct->m_uDEPort15);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport14=0x%01x", (unsigned int)  pFromStruct->m_uDEPort14);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport13=0x%01x", (unsigned int)  pFromStruct->m_uDEPort13);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport12=0x%01x", (unsigned int)  pFromStruct->m_uDEPort12);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport11=0x%01x", (unsigned int)  pFromStruct->m_uDEPort11);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport10=0x%01x", (unsigned int)  pFromStruct->m_uDEPort10);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport9=0x%01x", (unsigned int)  pFromStruct->m_uDEPort9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport8=0x%01x", (unsigned int)  pFromStruct->m_uDEPort8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport7=0x%01x", (unsigned int)  pFromStruct->m_uDEPort7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport6=0x%01x", (unsigned int)  pFromStruct->m_uDEPort6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport5=0x%01x", (unsigned int)  pFromStruct->m_uDEPort5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport4=0x%01x", (unsigned int)  pFromStruct->m_uDEPort4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: deport3=0x%01x", (unsigned int)  pFromStruct->m_uDEPort3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport2=0x%01x", (unsigned int)  pFromStruct->m_uDEPort2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport1=0x%01x", (unsigned int)  pFromStruct->m_uDEPort1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," deport0=0x%01x", (unsigned int)  pFromStruct->m_uDEPort0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: drop=0x%01x", (unsigned int)  pFromStruct->m_uDrop);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," queueaction=0x%01x", (unsigned int)  pFromStruct->m_uQueueAction);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," queue=0x%02x", (unsigned int)  pFromStruct->m_uQueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," e2ecc=0x%01x", (unsigned int)  pFromStruct->m_uE2ECC);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcCamRamLastEntry:: last=0x%01x", (unsigned int)  pFromStruct->m_uLast);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2PrCcCamRamLastEntry_Validate(sbZfC2PrCcCamRamLastEntry_t *pZf) {

  if (pZf->m_uDEPort31 > 0x3) return 0;
  if (pZf->m_uDEPort30 > 0x3) return 0;
  if (pZf->m_uDEPort29 > 0x3) return 0;
  if (pZf->m_uDEPort28 > 0x3) return 0;
  if (pZf->m_uDEPort27 > 0x3) return 0;
  if (pZf->m_uDEPort26 > 0x3) return 0;
  if (pZf->m_uDEPort25 > 0x3) return 0;
  if (pZf->m_uDEPort24 > 0x3) return 0;
  if (pZf->m_uDEPort23 > 0x3) return 0;
  if (pZf->m_uDEPort22 > 0x3) return 0;
  if (pZf->m_uDEPort21 > 0x3) return 0;
  if (pZf->m_uDEPort20 > 0x3) return 0;
  if (pZf->m_uDEPort19 > 0x3) return 0;
  if (pZf->m_uDEPort18 > 0x3) return 0;
  if (pZf->m_uDEPort17 > 0x3) return 0;
  if (pZf->m_uDEPort16 > 0x3) return 0;
  if (pZf->m_uDEPort15 > 0x3) return 0;
  if (pZf->m_uDEPort14 > 0x3) return 0;
  if (pZf->m_uDEPort13 > 0x3) return 0;
  if (pZf->m_uDEPort12 > 0x3) return 0;
  if (pZf->m_uDEPort11 > 0x3) return 0;
  if (pZf->m_uDEPort10 > 0x3) return 0;
  if (pZf->m_uDEPort9 > 0x3) return 0;
  if (pZf->m_uDEPort8 > 0x3) return 0;
  if (pZf->m_uDEPort7 > 0x3) return 0;
  if (pZf->m_uDEPort6 > 0x3) return 0;
  if (pZf->m_uDEPort5 > 0x3) return 0;
  if (pZf->m_uDEPort4 > 0x3) return 0;
  if (pZf->m_uDEPort3 > 0x3) return 0;
  if (pZf->m_uDEPort2 > 0x3) return 0;
  if (pZf->m_uDEPort1 > 0x3) return 0;
  if (pZf->m_uDEPort0 > 0x3) return 0;
  if (pZf->m_uDrop > 0x1) return 0;
  if (pZf->m_uQueueAction > 0x3) return 0;
  if (pZf->m_uQueue > 0xff) return 0;
  if (pZf->m_uE2ECC > 0x1) return 0;
  if (pZf->m_uLast > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2PrCcCamRamLastEntry_SetField(sbZfC2PrCcCamRamLastEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_udeport31") == 0) {
    s->m_uDEPort31 = value;
  } else if (SB_STRCMP(name, "m_udeport30") == 0) {
    s->m_uDEPort30 = value;
  } else if (SB_STRCMP(name, "m_udeport29") == 0) {
    s->m_uDEPort29 = value;
  } else if (SB_STRCMP(name, "m_udeport28") == 0) {
    s->m_uDEPort28 = value;
  } else if (SB_STRCMP(name, "m_udeport27") == 0) {
    s->m_uDEPort27 = value;
  } else if (SB_STRCMP(name, "m_udeport26") == 0) {
    s->m_uDEPort26 = value;
  } else if (SB_STRCMP(name, "m_udeport25") == 0) {
    s->m_uDEPort25 = value;
  } else if (SB_STRCMP(name, "m_udeport24") == 0) {
    s->m_uDEPort24 = value;
  } else if (SB_STRCMP(name, "m_udeport23") == 0) {
    s->m_uDEPort23 = value;
  } else if (SB_STRCMP(name, "m_udeport22") == 0) {
    s->m_uDEPort22 = value;
  } else if (SB_STRCMP(name, "m_udeport21") == 0) {
    s->m_uDEPort21 = value;
  } else if (SB_STRCMP(name, "m_udeport20") == 0) {
    s->m_uDEPort20 = value;
  } else if (SB_STRCMP(name, "m_udeport19") == 0) {
    s->m_uDEPort19 = value;
  } else if (SB_STRCMP(name, "m_udeport18") == 0) {
    s->m_uDEPort18 = value;
  } else if (SB_STRCMP(name, "m_udeport17") == 0) {
    s->m_uDEPort17 = value;
  } else if (SB_STRCMP(name, "m_udeport16") == 0) {
    s->m_uDEPort16 = value;
  } else if (SB_STRCMP(name, "m_udeport15") == 0) {
    s->m_uDEPort15 = value;
  } else if (SB_STRCMP(name, "m_udeport14") == 0) {
    s->m_uDEPort14 = value;
  } else if (SB_STRCMP(name, "m_udeport13") == 0) {
    s->m_uDEPort13 = value;
  } else if (SB_STRCMP(name, "m_udeport12") == 0) {
    s->m_uDEPort12 = value;
  } else if (SB_STRCMP(name, "m_udeport11") == 0) {
    s->m_uDEPort11 = value;
  } else if (SB_STRCMP(name, "m_udeport10") == 0) {
    s->m_uDEPort10 = value;
  } else if (SB_STRCMP(name, "m_udeport9") == 0) {
    s->m_uDEPort9 = value;
  } else if (SB_STRCMP(name, "m_udeport8") == 0) {
    s->m_uDEPort8 = value;
  } else if (SB_STRCMP(name, "m_udeport7") == 0) {
    s->m_uDEPort7 = value;
  } else if (SB_STRCMP(name, "m_udeport6") == 0) {
    s->m_uDEPort6 = value;
  } else if (SB_STRCMP(name, "m_udeport5") == 0) {
    s->m_uDEPort5 = value;
  } else if (SB_STRCMP(name, "m_udeport4") == 0) {
    s->m_uDEPort4 = value;
  } else if (SB_STRCMP(name, "m_udeport3") == 0) {
    s->m_uDEPort3 = value;
  } else if (SB_STRCMP(name, "m_udeport2") == 0) {
    s->m_uDEPort2 = value;
  } else if (SB_STRCMP(name, "m_udeport1") == 0) {
    s->m_uDEPort1 = value;
  } else if (SB_STRCMP(name, "m_udeport0") == 0) {
    s->m_uDEPort0 = value;
  } else if (SB_STRCMP(name, "m_udrop") == 0) {
    s->m_uDrop = value;
  } else if (SB_STRCMP(name, "m_uqueueaction") == 0) {
    s->m_uQueueAction = value;
  } else if (SB_STRCMP(name, "m_uqueue") == 0) {
    s->m_uQueue = value;
  } else if (SB_STRCMP(name, "m_ue2ecc") == 0) {
    s->m_uE2ECC = value;
  } else if (SB_STRCMP(name, "m_ulast") == 0) {
    s->m_uLast = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
