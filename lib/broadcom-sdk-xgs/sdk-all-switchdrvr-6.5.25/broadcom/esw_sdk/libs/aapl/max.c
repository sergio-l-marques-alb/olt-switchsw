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
 ** @brief MAX functions.
 ** @defgroup MAX MAX PHY functions
 ** @{
 **/

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#ifdef interface
#undef interface
#endif

#if AAPL_ENABLE_MAX

#define WIR_WRITE 1
#define WDR_WRITE 2
#define WDR_READ  4
#define WDR_RESET 8
#define WR_CLEAR  0

#define LOCAL 0
#define REMOTE 1

#define WDR_BYPASS                       0x0
#define WDR_BYPASS_LENGTH                1
#define WDR_EXTEST_RX                    0x30
#define WDR_EXTEST_RX_LENGTH             88
#define WDR_EXTEST_TX                    0x32
#define WDR_EXTEST_TX_LENGTH             88
#define WDR_TX_LANE_REPAIR               0x34
#define WDR_TX_LANE_REPAIR_LENGTH        32
#define WDR_RX_LANE_REPAIR               0x35
#define WDR_RX_LANE_REPAIR_LENGTH        32
#define WDR_TX_PHY_CONFIG                0x36
#define WDR_TX_PHY_CONFIG_LENGTH         32
#define WDR_RX_PHY_CONFIG                0x37
#define WDR_RX_PHY_CONFIG_LENGTH         32
#define WDR_TX_BIST                      0x38
#define WDR_TX_BIST_LENGTH               320
#define WDR_RX_BIST                      0x39
#define WDR_RX_BIST_LENGTH               320
#define WDR_TX_DW_READY                  0x3a
#define WDR_TX_DW_READY_LENGTH           32
#define WDR_RX_DW_READY                  0x3b
#define WDR_RX_DW_READY_LENGTH           32

#define MPC_OBS_RX_1500_READY           0x01000000
#define MPC_OBS_RX_1500_RX_REQ          0x02000000
#define MPC_OBS_RX_1500_RX_ACK          0x04000000
#define MPC_OBS_RX_1500_EX_REQ          0x08000000
#define MPC_OBS_RX_1500_EX_ACK          0x10000000
#define MPC_WR_1500_REM_REQ_RX_BUSY     0x00000400
#define MPC_WR_1500_REM_REQ_EX_BUSY     0x00000800
#define MPC_WR_1500_REM_REL_RX_BUSY     0x00001000
#define MPC_WR_1500_REM_REL_EX_BUSY     0x00002000
#define MPC_WR_1500_LOC_REL_RX_BUSY     0x00000100
#define MPC_WR_1500_LOC_REL_EX_BUSY     0x00000200
#define MPC_OBS_CONFIG_RX_1500_REM_BUS  0x00020000
#define MPC_OBS_CONFIG_TX_1500_REM_BUS  0x00040000
#define MPC_RELINQUISH_RX               0x00000040
#define MPC_RELINQUISH_EX               0x00000080
#define MPC_WR_1500_COMMAND_ERROR       0x00004000
#define MPC_WR_1500_ARB_DONE            0x00000020
#define MPC_REQUEST_RX                  0x00000010
#define MPC_REQUEST_EX                  0x00000020
#define MPC_WR_1500_LOC_REQ_RX_BUSY     0x00000040
#define MPC_WR_1500_LOC_REQ_EX_BUSY     0x00000080

int avago_max_default_timeout = 15000;
int avago_max_dwell_time = 500;

/**
 ** @brief  Returns the SPICO sbus address if either an APC or SPICO address is provided
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr    Sbus address of the max
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static uint avago_max_get_spico_address(Aapl_t *aapl, uint sbus_addr)
{
  if (aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_MAX)) {
    sbus_addr = avago_make_sbus_master_addr(sbus_addr);
  }

  if (!aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_SPICO)) {
    Avago_ip_type_t ip_type = aapl_get_ip_type(aapl, sbus_addr);
    aapl_fail(aapl, __func__, __LINE__, "The address 0x%0x is not a SPICO or MAX address.  IP Type = %d\n", sbus_addr, ip_type);
  }

  return sbus_addr;
}


/**
 ** @brief  Verify firmware has been loaded
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr    Sbus address of the max
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_fw_check(Aapl_t *aapl, uint sbus_addr)
{
  uint spico_addr;
  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  avago_firmware_get_rev(aapl, spico_addr);
  if( !aapl_get_spico_running_flag(aapl, spico_addr) ) {
    aapl_fail(aapl, __func__, __LINE__, "MAX operation cannot run because the firmware has not been loaded or SPICO at address %s is not running.\n", aapl_addr_to_str(spico_addr));
    return -1;
  }
  return 0;
}

#if 0
/**
 ** @brief  Verifies that the sbus ring includes a MAX PHY
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr    Sbus address of the any devce on the max ring
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_ring_check(Aapl_t *aapl, uint sbus_addr)
{
  uint addr;
  uint max_sbus_addr;

  Avago_addr_t addr_struct;
  avago_addr_to_struct(sbus_addr, &addr_struct);

  max_sbus_addr = aapl_get_max_sbus_addr(aapl, sbus_addr);
  for (addr = 1; addr <= max_sbus_addr; addr++) {
    Avago_ip_type_t ip_type;
    addr_struct.sbus = addr;
    ip_type = aapl_get_ip_type(aapl, avago_struct_to_addr(&addr_struct));
    if (ip_type == AVAGO_MAX) {
      return 0;
    }
  }

  return -1;
}
#endif

/**
 ** @brief  Returns the max interface number for the given sbus address
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   Sbus address of the max device
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_get_interface_from_addr(Aapl_t *aapl, uint sbus_addr)
{
  uint local_addr;
  uint max_sbus_addr;
  uint max_interface;
  Avago_addr_t addr_struct;

  if (!aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, TRUE, 1, AVAGO_MAX)) {
    return -1;
  }

  avago_addr_to_struct(sbus_addr, &addr_struct);

  max_interface = 0;
  max_sbus_addr = aapl_get_max_sbus_addr(aapl, sbus_addr);
  for (local_addr = 1; local_addr <= max_sbus_addr; local_addr++) {
    uint addr;
    addr_struct.sbus = local_addr;
    addr = avago_struct_to_addr(&addr_struct);

    if (aapl_check_ip_type(aapl, addr, __func__, __LINE__, FALSE, 1, AVAGO_MAX)) {
      if (sbus_addr == avago_struct_to_addr(&addr_struct)) {
        return max_interface;
      } else {
        max_interface += 1;
      }
    }
  }

  aapl_fail(aapl, __func__, __LINE__, "Failed to determine Max interface from sbus address 0x%0x\n", sbus_addr);
  return -1;
}


/**
 ** @brief  Returns the max sbus address for a given interface on a ring
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   Sbus address of the device on the same ring as a max device
 ** @param  max_interface   Max interface number
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_get_addr_from_interface(Aapl_t *aapl, uint sbus_addr, uint max_interface)
{
  uint addr;
  uint max_sbus_addr;
  uint i;
  Avago_addr_t addr_struct;

  avago_addr_to_struct(sbus_addr, &addr_struct);

  i = 0;
  max_sbus_addr = aapl_get_max_sbus_addr(aapl, sbus_addr);
  for (addr = 1; addr <= max_sbus_addr; addr++) {
    addr_struct.sbus = addr;
    if (aapl_check_ip_type(aapl, addr, __func__, __LINE__, FALSE, 1, AVAGO_MAX)) {
      if (max_interface == i) {
        return avago_struct_to_addr(&addr_struct);
      } else {
        i += 1;
      }
    }
  }

  aapl_fail(aapl, __func__, __LINE__, "Failed to determine Max interface from sbus address 0x%0x\n", sbus_addr);
  return -1;
}

/**
 ** @brief  Returns true if remote arbitation is not busy
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   Sbus address of the device on the same ring as a max device
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static BOOL avago_max_ok_to_arbitrate(Aapl_t *aapl, uint sbus_addr) {
  uint data;

  data = avago_sbus_rd(aapl, sbus_addr, 0x08);

  if ((!(data & MPC_OBS_RX_1500_READY)) ||
      (data & (MPC_WR_1500_REM_REQ_RX_BUSY | MPC_WR_1500_REM_REQ_EX_BUSY | MPC_WR_1500_REM_REL_RX_BUSY | MPC_WR_1500_REM_REL_EX_BUSY)))
  {
    return FALSE;
  }
  return TRUE;
}

/**
 ** @brief  Wait for arbitration or either the remote or local interface to complete
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr    Sbus address of the device on the same ring as a max device
 ** @param  busy_command Code for busy bits to query
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_wait_for_1500_arb_done(Aapl_t *aapl, uint sbus_addr, uint busy_command) {
  uint data;
  uint sbus_data;
  uint error;
  uint timeout;


  timeout = avago_max_default_timeout;
  do {
    sbus_data = avago_sbus_rd(aapl, sbus_addr, 0x08);
    data  = sbus_data & busy_command;
    error = sbus_data & MPC_WR_1500_COMMAND_ERROR;
    if (error != 0) {
      aapl_fail(aapl, __func__, __LINE__, "Command error during arbitration at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
      return -1;
    }
    timeout--;
  } while ( (data != busy_command) && (timeout > 0) );

  if (timeout == 0) {
    aapl_fail(aapl, __func__, __LINE__, "Timeout waiting for 1500 arbitration at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
    return -1;
  }

  timeout = avago_max_default_timeout;
  do {
    sbus_data = avago_sbus_rd(aapl, sbus_addr, 0x08);
    data  = sbus_data & MPC_WR_1500_ARB_DONE;
    error = sbus_data & MPC_WR_1500_COMMAND_ERROR;
    if (error != 0) {
      aapl_fail(aapl, __func__, __LINE__, "Command error during arbitration at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
      return -1;
    }
    timeout--;
  } while ( (!data) && (timeout > 0) );

  if (timeout == 0) {
    aapl_fail(aapl, __func__, __LINE__, "Timeout waiting for 1500 arbitration at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
    return -1;
  }

  avago_sbus_wr(aapl, sbus_addr, 0x06, 0x0);

  timeout = avago_max_default_timeout;
  do {
    sbus_data = avago_sbus_rd(aapl, sbus_addr, 0x08);
    data  = sbus_data & busy_command;
    error = sbus_data & MPC_WR_1500_COMMAND_ERROR;
    if (error != 0) {
      aapl_fail(aapl, __func__, __LINE__, "Command error during arbitration at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
      return -1;
    }
    timeout--;
  } while ( (data == busy_command) && (timeout > 0) );

  return 0;
}


/**
 ** @brief  Give up control of the WDR interface
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   Sbus address of the device on the same ring as a max device
 ** @param  rx          Indicate whether the EX or RX will be released
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_relinquish_wdr(Aapl_t *aapl, uint sbus_addr, uint rx) {
  uint timeout;
  uint data;
  unsigned long status;

  timeout = avago_max_default_timeout;

  data = avago_sbus_rd(aapl, sbus_addr, 0x08);
  if ((rx  && !(data & MPC_OBS_CONFIG_RX_1500_REM_BUS)) || (!rx && !(data & MPC_OBS_CONFIG_TX_1500_REM_BUS))) {
    return 0;
  }

  while(timeout > 0) {
    if(avago_max_ok_to_arbitrate(aapl, sbus_addr)) {

      if (rx == 1) {
        data = MPC_RELINQUISH_RX;
        status = MPC_WR_1500_LOC_REL_RX_BUSY;
      } else {
        data = MPC_RELINQUISH_EX;
        status = MPC_WR_1500_LOC_REL_EX_BUSY;
      }
      avago_sbus_wr(aapl, sbus_addr, 0x06, data);


      if (avago_max_ok_to_arbitrate(aapl, sbus_addr)) {
        if (avago_max_wait_for_1500_arb_done(aapl, sbus_addr, status) != 0) { return -1; }
        avago_sbus_wr(aapl, sbus_addr, 0x06, 0x0);

        data = avago_sbus_rd(aapl, sbus_addr, 0x08);
        if ((rx  && !(data & MPC_OBS_CONFIG_RX_1500_REM_BUS)) || (!rx && !(data & MPC_OBS_CONFIG_TX_1500_REM_BUS))) {
          return 0;
        }
      } else {
        avago_sbus_wr(aapl, sbus_addr, 0x06, 0x0);
      }
    }

    timeout --;
  }

  aapl_fail(aapl, __func__, __LINE__, "Timeout waiting for arbitration reliquish at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
  return -1;
}

/**
 ** @brief  Requests control of the WDR interface
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   Sbus address of the device on the same ring as a max device
 ** @param  rx          Indicate whether the EX or RX will be requested
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_request_wdr(Aapl_t *aapl, uint sbus_addr, uint rx) {
  uint timeout;
  uint status;
  uint data;

  timeout = avago_max_default_timeout;
  while(timeout > 0) {
    if (avago_max_ok_to_arbitrate(aapl, sbus_addr)) {
      if (rx == 1) {
        data = MPC_REQUEST_RX;
        status = MPC_WR_1500_LOC_REQ_RX_BUSY;
      } else {
        data = MPC_REQUEST_EX;
        status = MPC_WR_1500_LOC_REQ_EX_BUSY;
      }
      avago_sbus_wr(aapl, sbus_addr, 0x06, data);


      if (avago_max_ok_to_arbitrate(aapl, sbus_addr)) {
        if (avago_max_wait_for_1500_arb_done(aapl, sbus_addr, status) != 0) { return -1; }
        avago_sbus_wr(aapl, sbus_addr, 0x06, 0x0);

        data = avago_sbus_rd(aapl, sbus_addr, 0x08);
        if ((rx  && (data & MPC_OBS_CONFIG_RX_1500_REM_BUS)) || (!rx && (data & MPC_OBS_CONFIG_TX_1500_REM_BUS))) {
          return 0;
        }
      } else {
        avago_sbus_wr(aapl, sbus_addr, 0x06, 0x0);
      }

    }
    timeout --;
  }

  aapl_fail(aapl, __func__, __LINE__, "Timeout waiting for arbitration request at SBus address %s.\n", aapl_addr_to_str(sbus_addr));
  return -1;
}

/**
 ** @brief  Requests control of the remote RX or TX WDR interface
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   Sbus address of the device on the same ring as a max device
 ** @param  rx          Indicate whether the EX or RX will be requested
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_request_arbitration(Aapl_t *aapl, uint sbus_addr, uint tx_rem, uint rx_rem) {
  uint data;

  data = avago_sbus_rd(aapl, sbus_addr, 0x08);

  if ((tx_rem == 0) && ((data & MPC_OBS_CONFIG_TX_1500_REM_BUS) != 0)) {
    if (avago_max_relinquish_wdr(aapl, sbus_addr, 0) != 0) { return -1; }

  } else if ((rx_rem == 0) && ((data & MPC_OBS_CONFIG_RX_1500_REM_BUS) != 0)) {
    if (avago_max_relinquish_wdr(aapl, sbus_addr, 1) != 0) { return -1; }
  }

  data = avago_sbus_rd(aapl, sbus_addr, 0x08);

  if ((tx_rem == 1) && ((data & MPC_OBS_CONFIG_TX_1500_REM_BUS) == 0)) {
    if (avago_max_request_wdr(aapl, sbus_addr, 0) != 0) { return -1; }

  } else if ((rx_rem == 1) && ((data & MPC_OBS_CONFIG_RX_1500_REM_BUS) == 0)) {
    if (avago_max_request_wdr(aapl, sbus_addr, 1) != 0) { return -1; };
  }

  return 0;
}

/**
 ** @brief  Waits for the 1500 BUSY DONE register to equal the expected value
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr   sbus_address of the APC receiver
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_wait_for_1500_done(Aapl_t *aapl, uint sbus_addr, uint command)
{
  uint data;
  uint timeout;

  data = 0;
  timeout = avago_max_default_timeout;
  while ( (((data >> 1) & 0xf) != command) && timeout > 0) {
    data = avago_sbus_rd(aapl, sbus_addr, 0x08);
    if ( ((data >> 14) & 0x1) == 1) {
      aapl_fail(aapl, __func__, __LINE__, "WR_1500_COMMAND_ERROR detected\n");
      return -1;
    }
    timeout = timeout - 1;
  }

  if (timeout == 0) {
    switch(command) {
      case WIR_WRITE: { aapl_fail(aapl, __func__, __LINE__, "Timeout while waiting for WR_1500_WIR_WRITE_BUSY\n"); break;}
      case WDR_WRITE: { aapl_fail(aapl, __func__, __LINE__, "Timeout while waiting for WR_1500_WDR_WRITE_BUSY\n"); break;}
      case WDR_READ:  { aapl_fail(aapl, __func__, __LINE__, "Timeout while waiting for WR_1500_WDR_READ_BUSY\n");  break;}
      case WDR_RESET: { aapl_fail(aapl, __func__, __LINE__, "Timeout while waiting for WR_1500_WDR_RESET_BUSY\n"); break;}
    }
  }

  data = 0;
  timeout = avago_max_default_timeout;
  while ( ((data & 0x1) != 1) && timeout > 0) {
    data = avago_sbus_rd(aapl, sbus_addr, 0x08);
    if ( ((data >> 14) & 0x1) == 1) {
      aapl_fail(aapl, __func__, __LINE__, "WR_1500_COMMAND_ERROR detected\n");
      return -1;
    }
    timeout = timeout - 1;
  }

  if (timeout == 0) {
    aapl_fail(aapl, __func__, __LINE__, "Timeout while waiting for WR_1500_DONE\n");
    return -1;
  }


  return 0;
}


/**
 ** @brief  Sets the IEEE 1500 WIR
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr    sbus_address of the APC receiver
 ** @param  rx_enable   Set to enable remote WDR
 ** @param  channel     MAX channel to write to
 ** @param  instruction WIR instruction
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_wir_write_channel(Aapl_t *aapl, uint sbus_addr, uint rx_enable, uint channel, uint instruction)
{
  avago_sbus_wr(aapl, sbus_addr, 0x10, ((rx_enable << 12) | (channel << 6) | instruction));
  avago_sbus_wr(aapl, sbus_addr, 0x06, WIR_WRITE);
  avago_max_wait_for_1500_done(aapl, sbus_addr, WIR_WRITE);
  avago_sbus_wr(aapl, sbus_addr, 0x06, WR_CLEAR);

  return 0;
}

/**
 ** @brief  Performs a WDR write operation
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr   sbus_address of the APC receiver
 ** @param  control_1500   1500 write instruction
 ** @param  length         WIR length
 ** @param  word_count     Number of 32-data words to write
 ** @param  ...            Data words
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/


/**
 ** @brief  Performs a WDR read operation
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr     sbus_address of the APC receiver
 ** @param  length       WDR length
 ** @param  shift_only   Shift the WDR without a capture
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
static int avago_max_wdr_read(Aapl_t *aapl, uint sbus_addr, int length, int shift_only)
{
  avago_sbus_wr(aapl, sbus_addr, 0x06, WR_CLEAR);
  avago_sbus_wr(aapl, sbus_addr, 0x11, (shift_only << 9) | length);
  avago_sbus_wr(aapl, sbus_addr, 0x06, WDR_READ);
  avago_max_wait_for_1500_done(aapl, sbus_addr, WDR_READ);
  avago_sbus_wr(aapl, sbus_addr, 0x06, WR_CLEAR);

  return 0;
}



/**
 ** @brief  Sets an max parameter in firmware
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 ** @param  param      Parameter number to set
 ** @param  value      Parameter value to set
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_set_parameter(Aapl_t *aapl, uint sbus_addr, Avago_max_parameter_t param, uint value)
{
  uint result;
  uint status;
  uint spico_addr;

  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  result = avago_spico_int(aapl, spico_addr, 0x34, param);
  status = result >> 16;

  if (status == 0x7fff) {
    aapl_fail(aapl, __func__, __LINE__, "Illegal MAX parameter number, %d.\n", param);
    return -1;
  }

  avago_spico_int(aapl, spico_addr, 0x35, value);

  return 0;
}


/**
 ** @brief  Gets an MAX firmware parameter
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max interface
 ** @param  param      Parameter number to get
 **
 ** @return  Returns the parameter value.  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_get_parameter(Aapl_t *aapl, uint sbus_addr, Avago_max_parameter_t param)
{
  uint result;
  uint data;
  uint status;
  uint spico_addr;

  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  result = avago_spico_int(aapl, spico_addr, 0x33, param);
  data = result & 0xffff;
  status = result >> 16;

  if (status == 0x7fff) {
    aapl_fail(aapl, __func__, __LINE__, "Illegal MAX parameter number, %d.\n", param);
    return -1;
  }

  return data;
}


/**
 ** @brief  Returns true if the max device is a master
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 **
 ** @return  Returns true if master
 **/
BOOL avago_max_is_master(Aapl_t *aapl, uint sbus_addr)
{
  uint fw_config;
  if (aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, TRUE, 1, AVAGO_MAX)) {
    fw_config = avago_sbus_rd(aapl, sbus_addr, 0x33);
  } else {
    fw_config = 0;
  }

  return ((fw_config & 0x1) == 1);
}

