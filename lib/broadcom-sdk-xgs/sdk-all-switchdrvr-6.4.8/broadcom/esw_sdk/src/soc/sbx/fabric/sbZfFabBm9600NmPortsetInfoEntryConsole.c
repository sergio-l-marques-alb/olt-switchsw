/*
 * $Id: $
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
#include "sbZfFabBm9600NmPortsetInfoEntryConsole.hx"



/* Print members in struct */
void
sbZfFabBm9600NmPortsetInfoEntry_Print(sbZfFabBm9600NmPortsetInfoEntry_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("FabBm9600NmPortsetInfoEntry:: virtualport=0x%01x"), (unsigned int)  pFromStruct->m_uVirtualPort));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" vporteopp=0x%01x"), (unsigned int)  pFromStruct->m_uVportEopp));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" startport=0x%02x"), (unsigned int)  pFromStruct->m_uStartPort));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("FabBm9600NmPortsetInfoEntry:: egnode=0x%02x"), (unsigned int)  pFromStruct->m_uEgNode));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfFabBm9600NmPortsetInfoEntry_SPrint(sbZfFabBm9600NmPortsetInfoEntry_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"FabBm9600NmPortsetInfoEntry:: virtualport=0x%01x", (unsigned int)  pFromStruct->m_uVirtualPort);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," vporteopp=0x%01x", (unsigned int)  pFromStruct->m_uVportEopp);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," startport=0x%02x", (unsigned int)  pFromStruct->m_uStartPort);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"FabBm9600NmPortsetInfoEntry:: egnode=0x%02x", (unsigned int)  pFromStruct->m_uEgNode);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfFabBm9600NmPortsetInfoEntry_Validate(sbZfFabBm9600NmPortsetInfoEntry_t *pZf) {

  if (pZf->m_uVirtualPort > 0x1) return 0;
  if (pZf->m_uVportEopp > 0x7) return 0;
  if (pZf->m_uStartPort > 0xff) return 0;
  if (pZf->m_uEgNode > 0x7f) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfFabBm9600NmPortsetInfoEntry_SetField(sbZfFabBm9600NmPortsetInfoEntry_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_uvirtualport") == 0) {
    s->m_uVirtualPort = value;
  } else if (SB_STRCMP(name, "m_uvporteopp") == 0) {
    s->m_uVportEopp = value;
  } else if (SB_STRCMP(name, "m_ustartport") == 0) {
    s->m_uStartPort = value;
  } else if (SB_STRCMP(name, "m_uegnode") == 0) {
    s->m_uEgNode = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
