/* $Id: soc_petra_packet.c,v 1.6 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>


#include <soc/dpp/Petra/petra_api_packet.h>
#include <soc/dpp/Petra/petra_packet.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


/* $Id: soc_petra_packet.c,v 1.6 Broadcom SDK $
 * Access to the buffer FIFO is in 256bit=32byte fragments.
 * The CPU writes a 256b fragment, or less if end of packet,
 * to a register buffer.
 */
#define SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE (32)
/*
 * command value that indicates that this burst is start of new packet
 */
/*
 * command value that indicates that this burst is the middle of the packet
 */
/*
 * command value that indicates that this burst is the end of the packet
 * and  TWO byte of it is valid
 */
#define SOC_PETRA_PKT_CPU_RX_CMD_END_2B_VALID (3)
/*
 * command value that indicates that this burst is the end of the packet
 * and only one byte of it is valid
 */
#define SOC_PETRA_PKT_CPU_RX_CMD_END_1B_VALID (4)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PETRA_PKT_UPDATE_RECV_INDX(read_indx)   \
  (Soc_petra_pkt_packet_recv_to_msb)?(--read_indx):(++read_indx)
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
SOC_PETRA_PKT_PACKET_TRANSFER
  Soc_petra_pkt_packet_transfer;
static uint8
  Soc_petra_pkt_packet_recv_to_msb = FALSE;


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     soc_petra_pkt_init
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
  soc_petra_pkt_init(
    SOC_SAND_IN  int                 unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_INIT);

  /*
   *  Default values to the callback for the packet send and receive functions
   */
  soc_petra_PETRA_PKT_PACKET_TRANSFER_clear(&Soc_petra_pkt_packet_transfer);
  Soc_petra_pkt_packet_recv_to_msb = FALSE;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_init()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_CALLBACK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(packet_transfer);

  if (packet_transfer->packet_send)
  {
    Soc_petra_pkt_packet_transfer.packet_send = packet_transfer->packet_send;
  }
  else
  {
    Soc_petra_pkt_packet_transfer.packet_send = soc_petra_pkt_async_interface_packet_send;
  }
  if (packet_transfer->packet_recv)
  {
    Soc_petra_pkt_packet_transfer.packet_recv = packet_transfer->packet_recv;
  }
  else
  {
    Soc_petra_pkt_packet_transfer.packet_recv = soc_petra_pkt_async_interface_packet_recv;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_callback_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_CALLBACK_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(packet_transfer);

  SOC_SAND_MAGIC_NUM_VERIFY(packet_transfer);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_callback_verify()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_CALLBACK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(packet_transfer);

  packet_transfer->packet_send = Soc_petra_pkt_packet_transfer.packet_send;
  packet_transfer->packet_recv = Soc_petra_pkt_packet_transfer.packet_recv;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_callback_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure transmits a packet to the destination
*     FAP for an asynchronous interface.
*********************************************************************/
uint32
  soc_petra_pkt_async_interface_packet_send(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO     *tx_packet
  )
{
  uint32
    res,
    fld_val,
    fragment[SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE / sizeof(uint32)],
    cpu_pkt_ctrl,
    bytes_to_write;
  SOC_SAND_IN uint8
    *frag_lsb,
    *paylod_indx,
    *frag_msb;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    fragment_i,
    frag_indx,
    bytes_count,
    nof_fragments;
  SOC_SAND_IN uint8
    *payload;
  uint32
    payload_byte_size;
  uint8
    is_low_sim_active;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_ASYNC_INTERFACE_PACKET_SEND);

  SOC_SAND_CHECK_NULL_INPUT(tx_packet);

  regs = soc_petra_regs();

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

#ifdef LINK_PA_LIBRARIES
  /*
   *  Verify the asynchronous mode is set
   */
  if SOC_PETRA_IS_DEV_PETRA_A
  {
    SOC_PA_FLD_GET(regs->ire.cpu_interface_select_non_streaming_packet_path_reg.cpu_if_sel_non_stream_pkt, fld_val, 10 , exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PKT_ASYNC_MODE_CONFIG_ERR, 12, exit);
    }
  }