/**
 ** @brief  Returns a string description of the integer error code
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 **
 ** @return  Error code description
 **/
const char *avago_max_master_slave_str(Aapl_t *aapl, uint sbus_addr)
{
  return (avago_max_is_master(aapl,sbus_addr) == TRUE) ? "master" : "slave";
}

/**
 ** @brief  Returns a string description of the integer error code
 ** @param  error_code    error code integer
 **
 ** @return  Error code description
 **/
static const char * avago_max_status_code_desc(uint error_code)
{
  const char * desc;

  switch(error_code) {
    case 0x00: { desc = "NO_OPERATION_RUN";     break; }
    case 0x01: { desc = "SUCCESS";              break; }
    case 0x02: { desc = "OPERATION_ACTIVE";     break; }
    case 0x05: { desc = "ERROR_DETECTED";       break; }
    default:   { desc =  "UNKNOWN_STATUS_CODE"; break; }
  }
  return desc;
}

/**
 ** @brief  Returns a string description of the integer error code
 ** @param  error_code    error code integer
 **
 ** @return  Error code description
 **/
static const char * avago_max_error_code_desc(uint error_code)
{
  const char * desc;

  switch(error_code) {
    case 0x00: { desc = "SUCCESS";                                   break; }
    case 0x08: { desc = "ERROR_UNKNOWN_ERROR";                       break; }
    case 0x09: { desc = "ERROR_MAX_POWER_ON_RESET_L_ASSERTED";       break; }
    case 0x0a: { desc = "ERROR_MAX_POWER_UP_DRV_DISABLE_L_ASSERTED"; break; }
    case 0x0b: { desc = "ERROR_CHANNEL_FAILURES_EXIST";              break; }
    case 0x0c: { desc = "ERROR_ILLEGAL_CHANNEL_NUMBER_REQUESTED";    break; }
    case 0x0d: { desc = "ERROR_ILLEGAL_INTERFACE_NUMBER_REQUESTED";  break; }
    case 0x0e: { desc = "ERROR_TIMEOUT_WAITING_FOR_1500_BUSY";       break; }
    case 0x0f: { desc = "ERROR_TIMEOUT_WAITING_FOR_1500_DONE";       break; }
    case 0x10: { desc = "ERROR_WR_1500_COMMAND_ERROR";               break; }
    case 0x11: { desc = "ERROR_TIMEOUT_WAITING_FOR_1500_ARB_BUSY";   break; }
    case 0x12: { desc = "ERROR_TIMEOUT_WAITING_FOR_1500_ARB_DONE";   break; }
    case 0x13: { desc = "ERROR_TIMEOUT_RELINQUISH_WDR";              break; }
    case 0x14: { desc = "ERROR_TIMEOUT_REQUEST_WDR";                 break; }
    case 0x15: { desc = "ERROR_DATA_COMPARE_FAILED";                 break; }
    case 0x16: { desc = "ERROR_LANE_ERRORS_DETECTED";                break; }
    case 0x17: { desc = "ERROR_ALL_CHANNELS_NOT_ENABLED";            break; }
    case 0x18: { desc = "ERROR_REPAIR_LIMIT_EXCEEDED";               break; }
    case 0x19: { desc = "ERROR_NON_REPAIRABLE_FAULTS_FOUND";         break; }
    case 0x1a: { desc = "ERROR_EFUSE_BURN_FAILED";                   break; }
    case 0x1b: { desc = "ERROR_TIMEOUT_WAITING_FOR_EFUSE_ACK";       break; }
    case 0x1c: { desc = "ERROR_TIMEOUT_WAITING_FOR_EFUSE_READY";     break; }
    case 0x1d: { desc = "ERROR_TIMEOUT_WAITING_FOR_EFUSE_DONE";      break; }
    case 0x1e: { desc = "ERROR_EFUSE_BYTE_MISMATCH";                 break; }
    case 0x1f: { desc = "ERROR_MULTIPLE_EFUSE_ON_RING";              break; }
    case 0x20: { desc = "ERROR_EFUSE_NOT_FOUND_ON_RING";             break; }
    case 0x21: { desc = "ERROR_TIMEOUT_WAITING_FOR_BIST_DONE";       break; }
    case 0x22: { desc = "ERROR_REPAIRABLE_FAULTS_FOUND";             break; }
    case 0x23: { desc = "ERROR_SLAVE_DEVICE";                        break; }
    case 0x24: { desc = "ERROR_INVALID_REPAIR";                      break; }
    case 0x25: { desc = "ERROR_MAX_CANT_RESET_WITH_ARBITRATION";     break; }
    case 0x26: { desc = "ERROR_INVALID_BIST_STATE";                  break; }
    case 0x27: { desc = "ERROR_INJECTED_ERROR_NOT_DETECTED";         break; }
    default:   { desc =  "UNKNOWN_ERROR_CODE";                       break; }
  }
  return desc;
}


