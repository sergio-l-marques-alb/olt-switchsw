/* $Id: petra_diagnostics.c,v 1.13 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_diagnostics.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_diagnostics.h>
#include <soc/dpp/Petra/petra_api_diagnostics.h>
#include <soc/dpp/Petra/petra_header_parsing_utils.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_scheduler_ports.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>
#include <soc/dpp/Petra/petra_scheduler_flows.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_egr_queuing.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_api_packet.h>
#include <soc/dpp/Petra/petra_api_debug.h>
#include <soc/dpp/Petra/petra_packet.h>
#include <soc/dpp/Petra/petra_statistics.h>
#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_debug.h>
#include <soc/dpp/Petra/petra_init.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_diagnostics.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

typedef struct {
  SOC_PETRA_MODULE_ID module_id;
  char *name;
  uint32 start_address;
  uint32 last_address;
  uint16 nof_bits;
} SOC_PETRA_DIAG_DEV_TBL;

#ifndef SAND_LOW_LEVEL_SIMULATION
static SOC_PETRA_DIAG_DEV_TBL dev_tbls[] = {
  {SOC_PETRA_IDR_ID,"IDR:Reassembly F",0x000a0000,0x000a01ff,28},
  {SOC_PETRA_IDR_ID,"IDR:Pcb Link Table",0x000b0000,0x000b01ff,13},
  {SOC_PETRA_IDR_ID,"IDR:Chunk Status",0x000c0000,0x000c004f,50},
  {SOC_PETRA_IDR_ID,"IDR:Word Index",0x000d0000,0x000d00ff,12},
  {SOC_PETRA_IRR_ID,"IDR:Glag To Lag Range",0x000f0000,0x000f00ff,4},
  {SOC_PETRA_IHP_ID,"IDR:Port Info",0x000e0000,0x000e004f,39},
  {SOC_PETRA_IQM_ID,"IQM:Tx Descriptor Fifos Memory",0x00a00000,0x00a000ff,85},
  {SOC_PETRA_IQM_ID,"IQM:VSQ Average Qsize memory - group B",0x01a00000,0x01a0001f,12},
  {SOC_PETRA_IQM_ID,"IQM:VSQ Average Qsize memory - group C",0x01b00000,0x01b0003f,12},
  {SOC_PETRA_IQM_ID,"IQM:VSQ Average Qsize memory - group D",0x01c00000,0x01c000ff,12},
  {SOC_PETRA_IQM_ID,"IQM:VSQ Flow-Control Parameters table - group A",0x01d00000,0x01d0000f,46},
  {SOC_PETRA_IQM_ID,"IQM:VSQ Flow-Control Parameters table - group B",0x01e00000,0x01e0000f,46},
  {SOC_PETRA_IQM_ID,"IQM:VSQ Flow-Control Parameters table - group C",0x01f00000,0x01f0000f,46},
  {SOC_PETRA_QDR_ID,"QDR:Qdr Dll Mem",0x20000000,0x2000007f,28},
  {SOC_PETRA_IPS_ID,"IPS:Fsmrq Fifo Memory",0x00068000,0x000687ff,15},
  {SOC_PETRA_IPS_ID,"IPS:Time Stamp Fifo Memory",0x00088000,0x000880ff,13},
  {SOC_PETRA_IPT_ID,"IPT:Select Source Sum",0x000a0000,0x000a0000,47},
  {SOC_PETRA_EGQ_ID,"EGQ:Egress Shaper Recycling Ports Credit Configuration (Rcy Scm)",0x000a0000,0x000a00ff,25},
  {SOC_PETRA_EGQ_ID,"EGQ:Egress Shaper CPUPorts Credit Configuration(Cpu Scm)",0x000b0000,0x000b00ff,25},
  {SOC_PETRA_EGQ_ID,"EGQ:Egress Shaper Calendar Selector (CCM)",0x000c0000,0x000c00ff,4},
  {SOC_PETRA_EGQ_ID,"EGQ:Per Port Max Credit Memory (PMC)",0x000d0000,0x000d009f,16},
  {SOC_PETRA_EGQ_ID,"EGQ:Egress Shaper Per Port Credit Balance Memory (CBM)",0x000e0000,0x000e004f,25},
  {SOC_PETRA_EGQ_ID,"EGQ:Free Buffesr Allocation Bitmap Memory (FBM)",0x000f0000,0x000f007f,32},
  {SOC_PETRA_EGQ_ID,"EGQ:Nifb Flow Control",0x001a0000,0x001a0000,16},
  {SOC_PETRA_EGQ_ID,"EGQ:Cpu Last Header",0x001b0000,0x001b0000,64},
  {SOC_PETRA_EGQ_ID,"EGQ:Ipt Last Header",0x001c0000,0x001c0000,64},
  {SOC_PETRA_EGQ_ID,"EGQ:Fdr Last Header",0x001d0000,0x001d0000,64},
  {SOC_PETRA_EGQ_ID,"EGQ:Cpu Packet Counter",0x001e0000,0x001e0000,33},
  {SOC_PETRA_EGQ_ID,"EGQ:Ipt Packet Counter",0x001f0000,0x001f0000,33},
  {SOC_PETRA_EGQ_ID,"EGQ:Pqp Multicast Low Packet Counter",0x002a0000,0x002a0000,33},
  {SOC_PETRA_EGQ_ID,"EGQ:Pqp Unicast High Bytes Counter",0x002b0000,0x002b0000,47},
  {SOC_PETRA_EGQ_ID,"EGQ:Pqp Unicast Low Bytes Counter",0x002c0000,0x002c0000,47},
  {SOC_PETRA_EGQ_ID,"EGQ:Pqp Multicast High Bytes Counter",0x002d0000,0x002d0000,47},
  {SOC_PETRA_EGQ_ID,"EGQ:Pqp Multicast Low Bytes Counter",0x002e0000,0x002e0000,47},
  {SOC_PETRA_EGQ_ID,"EGQ:Pqp Discard Unicast Packet Counter",0x002f0000,0x002f0000,33},
  {SOC_PETRA_SCH_ID,"SCH:CL-Schedulers Type (SCT)",0x400a0000,0x400a00ff,48},
  {SOC_PETRA_SCH_ID,"SCH:Flow to Queue Mapping (FQM)",0x400b0000,0x400b3fff,18},
  {SOC_PETRA_SCH_ID,"SCH:Flow to FIP Mapping (FFM)",0x400c0000,0x400c1fff,11},
  {SOC_PETRA_SCH_ID,"SCH:Scheduler Active Data Base (SAD)",0x401a0000,0x401a017f,128},
  {SOC_PETRA_SCH_ID,"SCH:Shaper Installed Memory (SIM)",0x401b0000,0x401b0000,16},
  {SOC_PETRA_SCH_ID,"SCH:Dynamic Port2 NIF Mapping (DPN)",0x401c0000,0x401c0000,4},
  {SOC_PETRA_SCH_ID,"SCH:Port Queue Size (PQS)",0x401d0000,0x401d0050,21}};
#endif
















#define SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE                 (0x1000)
#define SOC_PETRA_DIAG_TBLS_DUMP_MAX_WORD_SIZE                     (10)

#define SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF_VAL                  (0xAAAAAAAA)
#define SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO_VAL                  (0x0)
#define SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE_VAL                   (0xFFFFFFFF)

#define SOC_PETRA_DIAGNOSTICS_COUNT_TYPE_MAX                       (SOC_PETRA_DIAG_NOF_SOFT_COUNT_TYPES-1)
#define SOC_PETRA_DIAGNOSTICS_PATTERN_MAX                          (SOC_TMC_NOF_DIAG_SOFT_ERROR_PATTERNS-1)
#define SOC_PETRA_DIAGNOSTICS_SMS_MIN                              (1)
#define SOC_PETRA_DIAGNOSTICS_SMS_MAX                              (SOC_PETRA_DIAG_NOF_SMS)
#define SOC_PETRA_DIAGNOSTICS_ERR_SP_MAX                           (SOC_SAND_U32_MAX)
#define SOC_PETRA_DIAGNOSTICS_ERR_DP_MAX                           (SOC_SAND_U32_MAX)
#define SOC_PETRA_DIAGNOSTICS_ERR_RF_MAX                           (SOC_SAND_U32_MAX)
/* $Id: petra_diagnostics.c,v 1.13 Broadcom SDK $
 *  Note: this is just the 18 MSB, completed to 36
 *  by concatenating 18-bit '0' as LSB
 */
#define SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1_VAL                    (0x1E000)
/*
 *  Note: this is just the 18 MSB, completed to 36
 *  by concatenating 18-bit '0' as LSB
 */
#define SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2_VAL                    (0x9800)

#define SOC_PETRA_DIAG_BIST_DATA_PATTERN_NOF_REGS                  (8)
#define SOC_PETRA_DIAG_BIST_READ_DELAY                             (30)

/*
 *  BIST - waiting time
 */
#define SOC_PETRA_DIAG_DRAM_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_DIAG_DRAM_TIMER_ITERATIONS       50
#define SOC_PETRA_DIAG_DRAM_TIMER_DELAY_MSEC       20


#define SOC_PETRA_DIAG_MAX_NOF_DUMP_TABLES 400

#define SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_TEST_BIST_START_ADDR (0x0)
#define SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_TEST_BIST_END_ADDR   (0x1fffcc)
#define SOC_PETRA_DIAG_QDR_DLL_MEMORY_TBL_NOF_ENTRIES           (128)
#define SOC_PETRA_DIAG_QDR_NOF_FAILURES_FOR_WINDOW_END          (3)

/*
 * Whether to print the name of the table.
 * this option to enable saving place in the object file.
 * (the address and block name of the table always will be printed)
 */
#define SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT 1

#define SOC_PETRA_DIAG_LBG_SLEEP_NANO       100000000


/*
 *  The ITMH header location in the last packet header
 *  Different places if it is Soc_petra-A or Soc_petra-B
 */
#define SOC_PA_DIAG_ITMH_BIT_START (48)

#ifdef LINK_PB_LIBRARIES
#define SOC_PB_DIAG_ITMH_BIT_START (992)
#else
#define SOC_PB_DIAG_ITMH_BIT_START SOC_PA_DIAG_ITMH_BIT_START
#endif

#define SOC_PETRA_DIAG_ITMH_BIT_START \
  SOC_PETRA_CST_VALUE_DISTINCT(DIAG_ITMH_BIT_START, uint32)

#define SOC_PETRA_DIAG_ITMH_SIZE_IN_BITS     (32)


#define SOC_PETRA_DIAG_HEADER_SIZE_IN_UINT32S \
  SOC_PETRA_CST_VALUE_DISTINCT(NOF_DEBUG_HEADER_REGS, uint32)

/* 3 for Soc_petra-A, 32 for Soc_petra-B */
#define SOC_PETRA_DIAG_HEADER_SIZE_IN_UINT32S_MAX (32)
/*
 *  The training sequence lock status
 */
#define SOC_PETRA_DIAG_TRAINING_NOF_PATTERNS        (2)
#define SOC_PETRA_DIAG_TRAINING_MSTR_TH_MIN         (2)
#define SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT    (8)
#define SOC_PETRA_DIAG_TRAINING_RTT_MAX             (25)
#define SOC_PETRA_DIAG_TRAINING_RTT_MIN             (5)
#define SOC_PETRA_DIAG_TRAINING_RTT_DIFF_ABS_TH     (4)
#define SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD   (32)

#define SOC_PETRA_DIAG_DRAM_NDX_MAX                 (SOC_PETRA_BLK_NOF_INSTANCES_DPI)


/*
 * Polling for the Soft error test
 */
#define SOC_PETRA_DIAG_MBIST_POLL_ITERS                  10000

/*
 *  SMS - memory correction
 */
#define SOC_PETRA_DIAG_SMS_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_DIAG_SMS_TIMER_ITERATIONS       10
#define SOC_PETRA_DIAG_SMS_TIMER_DELAY_MSEC       50

#define SOC_PA_DIAG_SMS_READY_POLL_VAL             (0x7fffffff)

#ifdef LINK_PB_LIBRARIES
  #define SOC_PB_DIAG_SMS_READY_POLL_VAL             (0xffffffff)
#else
  #define SOC_PB_DIAG_SMS_READY_POLL_VAL             SOC_PA_DIAG_SMS_READY_POLL_VAL
#endif

#define SOC_PETRA_DIAG_SMS_READY_POLL_VAL \
  SOC_PETRA_CST_VALUE_DISTINCT(DIAG_SMS_READY_POLL_VAL, uint32)


/* } */

/*************
 *  MACROS   *
 *************/
/* { */


#define SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(width_bits)                           \
          SOC_SAND_DIV_ROUND_UP(width_bits, SOC_SAND_NOF_BITS_IN_UINT32) * sizeof(uint32)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct{
  uint32      base;
  uint32      size;
  uint32     wrd_sz;
  uint32     mod_id;
  const char    *name;
}SOC_PETRA_TBL_PRINT_INFO;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_petra_diag_ipt_rate_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_SAND_64CNT               *rate
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL_DATA
    data;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_SAND_64CNT
    int_rate;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(rate);

  res = soc_petra_ipt_select_source_sum_get_unsafe(
          unit,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_REG_SET(regs->ipt.gtimer_config_cont_reg, 0x1, 20, exit);
  SOC_PETRA_REG_SET(regs->ipt.gtimer_config_cont_reg, 0x3, 22, exit);

  sal_msleep(1120);

  res = soc_petra_ipt_select_source_sum_get_unsafe(
          unit,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
   
  int_rate.u64.arr[0] = data.select_source_sum[0];
  int_rate.u64.arr[1] = data.select_source_sum[1];
  
  soc_sand_u64_devide_u64_long(&(int_rate.u64), 1024, &(int_rate.u64));
  soc_sand_u64_multiply_longs(int_rate.u64.arr[0], SOC_SAND_NOF_BITS_IN_CHAR, &(rate->u64));
  rate->overflowed = FALSE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_ipt_rate_get()",0,0);
}

uint32
  soc_petra_diag_iddr_set_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_IN  uint32     *data,
    SOC_SAND_IN  uint32     size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank,
    offset = 0,
    accesses_i = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PETRA_TBLS
    *tables = soc_petra_tbls();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_IDDR_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_PETRA_FLD_IGET(regs->drc.dram_compliance_configuration_reg.enable8_banks, bank, dram_ndx, 10, exit);

  if (bank)
  {
    offset |= SOC_SAND_SET_FLD_IN_PLACE(bank_ndx, 0, SOC_SAND_BITS_MASK(2, 0));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(dram_ndx, 3, SOC_SAND_BITS_MASK(4, 3));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(SOC_PETRA_TBL(tables->mmu.dram_address_space_tbl.addr.base) + dram_offset, 5, SOC_SAND_BITS_MASK(30, 5));
    offset &= SOC_SAND_BITS_MASK(30, 0);
  }
  else
  {
    offset |= SOC_SAND_SET_FLD_IN_PLACE(bank_ndx, 0, SOC_SAND_BITS_MASK(1, 0));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(dram_ndx, 2, SOC_SAND_BITS_MASK(3, 2));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(SOC_PETRA_TBL(tables->mmu.dram_address_space_tbl.addr.base) + dram_offset, 4, SOC_SAND_BITS_MASK(30, 4));
    offset &= SOC_SAND_BITS_MASK(30, 0);
  }

  /* Verify indirect trigger is ZERO. */
  res = soc_sand_trigger_verify_0(
          unit,
          SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.addr.base),
          SOC_SAND_TRIGGER_TIMEOUT,
          SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  for(accesses_i = 0; accesses_i < (size / SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S); ++accesses_i)
  {
    res = soc_petra_write_reg_buffer_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->mmu.indirect_command_wr_data_reg_0.addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S,
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    SOC_PETRA_REG_SET(regs->mmu.indirect_command_address_reg, offset, 22, exit);

    res = soc_sand_trigger_assert_1(
            unit,
            SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.addr.base),
            SOC_SAND_TRIGGER_TIMEOUT,
            SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.lsb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    data += SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_iddr_set_unsafe()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_diag_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  soc_petra_diag_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    reg_val,
    dram_ndx,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_REGS_INIT);

  regs = soc_petra_regs();

  /*
   *  BIST
   */

  /*
   *  Activate the independent R/W address mode
   */
  for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++dram_ndx)
  {
    SOC_PETRA_REG_IGET(regs->drc.bist_configurations_reg, reg_val, dram_ndx, 5 ,exit);
    fld_val = 0x0;
    SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.bist_en, fld_val, dram_ndx, 30, exit);
    SOC_PETRA_REG_ISET(regs->drc.bist_configurations_reg, reg_val, dram_ndx, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_diag_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_diag_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    cntr_idx;
  SOC_PETRA_STAT_IFP_SELECT_INFO
    ifp_select_info;
  SOC_PETRA_STAT_VOQ_SELECT_INFO
    voq_select_info;
  SOC_PETRA_STAT_VSQ_SELECT_INFO
    vsq_select_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_INIT);

  SOC_PETRA_STAT_IFP_SELECT_INFO_clear(&ifp_select_info);
  SOC_PETRA_STAT_VOQ_SELECT_INFO_clear(&voq_select_info);
  SOC_PETRA_STAT_VSQ_SELECT_INFO_clear(&vsq_select_info);

  res = soc_petra_diag_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (cntr_idx = 0; cntr_idx < SOC_PETRA_STAT_PER_IFP_NOF_CNTS; cntr_idx++)
  {
    ifp_select_info.ifp_id[cntr_idx] = cntr_idx;
  }

  /*
   *  Set IFP Programmable counters 0-3 to ports 0-3
   */
  res = soc_petra_stat_ifp_cnt_select_set_unsafe(
          unit,
          &ifp_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Set VOQ Programmable counter to the last VOQ (disable for typical traffic)
   */
  voq_select_info.voq_id = SOC_PETRA_MAX_QUEUE_ID;
  res = soc_petra_stat_voq_cnt_select_set(
          unit,
          &voq_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *  Set VSQ Programmable counter to STAG 255 (disable for typical traffic)
   */
  vsq_select_info.vsq_grp_id    = SOC_PETRA_ITM_VSQ_GROUP_STTSTCS_TAG;
  vsq_select_info.vsq_in_grp_id = SOC_PETRA_ITM_VSQ_GROUPD_SZE - 1;
  res = soc_petra_stat_vsq_cnt_select_set(
          unit,
          &vsq_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_init()",0,0);
}

/*********************************************************************
*     Read the last packet header entered the NIF and return
*     the values of the ingress ITMH header fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_diag_last_packet_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *last_incoming_header
  )
{
  uint32
    last_packet_first_bits[SOC_PETRA_DIAG_HEADER_SIZE_IN_UINT32S_MAX],
    to_unlock_registers = 0,
    res;
  SOC_PETRA_HPU_ITMH_HDR
    itmh;
  SOC_PETRA_REGS
    *regs;
  uint32
    reg_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_DIAG_LAST_PACKET_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(last_incoming_header);
  soc_petra_PETRA_HPU_ITMH_HDR_clear(&itmh);

  regs = soc_petra_regs();

  /*
   * Retrieve the ITMH header snapshot
   * Unlock these registers after reading them for the next packet
   */
   
  for (reg_ndx = 0; reg_ndx < SOC_PETRA_DIAG_HEADER_SIZE_IN_UINT32S; reg_ndx++)
  {
  /* * COVERITY * 
   * after checking overun was not found. */
   /* coverity[overrun-local] */
    SOC_PETRA_REG_GET(regs->ihp.debug_header_reg[reg_ndx], last_packet_first_bits[reg_ndx], 10, exit);
  }

  /* Unlock these last registers */

  SOC_PETRA_REG_GET(regs->ihp.debug_port_type_reg, to_unlock_registers, 35, exit);

  /*
   * Construct the ITMH from the retrieved data -
   *  assumption of no previous headers
   */
  res = soc_sand_bitstream_get_any_field(
          last_packet_first_bits,
          SOC_PETRA_DIAG_ITMH_BIT_START,
          SOC_PETRA_DIAG_ITMH_SIZE_IN_BITS,
          &(itmh.base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  itmh.extention_src_port = 0;

  res = soc_petra_hpu_itmh_parse(
          &itmh,
          last_incoming_header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_diag_last_packet_get_unsafe()",0,0);
}

/*********************************************************************
*     This function is used as a diagnostics tool that gives
*     indications about the dll status.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dll_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dll_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *dll_stt_info
  )
{
  uint32
    fld_val,
    fld_val2,
    dll_stat_lsb,
    dll_stat_msb,
    dll_ctrl,
    init_status,
    ddio_rnd_rtp,
    ddio_rnd_rtp_diff,
    min_sv,
    max_sv_7_lsb,
    max_sv_1_msb,
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DLL_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dll_stt_info);

  /************************************************************************/
  /* Pref-mode set to predefined                                          */
  /************************************************************************/
  fld_val = 0x1;
  SOC_PETRA_FLD_ISET(regs->dpi.ddl_ctrl_reg_1[dll_ndx].stat_predef_mode, fld_val, dram_ndx, 10, exit);

  SOC_PETRA_REG_IGET(regs->dpi.ddl1_stat_lsb_reg[dll_ndx], dll_stat_lsb, dram_ndx, 20, exit);
  SOC_PETRA_REG_IGET(regs->dpi.ddl1_stat_msb_reg[dll_ndx], dll_stat_msb, dram_ndx, 30, exit);

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].phsel_error, fld_val, dll_stat_lsb, 40, exit);
  fld_val2 = (fld_val << 3);
  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].dly_maxmin_mode, fld_val, dll_stat_lsb, 50, exit);
  fld_val2 += (fld_val << 2);
  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].ph_sel, fld_val, dll_stat_lsb, 60, exit);
  fld_val2 += fld_val;
  dll_stt_info->dll_ph_dn = (uint8) fld_val2;
  /* dll_stt_info->dll_ph_dn has 4 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].main_ph_sel, fld_val, dll_stat_lsb, 70, exit);
  dll_stt_info->main_ph_sel = (uint8) fld_val;
  /* dll_stt_info->main_ph_sel has 2 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].ph2_sel, fld_val, dll_stat_lsb, 80, exit);
  dll_stt_info->ph2sel = (uint8) fld_val;
  /* dll_stt_info->ph2sel has 2 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].hc_sel_vec, fld_val, dll_stat_lsb, 90, exit);
  dll_stt_info->hc_sel_vec = (uint8) fld_val;
  /* dll_stt_info->hc_sel_vec has 8 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].sel_vec, fld_val, dll_stat_lsb, 100, exit);
  dll_stt_info->sel_vec = (uint8) fld_val;
  /* dll_stt_info->sel_vec has 8 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].sel_hg, fld_val, dll_stat_lsb, 110, exit);
  dll_stt_info->sel_hg = SOC_SAND_NUM2BOOL(fld_val);
  /* dll_stt_info->sel_hg has 1 significant bit */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_lsb_reg[0].qc_sel_vec_0, fld_val, dll_stat_lsb, 120, exit);
  fld_val2 = fld_val;
  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_msb_reg[0].qc_sel_vec_1, fld_val, dll_stat_msb, 130, exit);
  fld_val2 += (fld_val << 7);
  dll_stt_info->qc_sel_vec = (uint8) fld_val2;
  /* dll_stt_info->qc_sel_vec has 8 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_msb_reg[0].ph_up, fld_val, dll_stat_msb, 140, exit);
  dll_stt_info->dll_ph_up = (uint8) fld_val;
  /* dll_stt_info->dll_ph_up has 4 significant bits */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_msb_reg[0].phsel_hc_up, fld_val, dll_stat_msb, 150, exit);
  dll_stt_info->ph_sel_hc_up = SOC_SAND_NUM2BOOL(fld_val);
  /* dll_stt_info->ph_sel_hc_up has 1 significant bit */

  SOC_PETRA_FLD_FROM_REG(regs->dpi.ddl1_stat_msb_reg[0].ins_dly_min_vec, fld_val, dll_stat_msb, 160, exit);
  dll_stt_info->ins_dly_min_vec = (uint8) fld_val;
  /* dll_stt_info->ins_dly_min_vec has 6 significant bits */

  SOC_PETRA_FLD_ISET(regs->dpi.ddl_ctrl_reg_1[dll_ndx].dly_maxmin_mode, 0x1, dram_ndx, 36, exit);

  /************************************************************************/
  /* Pref-mode set to normal                                              */
  /************************************************************************/
  fld_val = 0x0;
  SOC_PETRA_FLD_ISET(regs->dpi.ddl_ctrl_reg_1[dll_ndx].stat_predef_mode, fld_val, dram_ndx, 170, exit);

  SOC_PETRA_REG_IGET(regs->dpi.ddl1_stat_lsb_reg[dll_ndx], dll_stat_lsb, dram_ndx, 180, exit);
  SOC_PETRA_REG_IGET(regs->dpi.ddl1_stat_msb_reg[dll_ndx], dll_stat_msb, dram_ndx, 190, exit);

  fld_val = SOC_SAND_GET_BIT(dll_stat_msb, 5);
  dll_stt_info->ddl_init_main_ph_sel_ofst = SOC_SAND_NUM2BOOL(fld_val);

  fld_val = SOC_SAND_GET_BIT(dll_stat_msb, 4);
  dll_stt_info->ddl_ph_sel_hc_up = SOC_SAND_NUM2BOOL(fld_val);

  fld_val = SOC_SAND_GET_BIT(dll_stat_msb, 3);
  dll_stt_info->ddl_train_trig_up_limit = SOC_SAND_NUM2BOOL(fld_val);

  fld_val = SOC_SAND_GET_BIT(dll_stat_msb, 2);
  dll_stt_info->ddl_train_trig_dn_limit = SOC_SAND_NUM2BOOL(fld_val);

  fld_val = SOC_SAND_GET_BIT(dll_stat_lsb, 7);
  dll_stt_info->ph_sel_err = SOC_SAND_NUM2BOOL(fld_val);

  fld_val = SOC_SAND_GET_BIT(dll_stat_lsb, 6);
  dll_stt_info->dly_max_min_mode = SOC_SAND_NUM2BOOL(fld_val);

  fld_val = SOC_SAND_GET_BITS_RANGE(dll_stat_lsb, 1, 0);
  dll_stt_info->ph_sel = (uint8) fld_val;
  /* dll_stt_info->ph_sel has 2 significant bits */

  SOC_PETRA_REG_IGET(regs->dpi.dpi_rnd_trp_status_reg, ddio_rnd_rtp, dram_ndx, 200, exit);
  SOC_PETRA_FLD_FROM_REG(regs->dpi.dpi_rnd_trp_status_reg.rnd_trp[dll_ndx], fld_val, ddio_rnd_rtp, 220, exit);
  dll_stt_info->rnd_trp = (uint8) fld_val;
  /* dll_stt_info->rnd_trp has 6 significant bits */

  SOC_PETRA_REG_IGET(regs->dpi.dpi_rnd_trp_diff_status_reg, ddio_rnd_rtp_diff, dram_ndx, 210, exit);
  SOC_PETRA_FLD_FROM_REG(regs->dpi.dpi_rnd_trp_diff_status_reg.rnd_trp_diff[dll_ndx], fld_val, ddio_rnd_rtp_diff, 230, exit);
  dll_stt_info->rnd_trp_diff = (uint8) fld_val;
  /* dll_stt_info->rnd_trp_diff has 6 significant bits */

  SOC_PETRA_REG_IGET(regs->dpi.dpi_init_status_reg, init_status, dram_ndx, 240, exit);
  fld_val = SOC_SAND_GET_BIT(init_status, 1+dll_ndx);
  dll_stt_info->dll_init_done = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PETRA_REG_IGET(regs->dpi.ddl_ctrl_reg_0[dll_ndx], dll_ctrl, dram_ndx, 250, exit);
  dll_stt_info->ddl_control_0 = dll_ctrl;
  SOC_PETRA_REG_IGET(regs->dpi.ddl_ctrl_reg_1[dll_ndx], dll_ctrl, dram_ndx, 260, exit);
  dll_stt_info->ddl_control_1 = dll_ctrl;
  SOC_PETRA_REG_IGET(regs->dpi.ddl_ctrl_reg_2[dll_ndx], dll_ctrl, dram_ndx, 270, exit);
  dll_stt_info->ddl_control_2 = dll_ctrl;

  SOC_PETRA_FLD_IGET(regs->dpi.ddl1_stat_lsb_reg[dll_ndx].hc_sel_vec, min_sv, dram_ndx, 37, exit);
  dll_stt_info->min_sel_vec = (uint8)min_sv;

  SOC_PETRA_FLD_IGET(regs->dpi.ddl1_stat_lsb_reg[dll_ndx].qc_sel_vec_0, max_sv_7_lsb, dram_ndx, 38, exit);
  SOC_PETRA_FLD_IGET(regs->dpi.ddl1_stat_msb_reg[dll_ndx].qc_sel_vec_1, max_sv_1_msb, dram_ndx, 39, exit);
  dll_stt_info->max_sel_vec = (uint8)((max_sv_7_lsb + (max_sv_1_msb << 7)));

  SOC_PETRA_FLD_ISET(regs->dpi.ddl_ctrl_reg_1[dll_ndx].dly_maxmin_mode, 0x0, dram_ndx, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dll_status_get_unsafe()",0,0);
}

