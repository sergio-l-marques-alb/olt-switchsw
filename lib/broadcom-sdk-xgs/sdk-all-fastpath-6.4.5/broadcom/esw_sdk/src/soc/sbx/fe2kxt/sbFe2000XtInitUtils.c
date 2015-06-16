/* -*-  Mode:C; c-basic-offset:4 -*- */
/*****************************************************************************
 *
 * $Id: sbFe2000XtInitUtils.c,v 1.27 Broadcom SDK $
 *
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
 *
 * Utilities to facilitate bringup
 *****************************************************************************/

#include <shared/bsl.h>

#include <soc/error.h>
#include <soc/types.h>

#include "sbFe2000XtInitUtils.h"
#include "sbFe2000XtImfDriver.h"
#include "soc/drv.h"
#include "hal_c2_auto.h"
#include "sbZfC2QmQueueState0EnEntryConsole.hx"
#include "sbFe2000CommonUtil.h"

/* gnats 28299 - track per unit, per map, per port - queue writes */
#define P2Q_NUM_MAPS         7
#define P2Q_NUM_ADDRESSES   64
static uint8 p2q[SOC_MAX_NUM_DEVICES][P2Q_NUM_MAPS][P2Q_NUM_ADDRESSES];

/* smaller debug memory size for tuning only */
#define DDR_TUNE_MM0_N0_ADDR_SZ (19)
#define DDR_TUNE_MM0_N1_ADDR_SZ (19)
#define DDR_TUNE_MM0_W_ADDR_SZ  (18)
#define DDR_TUNE_MM1_N0_ADDR_SZ (19)
#define DDR_TUNE_MM1_N1_ADDR_SZ (19)
#define DDR_TUNE_MM1_W_ADDR_SZ  (18)
#define DDR_TUNE_RAM_SIZE_BASE (15)


/* temp -- only used in cleanup which should re-init */
uint32
DDRCalculateLrLoaderEpoch(sbFe2000XtInitParams_t *pInitParams)
{
  uint32 uLoaderEpoch = 0;

  int64 fIngressLoadTime;
  int64 fEgressLoadTime;
#if 0
  int64 fXferTime;
  int64 fFirstTime;
  int64 fIntermediate;
#endif
  /* rgf - Aug 29 2007 - The loader happens in the CORE domain but this value is used in the 
   * rgf - Aug 29 2007 - HPP domain so we need to scale it to accommodate.*/

/*uint64 fClockScale = (SB_FE2000XT_PIPELINE_CLOCK_RATE / SB_FE2000XT_CORE_CLOCK_RATE);*/
  uint32 ClockScale = 0;

  if (pInitParams->uHppFreq == 0 || pInitParams->uSwsFreq == 0) {
    SB_ASSERT(0);
  }

  ClockScale = (pInitParams->uHppFreq / pInitParams->uSwsFreq);

  /* rgf - Dec 08 2006 - From Nick's explanation in BUG23565
   * rgf - Dec 08 2006 - Context Load Time:
   * rgf - Dec 08 2006 -
   * rgf - Dec 08 2006 - xfer_time = ceiling(frames_per_context / (10 + 2*load_count)) *
   * rgf - Dec 08 2006 -                    (10 + 2*load_count) * ceiling(words0 / 2)
   * rgf - Dec 08 2006 - first_time = load_count + 4 ;; {init & first states}
   * rgf - Aug 29 2007 - MODIFIED FOR C2 first_time = load_count + 10; added latency
   * rgf - Dec 08 2006 - load_time = xfer_time + first_time + 2 ;; {done & start states}
   * rgf - Dec 08 2006 -  example:
   * rgf - Dec 08 2006 - frames_per_context = 48; load_count = 1; words0 = 8
   * rgf - Dec 08 2006 - => xfer_time  = 192
   * rgf - Dec 08 2006 - => first_time = 5
   * rgf - Dec 08 2006 - => load_time = 192 + 5 + 2 = 199
   * rgf - Dec 08 2006 -
   * rgf - Dec 08 2006 - In paired mode you perform a similar calculation using words1 and add the two.
   * rgf - Dec 08 2006 -
   * rgf - Dec 08 2006 - Let's look at your example:
   * rgf - Dec 08 2006 - frames_per_context = 48; load_count = 3; words0 = 10; words1 = 6
   * rgf - Dec 08 2006 -
   * rgf - Dec 08 2006 - xfer_time0 = 48 * 5 = 240
   * rgf - Dec 08 2006 - xfer_time1 = 48 * 3 = 144
   * rgf - Dec 08 2006 - first_time = 7
   * rgf - Dec 08 2006 - load_time0 = 240 + 7 + 2 = 249
   * rgf - Dec 08 2006 - load_time1 = 144 + 7 + 2 = 153
   * rgf - Dec 08 2006 - load_time = 249 + 153 = 402
   */

#if 0
  fIntermediate = (10.0 + 2.0*(int64)pInitParams->lr.uLoadCount);
  fXferTime = (uint32)(ceil( (int64)(pInitParams->lr.uFramesPerContext) / fIntermediate ) *
             fIntermediate * ceil( (int64)pInitParams->lr.uWords0 / 2.0 ));

  fFirstTime = (int64)pInitParams->lr.uLoadCount + 4.0;
  fIngressLoadTime = fXferTime + fFirstTime + 2.0;

  fXferTime = (uint32)(ceil( (int64)(pInitParams->lr.uFramesPerContext) / fIntermediate ) *
             fIntermediate * ceil( (int64)pInitParams->lr.uWords1 / 2.0 ));
  /* First time remains the same as ingress */
  fEgressLoadTime = fXferTime + fFirstTime + 2.0;

  if( pInitParams->lr.bPairedMode == TRUE ) {
    uLoaderEpoch = (uint32)(fIngressLoadTime + fEgressLoadTime);
    /* if( (uLoaderEpoch < SB_FE2000_LR_20G_DUPLEX_EPOCH) && (gZM.GetAttribute("allow_small_epochs",0) == 0) ) { */
    if(uLoaderEpoch < SB_FE2000_LR_20G_DUPLEX_EPOCH ) {
      uLoaderEpoch = SB_FE2000_LR_20G_DUPLEX_EPOCH;
    }
  } else {
    uLoaderEpoch = (uint32)fIngressLoadTime;
  }
#endif

  COMPILER_64_SET(fIngressLoadTime, 0, pInitParams->lr.uFramesPerContext);
  COMPILER_64_UMUL_32(fIngressLoadTime, (pInitParams->lr.uWords0 >> 1));
  COMPILER_64_ADD_32(fIngressLoadTime, pInitParams->lr.uLoadCount);
  COMPILER_64_ADD_32(fIngressLoadTime, 4);

  COMPILER_64_SET(fEgressLoadTime, 0, pInitParams->lr.uFramesPerContext);
  COMPILER_64_UMUL_32(fEgressLoadTime, (pInitParams->lr.uWords1 >> 1));
  COMPILER_64_ADD_32(fIngressLoadTime, pInitParams->lr.uLoadCount);
  COMPILER_64_ADD_32(fIngressLoadTime, 4);

  if( pInitParams->lr.bPairedMode == TRUE ) {
    COMPILER_64_ADD_64(fIngressLoadTime,fEgressLoadTime);
    COMPILER_64_UMUL_32(fIngressLoadTime, ClockScale);
    uLoaderEpoch = COMPILER_64_LO(fIngressLoadTime);
    uLoaderEpoch += 14;  /* HPP delay is 14 clocks */
    /* if( (uLoaderEpoch < SB_FE2000XT_LR_24G_DUPLEX_EPOCH) && (gZM.GetAttribute("allow_small_epochs",0) == 0) ) { */
    if(uLoaderEpoch < SB_FE2000XT_LR_24G_DUPLEX_EPOCH ) {
      uLoaderEpoch = SB_FE2000XT_LR_24G_DUPLEX_EPOCH;
    }
  } else {
    /* rgf - Nov 05 2007 - Add a fudge factor for C2 added latency, not yet understood from where.*/
    COMPILER_64_UMUL_32(fIngressLoadTime, ClockScale);
    uLoaderEpoch = COMPILER_64_LO(fIngressLoadTime) + 14;
  }


  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("LoaderEpoch %d based on Paired %d Words0 %d Words1 %d FramesPerContext %d LoadCount %d"),
            uLoaderEpoch, pInitParams->lr.bPairedMode, pInitParams->lr.uWords0,
            pInitParams->lr.uWords1, pInitParams->lr.uFramesPerContext, pInitParams->lr.uLoadCount));

  return uLoaderEpoch;

}

/* the memory size is endcoded as 16 - x for ucode
 * so a 22-bit memory would be encoded as 5 */
static uint32
_MemSizeEnc(uint32 bits)
{
  SB_ASSERT(bits >= DDR_TUNE_RAM_SIZE_BASE);
  SB_ASSERT(bits - DDR_TUNE_RAM_SIZE_BASE <= 0x7);
  return bits - DDR_TUNE_RAM_SIZE_BASE;
}

