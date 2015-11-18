/* $Id: soc_petra_api_general.h,v 1.8 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_GENERAL_INCLUDED__
/* { */
#define __SOC_PETRA_API_GENERAL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_api_general.h,v 1.8 Broadcom SDK $
 * Maximum number of FAPs in the system.
 */
#define SOC_PETRA_NOF_FAPS_IN_SYSTEM     (2048)
#define SOC_PETRA_MAX_FAP_ID             (SOC_PETRA_NOF_FAPS_IN_SYSTEM-1)
#define SOC_PETRA_MAX_DEVICE_ID          (SOC_PETRA_NOF_FAPS_IN_SYSTEM-1)

/*
 *	If Egress MC 16K members mode is enabled,
 *  the FAP-IDs range in the system is limited to 0 - 511.
 */
#define SOC_PETRA_MAX_FAP_ID_IF_MC_16K_EN 511

/*
 *  Typically used when the function demands unit parameter,
 *  but the device id is irrelevant in the given context,
 *  and the value is not used in fact.
 */
#define SOC_PETRA_DEVICE_ID_IRRELEVANT   SOC_PETRA_MAX_DEVICE_ID

/*     Maximal number of physical ports.                       */
#define SOC_PETRA_NOF_SYS_PHYS_PORTS     (4096)
#define SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID   (SOC_PETRA_NOF_SYS_PHYS_PORTS - 1)

/*     Maximal number of physical ports id is indication for Invalid port */
#define SOC_PETRA_SYS_PHYS_PORT_INVALID         SOC_TMC_SYS_PHYS_PORT_INVALID

/*     Maximal number of logical ports.                        */
#define SOC_PETRA_NOF_SYSTEM_PORTS              (8192)
#define SOC_PETRA_MAX_SYSTEM_PORT_ID            (SOC_PETRA_NOF_SYSTEM_PORTS - 1)

/*
 * Maximum number of LBP destination IDs.
 * Range 0:4095
 */

/*
 * Number of queues in Soc_petra - 32K.
 */
#define SOC_PETRA_NOF_QUEUES             (32*1024)
#define SOC_PETRA_MAX_QUEUE_ID           (SOC_PETRA_NOF_QUEUES-1)

/*
 * Number of flows in Soc_petra - 56K.
 */
#define SOC_PETRA_NOF_FLOWS              (56*1024)

/*
 * Number of queue types in SOC_PETRA - 16.
 */

/*
 * Number of drop precedences.
 */
#define SOC_PETRA_NOF_DROP_PRECEDENCE    4
#define SOC_PETRA_MAX_DROP_PRECEDENCE    (SOC_PETRA_NOF_DROP_PRECEDENCE-1)

/* Ingress-Packet-traffic-class: Value 0 */
/* Ingress-Packet-traffic-class: Value 7 */
#define SOC_PETRA_TR_CLS_MAX                SOC_PETRA_NOF_TRAFFIC_CLASSES-1

/* Soc_petra number of traffic classes.*/
#define SOC_PETRA_NOF_TRAFFIC_CLASSES        SOC_TMC_NOF_TRAFFIC_CLASSES

/*
 * Number of FAP-data-ports in SOC_PETRA.
 */
#define SOC_PETRA_NOF_FAP_PORTS                  SOC_TMC_NOF_FAP_PORTS_PETRA
#define SOC_PETRA_MAX_FAP_PORT_ID                (SOC_PETRA_NOF_FAP_PORTS-1)
#define SOC_PETRA_FAP_PORT_ID_INVALID            (SOC_TMC_NOF_FAP_PORTS_PETRA+1)

/*
 * ERP (Egress Replication Port), is responsible of
 *  replicating packets in the egress (Multicast)
 * SOC_PETRA_FAP_EGRESS_REPLICATION_SCH_PORT_ID:
 *  The Port ID When attaching flow / aggregate scheduler to it
 * SOC_PETRA_FAP_EGRESS_REPLICATION_IPS_PORT_ID:
 *  The port ID when mapping the system port to it.
 */
#define SOC_PETRA_FAP_EGRESS_REPLICATION_SCH_PORT_ID       80
#define SOC_PETRA_FAP_EGRESS_REPLICATION_IPS_PORT_ID       255

#define SOC_PETRA_NOF_MAC_LANES             8
#define SOC_PETRA_MAX_NIFS_PER_MAL          4