/**
 ** @brief  Returns a string description of the integer operation code
 ** @param  operation_code    operation code integer
 **
 ** @return  Error code description
 **/
static const char * avago_max_operation_desc(uint operation_code)
{
  const char * desc;

  switch(operation_code) {
    case 0x00: { desc = "OP_SUCCESS";             break; }
    case 0xc8: { desc = "OP_BYPASS";              break; }
    case 0xc9: { desc = "OP_RESET";               break; }
    case 0xca: { desc = "OP_MMT_BIST";            break; }
    case 0xcb: { desc = "OP_ILB_BIST";            break; }
    case 0xcc: { desc = "OP_SLB_BIST";            break; }
    case 0xcd: { desc = "OP_LANE_REPAIR";         break; }
    case 0xce: { desc = "OP_BURN_LANE_REPAIR";    break; }
    case 0xcf: { desc = "OP_READ_SOFT_REPAIRS";   break; }
    case 0xd0: { desc = "OP_READ_HARD_REPAIRS";   break; }
    case 0xd1: { desc = "OP_APPLY_SOFT_REPAIRS";  break; }
    case 0xd2: { desc = "OP_READ_PHY_CONFIG";     break; }
    case 0xd3: { desc = "OP_READ_LANE_REPAIR";    break; }
    default :  { desc = "UNKNOWN_OPERATION";      break; }
  }
  return desc;
}