int DDRMemDiagSetupCtlRegs(sbhandle userDeviceHandle, uint32 uMemoryMask, uint32 seed, uint32 uPhase)
{
  sbZfCaDiagUcodeShared_t zfSharedReg;
  sbZfCaDiagUcodeCtl_t zfCtlReg;
  uint32 ulCtlReg = 0;
  uint8 ulCtlRegBytes[4] = {0};
  uint32 ulSharedReg = 0;
  uint8 ulSharedRegBytes[4] = {0};
  uint32 ulEMask = uMemoryMask;
  uint32 uLrp;
  sbZfC2MmInternal0MemoryEntry_t zData0;
  sbZfC2MmInternal1MemoryEntry_t zData1;
  sbZfC2MmInternal0MemoryEntry_t zData2;
  sbZfC2MmInternal1MemoryEntry_t zData3;

  sbZfCaDiagUcodeShared_InitInstance(&zfSharedReg);
  sbZfCaDiagUcodeCtl_InitInstance(&zfCtlReg);

  zfCtlReg.m_ulStatus = 0;
  zfCtlReg.m_ulErrInject0 = 0;
  zfCtlReg.m_ulErrInject1 = 0;
  zfCtlReg.m_ulErrInd = 0;
  zfCtlReg.m_ulUcLoaded = 0;
  zfCtlReg.m_ulLrpState = 0;
  zfCtlReg.m_ulMemExstMm0N0 = GBITN(ulEMask, 0);
  zfCtlReg.m_ulMemExstMm0N1 = GBITN(ulEMask, 1);
  zfCtlReg.m_ulMemExstMm0W  = GBITN(ulEMask, 2);
  zfCtlReg.m_ulMemExstMm1N0 = GBITN(ulEMask, 3);
  zfCtlReg.m_ulMemExstMm1N1 = GBITN(ulEMask, 4);
  zfCtlReg.m_ulMemExstMm1W  = GBITN(ulEMask, 5);
  zfCtlReg.m_ulTestOffset = 0;

  sbZfCaDiagUcodeCtl_Pack(&zfCtlReg,&ulCtlRegBytes[0],1);
  CaSetDataWord(&ulCtlReg,&ulCtlRegBytes[0]);


  /* write out 48 per-lrp control entries (address 0-48 in int mem 0) */
  for (uLrp = 0; uLrp < 48; uLrp++) {
    sbZfC2MmInternal0MemoryEntry_InitInstance(&zData0);
    zData0.m_uData0 = ulCtlReg;
    C2Mm0Internal0MemoryWrite(userDeviceHandle,uLrp, &zData0);

  }

  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META("Writing MM0 Internal0 memory entry at addresses 0-47 -- 0x%08x\n"),
             ulCtlReg));

  /* define ram sizes and seed */
  zfSharedReg.m_ulMemSizeMm0N0 = _MemSizeEnc(DDR_TUNE_MM0_N0_ADDR_SZ);
  zfSharedReg.m_ulMemSizeMm0N1 = _MemSizeEnc(DDR_TUNE_MM0_N1_ADDR_SZ);
  zfSharedReg.m_ulMemSizeMm0W  = _MemSizeEnc(DDR_TUNE_MM0_W_ADDR_SZ);
  zfSharedReg.m_ulMemSizeMm1N0 = _MemSizeEnc(DDR_TUNE_MM1_N0_ADDR_SZ);
  zfSharedReg.m_ulMemSizeMm1N1 = _MemSizeEnc(DDR_TUNE_MM1_N1_ADDR_SZ);
  zfSharedReg.m_ulMemSizeMm1W  = _MemSizeEnc(DDR_TUNE_MM1_W_ADDR_SZ);

  zfSharedReg.m_ulRandomSeed   = seed & 0x3fff;

  sbZfCaDiagUcodeShared_Pack(&zfSharedReg,&ulSharedRegBytes[0],1);
  CaSetDataWord(&ulSharedReg,&ulSharedRegBytes[0]);

  /* write out shared config entry (address 0x0 in int mem 1) */
  sbZfC2MmInternal1MemoryEntry_InitInstance(&zData1);
  zData1.m_uData0 = ulSharedReg;
  C2Mm0Internal1MemoryWrite(userDeviceHandle,0, &zData1);

  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META(":Writing MM0 Internal1 memory entry at address 0x%08x -- 0x%08x\n"),
             0, ulSharedReg));

  /* write out shared memory size config entry (address 0x0 in int mem 2) */
  sbZfC2MmInternal0MemoryEntry_InitInstance(&zData2);
  zData2.m_uData0 = DDR_TUNE_RAM_SIZE_BASE;
  C2Mm1Internal0MemoryWrite(userDeviceHandle,0, &zData2);
  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META(":Writing MM1 Internal2 memory entry at address 0x%08x -- 0x%08x\n"),
             0, DDR_TUNE_RAM_SIZE_BASE));
  if ( uPhase == 2 ) {
    for (uLrp = 0; uLrp < 48; uLrp++) {
      sbZfC2MmInternal1MemoryEntry_InitInstance(&zData3);
      zData3.m_uData0 = uMemoryMask;
      C2Mm1Internal1MemoryWrite(userDeviceHandle,uLrp, &zData3);
    }
  }

  return SB_OK;
}

int
DDRMemDiagUpdateLrpEpoch(sbhandle userDeviceHandle, uint32 epoch)
{
  uint32 uTimeOut;
  uint32 uUpdateEvent;

  uint32 uConfig0 = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG0);
  uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, EPOCH, uConfig0, epoch - 11 );
#undef UPDATE
  uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, UPDATE, uConfig0, 1 );
  SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG0, uConfig0);

  uUpdateEvent = 0;
  uTimeOut = 2000;
  while ( !uUpdateEvent && uTimeOut ) {
    uUpdateEvent = SAND_HAL_GET_FIELD(C2, LR_EVENT, UPDATE, SAND_HAL_READ(userDeviceHandle, C2, LR_EVENT));
    --uTimeOut;
  }

  SAND_HAL_WRITE(userDeviceHandle, C2, LR_EVENT, SAND_HAL_SET_FIELD(C2, LR_EVENT, UPDATE, 0x1));

  if ( !uTimeOut ) {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("Timeout waiting for UPDATE event from the LRP.\n")));
    return SB_FAILED;
  }
 
  return SB_OK;
}

uint32 
DDRMemDiagWaitForPhase(sbhandle userDeviceHandle, uint32 phase)
{
  uint32 uTimeout = 0;
  uint32 uDone = 0;
  sbZfC2MmInternal0MemoryEntry_t zData;

  while ( !uDone ) {
    C2Mm0Internal0MemoryRead(userDeviceHandle,0, &zData);
    uDone = zData.m_uData0 & 0x1;

    if ( (++uTimeout) > 200000) {
      return 1;
    }
  }

  return 0;
}

uint32 
DDRMemDiagEnableProcessing(sbhandle userDeviceHandle)
{
  sbZfC2LrLrpInstructionMemoryBank0Entry_t zLrpInstructionMemoryBank0ClearEntry;
  uint32 status;

  sbZfC2LrLrpInstructionMemoryBank0Entry_InitInstance(&zLrpInstructionMemoryBank0ClearEntry);
  /* Remove the unconditional jump at stream0-instruction0 to start processing */
  C2LrLrpInstructionMemoryBank0Write( userDeviceHandle, 0, &zLrpInstructionMemoryBank0ClearEntry);
  status = sbFe2000SwapInstructionMemoryBank(userDeviceHandle);
  if (status != SB_OK)
      return status;

  return SB_OK;
}

uint32 
DDRMemDiagCheckForErrors(sbhandle userDeviceHandle, uint32 uMemMask) 
{
  uint32 uPortGood = 0x3f;
  uint32 uError = 0;
  uint32 uPe = 0;
  sbZfC2MmInternal1MemoryEntry_t zData;

  sbZfC2MmInternal1MemoryEntry_InitInstance(&zData);
  uError = SAND_HAL_READ(userDeviceHandle, C2, LU_ERROR);

  /* If no error interrupt, then all ports passed, so no need to read memory
   * to get status of ports.
   */

  if (uError != 0) {
    for(uPe=0;uPe<48;uPe++) {
      C2Mm1Internal1MemoryRead(userDeviceHandle,0+uPe,&zData);
      uPortGood &= zData.m_uData0;
    }
  }

  return (~(uMemMask & uPortGood)) & 0x3f;
}

void 
DDRMemDiagCleanUp(sbhandle userDeviceHandle, sbFe2000XtInitParams_t *pInitParams)
{
  soc_sbx_control_t *sbx;
  UINT uReg;

  sbx = SOC_SBX_CONTROL(UNIT_FROM_HANDLE(userDeviceHandle));
  if (!sbx) {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s sbx handle is invalid\n"),FUNCTION_NAME()));
    assert(0);
  }

  pInitParams->lr.uEpochLengthInCycles = DDRCalculateLrLoaderEpoch(pInitParams);

  /* return LR config registers to power-on values */
  SAND_HAL_RMW_FIELD(userDeviceHandle, C2, PC_CORE_RESET, LR_CORE_RESET, 0x1);
  uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG1) ;
  SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG1, uReg);
  SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG1, 0x1CC230 );
  uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG0) ;
  SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG0, uReg);
  SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG0, 0x4000000 );

  SAND_HAL_WRITE(userDeviceHandle, C2, LU_ERROR, 0xffffffff);
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, 0, MM_ERROR,  0xffffffff );
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, 1, MM_ERROR,  0xffffffff );


  SAND_HAL_WRITE_STRIDE(userDeviceHandle, C2, MM, 0, MM_CLIENT_CONFIG,pInitParams->mm[0].uMemDiagClientConfigSave);
  SAND_HAL_WRITE_STRIDE(userDeviceHandle, C2, MM, 1, MM_CLIENT_CONFIG,pInitParams->mm[1].uMemDiagClientConfigSave);
  SAND_HAL_WRITE_STRIDE(userDeviceHandle, C2, MM, 0, MM_RAM_CONFIG, pInitParams->mm[0].uMemDiagRamConfigSave);
  SAND_HAL_WRITE_STRIDE(userDeviceHandle, C2, MM, 1, MM_RAM_CONFIG, pInitParams->mm[1].uMemDiagRamConfigSave);

}

