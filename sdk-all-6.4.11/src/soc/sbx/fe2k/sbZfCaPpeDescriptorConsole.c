/*
 * $Id: sbZfCaPpeDescriptorConsole.c,v 1.3 Broadcom SDK $
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
#include "sbZfCaPpeDescriptorConsole.hx"



/* Print members in struct */
void
sbZfCaPpeDescriptor_Print(sbZfCaPpeDescriptor_t *pFromStruct) {
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: contbyte=0x%02x"), (unsigned int)  pFromStruct->m_uContinueByte));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr0=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr1=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr2=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr3=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: hdr4=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr5=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr6=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr7=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr8=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" hdr9=0x%01x"), (unsigned int)  pFromStruct->m_uHeaderType9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: locn0=0x%02x"), (unsigned int)  pFromStruct->m_uLocation0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn1=0x%02x"), (unsigned int)  pFromStruct->m_uLocation1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn2=0x%02x"), (unsigned int)  pFromStruct->m_uLocation2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn3=0x%02x"), (unsigned int)  pFromStruct->m_uLocation3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn4=0x%02x"), (unsigned int)  pFromStruct->m_uLocation4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: locn5=0x%02x"), (unsigned int)  pFromStruct->m_uLocation5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn6=0x%02x"), (unsigned int)  pFromStruct->m_uLocation6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn7=0x%02x"), (unsigned int)  pFromStruct->m_uLocation7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn8=0x%02x"), (unsigned int)  pFromStruct->m_uLocation8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" locn9=0x%02x"), (unsigned int)  pFromStruct->m_uLocation9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: str=0x%01x"), (unsigned int)  pFromStruct->m_uStreamSelector));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" buffer=0x%01x"), (unsigned int)  pFromStruct->m_uBuffer));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" expidx=0x%02x"), (unsigned int)  pFromStruct->m_uExceptIndex));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" sque=0x%02x"), (unsigned int)  pFromStruct->m_uSourceQueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" rxport0=0x%02x"), (unsigned int)  pFromStruct->m_uRxPortData0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: repcnt=0x%03x"), (unsigned int)  pFromStruct->m_uReplicantCnt));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" repbit=0x%01x"), (unsigned int)  pFromStruct->m_bReplicantBit));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" resv0=0x%01x"), (unsigned int)  pFromStruct->m_uResv0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" drop=0x%01x"), (unsigned int)  pFromStruct->m_uDrop));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" enq=0x%01x"), (unsigned int)  pFromStruct->m_uEnqueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: dqueue=0x%02x"), (unsigned int)  pFromStruct->m_uDqueue));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" rxport1=0x%05x"), (unsigned int)  pFromStruct->m_uRxPortData1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" frmlen=0x%04x"), (unsigned int)  pFromStruct->m_uFrameLength));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: aggrhash=0x%08x"), (unsigned int)  pFromStruct->m_uAggrHash));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" variable=0x%08x"), (unsigned int)  pFromStruct->m_uVariable));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata0=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte0));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata1=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte1));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata2=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte2));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata3=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte3));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata4=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte4));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata5=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte5));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata6=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte6));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata7=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte7));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata8=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte8));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata9=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte9));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata10=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte10));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata11=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte11));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata12=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte12));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata13=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte13));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata14=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte14));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata15=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte15));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata16=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte16));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata17=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte17));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata18=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte18));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata19=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte19));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata20=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte20));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata21=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte21));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata22=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte22));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata23=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte23));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata24=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte24));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata25=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte25));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata26=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte26));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata27=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte27));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata28=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte28));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata29=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte29));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata30=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte30));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata31=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte31));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata32=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte32));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata33=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte33));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata34=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte34));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata35=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte35));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata36=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte36));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata37=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte37));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata38=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte38));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata39=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte39));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata40=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte40));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata41=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte41));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata42=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte42));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata43=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte43));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata44=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte44));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata45=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte45));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata46=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte46));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata47=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte47));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata48=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte48));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata49=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte49));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata50=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte50));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata51=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte51));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata52=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte52));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata53=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte53));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata54=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte54));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata55=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte55));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata56=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte56));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata57=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte57));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata58=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte58));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata59=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte59));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata60=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte60));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata61=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte61));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata62=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte62));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata63=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte63));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata64=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte64));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata65=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte65));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata66=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte66));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata67=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte67));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata68=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte68));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata69=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte69));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata70=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte70));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata71=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte71));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata72=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte72));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata73=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte73));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata74=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte74));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata75=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte75));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata76=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte76));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata77=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte77));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata78=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte78));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata79=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte79));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata80=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte80));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata81=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte81));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata82=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte82));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata83=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte83));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata84=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte84));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata85=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte85));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata86=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte86));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata87=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte87));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata88=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte88));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata89=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte89));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata90=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte90));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata91=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte91));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata92=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte92));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata93=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte93));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata94=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte94));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata95=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte95));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata96=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte96));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata97=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte97));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata98=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte98));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata99=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte99));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata100=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte100));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata101=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte101));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata102=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte102));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata103=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte103));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata104=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte104));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata105=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte105));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata106=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte106));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata107=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte107));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata108=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte108));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata109=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte109));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata110=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte110));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata111=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte111));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata112=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte112));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata113=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte113));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata114=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte114));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata115=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte115));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata116=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte116));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata117=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte117));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata118=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte118));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata119=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte119));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata120=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte120));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata121=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte121));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata122=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte122));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata123=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte123));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata124=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte124));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata125=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte125));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata126=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte126));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata127=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte127));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata128=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte128));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata129=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte129));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata130=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte130));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata131=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte131));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata132=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte132));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata133=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte133));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata134=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte134));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata135=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte135));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata136=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte136));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata137=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte137));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata138=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte138));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata139=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte139));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata140=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte140));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata141=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte141));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata142=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte142));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata143=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte143));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata144=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte144));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata145=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte145));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata146=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte146));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata147=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte147));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata148=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte148));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata149=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte149));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata150=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte150));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata151=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte151));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata152=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte152));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("CaPpeDescriptor:: pdata153=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte153));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata154=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte154));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META(" pdata155=0x%02x"), (unsigned int)  pFromStruct->m_uPacketDataByte155));
  LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));

}

