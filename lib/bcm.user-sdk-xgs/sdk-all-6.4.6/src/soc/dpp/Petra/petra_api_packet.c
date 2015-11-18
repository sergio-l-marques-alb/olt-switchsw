/* $Id: petra_api_packet.c,v 1.7 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/petra_packet.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_packet.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_CALLBACK_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(packet_transfer);

  res = soc_petra_pkt_packet_callback_verify(
          unit,
          packet_transfer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_pkt_packet_callback_set_unsafe(
          unit,
          packet_transfer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_callback_set()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_CALLBACK_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(packet_transfer);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_pkt_packet_callback_get_unsafe(
          unit,
          packet_transfer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_callback_get()",0,0);
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
  soc_petra_pkt_packet_send(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO      *tx_packet
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_SEND);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tx_packet);

  /*
   * Check input parameters
   */
  if(
     (tx_packet->packet.data_byte_size > SOC_PETRA_PKT_MAX_CPU_PACKET_BYTE_SIZE) ||
     (tx_packet->packet.data_byte_size < SOC_PETRA_PKT_MIN_TX_CPU_PACKET_BYTE_SIZE)
    )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PKT_TX_CPU_PACKET_BYTE_SIZE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_pkt_packet_send_unsafe(
          unit,
          tx_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_send()",0,0);
}

/*********************************************************************
*     This procedure receives one packet from the device CPU
*     interface. This procedure reads the packet regardless of
*     its origin. That is, if it was sent from
*     soc_petra_packet_send_packet, or any other method.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_pkt_packet_recv(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     bytes_to_get,
    SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO     *rx_packet
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECV);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rx_packet);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_pkt_packet_recv_unsafe(
          unit,
          bytes_to_get,
          rx_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_recv()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the receive_mode
*     The mode to write the received packet to the buffer
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_receive_mode_set(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE  recv_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECEIVE_MODE_SET);

  res = soc_petra_pkt_packet_receive_mode_verify(
          recv_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_pkt_packet_receive_mode_set_unsafe(
          recv_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_receive_mode_set()",0,0);
}

/*********************************************************************
*     This procedure allows the definition of the receive_mode
*     The mode to write the received packet to the buffer
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_pkt_packet_receive_mode_get(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_RECV_MODE  *recv_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PKT_PACKET_RECEIVE_MODE_GET);

  SOC_SAND_CHECK_NULL_INPUT(recv_mode);
  res = soc_petra_pkt_packet_receive_mode_get_unsafe(
          recv_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_pkt_packet_receive_mode_get()",0,0);
}

void
  soc_petra_PETRA_PKT_PACKET_BUFFER_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_PACKET_BUFFER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PKT_TX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_TX_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_TX_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PKT_RX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_RX_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PKT_PACKET_TRANSFER_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_TRANSFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_PACKET_TRANSFER_clear(info);
  info->packet_send = soc_petra_pkt_async_interface_packet_send;
  info->packet_recv = soc_petra_pkt_async_interface_packet_recv;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_PACKET_SEND_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_PACKET_SEND_PATH_TYPE enum_val
  )
{
  return SOC_TMC_PACKET_SEND_PATH_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_PKT_PACKET_RECV_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE enum_val
  )
{
  return SOC_TMC_PKT_PACKET_RECV_MODE_to_string(enum_val);
}

void
  soc_petra_PETRA_PKT_PACKET_BUFFER_print(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_BUFFER *info
  )
{
  uint32
    res;
  SOC_TMC_PKT_PACKET_RECV_MODE
    recv_to_msb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_pkt_packet_receive_mode_get_unsafe(
        &recv_to_msb
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_TMC_PKT_PACKET_BUFFER_print(info, recv_to_msb);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PKT_TX_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO *info
  )
{
  uint32
    res;
  SOC_TMC_PKT_PACKET_RECV_MODE
    recv_to_msb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_pkt_packet_receive_mode_get_unsafe(
        &recv_to_msb
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_TMC_PKT_TX_PACKET_INFO_print(info, recv_to_msb);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PKT_RX_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PETRA_PKT_RX_PACKET_INFO *info
  )
{
  uint32
    res;
  SOC_TMC_PKT_PACKET_RECV_MODE
    recv_to_msb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_pkt_packet_receive_mode_get_unsafe(
        &recv_to_msb
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_TMC_PKT_RX_PACKET_INFO_print(info, recv_to_msb);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_PKT_PACKET_TRANSFER_print(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_TRANSFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_PACKET_TRANSFER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