/*
 *  Maximal Number of NIFS
 */

/*
 * Number of links from the Soc_petra device toward the fabric element.
 */
#define SOC_PETRA_FBR_NOF_LINKS               SOC_TMC_FBR_NOF_LINKS
#define SOC_PETRA_FBR_LINK_INVALID            SOC_PETRA_FBR_NOF_LINKS

#define SOC_PETRA_NOF_RMGR_UNITS_PER_LINK     128
/*
 *  The coefficient to convert 1Kilo-bit-per-second to bit-per-second (e.g.).
 */
#define SOC_PETRA_RATE_1K                           1000
/*
* Maximal interface rate, in Mega-bits per second.
* This is the upper boundary, it can be lower
*  depending on the credit size
*/
#define SOC_PA_SCH_MAX_RATE_MBPS                 (SOC_PETRA_RATE_1K * 105)

/*
 * Maximal interface rate, in Mega-bits per second.
 * This is the upper boundary, it can be lower
 *  depending on the credit size
 */
#define SOC_PA_IF_MAX_RATE_MBPS                 (SOC_PETRA_RATE_1K * 80)

#ifdef LINK_PB_LIBRARIES
  #define SOC_PB_SCH_MAX_RATE_MBPS        (SOC_PETRA_RATE_1K * 160)
  #define SOC_PB_IF_MAX_RATE_MBPS         (SOC_PETRA_RATE_1K * 140)
#else
  #define SOC_PB_SCH_MAX_RATE_MBPS        SOC_PA_SCH_MAX_RATE_MBPS
  #define SOC_PB_IF_MAX_RATE_MBPS         SOC_PA_IF_MAX_RATE_MBPS
#endif

/*
 * Maximal interface rate, in Kilo-bits per second.
 * This is the upper boundary, it can be lower
 *  depending on the credit size
 */
#define SOC_PETRA_IF_MAX_RATE_KBPS                 (SOC_PETRA_RATE_1K * SOC_PETRA_IF_MAX_RATE_MBPS)

/*
 * Default Values
 * {
 */

/*
 * This queue default as no real meaning,
 * just as a value to put in erase.
 */
/*
 * This FAP id default as no real meaning,
 * just as a value to put in erase.
 */
/*
 * This id default as no real meaning,
 * just as a value to put in erase.
 */
/*
 * This id default as no real meaning,
 * just as a value to put in erase.
 */
/*
 * This id default as no real meaning,
 * just as a value to put in erase.
 */

/*
* CPU Default port identifier is 0
*/
#define SOC_PETRA_DEFAULT_CPU_PORT_ID       (0)

/*
 * End Default Values
 * }
 */

/*
 * This is the maximum ingress queue size available in the device.
 * In granularity of bytes.
 */
#define SOC_PETRA_MAX_QUEUE_SIZE_BYTES        (256 * 1024 * 1024-1)

/* Multicast-ID min: Value 0 */
#define SOC_PETRA_MULT_ID_MIN                 0
/* Multicast-ID max: Value 16,383. */
#define SOC_PETRA_MULT_ID_MAX                 SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1

#define SOC_PETRA_MULT_NOF_MULTICAST_GROUPS       (16*1024)

#define SOC_PETRA_MULT_NOF_MULT_VLAN_ERP_GROUPS   (4*1024)

/*
 * Number of Snoop Commands.
 * There are 15 Snoop Commands altogether 1-15, 0 means disabling Snooping.
 */
#define SOC_PETRA_NOF_SNOOP_COMMAND_INDEX   15
#define SOC_PETRA_MAX_SNOOP_COMMAND_INDEX   (SOC_PETRA_NOF_SNOOP_COMMAND_INDEX)
/*
* Number of Signatures.
*/
#define SOC_PETRA_NOF_SIGNATURE        4
#define SOC_PETRA_MAX_SIGNATURE        (SOC_PETRA_NOF_SIGNATURE-1)

/*
 * Packet Size (Bytes)
 */
#define SOC_PETRA_NOF_PACKET_BYTES     (16*1024)
#define SOC_PETRA_MAX_PACKET_BYTES     (SOC_PETRA_NOF_PACKET_BYTES-1)

/*
 * Copy-unique-data (e.g. outlif) index
 */
#define SOC_PETRA_NOF_CUD_ID         65536
#define SOC_PETRA_MAX_CUD_ID         (SOC_PETRA_NOF_CUD_ID-1)

