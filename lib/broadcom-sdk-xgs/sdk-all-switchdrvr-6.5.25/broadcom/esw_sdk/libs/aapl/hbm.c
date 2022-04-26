/* AAPL CORE Revision: master
 *
 * Copyright (c) 2014-2021 Avago Technologies. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


/** Doxygen File Header
 ** @file
 ** @brief HBM functions.
 ** @defgroup HBM HBM Functions
 ** @{
 **/

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#if AAPL_ENABLE_HBM

#define START_WIR_WRITE 0x1
#define START_WDR_WRITE 0x2
#define START_WDR_READ  0x4
#define SHIFT_WRITE_WDR 0xa
#define SHIFT_READ_WDR  0xc
#define STOP_1500       0x0

#define HBM_SEL 1
#define PHY_SEL 0

#define PHY_MMT_CONFIG                 0x16
#define PHY_MMT_CONFIG_LENGTH          440
#define PHY_CONFIG                     0x14
#define PHY_CONFIG_LENGTH              104
#define PHY_CONFIG_HBM2E_LENGTH        216
#define PHY_CTC_ID                     0x16
#define PHY_CTC_ID_LENGTH              8
#define PHY_SOFT_LANE_REPAIR           0x12
#define PHY_SOFT_LANE_REPAIR_LENGTH    72
#define HBM_TEMP                       0xf
#define HBM_TEMP_LENGTH                8
#define HBM_MODE_REG_DUMP_SET          0x10
#define HBM_MODE_REG_DUMP_SET_LENGTH   128

#define HBM_CTC_IDCODE                 0x0f
#define HBM_STOP_IDCODE                0x10

#define SAMSUNG 1
#define SKH     6

int avago_hbm_default_timeout = 15000;
int avago_hbm_dwell_time = 2000;
int hbm2e_check = -1;

/**
 ** @brief  Returns true is the HBM PHY is an HBM2e PHY
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
BOOL avago_hbm2e_check(Aapl_t *aapl, uint spico_addr)
{
  uint apc_addr;

  if (hbm2e_check == -1) {
    int rc;
    rc = avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

    if (rc != 0) {
      hbm2e_check = FALSE;
    } else {
      uint revision;
      uint hbm2e_rev;
      revision = avago_sbus_rd(aapl, apc_addr, 252);
      hbm2e_rev = (revision >> 3) & 0x1;
      if ((aapl_get_process_id(aapl, apc_addr) == AVAGO_TSMC_05) || hbm2e_rev == 1) {
        hbm2e_check = TRUE;
      } else {
        hbm2e_check = FALSE;
      }
    }
  }

  return hbm2e_check;
}

/**
 ** @brief  Verify firmware has been loaded
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_fw_check(Aapl_t *aapl, uint spico_addr)
{
  avago_firmware_get_rev(aapl, spico_addr);
  if( !aapl_get_spico_running_flag(aapl, spico_addr) ) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation cannot run because the firmware has not been loaded or SPICO at address %s is not running.\n", aapl_addr_to_str(spico_addr));
    return -1;
  }
  return 0;
}

/**
 ** @brief  Verifies that the sbus ring includes an HBM PHY
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_ring_check(Aapl_t *aapl, uint spico_addr)
{
  uint addr;
  uint max_sbus_addr = aapl_get_max_sbus_addr(aapl, spico_addr);

  Avago_addr_t addr_struct;
  avago_addr_to_struct(spico_addr, &addr_struct);

  for (addr = 1; addr <= max_sbus_addr; addr++) {
    Avago_ip_type_t ip_type;
    addr_struct.sbus = addr;
    ip_type = aapl_get_ip_type(aapl, avago_struct_to_addr(&addr_struct));
    if (ip_type == AVAGO_APC) {
      return 0;
    }
  }

  return -1;
}


/**
 ** @brief  Waits for the 1500 BUSY DONE register to equal the expected value
 **
 ** @param  aapl       Aapl_t struct
 ** @param  apc_addr   sbus_address of the APC receiver
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_wait_for_1500_done(Aapl_t *aapl, uint apc_addr, uint expected_value)
{
  uint timeout = 15000;
  uint done_value = (expected_value ^ 1) & 0x1;

  while ((done_value != expected_value) && timeout > 0) {
    done_value = avago_sbus_rd(aapl, apc_addr, 0x12);
    done_value = done_value & 0x1;
    timeout = timeout - 1;
  }

  if (timeout == 0) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation timed out while waiting for 1500 DONE at APC addr 0x%02x.\n", apc_addr);
  }

  return 0;
}

/**
 ** @brief  Wait for HBM2e WIR/WDR operation to complete
 **
 ** @param  aapl       Aapl_t struct
 ** @param  apc_addr   sbus_address of the APC receiver
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm2e_wait_for_1500_done(Aapl_t *aapl, uint apc_addr, uint command)
{
  uint timeout;
  uint busy;
  uint done;
  uint status;

  timeout = 1000;
  busy = 0;

  while ((busy == 0) && timeout > 0) {
    status = avago_sbus_rd(aapl, apc_addr, 0x08);
    busy = (status >> 1) & command;
    timeout = timeout - 1;
  }

  if (timeout == 0) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation timed out while waiting for 1500 BUSY at APC addr 0x%02x.\n", apc_addr);
    return -1;
  }

  done = 0;
  while ((done != 1) && timeout > 0) {
    status = avago_sbus_rd(aapl, apc_addr, 0x08);
    done = status & 1;
    timeout = timeout - 1;
  }

  if (timeout == 0) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation timed out while waiting for 1500 DONE at APC addr 0x%02x.\n", apc_addr);
    return -1;
  }

  return 0;
}


/**
 ** @brief  Performs a 1500 operation handshake to ensure the command was accepted
 **
 ** @param  aapl       Aapl_t struct
 ** @param  apc_addr   sbus_address of the APC receiver
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_apc_1500_busy_done_handshake(Aapl_t *aapl, uint apc_addr)
{
  avago_hbm_wait_for_1500_done(aapl, apc_addr, 1);
  avago_sbus_wr(aapl, apc_addr, 0x10, STOP_1500);
  avago_sbus_wr(aapl, apc_addr, 0x0f, 0);
  avago_hbm_wait_for_1500_done(aapl, apc_addr, 0);
  return 0;
}


/**
 ** @brief  Sets the IEEE 1500 WIR
 **
 ** @param  aapl        Aapl_t struct
 ** @param  apc_addr    sbus_address of the APC receiver
 ** @param  channel     HBM channel to write to
 ** @param  instruction WIR instruction
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_wir_write_channel(Aapl_t *aapl, uint apc_addr, uint hbm_phy_sel, uint channel, uint instruction)
{
  if (avago_hbm2e_check(aapl, apc_addr)) {
    avago_sbus_wr(aapl, apc_addr, 0x0a, ((hbm_phy_sel << 12) | (channel << 8) | instruction));
    avago_sbus_wr(aapl, apc_addr, 0x06, START_WIR_WRITE);
    avago_hbm2e_wait_for_1500_done(aapl, apc_addr, START_WIR_WRITE);
    avago_sbus_wr(aapl, apc_addr, 0x06, STOP_1500);
  } else {
    avago_sbus_wr(aapl, apc_addr, 0x02, ((hbm_phy_sel << 12) | (channel << 8) | instruction));
    avago_sbus_wr(aapl, apc_addr, 0x10, START_WIR_WRITE);
    avago_hbm_apc_1500_busy_done_handshake(aapl, apc_addr);
  }

  return 0;
}


/**
 ** @brief  Performs a WDR read operation
 **
 ** @param  aapl         Aapl_t struct
 ** @param  apc_addr     sbus_address of the APC receiver
 ** @param  length       WIR length
 ** @param  shift_only   Shift the chain but do not capture
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_wdr_read(Aapl_t *aapl, uint apc_addr, int length, int shift_only)
{
  if (aapl_get_process_id(aapl, apc_addr) == AVAGO_PROCESS_B) {
    avago_sbus_wr(aapl, apc_addr, 0x10, STOP_1500);
    avago_sbus_wr(aapl, apc_addr, 0x10, ((length<<4) | START_WDR_READ));
    avago_hbm_apc_1500_busy_done_handshake(aapl, apc_addr);

  } else if (!(avago_hbm2e_check(aapl, apc_addr))) {
    avago_sbus_wr(aapl, apc_addr, 0x0f, (length | (shift_only<<9)));
    avago_sbus_wr(aapl, apc_addr, 0x10, STOP_1500);
    avago_sbus_wr(aapl, apc_addr, 0x10, START_WDR_READ);
    avago_hbm_apc_1500_busy_done_handshake(aapl, apc_addr);

  } else {
    avago_sbus_wr(aapl, apc_addr, 0x06, 0);
    avago_sbus_wr(aapl, apc_addr, 0x0c, (length | (shift_only<<9)));
    avago_sbus_wr(aapl, apc_addr, 0x06, START_WDR_READ);
    avago_hbm2e_wait_for_1500_done(aapl, apc_addr, START_WDR_READ);
    avago_sbus_wr(aapl, apc_addr, 0x06, STOP_1500);
  }

  return 0;
}

/**
 ** @brief  Performs a WDR write operation.  Data must already be set in the WDR WRITE registeers
 **
 ** @param  aapl         Aapl_t struct
 ** @param  apc_addr     sbus_address of the APC receiver
 ** @param  length       WIR length
 ** @param  shift_only   Shift the chian but to no update
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_wdr_write(Aapl_t *aapl, uint apc_addr, int length, int shift_only)
{
  if (aapl_get_process_id(aapl, apc_addr) == AVAGO_PROCESS_B) {
    avago_sbus_wr(aapl, apc_addr, 0x10, STOP_1500);
    avago_sbus_wr(aapl, apc_addr, 0x10, ((length<<4) | START_WDR_WRITE));
    avago_hbm_apc_1500_busy_done_handshake(aapl, apc_addr);

  } else if (!(avago_hbm2e_check(aapl, apc_addr))) {
    avago_sbus_wr(aapl, apc_addr, 0x0f, (length | (shift_only<<9)));
    avago_sbus_wr(aapl, apc_addr, 0x10, STOP_1500);
    avago_sbus_wr(aapl, apc_addr, 0x10, START_WDR_WRITE);
    avago_hbm_apc_1500_busy_done_handshake(aapl, apc_addr);

  } else {
    avago_sbus_wr(aapl, apc_addr, 0x06, 0);
    avago_sbus_wr(aapl, apc_addr, 0x0c, (length | (shift_only<<9)));
    avago_sbus_wr(aapl, apc_addr, 0x06, START_WDR_WRITE);
    avago_hbm2e_wait_for_1500_done(aapl, apc_addr, START_WDR_WRITE);
    avago_sbus_wr(aapl, apc_addr, 0x06, STOP_1500);
  }

  return 0;
}

/**
 ** @brief  Sets the channel to unload read data from
 **
 ** @param  aapl         Aapl_t struct
 ** @param  apc_addr     sbus_address of the APC receiver
 ** @param  channel      Read channel
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static uint avago_hbm_wdr_set_read_channel(Aapl_t *aapl, uint apc_addr, uint channel)
{
  if (avago_hbm2e_check(aapl, apc_addr)) {
    return avago_sbus_wr(aapl, apc_addr, 0x10, channel);
  } else {
    return avago_sbus_wr(aapl, apc_addr, 0x11, (1 << channel));
  }
}


/**
 ** @brief  Sets the channel to unload read data from
 **
 ** @param  aapl         Aapl_t struct
 ** @param  apc_addr     sbus_address of the APC receiver
 ** @param  channel      Read channel
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static uint avago_hbm_wdr_read_sbus_data(Aapl_t *aapl, uint apc_addr, uint data_word)
{
  uint result;
  if (avago_hbm2e_check(aapl, apc_addr)) {
    result = avago_sbus_rd(aapl, apc_addr, (0x1e + data_word));
  } else {
    result = avago_sbus_rd(aapl, apc_addr, (0x14 + data_word));
  }
  return result;
}


/**
 ** @brief  Returns TRUE if the HBM firmware supports dmem reads
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static BOOL avago_hbm_firmware_dmem_read_supported(Aapl_t *aapl, uint spico_addr)
{
  uint fw_revision;
  uint fw_build_id;

  fw_revision =  avago_firmware_get_rev(aapl, spico_addr);
  fw_build_id = avago_firmware_get_build_id(aapl, spico_addr);

  if (fw_build_id == 0x2002 && fw_revision < 0x550) {
    return FALSE;
  } else {
    return TRUE;
  }
}


/**
 ** @brief  Returns the SPICO sbus address if either an APC or SPICO address is provided
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico or apc
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static uint avago_hbm_get_spico_address(Aapl_t *aapl, uint spico_addr)
{
  if (aapl_check_ip_type(aapl, spico_addr, __func__, __LINE__, FALSE, 1, AVAGO_APC)) {
    spico_addr = avago_make_sbus_master_addr(spico_addr);
  }

  if (!aapl_check_ip_type(aapl, spico_addr, __func__, __LINE__, FALSE, 1, AVAGO_SPICO)) {
    Avago_ip_type_t ip_type = aapl_get_ip_type(aapl, spico_addr);
    aapl_fail(aapl, __func__, __LINE__, "The address 0x%0x is not a SPICO or APC address.  IP Type = %d\n", spico_addr, ip_type);
  }

  return spico_addr;
}


/**
 ** @brief  Reads the stored LFSR COMPARE STICKY results saved by firmware.  A
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_read_lfsr_compare_sticky(Aapl_t *aapl, uint spico_addr, int hbm_not_phy, int result_group, const char * prefix)
{
  uint base_addr;
  uint channel;
  uint fw_build_id;
  uint fw_revision;


  if (avago_hbm2e_check(aapl, spico_addr)) {
    return 0;
  }

  fw_revision =  avago_firmware_get_rev(aapl, spico_addr);
  fw_build_id = avago_firmware_get_build_id(aapl, spico_addr);

  if (avago_hbm_firmware_dmem_read_supported(aapl, spico_addr) == FALSE) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   LFSR_COMPARE_STICKY diagnostics not supported with this version of firmware\n");
    return 0;
  }

  if ((fw_build_id == 0x2002 && fw_revision <= 0x567) || (fw_build_id == 0x1002 && fw_revision <= 0x11)) {
    base_addr = 0x0518;
  } else {
    base_addr = 0x0568;
  }

  for( channel = 0;  channel < 8; channel++ ) {
    uint xdmem_start;
    uint lfsr_31_0;
    uint lfsr_63_42;
    uint lfsr_95_64;
    uint lfsr_127_96;
    uint lfsr_159_128;
    uint lfsr_191_160;

    xdmem_start = base_addr + 12*channel + 96*result_group;

    lfsr_31_0 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_start);
    lfsr_31_0 = lfsr_31_0 | (avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+1) << 16);
    lfsr_63_42 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+2);
    lfsr_63_42 = lfsr_63_42 | (avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+3) << 16);
    lfsr_95_64 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+4);
    lfsr_95_64 = lfsr_95_64 | (avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+5) << 16);
    lfsr_127_96 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+6);
    lfsr_127_96 = lfsr_127_96 | (avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+7) << 16);
    lfsr_159_128 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+8);
    lfsr_159_128 = lfsr_159_128 | (avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+9) << 16);
    lfsr_191_160 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+10);
    lfsr_191_160 = lfsr_191_160 | (avago_spico_int(aapl, spico_addr, 0x37, xdmem_start+11) << 16);

    if (hbm_not_phy == 1) {
      lfsr_191_160 = lfsr_191_160 & 0x3fff;
    } else {
      lfsr_191_160 = lfsr_191_160 & 0x7fffff;
    }

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   %s Channel:%d LFSR_COMPARE: 0x%06x%08x%08x%08x%08x%08x\n",
        prefix,
        channel,
        lfsr_191_160, lfsr_159_128, lfsr_127_96, lfsr_95_64, lfsr_63_42, lfsr_31_0);
  }
  return 0;
}


/**
 ** @brief  Returns a string description of the integer error code
 ** @param  error_code    error code integer
 **
 ** @return  Error code description
 **/
