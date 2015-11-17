/* -*-  Mode:C; c-basic-offset:4 -*- */
/*****************************************************************************
 *
 * $Id: sbFe2000InitUtils.h,v 1.8 Broadcom SDK $
 *
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
 *
 *****************************************************************************/

#include "soc/sbx/sbTypes.h"
#include "soc/sbx/glue.h"
#include "soc/sbx/sbWrappers.h"
#include <soc/sbx/fe2k_common/sbFe2000Common.h>
#include "sbZfFe2000PtQueueToPortEntry.hx"
#include "sbZfFe2000QmQueueConfigEntryConsole.hx"
#include "sbZfFe2000QmQueueConfigFlowControlPort.hx"
#include "sbZfFe2000QmQueueState0Entry.hx"
#include "sbZfFe2000QmQueueState1Entry.hx"
#include "sbZfFe2000PtPortToQueueEntry.hx"
#include "sbZfFe2000PrPortToQueueSr0Entry.hx"
#include "sbZfFe2000PrPortToQueueSr1Entry.hx"
#include "sbZfFe2000PrPortToQueueAg0Entry.hx"
#include "sbZfFe2000PrPortToQueueAg1Entry.hx"
#include "sbZfFe2000PrPortToQueueXg0Entry.hx"
#include "sbZfFe2000PrPortToQueueXg1Entry.hx"
#include "sbZfFe2000PrPortToQueuePciEntry.hx"
#include "sbZfFe2000QmQueueState0EnEntry.hx"
#include "sbZfFe2000PpInitialQueueStateEntry.hx"
#include "sbZfFe2000PpHeaderRecordSizeEntry.hx"
#include "sbZfFe2000PpQueuePriorityGroupEntry.hx"
#include "sbZfFe2000PdHeaderConfig.hx"
#include "sbZfFe2000MmInternal0MemoryEntry.hx"
#include "sbZfFe2000MmInternal1MemoryEntry.hx"

#define CA_PD_HEADER_CONFIG(i)  (SAND_HAL_REG_OFFSET(CA,PD_HDR_CONFIG0)+i*0x4)

void AddQueue(sbhandle userDeviceHandle, uint32 uInPre, uint32 uInPort, uint32 uOutPte,
              uint32 uOutPort, uint32 uQueue);
uint8 CaPrPortToQueueRead( sbhandle userDeviceHandle, uint32 uPre, uint32 uAddress, uint32 *nQueue);
uint8 CaPrPortToQueueWrite( sbhandle userDeviceHandle, uint32 uPre, uint32 uAddress, const uint32 nQueue);
uint8 CaPtQueueToPortRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtQueueToPortEntry_t *pZFrame );
uint8 CaPtQueueToPortWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtQueueToPortEntry_t *pZFrame );
uint8 CaQmQueueConfigRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000QmQueueConfigEntry_t *pZFrame);
uint8 CaQmQueueConfigWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000QmQueueConfigEntry_t *pZFrame);
uint8 CaQmQueueState0Read( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000QmQueueState0Entry_t *pZFrame );
uint8 CaQmQueueState0Write( sbhandle userDeviceHandle,uint32 uAddress,  sbZfFe2000QmQueueState0Entry_t *pZFrame );
uint8 CaQmQueueState1Read( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000QmQueueState1Entry_t *pZFrame );
uint8 CaPtPortToQueueRead( sbhandle userDeviceHandle, uint32 uPte, uint32 uAddress, uint32 *uQueue);
uint8 CaPtPortToQueueWrite( sbhandle userDeviceHandle, uint32 uPte, uint32 uAddress, const uint32 uQueue);
uint8 CaPtPortToQueuePciWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame );
uint8 CaPrPortToQueuePciWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PrPortToQueuePciEntry_t *pZFrame);
uint8 CaPrPortToQueueAg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PrPortToQueueAg1Entry_t *pZFrame );
uint8 CaPrPortToQueueAg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PrPortToQueueAg0Entry_t *pZFrame);
uint8 CaPrPortToQueueXg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PrPortToQueueXg1Entry_t *pZFrame );
uint8 CaPrPortToQueueXg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PrPortToQueueXg0Entry_t *pZFrame);
uint8 CaPrPortToQueueSr1Write( sbhandle userDeviceHandle,uint32 uAddress,  sbZfFe2000PrPortToQueueSr1Entry_t *pZFrame );
uint8 CaPrPortToQueueSr0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PrPortToQueueSr0Entry_t *pZFrame);
uint8 CaPtPortToQueueSt1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaPtPortToQueueSt0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaPtPortToQueueAg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaPtPortToQueueAg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaPtPortToQueueXg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaPtPortToQueueXg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaQmQueueState0EnWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000QmQueueState0EnEntry_t *pZFrame );
uint8 CaPpInitialQueueStateWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PpInitialQueueStateEntry_t *pZFrame );
uint8 CaPpInitialQueueStateRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PpInitialQueueStateEntry_t *pZFrame );
uint8 CaPpHeaderRecordSizeWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PpHeaderRecordSizeEntry_t *pZFrame );
uint8 CaPpQueuePriorityGroupWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000PpQueuePriorityGroupEntry_t *pZFrame );
uint8 CaPdHeaderConfigWrite(sbhandle userDeviceHandle, uint32 uIndex, sbZfFe2000PdHeaderConfig_t *pZFrame);
uint8 CaQmQueueConfigClear(sbhandle userDeviceHandle);
uint8 CaQmQueueConfigFillPattern(sbhandle userDeviceHandle, sbZfFe2000QmQueueConfigEntry_t *pZFrame);
uint8 CaPtPortToQueueAg0Clear(sbhandle userDeviceHandle);
uint8 CaPtPortToQueueAg0FillPattern(sbhandle userDeviceHandle, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaPtPortToQueueAg1Clear(sbhandle userDeviceHandle);
uint8 CaPtPortToQueueAg1FillPattern(sbhandle userDeviceHandle, sbZfFe2000PtPortToQueueEntry_t *pZFrame);
uint8 CaQmQueueState0Clear(sbhandle userDeviceHandle);
uint8 CaQmQueueState0FillPattern(sbhandle userDeviceHandle, sbZfFe2000QmQueueState0Entry_t *pZFrame);
uint8 CaQmQueueState1Clear(sbhandle userDeviceHandle);
uint8 CaQmQueueState1FillPattern(sbhandle userDeviceHandle, sbZfFe2000QmQueueState1Entry_t *pZFrame);
uint8 CaMm0Internal0MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal0MemoryEntry_t *pZFrame);
uint8 CaMm0Internal1MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal1MemoryEntry_t *pZFrame);
uint8 CaMm1Internal0MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal0MemoryEntry_t *pZFrame);
uint8 CaMm1Internal1MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal1MemoryEntry_t *pZFrame);
uint8 CaMm0Internal0MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal0MemoryEntry_t *pZFrame);
uint8 CaMm0Internal1MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal1MemoryEntry_t *pZFrame);
uint8 CaMm1Internal0MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal0MemoryEntry_t *pZFrame);
uint8 CaMm1Internal1MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfFe2000MmInternal1MemoryEntry_t *pZFrame);