/* SPrint members in struct */
char *
sbZfCaPpeDescriptor_SPrint(sbZfCaPpeDescriptor_t *pFromStruct, char *pcToString, uint32 lStrSize) {
  uint32 WrCnt = 0x0;

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: contbyte=0x%02x", (unsigned int)  pFromStruct->m_uContinueByte);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr0=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr1=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr2=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr3=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: hdr4=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr5=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr6=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr7=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr8=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," hdr9=0x%01x", (unsigned int)  pFromStruct->m_uHeaderType9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: locn0=0x%02x", (unsigned int)  pFromStruct->m_uLocation0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn1=0x%02x", (unsigned int)  pFromStruct->m_uLocation1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn2=0x%02x", (unsigned int)  pFromStruct->m_uLocation2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn3=0x%02x", (unsigned int)  pFromStruct->m_uLocation3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn4=0x%02x", (unsigned int)  pFromStruct->m_uLocation4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: locn5=0x%02x", (unsigned int)  pFromStruct->m_uLocation5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn6=0x%02x", (unsigned int)  pFromStruct->m_uLocation6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn7=0x%02x", (unsigned int)  pFromStruct->m_uLocation7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn8=0x%02x", (unsigned int)  pFromStruct->m_uLocation8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," locn9=0x%02x", (unsigned int)  pFromStruct->m_uLocation9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: str=0x%01x", (unsigned int)  pFromStruct->m_uStreamSelector);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," buffer=0x%01x", (unsigned int)  pFromStruct->m_uBuffer);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," expidx=0x%02x", (unsigned int)  pFromStruct->m_uExceptIndex);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," sque=0x%02x", (unsigned int)  pFromStruct->m_uSourceQueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," rxport0=0x%02x", (unsigned int)  pFromStruct->m_uRxPortData0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: repcnt=0x%03x", (unsigned int)  pFromStruct->m_uReplicantCnt);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," repbit=0x%01x", (unsigned int)  pFromStruct->m_bReplicantBit);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," resv0=0x%01x", (unsigned int)  pFromStruct->m_uResv0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," drop=0x%01x", (unsigned int)  pFromStruct->m_uDrop);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," enq=0x%01x", (unsigned int)  pFromStruct->m_uEnqueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: dqueue=0x%02x", (unsigned int)  pFromStruct->m_uDqueue);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," rxport1=0x%05x", (unsigned int)  pFromStruct->m_uRxPortData1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," frmlen=0x%04x", (unsigned int)  pFromStruct->m_uFrameLength);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: aggrhash=0x%08x", (unsigned int)  pFromStruct->m_uAggrHash);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," variable=0x%08x", (unsigned int)  pFromStruct->m_uVariable);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata0=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte0);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata1=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte1);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata2=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte2);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata3=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte3);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata4=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte4);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata5=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte5);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata6=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte6);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata7=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte7);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata8=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte8);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata9=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte9);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata10=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte10);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata11=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte11);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata12=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte12);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata13=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte13);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata14=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte14);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata15=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte15);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata16=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte16);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata17=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte17);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata18=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte18);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata19=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte19);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata20=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte20);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata21=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte21);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata22=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte22);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata23=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte23);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata24=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte24);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata25=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte25);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata26=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte26);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata27=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte27);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata28=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte28);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata29=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte29);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata30=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte30);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata31=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte31);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata32=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte32);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata33=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte33);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata34=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte34);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata35=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte35);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata36=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte36);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata37=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte37);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata38=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte38);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata39=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte39);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata40=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte40);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata41=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte41);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata42=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte42);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata43=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte43);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata44=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte44);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata45=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte45);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata46=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte46);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata47=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte47);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata48=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte48);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata49=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte49);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata50=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte50);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata51=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte51);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata52=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte52);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata53=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte53);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata54=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte54);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata55=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte55);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata56=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte56);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata57=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte57);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata58=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte58);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata59=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte59);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata60=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte60);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata61=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte61);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata62=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte62);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata63=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte63);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata64=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte64);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata65=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte65);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata66=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte66);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata67=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte67);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata68=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte68);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata69=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte69);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata70=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte70);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata71=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte71);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata72=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte72);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata73=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte73);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata74=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte74);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata75=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte75);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata76=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte76);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata77=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte77);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata78=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte78);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata79=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte79);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata80=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte80);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata81=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte81);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata82=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte82);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata83=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte83);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata84=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte84);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata85=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte85);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata86=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte86);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata87=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte87);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata88=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte88);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata89=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte89);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata90=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte90);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata91=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte91);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata92=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte92);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata93=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte93);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata94=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte94);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata95=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte95);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata96=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte96);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata97=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte97);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata98=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte98);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata99=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte99);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata100=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte100);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata101=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte101);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata102=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte102);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata103=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte103);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata104=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte104);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata105=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte105);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata106=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte106);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata107=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte107);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata108=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte108);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata109=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte109);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata110=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte110);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata111=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte111);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata112=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte112);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata113=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte113);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata114=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte114);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata115=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte115);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata116=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte116);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata117=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte117);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata118=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte118);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata119=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte119);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata120=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte120);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata121=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte121);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata122=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte122);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata123=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte123);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata124=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte124);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata125=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte125);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata126=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte126);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata127=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte127);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata128=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte128);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata129=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte129);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata130=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte130);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata131=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte131);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata132=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte132);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata133=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte133);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata134=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte134);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata135=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte135);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata136=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte136);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata137=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte137);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata138=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte138);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata139=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte139);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata140=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte140);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata141=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte141);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata142=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte142);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata143=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte143);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata144=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte144);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata145=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte145);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata146=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte146);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata147=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte147);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata148=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte148);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata149=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte149);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata150=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte150);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata151=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte151);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata152=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte152);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"CaPpeDescriptor:: pdata153=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte153);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata154=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte154);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt]," pdata155=0x%02x", (unsigned int)  pFromStruct->m_uPacketDataByte155);
  WrCnt += SB_SPRINTF(&pcToString[WrCnt],"\n");

  /* assert if we've overrun the buffer */
  SB_ASSERT(WrCnt < lStrSize);
  return(pcToString);

}