static const char * avago_hbm_error_code_desc(uint error_code)
{
  const char * desc;

  if (hbm2e_check) {
    switch(error_code) {
      case 0x00:  { desc = "NO_ERROR";                                   break; }
      case 0x08:  { desc = "ERROR_UNKNOWN_ERROR";                        break; }
      case 0x09:  { desc = "ERROR_POWER_ON_RESET_L_ASSERTED";            break; }
      case 0x0a:  { desc = "ERROR_POWER_UP_DRV_DISABLE_L_ASSERTED";      break; }
      case 0x0b:  { desc = "ERROR_ALL_CHANNELS_NOT_SELECTED_FOR_RESET";  break; }
      case 0x0c:  { desc = "ERROR_COMMAND_ERROR_DURING_CKE_CHANGE";      break; }
      case 0x0d:  { desc = "ERROR_TIMEOUT_WATING_FOR_CKE_TO_ASSERT";     break; }
      case 0x0e:  { desc = "ERROR_TIMEOUT_WATING_FOR_CKE_TO_DEASSERT";   break; }
      case 0x0f:  { desc = "ERROR_TIMEOUT_WAITING_FOR_1500_BUSY";        break; }
      case 0x10:  { desc = "ERROR_TIMEOUT_WAITING_FOR_1500_DONE";        break; }
      case 0x11:  { desc = "ERROR_1500_COMMAND_ERROR";                   break; }
      case 0x12:  { desc = "ERROR_CHANNEL_FAILURES_EXIST";               break; }
      case 0x13:  { desc = "ERROR_ILLEGAL_CHANNEL_NUMBER_REQUESTED";     break; }
      case 0x14:  { desc = "ERROR_TIMEOUT_WAITING_FOR_VALID_TEMP";       break; }
      case 0x15:  { desc = "ERROR_COMMAND_ERROR_WAITING_FOR_BIST_DONE";  break; }
      case 0x16:  { desc = "ERROR_TIMEOUT_WATING_FOR_BIST_DONE";         break; }
      case 0x17:  { desc = "ERROR_COMMAND_ERROR_WAITING_FOR_BIST_BUSY";  break; }
      case 0x18:  { desc = "ERROR_TIMEOUT_WATING_FOR_BIST_BUSY";         break; }
      case 0x19:  { desc = "ERROR_DATA_COMPARE_FAILED";                  break; }
      case 0x1a:  { desc = "ERROR_EXPECTED_ERROR_NOT_DETECTED";          break; }
      case 0x1b:  { desc = "ERROR_ILLEGAL_STATE_FOR_FIFO_POINTER_RESET"; break; }
      case 0x1c:  { desc = "ERROR_LANE_ERRORS_DETECTED";                 break; }
      case 0x1d:  { desc = "ERROR_NON_REPAIRABLE_FAULTS_FOUND";          break; }
      case 0x1e:  { desc = "ERROR_REPAIR_LIMIT_EXCEEDED";                break; }
      case 0x1f:  { desc = "ERROR_RC_FAULT_DETECTED";                    break; }
      case 0x20:  { desc = "ERROR_RR_FAULT_DETECTED";                    break; }
      case 0x21:  { desc = "ERROR_NEW_REPAIR_CONFLICTS_WITH_OLD_REPAIR"; break; }
      case 0x22:  { desc = "ERROR_ALL_CHANNELS_NOT_ENABLED";             break; }
      case 0x23:  { desc = "ERROR_UNSUPPORTED_HBM_CONFIGURATION";        break; }
      case 0x24:  { desc = "ERROR_MBIST_CELL_FAILURES_DETECTED";         break; }
      case 0x25:  { desc = "ERROR_NO_FUSES_AVAILABLE_FOR_REPAIR";        break; }
      case 0x26:  { desc = "ERROR_MBIST_ENGINE_FAILURE";                 break; }
      case 0x27:  { desc = "ERROR_MBIST_OP_COUNT_EXCEEDED";              break; }
      case 0x28:  { desc = "ERROR_INVALID_MBIST_PROGRAM";                break; }
      case 0x29:  { desc = "ERROR_INVALID_MMT_CONFIGURATION";            break; }
      case 0x2a:  { desc = "ERROR_FAILED_TO_ENTER_AWORD_BIST_PREP";      break; }
      case 0x2b:  { desc = "ERROR_PHY_MISR_PRESET_FAILED";               break; }
      case 0x2c:  { desc = "ERROR_HBM_MISR_PRESET_FAILED";               break; }
      case 0x2d:  { desc = "ERROR_CURRENT_TEST_NOT_SET";                 break; }
      case 0x2e:  { desc = "ERROR_TUNING_FAILED";                        break; }
      case 0x3e7: { desc = "ERROR_FUNCTION_NOT_IMPLEMENTED";             break; }
      default:    { desc =  "UNKNOWN_ERROR_CODE";                        break; }
    }

  } else {

    switch(error_code) {
      case 0x00: { desc =  "NO_ERROR";                                   break; }
      case 0x01: { desc =  "ERROR_DETECTED";                             break; }
      case 0x02: { desc =  "ERROR_UNEXPECTED_RESET_STATE";               break; }
      case 0x03: { desc =  "ERROR_ILLEGAL_CHANNEL_NUMBER";               break; }
      case 0x04: { desc =  "ERROR_TIMEOUT_WAITING_FOR_1500_DONE";        break; }
      case 0x05: { desc =  "ERROR_TIMEOUT_WAITING_FOR_BIST_DONE";        break; }
      case 0x06: { desc =  "ERROR_DATA_COMPARE_FAILED";                  break; }
      case 0x07: { desc =  "ERROR_ALL_CHANNELS_NOT_SELECTED_FOR_RESET";  break; }
      case 0x08: { desc =  "ERROR_REPAIR_LIMIT_EXCEEDED";                break; }
      case 0x09: { desc =  "ERROR_NON_REPAIRABLE_FAULTS_FOUND";          break; }
      case 0x0a: { desc =  "ERROR_MBIST_FAILED";                         break; }
      case 0x0b: { desc =  "ERROR_EXCEEDED_BANK_REPAIR_LIMIT";           break; }
      case 0x0c: { desc =  "ERROR_ALL_CHANNELS_NOT_ENABLED";             break; }
      case 0x0d: { desc =  "ERROR_TIMEOUT_WAITING_FOR_PHYUPD_HANDSHAKE"; break; }
      case 0x0e: { desc =  "ERROR_CHANNEL_UNREPAIRABLE";                 break; }
      case 0x0f: { desc =  "ERROR_NO_FUSES_AVAILBALE_FOR_REPAIR";        break; }
      case 0x10: { desc =  "ERROR_TIMEOUT_WAITING_FOR_VALID_TEMP";       break; }
      case 0x11: { desc =  "ERROR_CHANNEL_FAILURES_EXIST";               break; }
      case 0x12: { desc =  "ERROR_UNKNOWN_ERROR";                        break; }
      case 0x13: { desc =  "ERROR_TIMEOUT_WAITING_FOR_NWL_INIT";         break; }
      case 0x14: { desc =  "ERROR_CTC_WRITE_READ_COMPARE_FAILURE";       break; }
      case 0x15: { desc =  "ERROR_CTC_NO_WRITES_PERFORMED";              break; }
      case 0x16: { desc =  "ERROR_CTC_NO_READS_PERFORMED";               break; }
      case 0x17: { desc =  "ERROR_LANE_ERRORS_DETECTED";                 break; }
      case 0x18: { desc =  "ERROR_CTC_TIMEOUT_WAITING_FOR_CTC_BUSY";     break; }
      case 0x19: { desc =  "ERROR_UNSUPPORTED_HBM_CONFIGURATION";        break; }
      case 0x1a: { desc =  "ERROR_HBM_MISR_PRESET_FAILED";               break; }
      case 0x1b: { desc =  "ERROR_PHY_MISR_PRESET_FAILED";               break; }
      case 0x1c: { desc =  "ERROR_CORE_POWERON_RST_L_ASSERTED";          break; }
      case 0x1d: { desc =  "ERROR_UNSUPPORTED_INTERRUPT";                break; }
      case 0x1e: { desc =  "ERROR_EXTEST_SLB_FAILURE";                   break; }
      default:   { desc =  "UNKNOWN_ERROR_CODE";                         break; }
    }
  }
  return desc;
}


/**
 ** @brief  Returns a string description of the integer operation code
 ** @param  operation_code    operation code integer
 **
 ** @return  Error code description
 **/
static const char * avago_hbm_operation_desc(uint operation_code)
{
  const char * desc;

  if (hbm2e_check) {
    switch(operation_code) {
      case 0x00: { desc = "OP_SUCCESS";                    break; }
      case 0xc8: { desc = "OP_DERR";                       break; }
      case 0xc9: { desc = "OP_DERR_ILB";                   break; }
      case 0xca: { desc = "OP_DERR_ILB_INJECT_ERR";        break; }
      case 0xcb: { desc = "OP_DERR_INJECT_ERR";            break; }
      case 0xcc: { desc = "OP_DERR_SLB";                   break; }
      case 0xcd: { desc = "OP_DERR_SLB_INJECT_ERR";        break; }
      case 0xce: { desc = "OP_DWORD_LOWER_ILB";            break; }
      case 0xcf: { desc = "OP_DWORD_LOWER_ILB_INJECT_ERR"; break; }
      case 0xd0: { desc = "OP_DWORD_LOWER_SLB";            break; }
      case 0xd1: { desc = "OP_DWORD_LOWER_SLB_INJECT_ERR"; break; }
      case 0xd2: { desc = "OP_DWORD_READ";                 break; }
      case 0xd3: { desc = "OP_DWORD_UPPER_ILB";            break; }
      case 0xd4: { desc = "OP_DWORD_UPPER_ILB_INJECT_ERR"; break; }
      case 0xd5: { desc = "OP_DWORD_UPPER_SLB";            break; }
      case 0xd6: { desc = "OP_DWORD_UPPER_SLB_INJECT_ERR"; break; }
      case 0xd7: { desc = "OP_DWORD_WRITE";                break; }
      case 0xd8: { desc = "OP_DWORD_WRITE_INJECT_ERR";     break; }
      case 0xd9: { desc = "OP_READ_DEVICE_ID";             break; }
      case 0xda: { desc = "OP_READ_MRS";                   break; }
      case 0xdb: { desc = "OP_READ_PHY_CONFIG";            break; }
      case 0xdc: { desc = "OP_READ_TEMP";                  break; }
      case 0xdd: { desc = "OP_RESET";                      break; }
      case 0xde: { desc = "OP_AERR";                       break; }
      case 0xdf: { desc = "OP_AERR_INJECT_ERR";            break; }
      case 0xe0: { desc = "OP_AERR_ILB";                   break; }
      case 0xe1: { desc = "OP_AERR_ILB_INJECT_ERR";        break; }
      case 0xe2: { desc = "OP_AERR_SLB";                   break; }
      case 0xe3: { desc = "OP_AERR_SLB_INJECT_ERR";        break; }
      case 0xe4: { desc = "OP_AWORD";                      break; }
      case 0xe5: { desc = "OP_AWORD_INJECT_ERR";           break; }
      case 0xe6: { desc = "OP_AWORD_ILB";                  break; }
      case 0xe7: { desc = "OP_AWORD_ILB_INJECT_ERR";       break; }
      case 0xe8: { desc = "OP_AWORD_SLB";                  break; }
      case 0xe9: { desc = "OP_AWORD_SLB_INJECT_ERR";       break; }
      case 0xea: { desc = "OP_PHY_BYPASS";                 break; }
      case 0xeb: { desc = "OP_HBM_BYPASS";                 break; }
      case 0xec: { desc = "OP_CATTRIP";                    break; }
      case 0xed: { desc = "OP_CHIPPING";                   break; }
      case 0xee: { desc = "OP_TMRS";                       break; }
      case 0xef: { desc = "OP_COPY_HARD_REPAIRS";          break; }
      case 0xf0: { desc = "OP_LANE_REPAIR";                break; }
      case 0xf1: { desc = "OP_MBIST";                      break; }
      case 0xf2: { desc = "OP_MMT";                        break; }
      default  : { desc = "UNKNOWN_OPERATION";             break; }
    }

  } else {
    switch(operation_code) {
      case 0x00: { desc = "OP_SUCCESS";             break; }
      case 0x01: { desc = "OP_BYPASS";              break; }
      case 0x02: { desc = "OP_DEVICE_ID";           break; }
      case 0x03: { desc = "OP_AWORD";               break; }
      case 0x04: { desc = "OP_AERR";                break; }
      case 0x05: { desc = "OP_AWORD_ILB";           break; }
      case 0x06: { desc = "OP_AERR_ILB";            break; }
      case 0x07: { desc = "OP_AERR_INJ_ILB";        break; }
      case 0x08: { desc = "OP_DWORD_WRITE";         break; }
      case 0x09: { desc = "OP_DWORD_READ";          break; }
      case 0x0a: { desc = "OP_DERR";                break; }
      case 0x0b: { desc = "OP_DWORD_UPPER_ILB";     break; }
      case 0x0c: { desc = "OP_DWORD_LOWER_ILB";     break; }
      case 0x0d: { desc = "OP_DERR_ILB";            break; }
      case 0x0e: { desc = "OP_DERR_IBJ_ILB";        break; }
      case 0x0f: { desc = "OP_LANE_REPAIR";         break; }
      case 0x10: { desc = "OP_DEVICE_TEMP";         break; }
      case 0x11: { desc = "OP_CONNECTIVITY_CHECK";  break; }
      case 0x12: { desc = "OP_RESET";               break; }
      case 0x13: { desc = "OP_MBIST";               break; }
      case 0x14: { desc = "OP_BITCELL_REPAIR";      break; }
      case 0x15: { desc = "OP_AWORD_SLB";           break; }
      case 0x16: { desc = "OP_AERR_SLB";            break; }
      case 0x17: { desc = "OP_AERR_INJ_SLB";        break; }
      case 0x18: { desc = "OP_DWORD_UPPER_SLB";     break; }
      case 0x19: { desc = "OP_DWORD_LOWER_SLB";     break; }
      case 0x1a: { desc = "OP_DERR_SLB";            break; }
      case 0x1b: { desc = "OP_DERR_INJ_SLB";        break; }
      case 0x1c: { desc = "OP_TMRS";                break; }
      case 0x1d: { desc = "OP_CHIPPING";            break; }
      case 0x1e: { desc = "OP_MC_INIT";             break; }
      case 0x1f: { desc = "OP_CTC_OR_MMT";          break; }
      case 0x20: { desc = "OP_APPLY_LANE_REPAIR";   break; }
      case 0x21: { desc = "OP_BURN_LANE_REPAIR";    break; }
      case 0x22: { desc = "OP_CATTRIP";             break; }
      case 0x23: { desc = "OP_FUSE_SCAN";           break; }
      case 0x24: { desc = "OP_EXTEST_SLB";          break; }
      default :  { desc = "UNKNOWN_OPERATION";      break; }
    }
  }

  return desc;
}

/**
 ** @brief  Return a description for a given mmt pattern number
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the sbus master spico or APC
 ** @param  pattern      MMT pattern number
 **
 ** @return  MMT pattern name
 **/
static const char * avago_hbm_mmt_pattern_to_str(uint pattern)
{
  const char * pattern_name;
  if (hbm2e_check) {
    switch(pattern) {
      case   0: { pattern_name = "User";                    break; }
      case   1: { pattern_name = "Short";                   break; }
      case   2: { pattern_name = "PRBS write/read mix";     break; }
      case   3: { pattern_name = "PRBS write then read";    break; }
      case   4: { pattern_name = "PRBS read only";          break; }
      case   5: { pattern_name = "F1_50T write/read mix";   break; }
      case   6: { pattern_name = "F1_50T write then read";  break; }
      case   7: { pattern_name = "F1_50T read only";        break; }
      case   8: { pattern_name = "F1_100T write/read mix";  break; }
      case   9: { pattern_name = "F1_100T write then read"; break; }
      case  10: { pattern_name = "F1_100T read only";       break; }
      case  11: { pattern_name = "F4_100T write/read mix";  break; }
      case  12: { pattern_name = "F4_100T write then read"; break; }
      case  13: { pattern_name = "F4_100T read only";       break; }
      case  14: { pattern_name = "SI0 write/read mix";      break; }
      case  15: { pattern_name = "SI1 write/read mix";      break; }
      case  16: { pattern_name = "SI2 write/read mix";      break; }
      case  17: { pattern_name = "SI3 write/read mix";      break; }
      case 100: { pattern_name = "XMMT SBR2 2X2";           break; }
      case 101: { pattern_name = "XMMT SBR2 4X";            break; }
      case 102: { pattern_name = "XMMT ARB8 2X2";           break; }
      case 103: { pattern_name = "XMMT ARB8 4X";            break; }
      case 104: { pattern_name = "XMMT ARB32 2X2";          break; }
      case 105: { pattern_name = "XMMT ARB32 4X";           break; }
      default: { pattern_name = "Unknown";                  break; }
    }

  } else {
    switch(pattern) {
      case  0: { pattern_name = "User";                  break; }
      case  1: { pattern_name = "Short";                 break; }
      case  2: { pattern_name = "PBRS Write/Read";       break; }
      case  3: { pattern_name = "High Power Write/Read"; break; }
      case  4: { pattern_name = "High Power Read Only";  break; }
      case  5: { pattern_name = "Checkerboard P0";       break; }
      case  6: { pattern_name = "Checkerboard P1";       break; }
      case  7: { pattern_name = "Checkerboard P2";       break; }
      case  8: { pattern_name = "Checkerboard P3";       break; }
      case  9: { pattern_name = "PRBS Write Only";       break; }
      case 10: { pattern_name = "PRBS Read Only";        break; }
      default: { pattern_name = "Unknown";               break; }
    }
  }
  return pattern_name;
}

/**
 ** @brief  Return a description for a given mbist pattern number
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the sbus master spico or APC
 ** @param  pattern      MBIST pattern number
 **
 ** @return  MBIST pattern name
 **/
static const char * avago_hbm_mbist_pattern_to_str(uint vendor, uint pattern)
{
  const char * pattern_name;
  if (vendor == SAMSUNG) {
    switch(pattern) {
      case   0: { pattern_name = "Scan";     break; }
      case   1: { pattern_name = "March";    break; }
      case   2: { pattern_name = "TSV";      break; }
      default:  { pattern_name = "Unknown";  break; }
    }

  } else {
    switch(pattern) {
      case  0: { pattern_name = "YMC";       break; }
      case  1: { pattern_name = "Gross";     break; }
      case  2: { pattern_name = "XMC";       break; }
      default: { pattern_name = "Unknown";   break; }
    }
  }
  return pattern_name;
}


