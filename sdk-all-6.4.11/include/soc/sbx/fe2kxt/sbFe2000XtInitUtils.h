/* -*-  Mode:C; c-basic-offset:4 -*- */
/*****************************************************************************
 * $Id: sbFe2000XtInitUtils.h,v 1.15 Broadcom SDK $
 *
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
 *
 *****************************************************************************/

#ifndef SB_FE2000XT_INITUTILS_H
#define SB_FE2000XT_INITUTILS_H

#include "soc/sbx/sbTypes.h"
#include "soc/sbx/glue.h"
#include "soc/sbx/sbWrappers.h"
#include <soc/sbx/fe2k/sbFe2000Init.h>
#include <soc/sbx/fe2kxt/sbFe2000XtInit.h>
#include "soc/sbx/fe2k_common/sbFe2000UcodeLoad.h"
#include "sbZfC2PtQueueToPortEntry.hx"
#include "sbZfC2QmQueueConfigEntryConsole.hx"
#include "sbZfC2QmQueueConfigFlowControlPort.hx"
#include "sbZfC2QmQueueState0Entry.hx"
#include "sbZfC2QmQueueState1Entry.hx"
#include "sbZfC2PtPortToQueueEntry.hx"
#include "sbZfC2PrPortToQueueSr0Entry.hx"
#include "sbZfC2PrPortToQueueSr1Entry.hx"
#include "sbZfC2PrPortToQueueAg0Entry.hx"
#include "sbZfC2PrPortToQueueAg1Entry.hx"
#include "sbZfC2PrPortToQueueXg0Entry.hx"
#include "sbZfC2PrPortToQueueXg1Entry.hx"
#include "sbZfC2PrPortToQueuePciEntry.hx"
#include "sbZfC2QmQueueState0EnEntry.hx"
#include "sbZfC2PpInitialQueueStateEntry.hx"
#include "sbZfC2PpHeaderRecordSizeEntry.hx"
#include "sbZfC2PpQueuePriorityGroupEntry.hx"
#include "sbZfC2PdHeaderConfig.hx"
#include "sbZfC2MmInternal0MemoryEntry.hx"
#include "sbZfC2MmInternal1MemoryEntry.hx"
#include "sbZfC2LrLrpInstructionMemoryBank0Entry.hx"
#include "sbZfC2PrCcTcamConfigEntry.hx"
#include "sbZfC2PrCcCamRamLastEntry.hx"
#include "soc/sbx/fe2kxt/sbZfC2Includes.h"
#include "soc/sbx/fe2000.h"
#include "soc/sbx/fe2kxt/fe2kdiag_vcdl_phase0_ucode.h"
#include "soc/sbx/fe2kxt/fe2kdiag_vcdl_phase1_ucode.h"
#include "soc/sbx/fe2kxt/fe2kdiag_vcdl_phase2_ucode.h"
#include "soc/sbx/fe2kxt/fe2kdiag_vcdl_tuning_phase0_ucode.h"
#include "soc/sbx/fe2kxt/fe2kdiag_vcdl_all_bits_phase2_ucode.h"
#include "soc/sbx/fe2kxt/fe2kdiag_vcdl_ms_bits_phase2_ucode.h"
#include "soc/sbx/fe2kxt/fe2kdiag_clearcc_ucode.h"
#include "soc/sbx/g2p3/c2_g2p3_ucode.h"

#include "soc/sbx/fe2k/sbZfCaDiagUcodeCtl.hx"
#include "soc/sbx/fe2k/sbZfCaDiagUcodeShared.hx"


#define UNIT_FROM_HANDLE(_h_) ( (uint32)_h_)

#define C2_PD_HEADER_CONFIG(i)  (SAND_HAL_REG_OFFSET(C2,PD_HDR_CONFIG0)+i*0x4)
#define RAM_SIZE_BASE  17
#define GBITN(x,n) (((x) >> n) & 0x1)

/* ssm - Feb 2 2009 - Added to support training.  Note that only settings 0-28 are tested, as 29, 30
 * ssm - Feb 2 2009 - and 31 are not valid (no additional increase over 28)
 */
#define NUM_QK_VCDL_SETTINGS 29
#define NUM_X_VCDL_SETTINGS 29
#define NUM_PHASE_SELECT_SETTINGS 3
#define NUM_DDR_PORTS_PER_C2 6
#define CC2_TEST_ALL_BITS 0
#define CC2_TEST_MS_BITS 1
#define CC2_TEST_LS_BITS 2
#define CC2_MEMDIAG_PHASE0_ONLY 0
#define CC2_MEMDIAG_PHASE2_ONLY 1
#define CC2_MEMDIAG_ALL_PHASES 2



uint32 DDRCalculateLrLoaderEpoch(sbFe2000XtInitParams_t *pInitParams);

int DDRMemDiagSetupCtlRegs(sbhandle userDeviceHandle, uint32 uMemoryMask, uint32 seed, uint32 uPhase);
int DDRMemDiagUpdateLrpEpoch(sbhandle userDeviceHandle, uint32 epoch);
uint32 DDRMemDiagWaitForPhase(sbhandle userDeviceHandle, uint32 phase);
uint32 DDRMemDiagEnableProcessing(sbhandle userDeviceHandle);
uint32 DDRMemDiagCheckForErrors(sbhandle userDeviceHandle, uint32 uMemoryMask);
void DDRMemDiagCleanUp(sbhandle userDeviceHandle, sbFe2000XtInitParams_t *pInitParams);
uint32 DDRMemDiag(sbhandle userDeviceHandle, sbFe2000XtInitParams_t *pInitParams, uint32 uMemoryMask,
		 int32 nDataMask, int32 nTestType);

