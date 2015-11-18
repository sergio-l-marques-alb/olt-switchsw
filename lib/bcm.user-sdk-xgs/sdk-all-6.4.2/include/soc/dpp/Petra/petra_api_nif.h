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


#ifndef __SOC_PETRA_API_NIF_INCLUDED__
/* { */
#define __SOC_PETRA_API_NIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/*     Number of SerDes quartets.                              */

/*     Maximal number of channels in channelized NIF.          */
#define  SOC_PETRA_NOF_IF_CHANNELS_MAX 64

/*     Maximal number of FAP ports in Fat Pipe                         */
#define  SOC_PETRA_NIF_FAT_PIPE_NOF_PORTS_MAX 4

/*     Minimal number of FAP ports in Fat Pipe                         */
#define  SOC_PETRA_NIF_FAT_PIPE_NOF_PORTS_MIN 1

/* $Id$
 * FAP Port serving as Fat Pipe context after reassembly.
 * Ports 2-4 may also be consumed, but all port-related configurations,
 * e.g. port rate, flow control etc. are configured using FAP port 1
 */
#define  SOC_PETRA_FAT_PIPE_FAP_PORT_ID 1

/*     Maximal number of FAP ports in Fat Pipe                         */
#define  SOC_PETRA_FAT_PIPE_FAP_PORT_ID_LAST \
  (SOC_PETRA_FAT_PIPE_FAP_PORT_ID + SOC_PETRA_NIF_FAT_PIPE_NOF_PORTS_MAX - 1)

/*
 *  Number of Flow Control Classes for Class Based Flow Control
 *  (802.3x Class Based Flow Control extension).
 */
#define  SOC_PETRA_NIF_NOF_FC_CLASSES   8

/*
 *  Number of Classes supported by SGMII NIF.
 */
#define  SOC_PETRA_NIF_NOF_FC_CLASSES_SGMII 2

/* Internally, 8 MAC lanes are divided between 2 NIF groups - A and B */
#define  SOC_PETRA_MALS_IN_NIF_GRP 4
#define  SOC_PETRA_NOF_NIF_GRPS    (SOC_PETRA_NOF_MAC_LANES/SOC_PETRA_MALS_IN_NIF_GRP)

/*
 *  The size, in bytes of resequencing index header for the Fat pipe interface.
 *  This header is added before the layer 2 header (e.g. ITMH header for TM ports).
 */
#define SOC_PETRA_NIF_FAT_PIPE_RESEQ_HDR_SIZE_BYTES 2

/*
 *  Convert NIF MAL index to SerDes quartet.
 *  The conversion is the identity-function.
 */
#define SOC_PETRA_NIF_MAL2QRTT_ID(mal_id) mal_id


/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PETRA_NIF_SGMII_LANE(if_id) \
  ((if_id) % SOC_PETRA_MAX_NIFS_PER_MAL)

/*
 *  Fat Pipe
 */

/*
 *  The maximal port in the range of ports consumed for Fat pipe.
 *  Note: only port 1 is used as a parameter for port-based API-s
 *  for a Fat pipe.
 */
#define SOC_PETRA_MAX_PRT_IF_FAT_PIPE(port_id, fp_nof_ports) \
  ( (((port_id) == SOC_PETRA_FAT_PIPE_FAP_PORT_ID) && \
  ((fp_nof_ports) != 0))?(SOC_PETRA_FAT_PIPE_FAP_PORT_ID + (fp_nof_ports) - 1):(port_id) )

#define SOC_PETRA_IS_FAT_PIPE_PORT_ID(port_id) \
  SOC_SAND_IS_VAL_IN_RANGE(port_id, SOC_PETRA_FAT_PIPE_FAP_PORT_ID, SOC_PETRA_FAT_PIPE_FAP_PORT_ID_LAST)

#define SOC_PETRA_IS_FAT_PIPE_FIRST(port_id) \
  ((port_id) == SOC_PETRA_FAT_PIPE_FAP_PORT_ID)

#define SOC_PETRA_IS_FAT_PIPE_PORT(port_id, fp_nof_ports) \
   ( ((port_id) == SOC_PETRA_FAT_PIPE_FAP_PORT_ID) && ((fp_nof_ports) != 0) )
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */


typedef enum
{
  /*
   *  Single rate - each SerDes physical lane holds single
   *  Logical Lane (LL)
   */
  SOC_PETRA_NIF_SERDES_RATE_SINGLE=0,
  /*
   *  Double rate - each SerDes physical lane holds two
   *  Logical Lanes (LL)
   */
  SOC_PETRA_NIF_SERDES_RATE_DOUBLE=1,
  /*
   *  Total number of SerDes rate configurations
   */
  SOC_PETRA_NIF_NOF_SERDES_RATES=2
}SOC_PETRA_NIF_SERDES_RATE;


typedef enum
{
  /*
   *  Undefined or invalid.
   */
  SOC_PETRA_NIF_TYPE_NONE=0,
  /*
   *  XAUI standard 10Gbps interface
   */
  SOC_PETRA_NIF_TYPE_XAUI=1,
  /*
   *  SPAUI interface (extended XAUI) - 3.125Gbps
   */
  SOC_PETRA_NIF_TYPE_SPAUI=2,
  /*
   *  SGMII standard interface - 1 Gbps
   */
  SOC_PETRA_NIF_TYPE_SGMII=3,
  /*
   *  Total number of NIF types
   */
  SOC_PETRA_NIF_NOF_TYPES=4
}SOC_PETRA_NIF_TYPE;