#endif

  payload_byte_size = tx_packet->packet.data_byte_size;
  payload = tx_packet->packet.data;

  nof_fragments = SOC_SAND_DIV_ROUND_UP(payload_byte_size, SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE);
  frag_msb = payload + SOC_PETRA_PKT_MAX_CPU_PACKET_BYTE_SIZE - 1;
  for(fragment_i = 0; fragment_i < nof_fragments; ++fragment_i, frag_msb-= SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE)
  {
    res = SOC_SAND_OK; sal_memset(
            fragment,
            0x0,
            SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE
         );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

    if (payload_byte_size >= (fragment_i + 1) * SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE)
    {
      frag_lsb = frag_msb - SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE + 1;
    }
    else
    {
      frag_lsb =  payload + SOC_PETRA_PKT_MAX_CPU_PACKET_BYTE_SIZE - payload_byte_size;
    }

    bytes_to_write = frag_msb - frag_lsb + 1;

    frag_indx = SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE / sizeof(uint32) - 1;
    bytes_count = 0;
    if (tx_packet->path_type == SOC_PETRA_PACKET_SEND_PATH_TYPE_INGRESS )
    {
      for (paylod_indx = frag_msb; paylod_indx >= frag_lsb; --paylod_indx)
      {
        fragment[frag_indx] <<= 8;
        fragment[frag_indx] |=  *paylod_indx;
        bytes_count++;
        if (bytes_count >= 4)
        {
          bytes_count = 0;
          --frag_indx;
        }
      }
      if (bytes_count != 0)
      {
        fragment[frag_indx] <<= 8*(4-bytes_count);
      }
      
    }
    else
    {
      frag_indx = 0;
      for (paylod_indx = frag_msb; paylod_indx >= frag_lsb; --paylod_indx)
      {
        fragment[frag_indx] <<= 8;
        fragment[frag_indx] |=  *paylod_indx;
        bytes_count++;
        if (bytes_count >= 4)
        {
          bytes_count = 0;
          ++frag_indx;
        }
      }
    }
    /*
     * Copy the data.
     */
    if (tx_packet->path_type == SOC_PETRA_PACKET_SEND_PATH_TYPE_INGRESS )
    {
      if (!is_low_sim_active)
      {
        ret = soc_sand_trigger_verify_0(
                unit,
                SOC_PETRA_REG_DB_ACC(regs->ire.cpu_interface_packet_control_reg.addr.base),
                SOC_PETRA_PKT_PACKET_SEND_TIMEOUT,
                SOC_PETRA_REG_DB_ACC(regs->ire.cpu_interface_packet_control_reg.cpu_if_pkt_send_data.lsb)
              );
        SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);
      }

      res = soc_petra_write_reg_buffer_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->ire.cpu_interface_packet_data_reg_0.addr),
              SOC_PETRA_DEFAULT_INSTANCE,
              SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE / sizeof(uint32),
              fragment
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      ret = soc_sand_trigger_verify_0(
              unit,
              SOC_PETRA_REG_DB_ACC(regs->egq.cpu_packet_control_reg.addr.base),
              SOC_PETRA_PKT_PACKET_SEND_TIMEOUT,
              SOC_PETRA_REG_DB_ACC(regs->egq.cpu_packet_control_reg.send_pkt_frag.lsb)
            );
      SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

      res = soc_petra_write_reg_buffer_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->egq.cpu_packet_fragmentation_words_reg[0].addr),
              SOC_PETRA_DEFAULT_INSTANCE,
              SOC_PETRA_CPU_TX_FRAG_BYTE_SIZE / sizeof(uint32),
              fragment
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    cpu_pkt_ctrl = 1 ;
    if(0 == fragment_i)
    {
      /*
       * Start of packet
       */
      cpu_pkt_ctrl |= 2 ;
    }
    if( (nof_fragments - 1) == fragment_i)
    {
      /*
       * End of packet
       */
      cpu_pkt_ctrl |= 4 ;
      if (tx_packet->path_type == SOC_PETRA_PACKET_SEND_PATH_TYPE_INGRESS )
      {
        cpu_pkt_ctrl |=
          SOC_SAND_SET_BITS_RANGE(
            bytes_to_write - 1,
            SOC_PETRA_REG_DB_ACC(regs->ire.cpu_interface_packet_control_reg.cpu_if_pkt_be.msb),
            SOC_PETRA_REG_DB_ACC(regs->ire.cpu_interface_packet_control_reg.cpu_if_pkt_be.lsb)
          );
      }
    }
    /*
     * Set the trigger.
     */
    if (tx_packet->path_type == SOC_PETRA_PACKET_SEND_PATH_TYPE_INGRESS )
    {
      SOC_PETRA_REG_SET(regs->ire.cpu_interface_packet_control_reg, cpu_pkt_ctrl, 30, exit);
    }
    else
    {
      SOC_PETRA_REG_SET(regs->egq.cpu_packet_control_reg, cpu_pkt_ctrl, 35, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_async_interface_packet_send()",0,0);
}

/*********************************************************************
*     This procedure transmits a packet to the destination
*     FAP. The packet that gets sent is unicast packet or
*     fabric multicast. The packet is sent directly to the FAP
*     Fabric interface, and is aimed to the other FAP CPU
*     interface, that is, no Queuing or Scheduling is
*     involved.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_pkt_packet_send_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO     *tx_packet
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_SEND_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tx_packet);
  SOC_SAND_CHECK_NULL_INPUT(Soc_petra_pkt_packet_transfer.packet_send);

  res = Soc_petra_pkt_packet_transfer.packet_send(
                 unit,
                 tx_packet
               ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_send_unsafe()",0,0);
}

/*********************************************************************
*     This procedure receives one packet from the device CPU
*     interface for an asynchronous interface.
*********************************************************************/
uint32
  soc_petra_pkt_async_interface_packet_recv(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       bytes_to_get,
    SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO       *rx_packet
  )
{
  uint32
    reg_val,
    fld_val,
    tdv,
    tdata,
    tcmnd,
    nof_bytes,
    read_indx=0;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_ASYNC_INTERFACE_PACKET_RECV);

  SOC_SAND_CHECK_NULL_INPUT(rx_packet);
  regs = soc_petra_regs();

#ifdef LINK_PA_LIBRARIES
  /*
   *  Verify the asynchronous mode is set (only in Soc_petra-A)
   */
  if SOC_PETRA_IS_DEV_PETRA_A
  {
    SOC_PA_FLD_GET(regs->ire.cpu_interface_select_non_streaming_packet_path_reg.cpu_if_sel_non_stream_pkt, fld_val, 10 , exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PKT_ASYNC_MODE_CONFIG_ERR, 12, exit);
    }
  }
