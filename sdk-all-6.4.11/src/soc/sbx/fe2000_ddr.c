/*
 * $Id: fe2000_ddr.c,v 1.23 Broadcom SDK $
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
 * File:        fe2000_ddr.c
 * Purpose:     FE-2000-specific ddr training sequence
 * Requires:
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/types.h>
#include <assert.h>

#include <soc/sbx/fe2000.h>

#ifdef BCM_FE2000_SUPPORT
#include <soc/sbx/fe2kxt/sbFe2000XtInitUtils.h>
#include <soc/sbx/hal_user.h>
#include <soc/sbx/hal_ca_auto.h>
#include <soc/sbx/hal_c2_auto.h>
#include <soc/sbx/hal_ca_c2.h>
#include <soc/sbx/sbx_util.h>


extern int soc_sbx_counter_stop(int unit);
typedef int32 (*sbx_rdwr_t)(int unit, uint32 addr, uint32 data[2]);

int32  sbx_get_best_delay_tap(int32  nErrorsPerTap[], int32 nTaps);
uint32 sbx_ddr_error_count(int32 unit, sbx_rdwr_t writef, sbx_rdwr_t readf, 
                             uint32 num_words, uint32 uAddressWidth);

int32 wr_narrow0p0(int unit, uint32 addr, uint32 data[2]);
int32 wr_narrow0p1(int unit, uint32 addr, uint32 data[2]);
int32 wr_narrow1p0(int unit, uint32 addr, uint32 data[2]);
int32 wr_narrow1p1(int unit, uint32 addr, uint32 data[2]);

int32 rd_narrow0p0(int unit, uint32 addr, uint32 data[2]);
int32 rd_narrow0p1(int unit, uint32 addr, uint32 data[2]);
int32 rd_narrow1p0(int unit, uint32 addr, uint32 data[2]);
int32 rd_narrow1p1(int unit, uint32 addr, uint32 data[2]);

int32 wr_wide0(int unit, uint32 addr, uint32 data[2]);
int32 wr_wide1(int unit, uint32 addr, uint32 data[2]);
int32 rd_wide0(int unit, uint32 addr, uint32 data[2]);
int32 rd_wide1(int unit, uint32 addr, uint32 data[2]);

uint32 _rand(uint32 *seed);

static char *
_getPortName(uint32 uPort)
{
  char * mem_name = NULL;
  uint32 uInstance = uPort / 3;
  switch (uPort) {
  case 0: 
  case 3: mem_name = (uInstance == 0) ? "MM0 Narrow Port 0" : "MM1 Narrow Port 0"; break;
  case 1: 
  case 4: mem_name = (uInstance == 0) ? "MM0 Narrow Port 1" : "MM1 Narrow Port 1"; break;
  case 2: 
  case 5: mem_name = (uInstance == 0) ? "MM0 Wide Port" : "MM1 Wide Port"; break;
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("Invalid port (%d)\n"),uPort));
    assert(0);
  }
  return (mem_name);
}

static uint32
_SetValidDelay(int unit, uint32 uPort, uint32 uMmRamConfiguration, uint32 uValidDelay) 
{
  uint32 uInstance = uPort/3;
  sbhandle sbh = SOC_SBX_CONTROL(unit)->sbhdl;
  uValidDelay &= 0xf;
  uPort = uPort % 3;
  switch (uPort) {
  case 0: uMmRamConfiguration = SAND_HAL_MOD_FIELD(C2,MM_RAM_CONFIG,NARROW_PORT0_VALID_DELAY,uMmRamConfiguration,uValidDelay); break;
  case 1: uMmRamConfiguration = SAND_HAL_MOD_FIELD(C2,MM_RAM_CONFIG,NARROW_PORT1_VALID_DELAY,uMmRamConfiguration,uValidDelay); break;
  case 2: uMmRamConfiguration = SAND_HAL_MOD_FIELD(C2,MM_RAM_CONFIG,WIDE_PORT_VALID_DELAY,uMmRamConfiguration,uValidDelay);    break;
/* coverity[dead_error_begin] */
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Invalid port (%d)\n"),uPort));
    assert(0);
  }

  SAND_HAL_WRITE_STRIDE(sbh,C2,MM,uInstance, MM_RAM_CONFIG,uMmRamConfiguration);

  return (uMmRamConfiguration);

}

static uint32
_SetPhaseSelect(int unit,uint32 uPort, uint32 uPhyConfig3, uint32 uPhaseSelect)
{

  uint32 uInstance = uPort/3;
  sbhandle sbh = SOC_SBX_CONTROL(unit)->sbhdl;
  uPhaseSelect &= 0x3;
  uPort = uPort % 3;
  uPhyConfig3 &= 0xffffffcf;
  uPhyConfig3 |= uPhaseSelect << 4;

  switch (uPort) {
  case 0:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION0_CONFIG_REG3, uPhyConfig3 ); break;
  case 1:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION1_CONFIG_REG3, uPhyConfig3 ); break;
  case 2:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_IOW_CONFIG_REG3,  uPhyConfig3 ); break;
    /* coverity[dead_error_begin] */
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Invalid port (%d)\n"),uPort));
    assert(0);
  }

 return (uPhyConfig3);

}

static uint32
_SetXVcdl(int unit, uint32 uPort, uint32 uPhyConfig1, uint32 uXVcdl) 
{

  uint32 uInstance = uPort/3;
  sbhandle sbh = SOC_SBX_CONTROL(unit)->sbhdl;
  uXVcdl &= 0x1f;
  uPort = uPort % 3;
  uPhyConfig1 &= 0xbf0fffff;
  uPhyConfig1 |= ((uXVcdl & 0x10) << (30 - 4)) | ((uXVcdl & 0x0f) << 20);

  switch (uPort) {
  case 0:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION0_CONFIG_REG1, uPhyConfig1 ); break;
  case 1:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION1_CONFIG_REG1, uPhyConfig1 ); break;
  case 2:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_IOW_CONFIG_REG1,  uPhyConfig1 ); break;
 /* coverity[dead_error_begin] */
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Invalid port (%d)\n"),uPort));
    assert(0);
  }

 return (uPhyConfig1);  

}

static uint32
_SetQkVcdl(int unit, uint32 uPort, uint32 uPhyConfig1, uint32 uQkVcdl)
{
  UINT uInstance = uPort/3;
  sbhandle sbh = SOC_SBX_CONTROL(unit)->sbhdl;

  uQkVcdl &= 0x1f;
  uPort = uPort % 3;
  uPhyConfig1 &= 0xefff0fff;
  uPhyConfig1 |= ((uQkVcdl & 0x10) << (28 - 4)) | ((uQkVcdl & 0x0f) << 12);

  switch (uPort) {
  case 0:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION0_CONFIG_REG1, uPhyConfig1 ); break;
  case 1:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION1_CONFIG_REG1, uPhyConfig1 ); break;
  case 2:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_IOW_CONFIG_REG1,  uPhyConfig1 ); break;
  /* coverity[dead_error_begin] */
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Invalid port (%d)\n"),uPort));
    assert(0);
  }

 return (uPhyConfig1);

}