/*********************************************************************
*     Get DRAM diagnostic.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dram_status_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *dram_status
  )
{
  uint32
    idx,
    idx2,
    dll_ndx,
    dll_ram_ndx,
    tr_seq,
    io_cal,
    init_status,
    per_tr,
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PETRA_DIAG_DLL_STATUS_INFO
    dll_status;
  SOC_PETRA_DPI_DLL_RAM_TBL_DATA
    dpi_dll_ram_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dram_status);
  SOC_SAND_ERR_IF_ABOVE_MAX(dram_ndx, SOC_PETRA_BLK_NOF_INSTANCES_DPI, SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 5, exit);

  soc_petra_PETRA_DIAG_DLL_STATUS_INFO_clear(&(dll_status));

  SOC_PETRA_REG_IGET(regs->drc.training_sequence_reg, tr_seq, dram_ndx, 10, exit);
  dram_status->training_seq = tr_seq;

  SOC_PETRA_REG_IGET(regs->dpi.io_calibration_status_reg, io_cal, dram_ndx, 20, exit);
  dram_status->calibration_st = io_cal;

  SOC_PETRA_REG_IGET(regs->dpi.ddl_periodic_training_reg, per_tr, dram_ndx, 30, exit);
  dram_status->ddl_periodic_training = per_tr;

  SOC_PETRA_REG_IGET(regs->dpi.dpi_init_status_reg, init_status, dram_ndx, 34, exit);
  dram_status->dll_mstr_s = (uint8) SOC_SAND_GET_BITS_RANGE(init_status, 11, 5);

  /* dram_status->dll_mstr_s has 7 significant bits */

  for (idx = 0; idx < SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS; ++idx)
  {
    idx2 = SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS - idx - 1;
    SOC_PETRA_REG_IGET(regs->drc.training_sequence_word_reg[idx2], tr_seq, dram_ndx, 41, exit);
    dram_status->ddr_training_sequence[idx] = tr_seq;
  }

  for (dll_ndx = 0; dll_ndx < SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM; ++dll_ndx)
  {
    res = soc_petra_diag_dll_status_get_unsafe(
            unit,
            dram_ndx,
            dll_ndx,
            &(dram_status->dll_status[dll_ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  for (dll_ram_ndx = 0; dll_ram_ndx < SOC_PETRA_DRAM_DLL_RAM_TABLE_SIZE; ++dll_ram_ndx)
  {
    res = soc_petra_dpi_dll_ram_tbl_get_unsafe(
            unit,
            dll_ram_ndx,
            dram_ndx,
            &dpi_dll_ram_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    dram_status->dll_ram_tbl_data[dll_ndx].dll_ram[0] = dpi_dll_ram_tbl_data.dll_ram[0];
    dram_status->dll_ram_tbl_data[dll_ndx].dll_ram[1] = dpi_dll_ram_tbl_data.dll_ram[1];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_status_get_unsafe()",dram_ndx,0);
}

/*********************************************************************
*     Get diagnostic for the DRAM interface training sequence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_diagnostic_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO                  *error_info
  )
{
  uint32
    fld_val,
    reg_val,
    dll_ndx,
    dqs_ndx,
    pat_ndx = 0,
    pat_vec_start,
    mstr_vec,
    bit_num = 0,
    bit = 0,
    word_ndx,
    rtt = 0,
    pat,
    pat_vec,
    pat_vec2,
    pati,
    rtt_arr[SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD],
    res;
  uint8
    bit_err[SOC_PETRA_DIAG_TRAINING_NOF_PATTERNS][SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD];
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(error_info);
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_clear(error_info);
  for (pat_ndx = 0; pat_ndx < SOC_PETRA_DIAG_TRAINING_NOF_PATTERNS; ++pat_ndx)
  {
    for (bit = 0; bit < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD; ++bit)
    {
      bit_err[pat_ndx][bit] = FALSE;
    }
  }

  for (pat_ndx = 0; pat_ndx < SOC_PETRA_DIAG_TRAINING_NOF_PATTERNS; ++pat_ndx)
  {
    pat = SOC_PETRA_DIAG_TRAINING_NOF_PATTERNS - pat_ndx - 1;
    if (pat == 0x0)
    {
      pati = 0x1;
      pat_vec_start = 0x0;
    }
    else
    {
      pati = 0x0;
      pat_vec_start = 0xFFFFFFFF;
    }

    /*
     *  Disable auto training
     */
    reg_val = 0x1;
    SOC_PETRA_REG_ISET(regs->dpi.ddl_periodic_training_reg, reg_val, dram_ndx, 10, exit);

    /*
     *  Disable ODT
     */
    fld_val = 0x1;
    SOC_PETRA_FLD_ISET(regs->dpi.io_odt_read_config_reg.io_odt_disable, fld_val, dram_ndx, 20, exit);

    SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.dll_mstr_s, fld_val, dram_ndx, 30, exit);
    mstr_vec = fld_val;

    SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.ready, fld_val, dram_ndx, 40, exit);
    if (fld_val != 0x1)
    {
      /*
       *  Ready error found
       */
      error_info->is_phy_ready_err = TRUE;

     SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.init_dn, fld_val, dram_ndx, 45, exit);

     if (mstr_vec <= SOC_PETRA_DIAG_TRAINING_MSTR_TH_MIN)
      {
        error_info->is_clocking_err = TRUE;
      }
     else if(SOC_SAND_GET_BITS_RANGE(fld_val,3,0) != 0xF)
     {
       for (dqs_ndx = 0; dqs_ndx < DIAG_DRAM_NOF_DQSS; ++dqs_ndx)
       {
         if (SOC_SAND_GET_BIT(fld_val, dqs_ndx) == 0)
         {
           error_info->is_dqs_con_err[dqs_ndx] = TRUE;
         }
       }
     }
   }

   for (dll_ndx = 0; dll_ndx < SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM; ++dll_ndx)
   {
     for (bit_num = 0; bit_num < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT; ++bit_num)
     {
       pat_vec = pat_vec_start;
       bit = SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT * dll_ndx + bit_num;
       for (word_ndx = 0; word_ndx < SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS; ++word_ndx)
       {
         SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[word_ndx], pat_vec, dram_ndx, 70, exit);
       }

       reg_val = 0x1;
       SOC_PETRA_REG_ISET(regs->dpi.dpi_init_reset_reg, reg_val, dram_ndx, 80, exit);
       SOC_PETRA_REG_ISET(regs->dpi.dpi_init_start_reg, reg_val, dram_ndx, 90, exit);

       SOC_SAND_SET_BIT(pat_vec, pati, bit);
       for (word_ndx = 0; word_ndx < SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS; ++word_ndx)
       {
         SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[word_ndx], pat_vec, dram_ndx, 100, exit);
       }

       pat_vec2 = pat_vec;
       SOC_SAND_SET_BIT(pat_vec2, pat, bit);
       word_ndx = SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS - 1;
       SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[word_ndx], pat_vec2, dram_ndx, 110, exit);

       reg_val = 0x1;
       SOC_PETRA_REG_ISET(regs->dpi.dpi_init_reset_reg, reg_val, dram_ndx, 120, exit);
       SOC_PETRA_REG_ISET(regs->dpi.dpi_init_start_reg, reg_val, dram_ndx, 130, exit);

       SOC_PETRA_FLD_IGET(regs->dpi.dpi_rnd_trp_diff_status_reg.rnd_trp_diff[dll_ndx], fld_val, dram_ndx, 140, exit);
       rtt += fld_val;
       rtt_arr[bit] = fld_val;
     }
   }
   rtt = rtt / (SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT * SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM);

   if (rtt > SOC_PETRA_DIAG_TRAINING_RTT_MAX)
   {
     error_info->is_rtt_avg_max_err = TRUE;
   }
   if (rtt < SOC_PETRA_DIAG_TRAINING_RTT_MIN)
   {
     error_info->is_rtt_avg_min_err = TRUE;
   }

   for (bit = 0; bit < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD; ++bit)
   {
     if (SOC_SAND_DELTA(rtt_arr[bit], rtt) > SOC_PETRA_DIAG_TRAINING_RTT_DIFF_ABS_TH)
     {
       bit_err[pat_ndx][bit] = TRUE;
     }
   }
 }

 for (bit = 0; bit < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD; ++bit)
 {
   if (
       (bit_err[0][bit] == TRUE)
       && (bit_err[1][bit] == TRUE)
      )
   {
     SOC_SAND_SET_BIT(error_info->bit_err_bitmap, TRUE, bit);
   }
 }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_diagnostic_get_unsafe()",bit,pat_ndx);
}



/* Retrieves roundtrip error count */
uint32
  soc_petra_diag_dram_roundtrip_status_check_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_OUT uint32                                  *err_count
  )
{
  uint32
    fld_val,
    dll_ndx,
    bit_num = 0,
    bit = 0,
    rtt = 0,
    rtt_arr[SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD],
    res,
    ret = 0;
  uint8
    bit_err[SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD];
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET_UNSAFE);

  for (bit = 0; bit < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD; ++bit)
  {
   bit_err[bit] = FALSE;
  }

  for (dll_ndx = 0; dll_ndx < SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM; ++dll_ndx)
  {
   for (bit_num = 0; bit_num < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT; ++bit_num)
   {
     bit = SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT * dll_ndx + bit_num;
     
     SOC_PETRA_FLD_IGET(regs->dpi.dpi_rnd_trp_diff_status_reg.rnd_trp_diff[dll_ndx], fld_val, dram_ndx, 140, exit);
     rtt += fld_val;
     rtt_arr[bit] = fld_val;
   }
  }
  rtt = rtt / (SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_PAT * SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM);

  if (rtt > SOC_PETRA_DIAG_TRAINING_RTT_MAX)
  {
    ret++;
  }

  for (bit = 0; bit < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD; ++bit)
  {
    if (SOC_SAND_DELTA(rtt_arr[bit], rtt) > SOC_PETRA_DIAG_TRAINING_RTT_DIFF_ABS_TH)
    {
      bit_err[bit] = TRUE;
    }
  }

  for (bit = 0; bit < SOC_PETRA_DIAG_TRAINING_NOF_BITS_PER_WORD; ++bit)
  {
    if (bit_err[bit] == TRUE)
    {
      ret++;
    }
  }

  *err_count = ret;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_roundtrip_status_check_unsafe()",bit,0);
}



/*********************************************************************
*     Get diagnostic for the DRAM interface training sequence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_diagnostic_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET_VERIFY);

  if (dram_ndx > SOC_PETRA_DIAG_DRAM_NDX_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_diagnostic_get_verify()",0,0);
}

/*****************************************************
 * See details in soc_petra_api_diagnostics.h
 *****************************************************/