uint32 
DDRMemDiag(sbhandle userDeviceHandle, sbFe2000XtInitParams_t *pInitParams, uint32 uMemoryMask,
	int32 nDataMask, int32 nTestType)
{
  uint32 uConfig0;
  uint32 uConfig1;
  uint32 uEvent;
  uint32 uTimeOut;
  uint32 uMmClientConfig;
  uint32 uMmRamConfig;
  uint32 uInstance;
  uint32 uIndex;
  sbZfC2LrLrpInstructionMemoryBank0Entry_t zLrpInstructionMemoryBank0ClearEntry;
  int status = SB_OK;
  soc_sbx_control_t *sbx;
  uint32 uReg;
  fe2kAsm2IntD *ucodeBytes;
  uint32 elen;

  if ( pInitParams->bMemDiagLrpEnable == FALSE ) {
    pInitParams->bMemDiagLrpEnable = TRUE;
#ifndef __KERNEL__
    /* coverity[secure_coding] */
    pInitParams->mm[0].uMemDiagSeed = rand();
#else
    
#endif
    /* LOG_INFO(BSL_LS_SOC_COMMON,
          (BSL_META("VCDL fine-tuning random seed 0x%0x\n"), pInitParams->mm[0].uMemDiagSeed)); */
    /* LOG_CLI((BSL_META("VCDL fine-tuning random seed 0x%0x\n"), pInitParams->mm[0].uMemDiagSeed)); */

    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, PC_CORE_RESET, LR_CORE_RESET, 0x1);

    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, PC_CORE_RESET, MM1_CORE_RESET, 0x0);
    if (1) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META(": forcing LR and LU to power-on defaults\n")));
        /* LOG_CLI((BSL_META(": forcing LR and LU to power-on defaults\n"))); */
        /* Set LR registers to power-on defaults...
           for each register:
           -read for clear-on-read fields
           -writeback for write-one-to-clear fields
           -then wirte register power-on value */
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_EVENT_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EVENT_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EVENT_MASK, 0xFF0F );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_EVENT) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EVENT, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EVENT, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG1, 0x1CC230 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG0, 0x4000000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_EPOCH_TIMER) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EPOCH_TIMER, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EPOCH_TIMER, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_EPOCH_CNTRL) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EPOCH_CNTRL, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_EPOCH_CNTRL, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STATUS) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STATUS, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STATUS, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM4) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM4, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM4, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM3, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM7) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM7, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM7, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM6) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM6, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM6, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_STREAM5) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM5, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_STREAM5, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK4) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK4, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK4, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK3, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK8) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK8, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK8, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK7) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK7, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK7, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK6) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK6, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK6, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK5) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK5, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK5, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK12) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK12, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK12, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK11) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK11, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK11, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK10) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK10, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK10, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK9) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK9, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK9, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK16) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK16, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK16, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK15) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK15, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK15, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK14) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK14, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK14, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK13) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK13, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK13, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK20) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK20, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK20, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK19) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK19, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK19, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK18) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK18, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK18, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK17) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK17, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK17, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK24) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK24, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK24, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK23) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK23, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK23, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK22) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK22, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK22, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK21) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK21, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK21, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK28) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK28, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK28, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK27) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK27, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK27, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK26) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK26, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK26, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK25) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK25, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK25, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_IMEM_ACC_CTRL) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_CTRL, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_CTRL, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK31) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK31, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK31, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK30) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK30, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK30, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_TASK29) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK29, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_TASK29, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_IMEM_ACC_DATA2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_DATA2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_DATA2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_IMEM_ACC_DATA1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_DATA1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_DATA1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_IMEM_ACC_DATA0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_DATA0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_IMEM_ACC_DATA0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_PD_ASSIST) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_PD_ASSIST, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_PD_ASSIST, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_SW_ASSIST) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_SW_ASSIST, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_SW_ASSIST, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP0_GPR1_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_GPR1_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_GPR1_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP0_GPR0_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_GPR0_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_GPR0_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_LD_TM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_LD_TM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_LD_TM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_PE_TM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_PE_TM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_PE_TM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP1_GPR1_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_GPR1_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_GPR1_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP1_GPR0_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_GPR0_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_GPR0_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP0_HDR1_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_HDR1_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_HDR1_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP0_HDR0_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_HDR0_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP0_HDR0_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM1_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM1_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM1_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM0_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM0_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM0_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP1_HDR1_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_HDR1_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_HDR1_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_GROUP1_HDR0_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_HDR0_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_GROUP1_HDR0_DEBUG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM5_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM5_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM5_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM4_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM4_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM4_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM3_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM3_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM3_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM2_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM2_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM2_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM1_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM1_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM1_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM0_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM0_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM0_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM7_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM7_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM7_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BA_IMEM6_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM6_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BA_IMEM6_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM5_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM5_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM5_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM4_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM4_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM4_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM3_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM3_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM3_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM2_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM2_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM2_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_ECC_ERROR_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_ECC_ERROR_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_ECC_ERROR_MASK, 0x3 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_ECC_ERROR) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_ECC_ERROR, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_ECC_ERROR, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM7_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM7_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM7_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_BB_IMEM6_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM6_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_BB_IMEM6_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LR_DEBUG_FUSE) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_DEBUG_FUSE, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LR_DEBUG_FUSE, 0x0 );
        /* Set LU to power-on defaults */
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE_EVENT0_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT0_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT0_MASK, 0xFFFFFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE_EVENT0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_EVENT_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_EVENT_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_EVENT_MASK, 0xF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_EVENT) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_EVENT, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_EVENT, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE_EVENT1_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT1_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT1_MASK, 0xFFFFFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE_EVENT1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE_EVENT1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CONFIG1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CONFIG1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CONFIG1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CONFIG0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CONFIG0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CONFIG0, 0x5530 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_ERROR_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ERROR_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ERROR_MASK, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_ERROR) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ERROR, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ERROR, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_BUBBLE_HEADER0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_HEADER0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_HEADER0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_BUBBLE) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE, 0xFF00 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_TRANSMIT_FRAMES) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_TRANSMIT_FRAMES, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_TRANSMIT_FRAMES, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RECEIVE_FRAMES) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RECEIVE_FRAMES, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RECEIVE_FRAMES, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_BUBBLE_STREAM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_STREAM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_STREAM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_BUBBLE_HEADER1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_HEADER1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_HEADER1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_B0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_A0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_BUBBLE_FRAMES) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_FRAMES, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_BUBBLE_FRAMES, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RECIRC_FRAMES) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RECIRC_FRAMES, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RECIRC_FRAMES, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_B1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_A1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RNG0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_B2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_A2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RNG1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_B3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_B3, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_LIST_CONFIG_A3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_LIST_CONFIG_A3, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RNG2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE2, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CORE_CLK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CORE_CLK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CORE_CLK, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CONFIG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CONFIG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CONFIG, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RNG3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RNG3, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_UCODE3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_UCODE3, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_CLEAR) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_CLEAR, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_CLEAR, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CLOCKS_PER_US) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CLOCKS_PER_US, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CLOCKS_PER_US, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_US_PER_MS) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_US_PER_MS, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_US_PER_MS, 0x3E8 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_MS_TIMER) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_MS_TIMER, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_MS_TIMER, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_S_OFFSET) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_S_OFFSET, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_S_OFFSET, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_NS_TO_S_MARK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_TO_S_MARK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_TO_S_MARK, 0x3B9ACA00 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_SUB_NS_INC) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_SUB_NS_INC, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0xCCCCCCCC );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_NS_INC) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_INC, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_INC, 0x1 );

        /* LU_RTC_NS_INC
         *    amount to increment nanosecond counter every clock
         *    tick Computed by taking floor of (hpp clock period-1)
         * 
         * LU_RTC_SUBNS_INC
         *    Fractional Amount of Nanosecond
         *    accumulated per clock tick
         *    The clock_period listed below would be an accurate measured period s
         *    ince this register accounts for fractional clock periods as well as drift 2^31 *
         *    (clock_period - rtc_ns_inc)
         *
         * The original code read the register, wrote it back, then updated its value.
         * The final value written into the ns_inc and sub_ns_inc was based on only
         * one HHP frequency (400 mhz).  The added code (below) was copied from
         * sbFe2000XtInit.c sbFe2000XtInitLr() and it sets the *_inc value based on
         * HPP frequency  (SDK-393528).
         */
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_SUB_NS_INC) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_SUB_NS_INC, uReg);
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_NS_INC) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_INC, uReg);
        if        ( pInitParams->uHppFreq == SB_FE2000XT_HPP_FREQ_400000) {
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_NS_INC, 1);
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0xBFFFFFFF);
        } else if ( pInitParams->uHppFreq == SB_FE2000XT_HPP_FREQ_375000) {
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_NS_INC, 1);
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0xD5555555);
        } else if ( pInitParams->uHppFreq == SB_FE2000XT_HPP_FREQ_300000) {
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_NS_INC, 2);
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0xAAAAAAAA);
        } else if ( pInitParams->uHppFreq == SB_FE2000XT_HPP_FREQ_262500) {
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_NS_INC, 3);
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0x679E79E7);
        } else if ( pInitParams->uHppFreq == SB_FE2000XT_HPP_FREQ_258333) {
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_NS_INC, 3);
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0x6F7C08DD);
        } else if ( pInitParams->uHppFreq == SB_FE2000XT_HPP_FREQ_250000) {
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_NS_INC, 3);
            SAND_HAL_WRITE( userDeviceHandle, C2, LU_RTC_SUB_NS_INC, 0x0);
        } else {
            SB_ASSERT(0);
        }

        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_NS_VALUE) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_VALUE, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_VALUE, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_S_VALUE) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_S_VALUE, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_S_VALUE, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RTC_NS_OFFSET) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_OFFSET, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RTC_NS_OFFSET, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM4) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM4, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM4, 0x44444444 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM3, 0x33333333 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM2, 0x22222222 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM1, 0x11111111 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE0, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM7) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM7, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM7, 0x77777777 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM6) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM6, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM6, 0x66666666 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_PROGRAM5) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM5, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_PROGRAM5, 0x55555555 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE4) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE4, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE4, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE3) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE3, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE3, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE2) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE2, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE2, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE1, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_OAM_FIFO_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_OAM_FIFO_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_OAM_FIFO_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE7) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE7, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE7, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE6) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE6, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE6, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_ACTIVATE5) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE5, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_ACTIVATE5, 0xFFFF );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_EXT_MM0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_EXT_MM0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_EXT_MM0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_PM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_PM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_PM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CMU_FIFO_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CMU_FIFO_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CMU_FIFO_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_OAM_CNTR_DEBUG) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_OAM_CNTR_DEBUG, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_OAM_CNTR_DEBUG, 0x8000 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_CM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_CM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_CM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_INT_MM1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_INT_MM1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_INT_MM1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_INT_MM0) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_INT_MM0, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_INT_MM0, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_EXT_MM1) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_EXT_MM1, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_SLIP_BUFFER_DEBUG_EXT_MM1, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_ECC_ERROR) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ECC_ERROR, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ECC_ERROR, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_CMU_TM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CMU_TM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_CMU_TM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_RCE_TM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_TM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_RCE_TM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_OAM_TM) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_OAM_TM, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_OAM_TM, 0x0 );
        uReg = SAND_HAL_READ(userDeviceHandle, C2, LU_ECC_ERROR_MASK) ;
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ECC_ERROR_MASK, uReg);
        SAND_HAL_WRITE(userDeviceHandle, C2, LU_ECC_ERROR_MASK, 0x3 );
    }else{
      LOG_VERBOSE(BSL_LS_SOC_COMMON,
                  (BSL_META(": LU_CONFIG.enable NOT set.  Registers untouched.\n")));
    }

    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, PC_CORE_RESET, LR_CORE_RESET, 0x0);

    sbZfC2LrLrpInstructionMemoryBank0Entry_InitInstance(&zLrpInstructionMemoryBank0ClearEntry);

    /* Load "nops" into instruction memory, blow out what was left behind */
    for ( uIndex = 0; uIndex < 1024; ++uIndex ) {
      C2LrLrpInstructionMemoryBank0Write( userDeviceHandle, uIndex, &zLrpInstructionMemoryBank0ClearEntry);
    }
    status = sbFe2000SwapInstructionMemoryBank(userDeviceHandle);
    if (status != SB_OK) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s: swap instruction bank failed\n"),FUNCTION_NAME()));
      return status;
    }

    /* Now clear out the other bank */
    for ( uIndex = 0; uIndex < 1024; ++uIndex ) {
      C2LrLrpInstructionMemoryBank0Write( userDeviceHandle, uIndex, &zLrpInstructionMemoryBank0ClearEntry);
    }

    uConfig0 = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG0);
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, ENABLE, uConfig0, TRUE );
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, EPOCH, uConfig0, (pInitParams->lr.uEpochLengthInCycles - 11) );
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, CONTEXTS, uConfig0, pInitParams->lr.uNumberOfContexts );
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, PAIRED, uConfig0, pInitParams->lr.bPairedMode);
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, BYPASS, uConfig0, 0 );
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, LOAD_ENABLE, uConfig0, 0 );
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, ONLINE, uConfig0, 0x01 );
    uConfig0 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG0, UPDATE, uConfig0, 1 );

    uConfig1 = SAND_HAL_READ(userDeviceHandle, C2, LR_CONFIG1);
    uConfig1 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG1, PPE_PACE, uConfig1,(pInitParams->lr.uPpeRequestPaceInCycles-1) );
    uConfig1 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG1, FRAMES_PER_CONTEXT, uConfig1, pInitParams->lr.uFramesPerContext );
    uConfig1 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG1, LOAD_COUNT, uConfig1, pInitParams->lr.uLoadCount );
    uConfig1 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG1, WORDS0, uConfig1, pInitParams->lr.uWords0 );
    uConfig1 = SAND_HAL_MOD_FIELD( C2, LR_CONFIG1, WORDS1, uConfig1, pInitParams->lr.uWords1 );
    SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG1, uConfig1);
    SAND_HAL_WRITE(userDeviceHandle, C2, LR_CONFIG0, uConfig0);

    uEvent = 0;
    uTimeOut = 0;
    while ( !uEvent && (1500 > uTimeOut) ) {
      uEvent=SAND_HAL_GET_FIELD(C2, LR_EVENT, ONLINE0, SAND_HAL_READ(userDeviceHandle, C2, LR_EVENT));
      uTimeOut++;
    }
    if ( uTimeOut == 1500 ) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("Timeout waiting for ONLINE0 event from LRP.\n")));
    }

    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, LR_GROUP0_GPR0_DEBUG, DISABLE_PARITY_ERROR, 0x1);
    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, LR_GROUP0_GPR1_DEBUG, DISABLE_PARITY_ERROR, 0x1);
    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, LR_GROUP1_GPR0_DEBUG, DISABLE_PARITY_ERROR, 0x1);
    SAND_HAL_RMW_FIELD(userDeviceHandle, C2, LR_GROUP1_GPR1_DEBUG, DISABLE_PARITY_ERROR, 0x1);

  }

  if ( (pInitParams->mm[0].uMemDiagConfigMm == TRUE && uMemoryMask < 8) ||
       (pInitParams->mm[1].uMemDiagConfigMm == TRUE && uMemoryMask >= 8) ) {
    for ( uInstance = 0; uInstance < SB_FE2000_NUM_MM_INSTANCES; ++uInstance ) {
      uMmClientConfig = 0;
      uMmClientConfig = SAND_HAL_MOD_FIELD(C2,MM_CLIENT_CONFIG, LRP0_MEMORY_CONFIG, uMmClientConfig, 1);
      uMmClientConfig = SAND_HAL_MOD_FIELD(C2,MM_CLIENT_CONFIG, LRP1_MEMORY_CONFIG, uMmClientConfig, 1);
      uMmClientConfig = SAND_HAL_MOD_FIELD(C2,MM_CLIENT_CONFIG, LRP2_MEMORY_CONFIG, uMmClientConfig, 1);
      uMmClientConfig = SAND_HAL_MOD_FIELD(C2,MM_CLIENT_CONFIG, LRP3_MEMORY_CONFIG, uMmClientConfig, 1);
      uMmClientConfig = SAND_HAL_MOD_FIELD(C2,MM_CLIENT_CONFIG, LRP4_MEMORY_CONFIG, uMmClientConfig, 1);
      SAND_HAL_WRITE_STRIDE(userDeviceHandle,C2, MM, uInstance, MM_CLIENT_CONFIG,uMmClientConfig);

      uMmRamConfig = SAND_HAL_READ_STRIDE(userDeviceHandle, C2, MM, uInstance, MM_RAM_CONFIG);; 
      uMmRamConfig = SAND_HAL_MOD_FIELD( C2, MM_RAM_CONFIG, INTERNAL_RAM_CONFIG, uMmRamConfig, SB_FE2000XT_MM_RAM0_16KBY36_AND_RAM1_16KBY36 );
      uMmRamConfig = SAND_HAL_MOD_FIELD( C2, MM_RAM_CONFIG, INT_RAM0_DMA_GT32, uMmRamConfig, SB_FE2000XT_ONE_32_BIT_DAM_XFERS );
      uMmRamConfig = SAND_HAL_MOD_FIELD( C2, MM_RAM_CONFIG, INT_RAM1_DMA_GT32, uMmRamConfig, SB_FE2000XT_ONE_32_BIT_DAM_XFERS );
      SAND_HAL_WRITE_STRIDE(userDeviceHandle, C2, MM, uInstance, MM_RAM_CONFIG, uMmRamConfig);
    }

    if ( uMemoryMask < 8 ) {
      pInitParams->mm[0].uMemDiagConfigMm = FALSE;
    } else {
      pInitParams->mm[1].uMemDiagConfigMm = FALSE;
    }
  }

  SAND_HAL_WRITE(userDeviceHandle, C2, LU_ERROR, 0xffffffff);
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, 0, MM_ERROR,  0xffffffff );
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, 1, MM_ERROR,  0xffffffff );

  sbx = SOC_SBX_CONTROL(UNIT_FROM_HANDLE(userDeviceHandle));
  if (!sbx) {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s sbx handle invalid\n"),FUNCTION_NAME()));
    return SOC_E_INIT;
  }

  /* Phase 0 */

  if (nTestType != CC2_MEMDIAG_PHASE2_ONLY) {

    if (soc_sbx_fe2000_ucode_load_from_buffer((int)userDeviceHandle, soc_sbx_fe2kxt_vcdl0_tuning_fe2kAsm2UcodeEc, sizeof (soc_sbx_fe2kxt_vcdl0_tuning_fe2kAsm2UcodeEc)) != 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s,ucode load from buffer failed - phase 0\n"),FUNCTION_NAME()));
      return SB_FAILED;
    }

    ucodeBytes = (fe2kAsm2IntD *)sbx->ucode;
    if(fe2kAsm2IntD__getNc(ucodeBytes, "elen", &elen) > 0){
      assert(0);
    }

    /* set epoch length */
    pInitParams->lr.uEpochLengthInCycles = elen;
    status = sbFe2000UcodeLoad(userDeviceHandle, sbx->ucode);


    if (status != SB_OK) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s: sbFe2000UcodeLoad failed - phase 0 (%d)\n"),FUNCTION_NAME(),status));
      return status;
    }

    if (DDRMemDiagUpdateLrpEpoch(userDeviceHandle, pInitParams->lr.uEpochLengthInCycles) != 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s: DDRMemDiagUpdateLrpEpoch failed - phase 0 \n"),FUNCTION_NAME()));
      assert(0);
    }

    DDRMemDiagSetupCtlRegs(userDeviceHandle, uMemoryMask, pInitParams->mm[0].uMemDiagSeed,0);

    if ( DDRMemDiagEnableProcessing(userDeviceHandle) != SB_OK) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s, DDRMemDiagEnableProcessing failed - phase 0 \n"),FUNCTION_NAME()));
      return SB_FAILED;
    }

    if ( DDRMemDiagWaitForPhase(userDeviceHandle, 0) ) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("Phase0 TIMEOUT for memory mask 0x%02x!\n"), uMemoryMask));
      assert(0);
    } 
  }

  /* ssm - Feb 16 2009 - Phase 1 (read, write invert) no longer used.  Instead, only phase 0 (write)
   * ssm - Feb 16 2009 - and phase 2 (read) are used
   */

  if (nTestType != CC2_MEMDIAG_PHASE0_ONLY) {

    /* Phase 2 */
    switch(nDataMask) {

    case CC2_TEST_ALL_BITS: 
    if (soc_sbx_fe2000_ucode_load_from_buffer((int)userDeviceHandle, soc_sbx_fe2kxt_vcdl2_all_bits_fe2kAsm2UcodeEc, sizeof (soc_sbx_fe2kxt_vcdl2_all_bits_fe2kAsm2UcodeEc)) != 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s error loading fe2kxt_vcdl2_all_bist ucode\n"),FUNCTION_NAME()));
      return SB_FAILED;
    }
    break;
    case CC2_TEST_MS_BITS:
    if (soc_sbx_fe2000_ucode_load_from_buffer((int)userDeviceHandle, soc_sbx_fe2kxt_vcdl2_ms_bits_fe2kAsm2UcodeEc, sizeof (soc_sbx_fe2kxt_vcdl2_ms_bits_fe2kAsm2UcodeEc)) != 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s error loading fe2kxt_vcdl2_ms_bist ucode\n"),FUNCTION_NAME()));
      return SB_FAILED;
    }
    break;
    default:
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("Unsupported memory config\n")));
      assert(0);
    }

    /* set epoch length */
    ucodeBytes = (fe2kAsm2IntD *)sbx->ucode;
    if(fe2kAsm2IntD__getNc(ucodeBytes, "elen", &elen) > 0){
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("Getting epoch length failed\n")));
      assert(0);
    }

    pInitParams->lr.uEpochLengthInCycles = elen;

    status = sbFe2000UcodeLoad(userDeviceHandle, sbx->ucode);
    if (status != SB_OK) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s, sbFe2000UcodeLoad failed - phase 2 (%d)\n"),FUNCTION_NAME(),status));
      return status;
    }

    if (DDRMemDiagUpdateLrpEpoch(userDeviceHandle, pInitParams->lr.uEpochLengthInCycles) != 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s: DDRMemDiagUpdateLrpEpoch failed - phase 2 \n"),FUNCTION_NAME()));
      return SB_FAILED;
    }

    DDRMemDiagSetupCtlRegs(userDeviceHandle, uMemoryMask, pInitParams->mm[0].uMemDiagSeed,2);

    if ( DDRMemDiagEnableProcessing(userDeviceHandle) != SB_OK) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s DDRMemDiagEnableProcessing failed phase 2\n"),FUNCTION_NAME()));
      return SB_FAILED;
    }

    if ( DDRMemDiagWaitForPhase(userDeviceHandle, 2) ) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("Phase2 TIMEOUT for memory mask 0x%02x!\n"), uMemoryMask));
      return 0x3f;
    } else {
      return (DDRMemDiagCheckForErrors(userDeviceHandle,uMemoryMask));
    } 
  }

  return 0;
}