static uint32
_SetQknVcdl(int unit, uint32 uPort, uint32 uPhyConfig1, uint32 uQknVcdl)
{

  uint32 uInstance = uPort/3;
  sbhandle sbh = SOC_SBX_CONTROL(unit)->sbhdl;

  uQknVcdl &= 0x1f;
  uPort = uPort % 3;
  uPhyConfig1 &= 0xdff0ffff;
  uPhyConfig1 |= ((uQknVcdl & 0x10) << (29 - 4)) | ((uQknVcdl & 0x0f) << 16);

  switch (uPort) {
  case 0:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION0_CONFIG_REG1, uPhyConfig1 ); break;
  case 1:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_ION1_CONFIG_REG1, uPhyConfig1 ); break;
  case 2:SAND_HAL_WRITE_STRIDE( sbh, C2, MM, uInstance, MM_MM_IOW_CONFIG_REG1,  uPhyConfig1 ); break;
  /* coverity[dead_error_begin] */
  default:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Invalid port (%d)\n"),uPort));
    assert(0);
  }

 return (uPhyConfig1);

}

static uint32
_GetMidRange(uint32 uPassVector, uint32 *uLoPass, uint32 *uHiPass)
{
  uint32 uMidRange = 8;
  uint8 bLoPassFound = 0;
  uint8 bHiPassFound = 0;
  int i;

  for (i=0; i<32; i++) {
    if ((uPassVector >> i) & 0x1) {
      *uLoPass = i;
      bLoPassFound = 1;
      break;
    }
  }
  
  for (i=31; i>=0; i--) {
    if ((uPassVector >> i) & 0x1) {
      *uHiPass = i;
      bHiPassFound = 1;
      break;
    }
  }

  if ((bLoPassFound == 0) || (bHiPassFound == 0) || (*uHiPass == *uLoPass)) {
    return (0);
  } else {
    uMidRange = (*uLoPass + *uHiPass) / 2;
    return (uMidRange);
  }
}