typedef enum
{
  /*
   *  Preamble size 0 columns. No preamble completion will be
   *  performed (i.e. only SOP and/or BCT).
   */
  SOC_PETRA_NIF_PRMBL_SIZE_COLUMNS_0=0,
  /*
   *  Preamble size 1 columns - complete to one column.
   */
  SOC_PETRA_NIF_PRMBL_SIZE_COLUMNS_1=1,
  /*
   *  Preamble size 2 columns - complete to two columns.
   */
  SOC_PETRA_NIF_PRMBL_SIZE_COLUMNS_2=2,
  /*
   *  Total number of preamble size configurations.
   */
  SOC_PETRA_NIF_NOF_PRMBL_SIZES=3
}SOC_PETRA_NIF_PREAMBLE_SIZE;


typedef enum
{
  /*
   *  Inter-Packet Gap Deficit Idle Count mode. Use average
   *  number of characters between frames configuration.
   */
  SOC_PETRA_NIF_IPG_DIC_MODE_AVERAGE=0,
  /*
   *  Use minimal number of characters between frames
   *  configuration.
   */
  SOC_PETRA_NIF_IPG_DIC_MODE_MINIMUM=1,
  /*
   *  Total number of Deficit Idle Count modes.
   */
  SOC_PETRA_NIF_NOF_IPG_DIC_MODES=2
}SOC_PETRA_NIF_IPG_DIC_MODE;


typedef enum
{
  /*
   *  Standard 4B CRC-32
   */
  SOC_PETRA_NIF_CRC_MODE_32=0,
  /*
   *  Non-standard 3B CRC-24
   */
  SOC_PETRA_NIF_CRC_MODE_24=1,

  /*
   *	RX: no CRC is expected, and the NIF does not
   *  remove the CRC bytes from the packet.
   *  TX: no CRC is calculated and added to the packet.
   *  This mode can be used when the packet CRC is handled by an external logic.
   */
  SOC_PETRA_NIF_CRC_MODE_NONE=2,
  /*
   *  Total number of CRC configurations
   */
  SOC_PETRA_NIF_NOF_CRC_MODES=3
}SOC_PETRA_NIF_CRC_MODE;


typedef enum
{
  /*
   *  1000BASE-X Gigabit Ethernet - 1G Ethernet MAC/PHY which
   *  connects directly to the fiber.
   */
  SOC_PETRA_NIF_1GE_MODE_1000BASE_X=0,
  /*
   *  SGMII MAC which communicates with an external PHY
   *  device.
   */
  SOC_PETRA_NIF_1GE_MODE_SGMII=1,
  /*
   *  Total number of 1GbE modes.
   */
  SOC_PETRA_NIF_NOF_1GE_MODES=2
}SOC_PETRA_NIF_1GE_MODE;


typedef enum
{
  /*
   *  SGMII rate is 10Mbit/sec
   */
  SOC_PETRA_NIF_SGMII_RATE_10Mbps = 0,
  /*
   *  SGMII rate is 100Mbit/sec
   */
  SOC_PETRA_NIF_SGMII_RATE_100Mbps = 1,
  /*
   *  SGMII rate is 1000Mbit/sec
   */
  SOC_PETRA_NIF_SGMII_RATE_1000Mbps = 2,
  /*
   *  Total number of SGMII rates.
   */
  SOC_PETRA_NIF_NOF_SGMII_RATES = 3
}SOC_PETRA_NIF_SGMII_RATE;

typedef enum
{
 /*
  *  Received Packets with frame error.
  */
  SOC_PETRA_NIF_RX_PACKETS_WITH_FRAMES_ERR ,
 /*
  *  Received Packets with CRC error.
  */
  SOC_PETRA_NIF_RX_PACKETS_WITH_CRC_ERR,
 /*
  *  Received Packets with BCT error.
  */
  SOC_PETRA_NIF_RX_PACKETS_WITH_BCT_ERR,
 /*
  *  Received Pause Frames.
  */
  SOC_PETRA_NIF_RX_PAUSE_FRAMES,
 /*
  *  Received MAC Control Frames (excluding Pause)
  */
  SOC_PETRA_NIF_RX_MAC_CTRL_FRAME_EX_PAUSE,
 /*
  *  Received MCAST Packets
  */
  SOC_PETRA_NIF_RX_MCAST_PACKETS,
 /*
  *  Received BCAST Packets
  */
  SOC_PETRA_NIF_RX_BCAST_PACKETS,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 0 - 63
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_0_63,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 64
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_64,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 65-127
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_65_127,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 128-255
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_128_255,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 256-511
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_256_511,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 512-1023
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_512_1023,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 1024-1517
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_1024_1517,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) 1518-MAX
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_1518_MAX,
 /*
  *  Received Packets Length Bins for CRC-32 mode (or CRC-24 mode) too long
  */
  SOC_PETRA_NIF_RX_PACKET_LEN_BINS_TOO_LONG,
 /*
  *  Transmitted MCAST Packets
  */
  SOC_PETRA_NIF_TX_MCAST_PACKETS,
 /*
  *  Transmitted BCAST Packets
  */
  SOC_PETRA_NIF_TX_BCAST_PACKETS,
 /*
  *  Transmitted MAC Control Frames (excluding FC bursts)
  */
  SOC_PETRA_NIF_TX_PAUSE_FRAMES,
 /*
  *  good packets Received that were discarded due to FIFO overflow
  */
  SOC_PETRA_NIF_RX_DISC_FIFO_OVF,

  /*
  *  Number of Received packets Includes all pkt types
  */
  SOC_PETRA_NIF_RX_OK_PACKETS,
 /*
  *  Number of Received octets in valid frames including CRC.
  */
  SOC_PETRA_NIF_RX_OK_OCTETS,
 /*
  *  Number of Transmitted Normal packets (exclude pause frames, M-CAST and B-CAST frames)
  */
  SOC_PETRA_NIF_TX_OK_NORMAL_PACKETS,
 /*
  *  Number of Transmitted octets in valid frames including CRC.
  */
  SOC_PETRA_NIF_TX_OK_OCTETS,

  SOC_PETRA_NIF_NOF_COUNTERS

}SOC_PETRA_NIF_COUNTER_TYPE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  * Current link status.
  * In XAUI mode, indicates that all lanes are
  * comma aligned and that lane alignment (deskew)
  * has been achieved. Equivalent to link_fault
  * variable in IEEE 802.3ae Clause 46.3.4.2.
  * In 1G Ethernet mode, indicates that the
  * XMIT indication for the corresponding lane's
  * AutoNegotiation FSM is XMIT=DATA.
  * In SGMII mode, reflects the link status
  * received from the PHY.
  */
  uint8 link_status;
  /*
   * indicates change in link status of the MAC Lane.
   */
  uint8 link_status_change;
  /*
   * indicates that Rx PCS  synchronization status of the
   * logical lane has changed.
   */
  uint8 pl_status_change;

}SOC_PETRA_NIF_LINK_STATUS;