/*     Maximal number of LAG groups.                        */
#define SOC_PETRA_NOF_LAG_GROUPS              (256)
#define SOC_PETRA_MAX_LAG_GROUP_ID            (SOC_PETRA_NOF_LAG_GROUPS - 1)

/*     Maximal number of LAG entries.                        */
#define SOC_PETRA_NOF_LAG_ENTRIES             (16)
#define SOC_PETRA_MAX_LAG_ENTRY_ID            (SOC_PETRA_NOF_LAG_ENTRIES - 1)

/*
 *  Rate configuration calendar sets - A and B
 */

/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PETRA_DO_NOTHING_AND_EXIT                       goto exit

/* Data Ports Macros { */
#define SOC_PETRA_FRST_CPU_PORT_ID              0
#define SOC_PETRA_SCND_CPU_PORT_ID              73
#define SOC_PETRA_LAST_CPU_PORT_ID              78

#define SOC_PETRA_OLP_PORT_ID                   79
#define SOC_PETRA_ERP_PORT_ID                   80

#define SOC_PETRA_IS_CPU_FAP_PORT_ID(port_id) \
  (((port_id) == SOC_PETRA_FRST_CPU_PORT_ID) || (((port_id) >=SOC_PETRA_SCND_CPU_PORT_ID) && ((port_id) <= SOC_PETRA_LAST_CPU_PORT_ID)))
#define SOC_PETRA_IS_OLP_FAP_PORT_ID(port_id) ((port_id) == SOC_PETRA_OLP_PORT_ID)

/* Note: ERP is a virtual port, not a data port */
/* Data Ports Macros } */

/* Interface Ports Macros { */
#define SOC_PETRA_IS_NIF_ID(if_id) \
  ((if_id) <= SOC_PETRA_MAX_NIF_ID)


#define SOC_PETRA_IS_CPU_IF_ID(if_id) \
  ((if_id) == SOC_PETRA_IF_ID_CPU)

#define SOC_PETRA_IS_OLP_IF_ID(if_id) \
  ((if_id) == SOC_PETRA_IF_ID_OLP)

#define SOC_PETRA_IS_RCY_IF_ID(if_id) \
  ((if_id) == SOC_PETRA_IF_ID_RCY)

#define SOC_PETRA_IS_ERP_IF_ID(if_id) \
  ((if_id) == SOC_PETRA_IF_ID_ERP)

#define SOC_PETRA_IS_ECI_IF_ID(if_id) \
  ( SOC_PETRA_IS_CPU_IF_ID(if_id) || SOC_PETRA_IS_OLP_IF_ID(if_id) )

#define SOC_PETRA_IS_NONE_IF_ID(if_id) \
  ((if_id) == SOC_PETRA_IF_ID_NONE)

#define SOC_PETRA_IS_MAL_EQUIVALENT_ID(mal_id) \
  (SOC_PETRA_IS_ECI_IF_ID(mal_id) || SOC_PETRA_IS_RCY_IF_ID(mal_id) || SOC_PETRA_IS_ERP_IF_ID(mal_id))

/* Interface Ports Macros } */

#define SOC_PA_MAX_NIFS_PER_MAL          4

#define SOC_PA_NIF2MAL_NDX(if_id) \
  ((uint32)((if_id) / SOC_PA_MAX_NIFS_PER_MAL))

#define SOC_PA_MAL2NIF_NDX(mal_id) \
  ((mal_id) * SOC_PA_MAX_NIFS_PER_MAL)

#define SOC_PETRA_NIF2MAL_NDX(if_id) \
  ((uint32)((if_id) / SOC_PETRA_MAX_NIFS_PER_MAL))

#define SOC_PETRA_MAL2NIF_NDX(mal_id) \
  ((mal_id) * SOC_PETRA_MAX_NIFS_PER_MAL)

#define SOC_PETRA_IF2MAL_NDX(if_id) \
  (SOC_PETRA_IS_NIF_ID(if_id)?SOC_PETRA_NIF2MAL_NDX(if_id):(uint32)(if_id))

#define SOC_PETRA_MAL2IF_NDX(mal_id) \
  ((mal_id)<SOC_PETRA_NOF_MAC_LANES?SOC_PETRA_MAL2NIF_NDX(mal_id):(mal_id))

/*
 *  TRUE for network interfaces 0, 4, 8... (first interface in each MAL).
 */