/* ssm - Feb 12 2009 - Start of code added for new tuning.
 *
 * Function to implement new tuning algorithm that tries all combinations of
 * Valid Delay pipeline, Resample Clock Phase Select, X VCDL and QK VCDL
 * settings. (The QK_n VCDL is also tuned, but only after the other settings
 * have been established.)  This is needed because the Valid Delay and Phase
 * Select can't be determined unless the X and QK VCDL settings are at their
 * desired values, but the VCDL settings can't be determined unless the Valid
 * Delay and Phase Select are set properly.
 *
 * This code is not part of BringUpMm(), because to save time the settings for
 * all six DDR2 ports are done at once, whereas BringUpMm() deals with only
 * one MM (three ports) at a time.
*/
static uint32
_CornerLengthToPassCount(uint32 uCornerLength)
{
  uint32 uCornerArea = 0;
  uint32 uIndex;
  for (uIndex=1 ; uIndex<=uCornerLength; uIndex++) {
    uCornerArea += uIndex;
  }
  return (uCornerArea);
}
int
soc_sbx_fe2000_ddr_tune(int unit, unsigned int uDerateMargin)
{

  uint32 uValidDelay;
  uint32 uPhaseSelect;
  uint32 uQkVcdlPerPhasePassSettings[NUM_DDR_PORTS_PER_C2][SB_FE2000XT_MAX_VALID_DELAY+1][NUM_PHASE_SELECT_SETTINGS]; 
  uint32 uXVcdlPerPhasePassSettings [NUM_DDR_PORTS_PER_C2][SB_FE2000XT_MAX_VALID_DELAY+1][NUM_PHASE_SELECT_SETTINGS]; 
  uint32 uQkVcdlAggregatePassSettings[NUM_DDR_PORTS_PER_C2]; 
  uint32 uXVcdlAggregatePassSettings[NUM_DDR_PORTS_PER_C2]; 
  uint32 uQkVcdlTempPassSettings[NUM_DDR_PORTS_PER_C2];
  uint32 uQknVcdlPassSettings[NUM_DDR_PORTS_PER_C2]; 
  uint32 uXVcdlPassSettings[NUM_DDR_PORTS_PER_C2]; 
  uint32 uPassCount[NUM_DDR_PORTS_PER_C2][SB_FE2000XT_MAX_VALID_DELAY+1][NUM_PHASE_SELECT_SETTINGS]; 
  uint32 uMaxPassXVcdl[NUM_DDR_PORTS_PER_C2][SB_FE2000XT_MAX_VALID_DELAY+1][NUM_PHASE_SELECT_SETTINGS];
  uint32 uMaxPassQkVcdl[NUM_DDR_PORTS_PER_C2][SB_FE2000XT_MAX_VALID_DELAY+1][NUM_PHASE_SELECT_SETTINGS];
  uint32 uMaxPassXQkSum[NUM_DDR_PORTS_PER_C2][SB_FE2000XT_MAX_VALID_DELAY+1][NUM_PHASE_SELECT_SETTINGS];
  uint32 uXLoPass[NUM_DDR_PORTS_PER_C2];
  uint32 uXHiPass[NUM_DDR_PORTS_PER_C2];
  uint32 uXMidRange[NUM_DDR_PORTS_PER_C2];
  uint32 uQkLoPass[NUM_DDR_PORTS_PER_C2];
  uint32 uQkHiPass[NUM_DDR_PORTS_PER_C2];
  uint32 uQkMidRange[NUM_DDR_PORTS_PER_C2];
  uint32 uQknLoPass[NUM_DDR_PORTS_PER_C2];
  uint32 uQknHiPass[NUM_DDR_PORTS_PER_C2];
  uint32 uQknMidRange[NUM_DDR_PORTS_PER_C2];
  uint32 uPhyConfig1[NUM_DDR_PORTS_PER_C2];
  uint32 uPhyConfig3[NUM_DDR_PORTS_PER_C2];
  uint32 uBestPassCount[NUM_DDR_PORTS_PER_C2];
  uint32 uBestValidDelay[NUM_DDR_PORTS_PER_C2];
  uint32 uBestPhaseSelect[NUM_DDR_PORTS_PER_C2];
  uint32 uDriftSetupDerateTaps = uDerateMargin;
  uint32 uMmRamConfiguration[2];
  uint32 uMemMask = 0x3f;
  uint32 uMemDiagResult;
  uint32 uInstance;
  uint32 uMmSaveProtScheme0;
  uint32 uMmSaveProtScheme1;
  uint32 uTempScheme = 0x0;
  int32 nTemp0,nTemp1,nTemp2;
  uint32 uValidDelayTemp,uPhaseSelectTemp,uMaxRectangleXQkSum,uMaxCornerCutTaps,uPassCountDriftAdjust;
  uint32 uPort,uXVcdl,uQkVcdl,uQknVcdl;
  uint32 i,j,index;
  int32 nDataMask;
  sbhandle sbh;
  sbFe2000XtInitParams_t *feip;
  soc_sbx_control_t *sbx;

  sbFe2000XtInitParamsMm_t *pInstance0;
  sbFe2000XtInitParamsMm_t *pInstance1;

  feip = &SOC_SBX_CFG_FE2000(unit)->xt_init_params;
  sbx = SOC_SBX_CONTROL(unit);

  feip->bMemDiagLrpEnable = FALSE;
  feip->mm[0].uMemDiagConfigMm = TRUE;
  feip->mm[1].uMemDiagConfigMm = TRUE;

  pInstance0 = &(feip->mm[0]);
  pInstance1 = &(feip->mm[1]);

  if (!sbx) {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "bad sbx handle\n")));
    return 0;
  }
  
  /* not running g2p3 code */
  sbx->ucode_update_func = NULL;

  /* Note: no attempt made to handle case where only one MM is brought up. */
  if((pInstance0->bBringUp == TRUE) || (pInstance1->bBringUp == TRUE))  {
    LOG_CLI((BSL_META_U(unit,
                        "Performing tuning on MM0 and MM1 (takes 5-7 min) \n")));
    LOG_CLI((BSL_META_U(unit,
                        "Derate Marging value = 0x%x\n"),uDriftSetupDerateTaps));
  } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "NOT performing tuning on MM0 or MM1 - both must be enabled\n")));
    return 0;
  }

  sbh = SOC_SBX_CONTROL(unit)->sbhdl;

  /* stop the counter thread or CMU flush timeouts can occur */
  soc_sbx_counter_stop(unit);

  for (uInstance=0; uInstance<2; uInstance++) {
    uPhyConfig1[(uInstance * 3) + 0] = SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_MM_ION0_CONFIG_REG1);
    uPhyConfig1[(uInstance * 3) + 1] = SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_MM_ION1_CONFIG_REG1);
    uPhyConfig1[(uInstance * 3) + 2] = SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_MM_IOW_CONFIG_REG1);
      
    uPhyConfig3[(uInstance * 3) + 0] = SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_MM_ION0_CONFIG_REG3); 
    uPhyConfig3[(uInstance * 3) + 1] = SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_MM_ION1_CONFIG_REG3); 
    uPhyConfig3[(uInstance * 3) + 2] = SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_MM_IOW_CONFIG_REG3); 
      
    uMmRamConfiguration[uInstance] =   SAND_HAL_READ_STRIDE(sbh, C2, MM, uInstance, MM_RAM_CONFIG);
    uMmRamConfiguration[uInstance] =   SAND_HAL_MOD_FIELD( C2, MM_RAM_CONFIG, INTERNAL_RAM_CONFIG, uMmRamConfiguration[uInstance], SB_FE2000XT_MM_RAM0_16KBY36_AND_RAM1_16KBY36 );
  }


  /* Initialize vectors of VCDL settings that pass memory tests. */
  for (nTemp0=0; nTemp0<NUM_DDR_PORTS_PER_C2; nTemp0++) {
    uQkVcdlAggregatePassSettings[nTemp0] = 0; 
    uXVcdlAggregatePassSettings [nTemp0] = 0; 
    uQknVcdlPassSettings        [nTemp0] = 0; 
    uXVcdlPassSettings          [nTemp0] = 0; 
    for (nTemp1=0; nTemp1<SB_FE2000XT_MAX_VALID_DELAY+1; nTemp1++) {
      for (nTemp2=0; nTemp2<NUM_PHASE_SELECT_SETTINGS; nTemp2++) {
        uQkVcdlPerPhasePassSettings[nTemp0][nTemp1][nTemp2] = 0; 
        uXVcdlPerPhasePassSettings [nTemp0][nTemp1][nTemp2] = 0; 
        uPassCount                 [nTemp0][nTemp1][nTemp2] = 0; 
        uMaxPassXVcdl              [nTemp0][nTemp1][nTemp2] = 0; 
        uMaxPassQkVcdl             [nTemp0][nTemp1][nTemp2] = 0; 
        uMaxPassXQkSum             [nTemp0][nTemp1][nTemp2] = 0; 
      }
    }
  }

  /* Turn off protection. Disable wide port ECC. */
  uMmSaveProtScheme0 = SAND_HAL_READ( sbh, C2, MM0_PROT_SCHEME0 );
  uMmSaveProtScheme1 = SAND_HAL_READ( sbh, C2, MM1_PROT_SCHEME0 );
  uTempScheme = 0x0;
  uTempScheme = SAND_HAL_MOD_FIELD(C2,MM_PROT_SCHEME0,WPORT_DISABLE_ECC,uTempScheme,0x1);
  SAND_HAL_WRITE( sbh, C2, MM0_PROT_SCHEME0, uTempScheme );
  SAND_HAL_WRITE( sbh, C2, MM1_PROT_SCHEME0, uTempScheme );

  for (uValidDelay = SB_FE2000XT_MIN_VALID_DELAY; uValidDelay <= SB_FE2000XT_MAX_VALID_DELAY; uValidDelay++) {

    for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
      uMmRamConfiguration[uPort/3] = _SetValidDelay(unit,uPort, uMmRamConfiguration[uPort/3], uValidDelay);
    }

    for (uPhaseSelect=0; uPhaseSelect<NUM_PHASE_SELECT_SETTINGS; uPhaseSelect++) {
      /* only PhaseSelect of 0 and 2 are valid */
      if ((uPhaseSelect % 2) != 0) {
          continue;
      }
      for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
        uPhyConfig3[uPort] = _SetPhaseSelect(unit,uPort, uPhyConfig3[uPort], uPhaseSelect);
      }

      /* Nested FOR loop to test all valid combinations of QK and X VCDL setting.  It is
       * neccesary to do this because the QK and X VCDL settings can cause failures at
       * the resampling flops, thus causing false failures which can affect the setting
       * of the mid-point.
       *
       * Note that QK_N VCDL not tuned until later, as its value can't cause failures at
       * the resampling flops.
       */

      for (uXVcdl=0; uXVcdl<NUM_X_VCDL_SETTINGS; uXVcdl++) {
        for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
          uPhyConfig1[uPort] = _SetXVcdl(unit,uPort, uPhyConfig1[uPort], uXVcdl);
          uQkVcdlTempPassSettings[uPort] = 0; 
        }

        for (uQkVcdl=0; uQkVcdl<NUM_QK_VCDL_SETTINGS; uQkVcdl++) {
          for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
            uPhyConfig1[uPort] = _SetQkVcdl(unit,uPort, uPhyConfig1[uPort], uQkVcdl);
          }
    
          /* Test memory */
          uMemMask = 0x3F;  /* Test all six DDR ports simultaneously */

          /* Since we aren't tuning the QK_N VCDL just yet, we have to ignore errors on the LS
           * bits for 1.5-cycle DDR2 SRAM and 2.5-cycle DDR2+ SRAM.  For 2.0-cycle DDR2+ SRAM
           * errors on the MS bits need to be ignored.
           */

          if ( feip->uDdr2MemorySelect == SB_FE2000XT_DDR2_PLUS_20 ) {
            nDataMask = CC2_TEST_LS_BITS;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Memory Type - DDR2_PLUS_20 not supported\n")));
            return 0;
          } else {
            nDataMask = CC2_TEST_MS_BITS;
          }

          uMemDiagResult = DDRMemDiag(sbh,feip,uMemMask, nDataMask, CC2_MEMDIAG_ALL_PHASES);

          for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
            if (((uMemDiagResult >> uPort) & 0x1) == 0 ) {  /* Passed */
              uQkVcdlPerPhasePassSettings [uPort][uValidDelay][uPhaseSelect] |= 0x1 << uQkVcdl;
              uXVcdlPerPhasePassSettings  [uPort][uValidDelay][uPhaseSelect] |= 0x1 << uXVcdl;
              uQkVcdlAggregatePassSettings[uPort] |= 0x1 << uQkVcdl;
              uXVcdlAggregatePassSettings [uPort] |= 0x1 << uXVcdl;
              uQkVcdlTempPassSettings     [uPort] |= 0x1 << uQkVcdl;
              uPassCount[uPort][uValidDelay][uPhaseSelect]++;
              if ( uXVcdl > uMaxPassXVcdl[uPort][uValidDelay][uPhaseSelect] ) {
                uMaxPassXVcdl[uPort][uValidDelay][uPhaseSelect] = uXVcdl;
              }
              if ( uQkVcdl > uMaxPassQkVcdl[uPort][uValidDelay][uPhaseSelect] ) {
                uMaxPassQkVcdl[uPort][uValidDelay][uPhaseSelect] = uQkVcdl;
              }
              if ( (uXVcdl + uQkVcdl) > uMaxPassXQkSum[uPort][uValidDelay][uPhaseSelect] ) {
                uMaxPassXQkSum[uPort][uValidDelay][uPhaseSelect] = uXVcdl + uQkVcdl;
              }
            }
          }

          /* If not a single port is passing and X VCDL is higher than 15, break out of loop as
           * there is no need to test higher settings.
           */
          if ((uQkVcdl > 15) && (uMemDiagResult == 0x3f)) {
            break;
          }
        }

        for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
          LOG_VERBOSE(BSL_LS_SOC_DDR,
                      (BSL_META_U(unit,
                                  "%s QK VCDL pass vector for ValidDelay=%d, ResamplingPhaseSelect=%d, X VCDL=%d, uQkVcdlTempPassSettings[%d] = 0x%08x\n"),
                       _getPortName(uPort), uValidDelay, uPhaseSelect, uXVcdl, uPort,uQkVcdlTempPassSettings[uPort]));
        }

        /* If not a single port is passing for any Qk VCDL setting and X VCDL is higher than
         * 15, break out of loop as there is no need to test higher settings.
         */
        if ((uXVcdl > 15) &&
            ((uXVcdlPerPhasePassSettings[0][uValidDelay][uPhaseSelect] >> uXVcdl) == 0) &&
            ((uXVcdlPerPhasePassSettings[1][uValidDelay][uPhaseSelect] >> uXVcdl) == 0) &&
            ((uXVcdlPerPhasePassSettings[2][uValidDelay][uPhaseSelect] >> uXVcdl) == 0) &&
            ((uXVcdlPerPhasePassSettings[3][uValidDelay][uPhaseSelect] >> uXVcdl) == 0) &&
            ((uXVcdlPerPhasePassSettings[4][uValidDelay][uPhaseSelect] >> uXVcdl) == 0) &&
            ((uXVcdlPerPhasePassSettings[5][uValidDelay][uPhaseSelect] >> uXVcdl) == 0)) {
          break;
        }
      }

      for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {

        LOG_VERBOSE(BSL_LS_SOC_DDR,
                    (BSL_META_U(unit,
                                "%s  X and QK VCDL pass vectors for ValidDelay=%d, ResamplingPhaseSelect=%d, uXVcdlPerPhasePassSettings[%d][%d][%d] = 0x%08x,uQkVcdlPerPhasePassSettings[%d][%d][%d] = 0x%08x\n"),
                     _getPortName(uPort), uValidDelay, uPhaseSelect, uPort,uValidDelay,uPhaseSelect,
                     uXVcdlPerPhasePassSettings [uPort][uValidDelay][uPhaseSelect], uPort,uValidDelay,uPhaseSelect,
                     uQkVcdlPerPhasePassSettings[uPort][uValidDelay][uPhaseSelect])); 
      }
    }
  }

  for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {

    LOG_VERBOSE(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "%s X and QK VCDL aggregate pass vectors for all ValidDelay and ResamplingPhaseSelect = 0x%08x, 0x%08x\n"),
                 _getPortName(uPort), uXVcdlAggregatePassSettings[uPort], uQkVcdlAggregatePassSettings[uPort])); 

    /* Now find mid-point of passing settings for X and QK VCDLs, based on the aggregate test
     * results for all combinations of valid_delay and phase_select.
     */
    uXMidRange[uPort] = _GetMidRange(uXVcdlAggregatePassSettings[uPort], &uXLoPass[uPort], &uXHiPass[uPort]);
    if (uXMidRange[uPort] == 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Tuning failure for %s (port%d) X VCDL, pass vector=0x%08x"),
                 _getPortName(uPort),uPort, uXVcdlAggregatePassSettings[uPort]));
      uXMidRange[uPort] = 8;
    } else {
      LOG_VERBOSE(BSL_LS_SOC_DDR,
                  (BSL_META_U(unit,
                              "Memory tests pass for %s(port %d), X VCDL, settings from %d to %d (pass vector=0x%08x), setting to mid-range %d\n"),
                   _getPortName(uPort), uPort,uXLoPass[uPort], uXHiPass[uPort], uXVcdlAggregatePassSettings[uPort], uXMidRange[uPort]));
    }
    uQkMidRange[uPort] = _GetMidRange(uQkVcdlAggregatePassSettings[uPort], &uQkLoPass[uPort], &uQkHiPass[uPort]);
    if (uQkMidRange[uPort] == 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Tuning failure for %s (port %d), QK VCDL, pass vector=0x%08x\n"),
                 _getPortName(uPort), uPort,uQkVcdlAggregatePassSettings[uPort]));
      uQkMidRange[uPort] = 8;
    } else {
      LOG_VERBOSE(BSL_LS_SOC_DDR,
                  (BSL_META_U(unit,
                              "Memory tests pass for %s QK VCDL, settings from %d to %d (pass vector=0x%08x), setting to mid-range %d\n"),
                   _getPortName(uPort), uQkLoPass[uPort], uQkHiPass[uPort], uQkVcdlAggregatePassSettings[uPort], uQkMidRange[uPort])); 
    }
    for (uValidDelayTemp=SB_FE2000XT_MIN_VALID_DELAY; uValidDelayTemp<SB_FE2000XT_MAX_VALID_DELAY+1; uValidDelayTemp++) {
     LOG_VERBOSE(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Raw Pass Count for %s Valid_Delay=%d, Phase_Select=0,2: %4d %4d"),
                  _getPortName(uPort), uValidDelayTemp,
                  uPassCount[uPort][uValidDelayTemp][0], uPassCount[uPort][uValidDelayTemp][2]));

      /* Derate the pass count to account for decreased setup margin at the resampling
       * flop, do to delay increase as voltage decreases and temperature increases
       * while traffic is running.
       */
      for (uPhaseSelectTemp=0; uPhaseSelectTemp <= 2; uPhaseSelectTemp+=2) {
        uMaxRectangleXQkSum = uMaxPassXVcdl[uPort][uValidDelayTemp][uPhaseSelectTemp] +
                              uMaxPassQkVcdl[uPort][uValidDelayTemp][uPhaseSelectTemp];
        uMaxCornerCutTaps = uMaxRectangleXQkSum - uMaxPassXQkSum[uPort][uValidDelayTemp][uPhaseSelectTemp];
        uPassCountDriftAdjust = _CornerLengthToPassCount(uMaxCornerCutTaps + uDriftSetupDerateTaps) -
                                _CornerLengthToPassCount(uMaxCornerCutTaps);
        if (uPassCount[uPort][uValidDelayTemp][uPhaseSelectTemp] > uPassCountDriftAdjust) {
          uPassCount[uPort][uValidDelayTemp][uPhaseSelectTemp] -= uPassCountDriftAdjust;
        } else {
          uPassCount[uPort][uValidDelayTemp][uPhaseSelectTemp] = 0;
        }
      }
      LOG_VERBOSE(BSL_LS_SOC_DDR,
                  (BSL_META_U(unit,
                              "Drift Derated Pass Count for %s Valid_Delay=%d, Phase_Select=0,2: %4d %4d\n"),
                   _getPortName(uPort), uValidDelayTemp,
                   uPassCount[uPort][uValidDelayTemp][0], uPassCount[uPort][uValidDelayTemp][2]));
    }

    /* Now determine the best four combinations of valid_delay and resampling clock phase select 
     * based on the pass count.  These will be used later.
     */
    uBestPassCount[uPort] = 0;

    for (uValidDelay=SB_FE2000XT_MIN_VALID_DELAY; uValidDelay<SB_FE2000XT_MAX_VALID_DELAY+1; uValidDelay++) {
      for (uPhaseSelect=0; uPhaseSelect<NUM_PHASE_SELECT_SETTINGS; uPhaseSelect++) {
         /* only PhaseSelect of 0 and 2 are valid */
         if ((uPhaseSelect % 2) != 0) {
             continue;
         }
        /* If pass count for a particular valid_delay/phase_select is higher than the current
         * pass count, replace the pass count and valid_delay/phase_select
         */
        if (uPassCount[uPort][uValidDelay][uPhaseSelect] > uBestPassCount[uPort]) {
          uBestPassCount  [uPort] = uPassCount[uPort][uValidDelay][uPhaseSelect];
          uBestValidDelay [uPort] = uValidDelay;
          uBestPhaseSelect[uPort] = uPhaseSelect;
        }

      }
    }

    LOG_VERBOSE(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                            "Best setting for %s Valid_Delay/Phase_Select is %d/%d\n"),
                 _getPortName(uPort), uBestValidDelay[uPort], uBestPhaseSelect[uPort]));

    /* Load X and QK VCDL mid-point settings and resampling clock phase selection into PHY
     * registers, and valid_delay setting into MM register.
     */
    uPhyConfig1[uPort] = _SetXVcdl      (unit,uPort, uPhyConfig1[uPort], uXMidRange      [uPort]);
    uPhyConfig1[uPort] = _SetQkVcdl     (unit,uPort, uPhyConfig1[uPort], uQkMidRange     [uPort]);
    uPhyConfig3[uPort] = _SetPhaseSelect(unit,uPort, uPhyConfig3[uPort], uBestPhaseSelect[uPort]);
    uMmRamConfiguration[uPort/3] = _SetValidDelay(unit,uPort, uMmRamConfiguration[uPort/3], uBestValidDelay[uPort]);
  }

  /* Finally, tune the QK_n VCDL. */
  for (uQknVcdl=0; uQknVcdl<NUM_QK_VCDL_SETTINGS; uQknVcdl++) {
    for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
      uPhyConfig1[uPort] = _SetQknVcdl(unit,uPort, uPhyConfig1[uPort], uQknVcdl);
    }
    
    /* Test memory */

    uMemDiagResult = DDRMemDiag(sbh,feip,uMemMask, CC2_TEST_ALL_BITS, CC2_MEMDIAG_ALL_PHASES);

    for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
      if (((uMemDiagResult >> uPort) & 0x1) == 0 ) {  /* Passed */
        uQknVcdlPassSettings[uPort] |= 0x1 << uQknVcdl;
      }
    }
  }

  for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
    uQknMidRange[uPort] = _GetMidRange(uQknVcdlPassSettings[uPort], &uQknLoPass[uPort], &uQknHiPass[uPort]);
    if (uQknMidRange[uPort] == 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Tuning failure for %s QK_n VCDL, pass vector=0x%08x\n"),
                 _getPortName(uPort), uQknVcdlPassSettings[uPort]));
      uQknMidRange[uPort] = 8;
    } else {
      LOG_VERBOSE(BSL_LS_SOC_DDR,
                  (BSL_META_U(unit,
                               "Memory tests pass for %s QK_n VCDL, settings from %d to %d (pass vector=0x%08x), setting to mid-range %d\n"),
                   _getPortName(uPort), uQknLoPass[uPort], uQknHiPass[uPort], uQknVcdlPassSettings[uPort], uQknMidRange[uPort])); 
    }

    uPhyConfig1[uPort] = _SetQknVcdl (unit,uPort, uPhyConfig1[uPort], uQknMidRange[uPort]);
  }

  /* - Need to re-tune the X VCDL.  This is because mem_diag ignored the LS
   * - data bits when run during the nested loop, so that the QK_n VCDL did
   * - not have to get tuned.  (Otherwise would have been a 5-deep nested
   * - loop, instead of 4-deep.)  However, if the setup or hold time of data
   * - w.r.t. the rising edge of K is significantly worse than w.r.t. the
   * - rising edge of K_n, write errors could occur that will go undetected
   * - because the LS bits are ignored during reads.  This would would cause
   * - the range of passing X VCDL values and the resulting mid-point to be
   * - incorrect.
   */

  for (uXVcdl=0; uXVcdl<NUM_X_VCDL_SETTINGS; uXVcdl++) {
    for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
      uPhyConfig1[uPort] = _SetXVcdl(unit, uPort, uPhyConfig1[uPort], uXVcdl);
    }
    
    /* Test memory */
    uMemDiagResult = DDRMemDiag(sbh,feip,uMemMask, CC2_TEST_ALL_BITS, CC2_MEMDIAG_ALL_PHASES);

    for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
      if (((uMemDiagResult >> uPort) & 0x1) == 0 ) {  /* Passed */
        uXVcdlPassSettings[uPort] |= 0x1 << uXVcdl;
      }
    }
  }

  for (uPort=0; uPort<NUM_DDR_PORTS_PER_C2; uPort++) {
    uXMidRange[uPort] = _GetMidRange(uXVcdlPassSettings[uPort], &uXLoPass[uPort], &uXHiPass[uPort]);
    if (uXMidRange[uPort] == 0) {
      LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Tuning failure for %s X VCDL, pass vector=0x%08x\n"),
                 _getPortName(uPort), uXVcdlPassSettings[uPort]));
      uXMidRange[uPort] = 8;
    } else {
      LOG_VERBOSE(BSL_LS_SOC_DDR,
                  (BSL_META_U(unit,
                               "Memory tests pass for %s X VCDL, settings from %d to %d (pass vector=0x%08x), setting to mid-range %d\n"),
                   _getPortName(uPort), uXLoPass[uPort], uXHiPass[uPort], uXVcdlPassSettings[uPort], uXMidRange[uPort]));
    }

    uPhyConfig1[uPort] = _SetXVcdl (unit, uPort, uPhyConfig1[uPort], uXMidRange[uPort]);
  }  


  /* copy settings into init params structure */
  index = 0;
  for (i=0; i < SB_FE2000_NUM_MM_INSTANCES; i++) {
     for (j=0; j < SB_FE2000XT_DDR_PORTS_PER_MMU; j++) {
         feip->ddrconfig[i].uValidDelay[j] = uBestValidDelay[index];
         feip->ddrconfig[i].uPhaseSelect[j] = uBestPhaseSelect[index];
         feip->ddrconfig[i].uQkMidRange[j] = uQkMidRange[index];
         feip->ddrconfig[i].uQknMidRange[j] = uQknMidRange[index];
         feip->ddrconfig[i].uXMidRange[j] = uXMidRange[index];
         index++;
     }
  }

  /* Dump settings to console */
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "Add these variables to config.bcm or run ddrconfig to store to NV\n")));
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "bcm88025_mmu0_narrow0_config.%d=%d,%d,%d,%d,%d\n"),
               unit,uBestValidDelay[0],uBestPhaseSelect[0],uQkMidRange[0],uQknMidRange[0],uXMidRange[0]));
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "bcm88025_mmu0_narrow1_config.%d=%d,%d,%d,%d,%d\n"),
               unit,uBestValidDelay[1],uBestPhaseSelect[1],uQkMidRange[1],uQknMidRange[1],uXMidRange[1]));
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "bcm88025_mmu0_wide_config.%d=%d,%d,%d,%d,%d\n"),
               unit,uBestValidDelay[2],uBestPhaseSelect[2],uQkMidRange[2],uQknMidRange[2],uXMidRange[2]));
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "bcm88025_mmu1_narrow0_config.%d=%d,%d,%d,%d,%d\n"),
               unit,uBestValidDelay[3],uBestPhaseSelect[3],uQkMidRange[3],uQknMidRange[3],uXMidRange[3]));
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "bcm88025_mmu1_narrow1_config.%d=%d,%d,%d,%d,%d\n"),
               unit,uBestValidDelay[4],uBestPhaseSelect[4],uQkMidRange[4],uQknMidRange[4],uXMidRange[4]));
  LOG_VERBOSE(BSL_LS_SOC_DDR,
              (BSL_META_U(unit,
                          "bcm88025_mmu1_wide_config.%d=%d,%d,%d,%d,%d\n"),
               unit,uBestValidDelay[5],uBestPhaseSelect[5],uQkMidRange[5],uQknMidRange[5],uXMidRange[5]));

  /* Restore registers altered by MemDiag after it is done running. */
  DDRMemDiagCleanUp(sbh,feip); 
  SAND_HAL_WRITE( sbh, C2, MM0_PROT_SCHEME0, uMmSaveProtScheme0 );
  SAND_HAL_WRITE( sbh, C2, MM1_PROT_SCHEME0, uMmSaveProtScheme1 );

  return 0;
}