/**
 ** @brief  Launches an MAX operation on a single MAX channel
 **
 ** @param  aapl              Aapl_t struct
 ** @param  sbus_addr         sbus address of the max device
 ** @param  operation         Operations to run
 ** @param  results           Operations to run
 ** @param  channel           Channel number to run on
 ** @param  max_timeout       Timeout value
 ** @param  wait_for_complete Set to 1 to wait for results before returning
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_launch_channel_operation_base(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    Avago_max_operation_results_t *results,
    uint channel,
    int max_timeout,
    int wait_for_complete)
{
  uint result;
  uint status;
  uint spico_addr;
  int max_interface;

  Avago_addr_t addr_struct;
  avago_addr_to_struct(sbus_addr, &addr_struct);

  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);

  max_interface = avago_max_get_interface_from_addr(aapl, sbus_addr);
  if (max_interface == -1) {
    return -1;
  }

  if (avago_max_fw_check(aapl, spico_addr) != 0) {
    return -1;
  }

  memset(results, 0, sizeof(*results));

  if (channel == 0x10) {
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Begin MAX operation 0x%02x for all channels on interface %d SBus ring %d \n", operation, max_interface, addr_struct.ring);
    result = avago_spico_int(aapl, spico_addr, 0x30, (max_interface << 13) | operation);
  } else {
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Begin MAX operation 0x%02x for channel%d on interface %d SBus ring %d \n", operation, channel, max_interface, addr_struct.ring);
    result = avago_spico_int(aapl, spico_addr, 0x31, ((max_interface << 13) | (channel << 8) | operation));
  }
  status = result >> 16;


  if (status == 0x7fff) {
    aapl_fail(aapl, __func__, __LINE__, "MAX operation already in progress on SBus ring %d.\n", addr_struct.ring);
    return -1;
  }

  if (wait_for_complete == 1) {
    return avago_max_check_channel_operation(aapl, spico_addr, operation, results, channel, max_timeout);
  } else {
    return 0;
  }
}



/**
 ** @brief  Launches an MAX operation on all MAX channels
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 ** @param  operation  Operations to run
 ** @param  results    Results structure
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_launch_operation(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    Avago_max_operation_results_t *results,
    int max_timeout)
{
  return avago_max_launch_channel_operation_base(aapl, sbus_addr, operation, results, 0x10, max_timeout, 1);
}

/** @brief  Launches an MAX operation on all MAX channels and returns without checking for completion
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 ** @param  operation  Operations to run
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_launch_operation_nowait(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    int max_timeout)
{
  Avago_max_operation_results_t results;
  return avago_max_launch_channel_operation_base(aapl, sbus_addr, operation, &results, 0x10, max_timeout, 0);
}


/**
 ** @brief  Launches an MAX operation on a single MAX channel
 **
 ** @param  aapl        Aapl_t struct
 ** @param  sbus_addr   sbus_address of the max
 ** @param  operation   Operations to run
 ** @param  results     Results structure
 ** @param  channel     Channel number to run on
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_launch_channel_operation(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    Avago_max_operation_results_t *results,
    uint channel,
    int max_timeout)
{
  return avago_max_launch_channel_operation_base(aapl, sbus_addr, operation, results, channel, max_timeout, 1);
}


/**
 ** @brief  Launches an MAX operation on a single MAX channel without checking for completion
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 ** @param  operation  Operations to run
 ** @param  channel     Channel number to run on
 ** @param  max_timeout Timeout value
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_launch_channel_operation_nowait(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    uint channel,
    int max_timeout)
{
  Avago_max_operation_results_t results;
  return avago_max_launch_channel_operation_base(aapl, sbus_addr, operation, &results, channel, max_timeout, 0);
}

/**
 ** @brief  Waits for an MAX operation on a single MAX channel to complete and checks the results
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 ** @param  operation  Operations to run
 ** @param  results    Results structure
 ** @param  channel    Channel number to run on
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_check_channel_operation(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    Avago_max_operation_results_t *results,
    uint channel,
    int max_timeout)
{
  int timeout = 0;
  uint result;
  int rc;
  uint spico_addr;

  Avago_addr_t addr_struct;
  avago_addr_to_struct(sbus_addr, &addr_struct);

  (void)channel;

  memset(results, 0, sizeof(*results));

  rc = aapl->return_code;
  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  if (aapl->return_code != rc) { return -1; }

  do {
    result = avago_spico_int(aapl, spico_addr, 0x32, 0x00);
    if (max_timeout != 0) { timeout += 1; }
  } while ((result & 0x03) != 0x01 && timeout <= max_timeout);

  aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "MAX operation result code: 0x%02x\n", result);

  avago_max_get_operation_results(aapl, spico_addr, results);

  if ((timeout >= max_timeout) && (max_timeout != 0)) {
    aapl_fail(aapl, __func__, __LINE__, "MAX operation timed out on SBus address %s.\n", aapl_addr_to_str(spico_addr));
    return -1;
  }

  if ((result & 0x03) != 0x01) {
    aapl_fail(aapl, __func__, __LINE__, "MAX operation result produced an unexpected result: 0x%02x.\n", result);
    return -1;
  }

  if ((result & 0x04) == 0x04) {
    aapl_fail(aapl, __func__, __LINE__, "MAX operation 0x%02x failed at SBus address %s.\n", operation, aapl_addr_to_str(spico_addr));
    return -1;

  } else {
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "MAX operation 0x%02x successfully completed on all channels on SBus ring %d \n", operation, addr_struct.ring);
    return 0;
  }

  return 0;
}

/**
 ** @brief  Waits for and checks the results of an MAX operation on all MAX channels
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr  sbus_address of the max
 ** @param  operation  Operations to run
 ** @param  results    Results structure
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_check_operation(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_max_operation_t operation,
    Avago_max_operation_results_t *results,
    int max_timeout)
{
  return avago_max_check_channel_operation(aapl, sbus_addr, operation, results, 0x10, max_timeout);
}




/**
 ** @brief  Reads the MAX channel errors code from firmware and stores the
 **
 ** @param  aapl       Aapl_t struct
 ** @param  sbus_addr sbus_address of the sbus master spico
 ** @param  results    Updated with the result of the operation
 **
 ** @return Always returns 0
 **/