uint8 sbFe2000XtUtilPvtCompensation(sbhandle userDeviceHandle, uint32 uMmInstance);
uint8 C2PrPortToQueueRead( sbhandle userDeviceHandle, uint32 uPre, uint32 uAddress, uint32 *nQueue);
uint8 C2PrPortToQueueWrite( sbhandle userDeviceHandle, uint32 uPre, uint32 uAddress, const uint32 nQueue);
uint8 C2PrCcCamRuleWrite( sbhandle userDeviceHandle, uint32 uCcBlock, uint32 uCamEntryIndex, uint32 *pCamConfigEntry, uint32 *pCamRamEntry, uint32 valid);
uint8 C2PrCcCamRuleRead( sbhandle userDeviceHandle, uint32 uCcBlock, uint32 uCamEntryIndex, uint32 *pCamConfigEntry, uint32 *pCamRamEntry);
uint8 C2PtQueueToPortRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtQueueToPortEntry_t *pZFrame );
uint8 C2PtQueueToPortWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtQueueToPortEntry_t *pZFrame );
uint8 C2QmQueueConfigRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2QmQueueConfigEntry_t *pZFrame);
uint8 C2QmQueueConfigWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2QmQueueConfigEntry_t *pZFrame);
uint8 C2QmQueueState0Read( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2QmQueueState0Entry_t *pZFrame );
uint8 C2QmQueueState0Write( sbhandle userDeviceHandle,uint32 uAddress,  sbZfC2QmQueueState0Entry_t *pZFrame );
uint8 C2QmQueueState1Read( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2QmQueueState1Entry_t *pZFrame );
uint8 C2PtPortToQueueRead( sbhandle userDeviceHandle, uint32 uPte, uint32 uAddress, uint32 *uQueue);
uint8 C2PtPortToQueueWrite( sbhandle userDeviceHandle, uint32 uPte, uint32 uAddress, const uint32 uQueue);
uint8 C2PtPortToQueuePciWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame );
uint8 C2PrPortToQueuePciWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueuePciEntry_t *pZFrame);
uint8 C2PrPortToQueueAg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueAg1Entry_t *pZFrame );
uint8 C2PrPortToQueueAg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueAg0Entry_t *pZFrame);
uint8 C2PrPortToQueueXg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueXg1Entry_t *pZFrame );
uint8 C2PrPortToQueueXg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueXg0Entry_t *pZFrame);
uint8 C2PrPortToQueueSr1Write( sbhandle userDeviceHandle,uint32 uAddress,  sbZfC2PrPortToQueueSr1Entry_t *pZFrame );
uint8 C2PrPortToQueueSr0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueSr0Entry_t *pZFrame);
uint8 C2PtPortToQueueSt1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2PtPortToQueueSt0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2PtPortToQueueAg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2PtPortToQueueAg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2PtPortToQueueXg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2PtPortToQueueXg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2QmQueueState0EnWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2QmQueueState0EnEntry_t *pZFrame );
uint8 C2PpInitialQueueStateWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpInitialQueueStateEntry_t *pZFrame );
uint8 C2PpInitialQueueStateRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpInitialQueueStateEntry_t *pZFrame );
uint8 C2PpHeaderRecordSizeWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpHeaderRecordSizeEntry_t *pZFrame );
uint8 C2PpQueuePriorityGroupWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpQueuePriorityGroupEntry_t *pZFrame );
uint8 C2PdHeaderConfigWrite(sbhandle userDeviceHandle, uint32 uIndex, sbZfC2PdHeaderConfig_t *pZFrame);
uint8 C2QmQueueConfigClear(sbhandle userDeviceHandle);
uint8 C2QmQueueConfigFillPattern(sbhandle userDeviceHandle, sbZfC2QmQueueConfigEntry_t *pZFrame);
uint8 C2PtPortToQueueAg0Clear(sbhandle userDeviceHandle);
uint8 C2PtPortToQueueAg0FillPattern(sbhandle userDeviceHandle, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2PtPortToQueueAg1Clear(sbhandle userDeviceHandle);
uint8 C2PtPortToQueueAg1FillPattern(sbhandle userDeviceHandle, sbZfC2PtPortToQueueEntry_t *pZFrame);
uint8 C2QmQueueState0Clear(sbhandle userDeviceHandle);
uint8 C2QmQueueState0FillPattern(sbhandle userDeviceHandle, sbZfC2QmQueueState0Entry_t *pZFrame);
uint8 C2QmQueueState1Clear(sbhandle userDeviceHandle);
uint8 C2QmQueueState1FillPattern(sbhandle userDeviceHandle, sbZfC2QmQueueState1Entry_t *pZFrame);
uint8 C2Mm0Internal0MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame);
uint8 C2Mm0Internal1MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame);
uint8 C2Mm1Internal0MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame);
uint8 C2Mm1Internal1MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame);
uint8 C2Mm0Internal0MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame);
uint8 C2Mm0Internal1MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame);
uint8 C2Mm1Internal0MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame);
uint8 C2Mm1Internal1MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame);
uint8 C2LrLrpInstructionMemoryBank0Write(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2LrLrpInstructionMemoryBank0Entry_t *pZframe);
uint8 C2LrLrpInstructionMemoryBank0Read(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2LrLrpInstructionMemoryBank0Entry_t *pZFrame);
int sbFe2000XtCcPort2QidMapSet(sbhandle userDeviceHandle, uint32 instance, uint32 entry, int32 destport, int32 qid);
int sbFe2000XtCcPort2QidMapGet(sbhandle userDeviceHandle, uint32 instance, uint32 entry, int32* destport, int32* qid);

/* helper routines */
void C2SetDataWord(uint32 * pWord, uint8 *pDataBytes);
void C2SetDataBytes( uint32 uData, uint8 *pBytes);

#endif