int
soc_sbx_fe2000_ddr_train(int unit)
{
    int32 nErrorsPerTap[8];
    int32 nBestTap;
    int32 nErrors;
    int32 i, nPort;
    uint32 uTap;
    uint32 narrow_addr_width[2], wide_addr_width;
    sbx_rdwr_t narrow_wr_f[2][2]; /* instance, port */
    sbx_rdwr_t narrow_rd_f[2][2];
    sbx_rdwr_t wide_wr_f[2];
    sbx_rdwr_t wide_rd_f[2];
    sbhandle sbh;

    /*   instance, port */
    narrow_wr_f[0][0] = wr_narrow0p0;
    narrow_wr_f[0][1] = wr_narrow0p1;
    narrow_wr_f[1][0] = wr_narrow1p0;
    narrow_wr_f[1][1] = wr_narrow1p1;

    narrow_rd_f[0][0] = rd_narrow0p0;
    narrow_rd_f[0][1] = rd_narrow0p1;
    narrow_rd_f[1][0] = rd_narrow1p0;
    narrow_rd_f[1][1] = rd_narrow1p1;

    wide_wr_f[0] = wr_wide0;
    wide_wr_f[1] = wr_wide1;
    wide_rd_f[0] = rd_wide0;
    wide_rd_f[1] = rd_wide1;

    sbh = SOC_SBX_CONTROL(unit)->sbhdl;

    for (i = 0; i<2; i++) {
        
        uint32 uMmDllDebug;

        if (i == 0) {
            narrow_addr_width[0] = SOC_SBX_CFG_FE2000(unit)->mm0n0awidth;
            narrow_addr_width[1] = SOC_SBX_CFG_FE2000(unit)->mm0n1awidth;
            wide_addr_width      = SOC_SBX_CFG_FE2000(unit)->mm0wawidth;
        } else {
            narrow_addr_width[0] = SOC_SBX_CFG_FE2000(unit)->mm1n0awidth;
            narrow_addr_width[1] = SOC_SBX_CFG_FE2000(unit)->mm1n1awidth;
            wide_addr_width      = SOC_SBX_CFG_FE2000(unit)->mm1wawidth;
        }

        if (SOC_SBX_CONTROL(unit)->fetype == SOC_SBX_FETYPE_FE2K) {
            /* each memory port can be trained individually */
            uMmDllDebug = SAND_HAL_READ_STRIDE(sbh, CA, MM, i, MM_DLL_DEBUG);

            for (nPort=0; nPort < 2; nPort++) {
                LOG_VERBOSE(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "Mm%dNarrowPort%d:"),
                             i, nPort));

                for (uTap = 0; uTap<8; uTap++) {
                    if (nPort == 0) {
                        uMmDllDebug = SAND_HAL_MOD_FIELD(CA, MM0_DLL_DEBUG, NARROW_PORT0_DLL_PHASE, uMmDllDebug, uTap);
                    } else {
                        uMmDllDebug = SAND_HAL_MOD_FIELD(CA, MM0_DLL_DEBUG, NARROW_PORT1_DLL_PHASE, uMmDllDebug, uTap);
                    }
                    SAND_HAL_WRITE_STRIDE(sbh, CA, MM, i, MM_DLL_DEBUG, uMmDllDebug);

                    nErrors = sbx_ddr_error_count(unit, 
                                                  narrow_wr_f[i][nPort], 
                                                  narrow_rd_f[i][nPort],
                                                  1,
                                                  narrow_addr_width[nPort]);

                    nErrorsPerTap[uTap] = nErrors;
#if 0
                    LOG_VERBOSE(BSL_LS_SOC_DDR,
                                (BSL_META_U(unit,
                                            "  Mm%dNarrowPort%d, dly tap=%d errors=%d\n"), 
                                 i, nPort, uTap, nErrors));
#endif
                }

                LOG_VERBOSE(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "\n")));
                nBestTap = sbx_get_best_delay_tap(nErrorsPerTap, 8);

                if (nPort == 0) {
                    uMmDllDebug = SAND_HAL_MOD_FIELD(CA, MM0_DLL_DEBUG, NARROW_PORT0_DLL_PHASE, uMmDllDebug, nBestTap);
                } else {
                    uMmDllDebug = SAND_HAL_MOD_FIELD(CA, MM0_DLL_DEBUG, NARROW_PORT1_DLL_PHASE, uMmDllDebug, nBestTap);
                }

                SAND_HAL_WRITE_STRIDE(sbh, CA, MM, i, MM_DLL_DEBUG, uMmDllDebug);

                LOG_VERBOSE(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "Best tap for Mm%dNarrowPort%dMemory = %d\n"),
                             i, nPort, nBestTap));
            }

            LOG_VERBOSE(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "Mm%dWide:"),
                         i));
            for (uTap = 0; uTap<8; uTap++) {
                uMmDllDebug = SAND_HAL_MOD_FIELD(CA, MM0_DLL_DEBUG, WIDE_PORT_DLL_PHASE, uMmDllDebug, uTap);
                SAND_HAL_WRITE_STRIDE(sbh, CA, MM, i, MM_DLL_DEBUG, uMmDllDebug);
                
                nErrors = sbx_ddr_error_count(unit, 
                                              wide_wr_f[i],
                                              wide_rd_f[i],
                                              2,
                                              wide_addr_width);
                
                nErrorsPerTap[uTap] = nErrors;
#if 0
                LOG_VERBOSE(BSL_LS_SOC_DDR,
                            (BSL_META_U(unit,
                                        "  Mm%dWide, dly tap=%d errors=%d\n"),
                             i, uTap, nErrors));
#endif
            }
            LOG_VERBOSE(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "\n")));
            nBestTap = sbx_get_best_delay_tap(nErrorsPerTap, 8);
            uMmDllDebug = SAND_HAL_MOD_FIELD(CA, MM0_DLL_DEBUG, WIDE_PORT_DLL_PHASE, uMmDllDebug, nBestTap);
            SAND_HAL_WRITE_STRIDE(sbh, CA, MM, i, MM_DLL_DEBUG, uMmDllDebug);
            LOG_VERBOSE(BSL_LS_SOC_DDR,
                        (BSL_META_U(unit,
                                    "Best tap for Mm%dWidePortMemory = %d\n"),
                         i, nBestTap));    
        }else{
            
            return SOC_E_PARAM;
        }
    }
    
    return SOC_E_NONE;
}