/**
 ** @brief  Read the MBIST repair results from xdmem
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_read_mbist_results(Aapl_t *aapl, uint spico_addr, uint manufacturer_id)
{
  if (avago_hbm2e_check(aapl, spico_addr)) {
    uint repairs;
    uint ops;
    uint i;

    repairs = avago_spico_int(aapl, spico_addr, 0x32, 0x12);
    ops = avago_spico_int(aapl, spico_addr, 0x32, 0x13);

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Total Repairs: %d\n", repairs);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Total Ops    : %d\n", ops);

    for (i=0; i<=512; i+=2) {
      uint channel;
      uint mem;
      uint row;
      uint bank;
      uint sid;
      uint pc;
      uint enable;
      uint xdmem_addr;

      xdmem_addr = 0x620 + 260 + i;
      mem     = avago_spico_int(aapl, spico_addr, 0x37, xdmem_addr);
      bank    = mem & 0xf;
      sid     = (mem >> 4) & 0x1;
      pc      = (mem >> 5) & 0x1;
      channel = (mem >> 6) & 0x7;
      enable  = (mem >> 9) & 0x1;

      mem     = avago_spico_int(aapl, spico_addr, 0x37, xdmem_addr);
      row     = (mem >> 9) & 1;

      if (enable == 1) {
        aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   channel:%d pc:%d sid:%d bank:%d row:%d\n", channel, pc, sid, bank, row);
      }
    }

  } else {
    uint base_addr;
    uint channel;
    uint repair_num;
    uint xdmem_addr;
    uint repair_15_0;
    uint repair_31_16;
    uint repair;
    uint repair_en;
    uint sid_num;
    uint row;
    uint bank;
    uint sid;
    uint pc;
    uint dual_dq;

    if (avago_hbm_firmware_dmem_read_supported(aapl, spico_addr) == FALSE) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MBIST detailed diagnostics not supported with this version of firmware\n");
      return 0;
    }

    base_addr = 0x0518;

    for( channel = 0;  channel < 8; channel++ ) {
      for( sid_num = 0;  sid_num < 2; sid_num++ ) {
        for( repair_num = 0;  repair_num < 8; repair_num++ ) {

          xdmem_addr = base_addr + ( ((sid_num<<3) | channel ) * 16 * 2) + (repair_num*2);

          repair_15_0 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_addr);
          repair_31_16 = avago_spico_int(aapl, spico_addr, 0x37, xdmem_addr+1);
          repair = (repair_31_16 << 16) | repair_15_0;

          repair_en = 0;
          if (manufacturer_id == 0x1) {
            repair_en = (repair >> 20) & 0x1;
          } else if (manufacturer_id == 0x6) {
            repair_en = repair & 0x1;
          }

          if (repair_en == 1) {
            if (manufacturer_id == 0x1) {
              row     = repair & 0x3fff;
              bank    = (repair >> 14) & 0xf;
              pc      = (repair >> 18) & 0x1;
              sid     = (repair >> 19) & 0x1;
              dual_dq = (repair >> 21) & 0x1;
              aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d Repair:%d Row:%d Bank:%d PC:%d SID:%d DUAL_DQ:%d\n",
                  channel, repair_num, row, bank, pc, sid, dual_dq);

            } else if (manufacturer_id == 0x6) {
              row  = (repair >> 1) & 0x1fff;
              bank = (repair >> 14) & 0xf;
              sid  = (repair >> 18) & 0x1;
              aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d Repair:%d Row:%d Bank:%d SID:%d\n",
                  channel, repair_num, row, bank, sid);
            }
          }
        }
      }
    }
  }

  return 0;
}


/**
 ** @brief  Sets an hbm parameter in firmware
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  param      Parameter number to set
 ** @param  value      Parameter value to set
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_set_parameter(Aapl_t *aapl, uint spico_addr, Avago_hbm_parameter_t param, uint value)
{
  uint result;
  uint status;

  result = avago_spico_int(aapl, spico_addr, 0x34, param);
  status = result >> 16;

  if (status == 0x7fff) {
    aapl_fail(aapl, __func__, __LINE__, "Illegal HBM parameter number, %d.\n", param);
    return -1;
  }

  avago_spico_int(aapl, spico_addr, 0x35, value);

  return 0;
}


/**
 ** @brief  Gets an HBM firmware parameter
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  param      Parameter number to get
 **
 ** @return  Returns the parameter value.  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_get_parameter(Aapl_t *aapl, uint spico_addr, Avago_hbm_parameter_t param)
{
  uint result;
  uint data;
  uint status;

  result = avago_spico_int(aapl, spico_addr, 0x33, param);
  data = result & 0xffff;
  status = result >> 16;

  if (status == 0x7fff) {
    aapl_fail(aapl, __func__, __LINE__, "Illegal HBM parameter number, %d.\n", param);
    return -1;
  }

  return data;
}


/**
 ** @brief  Launches an HBM operation on a single PHY/HBM channel without checking for completion
 **
 ** @param  aapl              Aapl_t struct
 ** @param  spico_addr        sbus_address of the sbus master spico
 ** @param  operation         Operations to run
 ** @param  results           Operations to run
 ** @param  channel           Channel number to run on
 ** @param  max_timeout       Timeout value
 ** @param  wait_for_complete Set to 1 to wait for results before returning
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_launch_channel_operation_base(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    Avago_hbm_operation_results_t *results,
    uint channel,
    int max_timeout,
    int wait_for_complete)
{
  uint result;
  uint status;
  int rc;

  Avago_addr_t addr_struct;
  avago_addr_to_struct(spico_addr, &addr_struct);

  rc = aapl->return_code;
  spico_addr = avago_hbm_get_spico_address(aapl, spico_addr);
  if (aapl->return_code != rc) { return -1; }

  avago_hbm2e_check(aapl, spico_addr);

  if (avago_hbm_fw_check(aapl, spico_addr) != 0) {
    return -1;
  }

  memset(results, 0, sizeof(*results));

  if (channel == 0xf) {
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Begin HBM operation 0x%02x for all channels at Sbus address %s\n", operation, aapl_addr_to_str(spico_addr));
    result = avago_spico_int(aapl, spico_addr, 0x30, operation);
  } else {
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Begin HBM operation 0x%02x for channel%d at Sbus address%s\n", channel, aapl_addr_to_str(spico_addr));
    result = avago_spico_int(aapl, spico_addr, 0x31, ((channel << 8) | operation));
  }
  status = result >> 16;


  if (status == 0x7fff) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation already in progress at Sbus address %s\n", aapl_addr_to_str(spico_addr));
    return -1;
  }

  if (wait_for_complete == 1) {
    return avago_hbm_check_channel_operation(aapl, spico_addr, operation, results, channel, max_timeout);
  } else {
    return 0;
  }
}



/**
 ** @brief  Launches an HBM operation on all PHY/HBM channels
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  operation  Operations to run
 ** @param  results    Results structure
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_launch_operation(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    Avago_hbm_operation_results_t *results,
    int max_timeout)
{
  return avago_hbm_launch_channel_operation_base(aapl, spico_addr, operation, results, 0xf, max_timeout, 1);
}

/** @brief  Launches an HBM operation on all PHY/HBM channels and returns without checking for completion
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  operation  Operations to run
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_launch_operation_nowait(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    int max_timeout)
{
  Avago_hbm_operation_results_t results;
  return avago_hbm_launch_channel_operation_base(aapl, spico_addr, operation, &results, 0xf, max_timeout, 0);
}


/**
 ** @brief  Launches an HBM operation on a single PHY/HBM channel
 **
 ** @param  aapl        Aapl_t struct
 ** @param  spico_addr  sbus_address of the sbus master spico
 ** @param  operation   Operations to run
 ** @param  results     Results structure
 ** @param  channel     Channel number to run on
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_launch_channel_operation(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    Avago_hbm_operation_results_t *results,
    uint channel,
    int max_timeout)
{
  return avago_hbm_launch_channel_operation_base(aapl, spico_addr, operation, results, channel, max_timeout, 1);
}


/**
 ** @brief  Launches an HBM operation on a single PHY/HBM channel without checking for completion
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  operation  Operations to run
 ** @param  channel     Channel number to run on
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_launch_channel_operation_nowait(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    uint channel,
    int max_timeout)
{
  Avago_hbm_operation_results_t results;
  return avago_hbm_launch_channel_operation_base(aapl, spico_addr, operation, &results, channel, max_timeout, 0);
}

/**
 ** @brief  Waits for and checks the results of an HBM operation on all PHY/HBM channels
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  operation  Operations to run
 ** @param  results    Results structure
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_check_operation(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    Avago_hbm_operation_results_t *results,
    int max_timeout)
{
  return avago_hbm_check_channel_operation(aapl, spico_addr, operation, results, 0xf, max_timeout);
}


/**
 ** @brief  Waits for an HBM operation on a single PHY/HBM channel to complete and checks the results
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  operation  Operations to run
 ** @param  results    Results structure /// @param  channel    Channel number to run on
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_check_channel_operation(
    Aapl_t *aapl,
    uint spico_addr,
    Avago_hbm_operation_t operation,
    Avago_hbm_operation_results_t *results,
    uint channel,
    int max_timeout)
{
  int timeout = 0;
  uint result;
  int rc;

  Avago_addr_t addr_struct;
  avago_addr_to_struct(spico_addr, &addr_struct);

  (void)channel;

  memset(results, 0, sizeof(*results));

  rc = aapl->return_code;
  spico_addr = avago_hbm_get_spico_address(aapl, spico_addr);
  if (aapl->return_code != rc) { return -1; }


  avago_hbm2e_check(aapl, spico_addr);

  do {
    result = avago_spico_int(aapl, spico_addr, 0x32, 0x00);
    if (max_timeout != 0) { timeout += 1; }
  } while ((result & 0x03) != 0x01 && timeout <= max_timeout);

  aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "HBM operation result code: 0x%02x\n", result);

  if ((timeout >= max_timeout) && (max_timeout != 0)) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation timed out at Sbus address %s\n", aapl_addr_to_str(spico_addr));
    return -1;
  }

  if ((result & 0x03) != 0x01) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation result produced an unexpected result: 0x%02x.\n", result);
    return -1;
  }

  if ((result & 0x04) == 0x04) {
    avago_hbm_get_operation_results(aapl, spico_addr, results);
    aapl_fail(aapl, __func__, __LINE__, "HBM operation 0x%02x failed at Sbus address %s\n", operation, aapl_addr_to_str(spico_addr));
    return -1;

  } else {
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "HBM operation 0x%02x successfully completed on all channels at Sbus address %s\n", operation, aapl_addr_to_str(spico_addr));
    return 0;
  }

  return 0;
}

static int avago_hbm_check_operation_brief(
    Aapl_t *aapl,
    uint spico_addr,
    int max_timeout)
{
  int timeout = 0;
  uint result;
  int rc;

  Avago_addr_t addr_struct;
  avago_addr_to_struct(spico_addr, &addr_struct);

  rc = aapl->return_code;
  spico_addr = avago_hbm_get_spico_address(aapl, spico_addr);
  if (aapl->return_code != rc) { return -1; }

  avago_hbm2e_check(aapl, spico_addr);

  do {
    result = avago_spico_int(aapl, spico_addr, 0x32, 0x00);
    if (max_timeout != 0) { timeout += 1; }
  } while ((result & 0x03) != 0x01 && timeout <= max_timeout);

  aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "HBM operation result code: 0x%02x\n", result);

  if ((timeout >= max_timeout) && (max_timeout != 0)) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation timed out at Sbus address %s\n", aapl_addr_to_str(spico_addr));
    return -1;
  }

  if ((result & 0x03) != 0x01) {
    aapl_fail(aapl, __func__, __LINE__, "HBM operation result produced an unexpected result: 0x%02x.\n", result);
    return -1;
  }

  if ((result & 0x04) == 0x04) {
    return -1;
  } else {
    return 0;
  }
}


/**
 ** @brief  Reads the HBM channel errors code from firmware and stores the
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  results    Updated with the result of the operation
 **
 ** @return Always returns 0
 **/
int avago_hbm_get_operation_results(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_results_t *results)
{
  int channel;

  avago_hbm2e_check(aapl, spico_addr);

  results->global_error_code = avago_spico_int(aapl, spico_addr, 0x32, 0x01);
  for (channel = 0; channel <= 7; channel++) {
    results->channel_error_code[channel] = avago_spico_int(aapl, spico_addr, 0x32, 0x02+channel);
    results->channel_operation_code[channel]  = avago_spico_int(aapl, spico_addr, 0x32, 0x0a+channel);
  }

  return 0;
}


/**
 ** @brief  Print the operation results
 **
 ** @param  aapl       Aapl_t struct
 ** @param  results    Operation results struct
 **
 ** @return  0
 **/
int avago_hbm_print_operation_results(Aapl_t *aapl, Avago_hbm_operation_results_t *results)
{

  if (results->global_error_code == 0) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Operation passed\n");

  } else {
    int channel;
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Operation failed\n");

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   global_error_code:%s\n",
        avago_hbm_error_code_desc(results->global_error_code));

    for (channel = 0; channel <= 7; channel++) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   channel:%1d error_code:%s\n",
          channel, avago_hbm_error_code_desc(results->channel_error_code[channel]) );
    }

    for (channel = 0; channel <= 7; channel++) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   channel:%1d operation:%s\n",
          channel, avago_hbm_operation_desc(results->channel_operation_code[channel]) );
    }
  }

  return 0;
}


/**
 ** @brief  Retrieves the APC Address via firmware
 **
 ** @param  spico_addr sbus_address of the sbus master spico
 **
 ** @return  Error code description
 **/
int avago_hbm_get_apc_addr(Aapl_t *aapl, uint spico_addr, uint *apc_addr)
{
  uint max_sbus_addr;
  uint local_addr;
  int rc;
  Avago_addr_t addr_struct;

  rc = aapl->return_code;
  spico_addr = avago_hbm_get_spico_address(aapl, spico_addr);
  if (aapl->return_code != rc) { return -1; }




  avago_addr_to_struct(spico_addr, &addr_struct);
  *apc_addr = 0;

  max_sbus_addr = aapl_get_max_sbus_addr(aapl, spico_addr);
  for (local_addr = 1; local_addr <= max_sbus_addr; local_addr++) {
    addr_struct.sbus = local_addr;

    if (aapl_check_ip_type(aapl, avago_struct_to_addr(&addr_struct), __func__, __LINE__, FALSE, 1, AVAGO_APC)) {
      *apc_addr = avago_struct_to_addr(&addr_struct);
      *apc_addr = avago_make_addr3(addr_struct.chip, addr_struct.ring, *apc_addr);
      break;
    }
  }

  if (*apc_addr == 0) {
    aapl_fail(aapl, __func__, __LINE__, "Failed to find a valid APC address\n");
    return -1;
  } else {
    return 0;
  }
}


/**
 ** @brief  Sets the HBM drive strength in the mode registers parameters
 **
 ** @param  spico_addr sbus_address of the sbus master spico
 **
 ** @return  Error code description
 **/
int avago_hbm_set_hbm_drv_str(Aapl_t *aapl, uint spico_addr, uint drvstr)
{
  uint hbm_mr1;
  uint hbm_test_mr1;

  uint hbm_mr1_param;
  uint hbm_test_mr1_param;

  if (avago_hbm2e_check(aapl, spico_addr)) {
    hbm_mr1_param = AVAGO_HBM2E_MR1;
    hbm_test_mr1_param = AVAGO_HBM2E_TEST_MR1;
  } else {
    hbm_mr1_param = AVAGO_HBM_MODE_REGISTER1;
    hbm_test_mr1_param = AVAGO_HBM_TEST_MODE_REGISTER1;
  }

  hbm_mr1 = avago_hbm_get_parameter(aapl, spico_addr, (Avago_hbm_parameter_t)hbm_mr1_param);
  hbm_mr1 = (hbm_mr1 & 0x1f) | (drvstr << 5);
  avago_hbm_set_parameter(aapl, spico_addr, (Avago_hbm_parameter_t)hbm_mr1_param, hbm_mr1);

  hbm_test_mr1 = avago_hbm_get_parameter(aapl, spico_addr, (Avago_hbm_parameter_t)hbm_test_mr1_param);
  hbm_test_mr1 = (hbm_test_mr1 & 0x1f) | (drvstr << 5);
  avago_hbm_set_parameter(aapl, spico_addr, (Avago_hbm_parameter_t)hbm_mr1_param, hbm_mr1);
  avago_hbm_set_parameter(aapl, spico_addr, (Avago_hbm_parameter_t)hbm_test_mr1_param, hbm_test_mr1);


  return 0;
}


/**
 ** @brief  Sets the PHY drive strength firwmare parameters
 **
 ** @param  spico_addr sbus_address of the sbus master spico
 **
 ** @return  Error code description
 **/
int avago_hbm_set_phy_drv_str(Aapl_t *aapl, uint spico_addr, uint drvstr)
{
  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_PHY_CONFIG_DRV_IMP, drvstr);

  } else {
    uint phy_config0;
    uint phy_config1;
    uint phy_config2;
    uint phy_config3;
    uint phy_config4;
    uint phy_config5;
    uint phy_config6;

    phy_config0 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG0);
    phy_config1 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG1);
    phy_config2 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG2);
    phy_config3 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG3);
    phy_config4 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG4);
    phy_config5 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG5);
    phy_config6 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG6);

    phy_config0 = (phy_config0 & 0x7fff) | ((drvstr & 0x1) << 15);
    phy_config1 = (phy_config1 & 0xfff8) | ((drvstr & 0x7) >> 1);
    phy_config2 = (phy_config2 & 0xf87f) | (drvstr << 7);
    phy_config3 = (phy_config3 & 0xff0f) | (drvstr << 4);
    phy_config4 = (phy_config4 & 0x787f) | (drvstr << 7);
    phy_config5 = (phy_config5 & 0x7fff) | ((drvstr & 0x1) << 15);
    phy_config6 = (phy_config6 & 0xfff8) | ((drvstr & 0x7) >> 1);

    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG0, phy_config0);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG1, phy_config1);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG2, phy_config2);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG3, phy_config3);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG4, phy_config4);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG5, phy_config5);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG6, phy_config6);
  }

  return 0;
}