#endif
  /*
   * Initialize the output
   */
  rx_packet->packet.data_byte_size = 0 ;

  SOC_PETRA_REG_GET(regs->eci.cpu_asynchronous_packet_data_reg, reg_val, 10, exit);

  /*
  * Read one packet from the device.
  */
  tdv =
    SOC_SAND_GET_BITS_RANGE(
      reg_val,
      SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tdv.msb),
      SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tdv.lsb)
    );

  /*
   * Note: In Multi-Port mode, the first word of burst contains burst attributes.
   */
  while (tdv)
  {

   /*
    * check which part of the packet this burst (first,middle,end)
    */
    tcmnd =
      SOC_SAND_GET_BITS_RANGE(
        reg_val,
        SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tcmd.msb),
        SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tcmd.lsb)
      );

   /*
    * if reached the required size, empty the rest till end of packet.
    */
    if (rx_packet->packet.data_byte_size < bytes_to_get)
    {
      tdata =
        SOC_SAND_GET_BITS_RANGE(
          reg_val,
          SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tdata.msb),
          SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tdata.lsb)
        );

      if (tcmnd == SOC_PETRA_PKT_CPU_RX_CMD_END_1B_VALID)
      {
        nof_bytes = 1;
      }
      else
      {
        nof_bytes = 2;
      }

      /*
       * if only one byte is needed.
       */
      if (rx_packet->packet.data_byte_size + 1 == bytes_to_get)
      {
        nof_bytes = 1;
      }

      if (Soc_petra_pkt_packet_recv_to_msb)
      {
        read_indx = SOC_PETRA_PKT_MAX_CPU_PACKET_BYTE_SIZE - rx_packet->packet.data_byte_size - 1;
      }
      else
      {
        read_indx = rx_packet->packet.data_byte_size;
      }

      rx_packet->packet.data_byte_size += nof_bytes;
      rx_packet->packet.data[read_indx] =
        (uint8)SOC_SAND_GET_BITS_RANGE(
          tdata,
          2 * SOC_SAND_NOF_BITS_IN_CHAR - 1,
          SOC_SAND_NOF_BITS_IN_CHAR
        );

      SOC_PETRA_PKT_UPDATE_RECV_INDX(read_indx);

      if (nof_bytes == 2)
      {
        rx_packet->packet.data[read_indx] =
          (uint8)SOC_SAND_GET_BITS_RANGE(
            tdata,
            SOC_SAND_NOF_BITS_IN_CHAR - 1,
            0
          );
        SOC_PETRA_PKT_UPDATE_RECV_INDX(read_indx);
      }
    }
   /*
    * if reached the end of the packet.
    */
    if (tcmnd == SOC_PETRA_PKT_CPU_RX_CMD_END_1B_VALID || tcmnd == SOC_PETRA_PKT_CPU_RX_CMD_END_2B_VALID)
    {
      break;
    }

    SOC_PETRA_REG_GET(regs->eci.cpu_asynchronous_packet_data_reg, reg_val, 10, exit);
   /*
    * Read one packet from the device.
    */
    tdv =
      SOC_SAND_GET_BITS_RANGE(
        reg_val,
        SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tdv.msb),
        SOC_PETRA_REG_DB_ACC(regs->eci.cpu_asynchronous_packet_data_reg.tdv.lsb)
      );
  }
  /*
   * If rx_packet->packet.data_byte_size then No packet was found
   */


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_async_interface_packet_recv()",0,0);
}