int avago_max_get_operation_results(Aapl_t *aapl, uint sbus_addr, Avago_max_operation_results_t *results)
{
  int channel;
  uint spico_addr;

  spico_addr = avago_make_sbus_master_addr(sbus_addr);
  results->operation_status = avago_spico_int(aapl, spico_addr, 0x32, 0x00);
  results->global_error_code = avago_spico_int(aapl, spico_addr, 0x32, 0x01);
  for (channel = 0; channel <= 15; channel++) {
    results->channel_error_code[channel] = avago_spico_int(aapl, spico_addr, 0x32, 0x02+channel);
    results->channel_operation_code[channel]  = avago_spico_int(aapl, spico_addr, 0x32, 0x12+channel);
  }

  return 0;
}


/**
 ** @brief  Print the operation results
 **
 ** @param  aapl       Aapl_t struct
 ** @param  results    Operation results struct
 ** @param  verbose    Print error/op codes for passing channels if 1
 **
 ** @return  0
 **/
int avago_max_print_operation_results(Aapl_t *aapl, Avago_max_operation_results_t *results, uint verbose)
{
  if (results->operation_status == 1 && results->global_error_code == 0) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Operation passed\n");

  } else {
    int channel;
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Operation failed\n");
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   operation status:%s\n", avago_max_status_code_desc(results->operation_status));
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   global_error_code:%s\n", avago_max_error_code_desc(results->global_error_code));


    for (channel = 0; channel <= 15; channel++) {

      if (results->channel_error_code[channel] != 0 || verbose==1) {


        if (results->channel_operation_code[channel] == 0) {
          aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   channel%d error_codes:%s\n",
              channel,
              avago_max_error_code_desc(results->channel_error_code[channel])
              );
        } else {
          aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   channel%d error_codes:%s / %s\n",
              channel,
              avago_max_error_code_desc(results->channel_error_code[channel]),
              avago_max_operation_desc(results->channel_operation_code[channel])
              );
        }
      }
    }
  }

  return 0;
}

/**
 ** @brief  Prints the MAX spare results
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr    Sbus address of the max
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_print_spare_results(Aapl_t *aapl, uint sbus_addr, uint skip_zero_results)
{
  int spare;
  uint spico_addr;

  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);

  for (spare=0; spare<=23; spare++) {
    int value;
    value = avago_spico_int(aapl, spico_addr, 0x32, 0x22+spare);
    if ((skip_zero_results == 1)  && (value == 0)) {
    } else {
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   MAX Spare%d: 0x%0x\n", spare, value);
    }
  }

  return 0;
}

/**
 ** @brief   Decodes and prints the TX_PHY_CONFIG or RX_PHY_CONFIG
 **
 ** @param   sbus_addr sbus_address of the max
 ** @param   channel   Max channel to read the PHY_CONFIG from
 ** @param   tx_rx     0 tx, 1 rx
 **
 ** @return  0
 **/