/**
 ** @brief  Sets the PHY drive strength in the mode registers parameters
 **
 ** @param  spico_addr sbus_address of the sbus master spico
 **
 ** @return  Error code description
 **/
int avago_hbm_set_phy_quarter_cycle(Aapl_t *aapl, uint spico_addr, uint qc_rd, uint qc_wr, uint qc_ck)
{
  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_PHY_CONFIG_QC_DELAY,
        ((qc_ck & 0xf) << 6) | ((qc_wr & 0xf) << 3) | (qc_rd & 0xf));

  } else {
    uint phy_config0;
    uint phy_config1;
    uint phy_config2;
    uint phy_config3;
    uint phy_config4;
    uint phy_config5;
    uint phy_config6;
    Avago_hbm_operation_results_t results;

    phy_config0 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG0);
    phy_config1 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG1);
    phy_config2 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG2);
    phy_config3 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG3);
    phy_config4 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG4);
    phy_config5 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG5);
    phy_config6 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG6);

    phy_config0 = (phy_config0 & 0xc0ff) | (qc_rd << 8) | (qc_wr << 11);
    phy_config2 = (phy_config2 & 0xffc0) | (qc_rd << 0) | (qc_wr << 3);
    phy_config3 = (phy_config3 & 0xfff1) | (qc_ck << 1);
    phy_config4 = (phy_config4 & 0xffc0) | (qc_rd << 0) | (qc_wr << 3);
    phy_config5 = (phy_config5 & 0xc0ff) | (qc_rd << 8) | (qc_wr << 11);

    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG0, phy_config0);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG1, phy_config1);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG2, phy_config2);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG3, phy_config3);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG4, phy_config4);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG5, phy_config5);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG6, phy_config6);

    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET_PHY_CONFIG, &results, avago_hbm_default_timeout);
  }

  return 0;
}


/**
 ** @brief  Retrieves the HBM device id for the HBM on the SBus ring
 **
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @param  device_id  reference to a Avago_hbm_device_id_t struct
 **
 ** @return  Error code description
 **/
int avago_hbm_read_device_id(Aapl_t *aapl, uint spico_addr, Avago_hbm_device_id_t *device_id)
{
  uint device_id_15_0;
  uint device_id_31_16;
  uint device_id_47_32;
  uint device_id_63_48;
  uint device_id_79_64;
  uint device_id_82_80;
  int rc;
  Avago_hbm_operation_results_t results;

  if (avago_hbm2e_check(aapl, spico_addr)) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_READ_DEVICE_ID, &results, avago_hbm_default_timeout);
  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_READ_DEVICE_ID, &results, avago_hbm_default_timeout);
  }
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    return -1;
  }

  device_id_15_0  = avago_spico_int(aapl, spico_addr, 0x32, 0x12);
  device_id_31_16 = avago_spico_int(aapl, spico_addr, 0x32, 0x13);
  device_id_47_32 = avago_spico_int(aapl, spico_addr, 0x32, 0x14);
  device_id_63_48 = avago_spico_int(aapl, spico_addr, 0x32, 0x15);
  device_id_79_64 = avago_spico_int(aapl, spico_addr, 0x32, 0x16);
  device_id_82_80 = avago_spico_int(aapl, spico_addr, 0x32, 0x17);

  memset(device_id, 0, sizeof(*device_id));

  device_id->value0 = ( device_id_31_16 << 16) | device_id_15_0;
  device_id->value1 = ( device_id_63_48 << 16) | device_id_47_32;
  device_id->value2 = ( device_id_82_80 << 16) | device_id_79_64;

  device_id->gen2_test = device_id_82_80 >> 1;

  device_id->ecc = device_id_82_80 & 0x1;

  device_id->density = device_id_79_64 >> 12;

  device_id->manufacturer_id = (device_id_79_64 >> 8) & 0xf;

  device_id->manufacturing_loc = (device_id_79_64 >> 4) & 0xf;

  device_id->manufacturing_year = ((device_id_79_64 & 0xf) << 4) | ((device_id_63_48 >> 12) & 0xf);

  device_id->manufacturing_week = (device_id_63_48 >> 4) & 0xff;

  device_id->serial_number_31_0 = ((bigint) device_id_31_16 >> 2) | ((bigint) device_id_47_32 << 14) | ((device_id_63_48 & 0xfffff) << 30);

  if ((device_id->manufacturer_id == SAMSUNG) && (device_id->manufacturing_year >= 9) && (device_id->manufacturing_week >= 6)) {
    device_id->serial_number_35_32 = (((device_id_79_64 >> 5) & 0x3) << 2) | ((device_id_63_48 >> 2) & 0x3);
  } else {
    device_id->serial_number_35_32 = (device_id_63_48 >> 2) & 0x3;
  }

  device_id->addressing_mode = device_id_31_16 & 0x3;

  device_id->channel_available = (device_id_15_0 >> 8);

  device_id->hbm_stack_height = (device_id_15_0 >> 7) & 1;

  device_id->model_number = device_id_15_0 & 0x7f;

  return 0;
}


/**
 ** @brief   Prints the HBM device id to the log
 **
 ** @param   spico_addr sbus_address of the sbus master spico
 **
 ** @return  0
 **/
int avago_hbm_print_device_id(Aapl_t *aapl, uint spico_addr)
{
  Avago_hbm_device_id_t device_id;
  avago_hbm_read_device_id(aapl, spico_addr, &device_id);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "HBM Device ID: 0x%05x%08x%08x\n",
      device_id.value2,
      device_id.value1,
      device_id.value0);

  if (device_id.gen2_test == 1) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Gen2 Features:      supported\n");
  } else {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Gen2 Features:      not supported\n");
  }

  if (device_id.ecc == 1) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   ECC Support:        supported\n");
  } else {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   ECC Support:        not supported\n");
  }

  switch(device_id.density) {
    case 0x01: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            1Gb\n");     break; }
    case 0x02: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            2Gb\n");     break; }
    case 0x03: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            4Gb\n");     break; }
    case 0x04: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            8Gb\n");     break; }
    case 0x05: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            16Gb\n");    break; }
    case 0x06: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            8Gb\n");    break; }
    case 0x08: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            12Gb\n");    break; }
    case 0x0a: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            16Gb\n");    break; }
    case 0x0b: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            12Gb\n");    break; }
    case 0x0c: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            16Gb\n");    break; }
    default:   { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Density:            Unknown\n"); break; }
  }

  switch(device_id.manufacturer_id) {
    case 0x01: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Manufacturer ID:    Samsung\n");  break; }
    case 0x06: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Manufacturer ID:    SK Hynix\n"); break; }
    default:   { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Manufacturer ID:    Unknown\n");  break; }
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Manufacturer Loc:   %d\n", device_id.manufacturing_loc);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Manufacturer Year:  %d\n", 2011+device_id.manufacturing_year);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Manufacturer Week:  %d\n", device_id.manufacturing_week);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Serial Number:      0x%x%08lx\n", device_id.serial_number_35_32, device_id.serial_number_31_0);

  switch(device_id.addressing_mode) {
    case 0x01: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Addressing Mode:    Pseudo Channel Support\n");  break; }
    case 0x02: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Addressing Mode:    Legacy Mode Support\n");  break; }
    default:   { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Addressing Mode:    Unknown\n");  break; }
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channels Available: 0x%02x\n", device_id.channel_available);

  switch(device_id.hbm_stack_height) {
    case 0x0: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   HBM Stack Height:   2 or 4 High Stack\n");  break; }
    case 0x1: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   HBM Stack Height:   8 High Stack\n");  break; }
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Model Number:       %d\n", device_id.model_number);

  return 0;
}


/**
 ** @brief  Retrieves the HBM device id for the HBM on the SBus ring
 **
 ** @param  spico_addr sbus_address of the sbus master spico
 **
 ** @return  Error code description
 **/
int avago_hbm_read_mrs(Aapl_t *aapl, uint spico_addr, Avago_hbm_mrs_t *mrs, uint channel)
{
  uint apc_addr;
  uint data;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  memset(mrs, 0, sizeof(*mrs));

  avago_hbm_wir_write_channel(aapl, apc_addr, HBM_SEL, channel, HBM_MODE_REG_DUMP_SET);
  avago_hbm_wdr_read(aapl, apc_addr, HBM_MODE_REG_DUMP_SET_LENGTH, 0);
  avago_hbm_wdr_set_read_channel(aapl, apc_addr, channel);

  data = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0);
  mrs->mr0 = data & 0xff;
  mrs->mr1 = (data >> 8) & 0xff;
  mrs->mr2 = (data >> 16) & 0xff;
  mrs->mr3 = (data >> 24) & 0xff;

  data = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 1);
  mrs->mr4 = data & 0xff;
  mrs->mr5 = (data >> 8) & 0xff;
  mrs->mr6 = (data >> 16) & 0xff;
  mrs->mr7 = (data >> 24) & 0xff;

  data = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 2);
  mrs->mr8 = data & 0xff;
  mrs->mr9 = (data >> 8) & 0xff;
  mrs->mr10 = (data >> 16) & 0xff;
  mrs->mr11 = (data >> 24) & 0xff;

  data = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 3);
  mrs->mr12 = data & 0xff;
  mrs->mr13 = (data >> 8) & 0xff;
  mrs->mr14 = (data >> 16) & 0xff;
  mrs->mr15 = (data >> 24) & 0xff;

  return 0;
}


/**
 ** @brief   Prints the HBM mode registers to the log
 **
 ** @param   spico_addr sbus_address of the sbus master spico
 **
 ** @return  0
 **/
int avago_hbm_print_mrs(Aapl_t *aapl, uint spico_addr, uint channel)
{
  Avago_hbm_mrs_t mrs;
  uint dbiac_read;
  uint dbiac_write;
  uint tcsr;
  uint dq_read_parity;
  uint dq_write_parity;
  uint addr_cmd_parity;
  uint test_mode;
  uint write_recovery;
  uint drive_strength;
  uint wl;
  uint rl;
  uint ras;
  uint bank_group;
  uint bl;
  uint ecc;
  uint dm;
  uint pl;
  uint trr_mode_ban;
  uint trr_ps_select;
  uint trr;
  uint impre_trp;
  uint loopback;
  uint read_mux_ctrl;
  uint misr_ctrl;
  uint cattrip;
  uint da28_lockout;
  uint int_vref;

  avago_hbm_read_mrs(aapl, spico_addr, &mrs, channel);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "HBM MRS: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
      mrs.mr15,
      mrs.mr14,
      mrs.mr13,
      mrs.mr12,
      mrs.mr11,
      mrs.mr10,
      mrs.mr9,
      mrs.mr8,
      mrs.mr7,
      mrs.mr6,
      mrs.mr5,
      mrs.mr4,
      mrs.mr3,
      mrs.mr2,
      mrs.mr1,
      mrs.mr0);

  dbiac_read      = (mrs.mr0 >> 0) & 0x1;
  dbiac_write     = (mrs.mr0 >> 1) & 0x1;
  tcsr            = (mrs.mr0 >> 2) & 0x1;
  dq_read_parity  = (mrs.mr0 >> 4) & 0x1;
  dq_write_parity = (mrs.mr0 >> 5) & 0x1;
  addr_cmd_parity = (mrs.mr0 >> 6) & 0x1;
  test_mode       = (mrs.mr0 >> 7) & 0x1;
  write_recovery  = (mrs.mr1 >> 0) & 0x1f;
  drive_strength  = (mrs.mr1 >> 5) & 0x7;
  wl              = (mrs.mr2 >> 0) & 0x7;
  rl              = (mrs.mr2 >> 3) & 0x1f;
  ras             = (mrs.mr3 >> 0) & 0x3f;
  bank_group      = (mrs.mr3 >> 6) & 0x1;
  bl              = (mrs.mr3 >> 7) & 0x1;
  ecc             = (mrs.mr4 >> 0) & 0x1;
  dm              = (mrs.mr4 >> 1) & 0x1;
  pl              = (mrs.mr4 >> 2) & 0x3;
  trr_mode_ban    = (mrs.mr5 >> 0) & 0xf;
  trr_ps_select   = (mrs.mr5 >> 6) & 0x1;
  trr             = (mrs.mr5 >> 7) & 0x1;
  impre_trp       = (mrs.mr6 >> 3) & 0x1f;
  loopback        = (mrs.mr7 >> 0) & 0x1;
  read_mux_ctrl   = (mrs.mr7 >> 1) & 0x3;
  misr_ctrl       = (mrs.mr7 >> 3) & 0x7;
  cattrip         = (mrs.mr7 >> 7) & 0x1;
  da28_lockout    = (mrs.mr8 >> 0) & 0x1;
  int_vref        = (mrs.mr15 >> 0) & 0x7;

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- %s\n", test_mode == 1 ? "Test Mode" : "Normal Mode");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- Address, Command Parity %s\n", addr_cmd_parity == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- DQ Bus Write Parity %s\n", dq_write_parity == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- DQ Bus Read Parity %s\n", dq_read_parity == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- Temp Compensated Self Refresh %s\n", tcsr == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- Write DBIac %s\n", dbiac_write == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR0 -- Read DBIac %s\n", dbiac_read == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR1 -- Nominal Drive Strength = %d mA driver\n", drive_strength*3+6);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR1 -- Write Recovery WR = %d nCK\n", write_recovery);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR2 -- Read Latency = %d nCK\n", rl+2);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR2 -- Write Latency = %d nCK\n", wl+1);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR3 -- Burst Length = %d\n", bl == 1 ? 4 : 2);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR3 -- Bank Group %s\n", bank_group == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR3 -- Activate to Precharge RAS = %d nCK\n", ras);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR4 -- Parity Latency = %d nCK\n", pl);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR4 -- DM %s\n", dm == 1 ? "Disabled" : "Enabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR4 -- ECC %s\n", ecc == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR5 -- TRR Mode %s\n", trr == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR5 -- Enable TRR mode for Pseudo Channel %d\n", trr_ps_select);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR5 -- TRR Mode Bank %d\n", trr_mode_ban);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR6 -- imPRE tRP Value = %d nCK\n", impre_trp+2);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- %s CATTRIP\n", cattrip == 1 ? "Assert" : "Clear");

  switch(misr_ctrl) {
    case 0x0: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Preset mode\n"); break; }
    case 0x1: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- LFSR mode (read)\n"); break; }
    case 0x2: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Register mode\n"); break; }
    case 0x3: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- MISR mode\n"); break; }
    case 0x4: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- LFSR compare mode (write)\n"); break; }
  }

  switch(read_mux_ctrl) {
    case 0x0: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Reserved DWORD Read Mux Control\n"); break; }
    case 0x1: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Return data from MISR registers\n"); break; }
    case 0x2: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Return data from Rx path sampler\n"); break; }
    case 0x3: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Return LFSR_COMPARE_STICKY\n"); break; }
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR7 -- Loopback %s\n", loopback == 1 ? "Enabled" : "Disabled");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR8 -- DA28 lockout %s\n", da28_lockout == 1 ? "Enabled" : "Disabled");

  switch(int_vref) {
    case 0x0: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 50 percent VDD\n"); break; }
    case 0x1: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 46 percent VDD\n"); break; }
    case 0x2: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 42 percent VDD\n"); break; }
    case 0x3: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 38 percent VDD\n"); break; }
    case 0x4: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 54 percent VDD\n"); break; }
    case 0x5: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 58 percent VDD\n"); break; }
    case 0x6: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 62 percent VDD\n"); break; }
    case 0x7: { aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MR15 -- Internal Vref 66 percent VDD\n"); break; }
  }

  return 0;
}


/**
 ** @brief   Reads the PHY_CONFIG WDR contents from the PHY
 **
 ** @param   spico_addr sbus_address of the sbus master spico
 **
 ** @return  0
 **/
int avago_hbm_read_phy_config(Aapl_t *aapl, uint spico_addr, Avago_hbm_phy_config_t *phy_config, uint channel)
{
  Avago_addr_t addr_struct;
  uint apc_addr;
  uint length;
  uint phy_config_31_0;
  uint phy_config_63_32;
  uint phy_config_95_64;
  uint phy_config_127_96;
  uint phy_config_159_128;
  uint phy_config_191_160;
  uint phy_config_223_192;

  avago_addr_to_struct(spico_addr, &addr_struct);
  apc_addr = avago_spico_int(aapl, spico_addr, 0x36, 0);
  apc_addr = avago_make_addr3(addr_struct.chip, addr_struct.ring, apc_addr);

  memset(phy_config, 0, sizeof(*phy_config));

  if (avago_hbm2e_check(aapl, spico_addr)) {
    length = PHY_CONFIG_HBM2E_LENGTH;
  } else {
    length = PHY_CONFIG_LENGTH;
  }

  avago_hbm_wir_write_channel(aapl, apc_addr, PHY_SEL, channel, PHY_CONFIG);
  avago_hbm_wdr_read(aapl, apc_addr, length, 0);
  avago_hbm_wdr_set_read_channel(aapl, apc_addr, channel);

  phy_config_31_0    = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0);
  phy_config_63_32   = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 1);
  phy_config_95_64   = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 2);
  phy_config_127_96  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 3);
  phy_config_159_128 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 4);
  phy_config_191_160 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 5);
  phy_config_223_192 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 6);

  if (avago_hbm2e_check(aapl, spico_addr)) {
    phy_config->dword0 = ((bigint)(phy_config_63_32 & 0xffff) << 32) | (bigint)phy_config_31_0;
    phy_config->dword1 = ((bigint)phy_config_95_64 << 16) | (bigint)(phy_config_63_32 >> 16);
    phy_config->aword  = (bigint)(phy_config_127_96 & 0xfffff);
    phy_config->dword2 = ((bigint)(phy_config_191_160 & 0xff) << 40) | ((bigint)(phy_config_159_128) << 8) | (bigint)(phy_config_127_96 >> 24);
    phy_config->dword3 = ((bigint)(phy_config_223_192 & 0xffffff) << 24) | (bigint)(phy_config_191_160 >> 8);
  } else {
    phy_config->dword0 = (bigint)(phy_config_31_0 & 0xffffff);
    phy_config->dword1 = ((bigint)(phy_config_63_32 & 0xffff) << 8) | (bigint)(phy_config_31_0 >> 24);
    phy_config->aword =  (bigint)((phy_config_63_32 >> 16) & 0xff);
    phy_config->dword2 = ((bigint)(phy_config_95_64 & 0xffff) << 8) | (bigint)(phy_config_63_32 >> 24);
    phy_config->dword3 = ((bigint)(phy_config_127_96 & 0xff) << 16) | (bigint)(phy_config_95_64 >> 16);
  }

  return 0;
}