/*********************************************************************
*     This procedure receives one packet from the device CPU
*     interface. This procedure reads the packet regardless of
*     its origin. That is, if it was sent from
*     soc_petra_packet_send_packet, or any other method.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_pkt_packet_recv_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       bytes_to_get,
    SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO       *rx_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECV_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rx_packet);
  SOC_SAND_CHECK_NULL_INPUT(Soc_petra_pkt_packet_transfer.packet_recv);

  res = Soc_petra_pkt_packet_transfer.packet_recv(
          unit,
          bytes_to_get,
          rx_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_recv_unsafe()",0,0);
}

uint32
  soc_petra_pkt_packet_receive_mode_set_unsafe(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE  recv_mode
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECEIVE_MODE_SET_UNSAFE);

  Soc_petra_pkt_packet_recv_to_msb = SOC_SAND_NUM2BOOL(recv_mode == SOC_PETRA_PKT_PACKET_RECV_MODE_MSB_TO_LSB);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_receive_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the receive_mode
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_receive_mode_verify(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE  recv_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECEIVE_MODE_VERIFY);

  if (recv_mode >= SOC_PETRA_PACKET_SEND_NOF_RECV_MODES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FLD_OUT_OF_RANGE, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_receive_mode_verify()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the receive_mode
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_receive_mode_get_unsafe(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_RECV_MODE  *recv_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECEIVE_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(recv_mode);

  if (Soc_petra_pkt_packet_recv_to_msb)
  {
    *recv_mode = SOC_PETRA_PKT_PACKET_RECV_MODE_MSB_TO_LSB;
  }
  else
  {
    *recv_mode = SOC_PETRA_PKT_PACKET_RECV_MODE_LSB_TO_MSB;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_receive_mode_get_unsafe()",0,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