/* validate members in struct (1 = PASS, 0 = FAIL) */
int
sbZfCaPpeDescriptor_Validate(sbZfCaPpeDescriptor_t *pZf) {

  if (pZf->m_uContinueByte > 0xff) return 0;
  if (pZf->m_uHeaderType0 > 0xf) return 0;
  if (pZf->m_uHeaderType1 > 0xf) return 0;
  if (pZf->m_uHeaderType2 > 0xf) return 0;
  if (pZf->m_uHeaderType3 > 0xf) return 0;
  if (pZf->m_uHeaderType4 > 0xf) return 0;
  if (pZf->m_uHeaderType5 > 0xf) return 0;
  if (pZf->m_uHeaderType6 > 0xf) return 0;
  if (pZf->m_uHeaderType7 > 0xf) return 0;
  if (pZf->m_uHeaderType8 > 0xf) return 0;
  if (pZf->m_uHeaderType9 > 0xf) return 0;
  if (pZf->m_uLocation0 > 0xff) return 0;
  if (pZf->m_uLocation1 > 0xff) return 0;
  if (pZf->m_uLocation2 > 0xff) return 0;
  if (pZf->m_uLocation3 > 0xff) return 0;
  if (pZf->m_uLocation4 > 0xff) return 0;
  if (pZf->m_uLocation5 > 0xff) return 0;
  if (pZf->m_uLocation6 > 0xff) return 0;
  if (pZf->m_uLocation7 > 0xff) return 0;
  if (pZf->m_uLocation8 > 0xff) return 0;
  if (pZf->m_uLocation9 > 0xff) return 0;
  if (pZf->m_uStreamSelector > 0x7) return 0;
  if (pZf->m_uBuffer > 0x3) return 0;
  if (pZf->m_uExceptIndex > 0xff) return 0;
  if (pZf->m_uSourceQueue > 0xff) return 0;
  if (pZf->m_uRxPortData0 > 0xff) return 0;
  if (pZf->m_uReplicantCnt > 0xfff) return 0;
  if (pZf->m_bReplicantBit > 0x1) return 0;
  if (pZf->m_uResv0 > 0x1) return 0;
  if (pZf->m_uDrop > 0x1) return 0;
  if (pZf->m_uEnqueue > 0x1) return 0;
  if (pZf->m_uDqueue > 0xff) return 0;
  if (pZf->m_uRxPortData1 > 0x3ffff) return 0;
  if (pZf->m_uFrameLength > 0x3fff) return 0;
  /* pZf->m_uAggrHash implicitly masked by data type */
  /* pZf->m_uVariable implicitly masked by data type */
  /* pZf->m_uPacketDataByte0 implicitly masked by data type */
  /* pZf->m_uPacketDataByte1 implicitly masked by data type */
  /* pZf->m_uPacketDataByte2 implicitly masked by data type */
  /* pZf->m_uPacketDataByte3 implicitly masked by data type */
  /* pZf->m_uPacketDataByte4 implicitly masked by data type */
  /* pZf->m_uPacketDataByte5 implicitly masked by data type */
  /* pZf->m_uPacketDataByte6 implicitly masked by data type */
  /* pZf->m_uPacketDataByte7 implicitly masked by data type */
  /* pZf->m_uPacketDataByte8 implicitly masked by data type */
  /* pZf->m_uPacketDataByte9 implicitly masked by data type */
  /* pZf->m_uPacketDataByte10 implicitly masked by data type */
  /* pZf->m_uPacketDataByte11 implicitly masked by data type */
  /* pZf->m_uPacketDataByte12 implicitly masked by data type */
  /* pZf->m_uPacketDataByte13 implicitly masked by data type */
  /* pZf->m_uPacketDataByte14 implicitly masked by data type */
  /* pZf->m_uPacketDataByte15 implicitly masked by data type */
  /* pZf->m_uPacketDataByte16 implicitly masked by data type */
  /* pZf->m_uPacketDataByte17 implicitly masked by data type */
  /* pZf->m_uPacketDataByte18 implicitly masked by data type */
  /* pZf->m_uPacketDataByte19 implicitly masked by data type */
  /* pZf->m_uPacketDataByte20 implicitly masked by data type */
  /* pZf->m_uPacketDataByte21 implicitly masked by data type */
  /* pZf->m_uPacketDataByte22 implicitly masked by data type */
  /* pZf->m_uPacketDataByte23 implicitly masked by data type */
  /* pZf->m_uPacketDataByte24 implicitly masked by data type */
  /* pZf->m_uPacketDataByte25 implicitly masked by data type */
  /* pZf->m_uPacketDataByte26 implicitly masked by data type */
  /* pZf->m_uPacketDataByte27 implicitly masked by data type */
  /* pZf->m_uPacketDataByte28 implicitly masked by data type */
  /* pZf->m_uPacketDataByte29 implicitly masked by data type */
  /* pZf->m_uPacketDataByte30 implicitly masked by data type */
  /* pZf->m_uPacketDataByte31 implicitly masked by data type */
  /* pZf->m_uPacketDataByte32 implicitly masked by data type */
  /* pZf->m_uPacketDataByte33 implicitly masked by data type */
  /* pZf->m_uPacketDataByte34 implicitly masked by data type */
  /* pZf->m_uPacketDataByte35 implicitly masked by data type */
  /* pZf->m_uPacketDataByte36 implicitly masked by data type */
  /* pZf->m_uPacketDataByte37 implicitly masked by data type */
  /* pZf->m_uPacketDataByte38 implicitly masked by data type */
  /* pZf->m_uPacketDataByte39 implicitly masked by data type */
  /* pZf->m_uPacketDataByte40 implicitly masked by data type */
  /* pZf->m_uPacketDataByte41 implicitly masked by data type */
  /* pZf->m_uPacketDataByte42 implicitly masked by data type */
  /* pZf->m_uPacketDataByte43 implicitly masked by data type */
  /* pZf->m_uPacketDataByte44 implicitly masked by data type */
  /* pZf->m_uPacketDataByte45 implicitly masked by data type */
  /* pZf->m_uPacketDataByte46 implicitly masked by data type */
  /* pZf->m_uPacketDataByte47 implicitly masked by data type */
  /* pZf->m_uPacketDataByte48 implicitly masked by data type */
  /* pZf->m_uPacketDataByte49 implicitly masked by data type */
  /* pZf->m_uPacketDataByte50 implicitly masked by data type */
  /* pZf->m_uPacketDataByte51 implicitly masked by data type */
  /* pZf->m_uPacketDataByte52 implicitly masked by data type */
  /* pZf->m_uPacketDataByte53 implicitly masked by data type */
  /* pZf->m_uPacketDataByte54 implicitly masked by data type */
  /* pZf->m_uPacketDataByte55 implicitly masked by data type */
  /* pZf->m_uPacketDataByte56 implicitly masked by data type */
  /* pZf->m_uPacketDataByte57 implicitly masked by data type */
  /* pZf->m_uPacketDataByte58 implicitly masked by data type */
  /* pZf->m_uPacketDataByte59 implicitly masked by data type */
  /* pZf->m_uPacketDataByte60 implicitly masked by data type */
  /* pZf->m_uPacketDataByte61 implicitly masked by data type */
  /* pZf->m_uPacketDataByte62 implicitly masked by data type */
  /* pZf->m_uPacketDataByte63 implicitly masked by data type */
  /* pZf->m_uPacketDataByte64 implicitly masked by data type */
  /* pZf->m_uPacketDataByte65 implicitly masked by data type */
  /* pZf->m_uPacketDataByte66 implicitly masked by data type */
  /* pZf->m_uPacketDataByte67 implicitly masked by data type */
  /* pZf->m_uPacketDataByte68 implicitly masked by data type */
  /* pZf->m_uPacketDataByte69 implicitly masked by data type */
  /* pZf->m_uPacketDataByte70 implicitly masked by data type */
  /* pZf->m_uPacketDataByte71 implicitly masked by data type */
  /* pZf->m_uPacketDataByte72 implicitly masked by data type */
  /* pZf->m_uPacketDataByte73 implicitly masked by data type */
  /* pZf->m_uPacketDataByte74 implicitly masked by data type */
  /* pZf->m_uPacketDataByte75 implicitly masked by data type */
  /* pZf->m_uPacketDataByte76 implicitly masked by data type */
  /* pZf->m_uPacketDataByte77 implicitly masked by data type */
  /* pZf->m_uPacketDataByte78 implicitly masked by data type */
  /* pZf->m_uPacketDataByte79 implicitly masked by data type */
  /* pZf->m_uPacketDataByte80 implicitly masked by data type */
  /* pZf->m_uPacketDataByte81 implicitly masked by data type */
  /* pZf->m_uPacketDataByte82 implicitly masked by data type */
  /* pZf->m_uPacketDataByte83 implicitly masked by data type */
  /* pZf->m_uPacketDataByte84 implicitly masked by data type */
  /* pZf->m_uPacketDataByte85 implicitly masked by data type */
  /* pZf->m_uPacketDataByte86 implicitly masked by data type */
  /* pZf->m_uPacketDataByte87 implicitly masked by data type */
  /* pZf->m_uPacketDataByte88 implicitly masked by data type */
  /* pZf->m_uPacketDataByte89 implicitly masked by data type */
  /* pZf->m_uPacketDataByte90 implicitly masked by data type */
  /* pZf->m_uPacketDataByte91 implicitly masked by data type */
  /* pZf->m_uPacketDataByte92 implicitly masked by data type */
  /* pZf->m_uPacketDataByte93 implicitly masked by data type */
  /* pZf->m_uPacketDataByte94 implicitly masked by data type */
  /* pZf->m_uPacketDataByte95 implicitly masked by data type */
  /* pZf->m_uPacketDataByte96 implicitly masked by data type */
  /* pZf->m_uPacketDataByte97 implicitly masked by data type */
  /* pZf->m_uPacketDataByte98 implicitly masked by data type */
  /* pZf->m_uPacketDataByte99 implicitly masked by data type */
  /* pZf->m_uPacketDataByte100 implicitly masked by data type */
  /* pZf->m_uPacketDataByte101 implicitly masked by data type */
  /* pZf->m_uPacketDataByte102 implicitly masked by data type */
  /* pZf->m_uPacketDataByte103 implicitly masked by data type */
  /* pZf->m_uPacketDataByte104 implicitly masked by data type */
  /* pZf->m_uPacketDataByte105 implicitly masked by data type */
  /* pZf->m_uPacketDataByte106 implicitly masked by data type */
  /* pZf->m_uPacketDataByte107 implicitly masked by data type */
  /* pZf->m_uPacketDataByte108 implicitly masked by data type */
  /* pZf->m_uPacketDataByte109 implicitly masked by data type */
  /* pZf->m_uPacketDataByte110 implicitly masked by data type */
  /* pZf->m_uPacketDataByte111 implicitly masked by data type */
  /* pZf->m_uPacketDataByte112 implicitly masked by data type */
  /* pZf->m_uPacketDataByte113 implicitly masked by data type */
  /* pZf->m_uPacketDataByte114 implicitly masked by data type */
  /* pZf->m_uPacketDataByte115 implicitly masked by data type */
  /* pZf->m_uPacketDataByte116 implicitly masked by data type */
  /* pZf->m_uPacketDataByte117 implicitly masked by data type */
  /* pZf->m_uPacketDataByte118 implicitly masked by data type */
  /* pZf->m_uPacketDataByte119 implicitly masked by data type */
  /* pZf->m_uPacketDataByte120 implicitly masked by data type */
  /* pZf->m_uPacketDataByte121 implicitly masked by data type */
  /* pZf->m_uPacketDataByte122 implicitly masked by data type */
  /* pZf->m_uPacketDataByte123 implicitly masked by data type */
  /* pZf->m_uPacketDataByte124 implicitly masked by data type */
  /* pZf->m_uPacketDataByte125 implicitly masked by data type */
  /* pZf->m_uPacketDataByte126 implicitly masked by data type */
  /* pZf->m_uPacketDataByte127 implicitly masked by data type */
  /* pZf->m_uPacketDataByte128 implicitly masked by data type */
  /* pZf->m_uPacketDataByte129 implicitly masked by data type */
  /* pZf->m_uPacketDataByte130 implicitly masked by data type */
  /* pZf->m_uPacketDataByte131 implicitly masked by data type */
  /* pZf->m_uPacketDataByte132 implicitly masked by data type */
  /* pZf->m_uPacketDataByte133 implicitly masked by data type */
  /* pZf->m_uPacketDataByte134 implicitly masked by data type */
  /* pZf->m_uPacketDataByte135 implicitly masked by data type */
  /* pZf->m_uPacketDataByte136 implicitly masked by data type */
  /* pZf->m_uPacketDataByte137 implicitly masked by data type */
  /* pZf->m_uPacketDataByte138 implicitly masked by data type */
  /* pZf->m_uPacketDataByte139 implicitly masked by data type */
  /* pZf->m_uPacketDataByte140 implicitly masked by data type */
  /* pZf->m_uPacketDataByte141 implicitly masked by data type */
  /* pZf->m_uPacketDataByte142 implicitly masked by data type */
  /* pZf->m_uPacketDataByte143 implicitly masked by data type */
  /* pZf->m_uPacketDataByte144 implicitly masked by data type */
  /* pZf->m_uPacketDataByte145 implicitly masked by data type */
  /* pZf->m_uPacketDataByte146 implicitly masked by data type */
  /* pZf->m_uPacketDataByte147 implicitly masked by data type */
  /* pZf->m_uPacketDataByte148 implicitly masked by data type */
  /* pZf->m_uPacketDataByte149 implicitly masked by data type */
  /* pZf->m_uPacketDataByte150 implicitly masked by data type */
  /* pZf->m_uPacketDataByte151 implicitly masked by data type */
  /* pZf->m_uPacketDataByte152 implicitly masked by data type */
  /* pZf->m_uPacketDataByte153 implicitly masked by data type */
  /* pZf->m_uPacketDataByte154 implicitly masked by data type */
  /* pZf->m_uPacketDataByte155 implicitly masked by data type */

  return 1; /* success */

}