typedef enum
{
  /*
   *  Indication of type of Remote Fault - OK, no error.
   */
  SOC_PETRA_NIF_SGMII_RFLT_OK=0,
  /*
   *  Indication of type of Remote Fault - Offline request by
   *  link partner.
   */
  SOC_PETRA_NIF_SGMII_RFLT_OFFLINE_REQ=1,
  /*
   *  Indication of type of Remote Fault - Link failure
   *  detected by partner.
   */
  SOC_PETRA_NIF_SGMII_RFLT_LNK_FAIL=2,
  /*
   *  Indication of type of Remote Fault - AutoNegotiation
   *  Error.
   */
  SOC_PETRA_NIF_SGMII_RFLT_AUTONEG_ERR=3,
  /*
   *  Total number of SGMII remote fault type indications.
   *  Note: only valid with GotRemoteFault bit for
   *  corresponding lane.
   */
  SOC_PETRA_NIF_NOF_SGMII_RFLTS=4
}SOC_PETRA_NIF_SGMII_RFLT;


typedef enum
{
  /*
   *  NIF is turned on (normal operation).
   */
  SOC_PETRA_NIF_STATE_ON=0,
  /*
   *  NIF is turned off.
   */
  SOC_PETRA_NIF_STATE_OFF=1,
  /*
   *  Total Number of fabric NIF states.
   */
  SOC_PETRA_NIF_NOF_STATES=2
}SOC_PETRA_NIF_STATE;