int32 sbx_get_best_delay_tap(int32 nErrorsPerTap[], int32 nTaps) 
{
#define MAX_TAPS   8
    /* nErrorsPerTap[] has error counts for each delay tap
     * This method looks for a band of contiguous taps with no error,
     *  and return the mid point of the band.
     */

    /* processed the results, get groups of contiguous band */
    uint32 delayMap[MAX_TAPS]; /* map{pos, count} */
    int32 nNumInMap = 0;
    int32 nDly;
    int32 nBestPos, nStartPos, nCount, nBestCount, nBestDly;

    nBestPos = 0;

    if (nTaps > MAX_TAPS) {
        LOG_WARN(BSL_LS_SOC_DDR,
                 (BSL_META("Error upto 8 taps supported\n")));
        nTaps = 8;
    }

    for(nDly = 0; nDly < MAX_TAPS; nDly++) {
        delayMap[nDly] = ~0;
    }

    nDly = 0;
    while (nDly < 8) {

        if (nErrorsPerTap[nDly] == 0) {
            /* find out the run length */
            int32 j;
            nStartPos = nDly;
            nCount = 0;

            for (j = nDly; j < MAX_TAPS; j++) {
                nCount++;
                nDly++;
                if ((nErrorsPerTap[nDly] != 0) ||
                    (nDly == MAX_TAPS)) break;
            }
            delayMap[nStartPos] = nCount;
            nNumInMap++;
        } else {
            nDly++;
        }
    }
  
    if (nNumInMap == 0) {
        /* look for tap with the least errors */
        int32 nMinErrors = nErrorsPerTap[0];
        nBestPos = 0;
        for (nDly = 1; nDly < MAX_TAPS; nDly++) {
            if (nErrorsPerTap[nDly] < nMinErrors) {
                nMinErrors = nErrorsPerTap[nDly];
                nBestPos = nDly;
            }
        }

        LOG_WARN(BSL_LS_SOC_DDR,
                 (BSL_META("All delay taps have errors, delay tap %d has the least errors=%d\n"),
                  nBestPos, nErrorsPerTap[nBestPos]));
        return nBestPos;
    }

    /* look for the biggest contiguous band of delay taps with no error */

    nBestCount = 0;    
    for(nDly=0; nDly<MAX_TAPS; nDly++) {
        if (delayMap[nDly] != ~0) {
            nStartPos = nDly;
            nCount = delayMap[nDly];
#if 0
            LOG_VERBOSE(BSL_LS_SOC_DDR,
                        (BSL_META(".. nStartPos = %d, nCount = %d\n"),
                         nStartPos, nCount));
#endif
            if (nBestCount < nCount) {
                nBestPos   = nStartPos;
                nBestCount = nCount;
#if 0
                LOG_VERBOSE(BSL_LS_SOC_DDR,
                            (BSL_META("... nBestPos = %d, nCount = %d\n"),
                             nBestPos, nBestCount));
#endif
            }
        }
    }
    nBestDly = nBestPos + (nBestCount - 1)/2;
    return nBestDly;
}