/* populate field from string and value */
int
sbZfCaPpeDescriptor_SetField(sbZfCaPpeDescriptor_t *s, char* name, int value) {

  if (SB_STRCMP(name, "") == 0 ) {
    return -1;
  } else if (SB_STRCMP(name, "m_ucontinuebyte") == 0) {
    s->m_uContinueByte = value;
  } else if (SB_STRCMP(name, "m_uheadertype0") == 0) {
    s->m_uHeaderType0 = value;
  } else if (SB_STRCMP(name, "m_uheadertype1") == 0) {
    s->m_uHeaderType1 = value;
  } else if (SB_STRCMP(name, "m_uheadertype2") == 0) {
    s->m_uHeaderType2 = value;
  } else if (SB_STRCMP(name, "m_uheadertype3") == 0) {
    s->m_uHeaderType3 = value;
  } else if (SB_STRCMP(name, "m_uheadertype4") == 0) {
    s->m_uHeaderType4 = value;
  } else if (SB_STRCMP(name, "m_uheadertype5") == 0) {
    s->m_uHeaderType5 = value;
  } else if (SB_STRCMP(name, "m_uheadertype6") == 0) {
    s->m_uHeaderType6 = value;
  } else if (SB_STRCMP(name, "m_uheadertype7") == 0) {
    s->m_uHeaderType7 = value;
  } else if (SB_STRCMP(name, "m_uheadertype8") == 0) {
    s->m_uHeaderType8 = value;
  } else if (SB_STRCMP(name, "m_uheadertype9") == 0) {
    s->m_uHeaderType9 = value;
  } else if (SB_STRCMP(name, "m_ulocation0") == 0) {
    s->m_uLocation0 = value;
  } else if (SB_STRCMP(name, "m_ulocation1") == 0) {
    s->m_uLocation1 = value;
  } else if (SB_STRCMP(name, "m_ulocation2") == 0) {
    s->m_uLocation2 = value;
  } else if (SB_STRCMP(name, "m_ulocation3") == 0) {
    s->m_uLocation3 = value;
  } else if (SB_STRCMP(name, "m_ulocation4") == 0) {
    s->m_uLocation4 = value;
  } else if (SB_STRCMP(name, "m_ulocation5") == 0) {
    s->m_uLocation5 = value;
  } else if (SB_STRCMP(name, "m_ulocation6") == 0) {
    s->m_uLocation6 = value;
  } else if (SB_STRCMP(name, "m_ulocation7") == 0) {
    s->m_uLocation7 = value;
  } else if (SB_STRCMP(name, "m_ulocation8") == 0) {
    s->m_uLocation8 = value;
  } else if (SB_STRCMP(name, "m_ulocation9") == 0) {
    s->m_uLocation9 = value;
  } else if (SB_STRCMP(name, "m_ustreamselector") == 0) {
    s->m_uStreamSelector = value;
  } else if (SB_STRCMP(name, "m_ubuffer") == 0) {
    s->m_uBuffer = value;
  } else if (SB_STRCMP(name, "m_uexceptindex") == 0) {
    s->m_uExceptIndex = value;
  } else if (SB_STRCMP(name, "m_usourcequeue") == 0) {
    s->m_uSourceQueue = value;
  } else if (SB_STRCMP(name, "m_urxportdata0") == 0) {
    s->m_uRxPortData0 = value;
  } else if (SB_STRCMP(name, "m_ureplicantcnt") == 0) {
    s->m_uReplicantCnt = value;
  } else if (SB_STRCMP(name, "replicantbit") == 0) {
    s->m_bReplicantBit = value;
  } else if (SB_STRCMP(name, "m_uresv0") == 0) {
    s->m_uResv0 = value;
  } else if (SB_STRCMP(name, "m_udrop") == 0) {
    s->m_uDrop = value;
  } else if (SB_STRCMP(name, "m_uenqueue") == 0) {
    s->m_uEnqueue = value;
  } else if (SB_STRCMP(name, "m_udqueue") == 0) {
    s->m_uDqueue = value;
  } else if (SB_STRCMP(name, "m_urxportdata1") == 0) {
    s->m_uRxPortData1 = value;
  } else if (SB_STRCMP(name, "m_uframelength") == 0) {
    s->m_uFrameLength = value;
  } else if (SB_STRCMP(name, "m_uaggrhash") == 0) {
    s->m_uAggrHash = value;
  } else if (SB_STRCMP(name, "m_uvariable") == 0) {
    s->m_uVariable = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte0") == 0) {
    s->m_uPacketDataByte0 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte1") == 0) {
    s->m_uPacketDataByte1 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte2") == 0) {
    s->m_uPacketDataByte2 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte3") == 0) {
    s->m_uPacketDataByte3 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte4") == 0) {
    s->m_uPacketDataByte4 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte5") == 0) {
    s->m_uPacketDataByte5 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte6") == 0) {
    s->m_uPacketDataByte6 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte7") == 0) {
    s->m_uPacketDataByte7 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte8") == 0) {
    s->m_uPacketDataByte8 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte9") == 0) {
    s->m_uPacketDataByte9 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte10") == 0) {
    s->m_uPacketDataByte10 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte11") == 0) {
    s->m_uPacketDataByte11 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte12") == 0) {
    s->m_uPacketDataByte12 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte13") == 0) {
    s->m_uPacketDataByte13 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte14") == 0) {
    s->m_uPacketDataByte14 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte15") == 0) {
    s->m_uPacketDataByte15 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte16") == 0) {
    s->m_uPacketDataByte16 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte17") == 0) {
    s->m_uPacketDataByte17 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte18") == 0) {
    s->m_uPacketDataByte18 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte19") == 0) {
    s->m_uPacketDataByte19 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte20") == 0) {
    s->m_uPacketDataByte20 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte21") == 0) {
    s->m_uPacketDataByte21 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte22") == 0) {
    s->m_uPacketDataByte22 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte23") == 0) {
    s->m_uPacketDataByte23 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte24") == 0) {
    s->m_uPacketDataByte24 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte25") == 0) {
    s->m_uPacketDataByte25 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte26") == 0) {
    s->m_uPacketDataByte26 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte27") == 0) {
    s->m_uPacketDataByte27 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte28") == 0) {
    s->m_uPacketDataByte28 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte29") == 0) {
    s->m_uPacketDataByte29 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte30") == 0) {
    s->m_uPacketDataByte30 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte31") == 0) {
    s->m_uPacketDataByte31 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte32") == 0) {
    s->m_uPacketDataByte32 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte33") == 0) {
    s->m_uPacketDataByte33 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte34") == 0) {
    s->m_uPacketDataByte34 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte35") == 0) {
    s->m_uPacketDataByte35 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte36") == 0) {
    s->m_uPacketDataByte36 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte37") == 0) {
    s->m_uPacketDataByte37 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte38") == 0) {
    s->m_uPacketDataByte38 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte39") == 0) {
    s->m_uPacketDataByte39 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte40") == 0) {
    s->m_uPacketDataByte40 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte41") == 0) {
    s->m_uPacketDataByte41 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte42") == 0) {
    s->m_uPacketDataByte42 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte43") == 0) {
    s->m_uPacketDataByte43 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte44") == 0) {
    s->m_uPacketDataByte44 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte45") == 0) {
    s->m_uPacketDataByte45 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte46") == 0) {
    s->m_uPacketDataByte46 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte47") == 0) {
    s->m_uPacketDataByte47 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte48") == 0) {
    s->m_uPacketDataByte48 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte49") == 0) {
    s->m_uPacketDataByte49 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte50") == 0) {
    s->m_uPacketDataByte50 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte51") == 0) {
    s->m_uPacketDataByte51 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte52") == 0) {
    s->m_uPacketDataByte52 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte53") == 0) {
    s->m_uPacketDataByte53 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte54") == 0) {
    s->m_uPacketDataByte54 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte55") == 0) {
    s->m_uPacketDataByte55 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte56") == 0) {
    s->m_uPacketDataByte56 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte57") == 0) {
    s->m_uPacketDataByte57 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte58") == 0) {
    s->m_uPacketDataByte58 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte59") == 0) {
    s->m_uPacketDataByte59 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte60") == 0) {
    s->m_uPacketDataByte60 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte61") == 0) {
    s->m_uPacketDataByte61 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte62") == 0) {
    s->m_uPacketDataByte62 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte63") == 0) {
    s->m_uPacketDataByte63 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte64") == 0) {
    s->m_uPacketDataByte64 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte65") == 0) {
    s->m_uPacketDataByte65 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte66") == 0) {
    s->m_uPacketDataByte66 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte67") == 0) {
    s->m_uPacketDataByte67 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte68") == 0) {
    s->m_uPacketDataByte68 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte69") == 0) {
    s->m_uPacketDataByte69 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte70") == 0) {
    s->m_uPacketDataByte70 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte71") == 0) {
    s->m_uPacketDataByte71 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte72") == 0) {
    s->m_uPacketDataByte72 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte73") == 0) {
    s->m_uPacketDataByte73 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte74") == 0) {
    s->m_uPacketDataByte74 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte75") == 0) {
    s->m_uPacketDataByte75 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte76") == 0) {
    s->m_uPacketDataByte76 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte77") == 0) {
    s->m_uPacketDataByte77 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte78") == 0) {
    s->m_uPacketDataByte78 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte79") == 0) {
    s->m_uPacketDataByte79 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte80") == 0) {
    s->m_uPacketDataByte80 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte81") == 0) {
    s->m_uPacketDataByte81 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte82") == 0) {
    s->m_uPacketDataByte82 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte83") == 0) {
    s->m_uPacketDataByte83 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte84") == 0) {
    s->m_uPacketDataByte84 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte85") == 0) {
    s->m_uPacketDataByte85 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte86") == 0) {
    s->m_uPacketDataByte86 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte87") == 0) {
    s->m_uPacketDataByte87 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte88") == 0) {
    s->m_uPacketDataByte88 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte89") == 0) {
    s->m_uPacketDataByte89 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte90") == 0) {
    s->m_uPacketDataByte90 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte91") == 0) {
    s->m_uPacketDataByte91 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte92") == 0) {
    s->m_uPacketDataByte92 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte93") == 0) {
    s->m_uPacketDataByte93 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte94") == 0) {
    s->m_uPacketDataByte94 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte95") == 0) {
    s->m_uPacketDataByte95 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte96") == 0) {
    s->m_uPacketDataByte96 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte97") == 0) {
    s->m_uPacketDataByte97 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte98") == 0) {
    s->m_uPacketDataByte98 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte99") == 0) {
    s->m_uPacketDataByte99 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte100") == 0) {
    s->m_uPacketDataByte100 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte101") == 0) {
    s->m_uPacketDataByte101 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte102") == 0) {
    s->m_uPacketDataByte102 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte103") == 0) {
    s->m_uPacketDataByte103 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte104") == 0) {
    s->m_uPacketDataByte104 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte105") == 0) {
    s->m_uPacketDataByte105 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte106") == 0) {
    s->m_uPacketDataByte106 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte107") == 0) {
    s->m_uPacketDataByte107 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte108") == 0) {
    s->m_uPacketDataByte108 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte109") == 0) {
    s->m_uPacketDataByte109 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte110") == 0) {
    s->m_uPacketDataByte110 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte111") == 0) {
    s->m_uPacketDataByte111 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte112") == 0) {
    s->m_uPacketDataByte112 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte113") == 0) {
    s->m_uPacketDataByte113 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte114") == 0) {
    s->m_uPacketDataByte114 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte115") == 0) {
    s->m_uPacketDataByte115 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte116") == 0) {
    s->m_uPacketDataByte116 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte117") == 0) {
    s->m_uPacketDataByte117 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte118") == 0) {
    s->m_uPacketDataByte118 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte119") == 0) {
    s->m_uPacketDataByte119 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte120") == 0) {
    s->m_uPacketDataByte120 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte121") == 0) {
    s->m_uPacketDataByte121 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte122") == 0) {
    s->m_uPacketDataByte122 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte123") == 0) {
    s->m_uPacketDataByte123 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte124") == 0) {
    s->m_uPacketDataByte124 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte125") == 0) {
    s->m_uPacketDataByte125 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte126") == 0) {
    s->m_uPacketDataByte126 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte127") == 0) {
    s->m_uPacketDataByte127 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte128") == 0) {
    s->m_uPacketDataByte128 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte129") == 0) {
    s->m_uPacketDataByte129 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte130") == 0) {
    s->m_uPacketDataByte130 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte131") == 0) {
    s->m_uPacketDataByte131 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte132") == 0) {
    s->m_uPacketDataByte132 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte133") == 0) {
    s->m_uPacketDataByte133 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte134") == 0) {
    s->m_uPacketDataByte134 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte135") == 0) {
    s->m_uPacketDataByte135 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte136") == 0) {
    s->m_uPacketDataByte136 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte137") == 0) {
    s->m_uPacketDataByte137 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte138") == 0) {
    s->m_uPacketDataByte138 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte139") == 0) {
    s->m_uPacketDataByte139 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte140") == 0) {
    s->m_uPacketDataByte140 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte141") == 0) {
    s->m_uPacketDataByte141 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte142") == 0) {
    s->m_uPacketDataByte142 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte143") == 0) {
    s->m_uPacketDataByte143 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte144") == 0) {
    s->m_uPacketDataByte144 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte145") == 0) {
    s->m_uPacketDataByte145 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte146") == 0) {
    s->m_uPacketDataByte146 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte147") == 0) {
    s->m_uPacketDataByte147 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte148") == 0) {
    s->m_uPacketDataByte148 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte149") == 0) {
    s->m_uPacketDataByte149 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte150") == 0) {
    s->m_uPacketDataByte150 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte151") == 0) {
    s->m_uPacketDataByte151 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte152") == 0) {
    s->m_uPacketDataByte152 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte153") == 0) {
    s->m_uPacketDataByte153 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte154") == 0) {
    s->m_uPacketDataByte154 = value;
  } else if (SB_STRCMP(name, "m_upacketdatabyte155") == 0) {
    s->m_uPacketDataByte155 = value;
  } else {
    /* string failed to match any field--ignored */
    return -1;
  }

  return(0);

}
