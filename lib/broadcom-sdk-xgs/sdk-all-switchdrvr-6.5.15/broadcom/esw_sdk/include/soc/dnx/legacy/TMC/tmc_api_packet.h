/* $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __SOC_DNX_API_PACKET_INCLUDED__
/* { */
#define __SOC_DNX_API_PACKET_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

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
#define SOC_DNX_PKT_MAX_CPU_PACKET_BYTE_SIZE        (10 * 1024)


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
  *       - refer to soc_jer2_jer2_jer2_tmchpu_itmh_build in order to build  ITMH header.
  *   - Ethernet Header (with or without VLAN Tag) (for Ethernet ports).
  *       - refer to soc_jer2_jer2_jer2_tmcpp_ethernet_header_build in order to build the Eth Header.
  */
  SOC_DNX_PACKET_SEND_PATH_TYPE_INGRESS=0,
 /*
  *  This is the "reduced" packet sending path:
  *  packet pass through the soc_petra starting from the egress.
  *  this packet may contain
  *   - packets transmitted through this path are in fabric format (FTMH)
  *     and may be transmitted out to any port type (RAW, TM, Ethernet).
  */
  SOC_DNX_PACKET_SEND_PATH_TYPE_EGRESS=1,

  SOC_DNX_PACKET_SEND_NOF_PATH_TYPES=2
}SOC_DNX_PACKET_SEND_PATH_TYPE;

typedef enum
{
 /*
  * copy packets received by CPU to buffer starting from MSB byte
  * downward to LSB bytes
  */
  SOC_DNX_PKT_PACKET_RECV_MODE_MSB_TO_LSB=0,
 /*
  * copy packets received by CPU to buffer starting from LSB byte
  * upward to MSB bytes
  */
  SOC_DNX_PKT_PACKET_RECV_MODE_LSB_TO_MSB=1,

  SOC_DNX_PACKET_SEND_NOF_RECV_MODES=2
}SOC_DNX_PKT_PACKET_RECV_MODE;

typedef struct
{
  /*
   *  10K byte Data buffer to carry on packet
   */
  uint8 data[SOC_DNX_PKT_MAX_CPU_PACKET_BYTE_SIZE];
  /*
   *  Number of bytes in packet
   */
  uint32 data_byte_size;

}SOC_DNX_PKT_PACKET_BUFFER;

typedef struct
{
 /*
  *  Buffer to include packet to send. With an asynchronous
  *  interface, the buffer is sent starting from the buffer MSB.
  */
  SOC_DNX_PKT_PACKET_BUFFER packet;
 /*
  *  Packet sending path
  */
  SOC_DNX_PACKET_SEND_PATH_TYPE path_type;

}SOC_DNX_PKT_TX_PACKET_INFO;

typedef struct
{
 /*
  *  Buffer to include packet to receive
  */
  SOC_DNX_PKT_PACKET_BUFFER packet;

}SOC_DNX_PKT_RX_PACKET_INFO;

typedef
  uint32
    (*SOC_DNX_PKT_PACKET_SEND_TRANSFER_PTR)(
      DNX_SAND_IN  int                    unit,
      DNX_SAND_IN  SOC_DNX_PKT_TX_PACKET_INFO     *tx_packet
   );

typedef
  uint32
    (*SOC_DNX_PKT_PACKET_RECV_TRANSFER_PTR)(
      DNX_SAND_IN  int                    unit,
      DNX_SAND_IN  uint32                     bytes_to_get,
      DNX_SAND_OUT SOC_DNX_PKT_RX_PACKET_INFO     *rx_packet
    );

typedef struct
{
  /*
  * Send function pointer.
  */
  SOC_DNX_PKT_PACKET_SEND_TRANSFER_PTR packet_send;

  /*
  * Receive function pointer.
  */
  SOC_DNX_PKT_PACKET_RECV_TRANSFER_PTR  packet_recv;
} SOC_DNX_PKT_PACKET_TRANSFER;

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
  SOC_DNX_PKT_PACKET_BUFFER_clear(
    DNX_SAND_OUT SOC_DNX_PKT_PACKET_BUFFER *info
  );

void
  SOC_DNX_PKT_TX_PACKET_INFO_clear(
    DNX_SAND_OUT SOC_DNX_PKT_TX_PACKET_INFO *info
  );

void
  SOC_DNX_PKT_RX_PACKET_INFO_clear(
    DNX_SAND_OUT SOC_DNX_PKT_RX_PACKET_INFO *info
  );

void
  SOC_DNX_PKT_PACKET_TRANSFER_clear(
    DNX_SAND_OUT SOC_DNX_PKT_PACKET_TRANSFER *info
  );

const char*
  SOC_DNX_PACKET_SEND_PATH_TYPE_to_string(
    DNX_SAND_IN  SOC_DNX_PACKET_SEND_PATH_TYPE enum_val
  );

const char*
  SOC_DNX_PKT_PACKET_RECV_MODE_to_string(
    DNX_SAND_IN  SOC_DNX_PKT_PACKET_RECV_MODE enum_val
  );

void
  SOC_DNX_PKT_PACKET_BUFFER_print(
    DNX_SAND_IN SOC_DNX_PKT_PACKET_BUFFER *info,
    DNX_SAND_IN SOC_DNX_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_DNX_PKT_TX_PACKET_INFO_print(
    DNX_SAND_IN SOC_DNX_PKT_TX_PACKET_INFO *info,
    DNX_SAND_IN SOC_DNX_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_DNX_PKT_RX_PACKET_INFO_print(
    DNX_SAND_IN SOC_DNX_PKT_RX_PACKET_INFO *info,
    DNX_SAND_IN SOC_DNX_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_DNX_PKT_PACKET_TRANSFER_print(
    DNX_SAND_IN SOC_DNX_PKT_PACKET_TRANSFER *info
  );


/* } */


/* } __SOC_DNX_API_PACKET_INCLUDED__*/
#endif