typedef enum
{
  /*
   *  Disable Link Level flow control in the specified
   *  direction
   */
  SOC_PETRA_NIF_LL_FC_MODE_DISABLE=0,
  /*
   *  Link Level flow control using standard 802.3x pause
   *  frame
   */
  SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME=1,
  /*
   *  Link Level flow control via Burst Control Tag
   */
  SOC_PETRA_NIF_LL_FC_MODE_ENABLE_BCT=2,
  /*
   *  Total number of flow control modes
   */
  SOC_PETRA_NIF_NOF_LL_FC_MODES=3
}SOC_PETRA_NIF_LL_FC_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If set, order of the SerDes Physical Lanes will be
   *  swapped when mapped into Logical Lanes. For double-rate,
   *  also swap the order of each pair of Logical Lanes that
   *  were muxed together on a Double Rate Physical Lane
   */
  uint8 swap_rx;
  /*
   *  Same as RX
   */
  uint8 swap_tx;
}SOC_PETRA_NIF_LANES_SWAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Network interface type.
   */
  SOC_PETRA_NIF_TYPE type;
  /*
   *  Single or double SereDes rate.
   */
  SOC_PETRA_NIF_SERDES_RATE serdes_rate;
  /*
   *  Enable/Disable Rx/Tx lanes swap.
   */
  SOC_PETRA_NIF_LANES_SWAP lanes_swap;
}SOC_PETRA_NIF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, Preamble Compression is enabled. Applicable for
   *  SPAUI interfaces only. Other fields are only applicable
   *  for get/set operations if enable = TRUE
   */
  uint8 enable;
  /*
   *  Preamble size
   */
  SOC_PETRA_NIF_PREAMBLE_SIZE size;
  /*
   *  If set, /S/ character will not be inserted at the
   *  beginning of a packet.
   */
  uint8 skip_SOP;
}SOC_PETRA_NIF_PREAMBLE_COMPRESS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, Inter Packet Gap Compression is
   *  enabled. Applicable for SPAUI interfaces only. Other
   *  fields are only applicable for get/set operations if
   *  enable = TRUE
   */
  uint8 enable;
  /*
   *  Inter Packet Gap size in bytes. Range: 1 - 255.
   */
  uint32 size;
  /*
   *  Deficit Idle Count mode - average or minimal.
   */
  SOC_PETRA_NIF_IPG_DIC_MODE dic_mode;
}SOC_PETRA_NIF_IPG_COMPRESS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, Burst Control is enabled. Applicable for SPAUI
   *  interfaces only. Other fields are only applicable for
   *  get/set operations if enable = TRUE
   */
  uint8 enable;
  /*
   *  Burst Control Tag Size. Range: 0 - 2.
   */
  uint32 bct_size;
  /*
   *  Index of the byte containing the CH (Channel) field
   *  inside the first column of the preamble. Possible values
   *  are 0 (if no SOP in preamble), 1, 2, 3
   */
  uint32 bct_channel_byte_ndx;
  /*
   *  If TRUE, the channelized interface will function in
   *  burst interleaving mode. Otherwise - in full packet mode
   */
  uint8 is_burst_interleaving;
}SOC_PETRA_NIF_CHANNELIZED;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable the interface in specified direction.
   */
  uint8 enable;
  /*
  *  0 - The link partner's bus size is 32 bits length.
  *  1 - The link partner's bus size is 64 bits length.
   */
  uint8 link_partner_double_size_bus;
  /*
   *  Preamble Compression configuration.
   */
  SOC_PETRA_NIF_PREAMBLE_COMPRESS preamble;
  /*
   *  Inter Packet Gap configuration
   */
  SOC_PETRA_NIF_IPG_COMPRESS ipg;
  /*
   *  24/32 Byte CRC mode configuration.
   */
  SOC_PETRA_NIF_CRC_MODE crc_mode;
  /*
   *  Channelized configuration.
   */
  SOC_PETRA_NIF_CHANNELIZED ch_conf;
}SOC_PETRA_NIF_SPAUI_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the appropriate SGMII interface is enabled.
   */
  uint8 enable;
  /*
   *  1000BASE-X or SGMII.
   */
  SOC_PETRA_NIF_1GE_MODE mode;
  /*
   *  If TRUE, the autonegotiation mechamism is enabled. Note:
   *  the autonegotiation is enabled/disabled according to the
   *  attached PHY. Regardless, the link rate must be
   *  specified. The link rate is forced regardless the
   *  autonegotiation link rate result.
   */
  uint8 is_autoneg;
  /*
   *  SGMII link-rate. Must be always specified (also when
   *  autonegotiation is enabled).
   */
  SOC_PETRA_NIF_SGMII_RATE rate;
}SOC_PETRA_NIF_SGMII_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the auto-negotiation is not complete.
   */
  uint8 autoneg_not_complete;
  /*
   *  If TRUE, indicates that received fault indication from
   *  link partner. In Ethernet mode, indication is Remote
   *  Fault in AutoNegotiation config register. In SGMII mode,
   *  indication is received from PHY.
   */
  uint8 remote_fault;
  /*
   *  Indication of type of Remote Fault. Only valid with
   *  GotRemoteFault bit for corresponding lane.
   */
  SOC_PETRA_NIF_SGMII_RFLT fault_type;
}SOC_PETRA_NIF_SGMII_STAT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  enable/disable Fat pipe interface. Notes: 1. To change
   *  the set of port members of a Fat pipe after enabled, it
   *  must be disabled (set with enable == FALSE), and then
   *  enabled with the updated list of member ports. 2. Some
   *  per-port configurations depend on fat pipe
   *  enable/disable state. Those configurations must be set
   *  after changing fat pipe enable/disable state - refer to
   *  the UM theory of operation for details.
   */
  uint8 enable;
  /*
   *  Number of FAP ports and network interfaces that will be
   *  part of the Fat Pipe. FAP ports that are used are 1 -
   *  nof_ports. Range: 2 - 4. Note: all port-related
   *  configurations (e.g. OFP rate, flow control etc. ) on
   *  Fat pipe must only relate to FAP Port 1, and not to FAP
   *  ports 2-4. Ports 2-4 may also be consumed for the Fat
   *  pipe (according to the nof_ports value), and must not be
   *  used for other purposes in this case (e.g. can not be
   *  mapped to another interface).
   */
  uint32 nof_ports;
  /*
   *  Enables a mode where some packets can bypass the
   *  resequencing process. If set, then the sequence number
   *  range is only 15b [14:0] instead of 16b, and bit 15, if
   *  set, indicates that the packet should bypass the
   *  resequencing process
   */
  uint8 bypass_enable;
}SOC_PETRA_NIF_FAT_PIPE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  NIF state - normal operation (up) or down.
   */
  SOC_PETRA_NIF_STATE on_off;
  /*
   *  Set the serdes lane accordingly - power down when NIF is
   *  turned off, power up when turned on.
   */
  uint8 serdes_also;
}SOC_PETRA_NIF_STATE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Select the transmission mode of Link Level Flow Control
   *  indications - using Burst Control Tag or Pause Frame, or
   *  disable.
   */
  SOC_PETRA_NIF_LL_FC_MODE ll_mode;
  /*
   *  If TRUE, Class Based Flow Control is enabled.
   */
  uint8 cb_enable;
  /*
   *  If TRUE (typical), the NIF will send a XON (transmit-on)
   *  indication to the link partner, that can be used to
   *  clear previous flow control request. Otherwise, the link
   *  partner must wait for the pause quanta before renewing
   *  the transmition.
   */
  uint8 on_if_cleared;
}SOC_PETRA_NIF_FC_TX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Select the reception mode of Link Level Flow Control
   *  indications - using Burst Control Tag or Pause Frame -
   *  Flow Control indications of the other mode will be
   *  ignored
   */
  SOC_PETRA_NIF_LL_FC_MODE ll_mode;
  /*
   *  If TRUE, the NIF will never stop the link. Instead, the
   *  FC will be treated as Class Based and passed for further
   *  handling to the egress (scheduler or EGQ).
   */
  uint8 treat_ll_class_based;
}SOC_PETRA_NIF_FC_RX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Flow Control reception configuration.
   */
  SOC_PETRA_NIF_FC_RX rx;
  /*
   *  Flow Control generation configuration.
   */
  SOC_PETRA_NIF_FC_TX tx;
}SOC_PETRA_NIF_FC_INFO;

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
* NAME:
*     soc_petra_nif_on_off_set
* TYPE:
*   PROC
* FUNCTION:
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0 - 31.
*  SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info -
*     Chooses whether to turn the NIF on or off, and whether
*     to power up/down the attached SerDes also.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_on_off_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_on_off_get
* TYPE:
*   PROC
* FUNCTION:
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0 - 31.
*  SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO      *info -
*     Chooses whether to turn the NIF on or off, and whether
*     to power up/down the attached SerDes also.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_on_off_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info -
*     Basic Network interface configuration -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_OUT SOC_PETRA_NIF_INFO            *nif_info -
*     Basic Network interface configuration -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_INFO            *nif_info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_set_all
* TYPE:
*   PROC
* FUNCTION:
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES] -
*     Basic Network interface configuration, all (8) MAL-s -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_set_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_basic_conf_get_all
* TYPE:
*   PROC
* FUNCTION:
*     Gets basic NIF configuration. After setting basic
*     configuration. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
SOC_SAND_OUT  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
*     Basic Network interface configuration, all (8) MAL-s -- type:
*     XAUI/SPAUI/SGMII.- topology-related: single/double rate,
*     swap.
* REMARKS:
*     1. Topology depends also on SerDes 24/32 links mode. 4th
*     SerDes quartet will be used if and only if in SerDes 32
*     links mode.2. Double rate SerDes and 6 lanes
*     configurations are only applicable for SPAUI NIF type.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_get_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_topology_validate
* TYPE:
*   PROC
* FUNCTION:
*     Validates NIF to SerDes mapping topology configuration.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     Effective after the relevant MAL-s were enabled.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_topology_validate(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_enable_set
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disable specified MAL.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_IN  uint8                 is_enabled -
*     If TRUE, the specified MAL will be enabled. Otherwise -
*     disabled.
* REMARKS:
*     This function must be called after completing NIF
*     configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mal_enable_get
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disable specified MAL.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
*  SOC_SAND_OUT uint8                 *is_enabled -
*     If TRUE, the specified MAL will be enabled. Otherwise -
*     disabled.
* REMARKS:
*     This function must be called after completing NIF
*     configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_conf_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets SGMII/1000BASE-X interface configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info -
*     SGMII-specific configuration.
* REMARKS:
*     1. This API does not configure the SerDes. Dedicated API
*     must be used for SerDes configuration. 2. The rate can
*     also be configured via a separate API.
*     3. The get function is not entirely symmetric to the set function
*     (where only rx, tx or both directions can be defined). The get
*     function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_conf_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets SGMII/1000BASE-X interface configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_rx -
*     SGMII-specific configuration for rx.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_tx -
*     SGMII-specific configuration for tx.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*     1. This API does not configure the SerDes. Dedicated API
*     must be used for SerDes configuration. 2. The rate can
*     also be configured via a separate API. 3. Not entirely
*     symmetric to the set function (where only rx,
*     tx or both directions can be defined). The get function returns
*     the both directions.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO      *info_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_status_get
* TYPE:
*   PROC
* FUNCTION:
*     Gets SGMII/1000BASE-X interface diagnostics status
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT      *status -
*     SGMII interface status.
* REMARKS:
*     None .
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT      *status
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_rate_set
* TYPE:
*   PROC
* FUNCTION:
*   Sets SGMII interface link-rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate -
*     SGMII rate (10/100/1000 Mbps).
* REMARKS:
*   1. The SGMII rate is typically configured upon
*   initialization by the sgmii_conf_get_verify API, called
*   by init_sequence_phase1. 2. The SGMII rate configuration
*   changes the link rate. It does not alter the
*   Autonegotiation enable/disable configuration, which is
*   PHY-dependant.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_sgmii_rate_get
* TYPE:
*   PROC
* FUNCTION:

*   Gets SGMII interface link-rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_SGMII_RATE      *rate -
*     SGMII rate (10/100/1000 Mbps).
* REMARKS:
*   1. The SGMII rate is typically configured upon
*   initialization by the sgmii_conf_get_verify API, called
*   by init_sequence_phase1. 2. The SGMII rate configuration
*   changes the link rate. It does not alter the
*   Autonegotiation enable/disable configuration, which is
*   PHY-dependant.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_RATE      *rate
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_spaui_conf_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info -
*     SPAUI extensions information - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
* REMARKS:
*     1. This is a SPAUI extension to standard XAUI
*     interface.2. IPG configuration is only applicable in TX
*     direction - will be ignored for RX configuration.
*     3. Not entirely symmetric to the set function (where only rx,
*      tx or both directions can be defined). The get function returns
*      the both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_spaui_conf_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_rx -
*     SPAUI extensions information for rx - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
*  SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_tx -
*     SPAUI extensions information for tx - - preamble- ipg
*     (inter-packet gap)- channelized interface configuration-
*     crc mode
* REMARKS:
*     1. This is a SPAUI extension to standard XAUI
*     interface.2. IPG configuration is only applicable in TX
*     direction - will be ignored for RX configuration.
*     3. Not entirely symmetric to the set function (where only rx,
*     tx or both directions can be defined). The get function returns
*     the both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_type_get
* TYPE:
*   PROC
* FUNCTION:
*     Gets interface type and channelized indication
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_OUT SOC_PETRA_NIF_TYPE            *nif_type -
*     Interface type (i.e. SGMII).
*  SOC_SAND_OUT uint8                 *is_channelized -
*     TRUE if the interface is channelized.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_TYPE            *nif_type,
    SOC_SAND_OUT uint8                 *is_channelized
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_local_fault_ovrd_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint8                 is_lf_ovrd -
*     If TRUE, Local Fault is indicated. This triggers the
*     Soc_petra to send Remote Fault indication on the MAL.
* REMARKS:
*     This API handles LF indicated by a source external to
*     the Soc_petra device (e.g. external PHY). Internal LFS is
*     always handled by the Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_lf_ovrd
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_local_fault_ovrd_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_OUT uint8                 *is_lf_ovrd -
*     If TRUE, Local Fault is indicated. This triggers the
*     Soc_petra to send Remote Fault indication on the MAL.
* REMARKS:
*     This API handles LF indicated by a source external to
*     the Soc_petra device (e.g. external PHY). Internal LFS is
*     always handled by the Soc_petra.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_lf_ovrd
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fat_pipe_set
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info -
*     FAT PIPE Configuration
* REMARKS:
*     After resequencing, the packets that arrived through one
*     of the Fat Pipe ports (FAP ports 1-4) appear as if they
*     arrived through FAP port 1, and must be treated as such
*     downstream (scheduling etc.). In other words, when Fat
*     Pipe enabled, FAP port 1 accumulates the bandwidth of
*     FAP ports 1-4, and FAP ports 2-4 cannot be used for
*     other purposes.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fat_pipe_get
* TYPE:
*   PROC
* FUNCTION:
*     Configures Fat Pipe interface, including FAP Ports to
*     NIF mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO   *info -
*     FAT PIPE Configuration
* REMARKS:
*     After resequencing, the packets that arrived through one
*     of the Fat Pipe ports (FAP ports 1-4) appear as if they
*     arrived through FAP port 1, and must be treated as such
*     downstream (scheduling etc.). In other words, when Fat
*     Pipe enabled, FAP port 1 accumulates the bandwidth of
*     FAP ports 1-4, and FAP ports 2-4 cannot be used for
*     other purposes.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio22_write
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 22 write function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  phy_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be written.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to write
*  SOC_SAND_IN  uint16                  *data -
*     Buffer containing the data to write. Must be at least of
*     size "size_in_words".
* REMARKS:
*     If 'size_in_words' is > 1, consecutive addresses are written with the
*     values provided in the 'data' buffer.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio22_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio22_read
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 22 read function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  phy_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be read.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to read
*  SOC_SAND_OUT uint16                  *data -
*     Buffer containing the data to read. Must be at least of
*     size "size_in_words".
* REMARKS:
*     If 'size_in_words' is > 1, consecutive addresses are read
*     to the 'data' buffer.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio22_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio45_write
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 45 write function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  port_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  dev_addr -
*     The device page address of the slave device.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be written.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to write
*  SOC_SAND_IN  uint16                  *data -
*     Buffer containing the data to write. Must be at least of
*     size "size_in_words".
* REMARKS:
*     If 'size_in_words' is > 1, consecutive addresses are written with the
*     values provided in the 'data' buffer.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio45_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_mdio45_read
* TYPE:
*   PROC
* FUNCTION:
*     MDIO Clause 45 read function
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  port_addr -
*     The address of the slave device on the MDIO bus.
*  SOC_SAND_IN  uint32                  dev_addr -
*     The device page address of the slave device.
*  SOC_SAND_IN  uint32                  reg_addr -
*     The address of the register within the slave device that
*     is to be read.
*  SOC_SAND_IN  uint32                  size_in_words -
*     Size in words (16 bit) of the data to read
*  SOC_SAND_OUT uint16                  *data -
*     Buffer for the data to be read. Must be at least of size
*     "size_in_words".
* REMARKS:
*     If 'size_in_words' is > 1, consecutive addresses are read
*     to the 'data' buffer.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_mdio45_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_min_packet_size_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets minimal packet size as limited by the NIF. Note:
*     Normally, the packet size is limited using
*     soc_petra_mgmt_pckt_size_range_set(), and not this API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_IN  uint32                  pckt_size -
*     Minimal packet size in Bytes.
* REMARKS:
*     1. Normally, the packet size is limited using
*     soc_petra_mgmt_pckt_size_range_set(), and not this API. 2.
*     For RX direction -- the NIF will drop smaller packets.
*     Note that the inspected packet size includes packet CRC.
*     3. For TX direction -- the NIF will pad smaller packets
*     with trailing zeros. The packet CRC is calculated on the
*     whole packet, including zeros. 4. The get function is
*     not entirely symmetric to the set function (where only
*     rx, tx or both directions can be defined). The get
*     function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_min_packet_size_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint32                  pckt_size
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_min_packet_size_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets minimal packet size as limited by the NIF. Note:
*     Normally, the packet size is limited using
*     soc_petra_mgmt_pckt_size_range_set(), and not this API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Defines whether the configuration refers to RX, TX or
*     both.
*  SOC_SAND_OUT uint32                  *pckt_size -
*     Minimal packet size in Bytes.
* REMARKS:
*     1. Normally, the packet size is limited using
*     soc_petra_mgmt_pckt_size_range_set(), and not this API. 2.
*     For RX direction -- the NIF will drop smaller packets.
*     Note that the inspected packet size includes packet CRC.
*     3. For TX direction -- the NIF will pad smaller packets
*     with trailing zeros. The packet CRC is calculated on the
*     whole packet, including zeros. 4. The get function is
*     not entirely symmetric to the set function (where only
*     rx, tx or both directions can be defined). The get
*     function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_min_packet_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint32                  *pckt_size_rx,
    SOC_SAND_OUT uint32                  *pckt_size_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_quanta_set
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx -
*     Selects whether the configuration refers to flow control
*     generation, reception or both.
*  SOC_SAND_IN  uint32                  pause_quanta -
*     Pause quanta: - FC Reception - pause quanta to be used
*     upon receiving link-level flow control via the BCT,
*     ignored for other modes. - FC Generation - pause quanta
*     to send. Units: 512 bit times (as defined in IEEE 802.3x
*     standard).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_IN  uint32                  pause_quanta
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_quanta_get
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx -
*     Selects whether the configuration refers to flow control
*     generation, reception or both.
*  SOC_SAND_OUT uint32                  *pause_quanta -
*     Pause quanta: - FC Reception - pause quanta to be used
*     upon receiving link-level flow control via the BCT,
*     ignored for other modes. - FC Generation - pause quanta
*     to send. Units: 512 bit times (as defined in IEEE 802.3x
*     standard).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_OUT uint32                  *pause_quanta
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_frame_src_addr_set
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS       *mac_addr -
*     Source MAC address, part of 802.3 pause frame.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS       *mac_addr
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_pause_frame_src_addr_get
* TYPE:
*   PROC
* FUNCTION:
*     Function description
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAC Lane index. Range: 0 - 7.
*  SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS       *mac_addr -
*     Source MAC address, part of 802.3 pause frame.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS       *mac_addr
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fc_set
* TYPE:
*   PROC
* FUNCTION:
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info -
*     Flow control configuration.
* REMARKS:
*     This API only covers flow control handling on a NIF port
*     level. Please refer to device-level flow control API-s
*     in the flow_control module.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_fc_get
* TYPE:
*   PROC
* FUNCTION:
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO         *info -
*     Flow control configuration.
* REMARKS:
*     This API only covers flow control handling on a NIF port
*     level. Please refer to device-level flow control API-s
*     in the flow_control module.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO         *info
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_loopback_set
* TYPE:
*   PROC
* FUNCTION:
*     Set all Network Interface SerDes in loopback.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_IN  uint8                 enable -
*     If TRUE, all the NIF SerDes lanes are set in loopback,
*     NSILB mode. Otherwise - all NIF SerDes lane are set not
*     to be in loopback.
* REMARKS:
*     The SerDes loopback mode that is used is NSILB.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_loopback_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_loopback_get
* TYPE:
*   PROC
* FUNCTION:
*     Set all Network Interface SerDes in loopback.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     Network interface index. Range: 0-31.
*  SOC_SAND_OUT uint8                 *enable -
*     If TRUE, all the NIF SerDes lanes are set in loopback,
*     NSILB mode. Otherwise - all NIF SerDes lane are set not
*     to be in loopback.
* REMARKS:
*     The SerDes loopback mode that is used is NSILB.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_nif_loopback_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT uint8                 *enable
  );