uint32
  soc_petra_diag_iddr_get_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_OUT uint32     *data,
    SOC_SAND_IN  uint32     size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank,
    offset = 0,
    accesses_i = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PETRA_TBLS
    *tables = soc_petra_tbls();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_IDDR_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_PETRA_FLD_IGET(regs->drc.dram_compliance_configuration_reg.enable8_banks, bank, dram_ndx, 10, exit);

  if (bank)
  {
    offset |= SOC_SAND_SET_FLD_IN_PLACE(bank_ndx, 0, SOC_SAND_BITS_MASK(2, 0));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(dram_ndx, 3, SOC_SAND_BITS_MASK(4, 3));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(SOC_PETRA_TBL(tables->mmu.dram_address_space_tbl.addr.base) + dram_offset, 5, SOC_SAND_BITS_MASK(30, 5));
    offset |= SOC_SAND_BIT(31);
  }
  else
  {
    offset |= SOC_SAND_SET_FLD_IN_PLACE(bank_ndx, 0, SOC_SAND_BITS_MASK(1, 0));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(dram_ndx, 2, SOC_SAND_BITS_MASK(3, 2));
    offset |= SOC_SAND_SET_FLD_IN_PLACE(SOC_PETRA_TBL(tables->mmu.dram_address_space_tbl.addr.base) + dram_offset, 4, SOC_SAND_BITS_MASK(30, 4));
    offset |= SOC_SAND_BIT(31);
  }

  /* Verify indirect trigger is ZERO. */
  res = soc_sand_trigger_verify_0(
          unit,
          SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.addr.base),
          SOC_SAND_TRIGGER_TIMEOUT,
          SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  for(accesses_i = 0; accesses_i < (size / SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S); ++accesses_i)
  {
    SOC_PETRA_REG_SET(regs->mmu.indirect_command_address_reg, offset, 22, exit);

    res = soc_sand_trigger_assert_1(
            unit,
            SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.addr.base),
            SOC_SAND_TRIGGER_TIMEOUT,
            SOC_PETRA_REG_DB_ACC(regs->mmu.indirect_command_reg.indirect_command_trigger.lsb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    res = soc_petra_read_reg_buffer_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->mmu.indirect_command_rd_data_reg_0.addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S,
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    data += SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_iddr_get_unsafe()",0,0);
}

uint32
  soc_petra_diag_tbls_dump_tables_get(
    SOC_SAND_OUT SOC_PETRA_TBL_PRINT_INFO   *indirect_print,
    SOC_SAND_IN  uint8                block_id,
    SOC_SAND_IN  uint32               table_offset,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode,
    SOC_SAND_OUT uint32              *nof_tbls
  )
{
  uint32
#ifndef SAND_LOW_LEVEL_SIMULATION
    indx,
#endif
    res = SOC_SAND_OK;
  SOC_PETRA_TBLS
    *tables;
  uint32
    counter = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_TBLS_DUMP_TABLES_GET);

  SOC_SAND_CHECK_NULL_INPUT(indirect_print);
  SOC_SAND_CHECK_NULL_INPUT(nof_tbls);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (block_id == SOC_PETRA_OLP_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Pge Mem */
    if (table_offset == SOC_PA_TBL(tables->olp.pge_mem_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->olp.pge_mem_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->olp.pge_mem_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->olp.pge_mem_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_OLP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "OLP.pge_mem_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IRE_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Nif Ctxt Map */
    if (table_offset == SOC_PA_TBL(tables->ire.nif_ctxt_map_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.nif_ctxt_map_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.nif_ctxt_map_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.nif_ctxt_map_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.nif_ctxt_map_tbl";
#endif
      counter++;
    }
    /* Nif Port2ctxt Bit Map */
    if (table_offset == SOC_PA_TBL(tables->ire.nif_port2ctxt_bit_map_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.nif_port2ctxt_bit_map_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.nif_port2ctxt_bit_map_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.nif_port2ctxt_bit_map_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.nif_port2ctxt_bit_map_tbl";
#endif
      counter++;
    }
    /* Rcy Ctxt Map */
    if (table_offset == SOC_PA_TBL(tables->ire.rcy_ctxt_map_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.rcy_ctxt_map_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.rcy_ctxt_map_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.rcy_ctxt_map_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.rcy_ctxt_map_tbl";
#endif
      counter++;
    }
    /* cpu packet counters */
    if (table_offset == SOC_PA_TBL(tables->ire.cpu_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.cpu_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.cpu_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.cpu_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.cpu_packet_counter_tbl";
#endif
      counter++;
    }
    /* olp packet counters */
    if (table_offset == SOC_PA_TBL(tables->ire.olp_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.olp_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.olp_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.olp_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.olp_packet_counter_tbl";
#endif
      counter++;
    }
    /* nifa packet counters */
    if (table_offset == SOC_PA_TBL(tables->ire.nifa_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.nifa_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.nifa_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.nifa_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.nifa_packet_counter_tbl";
#endif
      counter++;
    }
    /* nifb packet counters */
    if (table_offset == SOC_PA_TBL(tables->ire.nifb_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.nifb_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.nifb_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.nifb_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.nifb_packet_counter_tbl";
#endif
      counter++;
    }
    /* rcy packet counters */
    if (table_offset == SOC_PA_TBL(tables->ire.rcy_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ire.rcy_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ire.rcy_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ire.rcy_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRE_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRE.rcy_packet_counter_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IDR_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Complete PC */
    if (table_offset == SOC_PA_TBL(tables->idr.complete_pc_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->idr.complete_pc_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->idr.complete_pc_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->idr.complete_pc_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IDR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IDR.complete_pc_tbl";
#endif
      counter++;
    }
   

  }
  if (block_id == SOC_PETRA_IRR_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Is Ingress Replication DB */
    if (table_offset == SOC_PA_TBL(tables->irr.is_ingress_replication_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.is_ingress_replication_db_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.is_ingress_replication_db_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.is_ingress_replication_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.is_ingress_replication_db_tbl";
#endif
      counter++;
    }
    /* Ingress Replication Multicast DB */
    if (table_offset == SOC_PA_TBL(tables->irr.ingress_replication_multicast_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.ingress_replication_multicast_db_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.ingress_replication_multicast_db_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.ingress_replication_multicast_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.ingress_replication_multicast_db_tbl";
#endif
      counter++;
    }
    /* Egress Replication Multicast DB */
    if (table_offset == SOC_PA_TBL(tables->irr.egress_replication_multicast_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.egress_replication_multicast_db_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.egress_replication_multicast_db_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.egress_replication_multicast_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.egress_replication_multicast_db_tbl";
#endif
      counter++;
    }
    /* Mirror Table */
    if (table_offset == SOC_PA_TBL(tables->irr.mirror_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.mirror_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.mirror_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.mirror_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.mirror_table_tbl";
#endif
      counter++;
    }
    /* Snoop Table */
    if (table_offset == SOC_PA_TBL(tables->irr.snoop_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.snoop_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.snoop_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.snoop_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.snoop_table_tbl";
#endif
      counter++;
    }
    /* Glag To Lag Range */
    if (table_offset == SOC_PA_TBL(tables->irr.glag_to_lag_range_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.glag_to_lag_range_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.glag_to_lag_range_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.glag_to_lag_range_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.glag_to_lag_range_tbl";
#endif
      counter++;
    }
    /* Smooth Division */
    if (table_offset == SOC_PA_TBL(tables->irr.smooth_division_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.smooth_division_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.smooth_division_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.smooth_division_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.smooth_division_tbl";
#endif
      counter++;
    }
    /* Glag Mapping */
    if (table_offset == SOC_PA_TBL(tables->irr.glag_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.glag_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.glag_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.glag_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.glag_mapping_tbl";
#endif
      counter++;
    }
    /* Destination Table */
    if (table_offset == SOC_PA_TBL(tables->irr.destination_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.destination_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.destination_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.destination_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.destination_table_tbl";
#endif
      counter++;
    }
    /* Glag Next Member */
    if (table_offset == SOC_PA_TBL(tables->irr.glag_next_member_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.glag_next_member_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.glag_next_member_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.glag_next_member_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.glag_next_member_tbl";
#endif
      counter++;
    }
    /* Rlag Next Member */
    if (table_offset == SOC_PA_TBL(tables->irr.rlag_next_member_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->irr.rlag_next_member_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->irr.rlag_next_member_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->irr.rlag_next_member_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IRR_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.rlag_next_member_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IHP_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Port Info */
    if (table_offset == SOC_PA_TBL(tables->ihp.port_info_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.port_info_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.port_info_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.port_info_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.port_info_tbl";
#endif
      counter++;
    }
    /* Port To System Port Id */
    if (table_offset == SOC_PA_TBL(tables->ihp.port_to_system_port_id_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.port_to_system_port_id_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.port_to_system_port_id_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.port_to_system_port_id_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.port_to_system_port_id_tbl";
#endif
      counter++;
    }
    /* Static Header */
    if (table_offset == SOC_PA_TBL(tables->ihp.static_header_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.static_header_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.static_header_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.static_header_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.static_header_tbl";
#endif
      counter++;
    }
    /* System Port My Port Table */
    if (table_offset == SOC_PA_TBL(tables->ihp.system_port_my_port_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.system_port_my_port_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.system_port_my_port_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.system_port_my_port_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.system_port_my_port_table_tbl";
#endif
      counter++;
    }
    /* Ptc Commands1 */
    if (table_offset == SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.ptc_commands1_tbl";
#endif
      counter++;
    }
    /* Ptc Commands2 */
    if (table_offset == SOC_PA_TBL(tables->ihp.ptc_commands2_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.ptc_commands2_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.ptc_commands2_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.ptc_commands2_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.ptc_commands2_tbl";
#endif
      counter++;
    }
    /* Ptc Key Program Lut */
    if (table_offset == SOC_PA_TBL(tables->ihp.ptc_key_program_lut_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.ptc_key_program_lut_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.ptc_key_program_lut_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.ptc_key_program_lut_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.ptc_key_program_lut_tbl";
#endif
      counter++;
    }
    /* Key Program */
    if (table_offset == SOC_PA_TBL(tables->ihp.key_program_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.key_program_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.key_program_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.key_program_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.key_program_tbl";
#endif
      counter++;
    }
    /* Key Program0 */
    if (table_offset == SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.key_program0_tbl";
#endif
      counter++;
    }
    /* Key Program1 */
    if (table_offset == SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.key_program1_tbl";
#endif
      counter++;
    }
    /* Key Program2 */
    if (table_offset == SOC_PA_TBL(tables->ihp.key_program2_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.key_program2_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.key_program2_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.key_program2_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.key_program2_tbl";
#endif
      counter++;
    }
    /* Key Program3 */
    if (table_offset == SOC_PA_TBL(tables->ihp.key_program3_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.key_program3_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.key_program3_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.key_program3_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.key_program3_tbl";
#endif
      counter++;
    }
    /* Key Program4 */
    if (table_offset == SOC_PA_TBL(tables->ihp.key_program4_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.key_program4_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.key_program4_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.key_program4_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.key_program4_tbl";
#endif
      counter++;
    }

    if (table_offset == SOC_PA_TBL(tables->ihp.programmable_cos_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.programmable_cos_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.programmable_cos_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.programmable_cos_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.programmable_cos_tbl";
#endif
      counter++;
    }

    /* Programmable Cos1 */
    if (table_offset == SOC_PA_TBL(tables->ihp.programmable_cos1_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ihp.programmable_cos1_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ihp.programmable_cos1_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ihp.programmable_cos1_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IHP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IHP.programmable_cos1_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IQM_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* BDB Link List */
    if (table_offset == SOC_PA_TBL(tables->iqm.bdb_link_list_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.bdb_link_list_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.bdb_link_list_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.bdb_link_list_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.bdb_link_list_tbl";
#endif
      counter++;
    }
    /* Packet Queue Descriptor (Dynamic ) */
    if (table_offset == SOC_PA_TBL(tables->iqm.dynamic_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.dynamic_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.dynamic_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.dynamic_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.dynamic_tbl";
#endif
      counter++;
    }
    /* Packet Queue Descriptor (Static) */
    if (table_offset == SOC_PA_TBL(tables->iqm.static_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.static_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.static_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.static_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.static_tbl";
#endif
      counter++;
    }
    /* Packet Queue Tail Pointer */
    if (table_offset == SOC_PA_TBL(tables->iqm.packet_queue_tail_pointer_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.packet_queue_tail_pointer_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.packet_queue_tail_pointer_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.packet_queue_tail_pointer_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.packet_queue_tail_pointer_tbl";
#endif
      counter++;
    }
    /* Packet Queue Red Weight table */
    if (table_offset == SOC_PA_TBL(tables->iqm.packet_queue_red_weight_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.packet_queue_red_weight_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.packet_queue_red_weight_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.packet_queue_red_weight_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.packet_queue_red_weight_table_tbl";
#endif
      counter++;
    }
    /* Credit Discount table */
    if (table_offset == SOC_PA_TBL(tables->iqm.credit_discount_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.credit_discount_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.credit_discount_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.credit_discount_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.credit_discount_table_tbl";
#endif
      counter++;
    }
    /* Full User Count Memory */
    if (table_offset == SOC_PA_TBL(tables->iqm.full_user_count_memory_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.full_user_count_memory_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.full_user_count_memory_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.full_user_count_memory_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.full_user_count_memory_tbl";
#endif
      counter++;
    }
    /* Mini-Multicast User Count Memory */
    if (table_offset == SOC_PA_TBL(tables->iqm.mini_multicast_user_count_memory_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.mini_multicast_user_count_memory_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.mini_multicast_user_count_memory_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.mini_multicast_user_count_memory_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.mini_multicast_user_count_memory_tbl";
#endif
      counter++;
    }
    /* Packet Queue Red parameters table */
    if (table_offset == SOC_PA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.packet_queue_red_parameters_table_tbl";
#endif
      counter++;
    }
        /* Packet Descriptor Fifos Memory: Tx PD-FIFO fifos memory.       */
    if (table_offset == SOC_PA_TBL(tables->iqm.packet_descriptor_fifos_memory_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.packet_descriptor_fifos_memory_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.packet_descriptor_fifos_memory_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.packet_descriptor_fifos_memory_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.packet_descriptor_fifos_memory_tbl";
#endif
      counter++;
    }

    /* Tx Descriptor Fifos Memory: Tx Descriptor-FIFO fifos           */
    if (table_offset == SOC_PA_TBL(tables->iqm.tx_descriptor_fifos_memory_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.tx_descriptor_fifos_memory_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.tx_descriptor_fifos_memory_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.tx_descriptor_fifos_memory_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.tx_descriptor_fifos_memory_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group A */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_descriptor_rate_class_group_a_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group B */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_descriptor_rate_class_group_b_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group C */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_descriptor_rate_class_group_c_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group D */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_descriptor_rate_class_group_d_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group A */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_a_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_a_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_a_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_a_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_qsize_memory_group_a_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group B */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_b_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_b_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_b_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_b_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_qsize_memory_group_b_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group C */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_c_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_c_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_c_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_c_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_qsize_memory_group_c_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group D */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_d_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_d_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_d_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_d_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_qsize_memory_group_d_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group A */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_average_qsize_memory_group_a_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group B */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_average_qsize_memory_group_b_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group C */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_average_qsize_memory_group_c_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group D */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_average_qsize_memory_group_d_tbl";
#endif
      counter++;
    }
    /* VSQ Flow-Control Parameters table - group A */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_flow_control_parameters_table_group_tbl[0]";
#endif
      counter++;
    }
    /* VSQ Flow-Control Parameters table - group B */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_flow_control_parameters_table_group_tbl[1]";
#endif
      counter++;
    }
    /* VSQ Flow-Control Parameters table - group C */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_flow_control_parameters_table_group_tbl[2]";
#endif
      counter++;
    }
    /* VSQ Flow-Control Parameters table - group D */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_flow_control_parameters_table_group_tbl[3]";
#endif
      counter++;
    }
    /* VSQ Queue Parameters table - group A */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[0].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[0].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[0].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[0].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_queue_parameters_table_group_tbl[0]";
#endif
      counter++;
    }
    /* VSQ Queue Parameters table - group B */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[1].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[1].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[1].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[1].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_queue_parameters_table_group_tbl[1]";
#endif
      counter++;
    }
    /* VSQ Queue Parameters table - group C */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[2].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[2].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[2].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[2].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_queue_parameters_table_group_tbl[2]";
#endif
      counter++;
    }
    /* VSQ Queue Parameters table - group D */
    if (table_offset == SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[3].addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[3].addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[3].addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[3].addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.vsq_queue_parameters_table_group_tbl[3]";
#endif
      counter++;
    }
    /* System Red parameters table */
    if (table_offset == SOC_PA_TBL(tables->iqm.system_red_parameters_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.system_red_parameters_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.system_red_parameters_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.system_red_parameters_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.system_red_parameters_table_tbl";
#endif
      counter++;
    }
    /* System Red drop probability values */
    if (table_offset == SOC_PA_TBL(tables->iqm.system_red_drop_probability_values_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.system_red_drop_probability_values_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.system_red_drop_probability_values_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.system_red_drop_probability_values_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.system_red_drop_probability_values_tbl";
#endif
      counter++;
    }
    /* Source Qsize range thresholds (System Red) */
    if (table_offset == SOC_PA_TBL(tables->iqm.system_red_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->iqm.system_red_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->iqm.system_red_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->iqm.system_red_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IQM_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IQM.system_red_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IPS_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* System Physical Port Lookup Table */
    if (table_offset == SOC_PA_TBL(tables->ips.system_physical_port_lookup_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.system_physical_port_lookup_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.system_physical_port_lookup_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.system_physical_port_lookup_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.system_physical_port_lookup_table_tbl";
#endif
      counter++;
    }
    /* Destination Device And Port Lookup Table */
    if (table_offset == SOC_PA_TBL(tables->ips.destination_device_and_port_lookup_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.destination_device_and_port_lookup_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.destination_device_and_port_lookup_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.destination_device_and_port_lookup_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.destination_device_and_port_lookup_table_tbl";
#endif
      counter++;
    }
    /* Flow Id Lookup Table */
    if (table_offset == SOC_PA_TBL(tables->ips.flow_id_lookup_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.flow_id_lookup_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.flow_id_lookup_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.flow_id_lookup_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.flow_id_lookup_table_tbl";
#endif
      counter++;
    }
    /* Queue Type Lookup Table */
    if (table_offset == SOC_PA_TBL(tables->ips.queue_type_lookup_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.queue_type_lookup_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.queue_type_lookup_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.queue_type_lookup_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.queue_type_lookup_table_tbl";
#endif
      counter++;
    }
    /* Queue Priority Map Select */
    if (table_offset == SOC_PA_TBL(tables->ips.queue_priority_map_select_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.queue_priority_map_select_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.queue_priority_map_select_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.queue_priority_map_select_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.queue_priority_map_select_tbl";
#endif
      counter++;
    }
    /* Queue Priority Maps Table */
    if (table_offset == SOC_PA_TBL(tables->ips.queue_priority_maps_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.queue_priority_maps_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.queue_priority_maps_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.queue_priority_maps_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.queue_priority_maps_table_tbl";
#endif
      counter++;
    }
    /* Queue Size-Based Thresholds Table */
    if (table_offset == SOC_PA_TBL(tables->ips.queue_size_based_thresholds_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.queue_size_based_thresholds_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.queue_size_based_thresholds_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.queue_size_based_thresholds_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.queue_size_based_thresholds_table_tbl";
#endif
      counter++;
    }
    /* Credit Balance Based Thresholds Table */
    if (table_offset == SOC_PA_TBL(tables->ips.credit_balance_based_thresholds_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.credit_balance_based_thresholds_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.credit_balance_based_thresholds_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.credit_balance_based_thresholds_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.credit_balance_based_thresholds_table_tbl";
#endif
      counter++;
    }
    /* Empty Queue Credit Balance Table */
    if (table_offset == SOC_PA_TBL(tables->ips.empty_queue_credit_balance_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.empty_queue_credit_balance_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.empty_queue_credit_balance_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.empty_queue_credit_balance_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.empty_queue_credit_balance_table_tbl";
#endif
      counter++;
    }
    /* Credit Watchdog Thresholds Table */
    if (table_offset == SOC_PA_TBL(tables->ips.credit_watchdog_thresholds_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.credit_watchdog_thresholds_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.credit_watchdog_thresholds_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.credit_watchdog_thresholds_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.credit_watchdog_thresholds_table_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IPS_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Queue Descriptor Table */
    if (table_offset == SOC_PA_TBL(tables->ips.queue_descriptor_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.queue_descriptor_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.queue_descriptor_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.queue_descriptor_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.queue_descriptor_table_tbl";
#endif
      counter++;
    }
    /* Queue Size Table */
    if (table_offset == SOC_PA_TBL(tables->ips.queue_size_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.queue_size_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.queue_size_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.queue_size_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.queue_size_table_tbl";
#endif
      counter++;
    }
    /* System Red Max Queue Size Table */
    if (table_offset == SOC_PA_TBL(tables->ips.system_red_max_queue_size_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ips.system_red_max_queue_size_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ips.system_red_max_queue_size_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ips.system_red_max_queue_size_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPS_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPS.system_red_max_queue_size_table_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_IPT_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* BDQ */
    if (table_offset == SOC_PA_TBL(tables->ipt.bdq_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.bdq_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.bdq_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.bdq_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.bdq_tbl";
#endif
      counter++;
    }
    /* PCQ */
    if (table_offset == SOC_PA_TBL(tables->ipt.pcq_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.pcq_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.pcq_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.pcq_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.pcq_tbl";
#endif
      counter++;
    }
    /* Sop MMU */
    if (table_offset == SOC_PA_TBL(tables->ipt.sop_mmu_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.sop_mmu_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.sop_mmu_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.sop_mmu_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.sop_mmu_tbl";
#endif
      counter++;
    }
    /* Mop MMU */
    if (table_offset == SOC_PA_TBL(tables->ipt.mop_mmu_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.mop_mmu_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.mop_mmu_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.mop_mmu_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.mop_mmu_tbl";
#endif
      counter++;
    }
    /* FDTCTL */
    if (table_offset == SOC_PA_TBL(tables->ipt.fdtctl_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.fdtctl_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.fdtctl_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.fdtctl_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.fdtctl_tbl";
#endif
      counter++;
    }
    /* FDTDATA */
    if (table_offset == SOC_PA_TBL(tables->ipt.fdtdata_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.fdtdata_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.fdtdata_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.fdtdata_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.fdtdata_tbl";
#endif
      counter++;
    }
    /* EGQCTL */
    if (table_offset == SOC_PA_TBL(tables->ipt.egqctl_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.egqctl_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.egqctl_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.egqctl_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.egqctl_tbl";
#endif
      counter++;
    }
    /* EGQDATA */
    if (table_offset == SOC_PA_TBL(tables->ipt.egqdata_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.egqdata_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.egqdata_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.egqdata_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.egqdata_tbl";
#endif
      counter++;
    }

    if (table_offset == SOC_PA_TBL(tables->ipt.select_source_sum_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->ipt.select_source_sum_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->ipt.select_source_sum_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->ipt.select_source_sum_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_IPT_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IPT.select_source_sum_tbl";
#endif
      counter++;
    }
  }

  if ((block_id >= SOC_PETRA_DPI_A_ID) && (block_id <= SOC_PETRA_DPI_F_ID))
  {
    /* DLL_RAM */
    if (table_offset == SOC_PA_TBL(tables->dpi.dll_ram_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->dpi.dll_ram_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->dpi.dll_ram_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->dpi.dll_ram_tbl.addr.width_bits));
      indirect_print[counter].mod_id = block_id;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "DPI.dll_ram_tbl";
#endif
      counter++;
    }
  }

  if (block_id == SOC_PETRA_RTP_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Unicast Distribution Memory for data cells */
    if (table_offset == SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_RTP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "RTP.unicast_distribution_memory_for_data_cells_tbl";
#endif
      counter++;
    }
    /* Unicast Distribution Memory for control cells */
    if (table_offset == SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_RTP_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "RTP.unicast_distribution_memory_for_control_cells_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_EGQ_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Egress Shaper Credit Configuration (Nif Ch Scm) */
    if (table_offset == SOC_PA_TBL(tables->egq.nif_scm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.nif_scm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.nif_scm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.nif_scm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.nif_scm_tbl";
#endif
      counter++;
    }
    /* Egress Shaper Nifa And Nifb Non Channeleized Ports Credit Configuration (Nifab Nch Scm) */
    if (table_offset == SOC_PA_TBL(tables->egq.nifab_nch_scm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.nifab_nch_scm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.nifab_nch_scm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.nifab_nch_scm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.nifab_nch_scm_tbl";
#endif
      counter++;
    }
    /* Egress Shaper Recycling Ports Credit Configuration (Rcy Scm) */
    if (table_offset == SOC_PA_TBL(tables->egq.rcy_scm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.rcy_scm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.rcy_scm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.rcy_scm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.rcy_scm_tbl";
#endif
      counter++;
    }
    /* Egress Shaper CPUPorts Credit Configuration(Cpu Scm) */
    if (table_offset == SOC_PA_TBL(tables->egq.cpu_scm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.cpu_scm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.cpu_scm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.cpu_scm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.cpu_scm_tbl";
#endif
      counter++;
    }
    /* Egress Shaper Calendar Selector (CCM) */
    if (table_offset == SOC_PA_TBL(tables->egq.ccm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ccm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ccm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ccm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ccm_tbl";
#endif
      counter++;
    }
    /* Per Port Max Credit Memory (PMC) */
    if (table_offset == SOC_PA_TBL(tables->egq.pmc_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pmc_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pmc_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pmc_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pmc_tbl";
#endif
      counter++;
    }
    /* Egress Shaper Per Port Credit Balance Memory (CBM)*/
    if (table_offset == SOC_PA_TBL(tables->egq.cbm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.cbm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.cbm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.cbm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.cbm_tbl";
#endif
      counter++;
    }
    /* Free Buffesr Allocation Bitmap Memory (FBM) */
    if (table_offset == SOC_PA_TBL(tables->egq.fbm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.fbm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.fbm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.fbm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.fbm_tbl";
#endif
      counter++;
    }
    /* Free Descriptors Allocation Bitmap Memory (FDM) */
    if (table_offset == SOC_PA_TBL(tables->egq.fdm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.fdm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.fdm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.fdm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.fdm_tbl";
#endif
      counter++;
    }
    /* Ofp Dequeue Wfq Configuration Memory (DWM) */
    if (table_offset == SOC_PA_TBL(tables->egq.dwm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.dwm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.dwm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.dwm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.dwm_tbl";
#endif
      counter++;
    }
    /* RRDM */
    if (table_offset == SOC_PA_TBL(tables->egq.rrdm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.rrdm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.rrdm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.rrdm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.rrdm_tbl";
#endif
      counter++;
    }
    /* RPDM */
    if (table_offset == SOC_PA_TBL(tables->egq.rpdm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.rpdm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.rpdm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.rpdm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.rpdm_tbl";
#endif
      counter++;
    }
    /* Port Configuration Table (PCT) */
    if (table_offset == SOC_PA_TBL(tables->egq.pct_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pct_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pct_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pct_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pct_tbl";
#endif
      counter++;
    }
    /* Per Port Configuration Table(PPCT) */
    if (table_offset == SOC_PA_TBL(tables->egq.ppct_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ppct_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ppct_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ppct_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ppct_tbl";
#endif
      counter++;
    }
    /* Vlan Table Configuration Memory(Vlan Table) */
    if (table_offset == SOC_PA_TBL(tables->egq.vlan_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.vlan_table_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.vlan_table_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.vlan_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.vlan_table_tbl";
#endif
      counter++;
    }
    /* CfcFlowControl */
    if (table_offset == SOC_PA_TBL(tables->egq.cfc_flow_control_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.cfc_flow_control_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.cfc_flow_control_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.cfc_flow_control_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.cfc_flow_control_tbl";
#endif
      counter++;
    }
    /* NifaFlowControl */
    if (table_offset == SOC_PA_TBL(tables->egq.nifa_flow_control_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.nifa_flow_control_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.nifa_flow_control_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.nifa_flow_control_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.nifa_flow_control_tbl";
#endif
      counter++;
    }
    /* NifbFlowControl */
    if (table_offset == SOC_PA_TBL(tables->egq.nifb_flow_control_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.nifb_flow_control_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.nifb_flow_control_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.nifb_flow_control_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.nifb_flow_control_tbl";
#endif
      counter++;
    }
    /* CpuLastHeader */
    if (table_offset == SOC_PA_TBL(tables->egq.cpu_last_header_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.cpu_last_header_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.cpu_last_header_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.cpu_last_header_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.cpu_last_header_tbl";
#endif
      counter++;
    }
    /* IptLastHeader */
    if (table_offset == SOC_PA_TBL(tables->egq.ipt_last_header_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ipt_last_header_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ipt_last_header_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ipt_last_header_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ipt_last_header_tbl";
#endif
      counter++;
    }
    /* FdrLastHeader */
    if (table_offset == SOC_PA_TBL(tables->egq.fdr_last_header_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.fdr_last_header_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.fdr_last_header_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.fdr_last_header_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.fdr_last_header_tbl";
#endif
      counter++;
    }
    /* CpuPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.cpu_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.cpu_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.cpu_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.cpu_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.cpu_packet_counter_tbl";
#endif
      counter++;
    }
    /* IptPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.ipt_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ipt_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ipt_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ipt_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ipt_packet_counter_tbl";
#endif
      counter++;
    }
    /* FdrPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.fdr_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.fdr_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.fdr_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.fdr_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.fdr_packet_counter_tbl";
#endif
      counter++;
    }
    /* RqpPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.rqp_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.rqp_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.rqp_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.rqp_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.rqp_packet_counter_tbl";
#endif
      counter++;
    }
    /* RqpDiscardPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.rqp_discard_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.rqp_discard_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.rqp_discard_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.rqp_discard_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.rqp_discard_packet_counter_tbl";
#endif
      counter++;
    }
    /* EhpUnicastPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.ehp_unicast_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ehp_unicast_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ehp_unicast_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ehp_unicast_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ehp_unicast_packet_counter_tbl";
#endif
      counter++;
    }
    /* EhpMulticastHighPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.ehp_multicast_high_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ehp_multicast_high_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ehp_multicast_high_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ehp_multicast_high_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ehp_multicast_high_packet_counter_tbl";
#endif
      counter++;
    }
    /* EhpMulticastLowPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.ehp_multicast_low_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ehp_multicast_low_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ehp_multicast_low_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ehp_multicast_low_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ehp_multicast_low_packet_counter_tbl";
#endif
      counter++;
    }
    /* EhpDiscardPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.ehp_discard_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.ehp_discard_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.ehp_discard_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.ehp_discard_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.ehp_discard_packet_counter_tbl";
#endif
      counter++;
    }
    /* PqpUnicastHighPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_unicast_high_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_unicast_high_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_unicast_high_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_unicast_high_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_unicast_high_packet_counter_tbl";
#endif
      counter++;
    }
    /* PqpUnicastLowPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_unicast_low_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_unicast_low_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_unicast_low_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_unicast_low_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_unicast_low_packet_counter_tbl";
#endif
      counter++;
    }
    /* PqpMulticastHighPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_multicast_high_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_multicast_high_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_multicast_high_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_multicast_high_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_multicast_high_packet_counter_tbl";
#endif
      counter++;
    }
    /* PqpMulticastLowPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_multicast_low_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_multicast_low_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_multicast_low_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_multicast_low_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_multicast_low_packet_counter_tbl";
#endif
      counter++;
    }
    /* PqpUnicastHighBytesCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_unicast_high_bytes_counter_tbl";
#endif
      counter++;
    }
    /* PqpUnicastLowBytesCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_unicast_low_bytes_counter_tbl";
#endif
      counter++;
    }
    /* PqpMulticastHighBytesCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_multicast_high_bytes_counter_tbl";
#endif
      counter++;
    }
    /* PqpMulticastLowBytesCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_multicast_low_bytes_counter_tbl";
#endif
      counter++;
    }
    /* PqpDiscardUnicastPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_discard_unicast_packet_counter_tbl";
#endif
      counter++;
    }
    /* PqpDiscardMulticastPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.pqp_discard_multicast_packet_counter_tbl";
#endif
      counter++;
    }
    /* FqpPacketCounter */
    if (table_offset == SOC_PA_TBL(tables->egq.fqp_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->egq.fqp_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->egq.fqp_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->egq.fqp_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EGQ_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EGQ.fqp_packet_counter_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_EPNI_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* EPE2PNI packet counter */
    if (table_offset == SOC_PA_TBL(tables->epni.epe_packet_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->epni.epe_packet_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->epni.epe_packet_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->epni.epe_packet_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EPNI_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EPNI.epe_packet_counter_tbl";
#endif
      counter++;
    }
    /* EPE2PNI bytes counter */
    if (table_offset == SOC_PA_TBL(tables->epni.epe_bytes_counter_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->epni.epe_bytes_counter_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->epni.epe_bytes_counter_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->epni.epe_bytes_counter_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_EPNI_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "EPNI.epe_bytes_counter_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_CFC_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Recycle to Out Going Fap Port Mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.recycle_to_out_going_fap_port_mapping_tbl";
#endif
      counter++;
    }
    /* NIF A Class Based to OFP mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.nif_a_class_based_to_ofp_mapping_tbl";
#endif
      counter++;
    }
    /* NIF B Class Based to OFP mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.nif_b_class_based_to_ofp_mapping_tbl";
#endif
      counter++;
    }
    /* Out Of Band (A) Scheduler's based flow-control to OFP mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.a_schedulers_based_flow_control_to_ofp_mapping_tbl";
#endif
      counter++;
    }
    /* Out Of Band (B) Scheduler's based flow-control to OFP mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.b_schedulers_based_flow_control_to_ofp_mapping_tbl";
#endif
      counter++;
    }
    /* Out Of Band Rx A  calendar mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.out_of_band_rx_a_calendar_mapping_tbl";
#endif
      counter++;
    }
    /* Out Of Band Rx B calendar mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.out_of_band_rx_b_calendar_mapping_tbl";
#endif
      counter++;
    }
    /* Out Of Band Tx  calendar mapping */
    if (table_offset == SOC_PA_TBL(tables->cfc.out_of_band_tx_calendar_mapping_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->cfc.out_of_band_tx_calendar_mapping_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->cfc.out_of_band_tx_calendar_mapping_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->cfc.out_of_band_tx_calendar_mapping_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_CFC_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "CFC.out_of_band_tx_calendar_mapping_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PETRA_SCH_ID || block_id == SOC_PETRA_NOF_MODULES)
  {
    /* Scheduler Credit Generation Calendar (CAL) */
    if (table_offset == SOC_PA_TBL(tables->sch.cal_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.cal_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.cal_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.cal_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.cal_tbl";
#endif
      counter++;
    }
    /* Device Rate Memory (DRM) */
    if (table_offset == SOC_PA_TBL(tables->sch.drm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.drm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.drm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.drm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.drm_tbl";
#endif
      counter++;
    }
    /* Dual Shaper Memory (DSM) */
    if (table_offset == SOC_PA_TBL(tables->sch.dsm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.dsm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.dsm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.dsm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dsm_tbl";
#endif
      counter++;
    }
    /* Flow Descriptor Memory Static (FDMS) */
    if (table_offset == SOC_PA_TBL(tables->sch.fdms_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.fdms_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.fdms_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.fdms_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fdms_tbl";
#endif
      counter++;
    }
    /* Shaper Descriptor Memory Static (SHDS) */
    if (table_offset == SOC_PA_TBL(tables->sch.shds_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.shds_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.shds_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.shds_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.shds_tbl";
#endif
      counter++;
    }
    /* Scheduler Enable Memory (SEM) */
    if (table_offset == SOC_PA_TBL(tables->sch.sem_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.sem_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.sem_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.sem_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sem_tbl";
#endif
      counter++;
    }
    /* Flow Sub-Flow (FSF) */
    if (table_offset == SOC_PA_TBL(tables->sch.fsf_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.fsf_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.fsf_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.fsf_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fsf_tbl";
#endif
      counter++;
    }
    /* Flow Group Memory (FGM) */
    if (table_offset == SOC_PA_TBL(tables->sch.fgm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.fgm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.fgm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.fgm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fgm_tbl";
#endif
      counter++;
    }
    /* HR-Scheduler-Configuration (SHC) */
    if (table_offset == SOC_PA_TBL(tables->sch.shc_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.shc_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.shc_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.shc_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.shc_tbl";
#endif
      counter++;
    }
    /* CL-Schedulers Configuration (SCC) */
    if (table_offset == SOC_PA_TBL(tables->sch.scc_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.scc_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.scc_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.scc_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.scc_tbl";
#endif
      counter++;
    }
    /* CL-Schedulers Type (SCT) */
    if (table_offset == SOC_PA_TBL(tables->sch.sct_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.sct_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.sct_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.sct_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sct_tbl";
#endif
      counter++;
    }
    /* Flow to Queue Mapping (FQM) */
    if (table_offset == SOC_PA_TBL(tables->sch.fqm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.fqm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.fqm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.fqm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fqm_tbl";
#endif
      counter++;
    }
    /* Flow to FIP Mapping (FFM) */
    if (table_offset == SOC_PA_TBL(tables->sch.ffm_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.ffm_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.ffm_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.ffm_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.ffm_tbl";
#endif
      counter++;
    }
    /* Token Memory Controller (TMC) */
    if (table_offset == SOC_PA_TBL(tables->sch.soc_tmctbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.soc_tmctbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.soc_tmctbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.soc_tmctbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.soc_tmctbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40110000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40110000;
      indirect_print[counter].size   = 0x0e00;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fim_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40120000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40120000;
      indirect_print[counter].size   = 0x3800;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(8);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fsm_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40130000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40130000;
      indirect_print[counter].size   = 0xe000;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fdmd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40140000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40140000;
      indirect_print[counter].size   = 0xe000;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.shdd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40150000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40150000;
      indirect_print[counter].size   = 0x0100;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(32);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dfq_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40160000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40160000;
      indirect_print[counter].size   = 0x0100;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(78);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dhd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40170000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40170000;
      indirect_print[counter].size   = 0x2000;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(71);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dcd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40180000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40180000;
      indirect_print[counter].size   = 0x0054;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(32);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sflh_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40190000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40190000;
      indirect_print[counter].size   = 0x0054;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(32);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sflt_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x401a0000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x401a0000;
      indirect_print[counter].size   = 0x0300;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sad_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x401b0000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x401b0000;
      indirect_print[counter].size   = 0x0e00;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sim_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x401c0000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x401c0000;
      indirect_print[counter].size   = 0x0054;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(4);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dpn_tbl";
#endif
      counter++;
    }
    /* Port Queue Size (PQS) */
    if (table_offset == SOC_PA_TBL(tables->sch.pqs_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = SOC_PA_TBL(tables->sch.pqs_tbl.addr.base);
      indirect_print[counter].size   = SOC_PA_TBL(tables->sch.pqs_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(SOC_PA_TBL(tables->sch.pqs_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.pqs_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40300000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40300000;
      indirect_print[counter].size   = 0x0400;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flhhr_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40310000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40310000;
      indirect_print[counter].size   = 0x0400;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flthr_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40320000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40320000;
      indirect_print[counter].size   = 0x2000;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flhcl_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40330000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40330000;
      indirect_print[counter].size   = 0x2000;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fltcl_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40340000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40340000;
      indirect_print[counter].size   = 0x1f00;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flhfq_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40350000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40350000;
      indirect_print[counter].size   = 0x1f00;
      indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PETRA_SCH_ID;
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fltfq_tbl";
#endif
      counter++;
    }
  }

  if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL)
  {
#ifndef SAND_LOW_LEVEL_SIMULATION
    for (indx = 0; indx < (sizeof(dev_tbls) / sizeof(SOC_PETRA_DIAG_DEV_TBL));  indx ++)
    {
        indirect_print[counter].base   = (dev_tbls[indx].start_address);
        indirect_print[counter].size   = (dev_tbls[indx].last_address - dev_tbls[indx].start_address + 1);
        indirect_print[counter].wrd_sz = SOC_PETRA_WIDTH_BITS_TO_WIDTH_BYTES(dev_tbls[indx].nof_bits);
        indirect_print[counter].mod_id = (dev_tbls[indx].module_id);
#ifdef SOC_PETRA_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name = dev_tbls[indx].name;
#endif
        counter++;
    }
#endif
  }
  *nof_tbls = counter;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_tbls_dump_tables_get()",0,0);
}

/*********************************************************************
 *     Set simple pre-settings of the BIST.
 *********************************************************************/
uint32
  soc_petra_diag_dram_presettings_set(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          dram_ndx,
    SOC_SAND_IN  uint32          nof_reads,
    SOC_SAND_IN  uint32          nof_writes
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    fld_val,
    reg_val,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_PRESETTINGS_SET);

  regs = soc_petra_regs();

  SOC_PETRA_REG_IGET(regs->drc.bist_configurations_reg, reg_val, dram_ndx, 5, exit);

  /*
   *  Cancel the pattern bit mode the prbs mode, the BIST enable
   */
  fld_val = 0x0;
  SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.pattern_bit_mode, fld_val, reg_val, 10, exit);
  SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.prbsmode, fld_val, reg_val, 30, exit);

 /*
  * Set the number of reads and writes commands to be performed in a BIST cycle
  */
  SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.read_weight, nof_reads, reg_val, 60, exit);
  SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.write_weight, nof_writes, reg_val, 70, exit);

  SOC_PETRA_REG_ISET(regs->drc.bist_configurations_reg, reg_val, dram_ndx, 75 ,exit);

  /*
   *  Clear the counters used for read_and_compare
   */
  SOC_PETRA_REG_IGET(regs->drc.bist_error_occurred_reg.err_occurred, reg_val, dram_ndx, 80 ,exit);
  SOC_PETRA_REG_IGET(regs->drc.bist_full_mask_error_counter_reg.full_err_cnt,reg_val,dram_ndx,90,exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_presettings_set()",0,0);
}
/*********************************************************************
 *     Convert the offset to the formatted address for the BIST
 *     mechanism
 *********************************************************************/
STATIC
  uint32
    soc_petra_diag_offset2bist_addr_convert(
      SOC_SAND_IN  int    unit,
      SOC_SAND_IN  uint32     offset_start,
      SOC_SAND_IN  uint32     offset_fin,
      SOC_SAND_OUT uint32     *bist_addr_start,
      SOC_SAND_OUT uint32     *bist_addr_end
    )
{
  uint32
    offset_end = offset_fin,
    log_base_2_nof_columns,
    single_interface_dram_size_bytes,
    column_start,
    column_end,
    row_start,
    row_end,
    bank_start,
    bank_end,
    offset_local,
    bist_addr_start_local = 0,
    bist_addr_end_local = 0,
    nof_drams,
    res,
    nof_columns,
    nof_banks,
    offset_in_bursts;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_OFFSET2BIST_ADDR_CONVERT);

  SOC_SAND_CHECK_NULL_INPUT(bist_addr_start);
  SOC_SAND_CHECK_NULL_INPUT(bist_addr_end);

  /*
   *  The offsets must be aligned to 32 - if equals the total, then set - 32
   */
  res = soc_petra_sw_db_dram_dram_size_get(
          unit,
          &single_interface_dram_size_bytes
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  single_interface_dram_size_bytes *= SOC_PETRA_DIAG_DRAM_NOF_BYTES_IN_MBYTES;

  if (offset_end == single_interface_dram_size_bytes)
  {
    offset_end = offset_end - 32;
  }

  if ((offset_start % 32) != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DRAM_OFFSET_ILLEGAL_RANGE_ERR, 5, exit);
  }
  if ((offset_end % 32) != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DRAM_OFFSET_ILLEGAL_RANGE_ERR, 6, exit);
  }

  /*
   *  Get the dram characteristics: number of columns, banks and the dram size per interface
   */
  res = soc_petra_mmu_dram_address_space_info_get_unsafe(
          unit,
          &nof_drams,
          &nof_banks,
          &nof_columns
        );
  log_base_2_nof_columns = soc_sand_log2_round_up(nof_columns);

  /*
   *  Check the found values are not inconsistent
   */
  if (nof_banks * nof_columns * 4 > single_interface_dram_size_bytes)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_INCONSISTENT_DRAM_CONFIG_ERR, 50, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    offset_start,  single_interface_dram_size_bytes - 32,
    SOC_PETRA_DIAG_DRAM_OFFSET_ILLEGAL_RANGE_ERR, 60, exit
   );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    offset_end,  single_interface_dram_size_bytes - 32,
    SOC_PETRA_DIAG_DRAM_OFFSET_ILLEGAL_RANGE_ERR, 70, exit
   );

  /*
   *  Compute the characteristics of the bist addresses: column, row, bank
   */

  /*
   *  There are 32B per (Row, Column, Bank) triplet (2 chips, 16B per chip)
   */
  offset_in_bursts = offset_start / 32;
  row_start = offset_in_bursts / (nof_columns * nof_banks);
  offset_local = offset_in_bursts % (nof_columns * nof_banks);
  column_start = offset_local / nof_banks;
  bank_start = offset_local % nof_banks;

  offset_in_bursts = offset_end / 32;
  row_end = offset_in_bursts / (nof_columns * nof_banks);
  offset_local = offset_in_bursts % (nof_columns * nof_banks);
  column_end = offset_local / nof_banks;
  bank_end = offset_local % nof_banks;

  /*
   *  Build the bist addresses
   */

  bist_addr_start_local |= SOC_SAND_SET_FLD_IN_PLACE(bank_start, 0, SOC_SAND_BITS_MASK(2, 0));
  bist_addr_start_local |= SOC_SAND_SET_FLD_IN_PLACE((column_start/8), 3,
                                             SOC_SAND_BITS_MASK(log_base_2_nof_columns - 1, 3));
  bist_addr_start_local |= SOC_SAND_SET_FLD_IN_PLACE(row_start, log_base_2_nof_columns,
                                             SOC_SAND_BITS_MASK(25, log_base_2_nof_columns));

  bist_addr_end_local |= SOC_SAND_SET_FLD_IN_PLACE(bank_end, 0, SOC_SAND_BITS_MASK(2, 0));
  /* Petra code. Almost not in use. Ignore coverity defects */
  /* coverity[large_shift] */
  bist_addr_end_local |= SOC_SAND_SET_FLD_IN_PLACE((column_end/8), 3,
                                            SOC_SAND_BITS_MASK(log_base_2_nof_columns - 1, 3));
  bist_addr_end_local |= SOC_SAND_SET_FLD_IN_PLACE(row_end, log_base_2_nof_columns,
                                            SOC_SAND_BITS_MASK(25, log_base_2_nof_columns));

  *bist_addr_start = bist_addr_start_local;
  *bist_addr_end = bist_addr_end_local;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_offset2bist_addr_convert()",0,0);
}


/*********************************************************************
 *     Write in a range of addresses a specific pattern under the
 *     assumption that the pre-settings of the BIST are set.
 *********************************************************************/
uint32
  soc_petra_diag_dram_access(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO      *info,
    SOC_SAND_IN  uint32                         size_in_bytes,
    SOC_SAND_IN  uint32                         *buffer,
    SOC_SAND_IN  uint8                        is_read_and_write_performed
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    buff_idx,
    nof_addresses,
    address_start,
    address_end,
    bist_addr_start=0,
    bist_addr_end=0,
    nof_actions_fld,
    indx,
    res;
  uint8
    has_succeeded = FALSE;
  SOC_PETRA_POLL_INFO
    poll_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_BASIC_WRITE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_DIAG_DRAM_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_DIAG_DRAM_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_DIAG_DRAM_TIMER_DELAY_MSEC;

  address_start = info->address;
  address_end = address_start + size_in_bytes;
  SOC_SAND_ERR_IF_ABOVE_MAX(
    address_start,  address_end,
    SOC_PETRA_DIAG_BIST_ADDRESS_ILLEGAL_RANGE_ERR, 4, exit
  );

  res = soc_petra_diag_offset2bist_addr_convert(
          unit,
          address_start,
          address_end,
          &bist_addr_start,
          &bist_addr_end
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  nof_addresses = SOC_SAND_RNG_COUNT(bist_addr_start, bist_addr_end);
  if (info->is_infinite_nof_actions == TRUE)
  {
    nof_actions_fld = 0;
  }
  else
  {
    if (is_read_and_write_performed == TRUE)
    {
      nof_actions_fld =  2 * nof_addresses;
    }
    else
    {
      nof_actions_fld = nof_addresses;
    }
    /*
     *  Set the number of actions to at least 2,
     *  otherwise the BistFinsihed bit might not rise
     */
    nof_actions_fld = SOC_SAND_MAX(2, nof_actions_fld);
  }

  SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x0, 7, exit);

  /*
   *  Set the number of actions to be performed
   */
  SOC_PETRA_FLD_ISET(regs->drc.bist_number_of_actions_reg.bist_num_actions, nof_actions_fld, dram_ndx, 10, exit);

  /*
   *  Write the address
   */
  SOC_PETRA_REG_ISET(regs->drc.bist_start_address_reg, bist_addr_start, dram_ndx, 20, exit);
  SOC_PETRA_REG_ISET(regs->drc.bist_end_address_reg, bist_addr_end, dram_ndx, 30, exit);

  /*
   *  Write the pattern -  if is_data_eight_longs, the data is written
   *  in format 255:0, we invert it in 31:0, 63:32,..,255:224.
   *  Otherwise, the data is duplicated 8 times
   */
  for (indx = 0; indx < SOC_PETRA_DIAG_BIST_DATA_PATTERN_NOF_REGS; ++indx )
  {
    if (info->is_data_size_bits_256_not_32)
    {
      buff_idx = SOC_PETRA_DIAG_BIST_DATA_PATTERN_NOF_REGS - indx - 1;
    }
    else
    {
      buff_idx = 0;
    }

    SOC_PETRA_REG_ISET(regs->drc.bist_pattern_words_regs[indx], buffer[buff_idx], dram_ndx, 50, exit);
  }

  /*
   *  Enable the independent R/W mode: always 1 for BIST
   * (possible interference with the training sequence -
   *  always disable for training sequence)
   */
  SOC_PETRA_FLD_ISET(regs->drc.bist_configurations_reg.ind_wr_rd_addr_mode, 0x1, dram_ndx, 10, exit);

  /*
   *  Enable the BIST
   */
  SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x1, 70, exit);


  /*
   *  Check that the number of actions falls to zero for a finite mode
   */
  if (info->is_infinite_nof_actions == FALSE)
  {
    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->drc.bist_statuses_reg.bist_finished),
            dram_ndx,
            &poll_info,
            &has_succeeded
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);

    if (has_succeeded == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DRAM_ACCESS_TIMEOUT_ERR, 80, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_access()",0,0);
}


/*********************************************************************
 *     Read and compare with an expected pattern in a range of addresses
 *     under the assumption that the pre-settings of the BIST are set.
 *********************************************************************/
STATIC
  uint32
    soc_petra_diag_dram_basic_read_and_inner(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  uint32                             dram_ndx,
      SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO         *info,
      SOC_SAND_IN  uint32                            *expected_buffer,
      SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status
    )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    erroneous_bits_local = 0,
    nof_addr_errors_local = 0,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_BASIC_READ_AND_COMPARE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(expected_buffer);
  SOC_SAND_CHECK_NULL_INPUT(status);

  res = soc_petra_diag_dram_access(
          unit,
          dram_ndx,
          info,
          0,
          expected_buffer,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Count the number of erroneous bits (between 0 and 32)
   */
  SOC_PETRA_REG_IGET(regs->drc.bist_error_occurred_reg.err_occurred, erroneous_bits_local, dram_ndx, 110 ,exit);

  /*
   *  Count the number of different addresses with the expected pattern
   */
  SOC_PETRA_REG_IGET(regs->drc.bist_full_mask_error_counter_reg.full_err_cnt,nof_addr_errors_local,dram_ndx,120,exit);

  status->error_bits_global = erroneous_bits_local;
  status->nof_addr_with_errors = nof_addr_errors_local;
  if ((erroneous_bits_local == 0)  && (nof_addr_errors_local == 0))
  {
    status->success = TRUE;
  }
  else
  {
    status->success = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_basic_read_and_inner()",0,0);
}

/*********************************************************************
*     Write a pattern of 256 bits long into the DRAM via the
*     BIST mechanism.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dram_write_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO     *info,
    SOC_SAND_IN  uint32                        *buffer
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_WRITE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  /*
   *  Preset of the BIST configuration
   */
  res = soc_petra_diag_dram_presettings_set(
          unit,
          dram_ndx,
          0,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Write via the BIST mechanism
   */
  res = soc_petra_diag_dram_access(
          unit,
          dram_ndx,
          info,
          0,
          buffer,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_write_unsafe()",0,0);
}

/*********************************************************************
*     Indicates if a pattern is written in the specified
*     addresses by reading and comparing the written pattern
*     with the expected data via the BIST mechanism.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dram_read_and_compare_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                            dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO         *info,
    SOC_SAND_IN  uint32                            *buff_expected,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_READ_AND_COMPARE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(buff_expected);
  SOC_SAND_CHECK_NULL_INPUT(status);

  /*
   *  Preset of the BIST configuration
   */
  res = soc_petra_diag_dram_presettings_set(
          unit,
          dram_ndx,
          1,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Read and comparing via the BIST mechanism
   */
  res = soc_petra_diag_dram_basic_read_and_inner(
          unit,
          dram_ndx,
          info,
          buff_expected,
          status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_read_and_compare_unsafe()",0,0);
}

/*********************************************************************
*     Verify BIST test parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_start_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO  *test_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_BIST_TEST_START_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(test_info);
  if (test_info->data_mode >= SOC_PETRA_DIAG_NOF_QDR_BIST_DATA_MODES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_DATA_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (test_info->data_pattern >= SOC_PETRA_DIAG_NOF_BIST_DATA_PATTERNS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_DATA_PATERN_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (test_info->start_addr > test_info->end_addr)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_ADDRESS_ILLEGAL_RANGE_ERR, 50, exit);
  }
  if (test_info->address_mode == SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST &&
      test_info->data_mode != SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_NORMAL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_DATA_MODE_OUT_OF_RANGE_ERR, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_qdr_BIST_test_start_verify()",0,0);
}


STATIC uint32
  soc_petra_diag_qdr_BIST_test_counters_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *res_info
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_BIST_TEST_COUNTERS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(res_info);

  regs = soc_petra_regs();
  SOC_PETRA_REG_GET(regs->qdr.bist_error_burst_counter_reg,res_info->burst_err_counter,5,exit);
  SOC_PETRA_REG_GET(regs->qdr.bist_error_bit_counter_reg,res_info->reply_err_counter,10,exit);
  SOC_PETRA_REG_GET(regs->qdr.bist_error_occurred_reg,res_info->bits_error_bitmap,20,exit);
  SOC_PETRA_REG_GET(regs->qdr.bist_global_error_counter_reg,res_info->bit_err_counter,30,exit);
  SOC_PETRA_REG_GET(regs->qdr.bist_error_address_reg,res_info->last_addr_err,40,exit);
  SOC_PETRA_REG_GET(regs->qdr.bist_error_data1_reg,res_info->last_data_err,50,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_qdr_BIST_test_counters_get_unsafe()",0,0);
}

STATIC
  uint32
    soc_petra_diag_qdr_init(
      SOC_SAND_IN int unit
    )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_POLL_INFO
    poll_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  Reset
   */
  SOC_PETRA_REG_SET(regs->qdr.qdr_controller_reset_reg,         0x0, 20, exit);
  SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.qdio_reset,     0x0, 30, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_controller_reset_reg,         0x1, 40, exit);
  SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.qdio_reset,     0x1, 50, exit);
  SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.ddl_init_reset, 0x1, 60, exit);
  SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.ddl_init_reset, 0x0, 70, exit);

  /*
   *  Wait for controller lock
   */
  poll_info.expected_value      = 0x1;
  poll_info.busy_wait_nof_iters = 0;
  poll_info.timer_nof_iters     = 5;
  poll_info.timer_delay_msec    = 1000;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.qdr_reply_lock_reg.reply_lock),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_QDR_REPLY_LOCK_CANNOT_LOCK_ERR, 90, exit);
  }

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.dll_control_reg.dll_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_QDR_DLL_NOT_READY_ERR, 110, exit);
  }

  SOC_PETRA_FLD_GET(regs->qdr.qdr_training_fail_reg.fail, fld_val, 120, exit);
  if (fld_val == 0x1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_QDR_TRAINING_FAIL_ERR, 130, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_qdr_init()", 0, 0);
}

/*********************************************************************
*     Run BIST test with the given parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_start_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *test_info
  )
{
  SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO
    res_info;
  SOC_PETRA_REGS
    *regs;
  uint32
    indx,
    trng_seq_reg,
    pat_val,
    pat_val_curr,
    bist_pattern_bit_mode_fld = 0x0,
    bist_data_random_mode_fld = 0x0,
    bist_data_shift_mode_fld = 0x0,
    bist_address_shift_mode_fld = 0x0,
    bist_address_test_fld = 0x0;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_BIST_TEST_START_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(test_info);

  regs = soc_petra_regs();
 /*
  * Disable the BIST operation - will be started with
  * the requested settings after all the attributes are configured.
  */
  SOC_PETRA_REG_SET(regs->qdr.qdr_bist_enable_reg,0x0, 3,exit);

  SOC_PETRA_REG_SET(regs->qdr.bist_read_delay_reg,SOC_PETRA_DIAG_BIST_READ_DELAY,5,exit);

  /*
  * number of commands to perform, if this is 0 (infinite) so will be configured later
  * and this configuration has no effect.
  */
  SOC_PETRA_REG_SET(regs->qdr.bist_write_number_configuration_reg,test_info->nof_cmnds_write, 10 ,exit);
  SOC_PETRA_REG_SET(regs->qdr.bist_read_number_configuration_reg,test_info->nof_cmnds_read, 20 ,exit);
  /*
   * test run till stop operation is called.
   */
  if (test_info->nof_cmnds_write == SOC_PETRA_DIAG_QDR_BIST_INFINITE_TEST)
  {
    SOC_PETRA_FLD_SET(regs->qdr.bist_infinite_test_reg.bist_write_infinite, TRUE ,30, exit);
    SOC_PETRA_FLD_SET(regs->qdr.bist_infinite_test_reg.bist_read_infinite, TRUE ,40, exit);
  }
  else
  {
    SOC_PETRA_FLD_SET(regs->qdr.bist_infinite_test_reg.bist_write_infinite, FALSE, 45, exit);
    SOC_PETRA_FLD_SET(regs->qdr.bist_infinite_test_reg.bist_read_infinite, FALSE ,50, exit);
  }

  SOC_PETRA_FLD_SET(regs->qdr.bist_offset_address_reg.bist_offset_address, test_info->read_offset, 52, exit);
 /*
  * write the pattern values
  */

  switch(test_info->data_pattern)
  {
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF:
    pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE:
    pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO:
    pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1:
    pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2:
    pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2_VAL;
    break;
  default:
    pat_val = 0;
    break;
  }
  
  if (test_info->data_pattern == SOC_PETRA_DIAG_BIST_DATA_PATTERN_USER_DEF) 
  {
    /*
     * For user defined case just copy values from input
     */
    for (indx = 0; indx < SOC_PETRA_DIAG_BIST_DATA_PATTERN_NOF_REGS; ++indx )
    {
      pat_val_curr = test_info->user_defined_pattern[indx];
    SOC_PETRA_REG_SET(regs->qdr.bist_pattern_regs[indx], pat_val_curr, 54, exit);
    }
  }
  else
  {
    for (indx = 0; indx < SOC_PETRA_DIAG_BIST_DATA_PATTERN_NOF_REGS; ++indx )
    {
        /*
         *  For QDR-clear, the
         *  even registers (18-bit LSB) are '0'
         */
        if (
            (
              (test_info->data_pattern == SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1) || (
               test_info->data_pattern == SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2)
            ) &&
            (
              indx%2 == 0
            )
           )
        {
          pat_val_curr = 0x0;
        }
        else
        {
          pat_val_curr = pat_val;
        }
        SOC_PETRA_REG_SET(regs->qdr.bist_pattern_regs[indx],pat_val_curr,60,exit);
    }
  }
 /*
  * write start and end addresses
  */
  SOC_PETRA_REG_SET(regs->qdr.bist_start_address_reg.bist_start_address,test_info->start_addr,70,exit);
  SOC_PETRA_REG_SET(regs->qdr.bist_end_address_reg.bist_end_address,test_info->end_addr,80,exit);

  switch(test_info->data_mode)
  {
  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_NORMAL:
    break;
  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT:
    bist_pattern_bit_mode_fld = 0x1;
    break;
  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_RANDOM:
    bist_data_random_mode_fld = 0x1;
    break;
  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_DATA_SHIFT:
    bist_data_shift_mode_fld = 0x1;
    break;
  default:
    break;
  }

  SOC_PETRA_FLD_SET(regs->qdr.bist_test_mode_reg.bist_pattern_bit_mode,bist_pattern_bit_mode_fld,90,exit);
  SOC_PETRA_FLD_SET(regs->qdr.bist_test_mode_reg.bist_data_random_mode,bist_data_random_mode_fld,100,exit);
  SOC_PETRA_FLD_SET(regs->qdr.bist_test_mode_reg.bist_data_shift_mode,bist_data_shift_mode_fld,110,exit);

  switch(test_info->address_mode)
  {
  case SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_NORMAL:
    break;
  case SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_SHIFT:
    bist_address_shift_mode_fld = 0x1;
    break;
  case SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST:
    bist_address_test_fld = 0x1;
    break;
  default:
    break;
  }
  SOC_PETRA_FLD_SET(regs->qdr.bist_test_mode_reg.bist_address_shift_mode, bist_address_shift_mode_fld, 120, exit);
  SOC_PETRA_FLD_SET(regs->qdr.bist_test_mode_reg.bist_address_test, bist_address_test_fld, 130, exit);

 /*
  * before enabling the BIST
  *   - disable QDR PHY training
  *   - clear counters.
  */
  SOC_PETRA_REG_GET(regs->qdr.periodic_enable_reg, trng_seq_reg, 135, exit);
  res = soc_petra_sw_db_qdr_dll_periodic_trng_reg_set(
          unit,
          trng_seq_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 137, exit);

  SOC_PETRA_FLD_SET(regs->qdr.periodic_enable_reg.prd_trn_en,FALSE,140,exit);

  res = soc_petra_diag_qdr_BIST_test_counters_get_unsafe(
          unit,
          &res_info
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  /*
   *  Initialize QDR
   */
  res = soc_petra_diag_qdr_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

 /*
  * Start the BIST operation this should be after all the configuration have been done.
  */
  SOC_PETRA_REG_SET(regs->qdr.qdr_bist_enable_reg, 0x1, 170, exit)

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_qdr_BIST_test_start_unsafe()",0,0);
}


/*********************************************************************
*     Stop BIST test and return the result.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_result_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO  *res_info
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    fld_val,
    trng_seq_reg,
    reg_val;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_BIST_TEST_RESULT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(res_info);

  regs = soc_petra_regs();

  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RES_INFO_clear(res_info);

  SOC_PETRA_FLD_GET(regs->qdr.qdr_init_status_reg.ready, fld_val, 5, exit);

  res_info->is_qdr_up = SOC_SAND_NUM2BOOL(fld_val);

  if (res_info->is_qdr_up == FALSE)
  {
    res_info->is_test_finished = TRUE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

 /*
  * Check if the test run in infinite mode.
  */
  SOC_PETRA_FLD_GET(regs->qdr.bist_infinite_test_reg.bist_write_infinite, fld_val, 10, exit);
  if (fld_val)
  {
    res_info->is_test_finished = TRUE;
  }
  else
  {
   /*
    * Check if the test finished
    */
    SOC_PETRA_REG_GET(regs->qdr.bist_finished_reg, reg_val, 20, exit);
    res_info->is_test_finished = SOC_SAND_NUM2BOOL(reg_val);
  }

  if (res_info->is_test_finished == TRUE)
  {
   /*
    * Shut down the BIST
    */
    SOC_PETRA_REG_SET(regs->qdr.qdr_bist_enable_reg, 0x0, 30, exit);

    /* Read the counters */
    res = soc_petra_diag_qdr_BIST_test_counters_get_unsafe(
            unit,
            res_info
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   /*
    * Restore the training.
    */
    res = soc_petra_sw_db_qdr_dll_periodic_trng_reg_get(
            unit,
            &trng_seq_reg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    SOC_PETRA_REG_SET(regs->qdr.periodic_enable_reg, trng_seq_reg ,50,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_qdr_BIST_test_result_get_unsafe()",0,0);
}

STATIC
  uint8
    soc_petra_diag_qdr_BIST_test_passed(
      SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *res_info
    )
{
  return ((res_info->burst_err_counter == 0)
            && (res_info->bit_err_counter == 0)
            && (res_info->reply_err_counter == 0)
            && (res_info->bits_error_bitmap == 0)) ? TRUE : FALSE;
}

uint32
  soc_petra_diag_qdr_window_validity_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN     *pattern,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO *window_info
  )
{
  uint32
    res = SOC_SAND_OK,
    qc_sel_vec_lsb,
    qc_sel_vec_msb,
    qc_sel_vec,
    dll_mstr_s;
  uint32
    dll_val,
    entry,
    consecutive_failures;
  uint8
    found_start,
    found_end,
    test_passed;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO
    run_info;
  SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO
    res_info;
  SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pattern);
  SOC_SAND_CHECK_NULL_INPUT(window_info);

  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO_clear(&run_info);
  soc_petra_PETRA_DIAG_QDR_WINDOW_INFO_clear(window_info);

  regs = soc_petra_regs();

  /*
   *  Configure the BIST
   */
  run_info.nof_cmnds_write = SOC_PETRA_DIAG_QDR_BIST_INFINITE_TEST;
  run_info.start_addr      = SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_TEST_BIST_START_ADDR;
  run_info.end_addr        = SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_TEST_BIST_END_ADDR;
  run_info.address_mode    = SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_NORMAL;
  run_info.read_offset     = 0;
  switch (pattern->mode)
  {
  case SOC_PETRA_DIAG_QDR_PATTERN_MODE_RANDOM:
    run_info.data_mode = SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_RANDOM;
    break;
  case SOC_PETRA_DIAG_QDR_PATTERN_MODE_DIFF:
    run_info.data_mode    = SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT;
    run_info.data_pattern = SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF;
    break;
  case SOC_PETRA_DIAG_QDR_PATTERN_MODE_USER_DEF:
    run_info.data_mode    = SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_NORMAL;
    run_info.data_pattern = SOC_PETRA_DIAG_BIST_DATA_PATTERN_USER_DEF;
    sal_memcpy(
      run_info.user_defined_pattern,
      pattern->data,
      sizeof(run_info.user_defined_pattern)
    );
    break;
  default:
    break;
  }

  SOC_PETRA_FLD_SET(regs->qdr.dll_control_reg.dll_slave_override_en, 0x1, 10, exit);
  
  found_start          = FALSE;
  found_end            = FALSE;
  consecutive_failures = 0;
  for (dll_val = 0; !found_end && (dll_val < 128); ++dll_val)
  {
    tbl_data.qdr_dll_mem = (dll_val << 14) | dll_val;
    for (entry = 0; entry < SOC_PETRA_DIAG_QDR_DLL_MEMORY_TBL_NOF_ENTRIES; ++entry)
    {
      res = soc_petra_qdr_qdr_dll_mem_tbl_set_unsafe(
              unit,
              entry,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    res = soc_petra_diag_qdr_BIST_test_start_unsafe(
            unit,
            &run_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    sal_msleep(10000);

    res = soc_petra_diag_qdr_BIST_test_result_get_unsafe(
            unit,
            &res_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    test_passed = soc_petra_diag_qdr_BIST_test_passed(&res_info);
    if (test_passed)
    {
      consecutive_failures = 0;
    }
    else
    {
      ++consecutive_failures;
    }
    if (!found_start && test_passed)
    {
      window_info->start = dll_val;
      found_start        = TRUE;
    }
    else if (found_start && (consecutive_failures == SOC_PETRA_DIAG_QDR_NOF_FAILURES_FOR_WINDOW_END))
    {
      window_info->end = dll_val - SOC_PETRA_DIAG_QDR_NOF_FAILURES_FOR_WINDOW_END;
      found_end        = TRUE;
    }
  }

  if (!found_start)
  {
    /*
     *  Board is probably faulty
     */
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_QDR_CANNOT_ACCESS_ERR, 50, exit);
  }

  SOC_PETRA_FLD_GET(regs->qdr.qdr_qc_sel_vec_lsb_reg.qc_sel_vec_lsb, qc_sel_vec_lsb, 60, exit);
  SOC_PETRA_FLD_GET(regs->qdr.qdr_training_fail_reg.qc_sel_vec_msb, qc_sel_vec_msb, 70, exit);
  qc_sel_vec = (qc_sel_vec_msb << 7) | qc_sel_vec_lsb;
  window_info->total_size = 4 * qc_sel_vec;

  window_info->ratio_percent = (100 * window_info->total_size) / window_info->total_size;

  SOC_PETRA_FLD_GET(regs->qdr.qdr_init_status_reg.dll_mstr_s, dll_mstr_s, 80, exit);
  window_info->sampling_point = 2 * dll_mstr_s;

  SOC_PETRA_FLD_SET(regs->qdr.dll_control_reg.dll_slave_override_en, 0x0, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_qdr_window_validity_get()", 0, 0);
}

uint32
  soc_petra_diag_qdr_window_validity_get_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern
  )
{
  /*uint32
    res = SOC_SAND_OK;*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_WINDOW_VALIDITY_GET_VERIFY);*/

  /*SOC_PETRA_STRUCT_VERIFY(PETRA_DIAG_QDR_PATTERN, pattern, 10, exit);*/

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_qdr_window_validity_get_verify()", 0, 0);
}

/*********************************************************************
*     Run DRAM BIST test with the given parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_start_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info
  )
{
  uint32
    nof_banks,
    nof_drams,
    nof_cols;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_BIST_TEST_START_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(test_info);
  res = soc_petra_mmu_dram_address_space_info_get_unsafe(
          unit,
          &nof_drams,
          &nof_banks,
          &nof_cols
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (dram_ndx > SOC_PETRA_DIAG_DRAM_NDX_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (test_info->data_mode >= SOC_PETRA_DIAG_NOF_DRAM_BIST_DATA_MODES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_DATA_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (test_info->data_pattern >= SOC_PETRA_DIAG_NOF_BIST_DATA_PATTERNS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_DATA_PATERN_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (test_info->start_addr > test_info->end_addr)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_BIST_ADDRESS_ILLEGAL_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_BIST_test_start_verify()",0,0);
}

/*********************************************************************
*     Run DRAM BIST test with the given parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_start_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    pat_val[1] = {0},
    size_in_bytes,
    reg_val,
    pattern_fld_val = 0,
    random_fld_val = 0,
    trng_seq_reg,
    res;
  uint8
    is_infinite_nof_actions,
    is_read_and_write;
  SOC_PETRA_DIAG_DRAM_ACCESS_INFO
    info_local;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_BIST_TEST_START_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(test_info);
  regs = soc_petra_regs();

  soc_petra_PETRA_DIAG_DRAM_ACCESS_INFO_clear(&info_local);

  /*
   *  Preset of the BIST configuration and read write per cycle.
   */
  res = soc_petra_diag_dram_presettings_set(
          unit,
          dram_ndx,
          test_info->reads_per_cycle,
          test_info->writes_per_cycle
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_REG_IGET(regs->drc.bist_configurations_reg, reg_val, dram_ndx, 5 ,exit);

 /*
  * set data mode
  */
  switch(test_info->data_mode)
  {
  case SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE_NORMAL:
    break;
  case SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE_PATTERN_BIT:
    pattern_fld_val = 0x1;
    break;
  case SOC_PETRA_DIAG_DRAM_DATA_MODE_RANDOM:
    random_fld_val = 0x1;
    break;
  default:
    break;
  }

  SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.pattern_bit_mode, pattern_fld_val, reg_val, 10, exit);
  SOC_PETRA_FLD_TO_REG(regs->drc.bist_configurations_reg.prbsmode, random_fld_val, reg_val, 20, exit);

  SOC_PETRA_REG_ISET(regs->drc.bist_configurations_reg, reg_val, dram_ndx, 30 ,exit);

 /*
  * write the pattern values
  */
  switch(test_info->data_pattern)
  {
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF:
    *pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE:
    *pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO:
    *pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1:
    *pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1_VAL;
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2:
    *pat_val = SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2_VAL;
    break;
  default:
    *pat_val = 0;
    break;
  }

 /*
  * Disable the DLL auto training.
  * Clear Counters
  */
  SOC_PETRA_REG_IGET(regs->dpi.ddl_periodic_training_reg, trng_seq_reg, dram_ndx, 77, exit);
  res = soc_petra_sw_db_dram_dll_periodic_trng_reg_set(
          unit,
          trng_seq_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 78, exit);

  SOC_PETRA_FLD_ISET(regs->dpi.ddl_periodic_training_reg.ddl_auto_trn_en,FALSE,dram_ndx,80,exit);
  SOC_PETRA_REG_IGET(regs->drc.bist_global_error_counter_reg, reg_val, dram_ndx, 85, exit);

  /*
   *  We distinguish only finite and infinite number of actions
   */
  if (test_info->is_finite_nof_cmnds == 0)
  {
    is_infinite_nof_actions = TRUE;
  }
  else
  {
    is_infinite_nof_actions = FALSE;
  }

  /*
   *  The number of commands is influenced if both reads and writes are performed
   */
  if ((test_info->reads_per_cycle == 0) || (test_info->writes_per_cycle == 0))
  {
    is_read_and_write = FALSE;
  }
  else
  {
    is_read_and_write = TRUE;
  }

  /*
   *  Reads / Writes the data
   */
  if (!(
        (test_info->reads_per_cycle == 0)
        &&(test_info->writes_per_cycle == 0)
       )
      )
  {
    info_local.address = test_info->start_addr;
    size_in_bytes = test_info->end_addr - test_info->start_addr;
    info_local.is_data_size_bits_256_not_32 = FALSE;
    info_local.is_infinite_nof_actions = is_infinite_nof_actions;

    res = soc_petra_diag_dram_access(
            unit,
            dram_ndx,
            &info_local,
            size_in_bytes,
            pat_val,
            is_read_and_write
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_BIST_test_start_unsafe()",0,0);
}

/*********************************************************************
*     Stop DRAM BIST test and return the result.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_result_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *res_info
  )
{
  uint32
    reg_val,
    fld_val,
    trng_seq_reg,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_BIST_TEST_RESULT_GET);

  SOC_SAND_CHECK_NULL_INPUT(res_info);

  regs = soc_petra_regs();

  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO_clear(res_info);

  if (dram_ndx > SOC_PETRA_DIAG_DRAM_NDX_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 3, exit);
  }

  SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.ready, fld_val, dram_ndx, 5, exit);

  res_info->is_dram_up = SOC_SAND_NUM2BOOL(fld_val);

  if (res_info->is_dram_up == FALSE)
  {
    res_info->is_test_finished = TRUE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /*
   *  The checking of the test end is already performed in the test start function
   */
  res_info->is_test_finished = TRUE;

  if (res_info->is_test_finished == TRUE)
  {
    /*
     *  Read the counter
     */
    SOC_PETRA_REG_IGET(regs->drc.bist_global_error_counter_reg, reg_val, dram_ndx, 40, exit);
    res_info->reply_err_counter = reg_val;

   /*
    * Shut down the BIST
    */
    SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x0 , 70, exit);

   /*
    * Restore the training and set the independent mode to 0x0 for it.
    */
    fld_val = 0x0;
    SOC_PETRA_FLD_ISET(regs->drc.bist_configurations_reg.ind_wr_rd_addr_mode, fld_val, dram_ndx, 45, exit);

    res = soc_petra_sw_db_dram_dll_periodic_trng_reg_get(
            unit,
            &trng_seq_reg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    SOC_PETRA_REG_ISET(regs->dpi.ddl_periodic_training_reg, trng_seq_reg, dram_ndx, 80, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_BIST_test_result_get_unsafe()",0,0);
}


STATIC uint32
  soc_petra_diag_lpm_port_state_save(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32               port_indx,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     nif_ndx
  )
{
  SOC_PETRA_SW_DB_LBG_PORT
    port_stat;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LPM_PORT_STATE_SAVE);

 /*
  * get the hardware status and save it.
  */
  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_indx,
          &(port_stat.headers[SOC_PETRA_PORT_DIRECTION_INCOMING]),
          &(port_stat.headers[SOC_PETRA_PORT_DIRECTION_OUTGOING])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ports_forwarding_header_get_unsafe(
          unit,
          port_indx,
          &(port_stat.itmh)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (nif_ndx != SOC_PETRA_IF_ID_CPU)
  {
    res = soc_petra_nif_loopback_get_unsafe(
            unit,
            nif_ndx,
            &(port_stat.is_nif_lpbck_on)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else
  {
    port_stat.is_nif_lpbck_on = TRUE;
  }

  port_stat.saved_valid = TRUE;

  res = soc_petra_sw_db_lbg_port_stat_save(
          unit,
          port_indx,
          &port_stat
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lpm_port_state_save()",0,0);
}


STATIC uint32
  soc_petra_diag_lpm_port_state_load_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                          port_indx
  )
{
  uint8
    saved;
  SOC_PETRA_SW_DB_LBG_PORT
    port_stat;
  uint32
    res;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    port2if_in,
    port2if_out;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LPM_PORT_STATE_LOAD_UNSAFE);

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_in);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_out);

 /*
  * Consider only saved configurations (modified since).
  */
  saved =
    soc_petra_sw_db_lbg_port_save_valid_get(
      unit,
      port_indx
    );

  if (saved == TRUE)
  {
    res = soc_petra_sw_db_lbg_port_stat_load(
            unit,
            port_indx,
            &port_stat
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            port_indx,
            &port2if_in,
            &port2if_out
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

    if (port2if_in.if_id != SOC_PETRA_IF_ID_CPU)
    {
      res = soc_petra_nif_loopback_set_unsafe(
              unit,
              port2if_in.if_id,
              port_stat.is_nif_lpbck_on
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
    }

   /*
    * get the hardware status and save it.
    */
    res = soc_petra_port_header_type_set_unsafe(
            unit,
            port_indx,
            SOC_PETRA_PORT_DIRECTION_INCOMING,
            port_stat.headers[SOC_PETRA_PORT_DIRECTION_INCOMING]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_port_header_type_set_unsafe(
            unit,
            port_indx,
            SOC_PETRA_PORT_DIRECTION_OUTGOING,
            port_stat.headers[SOC_PETRA_PORT_DIRECTION_OUTGOING]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_ports_forwarding_header_set_unsafe(
            unit,
            port_indx,
            &(port_stat.itmh)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    soc_petra_sw_db_lbg_port_save_valid_set(
        unit,
        port_indx,
        FALSE
      );
  } /* Previous configuration saved */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lpm_port_state_load_unsafe()",port_indx,0);
}

STATIC
  uint32
    soc_pa_diag_lbg_conf_set_unsafe(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
    )
{
  uint32
    port_indx;
  uint32
    next_local_port_id,
    cur_local_port_id,
    sys_port_id,
    sys_fap_id_self;
  SOC_PETRA_PORTS_ITMH
    itmh;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    port2if_in,
    port2if_out;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_DIAG_LBG_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lbg_info);

  soc_petra_PETRA_PORTS_ITMH_clear(&itmh);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_in);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_out);

  res = soc_petra_diag_lpm_port_state_save(
          unit,
          SOC_PETRA_DIAG_LBG_CPU_PORT_ID,
          SOC_PETRA_IF_ID_CPU
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * set CPU port to raw and to send to the first port in the list.
  */
  res = soc_petra_port_header_type_set_unsafe(
          unit,
          SOC_PETRA_DIAG_LBG_CPU_PORT_ID,
          SOC_PETRA_PORT_DIRECTION_BOTH,
          SOC_PETRA_PORT_HEADER_TYPE_RAW
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  itmh.base.destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
        unit,
        &sys_fap_id_self
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_local_to_sys_phys_port_map_get_unsafe(
          unit,
          sys_fap_id_self,
          lbg_info->path.ports[0],
          &sys_port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  itmh.base.destination.id = sys_port_id;

  res = soc_petra_ports_forwarding_header_set_unsafe(
          unit,
          SOC_PETRA_DIAG_LBG_CPU_PORT_ID,
          &itmh
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 /*
  * build snake that includes all the given ports.
  */
  for (port_indx = 0; port_indx < lbg_info->path.nof_ports; ++port_indx)
  {
   /*
    * if the previous status of the ports is not already saved, save it.
    */
    cur_local_port_id = lbg_info->path.ports[port_indx];

    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            cur_local_port_id,
            &port2if_in,
            &port2if_out
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    res = soc_petra_diag_lpm_port_state_save(
            unit,
            cur_local_port_id,
            port2if_in.if_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
   /*
    * if this is not last port make it forward to the next port in the list
    */
    if (port_indx < lbg_info->path.nof_ports - 1)
    {
      next_local_port_id = lbg_info->path.ports[port_indx + 1];
    }
   /*
    * otherwise make it forward back to the first port in the list (make loop).
    */
    else
    {
      next_local_port_id = lbg_info->path.ports[0];
    }

    res = soc_petra_local_to_sys_phys_port_map_get_unsafe(
            unit,
            sys_fap_id_self,
            next_local_port_id,
            &sys_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    itmh.base.destination.id = sys_port_id;

    res = soc_petra_port_header_type_set_unsafe(
            unit,
            cur_local_port_id,
            SOC_PETRA_PORT_DIRECTION_BOTH,
            SOC_PETRA_PORT_HEADER_TYPE_RAW
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_petra_ports_forwarding_header_set_unsafe(
            unit,
            cur_local_port_id,
            &itmh
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_petra_nif_loopback_set_unsafe(
            unit,
            port2if_in.if_id,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_diag_lbg_conf_set_unsafe()",0,0);
}


uint32
  soc_petra_diag_lbg_conf_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_CONF_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(diag_lbg_conf_set_unsafe, (unit, lbg_info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_conf_set_unsafe()",0,0);
}


uint32
  soc_petra_diag_lbg_traffic_send_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_TRAFFIC_INFO        *traffic_info
  )
{
  uint32
    packet_idx,
    pattern_idx,
    data_idx;
  SOC_PETRA_PKT_TX_PACKET_INFO
    *packet=NULL;
  uint32
    res;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_TRAFFIC_SEND_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traffic_info);

  SOC_PETRA_ALLOC(packet, SOC_PETRA_PKT_TX_PACKET_INFO, 1);
  soc_petra_PETRA_PKT_TX_PACKET_INFO_clear(packet);

 

 /*
  * send packet from the CPU to port in ports[0].
  */
  packet->path_type = SOC_PETRA_PACKET_SEND_PATH_TYPE_INGRESS;

  if (traffic_info->packet_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_LBG_PATTERN_DATA_SIZE_OUT_OF_RANGE_ERR, 3, exit);
  }

  if (traffic_info->pattern.data_byte_size > SOC_PETRA_DIAG_LBG_PAYLOAD_BYTE_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_LBG_PATTERN_DATA_SIZE_OUT_OF_RANGE_ERR, 5, exit);
  }

  packet->packet.data_byte_size = traffic_info->packet_size;

  for (data_idx = 0; data_idx < traffic_info->packet_size; data_idx++)
  {
    pattern_idx = data_idx % traffic_info->pattern.data_byte_size;
    packet->packet.data[SOC_PETRA_PKT_MAX_CPU_PACKET_BYTE_SIZE - 1 - data_idx] = traffic_info->pattern.data[pattern_idx];
  }

  for (packet_idx = 0; packet_idx < traffic_info->nof_packets; ++packet_idx)
  {
    res = soc_petra_pkt_packet_send_unsafe(
            unit,
            packet
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    sal_msleep(10);
  }

  res = soc_petra_sw_db_lbg_packet_size_set(
          unit,
          traffic_info->pattern.data_byte_size
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_PETRA_FREE(packet);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_traffic_send_unsafe()",0,0);
}



uint32
  soc_petra_diag_lbg_result_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT  SOC_PETRA_DIAG_LBG_RESULT_INFO        *res_info
  )
{
  SOC_SAND_64CNT
    iqm_deq,
    ipt_deq;
  SOC_SAND_U64
    u64_res,
    u64_res2;
  uint32
    premature,
    credit_worth;
  uint32
    level,
    packet_size;
  uint32
    over;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_RESULT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(res_info);

  level = SOC_PETRA_STAT_PRINT_LEVEL_ALL;

  res = soc_petra_stat_all_counters_get_unsafe(
          unit,
          level,
          0,
          &(res_info->counters)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_stat_counter_get_unsafe(
          unit,
          SOC_PETRA_IQM_DEQUEUE_PACKET_CNT,
          &(iqm_deq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_stat_counter_get_unsafe(
          unit,
          SOC_PETRA_EGQ_IPT_PACKET_CNT,
          &(ipt_deq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  soc_sand_os_nano_sleep(SOC_PETRA_DIAG_LBG_SLEEP_NANO, &premature);

  if (premature == 0)
  {
    premature = SOC_PETRA_DIAG_LBG_SLEEP_NANO;
  }

  res = soc_petra_stat_counter_get_unsafe(
          unit,
          SOC_PETRA_IQM_DEQUEUE_PACKET_CNT,
          &(iqm_deq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_stat_counter_get_unsafe(
          unit,
          SOC_PETRA_EGQ_IPT_PACKET_CNT,
          &(ipt_deq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_stat_all_counters_get_unsafe(
          unit,
          level,
          0,
          &(res_info->counters)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_sw_db_lbg_packet_size_get(
          unit,
          &packet_size
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

 /*
  * ingress rate.
  */
  over = soc_sand_u64_to_long(
        &(iqm_deq.u64),
        &(res_info->ingress_rate)
      );

  if (over)
  {
    soc_sand_u64_devide_u64_long(
            &(iqm_deq.u64),
            premature / 1000000,
            &u64_res
          );

    soc_sand_u64_to_long(
            &u64_res,
            &(res_info->ingress_rate)
          );
    res_info->ingress_rate *= packet_size;
  }
  else
  {
    soc_sand_u64_multiply_longs(
        res_info->ingress_rate,
        packet_size,
        &iqm_deq.u64
      );

    soc_sand_u64_devide_u64_long(
            &(iqm_deq.u64),
            premature / 1000000,
            &u64_res
          );

    soc_sand_u64_to_long(
            &u64_res,
            &(res_info->ingress_rate)
          );
  }

 /*
  * Egress rate.
  */

  over = soc_sand_u64_to_long(
        &(ipt_deq.u64),
        &(res_info->egress_rate)
      );

  if (over)
  {
    soc_sand_u64_devide_u64_long(
            &(ipt_deq.u64),
            premature / 1000000,
            &u64_res
          );

    soc_sand_u64_to_long(
            &u64_res,
            &(res_info->egress_rate)
          );
    res_info->egress_rate *= packet_size;
  }
  else
  {
    soc_sand_u64_multiply_longs(
        res_info->egress_rate,
        packet_size,
        &ipt_deq.u64
      );

    soc_sand_u64_devide_u64_long(
            &(ipt_deq.u64),
            premature / 1000000,
            &u64_res
          );

    soc_sand_u64_to_long(
            &u64_res,
            &(res_info->egress_rate)
          );
  }

 /*
  * Credits rate.
  */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  over = soc_sand_u64_to_long(
        &(res_info->counters.counters[SOC_PETRA_SCH_CREDIT_CNT].u64),
        &(res_info->credits)
      );

  if (over)
  {
    soc_sand_u64_devide_u64_long(
            &(res_info->counters.counters[SOC_PETRA_SCH_CREDIT_CNT].u64),
            1000,
            &(u64_res)
          );

    soc_sand_u64_to_long(
            &u64_res,
            &(res_info->credits)
          );

    res_info->credits *= credit_worth;
  }
  else
  {
    soc_sand_u64_multiply_longs(
        res_info->credits,
        credit_worth,
        &u64_res2
      );

    soc_sand_u64_devide_u64_long(
            &u64_res2,
            premature / 1000000,
            &u64_res
          );

    soc_sand_u64_to_long(
            &u64_res,
            &(res_info->credits)
          );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_result_get_unsafe()",0,0);
}



uint32
  soc_pa_diag_lbg_close_unsafe(
    SOC_SAND_IN  int                          unit
  )
{
  uint32
    port_indx;
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_DIAG_LBG_CLOSE_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->iqm.iqm_enablers_reg.dscrd_all_pkt, 0x1, 15, exit);
  sal_msleep(100);
  SOC_PETRA_FLD_SET(regs->iqm.iqm_enablers_reg.dscrd_all_pkt, 0x0, 17, exit);

  for (port_indx = 0; port_indx < SOC_PETRA_NOF_FAP_PORTS; ++port_indx)
  {
    res = soc_petra_diag_lpm_port_state_load_unsafe(
            unit,
            port_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_diag_lbg_close_unsafe()",0,0);
}

uint32
  soc_petra_diag_lbg_close_unsafe(
    SOC_SAND_IN  int                          unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_CLOSE_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(diag_lbg_close_unsafe, (unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_close_unsafe()",0,0);
}

STATIC
  uint32
    soc_petra_diag_simple_mbist_reset(
      SOC_SAND_IN int unit
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  Take MCC Out of Reset
   */
  SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.mcc_reset, 0x0, 10, exit);

  /*
   *  Start MBist mode
   */
  SOC_PETRA_FLD_SET(regs->mcc.memory_configuration_reg.mbist_mode, 0x1, 15, exit);

  /*
   *  Start MBist reset sequence
   */
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x7, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x0, 21, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x4, 22, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x6, 23, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x7, 24, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x6, 25, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x0, 26, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_reset()", 0, 0);
}

STATIC
  uint32
    soc_petra_diag_simple_mbist_deassert_tap_reset(
      SOC_SAND_IN int unit
    )
{
  uint32
    iter_idx = 0,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  Deasserting TAP Reset
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_config_reg, 0x11, 10, exit);

  fld_val = 1;
  while(fld_val == 1)
  {
    SOC_PETRA_FLD_GET(regs->mcc.tap_config_reg.tap_cmd, fld_val, 15, exit);
    iter_idx++;
    if (iter_idx > SOC_PETRA_DIAG_MBIST_POLL_ITERS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_MBIST_POLL_TIMEOUT_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_deassert_tap_reset()", 0, 0);
}
STATIC
  uint32
    soc_pa_diag_simple_mbist_sms_type_get(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx,
      SOC_SAND_OUT SOC_PETRA_DIAG_MBIST_SMS_TYPE    *sms_tp
    )
{
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (
      ((sms_ndx >= 2) && (sms_ndx <= 8))
      || (sms_ndx == 10)
      || (sms_ndx == 12)
      || (sms_ndx == 14)
      || (sms_ndx == 16)
      || (sms_ndx == 19)
      || (sms_ndx == 22)
      || (sms_ndx == 24)
      || (sms_ndx == 26)
      || ((sms_ndx >= 29) && (sms_ndx <= 31))
      || (sms_ndx == 34)
      || (sms_ndx == 35)
      || (sms_ndx == 38)
      || (sms_ndx == 41)
      || (sms_ndx == 42)
    )
  {
    sms_type = SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF;
  }
  else if (
          (sms_ndx == 13)
          || (sms_ndx == 17)
          || (sms_ndx == 20)
          || (sms_ndx == 36)
          || (sms_ndx == 39)
    )
  {
    sms_type = SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D;
  }
  else
  {
    sms_type = SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_S;
  }

  *sms_tp = sms_type;
  
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_diag_simple_mbist_sms_type_get()",sms_ndx,0);
}

STATIC
  uint32
    soc_petra_diag_simple_mbist_sms_type_get(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx,
      SOC_SAND_OUT SOC_PETRA_DIAG_MBIST_SMS_TYPE    *sms_tp
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_DIFF_DEVICE_CALL(diag_simple_mbist_sms_type_get,(unit, sms_ndx, sms_tp));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_sms_type_get()",0,0);
}

uint32
  soc_pa_diag_simple_mbist_sms_sel_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *sel_sz
  )
{
  uint32
    sel_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (sms_ndx)
  {
  case 1  : sel_size = 59    ;break;
  case 2  : sel_size = 28    ;break;
  case 3  : sel_size = 27    ;break;
  case 4  : sel_size = 27    ;break;
  case 5  : sel_size = 27    ;break;
  case 6  : sel_size = 27    ;break;
  case 7  : sel_size = 27    ;break;
  case 8  : sel_size = 27    ;break;
  case 9  : sel_size = 4530  ;break;
  case 10 : sel_size = 52    ;break;
  case 11 : sel_size = 209   ;break;
  case 12 : sel_size = 68    ;break;
  case 13 : sel_size = 3064  ;break;
  case 14 : sel_size = 35    ;break;
  case 15 : sel_size = 1296  ;break;
  case 16 : sel_size = 53    ;break;
  case 17 : sel_size = 1342  ;break;
  case 18 : sel_size = 619   ;break;
  case 19 : sel_size = 45    ;break;
  case 20 : sel_size = 169   ;break;
  case 21 : sel_size = 1145  ;break;
  case 22 : sel_size = 74    ;break;
  case 23 : sel_size = 37    ;break;
  case 24 : sel_size = 155   ;break;
  case 25 : sel_size = 70    ;break;
  case 26 : sel_size = 29    ;break;
  case 27 : sel_size = 52    ;break;
  case 28 : sel_size = 52    ;break;
  case 29 : sel_size = 72    ;break;
  case 30 : sel_size = 72    ;break;
  case 31 : sel_size = 29    ;break;
  case 32 : sel_size = 1984  ;break;
  case 33 : sel_size = 1984  ;break;
  case 34 : sel_size = 40    ;break;
  case 35 : sel_size = 40    ;break;
  case 36 : sel_size = 756   ;break;
  case 37 : sel_size = 308   ;break;
  case 38 : sel_size = 38    ;break;
  case 39 : sel_size = 265   ;break;
  case 40 : sel_size = 2276  ;break;
  case 41 : sel_size = 36    ;break;
    
 default:
    sel_size = 0;
  }

  *sel_sz = sel_size;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_diag_simple_mbist_sms_sel_size_get()",sms_ndx,0);
}

uint32
  soc_petra_diag_simple_mbist_sms_sel_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *sel_sz
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_DIFF_DEVICE_CALL(diag_simple_mbist_sms_sel_size_get,(unit, sms_ndx, sel_sz));  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_sms_sel_size_get()",sms_ndx,0);
}


STATIC
  uint32
    soc_pa_diag_simple_mbist_sms_addr_size_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  uint32                     sms_ndx,
      SOC_SAND_OUT uint32                      *addr_sz
    )
{
  uint32
    addr_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (sms_ndx)
  {
  case 1  : addr_size = 9  ; break;
  case 2  : addr_size = 7  ; break;
  case 3  : addr_size = 7  ; break;
  case 4  : addr_size = 7  ; break;
  case 5  : addr_size = 7  ; break;
  case 6  : addr_size = 7  ; break;
  case 7  : addr_size = 7  ; break;
  case 8  : addr_size = 7  ; break;
  case 9  : addr_size = 13 ; break;
  case 10 : addr_size = 10 ; break;
  case 11 : addr_size = 12 ; break;
  case 12 : addr_size = 7  ; break;
  case 13 : addr_size = 11 ; break;
  case 14 : addr_size = 7  ; break;
  case 15 : addr_size = 13 ; break;
  case 16 : addr_size = 8  ; break;
  case 17 : addr_size = 13 ; break;
  case 18 : addr_size = 13 ; break;
  case 19 : addr_size = 8  ; break;
  case 20 : addr_size = 12 ; break;
  case 21 : addr_size = 12 ; break;
  case 22 : addr_size = 9  ; break;
  case 23 : addr_size = 6  ; break;
  case 24 : addr_size = 9  ; break;
  case 25 : addr_size = 13 ; break;
  case 26 : addr_size = 8  ; break;
  case 27 : addr_size = 13 ; break;
  case 28 : addr_size = 13 ; break;
  case 29 : addr_size = 8  ; break;
  case 30 : addr_size = 8  ; break;
  case 31 : addr_size = 6  ; break;
  case 32 : addr_size = 9  ; break;
  case 33 : addr_size = 9  ; break;
  case 34 : addr_size = 6  ; break;
  case 35 : addr_size = 6  ; break;
  case 36 : addr_size = 13 ; break;
  case 37 : addr_size = 13 ; break;
  case 38 : addr_size = 9  ; break;
  case 39 : addr_size = 12 ; break;
  case 40 : addr_size = 13 ; break;
  case 41 : addr_size = 8  ; break;

  default:
    addr_size = 0;
  }

  *addr_sz = addr_size;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_diag_simple_mbist_sms_addr_size_get()",sms_ndx,0);
}

uint32
  soc_petra_diag_simple_mbist_sms_addr_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *addr_sz
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_DIFF_DEVICE_CALL(diag_simple_mbist_sms_addr_size_get,(unit, sms_ndx, addr_sz));  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_sms_sel_size_get()",sms_ndx,0);
}



/*
 *  Configure the SMS number in the SFP block
 */
STATIC
  uint32
    soc_petra_diag_simple_mbist_change_sms_number(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  JPC WIR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00c, 10, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WIR start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00d, 15, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WIR enter - select config
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x3, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 25, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 30, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WDR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006009, 35, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WDR start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000600a, 40, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WIR write SMS number
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, sms_ndx, 45, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 50, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 55, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_change_sms_number()", 0, 0);
}

STATIC
  uint32
    soc_petra_diag_simple_mbist_command(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     mbist_cmd
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

/*  soc_sand_os_printf(" Performing MBist command %d \n", mbist_cmd); */

  /*
   *  SMS WIR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006006, 10, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WIR Start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006007, 15, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WIR Command
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, mbist_cmd, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0, 25, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 30, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_command()", 0, 0);
}



STATIC
  uint32
    soc_petra_diag_simple_mbist_write_tbox_reg(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx,
      SOC_SAND_IN SOC_PETRA_DIAG_SOFT_ERROR_PATTERN simple_access_ndx,
      SOC_SAND_IN uint8                     is_write_op
    )
{
  uint32
    tbox_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  Get SMS type
   */
  res = soc_petra_diag_simple_mbist_sms_type_get(
              unit,
              sms_ndx,
              &sms_type
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  /*
   *  Switch Action type
   */
  if (is_write_op == TRUE)
  {
    if (simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_DIFF1)
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x0a011c00;
      }
      else
      {
        tbox_val = 0x0c11c000;
      }
    }
    else if (simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_DIFF2)
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x0a211c00;
      }
      else
      {
        tbox_val = 0x0c51c000;
      }
    }
    else if (simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ZERO)
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x02011c00;
      }
      else
      {
        tbox_val = 0x0411c000;
      }
    }
    else /* SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ONE */
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x02211c00;
      }
      else
      {
        tbox_val = 0x0451c000;
      }
    }
  }
  else /* Read access operation*/
  {
    if (simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_DIFF1)
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x0a021c00;
      }
      else
      {
        tbox_val = 0x0c09c000;
      }
    }
    else  if (simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_DIFF2)
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x0a221c00;
      }
      else
      {
        tbox_val = 0x0c49c000;
      }
    }
    else  if (simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ZERO)
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x02021c00;
      }
      else
      {
        tbox_val = 0x0409c000;
      }
    }
    else /* simple_access_ndx == SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ONE */
    {
      if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
      {
        tbox_val = 0x02221c00;
      }
      else
      {
        tbox_val = 0x0449c000;
      }
    }
  }

/*  soc_sand_os_printf("Setting TBOX register in SMS %d, type: %d, r-w: %d, 0-1: %d\n",
    sms_ndx, sms_type, is_write_op, simple_access_ndx);*/

  /*
   *  Change SMS number
   */
  res = soc_petra_diag_simple_mbist_change_sms_number(
          unit,
          sms_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Run Tbox sel
   */
  res = soc_petra_diag_simple_mbist_command(
          unit,
          12
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
/* sal_msleep(150); */

  /*
   *  SMS WDR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006003, 20, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR write start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006004, 25, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR write command
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, tbox_val, 30, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0, 30, exit);

  /*
   *  Wirte result - 28 bits
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8001c001, 35, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  Shift 48 bits (toward MSB) inside Tbox
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80030002, 40, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  Shift 31 bits (toward MSB) inside Tbox
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8001f002, 45, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  If SMS type is 512 D, or RF, shift 20 more bits (toward MSB)
   */
  if ((sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D) || (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF))
  {
    SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80014002, 50, exit);
    res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

  /*
   *  If SMS type is RF, shift 41 more bits (toward MSB)
   */
  if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
  {
    SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80029002, 55, exit);
    res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_write_tbox_reg()", 0, 0);
}

STATIC
  uint32
    soc_petra_diag_simple_mbist_read_stp_status(
      SOC_SAND_IN int            unit,
      SOC_SAND_IN uint8            is_sms_type_rf,
      SOC_SAND_OUT uint32           *reslt
    )
{
  uint32
    fld_val,
    shift,
    mask,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  Run Status Sel
   */
  res = soc_petra_diag_simple_mbist_command(
          unit,
          14
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  SMS WDR Enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006003, 15, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR Read
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0, 25, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006002, 30, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  Configure Shift register
   */
  shift = is_sms_type_rf;
  mask = 6;
  SOC_PETRA_FLD_SET(regs->mcc.tap_res_1_reg.shift, shift, 35, exit);
  SOC_PETRA_FLD_SET(regs->mcc.tap_res_1_reg.mask, mask, 40, exit);

  SOC_PETRA_REG_GET(regs->mcc.tap_res_0_reg, fld_val, 45, exit);

  *reslt = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_read_stp_status()", 0, 0);
}

STATIC
  uint32
    soc_petra_diag_simple_mbist_change_and_read_stp_status(
      SOC_SAND_IN int            unit,
      SOC_SAND_IN uint32            sms_ndx,
      SOC_SAND_IN uint8            is_sms_change_required,
      SOC_SAND_OUT uint8          *success
    )
{
  uint32
    reslt = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;
  uint8
    is_sms_type_rf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Only if SMS change is required
   */
  if (is_sms_change_required == TRUE)
  {
   res = soc_petra_diag_simple_mbist_change_sms_number(
           unit,
           sms_ndx
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /*
   *  Get SMS type
   */
  res = soc_petra_diag_simple_mbist_sms_type_get(
              unit,
              sms_ndx,
              &sms_type
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
  {
    is_sms_type_rf = TRUE;
  }
  else
  {
    is_sms_type_rf = FALSE;
  }

  res = soc_petra_diag_simple_mbist_read_stp_status(
          unit,
          is_sms_type_rf,
          &reslt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

/*  soc_sand_os_printf(" STP Read sms_num: %d, result: %d\n", sms_ndx, reslt); */

  if (reslt == 0x20)
  {
    *success = TRUE;
  }
  else
  {
    *success = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_change_and_read_stp_status()", 0, 0);
}
STATIC
  uint32
    soc_petra_diag_simple_mbist_scan_out_chain(
      SOC_SAND_IN int            unit,
      SOC_SAND_IN uint32            chain_length,
      SOC_SAND_IN uint8            is_msb_dont_care,
      SOC_SAND_OUT uint32            out_chain[SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S]
    )
{
  uint32
    remaining_chain_length,
    fld_val,
    shift,
    mask,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    loop_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *  Init the variable
   */
  for (loop_ndx = 0; loop_ndx < SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S; ++loop_ndx)
  {
    out_chain[loop_ndx] = 0;
  }

  /*
   *  JPC WIR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00c, 20, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WIR start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00d, 25, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  JPC WIR enter - select config with sms capture disabled
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x23, 30, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 35, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 40, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR Enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006003, 45, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR Read
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0, 50, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0, 55, exit);

  /*
   *  Read Result - 32 bits at a time
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8001f002, 60, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  Configure shift reg - first read is dummy bit + 31 bits
   */
  shift = 0;
  mask = 32 - is_msb_dont_care;
  SOC_PETRA_FLD_SET(regs->mcc.tap_res_1_reg.shift, shift, 65, exit);
  SOC_PETRA_FLD_SET(regs->mcc.tap_res_1_reg.mask, mask, 70, exit);
  SOC_PETRA_REG_GET(regs->mcc.tap_res_0_reg, fld_val, 75, exit);
  out_chain[0] = fld_val;

  if (chain_length > 32)
  {
    remaining_chain_length = chain_length - 32;

    /*
     *  Configure shift reg - second read and onward is 32 bits
     */
    shift = 0;
    mask = 32;
    SOC_PETRA_FLD_SET(regs->mcc.tap_res_1_reg.shift, shift, 75, exit);
    SOC_PETRA_FLD_SET(regs->mcc.tap_res_1_reg.mask, mask, 80, exit);

    /*
     *  Loop should run 149 times. Total length = 150*32 = 4800 bits
     */
    loop_ndx = 1;
    while (1)
    {
      SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80020002, 90, exit);
      res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      SOC_PETRA_REG_GET(regs->mcc.tap_res_0_reg, fld_val, 95, exit);
      out_chain[loop_ndx] = fld_val;
      loop_ndx ++;
      if (remaining_chain_length < 32)
      {
        break;
      }
      remaining_chain_length = remaining_chain_length - 32;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_scan_out_chain()", 0, 0);
}


STATIC
  uint32
    soc_petra_diag_simple_mbist_rd_diag_chain(
      SOC_SAND_IN int            unit,
      SOC_SAND_IN uint32            sms_ndx,
      SOC_SAND_IN uint8            change_sms_required,
      SOC_SAND_IN uint8            read_status_required,
      SOC_SAND_OUT uint32            out_chain[SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S],
      SOC_SAND_OUT uint32            *nof_errors
    )
{
  uint32
    msb,
    addr_size,
    chain_length,
    res = SOC_SAND_OK;
  uint8
    success;
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;
  uint32
    bit_ndx,
    loop_ndx,
    bit_modulo;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (read_status_required)
  {
    res = soc_petra_diag_simple_mbist_change_and_read_stp_status(
            unit,
            sms_ndx,
            change_sms_required,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (change_sms_required)
  {
    res = soc_petra_diag_simple_mbist_change_sms_number(
            unit,
            sms_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  /*
   *  Run DIAG_SEL
   */
  res = soc_petra_diag_simple_mbist_command(
          unit,
          15
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_diag_simple_mbist_sms_sel_size_get(unit, sms_ndx, &chain_length);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_diag_simple_mbist_sms_addr_size_get(unit, sms_ndx, &addr_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /*
   *  Get SMS type
   */
  res = soc_petra_diag_simple_mbist_sms_type_get(
              unit,
              sms_ndx,
              &sms_type
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  if (sms_type == SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF)
  {
    res = soc_petra_diag_simple_mbist_scan_out_chain(
            unit,
            chain_length,
            FALSE, /* is_msb_dont_care = False */
            out_chain
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    msb = 11 + 5 + addr_size + 3 - 1;
  }
  else
  {
    res = soc_petra_diag_simple_mbist_scan_out_chain(
            unit,
            chain_length,
            TRUE, /* is_msb_dont_care = True */
            out_chain
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    msb = 1 + 12 + 5 + addr_size + 3 - 1;
  }

  *nof_errors = 0;
  for (bit_ndx = msb; bit_ndx < SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S * 32; ++bit_ndx)
  {
    loop_ndx = bit_ndx / 32;
    bit_modulo = bit_ndx % 32;
    if (out_chain[loop_ndx] & (1 << (32 - bit_modulo - 1)))
    {
      *nof_errors  = *nof_errors + 1;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_rd_diag_chain()", 0, 0);
}


STATIC
  uint32
    soc_petra_diag_simple_mbist_write_sone_reg(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx,
      SOC_SAND_IN uint32                      sone_val,
      SOC_SAND_IN uint8                     change_sms_required
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  if (change_sms_required)
  {
    /*
     *  Change SMS number
     */
    res = soc_petra_diag_simple_mbist_change_sms_number(
            unit,
            sms_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /*
   *  Run SONE_SEL
   */
  res = soc_petra_diag_simple_mbist_command(
          unit,
          8
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *  SMS WDR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006003, 20, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR write start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006004, 25, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /*
   *  SMS WDR write command
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, sone_val, 30, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0, 30, exit);

  /*
   *  Write result - 13 bits
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000d001, 35, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_simple_mbist_write_tbox_reg()", 0, 0);
}

/*
 *  Run MBIST on a specified SMS.
 *  To run on all memories, use SOC_PETRA_DIAG_SMS_ALL as the sms value.
 *  SMS index memory to write the pattern in. 
 *  sms is in range: 1 -  29 (SOC_PETRA_DIAG_SMS_ALL).
 */
uint32
  soc_petra_diag_mbist_run_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32     sms,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT   *sms_result[SOC_PETRA_DIAG_SMS_MAX]
  )
{
  uint32
    nof_errors = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_DIAG_SOFT_ERR_INFO
    err_info;
  uint32
    sms_count,
    sms_ndx;
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(sms_result);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(sms, SOC_PETRA_DIAGNOSTICS_SMS_MIN, SOC_PETRA_DIAG_SMS_ALL, SOC_PETRA_DIAGNOSTICS_SMS_OUT_OF_RANGE_ERR, 5, exit);

  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_clear(unit, &err_info);
  err_info.pattern = SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ZERO; /* Doesn't matter, any valid value is good */

  res = soc_petra_diag_soft_error_test_start_unsafe(
          unit,
          &err_info,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   /*
   * Init the values
   */
  for (sms_ndx = 0; sms_ndx < SOC_PETRA_DIAG_NOF_SMS; ++sms_ndx)
  {
    soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_clear(sms_result[sms_ndx]);
  }

  sms_count = 0;
  for (sms_ndx = SOC_PETRA_DIAGNOSTICS_SMS_MIN; sms_ndx <= SOC_PETRA_DIAGNOSTICS_SMS_MAX; ++sms_ndx)
  {
    if ((sms_ndx == sms) || (sms == SOC_PETRA_DIAG_SMS_ALL))
    {
      /*
       *  Get SMS type
       */
      res = soc_petra_diag_simple_mbist_sms_type_get(
              unit,
              sms_ndx,
              &sms_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

      res = soc_petra_diag_simple_mbist_change_and_read_stp_status(
              unit,
              sms_ndx,
              TRUE, /* is_sms_change_required = False */
              &success
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      if (success == FALSE)
      {
        res = soc_petra_diag_simple_mbist_rd_diag_chain(
                unit,
                sms_ndx,
                FALSE, /* change_sms_required, */
                FALSE, /* read_status_required, */
                sms_result[sms_count]->diag_chain,
                &nof_errors
              );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

        switch (sms_type)
        {
        case SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF:
          sms_result[sms_count]->nof_errs.err_rf = nof_errors;
          break;
        case SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D:
          sms_result[sms_count]->nof_errs.err_dp = nof_errors;
          break;
        case SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_S:
          sms_result[sms_count]->nof_errs.err_sp = nof_errors;
          break;
        default:
          break;
        }
#if SOC_PETRA_DEBUG_IS_LVL2
        soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_print(unit, sms_ndx, SOC_SAND_INTERN_VAL_INVALID_32, sms_result[sms_count]);
#endif
      }
      sms_count++;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_mbist_run_unsafe()", sms, 0);
}

/*********************************************************************
*     Start the soft-error test analyzing the memory validity.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_start_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  uint8                           is_mbist_only
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    sms_ndx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_SOFT_ERROR_TEST_START_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.busy_wait_nof_iters = SOC_PETRA_DIAG_SMS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_DIAG_SMS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_DIAG_SMS_TIMER_DELAY_MSEC;

  res = soc_petra_diag_simple_mbist_reset(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_diag_simple_mbist_deassert_tap_reset(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  
  /*
   *  Change SMS number
   */
  res = soc_petra_diag_simple_mbist_change_sms_number(
          unit,
          SOC_PETRA_DIAG_SMS_ALL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  /*
   * Run RBOX_RST
   */
  res = soc_petra_diag_simple_mbist_command(
          unit,
          3
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
  
  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 23, exit);
  }
  
  /*
   * Run BIHR_RUN
   */
  res = soc_petra_diag_simple_mbist_command(
          unit,
          24
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  
  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 27, exit);
  }

  for (sms_ndx = SOC_PETRA_DIAGNOSTICS_SMS_MIN; sms_ndx <= SOC_PETRA_DIAGNOSTICS_SMS_MAX; ++sms_ndx)
  {
    if ((sms_ndx == info->sms) || (info->sms == SOC_PETRA_DIAG_SMS_ALL))
    {
      if(is_mbist_only == FALSE)
      {
        res = soc_petra_diag_simple_mbist_write_tbox_reg(
                unit,
                sms_ndx,
                info->pattern,
                TRUE /* Write operation*/
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }

        /*
         *  Run BIST_RUN
         */
      res = soc_petra_diag_simple_mbist_command(
                unit,
                20
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      poll_info.expected_value = SOC_PETRA_DIAG_SMS_READY_POLL_VAL;
      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses1.ready_sms1),
              SOC_PETRA_DEFAULT_INSTANCE,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      if (poll_success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 52, exit);
      }

      poll_info.expected_value = 0x3ff;
      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses2.ready_sms2),
              SOC_PETRA_DEFAULT_INSTANCE,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      if (poll_success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 62, exit);
      }
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_soft_error_test_start_unsafe()", 0, 0);
}

uint32
  soc_petra_diag_soft_error_test_start_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_SOFT_ERROR_TEST_START_VERIFY);

  res = soc_petra_PETRA_DIAG_SOFT_ERR_INFO_verify(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_soft_error_test_start_verify()", 0, 0);
}

/*********************************************************************
*     Start the soft-error test analyzing the memory validity.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_result_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT          *sms_result[SOC_PETRA_DIAG_SMS_MAX][SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX],
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT          *glbl_result
  )
{
  uint32
    sone_value = 1,
    sone_value_with_error = 1,
    nof_errors = 0,
    res = SOC_SAND_OK;
  uint32
    diag_chain_ndx,
    sms_count,
    sone_ndx,
    sms_ndx;
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    success,
    poll_success;
  SOC_PETRA_REGS
    *regs;
  uint32
    diag_chain[SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(*sms_result);
  SOC_SAND_CHECK_NULL_INPUT(glbl_result);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PETRA_DIAG_SMS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_DIAG_SMS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_DIAG_SMS_TIMER_DELAY_MSEC;

  /*
   * Init the values
   */
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_clear(glbl_result);
  for (sms_ndx = 0; sms_ndx < SOC_PETRA_DIAG_NOF_SMS; ++sms_ndx)
  {
    for (sone_ndx = 0; sone_ndx < SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX; ++sone_ndx)
    {
      soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_clear(sms_result[sms_ndx][sone_ndx]);
    }
  }

  for (diag_chain_ndx = 0; diag_chain_ndx < SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S; ++diag_chain_ndx)
  {
    diag_chain[diag_chain_ndx] = 0;
  }

  sms_count = 0;
  for (sms_ndx = SOC_PETRA_DIAGNOSTICS_SMS_MIN; sms_ndx <= SOC_PETRA_DIAGNOSTICS_SMS_MAX; ++sms_ndx)
  {
    if ((sms_ndx == info->sms) || (info->sms == SOC_PETRA_DIAG_SMS_ALL))
    {
      /*
       *  Get SMS type
       */
      res = soc_petra_diag_simple_mbist_sms_type_get(
              unit,
              sms_ndx,
              &sms_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

      res = soc_petra_diag_simple_mbist_write_tbox_reg(
              unit,
              sms_ndx,
              info->pattern,
              FALSE /* Read operation*/
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

      /*
       *  Run BIST_RUN
       */
      res = soc_petra_diag_simple_mbist_command(
              unit,
              20
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      poll_info.expected_value = SOC_PETRA_DIAG_SMS_READY_POLL_VAL;
      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses1.ready_sms1),
              SOC_PETRA_DEFAULT_INSTANCE,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      if (poll_success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 52, exit);
      }

      poll_info.expected_value = 0x3ff;
      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses2.ready_sms2),
              SOC_PETRA_DEFAULT_INSTANCE,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      if (poll_success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 62, exit);
      }

      res = soc_petra_diag_simple_mbist_change_and_read_stp_status(
              unit,
              sms_ndx,
              FALSE, /* is_sms_change_required = False */
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      if (success == FALSE)
      {
        if (count_type == SOC_PETRA_DIAG_SOFT_COUNT_TYPE_FAST)
        {
          res = soc_petra_diag_simple_mbist_rd_diag_chain(
                  unit,
                  sms_ndx,
                  FALSE, /* change_sms_required, */
                  FALSE, /* read_status_required, */
                  sms_result[sms_count][0]->diag_chain,
                  &nof_errors
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

          switch (sms_type)
          {
          case SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF:
            sms_result[sms_count][0]->nof_errs.err_rf = nof_errors;
            glbl_result->err_rf += nof_errors;
            break;
          case SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D:
            sms_result[sms_count][0]->nof_errs.err_dp = nof_errors;
            glbl_result->err_dp += nof_errors;
            break;
          case SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_S:
            sms_result[sms_count][0]->nof_errs.err_sp = nof_errors;
            glbl_result->err_sp += nof_errors;
            break;
          default:
            break;
          }
#if SOC_PETRA_DEBUG_IS_LVL2
          soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_print(unit, sms_ndx, 0, sms_result[sms_count][0]);
#endif
        }
        else /* SOC_PETRA_DIAG_SOFT_COUNT_TYPE_COMPLETE */
        {
          sone_value = 1;
          sone_value_with_error = 1;

          res = soc_petra_diag_simple_mbist_write_sone_reg(
                  unit,
                  sms_ndx,
                  sone_value,
                  FALSE /* change_sms_required */
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          /*
           * BIST_RUN
           */
          res = soc_petra_diag_simple_mbist_command(
                  unit,
                  20
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          poll_info.expected_value = SOC_PETRA_DIAG_SMS_READY_POLL_VAL;
          res = soc_petra_status_fld_poll_unsafe(
                  unit,
                  SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses1.ready_sms1),
                  SOC_PETRA_DEFAULT_INSTANCE,
                  &poll_info,
                  &poll_success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          if (poll_success == FALSE)
          {
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 52, exit);
          }

          poll_info.expected_value = 0x3ff;
          res = soc_petra_status_fld_poll_unsafe(
                  unit,
                  SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses2.ready_sms2),
                  SOC_PETRA_DEFAULT_INSTANCE,
                  &poll_info,
                  &poll_success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
          if (poll_success == FALSE)
          {
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 62, exit);
          }

          res = soc_petra_diag_simple_mbist_change_and_read_stp_status(
                  unit,
                  sms_ndx,
                  FALSE, /* is_sms_change_required = False */
                  &success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          while ((success == FALSE) && (sone_value < 4*1024))
          {
            res = soc_petra_diag_simple_mbist_rd_diag_chain(
                    unit,
                    sms_ndx,
                    FALSE, /* change_sms_required, */
                    FALSE, /* read_status_required, */
                    diag_chain,
                    &nof_errors
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            if (nof_errors > 0)
            {
              sal_memcpy(sms_result[sms_count][sone_value_with_error]->diag_chain, diag_chain, sizeof(uint32) * SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S);

              switch (sms_type)
              {
              case SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF:
                sms_result[sms_count][sone_value_with_error]->nof_errs.err_rf = nof_errors;
                sms_result[sms_count][0]->nof_errs.err_rf += nof_errors;
                glbl_result->err_rf += nof_errors;
                break;
              case SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D:
                sms_result[sms_count][sone_value_with_error]->nof_errs.err_dp = nof_errors;
                sms_result[sms_count][0]->nof_errs.err_dp += nof_errors;
                glbl_result->err_dp += nof_errors;
                break;
              case SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_S:
                sms_result[sms_count][sone_value_with_error]->nof_errs.err_sp = nof_errors;
                sms_result[sms_count][0]->nof_errs.err_sp += nof_errors;
                glbl_result->err_sp += nof_errors;
                break;
              default:
                break;
              }
#if SOC_PETRA_DEBUG_IS_LVL2
              soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_print(unit, sms_ndx, sone_value, sms_result[sms_count][sone_value_with_error]);
#endif
              sone_value_with_error ++;

              if (sone_value_with_error >= SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX)
              {
                sone_value_with_error --;
              }
            }

            sone_value ++;
            res = soc_petra_diag_simple_mbist_write_sone_reg(
                    unit,
                    sms_ndx,
                    sone_value,
                    TRUE /* change_sms_required */
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

            /*
             * BIST_RUN
             */
            res = soc_petra_diag_simple_mbist_command(
                    unit,
                    20
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            poll_info.expected_value = SOC_PETRA_DIAG_SMS_READY_POLL_VAL;
            res = soc_petra_status_fld_poll_unsafe(
                    unit,
                    SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses1.ready_sms1),
                    SOC_PETRA_DEFAULT_INSTANCE,
                    &poll_info,
                    &poll_success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

            if (poll_success == FALSE)
            {
              SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 112, exit);
            }

            poll_info.expected_value = 0x3ff;
            res = soc_petra_status_fld_poll_unsafe(
                    unit,
                    SOC_PETRA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses2.ready_sms2),
                    SOC_PETRA_DEFAULT_INSTANCE,
                    &poll_info,
                    &poll_success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
            if (poll_success == FALSE)
            {
              SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 122, exit);
            }

            res = soc_petra_diag_simple_mbist_change_and_read_stp_status(
                    unit,
                    sms_ndx,
                    FALSE, /* is_sms_change_required = False */
                    &success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
          }

           res = soc_petra_diag_simple_mbist_write_sone_reg(
                  unit,
                  sms_ndx,
                  0,
                  FALSE /* change_sms_required */
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
        }
      }
      sms_count ++;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_soft_error_test_result_get_unsafe()", 0, 0);
}

uint32
  soc_petra_diag_soft_error_test_result_get_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET_VERIFY);

  res = soc_petra_PETRA_DIAG_SOFT_ERR_INFO_verify(unit, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(count_type, SOC_PETRA_DIAGNOSTICS_COUNT_TYPE_MAX, SOC_PETRA_DIAGNOSTICS_COUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_soft_error_test_result_get_verify()", 0, 0);
}

uint32
  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pattern, SOC_PETRA_DIAGNOSTICS_PATTERN_MAX, SOC_PETRA_DIAGNOSTICS_PATTERN_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->sms, SOC_PETRA_DIAGNOSTICS_SMS_MIN, SOC_PETRA_DIAG_SMS_ALL, SOC_PETRA_DIAGNOSTICS_SMS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_PETRA_DIAG_SOFT_ERR_INFO_verify()",0,0);
}

uint32
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_verify(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->err_sp, SOC_PETRA_DIAGNOSTICS_ERR_SP_MAX, SOC_PETRA_DIAGNOSTICS_ERR_SP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->err_dp, SOC_PETRA_DIAGNOSTICS_ERR_DP_MAX, SOC_PETRA_DIAGNOSTICS_ERR_DP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->err_rf, SOC_PETRA_DIAGNOSTICS_ERR_RF_MAX, SOC_PETRA_DIAGNOSTICS_ERR_RF_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_verify()",0,0);
}

uint32
  soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_verify(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_SMS_RESULT *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_STRUCT_VERIFY(PETRA_DIAG_SOFT_ERR_RESULT, &(info->nof_errs), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_verify()",0,0);
}

#if SOC_PETRA_DEBUG

uint32
  soc_petra_diag_tbls_dump_print(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint32            *address_array,
    SOC_SAND_IN  uint32            size_bytes,
    SOC_SAND_IN  uint32           word_size_bytes,
    SOC_SAND_IN  uint32            print_options_bm,
    SOC_SAND_IN  uint32            address_start
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    word_i,
    entry_i,
    bit_i,
    same_as_prev = FALSE;
  uint32
    nof_entries = size_bytes,
    address,
    zero_word[SOC_PETRA_DIAG_TBLS_DUMP_MAX_WORD_SIZE];
  const uint32
    *data_next,
    *data_curr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ret = SOC_SAND_OK; sal_memset(zero_word, 0x0, SOC_PETRA_DIAG_TBLS_DUMP_MAX_WORD_SIZE * sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(entry_i = 0; entry_i < nof_entries; ++entry_i)
  {
    address = entry_i + address_start;
    data_curr = address_array + (entry_i + 0) * (word_size_bytes >> 2);
    data_next = address_array + (entry_i + 1) * (word_size_bytes >> 2);


    if(print_options_bm & SOC_SAND_PRINT_RANGES)
    {
      if (
          (sal_memcmp(data_curr, data_next, word_size_bytes) != 0) ||
          (entry_i + 1 == nof_entries)
         )
      {
        same_as_prev = FALSE;
      }
      else
      {
        if(!same_as_prev)
        {
          same_as_prev = TRUE;
          if (
              (sal_memcmp(data_curr, zero_word, word_size_bytes) != 0) ||
              (!(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS))
             )
          {
            soc_sand_os_printf("  From 0x%04x To ", address);
          }
        }
        if(entry_i != (nof_entries - 1))
        {
          continue;
        }
      }
    }
    if (
        (sal_memcmp(data_curr, zero_word, word_size_bytes) != 0) ||
        (!(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS))
       )
    {
      soc_sand_os_printf("  0x%04x: ", address);
      for (word_i = 0; word_i < (word_size_bytes / sizeof(uint32)); ++word_i)
      {
        soc_sand_os_printf("0x%08x ", data_curr[word_i]);

        if(print_options_bm & SOC_SAND_PRINT_BITS)
        {
          soc_sand_os_printf(" Bits:");

          for(bit_i = 0; bit_i < SOC_SAND_NOF_BITS_IN_UINT32; ++bit_i)
          {
            if(data_curr[word_i] & SOC_SAND_BIT(bit_i))
            {
              soc_sand_os_printf("%d,", bit_i);
            }
          }
        }
      }
      soc_sand_os_printf("\n\r");
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_tbls_dump_print()",0,0);
}

STATIC
  uint32
    soc_pa_diag_tbls_dump_unsafe(
      SOC_SAND_IN  int  unit,
      SOC_SAND_IN  uint8    block_id,
      SOC_SAND_IN  uint32   tbl_offset,
      SOC_SAND_IN  uint8  print_zero
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind,
    print_options_bm = 0,
    tmp_size,
    max_size_bytes = 0,
    counter;
  int32
    curr_tbl_size;
  uint32
    granularity,
    block_num,
    temp_size;
  uint32
    *data = NULL;
  SOC_PETRA_TBL_PRINT_INFO
    *indirect_print = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_DIAG_TBLS_DUMP);

  SOC_PETRA_ALLOC(indirect_print, SOC_PETRA_TBL_PRINT_INFO, SOC_PETRA_DIAG_MAX_NOF_DUMP_TABLES);


  res = soc_petra_diag_tbls_dump_tables_get(
          indirect_print,
          block_id,
          tbl_offset,
          SOC_PETRA_DIAG_TBLS_DUMP_MODE_SELECTIVE,
          &counter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  print_options_bm |= SOC_SAND_PRINT_RANGES;

  if(!print_zero)
  {
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[dead_error_line] */
    print_options_bm |= SOC_SAND_DONT_PRINT_ZEROS;
  }

  for (ind = 0; ind < counter; ++ind)
  {
    tmp_size = indirect_print[ind].size * indirect_print[ind].wrd_sz;
    max_size_bytes = tmp_size > max_size_bytes ? tmp_size : max_size_bytes;
  }

  max_size_bytes = (max_size_bytes > SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE ? SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE : max_size_bytes);

  SOC_PETRA_ALLOC(data, uint32, max_size_bytes);

  for (ind = 0; ind < counter; ++ind)
  {
    if (indirect_print[ind].name)
    {
      soc_sand_os_printf("%2d. Table: %s Addr: 0x%08x \n\r", ind, indirect_print[ind].name,indirect_print[ind].base);
    }
    else
    {
      /*print block and address*/
      soc_sand_os_printf("%2d. Block: %s Addr: 0x%08x\n\r", ind, soc_petra_PETRA_DIAG_DUMP_TBLS_BLOCK_ID_to_string(indirect_print[ind].mod_id),indirect_print[ind].base);
    }

    curr_tbl_size = indirect_print[ind].size;
    granularity = (SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE / indirect_print[ind].wrd_sz);


    block_num = 0;
    while (curr_tbl_size > 0)
    {
      temp_size = (uint32)(curr_tbl_size > (int32)granularity ? granularity : curr_tbl_size);

      res = soc_sand_tbl_read_unsafe(
              unit,
              data,
              indirect_print[ind].base,
              temp_size * indirect_print[ind].wrd_sz,
              indirect_print[ind].mod_id,
              indirect_print[ind].wrd_sz
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_petra_diag_tbls_dump_print(
            unit,
            data,
            temp_size,
            indirect_print[ind].wrd_sz,
            print_options_bm,
            block_num * granularity
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      curr_tbl_size -= granularity;
      indirect_print[ind].base += granularity;
      ++block_num;
    }
  }

exit:
  SOC_PETRA_FREE(indirect_print);
  SOC_PETRA_FREE(data);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_diag_tbls_dump()",0,0);
}

uint32
  soc_petra_diag_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8    block_id,
    SOC_SAND_IN  uint32   tbl_offset,
    SOC_SAND_IN  uint8  print_zero
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_TBLS_DUMP);

  SOC_PETRA_DIFF_DEVICE_CALL(diag_tbls_dump_unsafe,(unit, block_id, tbl_offset, print_zero));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_tbls_dump_unsafe()",0,0);
}

uint32
  soc_pa_diag_dev_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
  )
{
  uint8
    block_id = SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS;
  uint32
    tbl_offset = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS;
  uint8
    print_zero = TRUE;
  uint32
    res = SOC_SAND_OK;
  uint32
    ind,
    print_options_bm = 0,
    tmp_size,
    max_size_bytes = 0,
    counter;
  int32
    curr_tbl_size;
  uint32
    granularity,
    block_num,
    temp_size;


  uint32
    *data = NULL;
  SOC_PETRA_TBL_PRINT_INFO
    *indirect_print = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DEV_TBLS_DUMP);

  SOC_PETRA_ALLOC(indirect_print, SOC_PETRA_TBL_PRINT_INFO, SOC_PETRA_DIAG_MAX_NOF_DUMP_TABLES);

  res = soc_petra_diag_tbls_dump_tables_get(
          indirect_print,
          block_id,
          tbl_offset,
          dump_mode,
          &counter
        );
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  print_options_bm |= SOC_SAND_PRINT_RANGES;

  if(!print_zero)
  {
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[dead_error_begin] */
    print_options_bm |= SOC_SAND_DONT_PRINT_ZEROS;
  }

  for (ind = 0; ind < counter; ++ind)
  {
    tmp_size = indirect_print[ind].size * indirect_print[ind].wrd_sz;
    max_size_bytes = tmp_size > max_size_bytes ? tmp_size : max_size_bytes;
  }

  max_size_bytes = (max_size_bytes > SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE ? SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE : max_size_bytes);

  SOC_PETRA_ALLOC(data, uint32, max_size_bytes);

  for (ind = 0; ind < counter; ++ind)
  {
    soc_sand_os_printf("%2d. Block: %s Addr: 0x%08x\n\r", ind, soc_petra_PETRA_DIAG_DUMP_TBLS_BLOCK_ID_to_string(indirect_print[ind].mod_id),indirect_print[ind].base);
    if (indirect_print[ind].name)
    {

    }
    else
    {
      /*print block and address*/
      soc_sand_os_printf("%2d. Block: %s Addr: 0x%08x\n\r", ind, soc_petra_PETRA_DIAG_DUMP_TBLS_BLOCK_ID_to_string(indirect_print[ind].mod_id),indirect_print[ind].base);
    }


    curr_tbl_size = indirect_print[ind].size;
    granularity = (SOC_PETRA_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE / indirect_print[ind].wrd_sz);


    block_num = 0;
    while (curr_tbl_size > 0)
    {
      temp_size = (uint32)(curr_tbl_size > (int32)granularity ? granularity : curr_tbl_size);

      res = soc_sand_tbl_read_unsafe(
        unit,
        data,
        indirect_print[ind].base,
        temp_size * indirect_print[ind].wrd_sz,
        indirect_print[ind].mod_id,
        indirect_print[ind].wrd_sz
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_petra_diag_tbls_dump_print(
        unit,
        data,
        temp_size,
        indirect_print[ind].wrd_sz,
        print_options_bm,
        block_num * granularity
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      curr_tbl_size -= granularity;
      indirect_print[ind].base += granularity;
      ++block_num;
    }
  }

exit:
  SOC_PETRA_FREE(indirect_print);
  SOC_PETRA_FREE(data);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_tbls_dump()",0,0);
}

uint32
  soc_petra_diag_dev_tables_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
  )
{
  uint32
    res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_TBLS_DUMP);
   SOC_PETRA_DIFF_DEVICE_CALL(diag_dev_tbls_dump_unsafe,(unit, dump_mode));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dev_tbls_dump_unsafe()",0,0);
}




uint32
  soc_petra_egq_resources_print_unsafe(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    addr_i,
    free_buff = 0,
    free_desc,
    port_indx,
    port_use,
    regs_val[3];
  SOC_PETRA_EGQ_FBM_TBL_DATA
    free_buff_tbl_data;
  SOC_PETRA_EGQ_FDM_TBL_DATA
    free_descriptor_tbl_data;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_TBLS
    *tables;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();
  tables = soc_petra_tbls();

  for(addr_i = 0; addr_i < SOC_PETRA_TBL(tables->egq.fbm_tbl.addr.size); ++addr_i)
  {
    res = soc_petra_egq_fbm_tbl_get_unsafe(
            unit,
            addr_i,
            &free_buff_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    free_buff += soc_sand_bitstream_get_nof_on_bits(&(free_buff_tbl_data.free_buffer_memory), 1);
  }

  free_desc = 0;
  for(addr_i = 0; addr_i < SOC_PETRA_TBL(tables->egq.fdm_tbl.addr.size); ++addr_i)
  {
    res = soc_petra_egq_fdm_tbl_get_unsafe(
            unit,
            addr_i,
            &free_descriptor_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    free_desc += soc_sand_bitstream_get_nof_on_bits(&(free_descriptor_tbl_data.free_descriptor_memory[0]), 1);
    free_desc += soc_sand_bitstream_get_nof_on_bits(&(free_descriptor_tbl_data.free_descriptor_memory[1]), 1);
  }

  soc_sand_os_printf(
    "  Current number of FREE buffers : %d.(%02d%%)\n\r",
    free_buff,
    ((free_buff) * 100) / 4064
  );

  soc_sand_os_printf(
    "  Current number of Descriptors: %d. (%02d%%)\n\r",
    free_desc, (free_desc * 100) / 8128
  );

  res = soc_petra_read_reg_buffer_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.fragmentation_queues_empty_indication[0].addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          3,
          regs_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_sand_os_printf("  None empty Ports Queues:\n\r");
  for (port_indx = 0; port_indx<SOC_PETRA_NOF_FAP_PORTS; ++port_indx)
  {
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            port_indx,
            1,
            &port_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (!port_use)
    {
      soc_sand_os_printf("%u \n\r",port_indx);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_egq_resources_print_unsafe()",0,0);
}

const char*
  soc_petra_PETRA_DIAG_DUMP_TBLS_BLOCK_ID_to_string(
    SOC_SAND_IN uint32 block_id
  )
{
  const char* str = NULL;
  switch(block_id)
  {
  case SOC_PETRA_OLP_ID:
    str = "OLP";
    break;
  case SOC_PETRA_IRE_ID:
    str = "IRE";
    break;
  case SOC_PETRA_IDR_ID:
    str = "IDR";
    break;
  case SOC_PETRA_IRR_ID:
    str = "IRR";
    break;
  case SOC_PETRA_IHP_ID:
    str = "IHP";
    break;
  case SOC_PETRA_QDR_ID:
    str = "QDR";
    break;
  case SOC_PETRA_IPS_ID:
    str = "IPS";
    break;
  case SOC_PETRA_IPT_ID:
    str = "IPT";
    break;
  case SOC_PETRA_DPI_A_ID:
    str = "DPIA";
    break;
  case SOC_PETRA_DPI_B_ID:
    str = "DPIB";
    break;
  case SOC_PETRA_DPI_C_ID:
    str = "DPIC";
    break;
  case SOC_PETRA_DPI_D_ID:
    str = "DPID";
    break;
  case SOC_PETRA_DPI_E_ID:
    str = "DPIE";
    break;
  case SOC_PETRA_DPI_F_ID:
    str = "DPIF";
    break;
  case SOC_PETRA_RTP_ID:
    str = "RTP";
    break;
  case SOC_PETRA_EGQ_ID:
    str = "EGQ";
    break;
  case SOC_PETRA_SCH_ID:
    str = "SCH";
    break;
  case SOC_PETRA_CFC_ID:
    str = "CFC";
    break;
  case SOC_PETRA_EPNI_ID:
    str = "EPN";
    break;
  case SOC_PETRA_IQM_ID:
    str = "IQM";
    break;
  case SOC_PETRA_MMU_ID:
    str = "MMU";
    break;

  default:
    str = "Unknown";
  }
  return str;
}

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