#define SOC_PETRA_IS_MAL_NIF_ID(if_id) \
  (uint8)( (if_id <= SOC_PETRA_INTERN_MAX_NIFS) && \
   (SOC_PETRA_MAL2NIF_NDX(SOC_PETRA_NIF2MAL_NDX(if_id)) == (uint32)(if_id)) )

#define SOC_PETRA_IS_DIRECTION_RX(dir) \
  SOC_SAND_NUM2BOOL(((dir) == SOC_PETRA_CONNECTION_DIRECTION_RX  ) || ((dir) == SOC_PETRA_CONNECTION_DIRECTION_BOTH))

#define SOC_PETRA_IS_DIRECTION_TX(dir) \
  SOC_SAND_NUM2BOOL(((dir) == SOC_PETRA_CONNECTION_DIRECTION_TX  ) || ((dir) == SOC_PETRA_CONNECTION_DIRECTION_BOTH))

#define SOC_PETRA_IS_DIRECTION_REC(dir) \
  SOC_SAND_NUM2BOOL((dir) == SOC_PETRA_FC_DIRECTION_REC  )

#define SOC_PETRA_IS_DIRECTION_GEN(dir) \
  SOC_SAND_NUM2BOOL((dir) == SOC_PETRA_FC_DIRECTION_GEN  )

#define SOC_PETRA_MULTICAST_DEST_INDICATION    (SOC_SAND_BIT(14))

/*
 *	Maximal Number of Network Interfaces
 */
#define SOC_PETRA_IF_NOF_NIFS 32
/*
 *  Maximal number of Network devices for
 *  Soc_petra family devices Soc_petra-A, Soc_petra-B...,
 *  using internal representation
 */
#define SOC_PETRA_INTERN_MAX_NIFS 64
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
#define SOC_PETRA_FAR_DEVICE_TYPE_FE1                          SOC_TMC_FAR_DEVICE_TYPE_FE1
#define SOC_PETRA_FAR_DEVICE_TYPE_FE2                          SOC_TMC_FAR_DEVICE_TYPE_FE2
#define SOC_PETRA_FAR_DEVICE_TYPE_FE3                          SOC_TMC_FAR_DEVICE_TYPE_FE3
#define SOC_PETRA_FAR_DEVICE_TYPE_FAP                          SOC_TMC_FAR_DEVICE_TYPE_FAP
typedef SOC_TMC_FAR_DEVICE_TYPE                                SOC_PETRA_FAR_DEVICE_TYPE;

#define SOC_PETRA_IF_TYPE_NONE                                 SOC_TMC_IF_TYPE_NONE
#define SOC_PETRA_IF_TYPE_CPU                                  SOC_TMC_IF_TYPE_CPU
#define SOC_PETRA_IF_TYPE_RCY                                  SOC_TMC_IF_TYPE_RCY
#define SOC_PETRA_IF_TYPE_OLP                                  SOC_TMC_IF_TYPE_OLP
#define SOC_PETRA_IF_TYPE_NIF                                  SOC_TMC_IF_TYPE_NIF
#define SOC_PETRA_IF_TYPE_ERP                                  SOC_TMC_IF_TYPE_ERP
typedef SOC_TMC_INTERFACE_TYPE                                 SOC_PETRA_INTERFACE_TYPE;

#define SOC_PETRA_IF_ID_0                                      SOC_TMC_IF_ID_0
#define SOC_PETRA_IF_ID_31                                     SOC_TMC_IF_ID_31
#define SOC_PETRA_NIF_ID_XAUI_0                                SOC_TMC_NIF_ID_XAUI_0
#define SOC_PETRA_NIF_ID_XAUI_3                                SOC_TMC_NIF_ID_XAUI_3
#define SOC_PETRA_NIF_ID_XAUI_7                                SOC_TMC_NIF_ID_XAUI_7
#define SOC_PETRA_NIF_ID_RXAUI_0                               SOC_TMC_NIF_ID_RXAUI_0
#define SOC_PETRA_NIF_ID_RXAUI_15                              SOC_TMC_NIF_ID_RXAUI_15
#define SOC_PETRA_NIF_ID_SGMII_0                               SOC_TMC_NIF_ID_SGMII_0
#define SOC_PETRA_NIF_ID_SGMII_31                              SOC_TMC_NIF_ID_SGMII_31
#define SOC_PETRA_NIF_ID_QSGMII_0                              SOC_TMC_NIF_ID_QSGMII_0
#define SOC_PETRA_NIF_ID_QSGMII_63                             SOC_TMC_NIF_ID_QSGMII_63
#define SOC_PETRA_NIF_ID_ILKN_0                                SOC_TMC_NIF_ID_ILKN_0
#define SOC_PETRA_NIF_ID_ILKN_1                                SOC_TMC_NIF_ID_ILKN_1
#define SOC_PETRA_IF_ID_CPU                                    SOC_TMC_IF_ID_CPU
#define SOC_PETRA_IF_ID_OLP                                    SOC_TMC_IF_ID_OLP
#define SOC_PETRA_IF_ID_RCY                                    SOC_TMC_IF_ID_RCY
#define SOC_PETRA_IF_ID_ERP                                    SOC_TMC_IF_ID_ERP
typedef SOC_TMC_INTERFACE_ID                                   SOC_PETRA_INTERFACE_ID;

