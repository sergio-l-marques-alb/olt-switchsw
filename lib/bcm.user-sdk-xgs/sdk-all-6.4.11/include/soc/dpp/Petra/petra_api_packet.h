/* $Id: soc_petra_api_packet.h,v 1.5 Broadcom SDK $
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
*/

#ifndef __SOC_PETRA_API_PACKET_INCLUDED__
/* { */
#define __SOC_PETRA_API_PACKET_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_packet.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_api_packet.h,v 1.5 Broadcom SDK $
 * Minimum size of sent packets in bytes
 */
#define SOC_PETRA_PKT_MIN_TX_CPU_PACKET_BYTE_SIZE        (32)
/*
 * Minimum size of received packets in bytes
 */
/*     Maximum size of received/sent packets in bytes          */
#define  SOC_PETRA_PKT_MAX_CPU_PACKET_BYTE_SIZE (SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE)


#define SOC_PETRA_PKT_PACKET_SEND_TIMEOUT                (100)

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

#define SOC_PETRA_PACKET_SEND_PATH_TYPE_INGRESS                SOC_TMC_PACKET_SEND_PATH_TYPE_INGRESS
#define SOC_PETRA_PACKET_SEND_PATH_TYPE_EGRESS                 SOC_TMC_PACKET_SEND_PATH_TYPE_EGRESS
typedef SOC_TMC_PACKET_SEND_PATH_TYPE                          SOC_PETRA_PACKET_SEND_PATH_TYPE;

#define SOC_PETRA_PKT_PACKET_RECV_MODE_MSB_TO_LSB              SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB
#define SOC_PETRA_PKT_PACKET_RECV_MODE_LSB_TO_MSB              SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB
#define SOC_PETRA_PACKET_SEND_NOF_RECV_MODES                     SOC_TMC_PACKET_SEND_NOF_RECV_MODES
typedef SOC_TMC_PKT_PACKET_RECV_MODE                           SOC_PETRA_PKT_PACKET_RECV_MODE;

typedef SOC_TMC_PKT_PACKET_BUFFER                              SOC_PETRA_PKT_PACKET_BUFFER;
typedef SOC_TMC_PKT_TX_PACKET_INFO                             SOC_PETRA_PKT_TX_PACKET_INFO;
typedef SOC_TMC_PKT_RX_PACKET_INFO                             SOC_PETRA_PKT_RX_PACKET_INFO;
typedef SOC_TMC_PKT_PACKET_TRANSFER                            SOC_PETRA_PKT_PACKET_TRANSFER;

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
typedef
  uint32
    (*SOC_PETRA_PKT_PACKET_SEND_TRANSFER_PTR)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO     *tx_packet
   );

typedef
  uint32
    (*SOC_PETRA_PKT_PACKET_RECV_TRANSFER_PTR)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                     bytes_to_get,
      SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO     *rx_packet
    );


/*********************************************************************
* NAME:
*     soc_petra_pkt_packet_callback_set
* TYPE:
*   PROC
* FUNCTION:
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer -
*     Pointer to the callback functions to link.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  );

/*********************************************************************
* NAME:
*     soc_petra_pkt_packet_callback_get
* TYPE:
*   PROC
* FUNCTION:
*     This procedure allows the definition of the callback
*     function for the soc_petra_pkt_packet_send functionality.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer -
*     Pointer to the callback functions to link.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_pkt_packet_callback_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_TRANSFER *packet_transfer
  );

/*********************************************************************
* NAME:
*     soc_petra_pkt_packet_send
* TYPE:
*   PROC
* DATE:
*   Nov  1 2007
* FUNCTION:
*     This procedure transmits a packet to the Soc_petra
*     The packet is sent directly to the FAP
*     Fabric interface, and is aimed to the other FAP CPU
*     interface, that is, no Queuing or Scheduling is
*     involved.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO          *packet -
*     packet sending info.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. use soc_petra_hpu_itmh_build and soc_petra_pp_ethernet_header_build
*   in order to build the packet buffer.
*   2. This API is only valid when using asynchronous CPU interface.
*   When the Soc_petra device is accessed via the streaming interface,
*   the packet send/receive functionality needs to be implemented
*   using this interface.
*********************************************************************/
uint32
  soc_petra_pkt_packet_send(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_PKT_TX_PACKET_INFO     *tx_packet
  );

/*********************************************************************
* NAME:
*     soc_petra_pkt_packet_recv
* TYPE:
*   PROC
* DATE:
*   Nov  1 2007
* FUNCTION:
*     This procedure receives one packet from the device CPU
*     interface. This procedure reads the packet regardless of
*     its origin. That is, if it was sent from
*     soc_petra_packet_send_packet, or any other method.
* INPUT:
*  SOC_SAND_IN  uint32                     bytes_to_get, -
*     Number of bytes to read from the packet.
*     if the size of the packet to read is smaller than
*     this size then whole packet will be read
*  SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO     *rx_packet -
*     Pointer to packet structure to load received packet and
*     related information. Note, when the number of bytes is
*     'zero', no packet has been found.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This API is only valid when using asynchronous CPU interface.
*   When the Soc_petra device is accessed via the streaming interface,
*   the packet send/receive functionality needs to be implemented
*   using this interface.
*********************************************************************/
uint32
  soc_petra_pkt_packet_recv(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     bytes_to_get,
    SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO     *rx_packet
  );

/*********************************************************************
* NAME:
*     soc_petra_pkt_packet_receive_mode_set
* TYPE:
*   PROC
* FUNCTION:
*     This procedure allows the definition of the receive_mode,
*     The mode to write the received packet to the buffer
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE  recv_mode -
*     the mode to copy the packet to the buffer
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_pkt_packet_receive_mode_set(
    SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE  recv_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_pkt_packet_receive_mode_get
* TYPE:
*   PROC
* FUNCTION:
*     This procedure allows the definition of the receive_mode,
*     The mode to write the received packet to the buffer
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_PKT_PACKET_RECV_MODE  recv_mode -
*     the mode to copy the packet to the buffer
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_pkt_packet_receive_mode_get(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_RECV_MODE  *recv_mode
  );


void
  soc_petra_PETRA_PKT_TX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_TX_PACKET_INFO *info
  );


void
  soc_petra_PETRA_PKT_RX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_RX_PACKET_INFO *info
  );

void
  soc_petra_PETRA_PKT_PACKET_BUFFER_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_BUFFER *info
  );

void
  soc_petra_PETRA_PKT_PACKET_TRANSFER_clear(
    SOC_SAND_OUT SOC_PETRA_PKT_PACKET_TRANSFER *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

void
  soc_petra_PETRA_PKT_TX_PACKET_INFO_print(
    SOC_SAND_IN SOC_PETRA_PKT_TX_PACKET_INFO *info
  );

void
  soc_petra_PETRA_PKT_RX_PACKET_INFO_print(
    SOC_SAND_IN SOC_PETRA_PKT_RX_PACKET_INFO *info
  );

void
  soc_petra_PETRA_PKT_PACKET_BUFFER_print(
    SOC_SAND_IN SOC_PETRA_PKT_PACKET_BUFFER *info
  );


void
  soc_petra_PETRA_PKT_PACKET_TRANSFER_print(
    SOC_SAND_IN SOC_PETRA_PKT_PACKET_TRANSFER *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_PACKET_INCLUDED__*/
#endif