/*********************************************************************
* NAME:
*     soc_petra_nif_counter_get
* TYPE:
*   PROC
* FUNCTION:
*     Gets counter value of the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint32                 port_ndx -
*     Port Id, Range: For SGMII, 0-3 For XAUI 0-0.
*  SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type -
*     Counter Type.
*  SOC_SAND_OUT SOC_SAND_64CNT                *counter -
*     counter Value
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_counter_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                   *counter_val
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_all_counters_get
* TYPE:
*   PROC
* FUNCTION:
*     Gets counter value of the NIF.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0 - 7.
*  SOC_SAND_IN  uint32                 channel_ndx -
*     Port Id, Range: For SGMII, 0-3 For XAUI 0-0.
*  SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE  counter_type -
*     Counter Type.
*  SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS] -
*     all NIF counters Values, order according to SOC_PETRA_NIF_COUNTER_TYPE.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_all_counters_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS]
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_all_nifs_all_counters_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets all counters of the all NIFs.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS] -
*     all NIF counters Values, order according to SOC_PETRA_NIF_COUNTER_TYPE.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*  - when print all NIF counters (using soc_petra_PETRA_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print)
*    also the local-ports mapped to each NIF are printed, in case that
*    more than one port is mapped to the same NIF (for example SPAUI) only one
*    port is printed.
*********************************************************************/
uint32
  soc_petra_nif_all_nifs_all_counters_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS]
  );