int avago_max_print_phy_config(Aapl_t *aapl, uint sbus_addr, uint channel, uint tx_rx)
{
  unsigned long phy_config;
  uint phy_config_dw1;
  uint dw0_config_tx_enable;
  uint dw0_config_rx_enable;
  uint dw0_config_ilb_enable;
  uint dw0_config_rx_passthru;
  uint dw0_config_drv_imp;
  uint dw0_config_stb_disable;
  uint dw0_config_qc_delay_config;
  uint dw0_config_ext_ecc_enable;
  uint dw0_config_dbi_enable;
  uint dw0_config_pvt_dely_offset;
  uint dw1_config_tx_enable;
  uint dw1_config_rx_enable;
  uint dw1_config_ilb_enable;
  uint dw1_config_rx_passthru;
  uint dw1_config_drv_imp;
  uint dw1_config_stb_disable;
  uint dw1_config_qc_delay_config;
  uint dw1_config_ext_ecc_enable;
  uint dw1_config_dbi_enable;
  uint dw1_config_pvt_dely_offset;
  int rc;

  rc = aapl->return_code;
  if (tx_rx == 0) {
    phy_config = avago_max_read_tx_phy_config(aapl, sbus_addr, channel);
  } else {
    phy_config = avago_max_read_rx_phy_config(aapl, sbus_addr, channel);
  }
  if (aapl->return_code != rc) {
    return -1;
  }
  phy_config_dw1 = phy_config >> 16;

  dw0_config_tx_enable       = (phy_config >> 0) & 0x1;
  dw0_config_rx_enable       = (phy_config >> 1) & 0x1;
  dw0_config_ilb_enable      = (phy_config >> 2) & 0x1;
  dw0_config_rx_passthru     = (phy_config >> 3) & 0x1;
  dw0_config_drv_imp         = (phy_config >> 4) & 0x3;
  dw0_config_stb_disable     = (phy_config >> 6) & 0x1;
  dw0_config_qc_delay_config = (phy_config >> 7) & 0x7;
  dw0_config_ext_ecc_enable  = (phy_config >> 10) & 0x1;
  dw0_config_dbi_enable      = (phy_config >> 11) & 0x1;
  dw0_config_pvt_dely_offset = (phy_config >> 12) & 0x7;
  dw1_config_tx_enable       = (phy_config_dw1 >> 0) & 0x1;
  dw1_config_rx_enable       = (phy_config_dw1 >> 1) & 0x1;
  dw1_config_ilb_enable      = (phy_config_dw1 >> 2) & 0x1;
  dw1_config_rx_passthru     = (phy_config_dw1 >> 3) & 0x1;
  dw1_config_drv_imp         = (phy_config_dw1 >> 4) & 0x3;
  dw1_config_stb_disable     = (phy_config_dw1 >> 6) & 0x1;
  dw1_config_qc_delay_config = (phy_config_dw1 >> 7) & 0x7;
  dw1_config_ext_ecc_enable  = (phy_config_dw1 >> 10) & 0x1;
  dw1_config_dbi_enable      = (phy_config_dw1 >> 11) & 0x1;
  dw1_config_pvt_dely_offset = (phy_config_dw1 >> 12) & 0x7;

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_TX_ENABLE       : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_tx_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_RX_ENABLE       : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_rx_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_ILB_ENABLE      : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_ilb_enable);
  if (tx_rx == 1) aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_RX_CONFIG_RX_PASSTHRU     : %d\n", dw0_config_rx_passthru);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_DRV_IMP         : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_drv_imp);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_STB_DISABLE     : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_stb_disable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_QC_DELAY_CONFIG : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_qc_delay_config);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_EXT_ECC_ENABLE  : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_ext_ecc_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_DBI_ENABLE      : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_dbi_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW0_%s_CONFIG_PVT_DELY_OFFSET : %d\n", tx_rx==0 ? "TX" : "RX", dw0_config_pvt_dely_offset);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_TX_ENABLE       : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_tx_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_RX_ENABLE       : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_rx_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_ILB_ENABLE      : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_ilb_enable);
  if (tx_rx == 1) aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_RX_CONFIG_RX_PASSTHRU     : %d\n", dw1_config_rx_passthru);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_DRV_IMP         : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_drv_imp);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_STB_DISABLE     : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_stb_disable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_QC_DELAY_CONFIG : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_qc_delay_config);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_EXT_ECC_ENABLE  : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_ext_ecc_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_DBI_ENABLE      : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_dbi_enable);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "   DW1_%s_CONFIG_PVT_DELY_OFFSET : %d\n", tx_rx==0 ? "TX" : "RX", dw1_config_pvt_dely_offset);

  return 0;
}

/**
 ** @brief   Reads the TX_PHY_CONFIG via firmware
 **
 ** @param   sbus_addr sbus_address of the max
 **
 ** @return  0
 **/
unsigned long avago_max_read_tx_phy_config(Aapl_t *aapl, uint sbus_addr, uint channel)
{
  int rc;
  unsigned long tx_phy_config;
  uint spico_addr;
  Avago_max_operation_results_t results;

  rc = avago_max_launch_channel_operation(aapl, sbus_addr, AVAGO_MAX_OP_READ_TX_PHY_CONFIG, &results, channel, avago_max_default_timeout);
  if (rc != 0) {
    avago_max_print_operation_results(aapl, &results, 0);
    return -1;
  }


  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  tx_phy_config = (avago_spico_int(aapl, spico_addr, 0x32, 0x22+1) << 16) | avago_spico_int(aapl, spico_addr, 0x32, 0x22+0);
  return tx_phy_config;
}


/**
 ** @brief   Reads the RX_PHY_CONFIG via firmware
 **
 ** @param   sbus_addr sbus_address of the max
 **
 ** @return  0
 **/
unsigned long avago_max_read_rx_phy_config(Aapl_t *aapl, uint sbus_addr, uint channel)
{
  int rc;
  unsigned long rx_phy_config;
  uint spico_addr;
  Avago_max_operation_results_t results;

  rc = avago_max_launch_channel_operation(aapl, sbus_addr, AVAGO_MAX_OP_READ_RX_PHY_CONFIG, &results, channel, avago_max_default_timeout);
  if (rc != 0) {
    avago_max_print_operation_results(aapl, &results, 0);
    return -1;
  }


  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  rx_phy_config = (avago_spico_int(aapl, spico_addr, 0x32, 0x22+1) << 16) | avago_spico_int(aapl, spico_addr, 0x32, 0x22+0);
  return rx_phy_config;
}




/**
 ** @brief   Reads the RX_PHY_CONFIG via firmware
 **
 ** @param   sbus_addr sbus_address of the max
 **
 ** @return  0
 **/
