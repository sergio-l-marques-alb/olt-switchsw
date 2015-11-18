/* $Id$
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
*/

#ifndef __SOC_TMC_API_PACKET_INCLUDED__
/* { */
#define __SOC_TMC_API_PACKET_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id$
 * Minimum size of sent packets in bytes
 */
/*
 * Minimum size of received packets in bytes
 */
/*
 * Maximum size of received/sent packets in bytes
 */
#define SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE        (10 * 1024)


/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
 /*
  *  This is the "normal" packet sending path:
  *  packet pass through the soc_petra starting from the ingress.
  *  this packet may contain
  *   - payload only (for RAW ports)
  *   - ITMH header and payload (for TM ports)
  *       - refer to soc_tmchpu_itmh_build in order to build  ITMH header.
  *   - Ethernet Header (with or without VLAN Tag) (for Ethernet ports).
  *       - refer to soc_tmcpp_ethernet_header_build in order to build the Eth Header.
  */
  SOC_TMC_PACKET_SEND_PATH_TYPE_INGRESS=0,
 /*
  *  This is the "reduced" packet sending path:
  *  packet pass through the soc_petra starting from the egress.
  *  this packet may contain
  *   - packets transmitted through this path are in fabric format (FTMH)
  *     and may be transmitted out to any port type (RAW, TM, Ethernet).
  */
  SOC_TMC_PACKET_SEND_PATH_TYPE_EGRESS=1,

  SOC_TMC_PACKET_SEND_NOF_PATH_TYPES=2
}SOC_TMC_PACKET_SEND_PATH_TYPE;

typedef enum
{
 /*
  * copy packets received by CPU to buffer starting from MSB byte
  * downward to LSB bytes
  */
  SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB=0,
 /*
  * copy packets received by CPU to buffer starting from LSB byte
  * upward to MSB bytes
  */
  SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB=1,

  SOC_TMC_PACKET_SEND_NOF_RECV_MODES=2
}SOC_TMC_PKT_PACKET_RECV_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  10K byte Data buffer to carry on packet
   */
  uint8 data[SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE];
  /*
   *  Number of bytes in packet
   */
  uint32 data_byte_size;

}SOC_TMC_PKT_PACKET_BUFFER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  *  Buffer to include packet to send. With an asynchronous
  *  interface, the buffer is sent starting from the buffer MSB.
  */
  SOC_TMC_PKT_PACKET_BUFFER packet;
 /*
  *  Packet sending path
  */
  SOC_TMC_PACKET_SEND_PATH_TYPE path_type;

}SOC_TMC_PKT_TX_PACKET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  *  Buffer to include packet to receive
  */
  SOC_TMC_PKT_PACKET_BUFFER packet;

}SOC_TMC_PKT_RX_PACKET_INFO;

typedef
  uint32
    (*SOC_TMC_PKT_PACKET_SEND_TRANSFER_PTR)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_TMC_PKT_TX_PACKET_INFO     *tx_packet
   );

typedef
  uint32
    (*SOC_TMC_PKT_PACKET_RECV_TRANSFER_PTR)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                     bytes_to_get,
      SOC_SAND_OUT SOC_TMC_PKT_RX_PACKET_INFO     *rx_packet
    );

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
  * Send function pointer.
  */
  SOC_TMC_PKT_PACKET_SEND_TRANSFER_PTR packet_send;

  /*
  * Receive function pointer.
  */
  SOC_TMC_PKT_PACKET_RECV_TRANSFER_PTR  packet_recv;
} SOC_TMC_PKT_PACKET_TRANSFER;

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
void
  SOC_TMC_PKT_PACKET_BUFFER_clear(
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_BUFFER *info
  );

void
  SOC_TMC_PKT_TX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PKT_TX_PACKET_INFO *info
  );

void
  SOC_TMC_PKT_RX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PKT_RX_PACKET_INFO *info
  );

void
  SOC_TMC_PKT_PACKET_TRANSFER_clear(
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_TRANSFER *info
  );

#if SOC_TMC_DEBUG_IS_LVL1
const char*
  SOC_TMC_PACKET_SEND_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PACKET_SEND_PATH_TYPE enum_val
  );

const char*
  SOC_TMC_PKT_PACKET_RECV_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_PKT_PACKET_RECV_MODE enum_val
  );

void
  SOC_TMC_PKT_PACKET_BUFFER_print(
    SOC_SAND_IN SOC_TMC_PKT_PACKET_BUFFER *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_TMC_PKT_TX_PACKET_INFO_print(
    SOC_SAND_IN SOC_TMC_PKT_TX_PACKET_INFO *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_TMC_PKT_RX_PACKET_INFO_print(
    SOC_SAND_IN SOC_TMC_PKT_RX_PACKET_INFO *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_TMC_PKT_PACKET_TRANSFER_print(
    SOC_SAND_IN SOC_TMC_PKT_PACKET_TRANSFER *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_PACKET_INCLUDED__*/
#endif