uint32
_rand(uint32 *seed) 
{
    uint32 nLo = 0x41a7 * (*seed & 0xFFFF);
    uint32 nHi = 0x41a7 * ((*seed >> 16) & 0xFFFF);
    
    nLo += (nHi & 0x7FFF) << 16;
    nLo += nHi >> 15;

    if (nLo > 0x7FFFFFFF) {
        nLo -= 0x7FFFFFF;
    }

    *seed = nLo;
    return nLo;
}

uint32
sbx_ddr_error_count(int32 unit, sbx_rdwr_t writef, sbx_rdwr_t readf, 
                    uint32 num_words, uint32 uAddressWidth) 
{
    uint32 addrSeed, dataSeed;
    uint32 uAddress, uAddrIncr;
    uint32 uWriteData[2], uReadData[2];
    int32 nStatus, nErrors, i, nNumAddrs;

    nErrors = 0;

    nNumAddrs = soc_property_get(unit, spn_DDR_TRAIN_NUM_ADDRS, 100);

    /* cut the address space into numAddrs equal sizes, then choose a single
     * random number in each segment.  
     */
    uAddrIncr = (1<<uAddressWidth)/nNumAddrs;

    addrSeed = soc_property_get(unit, spn_SEED, 0xa8e3);
    dataSeed = soc_property_get(unit, spn_SEED, 0xa8e3);

    /* write */
    for (i=0; i < nNumAddrs; i++) {
        uAddress = _rand(&addrSeed) % uAddrIncr; /* choose addr in segment */
        uAddress += i * uAddrIncr;               /* adjust to segment */

        uWriteData[0] = _rand(&dataSeed);
        uWriteData[1] = _rand(&dataSeed);
       
        nStatus = writef(unit, uAddress, uWriteData);
        COMPILER_REFERENCE(nStatus);

#if 0
        LOG_VERBOSE(BSL_LS_SOC_DDR,
                    (BSL_META("Write: Address=0x%08x data0=0x%08x data1=0x%08x status=%d\n"), 
                     uAddress, uWriteData[0], uWriteData[1], nStatus));
#endif
    }

    /* reset the random seeds to generate the same sequence */
    addrSeed = soc_property_get(unit, spn_SEED, 0xa8e3);
    dataSeed = soc_property_get(unit, spn_SEED, 0xa8e3);

    for (i=0; i < nNumAddrs; i++) {
        uAddress = _rand(&addrSeed) % uAddrIncr; /* choose addr in segment */
        uAddress += i * uAddrIncr;               /* adjust to segment */

        /* read /check */
        nStatus = readf(unit, uAddress, uReadData);

        uWriteData[0] = _rand(&dataSeed);
        uWriteData[1] = _rand(&dataSeed);

#if 0
        LOG_VERBOSE(BSL_LS_SOC_DDR,
                    (BSL_META("s=%d addr=0x%08x r0=0x%08x(0x%08x) r1=0x%08x(0x%08x) num_words=%d\n"),
                     nStatus, uAddress, uReadData[0], uWriteData[0], 
                     uReadData[1], uWriteData[1],
                     num_words));
#endif
        
        if (nStatus || 
            ((uReadData[0] != uWriteData[0]) || 
             (num_words == 2 && (uReadData[1] != uWriteData[1])))) {
            nErrors++;
        }
   
        if ( (i&0xf) == 0xf) {
            LOG_VERBOSE(BSL_LS_SOC_DDR,
                        (BSL_META(".")));
        }
    }

    return nErrors;
}