int avago_max_print_rx_bist_results(Aapl_t *aapl, uint sbus_addr, uint channel, uint remote_rx)
{
  uint ch;
  uint start;
  uint end;
  uint dw0_dat_word_save_ph_reg;
  bigint dw0_dat_word_save_reg;
  uint dw0_count_ecc_err_reg;
  uint dw0_count_ecc_warn_reg;
  uint dw0_sticky_log_dat_reg;
  uint dw0_syndrome_save_reg;
  uint dw1_dat_word_save_ph_reg;
  bigint dw1_dat_word_save_reg;
  uint dw1_count_ecc_err_reg;
  uint dw1_count_ecc_warn_reg;
  uint dw1_sticky_log_dat_reg;
  uint dw1_syndrome_save_reg;

  if (channel == 0x10) {
    start = 0;
    end = 15;
  } else {
    start = channel;
    end = channel;
  }

  if (avago_max_request_arbitration(aapl, sbus_addr, 0, remote_rx) != 0) { return -1; }
  if (avago_max_wir_write_channel(aapl, sbus_addr, 1, channel, WDR_RX_BIST) != 0) { return -1; }
  if (avago_max_wdr_read(aapl, sbus_addr, WDR_RX_BIST_LENGTH, 0) != 0) { return -1; }
  if (avago_max_relinquish_wdr(aapl, sbus_addr, 1) != 0) { return -1; }

  for (ch=start; ch<=end; ch++) {
    uint wdr_rd_31_0;
    uint wdr_rd_63_32;
    uint wdr_rd_95_64;
    uint wdr_rd_127_96;
    uint wdr_rd_159_128;
    uint wdr_rd_191_160;
    uint wdr_rd_223_192;
    uint wdr_rd_255_224;
    uint wdr_rd_287_256;
    uint wdr_rd_319_298;
    uint errors;

    avago_sbus_wr(aapl, sbus_addr, 0x13, ch);
    wdr_rd_31_0    = avago_sbus_rd(aapl, sbus_addr, 0x1e);
    wdr_rd_63_32   = avago_sbus_rd(aapl, sbus_addr, 0x1f);
    wdr_rd_95_64   = avago_sbus_rd(aapl, sbus_addr, 0x20);
    wdr_rd_127_96  = avago_sbus_rd(aapl, sbus_addr, 0x21);
    wdr_rd_159_128 = avago_sbus_rd(aapl, sbus_addr, 0x22);

    wdr_rd_191_160 = avago_sbus_rd(aapl, sbus_addr, 0x23);
    wdr_rd_223_192 = avago_sbus_rd(aapl, sbus_addr, 0x24);
    wdr_rd_255_224 = avago_sbus_rd(aapl, sbus_addr, 0x25);
    wdr_rd_287_256 = avago_sbus_rd(aapl, sbus_addr, 0x26);
    wdr_rd_319_298 = avago_sbus_rd(aapl, sbus_addr, 0x27);

    errors = wdr_rd_31_0 |
      wdr_rd_63_32 |
      wdr_rd_95_64 |
      wdr_rd_127_96 |
      wdr_rd_159_128 |
      wdr_rd_191_160 |
      wdr_rd_223_192 |
      wdr_rd_255_224 |
      wdr_rd_287_256 |
      wdr_rd_319_298;


    if (errors != 0) {
      dw0_dat_word_save_ph_reg = (wdr_rd_159_128 >> 24) & 0x1;
      dw0_dat_word_save_reg    = ((((bigint)wdr_rd_159_128 >> 8) & 0xffffff) << 40) | (((bigint)wdr_rd_127_96) << 8) | (wdr_rd_95_64 >> 24);
      dw0_count_ecc_err_reg    = wdr_rd_95_64 & 0xffffff;
      dw0_count_ecc_warn_reg   = wdr_rd_63_32 >> 8;
      dw0_sticky_log_dat_reg   = ((wdr_rd_63_32 & 0xff) << 24) | (wdr_rd_31_0 >> 8);
      dw0_syndrome_save_reg    = wdr_rd_31_0 & 0xff;

      dw1_dat_word_save_ph_reg = (wdr_rd_319_298 >> 24) & 0x1;
      dw1_dat_word_save_reg    = ((((bigint)wdr_rd_319_298 >> 8) & 0xffffff) << 40) | (((bigint)wdr_rd_287_256) << 8) | (wdr_rd_255_224 >> 24);
      dw1_count_ecc_err_reg    = wdr_rd_255_224 & 0xffffff;
      dw1_count_ecc_warn_reg   = wdr_rd_223_192 >> 8;
      dw1_sticky_log_dat_reg   = ((wdr_rd_223_192 & 0xff) << 24) | (wdr_rd_191_160 >> 8);
      dw1_syndrome_save_reg    = wdr_rd_191_160 & 0xff;

      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Max Channel%d BIST Results for Sbus address %s\n", ch, aapl_addr_to_str(sbus_addr));


      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw0_dat_word_save_ph_reg : 0x%x\n", dw0_dat_word_save_ph_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw0_dat_word_save_reg    : 0x%016lx\n", dw0_dat_word_save_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw0_count_ecc_err_reg    : 0x%06x\n", dw0_count_ecc_err_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw0_count_ecc_warn_reg   : 0x%06x\n", dw0_count_ecc_warn_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw0_sticky_log_dat_reg   : 0x%08x\n", dw0_sticky_log_dat_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw0_syndrome_save_reg    : 0x%02x\n", dw0_syndrome_save_reg);

      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw1_dat_word_save_ph_reg : 0x%x\n", dw1_dat_word_save_ph_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw1_dat_word_save_reg    : 0x%016lx\n", dw1_dat_word_save_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw1_count_ecc_err_reg    : 0x%06x\n", dw1_count_ecc_err_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw1_count_ecc_warn_reg   : 0x%06x\n", dw1_count_ecc_warn_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw1_sticky_log_dat_reg   : 0x%08x\n", dw1_sticky_log_dat_reg);
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  dw1_syndrome_save_reg    : 0x%02x\n\n", dw1_syndrome_save_reg);
    }
  }


  return 0;
}


/**
 ** @brief   Prints the raw lane repair codes read from the repair WDRs
 **
 ** @param   sbus_addr sbus_address of the max
 **
 ** @return  0
 **/
int avago_max_print_lane_repairs(Aapl_t *aapl, uint sbus_addr)
{
  uint channel;
  uint ch;
  uint tx_lane_repair[16];
  uint rx_lane_repair[16];

  channel = 0x10;

  if (avago_max_request_arbitration(aapl, sbus_addr, 0, 0) != 0) { return -1; }
  if (avago_max_wir_write_channel(aapl, sbus_addr, 0, channel, WDR_TX_LANE_REPAIR) != 0) { return -1; }
  if (avago_max_wdr_read(aapl, sbus_addr, WDR_TX_LANE_REPAIR_LENGTH, 0) != 0) { return -1; }

  for (ch=0; ch<=15; ch++) {
    avago_sbus_wr(aapl, sbus_addr, 0x13, ch);
    tx_lane_repair[ch] = avago_sbus_rd(aapl, sbus_addr, 0x1e);
  }

  if (avago_max_request_arbitration(aapl, sbus_addr, 0, 0) != 0) { return -1; }
  if (avago_max_wir_write_channel(aapl, sbus_addr, 1, channel, WDR_RX_LANE_REPAIR) != 0) { return -1; }
  if (avago_max_wdr_read(aapl, sbus_addr, WDR_RX_LANE_REPAIR_LENGTH, 0) != 0) { return -1; }

  for (ch=0; ch<=15; ch++) {
    avago_sbus_wr(aapl, sbus_addr, 0x13, ch);
    rx_lane_repair[ch] = avago_sbus_rd(aapl, sbus_addr, 0x1e);
  }

  for (ch=0; ch<=15; ch++) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "  Channel%-2d repairs: tx:0x%08x rx:0x%08x\n", ch, tx_lane_repair[ch], rx_lane_repair[ch]);
  }

  return 0;
}


/**
 ** @brief   Runs basic diagnostics of lane repairs
 **
 ** @param   sbus_addr sbus_address of the max
 **
 ** @return  0
 **/
int avago_max_lane_diagnostics(Aapl_t *aapl, uint sbus_addr)
{
  int rc;
  uint new_repairs;
  Avago_max_operation_results_t results;
  uint spico_addr;

  if (!avago_max_is_master(aapl, sbus_addr)) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Skipping MAX slave read_new_lane_repair for SBus address %s\n", aapl_addr_to_str(sbus_addr));
    return 0;
  }
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Max lane diagnostics for SBus address %s\n", aapl_addr_to_str(sbus_addr));

  rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_READ_EFUSE_REPAIRS, &results, 0x10, avago_max_default_timeout, 1);
  if (rc != 0) {
    avago_max_print_operation_results(aapl, &results, 0);
  }

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Hard lane repairs\n");
  avago_max_print_lane_repairs(aapl, sbus_addr);

  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Running lane_repair operation\n");
  rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_RUN_LANE_REPAIR, &results, 0x10, avago_max_default_timeout, 1);
  if (rc != 0) {
    avago_max_print_operation_results(aapl, &results, 0);
  }

  rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_RUN_LANE_REPAIR, &results, 0x10, avago_max_default_timeout, 1);
  if (rc != 0) {
    avago_max_print_operation_results(aapl, &results, 0);
  }

  spico_addr = avago_max_get_spico_address(aapl, sbus_addr);
  new_repairs = avago_spico_int(aapl, spico_addr, 0x32, 0x22);
  aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Detected %d new repairs\n", new_repairs);

  if (new_repairs > 0) {
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Current lane repairs\n");
    avago_max_print_lane_repairs(aapl, sbus_addr);
  }

  return aapl->return_code;
}

/**
 ** @brief   Runs max single-phy unidirectional diagnostics
 **
 ** @param   sbus_addr sbus_address of the max
 **
 ** @return  0
 **/