#define SOC_PETRA_FC_DIRECTION_GEN                             SOC_TMC_FC_DIRECTION_GEN
#define SOC_PETRA_FC_DIRECTION_REC                             SOC_TMC_FC_DIRECTION_REC
typedef SOC_TMC_FC_DIRECTION                                   SOC_PETRA_FC_DIRECTION;

#define SOC_PETRA_COMBO_QRTT_0                                 SOC_TMC_COMBO_QRTT_0
#define SOC_PETRA_COMBO_QRTT_1                                 SOC_TMC_COMBO_QRTT_1
#define SOC_PETRA_COMBO_NOF_QRTTS                              SOC_TMC_COMBO_NOF_QRTTS
typedef SOC_TMC_COMBO_QRTT                                     SOC_PETRA_COMBO_QRTT;

#define SOC_PETRA_DEST_TYPE_QUEUE                              SOC_TMC_DEST_TYPE_QUEUE
#define SOC_PETRA_DEST_TYPE_MULTICAST                          SOC_TMC_DEST_TYPE_MULTICAST
#define SOC_PETRA_DEST_TYPE_SYS_PHY_PORT                       SOC_TMC_DEST_TYPE_SYS_PHY_PORT
#define SOC_PETRA_DEST_TYPE_LAG                                SOC_TMC_DEST_TYPE_LAG
typedef SOC_TMC_DEST_TYPE                                      SOC_PETRA_DEST_TYPE;

#define SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT              SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT
#define SOC_PETRA_DEST_SYS_PORT_TYPE_LAG                       SOC_TMC_DEST_SYS_PORT_TYPE_LAG
#define SOC_PETRA_DEST_SYS_PORT_NOF_TYPES                      SOC_TMC_DEST_SYS_PORT_NOF_TYPES
typedef SOC_TMC_DEST_SYS_PORT_TYPE                             SOC_PETRA_DEST_SYS_PORT_TYPE;

#define SOC_PETRA_CONNECTION_DIRECTION_RX                      SOC_TMC_CONNECTION_DIRECTION_RX
#define SOC_PETRA_CONNECTION_DIRECTION_TX                      SOC_TMC_CONNECTION_DIRECTION_TX
#define SOC_PETRA_CONNECTION_DIRECTION_BOTH                    SOC_TMC_CONNECTION_DIRECTION_BOTH
#define SOC_PETRA_NOF_CONNECTION_DIRECTIONS                    SOC_TMC_NOF_CONNECTION_DIRECTIONS
typedef SOC_TMC_CONNECTION_DIRECTION                           SOC_PETRA_CONNECTION_DIRECTION;

typedef SOC_TMC_DEST_SYS_PORT_INFO                             SOC_PETRA_DEST_SYS_PORT_INFO;
typedef SOC_TMC_DEST_INFO                                      SOC_PETRA_DEST_INFO;

#define SOC_PETRA_IF_ID_NONE                                   SOC_TMC_IF_ID_NONE
#define SOC_PETRA_NIF_ID_NONE                                  SOC_TMC_NIF_ID_NONE

typedef SOC_TMC_THRESH_WITH_HYST_INFO                          SOC_PETRA_THRESH_WITH_HYST_INFO;


/*
 * Soc_petra Multicast ID: 0-(16k-1)
 */
typedef SOC_TMC_MULT_ID SOC_PETRA_MULT_ID;

/*
 * Soc_petra IPQ Queue type - Traffic Class. Range: 0-7
 */