uint8
sbFe2000XtUtilPvtCompensation(sbhandle userDeviceHandle, uint32 uMmInstance)
{
  uint32 uData;
  uint8 bError = FALSE;
  uint32 uErrorCode = 1;
  uint32 uErrorRetry = 30;
  uint32 uTryCount = 1;
  uint32 uDone = 0;
  uint32 uNoDoneRetry = 100;
  uint32 uMm0PcompVal = 0;
  uint32 uMm0NcompVal = 0;
  uint32 uMm1PcompVal = 0;
  uint32 uMm1NcompVal = 0;
  uint32 uMmPcompVal = 0;
  uint32 uMmNcompVal = 0;

  if (SAL_BOOT_BCMSIM) {
      return FALSE; /* skip for BCMSIM environment */
  }

  /* dwl - Apr 12 2007 - Enable PVT compensation logic, use default sequence 
   *                   - (1) reset controller
   *                   - (2) setups up compensation paramters
   *                   - (3) Enable compensation
   *                   - (4) Wait for completion and check for error
   */

  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL0, PVT_RESETB, 0 );
  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1, PVT_COMP_EN, 0 );
  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL0, PVT_RESETB, 1 );


  uData = SAND_HAL_READ_STRIDE(userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL2);
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL2, PVT_OVERRIDE_EN, uData, 0);


  /* dwl - Jun 15 2007 - From Jim, PVT_RT75 should be set to 1
   *                   - 2 x 75 should be equal to external termination value
   */
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL2, PVT_RT75, uData, 1); /* 150 ohms on PCB */
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL2, uData );
  
  /* enable power_save, no offset is used */
  uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL3 );
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL3, PVT_POWER_SAVE_EN, uData, 1);
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL3, PVT_COMP_OFFSET_EN, uData, 0);
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL3, uData );


  while ( uErrorRetry && uErrorCode ) {
  
    /* compensation starts on rising edge oc PVT_COMP_EN */
    SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1, PVT_COMP_EN, 1 );
  
    /* wait for done */
    uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_STATUS );
    uDone = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_DONE, uData);
    uNoDoneRetry = 100;
    while ( uNoDoneRetry && !uDone ) {
      uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_STATUS );
      uDone = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_DONE, uData) ||
	SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_ERROR, uData);
      uNoDoneRetry--;
    }

    if ((uNoDoneRetry == 0) && !uDone) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("ERROR: MM%d PVT compensation timeout\n"), uMmInstance));
      bError = TRUE;
    }
  
    uErrorCode = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_ERROR, uData);
    if (uErrorCode != 0) {
      /* dwl - Aug 07 2007 - skip ERROR message, let the caller decide */
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("MM%d PVT compensation error on try %d, MM_PVT_STATUS=0x%x\n"), uMmInstance, uTryCount++, uData));
      /* decode the error status */
      uData = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_ERROR, uData);
      if (uData & 0x1)
	LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("Filter expired during PMOS compensation\n")));
      if (uData & 0x2)
	LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("Filter expired during NMOS compensation\n")));
      if (uData & 0x4)
	LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("Filter expired during ODT resistor compensation\n")));
      if (uData & 0x8)
	LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("P-code N-code invalid, their delta exceed limits\n")));
      if (uData & 0x10)
	LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("Watchdog timer expired\n")));
    }
    uErrorRetry--;
    SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1, PVT_COMP_EN, 0 );
  }
  
  if ((uErrorRetry == 0) && uErrorCode) {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("All %d MM%d PVT compensation attempts ended in failure\n"), --uTryCount, uMmInstance));
    bError = TRUE;
  } else {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("MM%d PVT compensation succeeded after %d attempts\n"), uMmInstance, uTryCount ));
    bError = FALSE;
  }
  
  if (SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_DONE, uData)) {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("MM%d PVT compensation done\n"), uMmInstance));
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("MM%d PVT compensation P,N,R code = {%d, %d, %d}\n"), 
              uMmInstance,
              SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCOMP_CODE_2CORE, uData),
              SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCOMP_CODE_2CORE, uData),
              SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_RCOMP_CODE_2CORE, uData)
              ));
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("MM%d PVT compensation P,N,R comparator outputs = {%d, %d, %d}\n"), 
              uMmInstance,
              SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCMPR_VAL_2CORE, uData),
              SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCMPR_VAL_2CORE, uData),
              SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_RCMPR_VAL_2CORE, uData)
              ));
    if (SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCOMP_CODE_2CORE, uData)==0 ||
	SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCOMP_CODE_2CORE, uData)==15)
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("Caution...:  MM%d PVT PCOMP_CODE is at extreme value of %d\n"),
                uMmInstance,
                SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCOMP_CODE_2CORE, uData)));

    if (SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCOMP_CODE_2CORE, uData)==0 ||
	SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCOMP_CODE_2CORE, uData)==15)
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("Caution...:  MM%d PVT NCOMP_CODE is at extreme value of %d\n"),
                uMmInstance,
                SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCOMP_CODE_2CORE, uData)));

    if (SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_RCOMP_CODE_2CORE, uData)==0 ||
	SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_RCOMP_CODE_2CORE, uData)==15)
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("Caution...:  MM%d PVT RCOMP_CODE is at extreme value of %d\n"),
                uMmInstance,
                SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_RCOMP_CODE_2CORE, uData)));
      
  }

   /* Steve has concluded based on timing margin characterization on the split
    * lot parts, that overriding the auto PVT values improves margin.  The
    * formula is to make PCOMP stronger by two (+2) and NCOMP weaker by 2 (+2).
    * So make that ajustment below.
    */

  if (uMmInstance == 0) {
    uMm0PcompVal = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCOMP_CODE_2CORE, uData);
    uMm0NcompVal = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCOMP_CODE_2CORE, uData);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("MM0 Pvt Comp, PMOS/NMOS from hw %d/%d\n"),
               uMm0PcompVal,uMm0NcompVal));

    uMm0PcompVal = (uMm0PcompVal > 13) ? 15 : uMm0PcompVal + 2;
    uMm0NcompVal = (uMm0NcompVal > 13) ? 15 : uMm0NcompVal + 2;

    uMmPcompVal = uMm0PcompVal;
    uMmNcompVal = uMm0NcompVal;

  } else {
    uMm1PcompVal = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_PCOMP_CODE_2CORE, uData);
    uMm1NcompVal = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_NCOMP_CODE_2CORE, uData);

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META("MM1 Pvt Comp, PMOS/NMOS from hw %d/%d\n"),
               uMm1PcompVal,uMm1NcompVal));

    uMm1PcompVal = (uMm1PcompVal > 13) ? 15 : uMm1PcompVal + 2;
    uMm1NcompVal = (uMm1NcompVal > 13) ? 15 : uMm1NcompVal + 2;

    uMmPcompVal = uMm1PcompVal;
    uMmNcompVal = uMm1NcompVal;

  }

  /* override PcompVal */
  uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1 );
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL1, PVT_OVERRIDE_MODE, uData, 0x1);
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL1, PVT_OVERRIDE_VAL,uData,uMmPcompVal);
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1, uData );
  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL2, PVT_OVERRIDE_EN, 1 );
  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META("MM%d Pvt Comp, PMOS compensation code change to %d\n"),
             uMmInstance,uMmPcompVal));

  sal_usleep(1000);

  uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_STATUS );
  uDone = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_DONE, uData);
  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META("MM%d Pvt Comp PMOS override, done=%d, status=0x%x\n"),
             uMmInstance,uDone,uData));
  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL2, PVT_OVERRIDE_EN, 0 );

  /* override NcompVal */
  uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1 );
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL1, PVT_OVERRIDE_MODE, uData, 0x2);
  uData = SAND_HAL_MOD_FIELD(C2, MM_PVT_CONTROL1, PVT_OVERRIDE_VAL, uData, uMmNcompVal);
  SAND_HAL_WRITE_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL1, uData );
  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META("MM%d Pvt Comp, NMOS compensation code change to %d\n"),
             uMmInstance,uMmNcompVal));
  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL2, PVT_OVERRIDE_EN, 1 );
    
  sal_usleep(1000);

  uData = SAND_HAL_READ_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_STATUS );
  uDone = SAND_HAL_GET_FIELD(C2, MM_PVT_STATUS, PVT_COMP_DONE, uData);
  LOG_DEBUG(BSL_LS_SOC_COMMON,
            (BSL_META("MM%d Pvt Comp NMOS override, done=%d, status=0x%x\n"),
             uMmInstance,uDone,uData));
  SAND_HAL_RMW_FIELD_STRIDE( userDeviceHandle, C2, MM, uMmInstance, MM_PVT_CONTROL2, PVT_OVERRIDE_EN, 0 );

  return bError;
}