int avago_max_bist_diagnostics(Aapl_t *aapl, uint sbus_addr)
{
  int rc;
  Avago_max_operation_results_t results;
  uint bist_mode;
  uint bist_dir;
  uint pattern;
  uint pattern_index;
  uint dbi;
  const char * bist_mode_name;
  const char * bist_dir_name;
  const char * pattern_name;

  uint pattern_list[2] = {1,8};

  for(pattern_index=0; pattern_index<=1; pattern_index++) {
    for(bist_mode=0; bist_mode<=2; bist_mode++) {
      for(bist_dir=0; bist_dir<=1; bist_dir++) {

        pattern = pattern_list[pattern_index];

        if ((bist_mode == 2) && (bist_dir == 1)) { continue; }
        dbi = (pattern >= 4) ? 0 : 1;

        switch(bist_mode) {
          case 0x00: { bist_mode_name = "ilb"; break; }
          case 0x01: { bist_mode_name = "slb"; break; }
          case 0x02: { bist_mode_name = "mmt";  break; }
        }

        switch(bist_dir) {
          case 0x00: { bist_dir_name = "tx"; break; }
          case 0x01: { bist_dir_name = "rx"; break; }
        }

        switch(pattern) {
          case 0x00: { pattern_name = "user";                  break; }
          case 0x01: { pattern_name = "bist_prbs_pattern";     break; }
          case 0x02: { pattern_name = "bist_f1_50t_pattern";   break; }
          case 0x03: { pattern_name = "bist_f1_100t_pattern";  break; }
          case 0x04: { pattern_name = "bist_si0_pattern";      break; }
          case 0x05: { pattern_name = "bist_si1_pattern";      break; }
          case 0x06: { pattern_name = "bist_si2_pattern";      break; }
          case 0x07: { pattern_name = "bist_si3_pattern";      break; }
          case 0x08: { pattern_name = "bist_f4_100t_pattern";  break; }
          default:   { pattern_name = "unknown"; }
        }

        aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Running bist_%s_%s pattern:%s dbi:%d for Sbus address %s\n", bist_mode_name, bist_dir_name, pattern_name, dbi, aapl_addr_to_str(sbus_addr));

        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_CONFIG_EXT_ECC_ENABLE, 1);
        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_CONFIG_DBI_ENABLE, dbi);
        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_BIST_DATA_PATTERN, pattern);
        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_BIST_MODE, 0);
        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_BIST_DIR, 1);
        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_CONFIG_BIST_MODE, 4);
        avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_BIST_FREERUN, 1);
        rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_BIST_CUSTOM, &results, 0x10, avago_max_default_timeout, 1);
        if (rc != 0) {
          avago_max_print_operation_results(aapl, &results, 0);
        }

        ms_sleep(avago_max_dwell_time);

        AAPL_SUPPRESS_ERRORS_PUSH(aapl);
        rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_BIST_STOP_N_CHECK, &results, 0x10, avago_max_default_timeout, 1);
        AAPL_SUPPRESS_ERRORS_POP(aapl);

        if (rc != 0) {
          aapl->return_code -= 1;
          avago_max_print_operation_results(aapl, &results, 0);
          avago_max_print_spare_results(aapl, sbus_addr, 1);
          avago_max_print_rx_bist_results(aapl, sbus_addr, 0x10, 0);
        }
      }
    }
  }

  avago_max_set_parameter(aapl, sbus_addr, AVAGO_MAX_CONFIG_DBI_ENABLE, 1);

  return aapl->return_code;
}


/**
 ** @brief   Runs inter-max bi-directional mmt diagnostics
 ** @return  0
 **/
int avago_max_bist_bidir_diagnostics(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    Avago_addr_t *addr,     /**< [in] SBus slice (broadcast) address. */
    Avago_addr_t *start,    /**< [out] Start of hardware address range. */
    Avago_addr_t *stop,     /**< [out] End of hardware address range. */
    Avago_addr_t *first,    /**< [out] First hardware address. */
    uint flags)             /**< [in] Flags to control iteration. */
{
  int rc;
  Avago_max_operation_results_t results;
  uint pattern_index;
  uint pattern_list[2];

  BOOL st;

  (void)flags;

  pattern_list[0] = 1;
  pattern_list[1] = 8;

  for(pattern_index=0; pattern_index<=1; pattern_index++) {
    const char * pattern_name;
    uint pattern = pattern_list[pattern_index];

    switch(pattern) {
      case 0x00: { pattern_name = "user";                  break; }
      case 0x01: { pattern_name = "bist_prbs_pattern";     break; }
      case 0x02: { pattern_name = "bist_f1_50t_pattern";   break; }
      case 0x03: { pattern_name = "bist_f1_100t_pattern";  break; }
      case 0x04: { pattern_name = "bist_si0_pattern";      break; }
      case 0x05: { pattern_name = "bist_si1_pattern";      break; }
      case 0x06: { pattern_name = "bist_si2_pattern";      break; }
      case 0x07: { pattern_name = "bist_si3_pattern";      break; }
      case 0x08: { pattern_name = "bist_f4_100t_pattern";  break; }
      default:   { pattern_name = "unknown"; }
    }


    for (st = aapl_broadcast_first(aapl, addr, start, stop, first, 0); st; st = aapl_broadcast_next(aapl, first, start, stop, 0))
    {
      uint sbus_addr = avago_struct_to_addr(first);
      if (!(aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_MAX))) { continue; }
      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Starting bist_mmt_freerun pattern:%s for Sbus address %s\n", pattern_name, aapl_addr_to_str(sbus_addr));
      rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_BIST_START_FREERUN, &results, 0x10, avago_max_default_timeout, 1);
      if (rc != 0) {
        avago_max_print_operation_results(aapl, &results, 0);
      }
    }

    ms_sleep(avago_max_dwell_time);


    for (st = aapl_broadcast_first(aapl, addr, start, stop, first, 0); st; st = aapl_broadcast_next(aapl, first, start, stop, 0))
    {
      uint sbus_addr = avago_struct_to_addr(first);
      if (!(aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, FALSE, 1, AVAGO_MAX))) { continue; }

      aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "Checking bist results for Sbus address %s\n", aapl_addr_to_str(sbus_addr));
      AAPL_SUPPRESS_ERRORS_PUSH(aapl);
      rc = avago_max_launch_channel_operation_base(aapl, sbus_addr, AVAGO_MAX_OP_BIST_STOP_N_CHECK, &results, 0x10, avago_max_default_timeout, 1);
      AAPL_SUPPRESS_ERRORS_POP(aapl);

      if (rc != 0) {
        aapl->return_code -= 1;
        avago_max_print_operation_results(aapl, &results, 0);
        avago_max_print_spare_results(aapl, sbus_addr, 1);
        avago_max_print_rx_bist_results(aapl, sbus_addr, 0x10, 1);
      }
    }
  }

  return aapl->return_code;
}


/**
 ** @brief  Prints all firmware parameters
 **
 ** @param  aapl         Aapl_t struct
 ** @param  sbus_addr sbus_address of the sbus master spico or APC
 **
 ** @return  On error, decrements aapl->return_code and returns -1.
 **/
int avago_max_parameter_diagnostics(Aapl_t *aapl, uint sbus_addr)
{
  int i;
  uint spico_addr = avago_max_get_spico_address(aapl, sbus_addr);

  for(i=0; i<=0x30; i++) {
    uint value = avago_max_get_parameter(aapl, spico_addr, (Avago_max_parameter_t)i);
    aapl_log_printf(aapl, AVAGO_INFO, 0, 1, "offset:0x%02x value:0x%x\n", i, value);
  }
  return 0;
}


#endif

/** @} */