typedef SOC_TMC_TR_CLS SOC_PETRA_TR_CLS;

/* 
 * Fap Port id.range: 0 - 79.
 */
typedef SOC_TMC_FAP_PORT_ID SOC_PETRA_FAP_PORT_ID;

typedef enum
{
  SOC_PETRA_OLP_ID       = 0 ,
  SOC_PETRA_IRE_ID       = 1 ,
  SOC_PETRA_IDR_ID       = 2 ,
  SOC_PETRA_IRR_ID       = 3 ,
  SOC_PETRA_IHP_ID       = 4 ,
  SOC_PETRA_QDR_ID       = 5 ,
  SOC_PETRA_IPS_ID       = 6 ,
  SOC_PETRA_IPT_ID       = 7 ,
  SOC_PETRA_DPI_A_ID     = 8 ,
  SOC_PETRA_DPI_B_ID     = 9 ,
  SOC_PETRA_DPI_C_ID     = 10,
  SOC_PETRA_DPI_D_ID     = 11,
  SOC_PETRA_DPI_E_ID     = 12,
  SOC_PETRA_DPI_F_ID     = 13,
  SOC_PETRA_RTP_ID       = 14,
  SOC_PETRA_EGQ_ID       = 15,
  SOC_PETRA_SCH_ID       = 16,
  SOC_PETRA_CFC_ID       = 17,
  SOC_PETRA_EPNI_ID      = 18,
  SOC_PETRA_IQM_ID       = 19,
  SOC_PETRA_MMU_ID       = 20,
  SOC_PETRA_NOF_MODULES  = 21
}SOC_PETRA_MODULE_ID;

/*
 *  The MAC Lane-equivalents for non-network interface identifiers.
 *  Some Network interfaces configuration are per-MAC Lane, and not per-NIF.
 *  These equivalents are used to identify non-network interfaces in this case.
 */
#define SOC_PETRA_MAL_ID_CPU SOC_PETRA_IF_ID_CPU
#define SOC_PETRA_MAL_ID_OLP SOC_PETRA_IF_ID_OLP
#define SOC_PETRA_MAL_ID_RCY SOC_PETRA_IF_ID_RCY
#define SOC_PETRA_MAL_ID_ERP SOC_PETRA_IF_ID_ERP
#define SOC_PETRA_MAL_ID_NONE SOC_PETRA_IF_ID_NONE

#define SOC_PETRA_MAL_TYPE_NONE         SOC_TMC_MAL_TYPE_NONE
#define SOC_PETRA_MAL_TYPE_CPU          SOC_TMC_MAL_TYPE_CPU
#define SOC_PETRA_MAL_TYPE_RCY          SOC_TMC_MAL_TYPE_RCY
#define SOC_PETRA_MAL_TYPE_OLP          SOC_TMC_MAL_TYPE_OLP
#define SOC_PETRA_MAL_TYPE_ERP          SOC_TMC_MAL_TYPE_ERP
#define SOC_PETRA_MAL_TYPE_NIF          SOC_TMC_MAL_TYPE_NIF
#define SOC_PETRA_MAL_TYPE_NOF_TYPES    SOC_TMC_MAL_TYPE_NOF_TYPES
typedef SOC_TMC_MAL_EQUIVALENT_TYPE     SOC_PETRA_MAL_EQUIVALENT_TYPE;

#define SOC_PETRA_MAX_NIF_ID (SOC_PETRA_IF_NOF_NIFS - 1)

typedef enum
{
  /*
   */
  SOC_PETRA_MULTICAST_CLASS_0=0,
  /*
   */
  SOC_PETRA_MULTICAST_CLASS_1=1,
  /*
   */
  SOC_PETRA_MULTICAST_CLASS_2=2,
  /*
   */
  SOC_PETRA_MULTICAST_CLASS_3=3,
  /*
   */
  SOC_PETRA_NOF_MULTICAST_CLASSES=4,
  /*
   *  Must be the last value
   */
  SOC_PETRA_MULTICAST_CLASS_LAST

}SOC_PETRA_MULTICAST_CLASS;



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

SOC_PETRA_FAR_DEVICE_TYPE
  soc_petra_device_type_get(
    SOC_SAND_IN  uint32 internal_device_type
  );

