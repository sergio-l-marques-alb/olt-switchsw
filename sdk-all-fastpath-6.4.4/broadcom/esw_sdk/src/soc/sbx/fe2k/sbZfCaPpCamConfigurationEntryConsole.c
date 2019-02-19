/*
 * $Id: sbZfCaPpCamConfigurationEntryConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaPpCamConfigurationEntryConsole.hx"



/* Print members in struct */
void
sbZfCaPpCamConfigurationEntry_Print(sbZfCaPpCamConfigurationEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpCamConfigurationEntry:: spare0=0x%02x"), (unsigned int)  pFromStruct->m_uSpare0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask5=0x%06x"), (unsigned int)  pFromStruct->m_uMask5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask4=0x%08x"), (unsigned int)  pFromStruct->m_uMask4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpCamConfigurationEntry:: mask3=0x%08x"), (unsigned int)  pFromStruct->m_uMask3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask2=0x%08x"), (unsigned int)  pFromStruct->m_uMask2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpCamConfigurationEntry:: mask1=0x%08x"), (unsigned int)  pFromStruct->m_uMask1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask0=0x%08x"), (unsigned int)  pFromStruct->m_uMask0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" spare1=0x%02x"), (unsigned int)  pFromStruct->m_uSpare1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpCamConfigurationEntry:: valid=0x%01x"), (unsigned int)  pFromStruct->m_uValid));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data5=0x%06x"), (unsigned int)  pFromStruct->m_uData5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data4=0x%08x"), (unsigned int)  pFromStruct->m_uData4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpCamConfigurationEntry:: data3=0x%08x"), (unsigned int)  pFromStruct->m_uData3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data2=0x%08x"), (unsigned int)  pFromStruct->m_uData2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpCamConfigurationEntry:: data1=0x%08x"), (unsigned int)  pFromStruct->m_uData1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" data0=0x%08x"), (unsigned int)  pFromStruct->m_uData0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaPpCamConfigurationEntry_SPrint(sbZfCaPpCamConfigurationEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpCamConfigurationEntry:: spare0=0x%02x", (unsigned int)  pFromStruct->m_uSpare0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask5=0x%06x", (unsigned int)  pFromStruct->m_uMask5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask4=0x%08x", (unsigned int)  pFromStruct->m_uMask4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpCamConfigurationEntry:: mask3=0x%08x", (unsigned int)  pFromStruct->m_uMask3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask2=0x%08x", (unsigned int)  pFromStruct->m_uMask2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpCamConfigurationEntry:: mask1=0x%08x", (unsigned int)  pFromStruct->m_uMask1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask0=0x%08x", (unsigned int)  pFromStruct->m_uMask0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," spare1=0x%02x", (unsigned int)  pFromStruct->m_uSpare1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpCamConfigurationEntry:: valid=0x%01x", (unsigned int)  pFromStruct->m_uValid);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data5=0x%06x", (unsigned int)  pFromStruct->m_uData5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data4=0x%08x", (unsigned int)  pFromStruct->m_uData4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpCamConfigurationEntry:: data3=0x%08x", (unsigned int)  pFromStruct->m_uData3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data2=0x%08x", (unsigned int)  pFromStruct->m_uData2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpCamConfigurationEntry:: data1=0x%08x", (unsigned int)  pFromStruct->m_uData1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," data0=0x%08x", (unsigned int)  pFromStruct->m_uData0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaPpCamConfigurationEntry_Validate(sbZfCaPpCamConfigurationEntry_t *pZf) {

  if (pZf->m_uSpare0 > 0xff) return 0;
  if (pZf->m_uMask5 > 0xffffff) return 0;
  /* pZf->m_uMask4 implicitly masked by data type */
  /* pZf->m_uMask3 implicitly masked by data type */
  /* pZf->m_uMask2 implicitly masked by data type */
  /* pZf->m_uMask1 implicitly masked by data type */
  /* pZf->m_uMask0 implicitly masked by data type */
  if (pZf->m_uSpare1 > 0x3f) return 0;
  if (pZf->m_uValid > 0x3) return 0;
  if (pZf->m_uData5 > 0xffffff) return 0;
  /* pZf->m_uData4 implicitly masked by data type */
  /* pZf->m_uData3 implicitly masked by data type */
  /* pZf->m_uData2 implicitly masked by data type */
  /* pZf->m_uData1 implicitly masked by data type */
  /* pZf->m_uData0 implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaPpCamConfigurationEntry_SetField(sbZfCaPpCamConfigurationEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_uspare0") == 0) {
    s->m_uSpare0 = value;
  } else if (SB_STRCMP(name, "m_umask5") == 0) {
    s->m_uMask5 = value;
  } else if (SB_STRCMP(name, "m_umask4") == 0) {
    s->m_uMask4 = value;
  } else if (SB_STRCMP(name, "m_umask3") == 0) {
    s->m_uMask3 = value;
  } else if (SB_STRCMP(name, "m_umask2") == 0) {
    s->m_uMask2 = value;
  } else if (SB_STRCMP(name, "m_umask1") == 0) {
    s->m_uMask1 = value;
  } else if (SB_STRCMP(name, "m_umask0") == 0) {
    s->m_uMask0 = value;
  } else if (SB_STRCMP(name, "m_uspare1") == 0) {
    s->m_uSpare1 = value;
  } else if (SB_STRCMP(name, "m_uvalid") == 0) {
    s->m_uValid = value;
  } else if (SB_STRCMP(name, "m_udata5") == 0) {
    s->m_uData5 = value;
  } else if (SB_STRCMP(name, "m_udata4") == 0) {
    s->m_uData4 = value;
  } else if (SB_STRCMP(name, "m_udata3") == 0) {
    s->m_uData3 = value;
  } else if (SB_STRCMP(name, "m_udata2") == 0) {
    s->m_uData2 = value;
  } else if (SB_STRCMP(name, "m_udata1") == 0) {
    s->m_uData1 = value;
  } else if (SB_STRCMP(name, "m_udata0") == 0) {
    s->m_uData0 = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
