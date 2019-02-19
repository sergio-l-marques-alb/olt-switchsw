/*
 * $Id: sbZfCaPpByteHashConverterConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaPpByteHashConverterConsole.hx"



/* Print members in struct */
void
sbZfCaPpByteHashConverter_Print(sbZfCaPpByteHashConverter_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit39=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit39));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit38=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit38));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit37=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit37));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit36=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit36));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit35=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit35));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit34=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit34));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit33=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit33));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit32=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit32));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit31=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit31));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit30=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit30));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit29=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit29));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit28=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit28));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit27=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit27));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit26=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit26));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit25=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit25));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit24=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit24));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit23=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit23));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit22=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit22));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit21=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit21));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit20=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit20));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit19=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit19));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit18=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit18));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit17=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit17));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit16=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit16));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit15=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit15));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit14=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit14));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit13=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit13));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit12=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit12));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit11=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit11));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit10=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit10));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit9=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit8=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit7=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit6=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit5=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit4=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit3=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit2=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpByteHashConverter:: hbit1=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hbit0=0x%01x"), (unsigned int)  pFromStruct->m_uHashByteBit0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaPpByteHashConverter_SPrint(sbZfCaPpByteHashConverter_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit39=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit39);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit38=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit38);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit37=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit37);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit36=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit36);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit35=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit35);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit34=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit34);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit33=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit33);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit32=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit32);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit31=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit31);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit30=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit30);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit29=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit29);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit28=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit28);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit27=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit27);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit26=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit26);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit25=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit25);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit24=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit24);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit23=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit23);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit22=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit22);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit21=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit21);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit20=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit20);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit19=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit19);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit18=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit18);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit17=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit17);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit16=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit16);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit15=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit15);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit14=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit14);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit13=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit13);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit12=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit12);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit11=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit11);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit10=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit10);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit9=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit8=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit7=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit6=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit5=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit4=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit3=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit2=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpByteHashConverter:: hbit1=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hbit0=0x%01x", (unsigned int)  pFromStruct->m_uHashByteBit0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaPpByteHashConverter_Validate(sbZfCaPpByteHashConverter_t *pZf) {

  if (pZf->m_uHashByteBit39 > 0x1) return 0;
  if (pZf->m_uHashByteBit38 > 0x1) return 0;
  if (pZf->m_uHashByteBit37 > 0x1) return 0;
  if (pZf->m_uHashByteBit36 > 0x1) return 0;
  if (pZf->m_uHashByteBit35 > 0x1) return 0;
  if (pZf->m_uHashByteBit34 > 0x1) return 0;
  if (pZf->m_uHashByteBit33 > 0x1) return 0;
  if (pZf->m_uHashByteBit32 > 0x1) return 0;
  if (pZf->m_uHashByteBit31 > 0x1) return 0;
  if (pZf->m_uHashByteBit30 > 0x1) return 0;
  if (pZf->m_uHashByteBit29 > 0x1) return 0;
  if (pZf->m_uHashByteBit28 > 0x1) return 0;
  if (pZf->m_uHashByteBit27 > 0x1) return 0;
  if (pZf->m_uHashByteBit26 > 0x1) return 0;
  if (pZf->m_uHashByteBit25 > 0x1) return 0;
  if (pZf->m_uHashByteBit24 > 0x1) return 0;
  if (pZf->m_uHashByteBit23 > 0x1) return 0;
  if (pZf->m_uHashByteBit22 > 0x1) return 0;
  if (pZf->m_uHashByteBit21 > 0x1) return 0;
  if (pZf->m_uHashByteBit20 > 0x1) return 0;
  if (pZf->m_uHashByteBit19 > 0x1) return 0;
  if (pZf->m_uHashByteBit18 > 0x1) return 0;
  if (pZf->m_uHashByteBit17 > 0x1) return 0;
  if (pZf->m_uHashByteBit16 > 0x1) return 0;
  if (pZf->m_uHashByteBit15 > 0x1) return 0;
  if (pZf->m_uHashByteBit14 > 0x1) return 0;
  if (pZf->m_uHashByteBit13 > 0x1) return 0;
  if (pZf->m_uHashByteBit12 > 0x1) return 0;
  if (pZf->m_uHashByteBit11 > 0x1) return 0;
  if (pZf->m_uHashByteBit10 > 0x1) return 0;
  if (pZf->m_uHashByteBit9 > 0x1) return 0;
  if (pZf->m_uHashByteBit8 > 0x1) return 0;
  if (pZf->m_uHashByteBit7 > 0x1) return 0;
  if (pZf->m_uHashByteBit6 > 0x1) return 0;
  if (pZf->m_uHashByteBit5 > 0x1) return 0;
  if (pZf->m_uHashByteBit4 > 0x1) return 0;
  if (pZf->m_uHashByteBit3 > 0x1) return 0;
  if (pZf->m_uHashByteBit2 > 0x1) return 0;
  if (pZf->m_uHashByteBit1 > 0x1) return 0;
  if (pZf->m_uHashByteBit0 > 0x1) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaPpByteHashConverter_SetField(sbZfCaPpByteHashConverter_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_uhashbytebit39") == 0) {
    s->m_uHashByteBit39 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit38") == 0) {
    s->m_uHashByteBit38 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit37") == 0) {
    s->m_uHashByteBit37 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit36") == 0) {
    s->m_uHashByteBit36 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit35") == 0) {
    s->m_uHashByteBit35 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit34") == 0) {
    s->m_uHashByteBit34 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit33") == 0) {
    s->m_uHashByteBit33 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit32") == 0) {
    s->m_uHashByteBit32 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit31") == 0) {
    s->m_uHashByteBit31 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit30") == 0) {
    s->m_uHashByteBit30 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit29") == 0) {
    s->m_uHashByteBit29 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit28") == 0) {
    s->m_uHashByteBit28 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit27") == 0) {
    s->m_uHashByteBit27 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit26") == 0) {
    s->m_uHashByteBit26 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit25") == 0) {
    s->m_uHashByteBit25 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit24") == 0) {
    s->m_uHashByteBit24 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit23") == 0) {
    s->m_uHashByteBit23 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit22") == 0) {
    s->m_uHashByteBit22 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit21") == 0) {
    s->m_uHashByteBit21 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit20") == 0) {
    s->m_uHashByteBit20 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit19") == 0) {
    s->m_uHashByteBit19 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit18") == 0) {
    s->m_uHashByteBit18 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit17") == 0) {
    s->m_uHashByteBit17 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit16") == 0) {
    s->m_uHashByteBit16 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit15") == 0) {
    s->m_uHashByteBit15 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit14") == 0) {
    s->m_uHashByteBit14 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit13") == 0) {
    s->m_uHashByteBit13 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit12") == 0) {
    s->m_uHashByteBit12 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit11") == 0) {
    s->m_uHashByteBit11 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit10") == 0) {
    s->m_uHashByteBit10 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit9") == 0) {
    s->m_uHashByteBit9 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit8") == 0) {
    s->m_uHashByteBit8 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit7") == 0) {
    s->m_uHashByteBit7 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit6") == 0) {
    s->m_uHashByteBit6 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit5") == 0) {
    s->m_uHashByteBit5 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit4") == 0) {
    s->m_uHashByteBit4 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit3") == 0) {
    s->m_uHashByteBit3 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit2") == 0) {
    s->m_uHashByteBit2 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit1") == 0) {
    s->m_uHashByteBit1 = value;
  } else if (SB_STRCMP(name, "m_uhashbytebit0") == 0) {
    s->m_uHashByteBit0 = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