/*********************************************************************
* MANE:
*     soc_petra_interrupt_handler
* TYPE:
*   PROC
* DATE:
*   Aug  9 2007
* FUNCTION:
*     This procedure is called from the user system ISR when
*     interrupt handling for SOC_PETRA is required.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access (and check for
*     interrupts).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_interrupt_handler(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* MANE:
*     soc_petra_fap_port_id_verify
* TYPE:
*   PROC
* DATE:
*   Aug  9 2007
* FUNCTION:
*     Verifies validity of port id
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access (and check for
*     interrupts).
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port_id -
*     The index of the FAP port to verify for validity.
*  SOC_SAND_IN  uint8          is_per_port_if_fatp
*     Only relevant if FAT Pipe is enabled.
*     If FAT Pipe enabled, and is_per_port_if_fatp is TRUE, it
*     is valid to call the API for each FAP port that is a member of Fat Pipe
*     (ports 1-4). If FALSE - the API must only be called on Fat Pipe port 1,
*     and calling it for ports 2 - 4 is an error.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_fap_port_id_verify(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID  port_id,
    SOC_SAND_IN  uint8          is_per_port_if_fatp
  );

SOC_PETRA_MAL_EQUIVALENT_TYPE
  soc_petra_mal_type_from_id(
    SOC_SAND_IN uint32 mal_id
  );

uint32
  soc_petra_if_type_from_id(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     if_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_TYPE   *interface_type
  );

uint32
  soc_petra_drop_precedence_verify(
    SOC_SAND_IN  uint32      dp_ndx
  );

uint32
  soc_petra_traffic_class_verify(
    SOC_SAND_IN  uint32      tc_ndx
  );

uint32
  soc_petra_dest_info_verify(
    SOC_SAND_IN  SOC_PETRA_DEST_INFO      *destination
  );

uint32
  soc_petra_dest_sys_port_info_verify(
    SOC_SAND_IN  SOC_PETRA_DEST_SYS_PORT_INFO      *dest_sys_port
  );

/*****************************************************
* NAME
*    soc_petra_interface_id_verify
* TYPE:
*   PROC
* DATE:
*   03/12/2007
* FUNCTION:
*   Input Verification for interface index
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_interface_id_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     if_ndx
  );

uint32
  soc_petra_mal_equivalent_id_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mal_ndx
  );

uint8
  soc_petra_is_channelized_interface_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx
  );

uint8
  soc_petra_is_channelized_nif_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx
  );

void
  soc_petra_PETRA_DEST_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DEST_SYS_PORT_INFO *info
  );

void
  soc_petra_PETRA_DEST_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DEST_INFO *info
  );

void
  soc_petra_PETRA_THRESH_WITH_HYST_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_THRESH_WITH_HYST_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_COMBO_QRTT_to_string(
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT enum_val
  );

const char*
  soc_petra_PETRA_FAR_DEVICE_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_FAR_DEVICE_TYPE enum_val
  );

const char*
  soc_petra_PETRA_INTERFACE_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_INTERFACE_TYPE enum_val
  );

const char*
  soc_petra_PETRA_INTERFACE_ID_to_string(
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID enum_val
  );

const char*
  soc_petra_PETRA_FC_DIRECTION_to_string(
    SOC_SAND_IN SOC_PETRA_FC_DIRECTION enum_val
  );

const char*
  soc_petra_PETRA_CONNECTION_DIRECTION_to_string(
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION enum_val
  );

const char*
  soc_petra_PETRA_DEST_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_DEST_TYPE enum_val,
    SOC_SAND_IN uint8       short_name
  );

const char*
  soc_petra_PETRA_DEST_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_DEST_SYS_PORT_TYPE enum_val
  );

void
  soc_petra_PETRA_INTERFACE_ID_print(
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID if_ndx
  );

void
  soc_petra_PETRA_DEST_SYS_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_DEST_SYS_PORT_INFO *info
  );

void
  soc_petra_PETRA_DEST_INFO_print(
    SOC_SAND_IN SOC_PETRA_DEST_INFO *info
  );

void
  soc_petra_PETRA_THRESH_WITH_HYST_INFO_print(
    SOC_SAND_IN SOC_PETRA_THRESH_WITH_HYST_INFO *info
  );

void
  soc_petra_PETRA_DEST_SYS_PORT_INFO_table_format_print(
    SOC_SAND_IN SOC_PETRA_DEST_SYS_PORT_INFO *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_GENERAL_INCLUDED__*/
#endif