/*********************************************************************
* NAME:
*     soc_petra_nif_link_status_get
* TYPE:
*   PROC
* FUNCTION:
*     Gets link status, and whether was change in the status.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx -
*     interface index. Range: 0-31.
*  SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS        *link_status -
*     link status, and whether was change in the status.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_nif_link_status_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS        *link_status
  );



uint32
  soc_petra_nif_is_channelized(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT  uint8         *is_channelized
  );

void
  soc_petra_PETRA_NIF_LANES_SWAP_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_LANES_SWAP *info
  );

void
  soc_petra_PETRA_NIF_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_INFO *info
  );

void
  soc_petra_PETRA_NIF_PREAMBLE_COMPRESS_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_PREAMBLE_COMPRESS *info
  );

void
  soc_petra_PETRA_NIF_IPG_COMPRESS_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_IPG_COMPRESS *info
  );

void
  soc_petra_PETRA_NIF_CHANNELIZED_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_CHANNELIZED *info
  );

void
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO *info
  );

void
  soc_petra_PETRA_NIF_SGMII_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO *info
  );

void
  soc_petra_PETRA_NIF_SGMII_STAT_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT *info
  );

void
  soc_petra_PETRA_NIF_FAT_PIPE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO *info
  );

void
  soc_petra_PETRA_NIF_STATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO *info
  );

void
  soc_petra_PETRA_NIF_FC_TX_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FC_TX *info
  );

void
  soc_petra_PETRA_NIF_FC_RX_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FC_RX *info
  );

void
  soc_petra_PETRA_NIF_FC_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO *info
  );

void
  soc_petra_PETRA_NIF_LINK_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS *info
  );


#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_NIF_SERDES_RATE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_SERDES_RATE enum_val
  );



const char*
  soc_petra_PETRA_NIF_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_TYPE enum_val
  );



const char*
  soc_petra_PETRA_NIF_PREAMBLE_SIZE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_PREAMBLE_SIZE enum_val
  );



const char*
  soc_petra_PETRA_NIF_IPG_DIC_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_IPG_DIC_MODE enum_val
  );



const char*
  soc_petra_PETRA_NIF_CRC_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_CRC_MODE enum_val
  );



const char*
  soc_petra_PETRA_NIF_1GE_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_1GE_MODE enum_val
  );



const char*
  soc_petra_PETRA_NIF_SGMII_RATE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_RATE enum_val
  );



const char*
  soc_petra_PETRA_NIF_SGMII_RFLT_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_RFLT enum_val
  );



const char*
  soc_petra_PETRA_NIF_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_STATE enum_val
  );



const char*
  soc_petra_PETRA_NIF_LL_FC_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_LL_FC_MODE enum_val
  );



void
  soc_petra_PETRA_NIF_LANES_SWAP_print(
    SOC_SAND_IN SOC_PETRA_NIF_LANES_SWAP *info
  );



void
  soc_petra_PETRA_NIF_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_INFO *info
  );



void
  soc_petra_PETRA_NIF_PREAMBLE_COMPRESS_print(
    SOC_SAND_IN SOC_PETRA_NIF_PREAMBLE_COMPRESS *info
  );



void
  soc_petra_PETRA_NIF_IPG_COMPRESS_print(
    SOC_SAND_IN SOC_PETRA_NIF_IPG_COMPRESS *info
  );



void
  soc_petra_PETRA_NIF_CHANNELIZED_print(
    SOC_SAND_IN SOC_PETRA_NIF_CHANNELIZED *info
  );



void
  soc_petra_PETRA_NIF_SPAUI_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_SPAUI_INFO *info
  );



void
  soc_petra_PETRA_NIF_SGMII_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_INFO *info
  );



void
  soc_petra_PETRA_NIF_SGMII_STAT_print(
    SOC_SAND_IN SOC_PETRA_NIF_SGMII_STAT *info
  );



void
  soc_petra_PETRA_NIF_FAT_PIPE_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_FAT_PIPE_INFO *info
  );



void
  soc_petra_PETRA_NIF_STATE_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_STATE_INFO *info
  );



void
  soc_petra_PETRA_NIF_FC_TX_print(
    SOC_SAND_IN SOC_PETRA_NIF_FC_TX *info
  );



void
  soc_petra_PETRA_NIF_FC_RX_print(
    SOC_SAND_IN SOC_PETRA_NIF_FC_RX *info
  );



void
  soc_petra_PETRA_NIF_FC_INFO_print(
    SOC_SAND_IN SOC_PETRA_NIF_FC_INFO *info
  );



void
  soc_petra_PETRA_NIF_LINK_STATUS_print(
    SOC_SAND_IN SOC_PETRA_NIF_LINK_STATUS *info
  );

const char*
  soc_petra_PETRA_NIF_COUNTER_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_NIF_COUNTER_TYPE enum_val,
    SOC_SAND_IN uint8                  short_format
  );


void
  soc_petra_PETRA_NIF_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN SOC_SAND_64CNT                        all_counters[SOC_PETRA_NIF_NOF_COUNTERS],
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS                flavor_bitmap
  );

/*********************************************************************
* NAME:
*     soc_petra_PETRA_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print
* TYPE:
*   PROC
* FUNCTION:
*     print all NIF counters in a table formated
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_64CNT                        all_counters[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS] -
*     all NIF counters Values, order according to SOC_PETRA_NIF_COUNTER_TYPE.
*     as returned by soc_petra_nif_all_counters_get().
*  SOC_SAND_PRINT_FLAVORS                        flavor_bitmap -
*     bitmap that indicates how to print the counters/table, including:
*     - SOC_SAND_PRINT_FLAVORS_NO_ZEROS: whether to print zero values or not
*     - SOC_SAND_PRINT_FLAVORS_SHORT: short/long format printing.
*                                  in this case (table) has to be set to short.
*     Example of use:
*       flavor_bitmap = SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS) | SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT)
*       to print in short format without zero entries.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*  - This function also print the local-ports mapped to the NIF, in case that
*    more than one port is mapped to the same NIF (for example SPAUI) only one
*    port is printed.
*********************************************************************/void
  soc_petra_PETRA_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN  int                        unit,
             SOC_SAND_64CNT                        all_counters[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS],
    SOC_SAND_IN  SOC_SAND_PRINT_FLAVORS                flavor_bitmap
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_NIF_INCLUDED__*/
#endif