/* FIX: function removed for sdk but needed for sbFe2000Diags.c */
#define SB_FE2000XT_QM_FLOW_CONTROL_PORT 0
/* modifed AddQueue method from verification */
void XtAddQueue(sbhandle userDeviceHandle, uint32 uInPre, uint32 uInPort, uint32 uOutPte,
              uint32 uOutPort, uint32 uQueue) {

  /* rgf - May 24 2006 - Write the appropriate PRE memory to map the inbound port to the */
  /* rgf - May 24 2006 - allocated queue. */
  C2PrPortToQueueWrite( userDeviceHandle, uInPre, uInPort, uQueue );

  /* rgf - May 15 2006 - Now enable that queue */
  /* rgf - May 15 2006 - enable=1,min_pages=1,max_pages=100,dropthresh2=80 */
  {

    sbZfC2QmQueueConfigEntry_t zQueueConfig;
    sbZfC2QmQueueConfigFlowControlPort_t zFlowControlPort;

    sbZfC2QmQueueConfigEntry_InitInstance(&zQueueConfig);
    sbZfC2QmQueueConfigFlowControlPort_InitInstance(&zFlowControlPort);

    C2QmQueueConfigRead( userDeviceHandle, uQueue, &zQueueConfig);
    zQueueConfig.m_uEnforcePoliceMarkings = 0;
    zQueueConfig.m_uFlowControlEnable = 0;
    zQueueConfig.m_uFlowControlThresh2 = 0;
    zQueueConfig.m_uFlowControlThresh1 = 0;
    zQueueConfig.m_uDropThresh2 = 80;
    zQueueConfig.m_uMaxPages = 100;
    zQueueConfig.m_uMinPages = 1;

    /* rgf - Jul 31 2006 - Associate any flow control with the inbound port/interface */

    zQueueConfig.m_uFlowControlTarget = SB_FE2000XT_QM_FLOW_CONTROL_PORT;
    zFlowControlPort.m_uFlowControlInterface = uInPre;
    zFlowControlPort.m_uFlowControlPort = uInPort;
    zQueueConfig.m_uFlowControlPort = zFlowControlPort.m_uFlowControlPort;

    /* jts - Oct 30 2006 - zQueueConfig.SetFlowControlInterface(uInPre); */
    /* jts - Oct 30 2006 - zQueueConfig.SetFlowControlPort(uInPort); */

    /* rgf - Jun 14 2006 - Queue is always enabled when added */
    zQueueConfig.m_uEnable = 1;
    C2QmQueueConfigWrite( userDeviceHandle, uQueue, &zQueueConfig);

  }

  /* rgf - May 15 2006 - Now write the enable for queue state 0 */
  {
    /* bms - Nov 08 2006 - Since only QueueState0 is back door and since we are only setting this enable bit before */
    /* bms - Nov 08 2006 - passing any traffic we can safely do a RMW */

    sbZfC2QmQueueState0Entry_t zQueueState;
    sbZfC2QmQueueState0Entry_InitInstance(&zQueueState);

    C2QmQueueState0Read( userDeviceHandle,uQueue, &zQueueState );
    zQueueState.m_uEnable = 1;
    zQueueState.m_uEmpty = 1;
    C2QmQueueState0Write( userDeviceHandle,uQueue, &zQueueState );
  }

  /* rgf - May 25 2006 - write PT queue to port and port to queue mappings */
  {
    sbZfC2PtQueueToPortEntry_t zQueueToPort;
    sbZfC2PtQueueToPortEntry_InitInstance(&zQueueToPort);
    zQueueToPort.m_uPpeBound = 0;
    zQueueToPort.m_uPpeEnqueue = 0;
    zQueueToPort.m_uDestInterface = uOutPte;
    zQueueToPort.m_uDestPort = uOutPort;
    zQueueToPort.m_uDestQueue = 0;

    C2PtQueueToPortWrite( userDeviceHandle,uQueue, &zQueueToPort );
    C2PtPortToQueueWrite( userDeviceHandle,uOutPte, uOutPort, uQueue );
  }
}