#define DDR_NARROW_WRITE(inst, port) \
    int32 nStatus; \
    SAND_HAL_FE2000_WRITE(unit, MM##inst##_NARROW_PORT##port##_MEM_ACC_DATA0, data[0]); \
    SOC_SBX_UTIL_FE2000_WRITE_REQUEST(unit, MM##inst##_NARROW_PORT##port##_MEM, addr); \
    SOC_SBX_UTIL_FE2000_WAIT_FOR_ACK(unit, MM##inst##_NARROW_PORT##port##_MEM, 100, nStatus); \
    return nStatus; \

int32 wr_narrow0p0(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_WRITE(0, 0);
}


int32 wr_narrow0p1(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_WRITE(0, 1);
}

int32 wr_narrow1p0(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_WRITE(1, 0);
}

int32 wr_narrow1p1(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_WRITE(1, 1);
}


#define DDR_NARROW_READ(inst, port) \
    int32 nStatus; \
    SOC_SBX_UTIL_FE2000_READ_REQUEST(unit, MM##inst##_NARROW_PORT##port##_MEM, addr); \
    SOC_SBX_UTIL_FE2000_WAIT_FOR_ACK(unit, MM##inst##_NARROW_PORT##port##_MEM, 100, nStatus); \
    data[0] = SAND_HAL_FE2000_READ(unit, MM##inst##_NARROW_PORT##port##_MEM_ACC_DATA0); \
    return nStatus; 

int32 rd_narrow0p0(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_READ(0, 0);
}

int32 rd_narrow0p1(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_READ(0, 1);
}

int32 rd_narrow1p0(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_READ(1, 0);
}

int32 rd_narrow1p1(int unit, uint32 addr, uint32 data[2])
{
    DDR_NARROW_READ(1, 1);
}


#define DDR_WIDE_WRITE(inst) \
    int32 nStatus; \
    SAND_HAL_FE2000_WRITE(unit, MM##inst##_WIDE_PORT_MEM_ACC_DATA0, data[0]); \
    SAND_HAL_FE2000_WRITE(unit, MM##inst##_WIDE_PORT_MEM_ACC_DATA1, data[1]); \
    SOC_SBX_UTIL_FE2000_WRITE_REQUEST(unit, MM##inst##_WIDE_PORT_MEM, addr); \
    SOC_SBX_UTIL_FE2000_WAIT_FOR_ACK(unit, MM##inst##_WIDE_PORT_MEM, 100, nStatus); \
    return nStatus; \

int32 wr_wide0(int unit, uint32 addr, uint32 data[2])
{
    DDR_WIDE_WRITE(0);
}
int32 wr_wide1(int unit, uint32 addr, uint32 data[2])
{
    DDR_WIDE_WRITE(1);
}


#define DDR_WIDE_READ(inst ) \
    int32 nStatus; \
    SOC_SBX_UTIL_FE2000_READ_REQUEST(unit, MM##inst##_WIDE_PORT_MEM, addr); \
    SOC_SBX_UTIL_FE2000_WAIT_FOR_ACK(unit, MM##inst##_WIDE_PORT_MEM, 100, nStatus); \
    data[0] = SAND_HAL_FE2000_READ(unit, MM##inst##_WIDE_PORT_MEM_ACC_DATA0); \
    data[1] = SAND_HAL_FE2000_READ(unit, MM##inst##_WIDE_PORT_MEM_ACC_DATA1); \
    return nStatus; 

int32 rd_wide0(int unit, uint32 addr, uint32 data[2])
{
    DDR_WIDE_READ(0);
}
int32 rd_wide1(int unit, uint32 addr, uint32 data[2])
{
    DDR_WIDE_READ(1);
}
#endif /* BCM_FE2000_SUPPORT */