/**
 ** @brief   Prints the PHY_CONFIG WDR contents from the PHY
 **
 ** @param   spico_addr sbus_address of the sbus master spico
 **
 ** @return  0
 **/
int avago_hbm_print_phy_config(Aapl_t *aapl, uint spico_addr, uint channel)
{
  BOOL common_dword_values;
  Avago_hbm_phy_config_t phy_config;
  (void) channel;

  avago_hbm_read_phy_config(aapl, spico_addr, &phy_config, 0);

  common_dword_values = TRUE;
  if (phy_config.dword0 != phy_config.dword1) { common_dword_values = FALSE; }
  if (phy_config.dword0 != phy_config.dword2) { common_dword_values = FALSE; }
  if (phy_config.dword0 != phy_config.dword3) { common_dword_values = FALSE; }


  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "PHY_CONFIG Raw Values\n");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   PHY_CONFIG DWORD0 -- 0x%lx\n", phy_config.dword0);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   PHY_CONFIG DWORD1 -- 0x%lx\n", phy_config.dword1);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   PHY_CONFIG AWORD  -- 0x%lx\n", phy_config.aword);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   PHY_CONFIG DWORD2 -- 0x%lx\n", phy_config.dword2);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   PHY_CONFIG DWORD3 -- 0x%lx\n", phy_config.dword3);


  if (avago_hbm2e_check(aapl, spico_addr)) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "PHY_CONFIG Decoded Values\n");
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 T_RDLAT_OFFSET[3:0]                   0x%0x\n",  (phy_config.dword0 & 0xf));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_RD_DISABLE                         0x%0x\n",  ((phy_config.dword0 >> 4) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_WR_DISABLE                         0x%0x\n",  ((phy_config.dword0 >> 5) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_RD_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword0 >> 6) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_WR_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword0 >> 9) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_RD_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword0 >> 12) & 0x7f));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_WR_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword0 >> 19) & 0x7f));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 DRV_IMP_CONFIG[3:0]                   0x%0x\n",  ((phy_config.dword0 >> 24) & 0xf));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 DIFF_ENHANCE_RDQS                     0x%0x\n",  ((phy_config.dword0 >> 30) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 DIFF_ENHANCE_WDQS                     0x%0x\n",  ((phy_config.dword0 >> 31) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 BPC_MODE                              0x%0x\n",  ((phy_config.dword0 >> 32) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 BPC_EN                                0x%0x\n",  ((phy_config.dword0 >> 33) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_PAR_LATENCY[1:0]               0x%0x\n",  ((phy_config.dword0 >> 34) & 0x3));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_PAR_RD_PAR_EN                  0x%0x\n",  ((phy_config.dword0 >> 36) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_PAR_WR_PAR_EN                  0x%0x\n",  ((phy_config.dword0 >> 37) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_DFI_FIFO_MC2PHY_ENABLE         0x%0x\n",  ((phy_config.dword0 >> 40) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_DFI_FIFO_PHY2MC_ENABLE         0x%0x\n",  ((phy_config.dword0 >> 41) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_DFI_FIFO_MC2PHY_UL_RESET_VALUE 0x%0x\n",  ((phy_config.dword0 >> 42) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_DFI_FIFO_MC2PHY_LD_RESET_VALUE 0x%0x\n",  ((phy_config.dword0 >> 43) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 CONFIG_DFI_FIFO_RESET_DELAY           0x%0x\n",  ((phy_config.dword0 >> 44) & 0x1));

    if (common_dword_values == FALSE) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 T_RDLAT_OFFSET[3:0]                   0x%0x\n",  (phy_config.dword1 & 0xf));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_RD_DISABLE                         0x%0x\n",  ((phy_config.dword1 >> 4) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_WR_DISABLE                         0x%0x\n",  ((phy_config.dword1 >> 5) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_RD_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword1 >> 6) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_WR_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword1 >> 9) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_RD_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword1 >> 12) & 0x7f));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_WR_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword1 >> 19) & 0x7f));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 DRV_IMP_CONFIG[3:0]                   0x%0x\n",  ((phy_config.dword1 >> 24) & 0xf));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 DIFF_ENHANCE_RDQS                     0x%0x\n",  ((phy_config.dword1 >> 30) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 DIFF_ENHANCE_WDQS                     0x%0x\n",  ((phy_config.dword1 >> 31) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 BPC_MODE                              0x%0x\n",  ((phy_config.dword1 >> 32) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 BPC_EN                                0x%0x\n",  ((phy_config.dword1 >> 33) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_PAR_LATENCY[1:0]               0x%0x\n",  ((phy_config.dword1 >> 34) & 0x3));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_PAR_RD_PAR_EN                  0x%0x\n",  ((phy_config.dword1 >> 36) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_PAR_WR_PAR_EN                  0x%0x\n",  ((phy_config.dword1 >> 37) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_DFI_FIFO_MC2PHY_ENABLE         0x%0x\n",  ((phy_config.dword1 >> 40) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_DFI_FIFO_PHY2MC_ENABLE         0x%0x\n",  ((phy_config.dword1 >> 41) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_DFI_FIFO_MC2PHY_UL_RESET_VALUE 0x%0x\n",  ((phy_config.dword1 >> 42) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_DFI_FIFO_MC2PHY_LD_RESET_VALUE 0x%0x\n",  ((phy_config.dword1 >> 43) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 CONFIG_DFI_FIFO_RESET_DELAY           0x%0x\n",  ((phy_config.dword1 >> 44) & 0x1));
    }

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  DRV_IMP_CONFIG[3:0]                   0x%0x\n",  ((phy_config.aword >> 0) & 0xf));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_QC_CK_DELAY[2:0]               0x%0x\n",  ((phy_config.aword >> 4) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_QC_CK_OFFSET[6:0]              0x%0x\n",  ((phy_config.aword >> 7) & 0x7f));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_DFI_FIFO_MC2PHY_EN             0x%0x\n",  ((phy_config.aword >> 16) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_DFI_FIFO_PHY2MC_EN             0x%0x\n",  ((phy_config.aword >> 17) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_DFI_FIFO_LD_RESET_VALUE        0x%0x\n",  ((phy_config.aword >> 18) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_DFI_FIFO_UL_RESET_VALUE        0x%0x\n",  ((phy_config.aword >> 19) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CONFIG_DFI_FIFO_RESET_DELAY           0x%0x\n",  ((phy_config.aword >> 20) & 0x1));

    if (common_dword_values == FALSE) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 T_RDLAT_OFFSET[3:0]                   0x%0x\n",  (phy_config.dword2 & 0xf));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_RD_DISABLE                         0x%0x\n",  ((phy_config.dword2 >> 4) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_WR_DISABLE                         0x%0x\n",  ((phy_config.dword2 >> 5) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_RD_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword2 >> 6) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_WR_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword2 >> 9) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_RD_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword2 >> 12) & 0x7f));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_WR_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword2 >> 19) & 0x7f));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 DRV_IMP_CONFIG[3:0]                   0x%0x\n",  ((phy_config.dword2 >> 24) & 0xf));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 DIFF_ENHANCE_RDQS                     0x%0x\n",  ((phy_config.dword2 >> 30) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 DIFF_ENHANCE_WDQS                     0x%0x\n",  ((phy_config.dword2 >> 31) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 BPC_MODE                              0x%0x\n",  ((phy_config.dword2 >> 32) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 BPC_EN                                0x%0x\n",  ((phy_config.dword2 >> 33) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_PAR_LATENCY[1:0]               0x%0x\n",  ((phy_config.dword2 >> 34) & 0x3));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_PAR_RD_PAR_EN                  0x%0x\n",  ((phy_config.dword2 >> 36) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_PAR_WR_PAR_EN                  0x%0x\n",  ((phy_config.dword2 >> 37) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_DFI_FIFO_MC2PHY_ENABLE         0x%0x\n",  ((phy_config.dword2 >> 40) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_DFI_FIFO_PHY2MC_ENABLE         0x%0x\n",  ((phy_config.dword2 >> 41) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_DFI_FIFO_MC2PHY_UL_RESET_VALUE 0x%0x\n",  ((phy_config.dword2 >> 42) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_DFI_FIFO_MC2PHY_LD_RESET_VALUE 0x%0x\n",  ((phy_config.dword2 >> 43) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 CONFIG_DFI_FIFO_RESET_DELAY           0x%0x\n",  ((phy_config.dword2 >> 44) & 0x1));

      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 T_RDLAT_OFFSET[3:0]                   0x%0x\n",  (phy_config.dword3 & 0xf));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_RD_DISABLE                         0x%0x\n",  ((phy_config.dword3 >> 4) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_WR_DISABLE                         0x%0x\n",  ((phy_config.dword3 >> 5) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_RD_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword3 >> 6) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_WR_DELAY[2:0]                      0x%0x\n",  ((phy_config.dword3 >> 9) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_RD_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword3 >> 12) & 0x7f));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_WR_OFFSET[6:0]                     0x%0x\n",  ((phy_config.dword3 >> 19) & 0x7f));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 DRV_IMP_CONFIG[3:0]                   0x%0x\n",  ((phy_config.dword3 >> 24) & 0xf));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 DIFF_ENHANCE_RDQS                     0x%0x\n",  ((phy_config.dword3 >> 30) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 DIFF_ENHANCE_WDQS                     0x%0x\n",  ((phy_config.dword3 >> 31) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 BPC_MODE                              0x%0x\n",  ((phy_config.dword3 >> 32) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 BPC_EN                                0x%0x\n",  ((phy_config.dword3 >> 33) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_PAR_LATENCY[1:0]               0x%0x\n",  ((phy_config.dword3 >> 34) & 0x3));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_PAR_RD_PAR_EN                  0x%0x\n",  ((phy_config.dword3 >> 36) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_PAR_WR_PAR_EN                  0x%0x\n",  ((phy_config.dword3 >> 37) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_DFI_FIFO_MC2PHY_ENABLE         0x%0x\n",  ((phy_config.dword3 >> 40) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_DFI_FIFO_PHY2MC_ENABLE         0x%0x\n",  ((phy_config.dword3 >> 41) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_DFI_FIFO_MC2PHY_UL_RESET_VALUE 0x%0x\n",  ((phy_config.dword3 >> 42) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_DFI_FIFO_MC2PHY_LD_RESET_VALUE 0x%0x\n",  ((phy_config.dword3 >> 43) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 CONFIG_DFI_FIFO_RESET_DELAY           0x%0x\n",  ((phy_config.dword3 >> 44) & 0x1));
    }

  } else {

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "PHY_CONFIG Decoded Values\n");
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 T_RDLAT_OFFSET[2:0]         0x%0x\n",  (phy_config.dword0 & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 MMT_PAR_ENABLE              0x%0x\n",  ((phy_config.dword0 >> 3) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 PHY_PAR_LATENCY[1:0]        0x%0x\n",  ((phy_config.dword0 >> 4) & 0x3));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_RD_DISABLE               0x%0x\n",  ((phy_config.dword0 >> 6) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_WR_DISABLE               0x%0x\n",  ((phy_config.dword0 >> 7) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_RD_DELAY[2:0]            0x%0x\n",  ((phy_config.dword0 >> 8) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 QC_WR_DELAY[2:0]            0x%0x\n",  ((phy_config.dword0 >> 11) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 MMT_ECC_EN                  0x%0x\n",  ((phy_config.dword0 >> 14) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 DRV_IMP_CONFIG[3:0]         0x%0x\n",  ((phy_config.dword0 >> 15) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 DIFF_ENHANCE_RDQS           0x%0x\n",  ((phy_config.dword0 >> 19) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 DIFF_ENHANCE_WDQS           0x%0x\n",  ((phy_config.dword0 >> 20) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 BPC_MODE                    0x%0x\n",  ((phy_config.dword0 >> 21) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 BPC_EN                      0x%0x\n",  ((phy_config.dword0 >> 22) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD0 MMT_DBI_EN                  0x%0x\n",  ((phy_config.dword0 >> 23) & 0x1));

    if (common_dword_values == FALSE) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 T_RDLAT_OFFSET[2:0]         0x%0x\n",  (phy_config.dword1 & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 MMT_PAR_ENABLE              0x%0x\n",  ((phy_config.dword1 >> 3) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 PHY_PAR_LATENCY[1:0]        0x%0x\n",  ((phy_config.dword1 >> 4) & 0x3));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_RD_DISABLE               0x%0x\n",  ((phy_config.dword1 >> 6) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_WR_DISABLE               0x%0x\n",  ((phy_config.dword1 >> 7) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_RD_DELAY[2:0]            0x%0x\n",  ((phy_config.dword1 >> 8) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 QC_WR_DELAY[2:0]            0x%0x\n",  ((phy_config.dword1 >> 11) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 MMT_ECC_EN                  0x%0x\n",  ((phy_config.dword1 >> 14) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 DRV_IMP_CONFIG[3:0]         0x%0x\n",  ((phy_config.dword1 >> 15) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 DIFF_ENHANCE_RDQS           0x%0x\n",  ((phy_config.dword1 >> 19) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 DIFF_ENHANCE_WDQS           0x%0x\n",  ((phy_config.dword1 >> 20) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 BPC_MODE                    0x%0x\n",  ((phy_config.dword1 >> 21) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 BPC_EN                      0x%0x\n",  ((phy_config.dword1 >> 22) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD1 MMT_DBI_EN                  0x%0x\n",  ((phy_config.dword1 >> 23) & 0x1));
    }

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  AERR_CAP_CONFIG             0x%0x\n",  ((phy_config.aword >> 0) & 0x1));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  CLK_QC_CONFIG[2:0]          0x%0x\n",  ((phy_config.aword >> 1) & 0x7));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   AWORD  DRV_IMP_CONFIG[2:0]         0x%0x\n",  ((phy_config.aword >> 4) & 0xf));

    if (common_dword_values == FALSE) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 T_RDLAT_OFFSET[2:0]         0x%0x\n",  (phy_config.dword2 & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 MMT_PAR_ENABLE              0x%0x\n",  ((phy_config.dword2 >> 3) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 PHY_PAR_LATENCY[1:0]        0x%0x\n",  ((phy_config.dword2 >> 4) & 0x3));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_RD_DISABLE               0x%0x\n",  ((phy_config.dword2 >> 6) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_WR_DISABLE               0x%0x\n",  ((phy_config.dword2 >> 7) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_RD_DELAY[2:0]            0x%0x\n",  ((phy_config.dword2 >> 8) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 QC_WR_DELAY[2:0]            0x%0x\n",  ((phy_config.dword2 >> 11) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 MMT_ECC_EN                  0x%0x\n",  ((phy_config.dword2 >> 14) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 DRV_IMP_CONFIG[3:0]         0x%0x\n",  ((phy_config.dword2 >> 15) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 DIFF_ENHANCE_RDQS           0x%0x\n",  ((phy_config.dword2 >> 19) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 DIFF_ENHANCE_WDQS           0x%0x\n",  ((phy_config.dword2 >> 20) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 BPC_MODE                    0x%0x\n",  ((phy_config.dword2 >> 21) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 BPC_EN                      0x%0x\n",  ((phy_config.dword2 >> 22) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD2 MMT_DBI_EN                  0x%0x\n",  ((phy_config.dword2 >> 23) & 0x1));

      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 T_RDLAT_OFFSET[2:0]         0x%0x\n",  (phy_config.dword3 & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 MMT_PAR_ENABLE              0x%0x\n",  ((phy_config.dword3 >> 3) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 PHY_PAR_LATENCY[1:0]        0x%0x\n",  ((phy_config.dword3 >> 4) & 0x3));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_RD_DISABLE               0x%0x\n",  ((phy_config.dword3 >> 6) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_WR_DISABLE               0x%0x\n",  ((phy_config.dword3 >> 7) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_RD_DELAY[2:0]            0x%0x\n",  ((phy_config.dword3 >> 8) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 QC_WR_DELAY[2:0]            0x%0x\n",  ((phy_config.dword3 >> 11) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 MMT_ECC_EN                  0x%0x\n",  ((phy_config.dword3 >> 14) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 DRV_IMP_CONFIG[3:0]         0x%0x\n",  ((phy_config.dword3 >> 15) & 0x7));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 DIFF_ENHANCE_RDQS           0x%0x\n",  ((phy_config.dword3 >> 19) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 DIFF_ENHANCE_WDQS           0x%0x\n",  ((phy_config.dword3 >> 20) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 BPC_MODE                    0x%0x\n",  ((phy_config.dword3 >> 21) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 BPC_EN                      0x%0x\n",  ((phy_config.dword3 >> 22) & 0x1));
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DWORD3 MMT_DBI_EN                  0x%0x\n",  ((phy_config.dword3 >> 23) & 0x1));
    }
  }


  return 0;
}


/**
 ** @brief  Retrieves the HBM device id for the HBM on the SBus ring
 ** @param  spico_addr sbus_address of the sbus master spico
 ** @return  On error, decrements aapl->return_code and returns -1.
 **
 ** @return  Error code description
 **/
int avago_hbm_read_device_temp(Aapl_t *aapl, uint spico_addr)
{
  Avago_hbm_operation_results_t results;
  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_READ_TEMPERATURE, &results, avago_hbm_default_timeout);
  } else {
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_READ_TEMPERATURE, &results, avago_hbm_default_timeout);
  }
  return avago_spico_int(aapl, spico_addr, 0x32, 0x12);
}

/**
 ** @brief
 **
 ** @param  aapl         Aapl_t struct
 ** @param  apc_addr sbus_address of the APC block
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_print_ctc_results_by_addr(Aapl_t *aapl, uint ctc_addr, uint channel)
{
  uint ctc_option;
  uint total_writes;
  uint total_reads;
  uint error_count;
  const char * ctc_type;

  ctc_option = avago_sbus_rd(aapl, ctc_addr, 0xf);
  total_writes = avago_sbus_rd(aapl, ctc_addr, 0x07);
  total_reads  = avago_sbus_rd(aapl, ctc_addr, 0x08);
  error_count  = avago_sbus_rd(aapl, ctc_addr, 0xa8);

  switch(ctc_option) {
    case 0x0: { ctc_type = "DIV1"; break; }
    case 0x1: { ctc_type = "DIV2"; break; }
    case 0x2: { ctc_type = "DUAL_MUX"; break; }
    case 0x3: { ctc_type = "DP_PC0"; break; }
    case 0x4: { ctc_type = "DP_PC1"; break; }
    default:  { ctc_type = "UNKNOWN"; break; }
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d CTC_Type:%-8s Writes:0x%08x Reads:0x%08x Errors:0x%08x\n",
      channel, ctc_type, total_writes, total_reads, error_count);

  return ctc_option;
}


/**
 ** @brief  Returns true if CTC blocks are includes on the sbus ring
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
BOOL avago_hbm_has_ctc(Aapl_t *aapl, uint spico_addr)
{
  uint sbus;
  uint max_sbus_addr = aapl_get_max_sbus_addr(aapl, spico_addr);
  Avago_addr_t addr_struct;
  avago_addr_to_struct(spico_addr, &addr_struct);

  for( sbus = 1; sbus <= max_sbus_addr; sbus++ )
  {
    uint addr = avago_make_addr3(addr_struct.chip, addr_struct.ring, sbus);
    if( aapl_get_ip_type(aapl, addr) == AVAGO_DDR_CTC ) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 ** @brief  Prints CTC read/write/error results
 **
 ** @param  aapl         Aapl_t struct
 ** @param  apc_addr sbus_address of the APC block
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_print_ctc_results(Aapl_t *aapl, uint apc_addr)
{
  uint channel;


  for (channel = 0; channel <= 7; channel++) {
    uint ctc_addr;
    uint idcode;
    uint ctc_option;
    Avago_addr_t addr_struct;

    avago_hbm_wir_write_channel(aapl, apc_addr, PHY_SEL, channel, PHY_CTC_ID);
    avago_hbm_wdr_read(aapl, apc_addr, PHY_CTC_ID_LENGTH, 0);
    avago_hbm_wdr_set_read_channel(aapl, apc_addr, channel);

    avago_addr_to_struct(apc_addr, &addr_struct);
    ctc_addr = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0) & 0xff;
    ctc_addr = avago_make_addr3(addr_struct.chip, addr_struct.ring, ctc_addr);

    idcode = avago_sbus_rd(aapl, ctc_addr, 0xff);
    if (idcode == HBM_STOP_IDCODE) {
      ctc_addr = ctc_addr - 1;
    }
    ctc_option = avago_hbm_print_ctc_results_by_addr(aapl, ctc_addr, channel);

    if (ctc_option == 4) {
      ctc_option = avago_hbm_print_ctc_results_by_addr(aapl, ctc_addr-1, channel);
    }

  }

  return 0;
}

/**
 ** @brief  Runs just the MBIST operation and reports extended dianostic info for any failures
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_mbist_diagnostics(Aapl_t *aapl, uint spico_addr)
{
  int rc;
  Avago_hbm_device_id_t device_id;
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  Avago_addr_t addr_struct;
  uint fw_build_id;
  uint fw_revision;
  int patterns[3] = {0,1,2};
  int i;
  BOOL hbm2e;

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "AAPL_version: %s, compiled %s %s\n", AAPL_VERSION, __DATE__, __TIME__);

  rc = aapl->return_code;
  spico_addr = avago_hbm_get_spico_address(aapl, spico_addr);
  if (aapl->return_code != rc) { return -1; }

  if (avago_hbm_fw_check(aapl, spico_addr) != 0) {
    return -1;
  }

  avago_addr_to_struct(spico_addr, &addr_struct);
  fw_revision =  avago_firmware_get_rev(aapl, spico_addr);
  fw_build_id = avago_firmware_get_build_id(aapl, spico_addr);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Revision: 0x%04x\n", fw_revision);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Build ID: 0x%04x\n", fw_build_id);

  apc_addr = avago_spico_int(aapl, spico_addr, 0x36, 0);
  apc_addr = avago_make_addr3(addr_struct.chip, addr_struct.ring, apc_addr);

  if (!aapl_check_ip_type(aapl, apc_addr, __func__, __LINE__, FALSE, 1, AVAGO_APC)) {
    aapl_fail(aapl, __func__, __LINE__, "Firmware returned an invalid APC address: 0x%0x\n", apc_addr);
    return -1;
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test Reset: %s\n", rc == 0 ? "PASS" : "FAIL");

  rc = avago_hbm_read_device_id(aapl, spico_addr, &device_id);
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  } else {
    avago_hbm_print_device_id(aapl, spico_addr);
  }

  hbm2e = avago_hbm2e_check(aapl, spico_addr);
  if ( !hbm2e && (device_id.manufacturer_id == SAMSUNG) ) {
    patterns[2] = -1;
  }

  for (i=0; i<=2; i++){
    int pattern;

    pattern = patterns[i];
    if (pattern == -1) { break; }

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Running MBIST Pattern%d - %s\n", pattern, avago_hbm_mbist_pattern_to_str(device_id.manufacturer_id, pattern));

    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_MBIST_PATTERN, pattern);
    rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_MBIST : AVAGO_HBM_OP_MBIST, &results, 0);

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Status: %s\n", rc == 0 ? "PASS" : "FAIL");

    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_mbist_results(aapl, spico_addr, device_id.manufacturer_id);
    }
  }



  return aapl->return_code;
}


/**
 ** @brief  Start the CTC running the requested pattern type
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the sbus master spico or APC
 ** @param  pattern_type The mmt pattern to run
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_ctc_start(Aapl_t *aapl, uint spico_addr, int pattern_type, int init)
{
  int rc;
  Avago_hbm_device_id_t device_id;
  Avago_hbm_operation_results_t results;
  uint apc_addr;

  memset(&device_id, 0, sizeof(device_id));

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  if (init == 1) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
    }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test Reset: %s\n", rc == 0 ? "PASS" : "FAIL");
  }

  rc = avago_hbm_read_device_id(aapl, spico_addr, &device_id);
    avago_hbm_print_device_id(aapl, spico_addr);

  if (init == 1) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
    }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test RESET: %s\n", rc == 0 ? "PASS" : "FAIL");

    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_INITIALIZE_NWL_MCS, &results, avago_hbm_default_timeout);
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
    }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test INIT NWL: %s\n", rc == 0 ? "PASS" : "FAIL");
  }

  avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_CTC_PATTERN_TYPE, pattern_type);

  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_START_CTC, &results, avago_hbm_default_timeout);
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  }
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CTC START: %s\n", rc == 0 ? "PASS" : "FAIL");


  return 0;
}


/**
 ** @brief  Stop the CTC running
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_ctc_stop(Aapl_t *aapl, uint spico_addr)
{
  int rc;
  Avago_hbm_operation_results_t results;
  uint apc_addr;


  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_STOP_CTC, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CTC: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_print_ctc_results(aapl, apc_addr);
  }


  return 0;
}


/**
 ** @brief  Prints all firmware parameters
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_parameter_diagnostics(Aapl_t *aapl, uint spico_addr)
{
  int i;
  int param_count;

  if (avago_hbm2e_check(aapl, spico_addr)) {
    param_count = 0x89;
  } else {
    param_count = 0x7b;
  }

  for(i=0; i<=param_count; i++) {
    uint value;
    value = avago_hbm_get_parameter(aapl, spico_addr, (Avago_hbm_parameter_t)i);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "offset:0x%02x value:0x%x\n", i, value);
  }
  return 0;
}

/**
 ** @brief  Runs all firmware operations and report extended diagnostic info for any failures
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_diagnostics(Aapl_t *aapl, uint spico_addr)
{
  uint hbm_temp;
  int rc;
  Avago_hbm_device_id_t device_id;
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  uint fw_build_id;
  uint fw_revision;
  Avago_addr_t addr_struct;
  BOOL hbm2e;
  uint save_soft_lane_repairs;


  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "AAPL_version: %s, compiled %s %s\n", AAPL_VERSION, __DATE__, __TIME__);

  if (avago_hbm_fw_check(aapl, spico_addr) != 0) {
    return -1;
  }

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  avago_addr_to_struct(spico_addr, &addr_struct);


  hbm2e = avago_hbm2e_check(aapl, spico_addr);

  fw_revision =  avago_firmware_get_rev(aapl, spico_addr);
  fw_build_id = avago_firmware_get_build_id(aapl, spico_addr);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Revision: 0x%04x\n", fw_revision);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Build ID: 0x%04x\n", fw_build_id);


  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test Reset: %s\n", rc == 0 ? "PASS" : "FAIL");

  rc = avago_hbm_read_device_id(aapl, spico_addr, &device_id);
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  } else {
    avago_hbm_print_device_id(aapl, spico_addr);
  }


  hbm_temp = avago_hbm_read_device_temp(aapl, spico_addr);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "HBM Temperature: %d\n", hbm_temp);


  avago_hbm_print_hard_lane_repairs(aapl, spico_addr);


  save_soft_lane_repairs = avago_hbm_get_parameter(aapl, spico_addr, hbm2e? AVAGO_HBM2E_SAVE_SOFT_LANE_REPAIRS : AVAGO_HBM_SAVE_SOFT_LANE_REPAIRS);

  if (save_soft_lane_repairs == 1) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Skipping lane repair because hbm_save_soft_lane_repairs == 1\n");

  } else {
    uint count;
    uint repair_count;

    rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_LANE_REPAIR : AVAGO_HBM_OP_LANE_REPAIR, &results, avago_hbm_default_timeout);
    repair_count  = avago_spico_int(aapl, spico_addr, 0x32, 0x12);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Total Lane Faults: %d\n", repair_count);

    if (repair_count > 24) { repair_count = 24; }
    for (count = 1; count <= repair_count; count++) {
      uint repaired_lane;
      repaired_lane  = avago_spico_int(aapl, spico_addr, 0x32, 0x12+count);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Lane Number: %d\n", repaired_lane);
    }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test LANE REPAIR: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
    }

    avago_hbm_print_hard_lane_repairs(aapl, spico_addr);
  }

  if (device_id.manufacturer_id == 1) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_TEST_CHIPPING : AVAGO_HBM_OP_SAMSUNG_CHIPPING_TEST, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test SAMSUNG CHIPPING: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
    }
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_AWORD_ILB : AVAGO_HBM_OP_AWORD_ILB, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AWORD ILB: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  if (hbm2e) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_BIST_DWORD_LOWER_ILB, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD LOWER ILB: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "DWORD_UPPER");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }

    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_BIST_DWORD_UPPER_ILB, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD UPPER ILB: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "DWORD_UPPER");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_DWORD_ILB, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD ILB: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "DWORD_LOWER");
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "DWORD_UPPER");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_AERR_ILB : AVAGO_HBM_OP_AERR_ILB, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AERR ILB: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "AERR_0");
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "AERR_1");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_DERR_ILB : AVAGO_HBM_OP_DERR_ILB, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DERR ILB: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_AWORD_SLB : AVAGO_HBM_OP_AWORD_SLB, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AWORD SLB: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  if (hbm2e) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_BIST_DWORD_LOWER_SLB, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD LOWER SLB: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "DWORD_UPPER");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }

    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_BIST_DWORD_UPPER_SLB, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD UPPER SLB: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "DWORD_UPPER");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_DWORD_SLB, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD SLB: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "DWORD_LOWER");
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "DWORD_UPPER");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_AERR_SLB : AVAGO_HBM_OP_AERR_SLB, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AERR SLB: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "AERR_0");
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "AERR_1");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_DERR_SLB : AVAGO_HBM_OP_DERR_SLB, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DERR SLB: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }



  if (device_id.manufacturer_id == 1 && !hbm2e) {
    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_AWORD_TEST, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AWORD: %s\n", rc == 0 ? "PASS" : "WARN");
    AAPL_SUPPRESS_ERRORS_POP(aapl);
  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_AWORD : AVAGO_HBM_OP_AWORD_TEST, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AWORD: %s\n", rc == 0 ? "PASS" : "FAIL");
  }
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, HBM_SEL, 0, "HBM");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_AERR : AVAGO_HBM_OP_AERR_TEST, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test AERR: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "AERR_0");
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 1, "AERR_1");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  if (hbm2e) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_BIST_DWORD_READ, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD READ: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, HBM_SEL, 1, "HBM");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }

    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_BIST_DWORD_WRITE, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD WRITE: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, HBM_SEL, 1, "HBM");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }

  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_DWORD_TEST, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DWORD: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
      avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, HBM_SEL, 1, "HBM");
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  }

  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_DERR : AVAGO_HBM_OP_DERR_TEST, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test DERR: %s\n", rc ==  0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_read_lfsr_compare_sticky(aapl, spico_addr, PHY_SEL, 0, "PHY");
    avago_hbm_print_spare_results(aapl, spico_addr);
  }

  if (fw_build_id == 0x2002 && fw_revision < 0x550) {
  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_TEST_CATTRIP : AVAGO_HBM_OP_CATTRIP, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CATTRIP: %s\n", rc ==  0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
    }
  }

  return aapl->return_code;
}


/**
 ** @brief  Runs ctc diagnostics
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_ctc_diagnostics(Aapl_t *aapl, uint spico_addr, int do_reset, int do_init_nwl)
{
  int rc;
  Avago_hbm_device_id_t device_id;
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  uint fw_build_id;
  uint fw_revision;
  Avago_addr_t addr_struct;

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "AAPL_version: %s, compiled %s %s\n", AAPL_VERSION, __DATE__, __TIME__);

  if (avago_hbm_fw_check(aapl, spico_addr) != 0) {
    return -1;
  }

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  if (aapl_get_process_id(aapl, apc_addr) != AVAGO_PROCESS_B) {
    aapl_log_printf(aapl, AVAGO_ERR, 0, 1, "CTC is only supported in 16nm\n");
    return -1;
  }


  avago_addr_to_struct(spico_addr, &addr_struct);

  fw_revision =  avago_firmware_get_rev(aapl, spico_addr);
  fw_build_id = avago_firmware_get_build_id(aapl, spico_addr);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Revision: 0x%04x\n", fw_revision);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Build ID: 0x%04x\n", fw_build_id);


  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test Reset: %s\n", rc == 0 ? "PASS" : "FAIL");

  rc = avago_hbm_read_device_id(aapl, spico_addr, &device_id);
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  } else {
    avago_hbm_print_device_id(aapl, spico_addr);
  }

  if (do_reset == 1) {
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  }

  if (do_init_nwl == 1) {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_INITIALIZE_NWL_MCS, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test INIT NWL: %s\n", rc == 0 ? "PASS" : "FAIL");
  }

  avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_CTC_PATTERN_TYPE, 0);
  avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_START_CTC, &results, avago_hbm_default_timeout);
  ms_sleep(500);
  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_STOP_CTC, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CTC PRBS: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_print_ctc_results(aapl, apc_addr);
  }

  avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_CTC_PATTERN_TYPE, 1);
  avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_START_CTC, &results, avago_hbm_default_timeout);
  ms_sleep(500);
  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_STOP_CTC, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CTC HIGH POWER: %s\n", rc == 0 ? "PASS" : "FAIL");
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
    avago_hbm_print_ctc_results(aapl, apc_addr);
  }

  if (fw_revision >= 0x55a) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_CTC_PATTERN_TYPE, 2);
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_START_CTC, &results, avago_hbm_default_timeout);
    ms_sleep(500);
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_STOP_CTC, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CTC HIGH POWER DBI: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_print_ctc_results(aapl, apc_addr);
    }

    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_CTC_PATTERN_TYPE, 3);
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_START_CTC, &results, avago_hbm_default_timeout);
    ms_sleep(500);
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_STOP_CTC, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test CTC HIGH POWER PRBS: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_print_ctc_results(aapl, apc_addr);
    }
  }

  avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RELEASE_CTC_CONTROL, &results, avago_hbm_default_timeout);

  return aapl->return_code;
}


/**
 ** @brief  Start the MMT running the requested pattern
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 ** @param  pattern_type The mmt pattern to run
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_mmt_start(Aapl_t *aapl, uint spico_addr, int pattern_type)
{
  int rc;
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  BOOL hbm2e;

  hbm2e = avago_hbm2e_check(aapl, spico_addr);

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);


  avago_hbm_set_parameter(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_MMT_CONFIGURATION : AVAGO_HBM_MMT_CONFIGURATION, pattern_type);

  if (hbm2e) {
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_MMT_CONFIGURE, &results, avago_hbm_default_timeout);
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_MMT_CUSTOM, &results, avago_hbm_default_timeout);
  } else {
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_START_MMT, &results, avago_hbm_default_timeout);
  }
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  }

  return aapl->return_code;
}


/**
 ** @brief  Stop the MMT running
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_mmt_stop(Aapl_t *aapl, uint spico_addr)
{
  int rc;
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  BOOL hbm2e;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  hbm2e = avago_hbm2e_check(aapl, spico_addr);
  rc = avago_hbm_launch_operation(aapl, spico_addr, hbm2e ? AVAGO_HBM2E_OP_BIST_STOP_AND_CHECK : AVAGO_HBM_OP_STOP_MMT, &results, avago_hbm_default_timeout);

  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  }

  return rc;
}

/**
 ** @brief  Iterate on running MMT
 **
 ** @param  aapl       Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 ** @param  count      The number of MMT loops to run.  0 runs continuously.
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_mmt_loop(Aapl_t *aapl, uint spico_addr, int count, int pattern_type)
{
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  uint fail_count = 0;
  int i;
  int rc;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);

  if (count <= 0) { count = -1; }

  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MMT_CONFIGURATION, pattern_type);
  } else {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_MMT_CONFIGURATION, pattern_type);
  }

  i=0;
  while (i != count) {
    i++;
    if (avago_hbm2e_check(aapl, spico_addr)) {
      rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_MMT, &results, avago_hbm_default_timeout);
    } else {
      rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RUN_MMT, &results, avago_hbm_default_timeout);
    }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "MMT loop:%d status:%s\n", i, rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      fail_count++;
      avago_hbm_print_operation_results(aapl, &results);
    }
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "MMT total_loops:%d total_fails:%d\n", count, fail_count);
  return aapl->return_code;
}



/**
 ** @brief Runs a MMT test with a hardcoded DLL PVT code
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the APC
 ** @param  pvt          PVT value (0-63)
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_mmt_run_with_pvt(Aapl_t * aapl, uint spico_addr, uint pvt)
{
  uint apc_addr;
  int rc;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  avago_sbus_wr(aapl, apc_addr, 0x36, ((pvt<<1) | 1));

  avago_sbus_wr(aapl, apc_addr, 0x38, ((pvt<<1) | 1));


  AAPL_SUPPRESS_ERRORS_PUSH(aapl);
  rc = avago_hbm_launch_operation_nowait(aapl, spico_addr, AVAGO_HBM_OP_RESET, avago_hbm_default_timeout);
  if (rc==0) { rc = avago_hbm_check_operation_brief(aapl, spico_addr, avago_hbm_default_timeout); }
  if (rc==0) { rc = avago_hbm_launch_operation_nowait(aapl, spico_addr, AVAGO_HBM_OP_RUN_MMT, avago_hbm_default_timeout); }
  if (rc==0) { rc = avago_hbm_check_operation_brief(aapl, spico_addr, avago_hbm_default_timeout); }

  avago_sbus_wr(aapl, apc_addr, 0x36, 0);
  avago_sbus_wr(aapl, apc_addr, 0x38, 0);

  AAPL_SUPPRESS_ERRORS_POP(aapl);

  if (rc != 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
 ** @brief  Runs mmt only if it has not already been run for this pvt condition
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_mmt_eye_run(Aapl_t *aapl, uint spico_addr, int *pvt_results, int pvt )
{
  if (pvt_results[pvt] == -1) {
    pvt_results[pvt] = avago_hbm_mmt_run_with_pvt(aapl, spico_addr, pvt);
  }
  return pvt_results[pvt];
}

/**
 ** @brief  Use MMT to find the left and right edges of the mmt data eye
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_hbm_mmt_eye_edge_bsearch(Aapl_t *aapl, uint spico_addr, int *left_pvt, int *right_pvt)
{
  int pvt_results[64];
  int min;
  int max;
  int i;

  for (i = 0; i <= 63 ; i++){
    pvt_results[i] = -1;
  }

  min = 0;
  max = 63;

  if (avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, min) == avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, max)) {
    if ( avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, min) == 0){
      *left_pvt = -1;
      *right_pvt = 64;
      return 0;
    } else {
      *left_pvt = -1;
      *right_pvt = -1;
      return 0;
    }
  }

  while ((max-min) != 1) {
    int half = (int)((min+max)/2);

    if (avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, min) != avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, half)) {
      max = half;
    } else if (avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, half) != avago_hbm_mmt_eye_run(aapl, spico_addr, pvt_results, max)) {
      min = half;
    }
  }

  if ((pvt_results[min] == 0) && (pvt_results[max] == 1)) {
    *left_pvt = -1;
    *right_pvt = min;
  } else {
    *left_pvt = max;
    *right_pvt = 64;
  }

  return 0;
}

/**
 ** @brief  Use MMT to measure the setup/hold margin on the HBM interface
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_mmt_margin(Aapl_t *aapl, uint spico_addr, Avago_hbm_eye_t *hbm_eye)
{
  int rc;
  uint init_hbm_phy_config0;
  uint init_hbm_phy_config3;
  uint apc_addr;
  float right_delay;
  float left_delay;
  float center_delay;
  int left_pvt;
  int right_pvt;
  int qc_delay;
  Avago_hbm_operation_results_t results;

  memset(hbm_eye, 0, sizeof(*hbm_eye));

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  avago_sbus_wr(aapl, apc_addr, 0x36, 0);
  avago_sbus_wr(aapl, apc_addr, 0x38, 0);
  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  if (rc != 0) { avago_hbm_print_operation_results(aapl, &results); }

  init_hbm_phy_config0          = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG0);
  init_hbm_phy_config3          = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_PHY_CONFIG3);
  hbm_eye->init_cfg_dll         = avago_sbus_rd(aapl, apc_addr, 0x30);
  hbm_eye->init_dll_clk         = hbm_eye->init_cfg_dll & 0x3;
  hbm_eye->init_dll_stb         = (hbm_eye->init_cfg_dll >> 2) & 0x3;
  hbm_eye->init_obs_pvt         = avago_sbus_rd(aapl, apc_addr, 0x4f);
  hbm_eye->init_pvt_clk         = hbm_eye->init_obs_pvt & 0x3f;
  hbm_eye->init_pvt_stb         = (hbm_eye->init_obs_pvt >> 6) & 0x3f;
  hbm_eye->init_qc_rd_delay     = (init_hbm_phy_config0 >> 8) & 0x7;
  hbm_eye->init_qc_wr_delay     = (init_hbm_phy_config0 >> 11) & 0x7;
  hbm_eye->init_qc_ck_delay     = (init_hbm_phy_config3 >> 1) & 0x7;
  hbm_eye->freq                 = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_FREQ);

  for (qc_delay=0; qc_delay<=7; qc_delay++) {
    avago_hbm_set_phy_quarter_cycle(aapl, spico_addr, qc_delay, qc_delay, qc_delay);
    avago_hbm_mmt_eye_edge_bsearch(aapl, spico_addr, &left_pvt, &right_pvt);
    avago_hbm_set_phy_quarter_cycle(aapl, spico_addr, hbm_eye->init_qc_rd_delay, hbm_eye->init_qc_wr_delay, hbm_eye->init_qc_ck_delay);

    hbm_eye->left_qc_pvts[qc_delay] = left_pvt;
    hbm_eye->right_qc_pvts[qc_delay] = right_pvt;
  }


  left_delay = 0;
  right_delay = 0;

  for (qc_delay=0; qc_delay<=7; qc_delay++) {
    left_pvt  = hbm_eye->left_qc_pvts[qc_delay];
    right_pvt = hbm_eye->right_qc_pvts[qc_delay];

    if (left_pvt == right_pvt) {
    } else if ((left_pvt == -1) && (right_pvt == 64)) {
    } else if (left_pvt == -1) {
      if (right_delay == 0) { right_delay = (qc_delay+2) * ((1/(hbm_eye->freq*1e6*(10+2*hbm_eye->init_dll_clk))) + (0.53e-12*right_pvt)) * 1e12; }
    } else {
      left_delay = (qc_delay+2) * ((1/(hbm_eye->freq*1e6*(10+2*hbm_eye->init_dll_clk))) + (0.53e-12*left_pvt)) * 1e12;
    }
  }

  if (right_delay == 0) { right_delay = (7 + 2) * ((1/(hbm_eye->freq*1e6*(10+2*hbm_eye->init_dll_clk))) + (0.53e-12*64)) * 1e12; }
  center_delay = (hbm_eye->init_qc_rd_delay + 2) * ((1/(hbm_eye->freq*1e6*(10+2*hbm_eye->init_dll_clk))) + (0.53e-12*hbm_eye->init_pvt_clk)) * 1e12;

  hbm_eye->setup_margin = center_delay-left_delay;
  hbm_eye->hold_margin = right_delay-center_delay;

  return 0;
}


/**
 ** @brief  Prints MMT eye results
 **
 ** @param  aapl         Aapl_t struct
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_print_mmt_eye(Aapl_t *aapl, Avago_hbm_eye_t *hbm_eye)
{
  char eye[65] = { 0 };
  char qc_id;
  char pvt_id[66] = { 0 };
  int qc_delay;
  int i;

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "\n");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "DLL_CONFIG_CLK : %d\n", hbm_eye->init_dll_clk );
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "DLL_CONFIG_STB : %d\n", hbm_eye->init_dll_stb);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "QC_RD_DELAY    : %d\n", hbm_eye->init_qc_rd_delay);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "QC_WR_DELAY    : %d\n", hbm_eye->init_qc_wr_delay);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "QC_CK_DELAY    : %d\n", hbm_eye->init_qc_ck_delay);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "DLL_PVT_CLK    : %d\n", hbm_eye->init_pvt_clk);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "DLL_PVT_STB    : %d\n", hbm_eye->init_pvt_stb);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "\n");

  memset(pvt_id, ' ', sizeof(pvt_id));
  pvt_id[hbm_eye->init_pvt_clk] = '^';
  pvt_id[65] = 0;

  for (qc_delay=0; qc_delay<=7; qc_delay++) {
    int left_pvt  = hbm_eye->left_qc_pvts[qc_delay];
    int right_pvt = hbm_eye->right_qc_pvts[qc_delay];

    if ((unsigned int)qc_delay == hbm_eye->init_qc_rd_delay) {
      qc_id = '<';
    } else {
      qc_id = ' ';
    }

    if (left_pvt == right_pvt) {
      for (i=0; i<=63; i++) { eye[i] = 'x'; }
    } else if ((left_pvt == -1) && (right_pvt == 64)) {
      for (i=0; i<=63; i++) { eye[i] = '-'; }
    } else if (left_pvt == -1) {
      for (i=0; i<=63; i++) { eye[i] = (i < right_pvt) ? '-' : 'x'; }
    } else {
      for (i=0; i<=63; i++) { eye[i] = (i < left_pvt) ? 'x' : '-'; }
    }

    if (hbm_eye->init_qc_rd_delay == (unsigned int)qc_delay) { eye[hbm_eye->init_pvt_clk] = 'O'; }
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "QC%d : %64s %c\n", qc_delay, eye, qc_id);
  }
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "PVT : %64s\n", pvt_id);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "\n");
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Eye margin setup:%0.1fps hold:%0.1fps\n", hbm_eye->setup_margin, hbm_eye->hold_margin);

  return 0;
}

/**
 ** @brief  Prints MMT read/write/error results
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_print_mmt_results(Aapl_t *aapl, uint spico_addr, uint verbose)
{
  Avago_addr_t addr_struct;
  uint apc_addr;
  uint channel;


  avago_addr_to_struct(spico_addr, &addr_struct);
  apc_addr = avago_spico_int(aapl, spico_addr, 0x36, 0);
  apc_addr = avago_make_addr3(addr_struct.chip, addr_struct.ring, apc_addr);

  avago_sbus_wr(aapl, apc_addr, 0x55, 0);
  avago_sbus_wr(aapl, apc_addr, 0x55, 1);
  avago_sbus_wr(aapl, apc_addr, 0x55, 0);

  for (channel = 0; channel <= 7; channel++) {
    uint mmt_config_31_0;
    uint mmt_config_63_32;
    uint mmt_config_95_64;
    uint mmt_config_127_96;
    uint mmt_config_159_128;
    uint mmt_config_191_160;
    uint mmt_config_223_192;
    uint mmt_config_255_224;
    uint mmt_config_287_256;
    uint mmt_config_319_298;
    uint mmt_config_351_320;
    uint mmt_config_383_352;
    uint mmt_config_415_384;
    uint mmt_config_447_416;
    uint mmt_config_479_448;
    uint mmt_config_511_480;

    uint mmt_col_address;
    uint mmt_row_address;
    uint mmt_bank_address;
    uint mmt_stack_id;
    uint mmt_state;
    uint mmt_col_state;
    uint mmt_row_state;

    avago_hbm_wir_write_channel(aapl, apc_addr, PHY_SEL, channel, PHY_MMT_CONFIG);
    avago_hbm_wdr_read(aapl, apc_addr, 320, 0);
    avago_hbm_wdr_set_read_channel(aapl, apc_addr, channel);

    mmt_config_31_0    = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0);
    mmt_config_63_32   = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 1);
    mmt_config_95_64   = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 2);
    mmt_config_127_96  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 3);
    mmt_config_159_128 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 4);
    mmt_config_191_160 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 5);
    mmt_config_223_192 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 6);
    mmt_config_255_224 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 7);
    mmt_config_287_256 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 8);
    mmt_config_319_298 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 9);


    if (avago_hbm2e_check(aapl, apc_addr)) {
      avago_hbm_wdr_read(aapl, apc_addr, 200, 1);
      mmt_config_351_320  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0);
      mmt_config_383_352  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 1);
      mmt_config_415_384  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 2);
      mmt_config_447_416  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 3);
      mmt_config_479_448  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 4);
      mmt_config_511_480  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 5) & 0xfffff;

      mmt_col_address  = (mmt_config_223_192 >> 12)  & 0x1f;
      mmt_row_address  = ((mmt_config_223_192 >> 16) & 0x07);
      mmt_bank_address = ((mmt_config_255_224 >> 0)  & 0x0f);
      mmt_stack_id     = ((mmt_config_255_224 >> 4)  & 0x01);
      mmt_state        = ((mmt_config_255_224 >> 6)  & 0x1f);
      mmt_col_state    = ((mmt_config_255_224 >> 11) & 0x0f);
      mmt_row_state    = ((mmt_config_255_224 >> 15) & 0x0f);

      mmt_config_223_192 = mmt_config_223_192 & 0xfff;
      mmt_config_255_224 = mmt_config_255_224 & 0xfff80000;

    } else {
      avago_hbm_wdr_read(aapl, apc_addr, 120, 1);
      mmt_config_351_320  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0);
      mmt_config_383_352  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 1);
      mmt_config_415_384  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 2);
      mmt_config_447_416  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 3) & 0xffffff;
      mmt_config_479_448 = 0;
      mmt_config_511_480 = 0;

      mmt_col_address  = (mmt_config_191_160 >> 24) & 0x1f;
      mmt_row_address  = ((mmt_config_191_160 >> 29) & 0x07) | ((mmt_config_223_192 & 0x1fff) << 3);
      mmt_bank_address = ((mmt_config_223_192 >> 13) & 0x0f);
      mmt_stack_id     = ((mmt_config_223_192 >> 17) & 0x01);
      mmt_state        = ((mmt_config_223_192 >> 18) & 0x1f);
      mmt_col_state    = ((mmt_config_223_192 >> 23) & 0x0f);
      mmt_row_state    = ((mmt_config_223_192 >> 27) & 0x0f);

      mmt_config_191_160 = mmt_config_191_160 & 0x00ffffff;
      mmt_config_223_192 = mmt_config_223_192 & 0x80000000;
    }



    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_CONFIG: 0x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x\n",
        channel,
        mmt_config_511_480,
        mmt_config_479_448,
        mmt_config_447_416,
        mmt_config_415_384,
        mmt_config_383_352,
        mmt_config_351_320,
        mmt_config_319_298,
        mmt_config_287_256,
        mmt_config_255_224,
        mmt_config_223_192,
        mmt_config_191_160,
        mmt_config_159_128,
        mmt_config_127_96,
        mmt_config_95_64,
        mmt_config_63_32,
        mmt_config_31_0
        );

    if (verbose == 1) {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_COL_ADDRESS : 0x%0x\n", channel, mmt_col_address);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_ROW_ADDRESS : 0x%0x\n", channel, mmt_row_address);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_BANK_ADDRESS: 0x%0x\n", channel, mmt_bank_address);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_STACK_ID    : 0x%0x\n", channel, mmt_stack_id);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_STATE       : 0x%0x\n", channel, mmt_state);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_COL_STATE   : 0x%0x\n", channel, mmt_col_state);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d MMT_ROW_STATE   : 0x%0x\n", channel, mmt_row_state);
    }

  }

  return 0;
}


/**
 ** @brief  Runs MMT diagnostics
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_mmt_diagnostics(Aapl_t *aapl, uint spico_addr)
{
  int rc;
  Avago_hbm_device_id_t device_id;
  Avago_hbm_operation_results_t results;
  uint apc_addr;
  uint fw_build_id;
  uint fw_revision;
  Avago_addr_t addr_struct;
  uint init_pattern;
  BOOL hbm2e;
  int patterns[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int i;

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "AAPL_version: %s, compiled %s %s\n", AAPL_VERSION, __DATE__, __TIME__);

  if (avago_hbm_fw_check(aapl, spico_addr) != 0) {
    return -1;
  }

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  avago_addr_to_struct(spico_addr, &addr_struct);

  hbm2e = avago_hbm2e_check(aapl, spico_addr);

  fw_revision =  avago_firmware_get_rev(aapl, spico_addr);
  fw_build_id = avago_firmware_get_build_id(aapl, spico_addr);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Revision: 0x%04x\n", fw_revision);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Firmware Build ID: 0x%04x\n", fw_build_id);

  rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test Reset: %s\n", rc == 0 ? "PASS" : "FAIL");

  rc = avago_hbm_read_device_id(aapl, spico_addr, &device_id);
  if (rc != 0) {
    avago_hbm_print_operation_results(aapl, &results);
  } else {
    avago_hbm_print_device_id(aapl, spico_addr);
  }

  init_pattern = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM_MMT_CONFIGURATION);

  if (!hbm2e) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_MMT_CONFIGURATION, 2);
    rc = avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RUN_MMT, &results, avago_hbm_default_timeout);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Test MMT ALL: %s\n", rc == 0 ? "PASS" : "FAIL");
    if (rc != 0) {
      avago_hbm_print_operation_results(aapl, &results);
      avago_hbm_print_mmt_results(aapl, spico_addr, 0);
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  }

  if (hbm2e) {
    patterns[0] = 2;
    patterns[1] = 3;
    patterns[2] = 5;
    patterns[3] = 6;
    patterns[4] = 8;
    patterns[5] = 11;
    patterns[6] = 102;
    patterns[7] = 103;
    patterns[8] = 104;
    patterns[9] = 105;
  } else {
    patterns[0] = 2;
    patterns[1] = 3;
    patterns[2] = 4;
  }

  for (i=0; i<=15; i++){
    uint hbm_temp;
    int pattern;

    pattern = patterns[i];
    if (pattern == -1) { break; }


    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Running MMT Pattern%d - %s\n", pattern, avago_hbm_mmt_pattern_to_str(pattern));
    avago_hbm_mmt_start(aapl, spico_addr, pattern);

    ms_sleep(avago_hbm_dwell_time);

    hbm_temp = avago_hbm_read_device_temp(aapl, spico_addr);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   HBM Temperature: %d\n", hbm_temp);

    rc = avago_hbm_mmt_stop(aapl, spico_addr);

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Status: %s\n", rc == 0 ? "PASS" : "FAIL");

    if (rc != 0) {
      avago_hbm_print_mmt_results(aapl, spico_addr, 0);
      avago_hbm_print_spare_results(aapl, spico_addr);
    }
  }

  avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_MMT_CONFIGURATION, init_pattern);

  return 0;
}



/**
 ** @brief  Print the hard repairs
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico or apc
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_print_hard_lane_repairs(Aapl_t *aapl, uint spico_addr)
{
  Avago_hbm_operation_results_t results;

  avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RESET, &results, avago_hbm_default_timeout);
  avago_hbm_print_lane_repairs(aapl, spico_addr);
  return 0;
}

int avago_hbm_print_lane_repairs(
    Aapl_t *aapl,       /**< [in] Aapl_t struct */
    uint spico_addr)    /**< [in] SBus address of the spico or apc */
{
  uint channel;
  uint apc_addr;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Lane Repair Codes\n");

  for( channel = 0;  channel <= 7; channel++ ) {
    uint repairs_31_0;
    uint repairs_63_32;
    uint repairs_95_64;

    avago_hbm_wir_write_channel(aapl, apc_addr, PHY_SEL, channel, PHY_SOFT_LANE_REPAIR);
    avago_hbm_wdr_read(aapl, apc_addr, PHY_SOFT_LANE_REPAIR_LENGTH, 0);
    avago_hbm_wdr_set_read_channel(aapl, apc_addr, channel);
    repairs_31_0  = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0);
    repairs_63_32 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 1);
    repairs_95_64 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 2) & 0xff;
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   Channel:%d Repairs:0x%02x%08x%08x\n",
        channel, repairs_95_64, repairs_63_32, repairs_31_0);
  }

  return 0;
}


/**
 ** @brief  Run hbm temperature diagnostics
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 ** @param  count        Times to loop.
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_temp_diagnostics(Aapl_t *aapl, uint spico_addr, int count)
{
  uint apc_addr;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  do {
    uint temp_1500;
    uint temp_valid;
    uint temp_value;
    uint temp_cattrip;
    uint temp_dfi;
    uint cattrip;

    avago_hbm_wir_write_channel(aapl, apc_addr, HBM_SEL, 0, HBM_TEMP);
    avago_hbm_wdr_read(aapl, apc_addr, HBM_TEMP_LENGTH, 0);
    avago_hbm_wdr_set_read_channel(aapl, apc_addr, 0);
    temp_1500 = avago_hbm_wdr_read_sbus_data(aapl, apc_addr, 0) & 0xff;
    temp_valid = temp_1500 >> 0x7;
    temp_value = temp_1500 & 0x7f;

    temp_cattrip = avago_sbus_rd(aapl, apc_addr, 0x2c);
    temp_dfi = temp_cattrip & 0x7;
    cattrip = temp_cattrip >> 3;

    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "temp_valid:%d temp_value:%d temp_dfi:%d cattrip:%d\n",
        temp_valid, temp_value, temp_dfi, cattrip);

    if (count > 1) { count = count - 1; }
  } while (count > 1);

  return 0;
}


/**
 ** @brief  Map a qc delay code to the number of delay elements
 **
 ** @param  qc_delay     QC delay value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static uint avago_hbm_qc_to_delay_elements(uint qc_delay)
{
  int mapping[8] = {2,3,4,5,8,1,2,0};
  return mapping[qc_delay];
}

/**
 ** @brief  Calculate the total delay of the qc circuit
 **
 ** @param  spico_addr   Sbus address of the spico
 ** @param  freq         Frequency in Mhz
 ** @param  dll          DLL setting
 ** @param  qc_delay     QC setting
 ** @param  pvt          Current PVT code including offset
 ** @param  default_pvt  Default PVT setting
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static uint avago_hbm_calc_strobe_delay(Aapl_t *aapl, uint spico_addr, uint freq, uint dll, uint qc_delay, uint pvt, uint default_pvt)
{
  int delay_elements;
  bigint delay;
  uint fine_step;

  delay_elements = avago_hbm_qc_to_delay_elements(qc_delay);

  if ((aapl_get_process_id(aapl, spico_addr) == AVAGO_PROCESS_E)) {
    fine_step=530;
  } else {
    fine_step=660;
  }

  if (default_pvt > pvt) {
    delay = (bigint)delay_elements * ( (1e9/(2*(2.0+2.0*(bigint)dll)*freq)) - ((fine_step)*(bigint)(default_pvt - pvt)) );
  } else {
    delay = (bigint)delay_elements * ( (1e9/(2*(2.0+2.0*(bigint)dll)*freq)) + ((fine_step)*(bigint)(pvt - default_pvt)) );
  }

  return (uint)delay;
}

/**
 ** @brief  Run hbm qc diagnostics
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_qc_diagnostics(Aapl_t *aapl, uint spico_addr)
{
  Avago_hbm_phy_config_t phy_config;
  uint rd_delay;
  uint wr_delay;
  uint rd_offset;
  uint wr_offset;
  uint ck_delay;
  uint ck_offset;
  uint pvt;
  uint dll;
  uint freq;
  int adjust_rd_pvt;
  int adjust_wr_pvt;
  int adjust_ck_pvt;
  uint apc_addr;
  uint rd_delay_ps;
  uint wr_delay_ps;
  uint ck_delay_ps;

  if (!avago_hbm2e_check(aapl, spico_addr)) {
    aapl_log_printf(aapl, AVAGO_ERR, 0, 1, "QC diagnotics is only supported for HBM2e\n");
    return -1;
  }

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  avago_hbm_read_phy_config(aapl, spico_addr, &phy_config, 0);



  rd_delay  = ((phy_config.dword0 >> 6) & 0x7);
  wr_delay  = ((phy_config.dword0 >> 9) & 0x7);
  rd_offset = ((phy_config.dword0 >> 12) & 0x7f);
  wr_offset = ((phy_config.dword0 >> 19) & 0x7f);
  ck_delay  = ((phy_config.aword >> 4) & 0x7);
  ck_offset = ((phy_config.aword >> 7) & 0x7f);

  pvt = avago_sbus_rd(aapl, apc_addr, 0x44) >> 6;
  dll = (avago_sbus_rd(aapl, apc_addr, 0x42) >> 2) & 0x3;
  freq = avago_hbm_get_parameter(aapl, spico_addr, AVAGO_HBM2E_CLK_F1_FREQ);

  adjust_rd_pvt = rd_offset & 0x3f;
  if ((rd_offset & 0x40) != 0) { adjust_rd_pvt = -1 * adjust_rd_pvt; }
  adjust_rd_pvt = pvt + adjust_rd_pvt;
  if (adjust_rd_pvt < 0) { adjust_rd_pvt = 0; }

  adjust_wr_pvt = wr_offset & 0x3f;
  if ((wr_offset & 0x40) != 0) { adjust_wr_pvt = -1 * adjust_wr_pvt; }
  adjust_wr_pvt = pvt + adjust_wr_pvt;
  if (adjust_wr_pvt < 0) { adjust_wr_pvt = 0; }

  adjust_ck_pvt = ck_offset & 0x3f;
  if ((ck_offset & 0x40) != 0) { adjust_ck_pvt = -1 * adjust_ck_pvt; }
  adjust_ck_pvt = pvt + adjust_ck_pvt;
  if (adjust_ck_pvt < 0) { adjust_ck_pvt = 0; }

  rd_delay_ps = avago_hbm_calc_strobe_delay(aapl, spico_addr, freq, dll, rd_delay, adjust_rd_pvt, pvt);
  wr_delay_ps = avago_hbm_calc_strobe_delay(aapl, spico_addr, freq, dll, wr_delay, adjust_wr_pvt, pvt);
  ck_delay_ps = avago_hbm_calc_strobe_delay(aapl, spico_addr, freq, dll, ck_delay, adjust_ck_pvt, pvt);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "freq           %0d\n", freq);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "dll            0x%0x\n", dll);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "pvt            0x%0x\n", pvt);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "rd_delay       0x%0x\n", rd_delay);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "wr_delay       0x%0x\n", wr_delay);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "ck_delay       0x%0x\n", ck_delay);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "rd_offset      0x%0x\n", rd_offset);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "wr_offset      0x%0x\n", wr_offset);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "ck_offset      0x%0x\n", ck_offset);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "rd_delay_ps    %d.%d\n", rd_delay_ps/1000, rd_delay_ps%1000);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "wr_delay_ps    %d.%d\n", wr_delay_ps/1000, wr_delay_ps%1000);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "ck_delay_ps    %d.%d\n", ck_delay_ps/1000, ck_delay_ps%1000);


  return 0;
}

/**
 ** @brief  Program a TMRS code
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 ** @param  tmrs_code    TMRS code
 ** @param  channel      Channel to apply code to
 ** @param  safety       Apply the safety bit to this code
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_run_tmrs(Aapl_t *aapl, uint spico_addr, const char *tmrs_code, uint channel, uint safety)
{
  uint die;
  uint apc_addr;
  Avago_hbm_operation_results_t results;

  if (channel == 0xf) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "TMRS Code:%s Safety:%d Channel:all\n", tmrs_code, safety);
  } else {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "TMRS Code:%s Safety:%d Channel:%d\n", tmrs_code, safety, channel);
  }

  if (safety > 1) {
    aapl_fail(aapl, __func__, __LINE__, "Illegal safety value %d.\n", safety);
    return -1;
  }
  die = safety;

  if (toupper(tmrs_code[0]) == 'B') {
    die |= 0x8;
  } else if (toupper(tmrs_code[0]) == 'C') {
    die |= 0x6;
  } else {
    aapl_fail(aapl, __func__, __LINE__, "Illegal die code %c.\n", tmrs_code[0]);
    return -1;
  }

  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_TMRS, 1);
    avago_hbm_launch_channel_operation(aapl, spico_addr, AVAGO_HBM2E_OP_PROGRAM_TMRS, &results, channel, avago_hbm_default_timeout);
    avago_hbm_print_operation_results(aapl, &results);

  } else {
    uint cat;
    uint subcat;
    uint name;

    cat    = tmrs_code[1] - '0';
    subcat = tmrs_code[2] - '0';
    name   = tmrs_code[3] - '0';

    avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

    avago_sbus_wr(aapl, apc_addr, 0x49, ((die << 16) | cat));
    avago_sbus_wr(aapl, apc_addr, 0x48, ((subcat << 16) | name));

    avago_hbm_launch_channel_operation(aapl, spico_addr, AVAGO_HBM_OP_TMRS, &results, channel, avago_hbm_default_timeout);
    avago_hbm_print_operation_results(aapl, &results);
  }

  return 0;
}

/**
 ** @brief  Prints the HBM spare results
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_print_spare_results(Aapl_t *aapl, uint spico_addr)
{
  int spare;
  int rc;

  rc = aapl->return_code;
  spico_addr = avago_hbm_get_spico_address(aapl, spico_addr);
  if (aapl->return_code != rc) { return -1; }

  for (spare=0; spare<=23; spare++) {
    int value;
    value = avago_spico_int(aapl, spico_addr, 0x32, 0x12+spare);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   HBM Spare%d: 0x%0x\n", spare, value);
  }

  return 0;
}


/**
 ** @brief  Runs cell repair to perform a repair on the hbm bitcells
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_cell_repair(Aapl_t *aapl, uint spico_addr, uint channel, uint pc, uint sid, uint bank, uint row, uint hard_repair)
{
  uint apc_addr;
  Avago_hbm_operation_results_t results;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Cell Repair channel:0x%0x pc:0x%0x sid:0x%0x bank:0x%0x row:0x%0x\n", channel, pc, sid, bank, row);
  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_0, channel);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_1, pc);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_2, sid);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_3, bank);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_4, row);
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_CELL_REPAIR, &results, avago_hbm_default_timeout);
    avago_hbm_print_operation_results(aapl, &results);

  } else {
    uint address;

    address = (sid << 22) | (channel << 19) | (pc << 18) | (bank << 14) | row;
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM_MBIST_REPAIR_MODE, hard_repair);
    avago_sbus_wr(aapl, apc_addr, 0x49, address);
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_RUN_CELL_REPAIR, &results, avago_hbm_default_timeout);
    avago_hbm_print_operation_results(aapl, &results);
  }

  return 0;
}

/**
 ** @brief  Runs fuse scan and reports available fuses
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_fuse_scan(Aapl_t *aapl, uint spico_addr, uint channel, uint pc, uint sid, uint bank, uint *result)
{
  uint apc_addr;
  Avago_hbm_operation_results_t results;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Fuse Scan channel:0x%0x pc:0x%0x sid:0x%0x bank:0x%0x\n", channel, pc, sid, bank);

  if (avago_hbm2e_check(aapl, spico_addr)) {
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_0, channel);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_1, pc);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_2, sid);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_3, bank);
    avago_hbm_set_parameter(aapl, spico_addr, AVAGO_HBM2E_MBIST_4, 0);
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM2E_OP_FUSE_SCAN, &results, avago_hbm_default_timeout);
    avago_hbm_print_operation_results(aapl, &results);

  } else {
    uint address;

    address = (sid << 22) | (channel << 19) | (pc << 18) | (bank << 14);
    avago_sbus_wr(aapl, apc_addr, 0x49, address);
    avago_hbm_launch_operation(aapl, spico_addr, AVAGO_HBM_OP_FUSE_SCAN, &results, avago_hbm_default_timeout);
    avago_hbm_print_operation_results(aapl, &results);
  }

  *result = avago_sbus_rd(aapl, apc_addr, 0x4a);

  return 0;
}


/**
 ** @brief  Create a soft lane repair in both the HBM and PHY
 **
 ** @param  aapl         Aapl_t struct
 ** @param  spico_addr   Sbus address of the spico
 ** @param  channel      Channel number to apply the repair to
 ** @param  repair0      Bits[31:0] of the repair code
 ** @param  repair1      Bits[63:32] of the repair code
 ** @param  repair2      Bits[71:64] of the repair code
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_hbm_soft_lane_repair(Aapl_t *aapl, uint spico_addr, uint channel, uint repair0, uint repair1, uint repair2)
{
  uint apc_addr;
  uint phy_hbm;

  avago_hbm_get_apc_addr(aapl, spico_addr, &apc_addr);


  for (phy_hbm=0; phy_hbm<=1; phy_hbm++) {
    if (avago_hbm2e_check(aapl, spico_addr)) {
      avago_sbus_wr(aapl, apc_addr, 0x14, repair0);
      avago_sbus_wr(aapl, apc_addr, 0x15, repair1);
      avago_sbus_wr(aapl, apc_addr, 0x16, repair2);
    } else {
      avago_sbus_wr(aapl, apc_addr, 0x04, repair0);
      avago_sbus_wr(aapl, apc_addr, 0x05, repair1);
      avago_sbus_wr(aapl, apc_addr, 0x06, repair2);
    }

    avago_hbm_wir_write_channel(aapl, apc_addr, phy_hbm, channel, PHY_SOFT_LANE_REPAIR);
    avago_hbm_wdr_write(aapl, apc_addr, PHY_SOFT_LANE_REPAIR_LENGTH, 0);
  }

  return 0;
}




#endif

/** @} */