void
C2P2qShadowWrite(int unit, uint8 uPte, uint8 uAddress, uint8 uQueue)
{
    p2q[unit][uPte][uAddress] = uQueue;
}

void
C2P2qShadowRead(int unit, uint8 uPte, uint8 uAddress, uint8 *pQueue)
{
    *pQueue = p2q[unit][uPte][uAddress];
}


uint8 C2PtPortToQueueRead( sbhandle userDeviceHandle, uint32 uPte, uint32 uAddress, uint32 *uQueue) {
  int unit = (int) userDeviceHandle;

  if (SOC_UNIT_VALID(unit) && (uPte < P2Q_NUM_MAPS) && 
      (uAddress < P2Q_NUM_ADDRESSES)) 
  {
      uint8 tmp;
      C2P2qShadowRead(unit, uPte, uAddress, &tmp);
      *uQueue = tmp;
      return TRUE;
  }
  return FALSE;
}

uint8 C2PtPortToQueueWrite( sbhandle userDeviceHandle, uint32 uPte, uint32 uAddress, const uint32 uQueue) {
  uint8 bSuccess = FALSE;
  int unit = (int)userDeviceHandle;
  sbZfC2PtPortToQueueEntry_t zQueueEntry;
  sbZfC2PtPortToQueueEntry_InitInstance(&zQueueEntry);

  zQueueEntry.m_uQueue = uQueue;

  if (SOC_UNIT_VALID(unit) && (uPte < P2Q_NUM_MAPS) && 
      (uAddress < P2Q_NUM_ADDRESSES)) 
  {
      C2P2qShadowWrite(unit, uPte, uAddress, uQueue);
  }

  switch( uPte ) {
  case 0:
    bSuccess = C2PtPortToQueueSt0Write( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  case 1:
    bSuccess = C2PtPortToQueueSt1Write( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  case 2:
    bSuccess = C2PtPortToQueueAg0Write( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  case 3:
    bSuccess = C2PtPortToQueueAg1Write( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  case 4:
    bSuccess = C2PtPortToQueueXg0Write( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  case 5:
    bSuccess = C2PtPortToQueueXg1Write( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  case 6:
    bSuccess = C2PtPortToQueuePciWrite( userDeviceHandle, uAddress, &zQueueEntry);
    break;
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "%s: Invalid PTE %d specified."), FUNCTION_NAME(),uPte));
  }
  return bSuccess;
}

uint8 C2PrPortToQueueRead( sbhandle userDeviceHandle,uint32 uPre, uint32 uAddress, uint32 *uQueue) {
  uint32 uData[1];
  uint8 uData_bytes[4];
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2PrPortToQueueSr0Entry_t zQueueEntry;

  switch( uPre ) {
  case 0:
      status = sbFe2000XtImfDriver_PrPortToQueueSr0Read((uint32)userDeviceHandle, uAddress, uData);
      break;
  case 1:
      status = sbFe2000XtImfDriver_PrPortToQueueSr1Read((uint32)userDeviceHandle, uAddress, uData);
      break;
   case 2:
      status = sbFe2000XtImfDriver_PrPortToQueueAg0Read((uint32)userDeviceHandle, uAddress, uData);
      break;
  case 3:
      status = sbFe2000XtImfDriver_PrPortToQueueAg1Read((uint32)userDeviceHandle, uAddress, uData);
      break;
  case 4:
      status = sbFe2000XtImfDriver_PrPortToQueueXg0Read((uint32)userDeviceHandle, uAddress, uData);
      break;
  case 5:
      status = sbFe2000XtImfDriver_PrPortToQueueXg1Read((uint32)userDeviceHandle, uAddress, uData);
      break;
  case 6:
      status = sbFe2000XtImfDriver_PrPortToQueuePciRead((uint32)userDeviceHandle, uAddress, uData);
      break;
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s: Invalid PRE %d specified."), FUNCTION_NAME(),uPre));
    return FALSE;
  }
  if ( status != SAND_DRV_C2_STATUS_OK ) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }

  C2SetDataBytes(uData[0],&uData_bytes[0]);
  sbZfC2PrPortToQueueSr0Entry_Unpack(&zQueueEntry, uData_bytes, 1);
  *uQueue = zQueueEntry.m_uQueue;
  return TRUE;
}

uint8 C2PrPortToQueueWrite( sbhandle userDeviceHandle,uint32 uPre, uint32 uAddress, const uint32 uQueue) {
  uint8 bSuccess = FALSE;
  switch( uPre ) {
  case 0:
    {
      sbZfC2PrPortToQueueSr0Entry_t zQueueEntry;
      sbZfC2PrPortToQueueSr0Entry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueueSr0Write( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  case 1:
    {
      sbZfC2PrPortToQueueSr1Entry_t zQueueEntry;
      sbZfC2PrPortToQueueSr1Entry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueueSr1Write( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  case 2:
    {
      sbZfC2PrPortToQueueAg0Entry_t zQueueEntry;
      sbZfC2PrPortToQueueAg0Entry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueueAg0Write( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  case 3:
    {
      sbZfC2PrPortToQueueAg1Entry_t zQueueEntry;
      sbZfC2PrPortToQueueAg1Entry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueueAg1Write( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  case 4:
    {
      sbZfC2PrPortToQueueXg0Entry_t zQueueEntry;
      sbZfC2PrPortToQueueXg0Entry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueueXg0Write( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  case 5:
    {
      sbZfC2PrPortToQueueXg1Entry_t zQueueEntry;
      sbZfC2PrPortToQueueXg1Entry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueueXg1Write( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  case 6:
    {
      sbZfC2PrPortToQueuePciEntry_t zQueueEntry;
      sbZfC2PrPortToQueuePciEntry_InitInstance(&zQueueEntry);
      zQueueEntry.m_uQueue = uQueue;
      bSuccess = C2PrPortToQueuePciWrite( userDeviceHandle, uAddress, &zQueueEntry);
    }
    break;
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s: Invalid PRE %d specified."), FUNCTION_NAME(),uPre));
  }
  return bSuccess;
}

uint8 C2QmQueueState0Read( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2QmQueueState0Entry_t *pZFrame ) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_QmQueueState0Read((uint32)userDeviceHandle, uAddress, &uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s: Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  sbZfC2QmQueueState0Entry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2QmQueueState0Write( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2QmQueueState0Entry_t *pZFrame ) {
  uint8 uData_bytes[4] = {0};
  uint32 uData[1] = {0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2QmQueueState0Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_QmQueueState0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2QmQueueState1Read( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2QmQueueState1Entry_t *pZFrame ) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_QmQueueState1Read((uint32)userDeviceHandle, uAddress, &uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s: Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  sbZfC2QmQueueState1Entry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2PtQueueToPortRead( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2PtQueueToPortEntry_t *pZFrame ) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_PtQueueToPortRead((uint32)userDeviceHandle, uAddress, &uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s: Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  sbZfC2PtQueueToPortEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2PtQueueToPortWrite( sbhandle userDeviceHandle,uint32 uAddress,  sbZfC2PtQueueToPortEntry_t *pZFrame ) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4] = {0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtQueueToPortEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtQueueToPortWrite((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2QmQueueConfigRead( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2QmQueueConfigEntry_t *pZFrame) {
  uint32 uData[3]={0};
  uint8 uData_bytes[12]={0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_QmQueueConfigRead((uint32)userDeviceHandle, uAddress, &uData[0], &uData[1], &uData[2]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  C2SetDataBytes(uData[2],&uData_bytes[8]);
  sbZfC2QmQueueConfigEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2QmQueueConfigWrite( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2QmQueueConfigEntry_t *pZFrame) {
  uint32 uData[3]={0};
  uint8 uData_bytes[12]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2QmQueueConfigEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  C2SetDataWord(&uData[2],&uData_bytes[8]);
  status = sbFe2000XtImfDriver_QmQueueConfigWrite((uint32)userDeviceHandle, uAddress, uData[0], uData[1], uData[2]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueSt0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueSt0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueSt1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueSt1Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueueSr0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueSr0Entry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  /* now read data from the Zframe into the uData buffer */
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2PrPortToQueueSr0Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueueSr0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueueSr1Write( sbhandle userDeviceHandle,uint32 uAddress,  sbZfC2PrPortToQueueSr1Entry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  /* now read data from the Zframe into the uData buffer */
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2PrPortToQueueSr1Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueueSr1Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueueAg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueAg0Entry_t *pZFrame) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4]={0};
  /* now read data from the Zframe into the uData buffer */
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2PrPortToQueueAg0Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueueAg0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueueAg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueAg1Entry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PrPortToQueueAg1Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueueAg1Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueAg0Write( sbhandle userDeviceHandle,uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueAg0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueAg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueAg1Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueXg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueXg0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueXg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueXg1Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueueXg0Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueXg0Entry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PrPortToQueueXg0Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueueXg0Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueueXg1Write( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueueXg1Entry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PrPortToQueueXg1Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueueXg1Write((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}


uint8 C2PtPortToQueuePciWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PtPortToQueueEntry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueuePciWrite((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PrPortToQueuePciWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PrPortToQueuePciEntry_t *pZFrame) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PrPortToQueuePciEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PrPortToQueuePciWrite((uint32)userDeviceHandle,uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2QmQueueState0EnWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2QmQueueState0EnEntry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2QmQueueState0EnEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_QmQueueState0EnWrite((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PpInitialQueueStateRead( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpInitialQueueStateEntry_t *pZFrame) {
  uint32 uData[5]={0};
  uint8 uData_bytes[20]={0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_PpInitialQueueStateRead((uint32)userDeviceHandle, uAddress, &uData[0], &uData[1], 
                                                       &uData[2], &uData[3], &uData[4]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  C2SetDataBytes(uData[2],&uData_bytes[8]);
  C2SetDataBytes(uData[3],&uData_bytes[12]);
  C2SetDataBytes(uData[4],&uData_bytes[16]);
  sbZfC2PpInitialQueueStateEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2PpInitialQueueStateWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpInitialQueueStateEntry_t *pZFrame) {
  uint32 uData[5]={0};
  uint8 uData_bytes[20]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PpInitialQueueStateEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  C2SetDataWord(&uData[2],&uData_bytes[8]);
  C2SetDataWord(&uData[3],&uData_bytes[12]);
  C2SetDataWord(&uData[4],&uData_bytes[16]);
  status = sbFe2000XtImfDriver_PpInitialQueueStateWrite((uint32)userDeviceHandle, uAddress, uData[0], uData[1], 
                                                        uData[2], uData[3], uData[4]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;
}

uint8 C2PpHeaderRecordSizeWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpHeaderRecordSizeEntry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PpHeaderRecordSizeEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PpHeaderRecordSizeWrite((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2PpQueuePriorityGroupWrite( sbhandle userDeviceHandle, uint32 uAddress, sbZfC2PpQueuePriorityGroupEntry_t *pZFrame ) {
  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
  sbFe2000XtImfDriverStatus_t status;
  /* now read data from the Zframe into the uData buffer */
  sbZfC2PpQueuePriorityGroupEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PpQueuePriorityGroupWrite((uint32)userDeviceHandle, uAddress, uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2PdHeaderConfigWrite(sbhandle userDeviceHandle, uint32 uIndex, sbZfC2PdHeaderConfig_t *pZFrame) {

  uint32 uData[1]={0};
  uint8 uData_bytes[4]={0};
/*   rZFrame.CalcBufferUint(&uData,1,CZFrame::ZF_LSB_IN_WORD0_LSB); */
  sbZfC2PdHeaderConfig_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  SAND_HAL_WRITE_OFFS(userDeviceHandle,C2_PD_HEADER_CONFIG(uIndex),uData[0]);
  return TRUE;
}


uint8 C2QmQueueConfigClear(sbhandle userDeviceHandle) {
  sbZfC2QmQueueConfigEntry_t zClearFrame;
  sbZfC2QmQueueConfigEntry_InitInstance(&zClearFrame);
  return C2QmQueueConfigFillPattern(userDeviceHandle,&zClearFrame);
}

uint8 C2QmQueueConfigFillPattern(sbhandle userDeviceHandle, sbZfC2QmQueueConfigEntry_t *pZFrame) {
  uint32 uData[3] = {0};
  uint8 uData_bytes[12] = {0};
  sbFe2000XtImfDriverStatus_t status;

  sbZfC2QmQueueConfigEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  C2SetDataWord(&uData[2],&uData_bytes[8]);
  status = sbFe2000XtImfDriver_QmQueueConfigFillPattern((uint32)userDeviceHandle,uData[0],uData[1],uData[2]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueAg0Clear(sbhandle userDeviceHandle) {
  sbZfC2PtPortToQueueEntry_t zClearFrame;
  sbZfC2PtPortToQueueEntry_InitInstance(&zClearFrame);
  return C2PtPortToQueueAg0FillPattern(userDeviceHandle,&zClearFrame);
}

uint8 C2PtPortToQueueAg0FillPattern(sbhandle userDeviceHandle, sbZfC2PtPortToQueueEntry_t *pZFrame) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4] = {0};
  sbFe2000XtImfDriverStatus_t status;

  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueAg0FillPattern((uint32)userDeviceHandle,uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2PtPortToQueueAg1Clear(sbhandle userDeviceHandle) {
  sbZfC2PtPortToQueueEntry_t zClearFrame;
  sbZfC2PtPortToQueueEntry_InitInstance(&zClearFrame);
  return C2PtPortToQueueAg1FillPattern(userDeviceHandle,&zClearFrame);
}

uint8 C2PtPortToQueueAg1FillPattern(sbhandle userDeviceHandle, sbZfC2PtPortToQueueEntry_t *pZFrame) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4] = {0};
  sbFe2000XtImfDriverStatus_t status;

  sbZfC2PtPortToQueueEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_PtPortToQueueAg1FillPattern((uint32)userDeviceHandle,uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2QmQueueState0Clear(sbhandle userDeviceHandle) {
  sbZfC2QmQueueState0Entry_t zClearFrame;
  sbZfC2QmQueueState0Entry_InitInstance(&zClearFrame);
  return C2QmQueueState0FillPattern(userDeviceHandle,&zClearFrame);
}

uint8 C2QmQueueState0FillPattern(sbhandle userDeviceHandle, sbZfC2QmQueueState0Entry_t *pZFrame) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4] = {0};
  sbFe2000XtImfDriverStatus_t status;

  sbZfC2QmQueueState0Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_QmQueueState0FillPattern((uint32)userDeviceHandle,uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;

}

uint8 C2QmQueueState1Clear(sbhandle userDeviceHandle) {
  sbZfC2QmQueueState1Entry_t zClearFrame;
  sbZfC2QmQueueState1Entry_InitInstance(&zClearFrame);
  return C2QmQueueState1FillPattern(userDeviceHandle,&zClearFrame);
}

uint8 C2QmQueueState1FillPattern(sbhandle userDeviceHandle, sbZfC2QmQueueState1Entry_t *pZFrame) {
  uint32 uData[1] = {0};
  uint8 uData_bytes[4] = {0};
  sbFe2000XtImfDriverStatus_t status;

  sbZfC2QmQueueState1Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  status = sbFe2000XtImfDriver_QmQueueState1FillPattern((uint32)userDeviceHandle,uData[0]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;

}

uint8 C2Mm0Internal1MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2MmInternal1MemoryEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  status = sbFe2000XtImfDriver_Mm0Internal1MemoryWrite((uint32)userDeviceHandle,uAddress,uData[0],uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2Mm1Internal0MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2MmInternal0MemoryEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  status = sbFe2000XtImfDriver_Mm1Internal0MemoryWrite((uint32)userDeviceHandle,uAddress,uData[0],uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2Mm1Internal1MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2MmInternal1MemoryEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  status = sbFe2000XtImfDriver_Mm1Internal1MemoryWrite((uint32)userDeviceHandle,uAddress,uData[0],uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2Mm0Internal0MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_Mm0Internal0MemoryRead((uint32)userDeviceHandle,uAddress,&uData[0],&uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  sbZfC2MmInternal0MemoryEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2Mm0Internal1MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_Mm0Internal1MemoryRead((uint32)userDeviceHandle,uAddress,&uData[0],&uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  sbZfC2MmInternal1MemoryEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2Mm1Internal0MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_Mm1Internal0MemoryRead((uint32)userDeviceHandle,uAddress,&uData[0],&uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  sbZfC2MmInternal0MemoryEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}

uint8 C2Mm1Internal1MemoryRead(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal1MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  status = sbFe2000XtImfDriver_Mm1Internal1MemoryRead((uint32)userDeviceHandle,uAddress,&uData[0],&uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  /* now write into the Zframe */
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  sbZfC2MmInternal1MemoryEntry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;
}


uint8 C2Mm0Internal0MemoryWrite(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2MmInternal0MemoryEntry_t *pZFrame) {
  uint32 uData[2] = {0};
  uint8 uData_bytes[8] = {0};
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2MmInternal0MemoryEntry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  status = sbFe2000XtImfDriver_Mm0Internal0MemoryWrite((uint32)userDeviceHandle,uAddress,uData[0],uData[1]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
      /* good */
  } else {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
      return FALSE;
  }
  return TRUE;
}

uint8 C2LrLrpInstructionMemoryBank0Write(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2LrLrpInstructionMemoryBank0Entry_t *pZFrame) {
  uint32 uData[3]={0};
  uint8 uData_bytes[12]={0};
  sbFe2000XtImfDriverStatus_t status;
  sbZfC2LrLrpInstructionMemoryBank0Entry_Pack(pZFrame,&uData_bytes[0],1);
  C2SetDataWord(&uData[0],&uData_bytes[0]);
  C2SetDataWord(&uData[1],&uData_bytes[4]);
  C2SetDataWord(&uData[2],&uData_bytes[8]);  
  status = sbFe2000XtImfDriver_LrLrpInstructionMemoryBank0Write((uint32)userDeviceHandle,uAddress,uData[0],uData[1],uData[2]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  return TRUE;  
}

uint8 C2LrLrpInstructionMemoryBank0Read(sbhandle userDeviceHandle, uint32 uAddress, sbZfC2LrLrpInstructionMemoryBank0Entry_t *pZFrame) {
  uint32 uData[3] = {0};
  uint8 uData_bytes[12] = {0};
  sbFe2000XtImfDriverStatus_t status;

  status = sbFe2000XtImfDriver_LrLrpInstructionMemoryBank0Read((uint32)userDeviceHandle,uAddress,&uData[0],&uData[1],&uData[2]);
  if ( status == SAND_DRV_C2_STATUS_OK ) {
    /* good */
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("%s:Got bad status 0x%08x"), FUNCTION_NAME(),status));
    return FALSE;
  }
  C2SetDataBytes(uData[0],&uData_bytes[0]);
  C2SetDataBytes(uData[1],&uData_bytes[4]);
  C2SetDataBytes(uData[2],&uData_bytes[8]);

  sbZfC2LrLrpInstructionMemoryBank0Entry_Unpack(pZFrame,&uData_bytes[0],1);
  return TRUE;

}


/* temp helper routines */
void C2SetDataWord( uint32 *pWord, uint8 *pBytes) {
  *pWord = pBytes[0] | pBytes[1] << 8 | pBytes[2] << 16 | pBytes[3] << 24;
}

void C2SetDataBytes( uint32 uData, uint8 *pBytes) {
  pBytes[0]  = uData & 0xFF;
  pBytes[1] = (uData >> 8 ) & 0xFF;
  pBytes[2] = (uData >> 16 ) & 0xFF;
  pBytes[3] = (uData >> 24 ) & 0xFF;
}

uint8 C2PrCcCamRuleWrite( sbhandle userDeviceHandle, 
                            uint32 uCcBlock,
                            uint32 uCamEntryIndex, 
                            uint32 *pCfgBuffer, 
                            uint32 *pCfgRamBuffer,
                            uint32 valid)
{
  uint32 addr = uCamEntryIndex;
  uint32 stride = SAND_HAL_REG_OFFSET(C2, PR_CC3_TCAM_CONFIG_MEM_ACC_CTRL) - SAND_HAL_REG_OFFSET(C2, PR_CC2_TCAM_CONFIG_MEM_ACC_CTRL);
  uint32 ctrladdr = SAND_HAL_REG_OFFSET(C2, PR_CC2_TCAM_CONFIG_MEM_ACC_CTRL) + stride * (uCcBlock - 2);
  uint32 ramaddr = SAND_HAL_REG_OFFSET(C2, PR_CC2_CAM_RAM_MEM_ACC_CTRL) + stride * (uCcBlock - 2);

  /*
   * We write to
   * case 1:- cam ram if we are enabling the rule.
   * else we
   * case 2:- disable the rule first i.e cam config first
   * and then cam ram config.
   */

  if(valid) {
    /* case 1:- */
    /* write cam ram config first */
    if (sbFe2000UtilWriteIndir(userDeviceHandle, TRUE, TRUE,
                               ramaddr,
                               addr, SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_WORDS,
                               pCfgRamBuffer)) {
      return FALSE;
    }
    if (sbFe2000UtilWriteIndir(userDeviceHandle, TRUE, TRUE,
                               ctrladdr,
                               addr, SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_WORDS,
                               pCfgBuffer)) {
      return FALSE;
    }
  } else {
    /* case 2:- */
    /* we need to write Cam configuration first to disable first */
    if (sbFe2000UtilWriteIndir(userDeviceHandle, TRUE, TRUE,
                               ctrladdr,
                               addr, SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_WORDS,
                               pCfgBuffer)) {
      return FALSE;
    }
    if (sbFe2000UtilWriteIndir(userDeviceHandle, TRUE, TRUE,
                               ramaddr,
                               addr, SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_WORDS,
                               pCfgRamBuffer)) {
      return FALSE;
    }
  }
  return TRUE;
}

uint8 C2PrCcCamRuleRead( sbhandle userDeviceHandle, 
                           uint32 uCcBlock, 
                           uint32 uCamEntryIndex, 
                           uint32 *pCfgBuffer, 
                           uint32 *pCfgRamBuffer)
{
  uint32 addr = uCamEntryIndex;
  uint32 stride = SAND_HAL_REG_OFFSET(C2, PR_CC3_TCAM_CONFIG_MEM_ACC_CTRL) - SAND_HAL_REG_OFFSET(C2, PR_CC2_TCAM_CONFIG_MEM_ACC_CTRL);
  uint32 ctrladdr = SAND_HAL_REG_OFFSET(C2, PR_CC2_TCAM_CONFIG_MEM_ACC_CTRL) + stride * (uCcBlock - 2);
  uint32 ramaddr = SAND_HAL_REG_OFFSET(C2, PR_CC2_CAM_RAM_MEM_ACC_CTRL) + stride * (uCcBlock - 2);


  if (sbFe2000UtilReadIndir(userDeviceHandle, TRUE, TRUE,
                            ctrladdr,
                            addr, SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_WORDS,
                            pCfgBuffer)) {
    return FALSE;
  }
  if (sbFe2000UtilReadIndir(userDeviceHandle, TRUE, TRUE,
                            ramaddr, addr,
                            SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_WORDS,
                            pCfgRamBuffer)) {
    return FALSE;
  }

  return TRUE;
}

int
sbFe2000XtCcPort2QidMapSet(sbhandle userDeviceHandle, uint32 instance, uint32 entry, int32 destport, int32 qid)
{
    sbZfC2PrCcCamRamLastEntry_t zLastEntryRam;
    sbZfC2PrCcTcamConfigEntryFirst_t zTcamConfigEntryFirst;
    uint32 uCfgBuffer[SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_WORDS];
    uint32 uCfgRamBuffer[SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_WORDS];
    uint32 tmp, portmask, shift;
    uint32 uMappedDestPort = -1;

    sbZfC2PrCcCamRamLastEntry_InitInstance(&zLastEntryRam);
    sbZfC2PrCcTcamConfigEntryFirst_InitInstance(&zTcamConfigEntryFirst);

    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("Setting one2many map, Iface: 0x%x, destport: 0x%x, qid: 0x%x, entry: 0x%x\n"),
              instance, destport, qid, entry));
    zTcamConfigEntryFirst.m_uValid = SB_FE2000XT_PR_CC_KEY_VALID;
    portmask = soc_property_get(UNIT_FROM_HANDLE(userDeviceHandle), spn_HIGIG_DESTPORT_MASK, 0x3f00);
    shift = 0;
    tmp = portmask;
    while ( (tmp & 0x1) == 0) {
        shift++;
        tmp = tmp >> 1;
        if (shift > 31) {
            SB_ASSERT(0);
            return SB_FAILED;
        }
    }

    /* check for override of default mapping */
    if (destport != -1) {
        uMappedDestPort = soc_property_port_get(UNIT_FROM_HANDLE(userDeviceHandle), destport, spn_88025_HG_DESTPORT, destport);
    }

    /* match on uMappedDestPort field of header*/
    zTcamConfigEntryFirst.m_uPacket0_31 = 0;
    zTcamConfigEntryFirst.m_uPacket32_63 = (uMappedDestPort == -1) ? 0 : ((uMappedDestPort & (portmask >> shift)) << shift) << 16;
    zTcamConfigEntryFirst.m_uMask0_31 = 0;
    zTcamConfigEntryFirst.m_uMask32_63 = (uMappedDestPort == -1) ? 0 : portmask << 16;

    /* use port-based default queues */
    zLastEntryRam.m_uQueueAction = SB_ZF_C2PRCCCAMRAMLASTENTRY_QUEUE_ACT_LOOKUP_ONLY;
    zLastEntryRam.m_uQueue = qid;
    zLastEntryRam.m_uDrop = (qid == -1) ? 1: 0;
    zLastEntryRam.m_uLast = 1;

    sbZfC2PrCcTcamConfigEntryFirst_Pack(&zTcamConfigEntryFirst, (uint8*)uCfgBuffer,
                           SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_BYTES);
    sbZfC2PrCcCamRamLastEntry_Pack(&zLastEntryRam, (uint8*)uCfgRamBuffer,
                              SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_BYTES);

    if (C2PrCcCamRuleWrite(userDeviceHandle, instance, entry, uCfgBuffer, uCfgRamBuffer, 1) != TRUE) {
        return SB_FAILED;
    }

    return SB_OK;
}

int
sbFe2000XtCcPort2QidMapGet(sbhandle userDeviceHandle, uint32 instance, uint32 entry, int32* destport, int32* qid)
{
    sbZfC2PrCcCamRamLastEntry_t zLastEntryRam;
    sbZfC2PrCcTcamConfigEntryFirst_t zTcamConfigEntryFirst;
    uint32 uCfgBuffer[SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_WORDS];
    uint32 uCfgRamBuffer[SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_WORDS];
    uint32 tmp, portmask, shift;

    sbZfC2PrCcCamRamLastEntry_InitInstance(&zLastEntryRam);
    sbZfC2PrCcTcamConfigEntryFirst_InitInstance(&zTcamConfigEntryFirst);

    if (C2PrCcCamRuleRead(userDeviceHandle, instance, entry, uCfgBuffer, uCfgRamBuffer) != TRUE) {
        return SB_FAILED;
    }
    sbZfC2PrCcTcamConfigEntryFirst_Unpack(&zTcamConfigEntryFirst, (uint8*)uCfgBuffer,
                           SB_ZF_C2PRCCTCAMCONFIGENTRY_SIZE_IN_BYTES);
    sbZfC2PrCcCamRamLastEntry_Unpack(&zLastEntryRam, (uint8*)uCfgRamBuffer,
                              SB_ZF_C2PRCCCAMRAMLASTENTRY_SIZE_IN_BYTES);

    if ( zTcamConfigEntryFirst.m_uValid != SB_FE2000XT_PR_CC_KEY_VALID ){
        return SB_FAILED;
    }

    portmask = soc_property_get(UNIT_FROM_HANDLE(userDeviceHandle), spn_HIGIG_DESTPORT_MASK, 0x3f000000);
    shift = 0;
    tmp = portmask;
    while ( (tmp & 0x1) == 0) {
        shift++;
        tmp = tmp >> 1;
        if (shift > 31) {
            SB_ASSERT(0);
            return SB_FAILED;
        }
    }
    
    *destport = (zTcamConfigEntryFirst.m_uPacket32_63 & portmask) >> shift;
    *qid = (zLastEntryRam.m_uDrop) ? -1 : zLastEntryRam.m_uQueue;

    /* check for match all case */
    if (zTcamConfigEntryFirst.m_uMask32_63 == 0){
        *destport = -1;
    }

    return SB_OK;
}
