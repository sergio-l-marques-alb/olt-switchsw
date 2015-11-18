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

#include "sbTypes.h"
#include <soc/sbx/sbWrappers.h>
#include "sbZfC2PrCcTcamConfigEntryNonFirstConsole.hx"



/* Print members in struct */
void
sbZfC2PrCcTcamConfigEntryNonFirst_Print(sbZfC2PrCcTcamConfigEntryNonFirst_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: mask0_31=0x%08x"), (unsigned int)  pFromStruct->m_uMask0_31));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask32_63=0x%08x"), (unsigned int)  pFromStruct->m_uMask32_63));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: mask64_95=0x%08x"), (unsigned int)  pFromStruct->m_uMask64_95));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask96_127=0x%08x"), (unsigned int)  pFromStruct->m_uMask96_127));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: mask128_159=0x%08x"), (unsigned int)  pFromStruct->m_uMask128_159));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: mask160_191=0x%08x"), (unsigned int)  pFromStruct->m_uMask160_191));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" mask192_199=0x%02x"), (unsigned int)  pFromStruct->m_uMask192_199));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: maskstate=0x%04x"), (unsigned int)  pFromStruct->m_uMaskState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" packet0_31=0x%08x"), (unsigned int)  pFromStruct->m_uPacket0_31));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: packet32_63=0x%08x"), (unsigned int)  pFromStruct->m_uPacket32_63));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: packet64_95=0x%08x"), (unsigned int)  pFromStruct->m_uPacket64_95));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: packet96_127=0x%08x"), (unsigned int)  pFromStruct->m_uPacket96_127));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: packet128_159=0x%08x"), (unsigned int)  pFromStruct->m_uPacket128_159));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: packet160_191=0x%08x"), (unsigned int)  pFromStruct->m_uPacket160_191));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" packet192_199=0x%02x"), (unsigned int)  pFromStruct->m_uPacket192_199));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("C2PrCcTcamConfigEntryNonFirst:: state=0x%04x"), (unsigned int)  pFromStruct->m_uState));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" valid=0x%01x"), (unsigned int)  pFromStruct->m_uValid));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfC2PrCcTcamConfigEntryNonFirst_SPrint(sbZfC2PrCcTcamConfigEntryNonFirst_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: mask0_31=0x%08x", (unsigned int)  pFromStruct->m_uMask0_31);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask32_63=0x%08x", (unsigned int)  pFromStruct->m_uMask32_63);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: mask64_95=0x%08x", (unsigned int)  pFromStruct->m_uMask64_95);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask96_127=0x%08x", (unsigned int)  pFromStruct->m_uMask96_127);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: mask128_159=0x%08x", (unsigned int)  pFromStruct->m_uMask128_159);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: mask160_191=0x%08x", (unsigned int)  pFromStruct->m_uMask160_191);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," mask192_199=0x%02x", (unsigned int)  pFromStruct->m_uMask192_199);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: maskstate=0x%04x", (unsigned int)  pFromStruct->m_uMaskState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," packet0_31=0x%08x", (unsigned int)  pFromStruct->m_uPacket0_31);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: packet32_63=0x%08x", (unsigned int)  pFromStruct->m_uPacket32_63);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: packet64_95=0x%08x", (unsigned int)  pFromStruct->m_uPacket64_95);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: packet96_127=0x%08x", (unsigned int)  pFromStruct->m_uPacket96_127);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: packet128_159=0x%08x", (unsigned int)  pFromStruct->m_uPacket128_159);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: packet160_191=0x%08x", (unsigned int)  pFromStruct->m_uPacket160_191);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," packet192_199=0x%02x", (unsigned int)  pFromStruct->m_uPacket192_199);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"C2PrCcTcamConfigEntryNonFirst:: state=0x%04x", (unsigned int)  pFromStruct->m_uState);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," valid=0x%01x", (unsigned int)  pFromStruct->m_uValid);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfC2PrCcTcamConfigEntryNonFirst_Validate(sbZfC2PrCcTcamConfigEntryNonFirst_t *pZf) {

  /* pZf->m_uMask0_31 implicitly masked by data type */
  /* pZf->m_uMask32_63 implicitly masked by data type */
  /* pZf->m_uMask64_95 implicitly masked by data type */
  /* pZf->m_uMask96_127 implicitly masked by data type */
  /* pZf->m_uMask128_159 implicitly masked by data type */
  /* pZf->m_uMask160_191 implicitly masked by data type */
  if (pZf->m_uMask192_199 > 0xff) return 0;
  if (pZf->m_uMaskState > 0x3fff) return 0;
  /* pZf->m_uPacket0_31 implicitly masked by data type */
  /* pZf->m_uPacket32_63 implicitly masked by data type */
  /* pZf->m_uPacket64_95 implicitly masked by data type */
  /* pZf->m_uPacket96_127 implicitly masked by data type */
  /* pZf->m_uPacket128_159 implicitly masked by data type */
  /* pZf->m_uPacket160_191 implicitly masked by data type */
  if (pZf->m_uPacket192_199 > 0xff) return 0;
  if (pZf->m_uState > 0x3fff) return 0;
  if (pZf->m_uValid > 0x3) return 0;

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfC2PrCcTcamConfigEntryNonFirst_SetField(sbZfC2PrCcTcamConfigEntryNonFirst_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_umask0_31") == 0) {
    s->m_uMask0_31 = value;
  } else if (SB_STRCMP(name, "m_umask32_63") == 0) {
    s->m_uMask32_63 = value;
  } else if (SB_STRCMP(name, "m_umask64_95") == 0) {
    s->m_uMask64_95 = value;
  } else if (SB_STRCMP(name, "m_umask96_127") == 0) {
    s->m_uMask96_127 = value;
  } else if (SB_STRCMP(name, "m_umask128_159") == 0) {
    s->m_uMask128_159 = value;
  } else if (SB_STRCMP(name, "m_umask160_191") == 0) {
    s->m_uMask160_191 = value;
  } else if (SB_STRCMP(name, "m_umask192_199") == 0) {
    s->m_uMask192_199 = value;
  } else if (SB_STRCMP(name, "m_umaskstate") == 0) {
    s->m_uMaskState = value;
  } else if (SB_STRCMP(name, "m_upacket0_31") == 0) {
    s->m_uPacket0_31 = value;
  } else if (SB_STRCMP(name, "m_upacket32_63") == 0) {
    s->m_uPacket32_63 = value;
  } else if (SB_STRCMP(name, "m_upacket64_95") == 0) {
    s->m_uPacket64_95 = value;
  } else if (SB_STRCMP(name, "m_upacket96_127") == 0) {
    s->m_uPacket96_127 = value;
  } else if (SB_STRCMP(name, "m_upacket128_159") == 0) {
    s->m_uPacket128_159 = value;
  } else if (SB_STRCMP(name, "m_upacket160_191") == 0) {
    s->m_uPacket160_191 = value;
  } else if (SB_STRCMP(name, "m_upacket192_199") == 0) {
    s->m_uPacket192_199 = value;
  } else if (SB_STRCMP(name, "m_ustate") == 0) {
    s->m_uState = value;
  } else if (SB_STRCMP(name, "m_uvalid") == 0) {
    s->m_uValid = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
