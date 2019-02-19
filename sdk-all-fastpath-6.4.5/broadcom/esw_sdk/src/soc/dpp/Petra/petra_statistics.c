/* $Id: petra_statistics.c,v 1.7 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_statistics.h>
#include <soc/dpp/Petra/petra_api_statistics.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_STAT_TBL_ENTRY_SIZE_MAX    (2)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define  SOC_PETRA_STAT_BUILD_PARAMS(words,prim,sec,err, block_id, has_gtimer) \
           SOC_SAND_SET_BITS_RANGE((words-1), 2,0) | \
           SOC_SAND_SET_BITS_RANGE(prim, 3,3) | SOC_SAND_SET_BITS_RANGE(sec, 4,4)  | \
           SOC_SAND_SET_BITS_RANGE(err, 5,5) | SOC_SAND_SET_BITS_RANGE(block_id, 10,6) | \
           SOC_SAND_SET_BITS_RANGE(has_gtimer, 11,11)


#define  SOC_PETRA_STAT_PARAMS_GET_BLOCK_ID(params) \
          SOC_SAND_GET_BITS_RANGE(params, 10,6)

#define  SOC_PETRA_STAT_PARAMS_GET_NOF_WORDS_ID(params) \
          SOC_SAND_GET_BITS_RANGE(params, 2,0)

#define  SOC_PETRA_STAT_PARAMS_GET_HAS_GTIMER_ID(params) \
           SOC_SAND_GET_BITS_RANGE(params, 11,11)


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

/* $Id: petra_statistics.c,v 1.7 Broadcom SDK $
 * Packet counters private information.
 * This information is get updated by,
 * and get read by 'soc_petra_counter_get()'.
 * the counters that will be used by the direct call to
 * soc_petra_counter_get
 * the counters get zeroed after every direct call
 */
static SOC_PETRA_COUNTER
  Soc_petra_direct_counters[SOC_SAND_MAX_DEVICE][SOC_PETRA_NOF_COUNTER_TYPES];

/*
 * the counters that will be used by the deferred call to
 * soc_petra_counter_get (only one is allowed)
 * the report gets zeroed when registering the callback
 */
static SOC_PETRA_COUNTER
  Soc_petra_deferred_counters[SOC_SAND_MAX_DEVICE][SOC_PETRA_NOF_COUNTER_TYPES];


static SOC_PETRA_COUNTER_INFO
  Soc_petra_counters_info[SOC_PETRA_NOF_COUNTER_TYPES] =
{
   {
   SOC_PETRA_IRE_NIFA_PACKET_CNT,
   "IRE.NifaPacketCnt",
   "NIFA ingress received packet counter",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,0,1,0,SOC_PETRA_IRE_ID,0)
  },
  {
   SOC_PETRA_IRE_NIFB_PACKET_CNT,
   "IRE.NifbPacketCnt",
   "NIFB ingress received packet counter",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,0,1,0,SOC_PETRA_IRE_ID,0)
  },
  {
    SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT,
    "IDR.ReceivedPackets0 (IFP-Programmable)",
    "Number of received packets for Incoming FAP port \n\r"
    "Counted by Per-IFP programmable counter 0 n\r"
    "(configured by the soc_petra_stat_ifp_cnt_select_set API)",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,1,0,0,0,0)
  },
  {
    SOC_PETRA_IDR_RECEIVED_PKT_PORT_1_CNT,
    "IDR.ReceivedPackets1 (IFP-Programmable)",
    "Number of received packets for Incoming FAP port \n\r"
    "Counted by Per-IFP programmable counter 1 n\r"
    "(configured by the soc_petra_stat_ifp_cnt_select_set API)",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,1,0,0,0,0)
  },
  {
    SOC_PETRA_IDR_RECEIVED_PKT_PORT_2_CNT,
    "IDR.ReceivedPackets2 (IFP-Programmable)",
    "Number of received packets for Incoming FAP port \n\r"
    "Counted by Per-IFP programmable counter 2 n\r"
    "(configured by the soc_petra_stat_ifp_cnt_select_set API)",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,1,0,0,0,0)
  },
  {
    SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT,
    "IDR.ReceivedPackets3 (IFP-Programmable)",
    "Number of received packets for Incoming FAP port \n\r"
    "Counted by Per-IFP programmable counter 3 n\r"
    "(configured by the soc_petra_stat_ifp_cnt_select_set API)",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,1,0,0,0,0)
  },
  {
    SOC_PETRA_IDR_REASSEMBLY_ERR_CNT,
   "IDR.ReassemblyErrors",
   "Number of reassembly errors",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,0,1,0,0)
  },
  {
    SOC_PETRA_IQM_GLOBAL_RESOURCE_CNT,
   "IQM.FreeBdbCount",
   "Number of free BDBs (buffer descriptors \n\r"
   "buffers). The IQM uses this counter to generate decide \n\r"
   "on packet Reject and generate Flow-Control signal.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,1)
  },
  {
    SOC_PETRA_IQM_VOQ_ENQ_PROG_PACKET_CNT,
   "IQM.QEnqPktCnt   (Programmable)",
   "Counts enqueued packets (does not include discarded packets) \n\r"
   "The Queue (VOQ) for which the packets are counted can be set using\n\r"
   "soc_petra_stat_voq_cnt_select_set API",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
  },
  {
    SOC_PETRA_IQM_VOQ_DEQ_PROG_PACKET_CNT,
   "IQM.QDeqPktCnt   (Programmable)",
   "Counts dequeued packets (does not include discarded packets) \n\r"
   "The Queue (VOQ) for which the packets are counted can be set using\n\r"
   "soc_petra_stat_voq_cnt_select_set API",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
  },
  {
    SOC_PETRA_IQM_VSQ_ENQ_PROG_PACKET_CNT,
   "IQM.VSQEnqPktCnt (Programmable)",
   "Counts enqueued packets (does not include discarded packets) \n\r"
   "The Queue (VSQ) for which the packets are counted can be set using\n\r"
   "soc_petra_itm_vsq_counter_set API",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
  },
  {
    SOC_PETRA_IQM_VSQ_DEQ_PROG_PACKET_CNT,
   "IQM.VSQDeqPktCnt (Programmable)",
   "Counts dequeued packets (does not include discarded packets) \n\r"
   "The Queue (VSQ) for which the packets are counted can be set using\n\r"
   "soc_petra_itm_vsq_counter_set API",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
  },
  {
    SOC_PETRA_IQM_ENQUEUE_PACKET_CNT,
   "IQM.EnqPktCnt",
   "Counts enqueued packets (does not include discarded packets) ",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
  },
  {
   SOC_PETRA_IQM_DEQUEUE_PACKET_CNT,
   "IQM.DeqPktCnt",
   "Counts dequeued packets (does not include discarded packets) ",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
  },
  {
   SOC_PETRA_IQM_TOTAL_DISCARDED_PACKET_CNT,
   "IQM.TotDscrdPktCnt",
   "Counts all the packets discarded at the ENQ pipe. Tail Discarded.",
   NULL,
   0,
   SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
 },
 {
   SOC_PETRA_IQM_DELETED_PACKET_CNT,
   "IQM.DeqDeletePktCnt",
   "Counts packets discarded in the DEQ   \n\r"
   " process (IPS signaled Discard for the packet). Head Discarded.",
   NULL,
   0,
   SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,1)
 },
 {
   SOC_PETRA_IQM_ISP_PACKET_CNT,
   "IQM.IspPktCnt",
   "Counts ISP packets passed on IQM2IRR  \n\r"
   " interface. Note: these packets are counted also as DEQ packets (DeqPktCnt)",
   NULL,
   0,
   SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,1)
 },
 {
   SOC_PETRA_IQM_REJECTED_PACKET_LACK_RESOURCE_DBUFFS_CNT,
    "IQM.RjctDbPktCnt",
    "Counts packets reject in the ENQ process due to lack of free-Dbuffs \n\r"
    "(UnicastMiniFull-multicast). ",
   NULL,
   0,
   SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,1)
 },
 {
   SOC_PETRA_IQM_REJECTED_PACKET_LACK_RESOURCE_DBD_CNT,
    "IQM.RjctBdbPktCnt",
    "Counts packets reject in the ENQ process \n\r"
    " due to lack of free-BDBs (free-bdb counter value reduced \n\r"
    "below reject thresholds). ",
   NULL,
   0,
   SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,1)
 },
 {
   SOC_PETRA_IQM_REJECTED_BDB_PROTCT_PKT_CNT,
    "IQM.RjctBdbProtctPktCnt",
    "Counts packets rejected in the ENQ \n\r"
    "process due to panic mode protection of free-bdb \n\r"
    "resource. Note: This type of discard is not related to \n\r"
    "thresholds and drop-p.",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,1)
   },
   {
     SOC_PETRA_IQM_REJECTED_OC_BD_PKT_CNT,
    "IQM.RjctOcBdPktCnt",
    "Counts packets rejected in the ENQ \n\r"
    "process due to lack of free-BDs (QDR entrees). Note: \n\r"
    "Packet is discarded if OcBdCounter (occupied BDs) is \n\r"
    "over threshold.",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,1)
   },
   {
     SOC_PETRA_IQM_IRR_RJCT_SN_ERR_PKT_CNT,
    "IQM.RjctSnErrPktCnt",
    "Counts packets rejected in the ENQ \n\r"
    "process due to sequence number error in the IRR (oc-768c re-sequencing).",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,1,SOC_PETRA_IQM_ID,1)
   },
   {
     SOC_PETRA_IQM_IRR_RJCT_MC_ERR_PKT_CNT,
    "IQM.RjctMcErrPktCnt",
    "Counts packets rejected in the ENQ  \n\r"
    "process due to multicast error in the IRR (IRR did not \n\r"
    "have place in the MC FIFO).",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,1,SOC_PETRA_IQM_ID,1)
   },
   {
     SOC_PETRA_IQM_IDR_RJCT_RSRC_ERR_PKT_CNT,
    "IQM.RjctRsrcErrPktCnt",
    "Counts packets rejected in the ENQ \n\r"
    "process resource error signal in the PD: the IDR run out \n\r"
    "of Dbuffs while re-assembling the packet. Note: in this \n\r"
    "case the IQM reject the packet but does not release Dbuffs.",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,1,SOC_PETRA_IQM_ID,1)
   },
   {
    SOC_PETRA_IQM_IDR_RJCT_QNVALID_ERR_PKT_CNT,
    "IQM.RjctQnvalidErrPktCnt",
    "Counts packets rejected in the ENQ \n\r"
    "process due to Q not valid in the ENQ command.",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,1,SOC_PETRA_IQM_ID,1)
   },
   {
     SOC_PETRA_IPT_CRC_ERR_CNT,
    "IPT.CrcErrCnt",
    "CRC Error counter.",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,1,SOC_PETRA_IPT_ID,1)
   },
  {
    SOC_PETRA_EGQ_CPU_PACKET_CNT,
    "EGQ.CpuPacketCounter",
    "CPU received packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_IPT_PACKET_CNT,
    "EGQ.IptPacketCounter",
    "IPT received packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_FDR_PACKET_CNT,
    "EGQ.FdrPacketCounter",
    "FDR received packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_RQP_PACKET_CNT,
    "EGQ.RqpPacketCounter",
    "RQP2EHP packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_RQP_DISCARD_PACKET_CNT,
    "EGQ.RqpDiscardPacketCounter",
    "RQP2EHP discarded packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_EHP_UNICAST_PACKET_CNT,
    "EGQ.EhpUnicastPacketCounter",
    "RQP2EHP Unicast packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_CNT,
    "EGQ.EhpMulticastHighPacketCounter",
    "EHP2PQP Multicast High packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_CNT,
    "EGQ.EhpMulticastLowPacketCounter",
    "EHP2PQP Multicast Low packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_EHP_DISCARD_PACKET_CNT,
    "EGQ.EhpDiscardPacketCounter",
    "EHP2PQP discarded packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_CNT,
    "EGQ.PqpUnicastHighPacketCounter",
    "PQP2FQP Unicast High packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_CNT,
    "EGQ.PqpUnicastLowPacketCounter",
    "PQP2FQP Unicast Low packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_CNT,
    "EGQ.PqpMulticastHighPacketCounter",
    "PQP2FQP Multicast High packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_CNT,
    "EGQ.PqpMulticastLowPacketCounter",
    "QP2FQP Multicast Low packet counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_CNT,
    "EGQ.PqpUnicastHighBytesCounter",
    "PQP2FQP Unicast High bytes counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_CNT,
    "EGQ.PqpUnicastLowBytesCounter",
    "PQP2FQP Unicast Low bytes counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_CNT,
    "EGQ.PqpMulticastHighBytesCounter",
    "PQP2FQP Multicast High bytes counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_CNT,
    "EGQ.PqpMulticastLowBytesCounter",
    "PQP2FQP Multicast Low bytes counter.\n\r"
    "If CheckBwToOfp is set counts OfpToCheckBw \n\r"
    "packets, otherwise counts all packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_CNT,
    "EGQ.PqpDiscardUnicastPacketCounter",
    "PQP discarded Unicast packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_CNT,
    "EGQ.PqpDiscardMulticastPacketCounter",
    "PQP discarded Multicast packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
  {
    SOC_PETRA_EGQ_FQP_PACKET_CNT,
    "EGQ.FqpPacketCounter",
    "FQP2EPE packet counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,1)
  },
   {
    SOC_PETRA_EGQ_FABRIC_ROUTE_SOP_CNT,
    "EGQ.FsrSopCnt",
    "Counts the number of Sop received from fabric \n\r"
    "and whether there was counter overflow",
     NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,1)
   },
   {
    SOC_PETRA_EGQ_LOCAL_ROUTE_SOP_CNT,
    "EGQ.LsrSopCnt",
    "Counts the number of Sop received from local",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,1)
   },
   {
    SOC_PETRA_EGQ_CPU_INTERFACE_SOP_CNT,
    "EGQ.CsrSopCnt",
    "Counts the number of Sop received from CPU \n\r"
    "interface and whether there was counter overflow",
    NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,1)
   },
   {
     SOC_PETRA_EGQ_PACEKT_REASSEMBLY_OUTPUT_SOP_CNT,
    "EGQ.PrpSopCnt",
    "Packet Reassembly Output Sop Counter: Indicates the \n\r"
    "number of Sop packets completed packet reassembly \n\r"
    "without errors and whether there was counter overflow",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(2,0,1,0,0,1)
   },
   {
     SOC_PETRA_EGQ_PACEKT_EPE_PACKET_CNT,
    "EGQ.EpePacketCounter",
    "EPE2PNI packet counter. \n\r"
    "if MaskCheckBwToPacketDescriptor is cleared counts \n\r"
    "accordingly, if all packet descriptors are masked counts \n\r"
    "all packets.",
     NULL,
     0,
     SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EPNI_ID,1)
   },
  {
    SOC_PETRA_FCR_PROGRAMMABLE_CELLS_CNT,
   "FCR.PrgCellCnt",
   "Programmable Cells Counter: Programmable-Cells-Counter \n\r"
   "-count all the cells trapped by the match filter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FCR_CREDIT_CELLS_CNT,
   "FCR.CreditCellsCounter",
   "Credit-Cells-Counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FCT_FLOW_STATUS_CELLS_CNT,
   "FCT.FsCellsCounter",
   "Flow Status Cells Counter: Flow Status-Cells-C",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FCT_REACHABILITY_CELLS_CNT,
   "FCT.ReachCellsCounter",
   "Reachability-Cells-Counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,0)
  },
  {
    SOC_PETRA_FCT_TOTAL_CELLS_CNT,
   "FCT.TotalCellsCounter",
   "Total-Cells-Counter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FCT_CREDIT_DROP_CNT,
   "FCT.CreditDropCount",
   "Credit_drop_count -counts dropped cells  \n\r"
   "according to fifo overflow. ",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FCT_FS_DROP_CNT,
   "FCT.FsDropCount",
   "Fs_drop_count -counts dropped cells \n\r"
   "according to fifo overflow. ",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,0)
  },
  {
    SOC_PETRA_FCT_REACH_DROP_CNT,
   "FCT.ReachDropCount",
   "Reach_drop_count -counts dropped cells according to fifo overflow.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,0)
  },
  {
    SOC_PETRA_FDT_DATA_CELL_CNT,
   "FDT.DataCellCnt",
   "Data-Cell-Counter.This 31-bit counter holds  \n\r"
   "the number of data cells transmitted to the MACT (in 32B  \n\r"
   "cell for FSC and 128B for VSC )",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FDT_TDM_DATA_CELL_CNT,
   "FDT.TdmDataCellCnt",
   "TDM-Data-Cell-Counter.This 31-bit   \n\r"
   "counter holds the number of TDM cells transmitted to the MACT .",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_FDT_LOCAL_TDM_DATA_CELL_CNT,
   "FDT.LocalTdmDataCellCnt",
   "Local-TDM-Data-Cell-Counter.This    \n\r"
   " 31-bit counter holds the number of TDM cells transmitted to the EGQ",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,0,0)
  },
  {
    SOC_PETRA_SCH_CREDIT_CNT,
   "SCH.CreditCnt",
   "Counts issued credits that match the filter.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_SCH_ID,1)
  },
  {
    SOC_PETRA_IRE_CPU_PACKET_CNT,
   "IRE.CpuPacketCnt",
   "CPU ingress received packet counter",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,0,1,0,SOC_PETRA_IRE_ID,0)
  },
  {
   SOC_PETRA_IRE_OLP_PACKET_CNT,
   "IRE.OlpPacketCnt",
   "OLP ingress received packet counter",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,0,1,0,SOC_PETRA_IRE_ID,0)
  },
  {
   SOC_PETRA_IRE_RCYCLE_PACKET_CNT,
   "IRE.RecyclePacketCnt",
   "Recycle ingress received packet counter",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(2,0,1,0,SOC_PETRA_IRE_ID,0)
  },
  {
   SOC_PETRA_IQM_CNM_PKT_CNT,
   "IQM.CnmPacketCnt",
   "Counts number of generated CNM packets",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_REJECTED_CNM_PKT_CNT,
   "IQM.RjctCnmPktCnt",
   "Counts number of rejected CNM packets",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,1,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_DRAM_DYNAMIC_REJECT_PKT_CNT,
   "IQM.RjctDramDynPktCnt",
   "Packets rejected due to DRAM dynamic space protection",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,1,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_DRAM_DYNAMIC_SIZE_CNT,
   "IQM.DramDynSize",
   "Presents the DRAM dynamic space in 16 bytes resolution",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_BDB_CNT,
   "IQM.OcBdbCount",
   "Number of occupied BDBs (buffer descriptors buffers)",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_CNTING_CMD_REC_PROC_A_CNT,
   "IQM.CrpsaCmdCnt",
   "The counter holds the counting-commands received",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_CNTING_CMD_REC_PROC_B_CNT,
   "IQM.CrpsbCmdCnt",
   "The counter holds the counting-commands received",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_CPU_RD_REQ_PROC_A_CNT,
   "IQM.CrpsaCpuReqCnt",
   "The counter holds the CPU read requests received",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_CPU_RD_REQ_PROC_B_CNT,
   "IQM.CrpsbCpuReqCnt",
   "The counter holds the CPU read requests received",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_NOF_CNTERS_OVER_TH_A_CNT,
   "IQM.CrpsaOvthCntrsCnt",
   "Number of counters that are over the threshold",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
   {
   SOC_PETRA_IQM_NOF_CNTERS_OVER_TH_B_CNT,
   "IQM.CrpsbOvthCntrsCnt",
   "Number of counters that are over the threshold",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
 {
   SOC_PETRA_IQM_NOF_NON_ZERO_CNTERS_A_CNT,
   "IQM.CrpsaActCntrsCnt",
   "Number of active (non-emtpy) pairs of counters",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
 {
   SOC_PETRA_IQM_NOF_NON_ZERO_CNTERS_B_CNT,
   "IQM.CrpsbActCntrsCnt",
   "Number of active (non-emtpy) pairs of counters",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_METER_A_CIR_LB_NOF_BYTES_CNT,
   "IQM.MtrpaCblCnt",
   "Number of bytes assigned by the committed leaky bucket",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_METER_B_CIR_LB_NOF_BYTES_CNT,
   "IQM.MtrpbaCblCnt",
   "Number of bytes assigned by the committed leaky bucket",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_METER_A_EIR_LB_NOF_BYTES_CNT,
   "IQM.MtrpaEblCnt",
   "Number of bytes assigned by the excessed leaky bucket",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_IQM_METER_B_EIR_LB_NOF_BYTES_CNT,
   "IQM.MtrpbEblCnt",
   "Number of bytes assigned by the excessed leaky bucket",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,0,1,0,SOC_PETRA_IQM_ID,0)
  },
  {
   SOC_PETRA_FCR_CREDIT_CELLS_TX_CNT,
   "FCR.CreditCellsCounter",
   "Counts all the transmitted Credit cells to the mac",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
   SOC_PETRA_FCT_FLOW_STATUS_CELLS_TX_CNT,
   "FCT.FsCellsCounter",
   "Counts all the transmitted flow status cell to the mac",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,0,0)
  },
  {
    SOC_PETRA_EGQ_PACKET_AGED_CNT,
   "EGQ.PktAgedCnt",
   "Number of aged packets.",
    NULL,
    0,
    SOC_PETRA_STAT_BUILD_PARAMS(1,1,0,0,SOC_PETRA_EGQ_ID,0)
  },
};

static
  uint8
    soc_petra_counters_info_init = FALSE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 *	Indicate according to the unit if the counter
 *  owns to this device type (Soc_petra-A / Soc_petra-B)
 */
uint32
  soc_petra_stat_is_relevant_counter(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE    counter_id,
    SOC_SAND_OUT uint8                 *is_cnt_relevant
  )
{
  uint8
    is_cnt_owned = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IS_RELEVANT_COUNTER);

  switch(soc_petra_sw_db_ver_get(unit))
  {
    case SOC_PETRA_DEV_VER_A:
      if ((counter_id >= SOC_PETRA_CNT_FOR_PA_FIRST) && (counter_id <= SOC_PETRA_CNT_FOR_PA_LAST))
      {
        is_cnt_owned = TRUE;
      }
      else
      {
        is_cnt_owned = FALSE;
      }
      break;

    case SOC_PETRA_DEV_VER_B:
      if ((counter_id >= SOC_PETRA_CNT_FOR_PB_FIRST) && (counter_id <= SOC_PETRA_CNT_FOR_PB_LAST))
      {
        is_cnt_owned = TRUE;
      }
      else
      {
        is_cnt_owned = FALSE;
      }
      break;

    default:
      break;
  }
  
  *is_cnt_relevant = is_cnt_owned;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_is_relevant_counter()",counter_id,0);
}
/*****************************************************
* see remarks & definitions in the soc_petra_api_statistics.h
*****************************************************/
/*********************************************************************
*     Selects the Incoming FAP Ports for which the number of
*     words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*     SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    cntr_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IFP_CNT_SELECT_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  for (cntr_idx = 0; cntr_idx < SOC_PETRA_STAT_PER_IFP_NOF_CNTS; cntr_idx++)
  {
    fld_val = info->ifp_id[cntr_idx];
    SOC_PETRA_FLD_SET(regs->idr.counters_fap_ports_reg.fap_port[cntr_idx], fld_val, 10, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_ifp_cnt_select_set_unsafe()",0,0);
}

/*********************************************************************
*     Selects the Incoming FAP Ports for which the number of
*     words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*     SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_get_verify(
    SOC_SAND_IN  int                  unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IFP_CNT_SELECT_GET_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_ifp_cnt_select_get_verify()",0,0);
}

/*********************************************************************
*     Selects the Incoming FAP Ports for which the number of
*     words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*     SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    cntr_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IFP_CNT_SELECT_SET_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  for (cntr_idx = 0; cntr_idx < SOC_PETRA_STAT_PER_IFP_NOF_CNTS; cntr_idx++)
  {
    if (info->ifp_id[cntr_idx] != SOC_PETRA_STAT_PER_IFP_CNT_DISABLE)
    {
      res = soc_petra_fap_port_id_verify(unit, info->ifp_id[cntr_idx], TRUE);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_ifp_cnt_select_set_verify()",0,0);
}

/*********************************************************************
*     Selects the Incoming FAP Ports for which the number of
*     words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*     SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IFP_SELECT_INFO *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    cntr_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IFP_CNT_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  for (cntr_idx = 0; cntr_idx < SOC_PETRA_STAT_PER_IFP_NOF_CNTS; cntr_idx++)
  {
    SOC_PETRA_FLD_GET(regs->idr.counters_fap_ports_reg.fap_port[cntr_idx], fld_val, 10, exit);
    info->ifp_id[cntr_idx] = fld_val;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_ifp_cnt_select_get_unsafe()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VOQ_CNT_SELECT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  regs = soc_petra_regs();

  reg_val = 0;
  fld_val = info->voq_id;
  SOC_PETRA_FLD_TO_REG(regs->iqm.programmable_counter_queue_select_reg.prg_cnt_q, fld_val, reg_val, 10, exit);
  fld_val = info->enable_also_mask;
  SOC_PETRA_FLD_TO_REG(regs->iqm.programmable_counter_queue_select_reg.prg_cnt_msk, fld_val, reg_val, 20, exit);
  SOC_PETRA_REG_SET(regs->iqm.programmable_counter_queue_select_reg, reg_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_voq_cnt_select_set_unsafe()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_get_verify(
    SOC_SAND_IN  int                  unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VOQ_CNT_SELECT_GET_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_voq_cnt_select_get_verify()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VOQ_CNT_SELECT_SET_VERIFY);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->voq_id, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->enable_also_mask, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_voq_cnt_select_set_verify()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  )
{
  uint32
    reg_val,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VOQ_CNT_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->iqm.programmable_counter_queue_select_reg, reg_val, 5, exit);
  SOC_PETRA_FLD_FROM_REG(regs->iqm.programmable_counter_queue_select_reg.prg_cnt_q, fld_val, reg_val, 10, exit);
  info->voq_id = fld_val;

  SOC_PETRA_FLD_FROM_REG(regs->iqm.programmable_counter_queue_select_reg.prg_cnt_msk, fld_val, reg_val, 20, exit);
  info->enable_also_mask = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_voq_cnt_select_get_unsafe()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VSQ_CNT_SELECT_SET_UNSAFE);

  regs = soc_petra_regs();

  reg_val = 0;
  fld_val = info->vsq_in_grp_id;
  SOC_PETRA_FLD_TO_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_q, fld_val, reg_val, 20, exit);
  fld_val = info->enable_also_mask;
  SOC_PETRA_FLD_TO_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_msk, fld_val, reg_val, 22, exit);
  fld_val = info->vsq_grp_id;
  SOC_PETRA_FLD_TO_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_grp_sel, fld_val, reg_val, 24, exit);
  SOC_PETRA_REG_SET(regs->iqm.vsq_programmable_counter_select_reg, reg_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_vsq_cnt_select_set_unsafe()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_get_verify(
    SOC_SAND_IN  int                  unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VSQ_CNT_SELECT_GET_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_vsq_cnt_select_get_verify()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_VSQ_CNT_SELECT_SET_VERIFY);

  res = soc_petra_itm_vsq_idx_verify(
          info->vsq_grp_id,
          info->vsq_in_grp_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_vsq_cnt_select_set_verify()",0,0);
}

/*********************************************************************
*     Selects the Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted (not including
*     discarded packets). Optionally an enable-mask can be
*     supplied to count groups of queues.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->iqm.vsq_programmable_counter_select_reg, reg_val, 30, exit);

  SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_q, fld_val, reg_val, 20, exit);
  info->vsq_in_grp_id = fld_val;

  SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_grp_sel, fld_val, reg_val, 24, exit);
  info->vsq_grp_id = fld_val;

  SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_msk, fld_val, reg_val, 26, exit);
  info->enable_also_mask = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_vsq_cnt_select_get_unsafe()",0,0);
}
uint32
  soc_petra_counter_to_device_counters_add(
    SOC_SAND_INOUT SOC_PETRA_COUNTER  *counter,
    SOC_SAND_IN    SOC_SAND_U64       *device_value,
    SOC_SAND_IN    uint8      counter_is_ac
  )
{
  SOC_SAND_64CNT
    difference_value;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_COUNTER_TO_DEVICE_COUNTERS_ADD);

  SOC_SAND_CHECK_NULL_INPUT(counter);

  soc_sand_64cnt_clear(&difference_value);

  difference_value.u64.arr[0] = device_value->arr[0];
  difference_value.u64.arr[1] = device_value->arr[1];

  if(!counter_is_ac)
  {
    /*
     * If the counter is not auto-clear, make the
     * diff from last time.
     */
    soc_sand_u64_subtract_u64(
      &(difference_value.u64),
      &(counter->last_device_counter)
    );
  }

  soc_sand_64cnt_add_64cnt(&(counter->counter), &(difference_value));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_counter_to_device_counters_add()",0,0);
}

uint32
  soc_petra_stat_pkt_counters_collect_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE   counter_type
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_U64
    device_counter,
    reg_val;
  SOC_PETRA_REG_ADDR
    addr2;
  SOC_PETRA_STAT_COUNTER_TYPE
    counter_i,
    counter_type_begin = 0,
    counter_type_end = SOC_PETRA_NOF_COUNTER_TYPES;
  uint32
    data[SOC_PETRA_STAT_TBL_ENTRY_SIZE_MAX],
    last_addr;
  uint8
    is_cnt_relevant;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_PKT_COUNTERS_COLLECT_UNSAFE);

  soc_sand_u64_clear(&device_counter);
  soc_sand_u64_clear(&reg_val);

  last_addr = SOC_SAND_U32_MAX;

  if(counter_type != SOC_PETRA_NOF_COUNTER_TYPES)
  {
    counter_type_begin = counter_type;
    counter_type_end = counter_type + 1;
  }

  for(counter_i = counter_type_begin; counter_i < counter_type_end; counter_i++)
  {
    res = soc_petra_stat_is_relevant_counter(
            unit,
            counter_i,
            &is_cnt_relevant
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(is_cnt_relevant == FALSE)
    {
      continue;
    }

    soc_sand_u64_clear(&device_counter);
    if (Soc_petra_counters_info[counter_i].field)
    {
     /*
      * some counter may be in the same register. read once.
      */
      if (last_addr != Soc_petra_counters_info[counter_i].field->addr.base )
      {
        soc_petra_read_reg_unsafe(
          unit,
          &(Soc_petra_counters_info[counter_i].field->addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          &(reg_val.arr[0])
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

       /*
        * if counter exit in more than one registers read both.
        */
        if (SOC_PETRA_STAT_PARAMS_GET_NOF_WORDS_ID(Soc_petra_counters_info[counter_i].params))
        {
          addr2.base = Soc_petra_counters_info[counter_i].field->addr.base + sizeof(uint32);
          addr2.step = Soc_petra_counters_info[counter_i].field->addr.step;
          res = soc_petra_read_reg_unsafe(
                  unit,
                  &(addr2),
                  SOC_PETRA_DEFAULT_INSTANCE,
                  &(reg_val.arr[1])
               );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
      }
      last_addr = Soc_petra_counters_info[counter_i].field->addr.base;
      soc_petra_field_from_reg_get(
        reg_val.arr,
        Soc_petra_counters_info[counter_i].field,
        device_counter.arr
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    }
    else if (Soc_petra_counters_info[counter_i].indirect_addr)
    {
      res = soc_sand_tbl_read_unsafe(
              unit,
              device_counter.arr,
              Soc_petra_counters_info[counter_i].indirect_addr,
              sizeof(data),
              SOC_PETRA_STAT_PARAMS_GET_BLOCK_ID(Soc_petra_counters_info[counter_i].params),
              SOC_PETRA_STAT_TBL_ENTRY_SIZE_MAX * sizeof(uint32)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
    /* direct */
    res = soc_petra_counter_to_device_counters_add(
            &Soc_petra_direct_counters[unit][counter_i],
            &device_counter,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /* deferred */
    res = soc_petra_counter_to_device_counters_add(
            &Soc_petra_deferred_counters[unit][counter_i],
            &device_counter,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_pkt_counters_collect_unsafe()",counter_i,Soc_petra_counters_info[counter_i].indirect_addr);
}

/*****************************************************
* see remarks & definitions in the soc_petra_api_statistics.h
*****************************************************/
uint32
  soc_petra_stat_counter_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE   counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                 *result_ptr
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(result_ptr);

  res = soc_petra_stat_pkt_counters_collect_unsafe(
          unit,
          counter_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *result_ptr = Soc_petra_direct_counters[unit][counter_type].counter ;

  /*
   * after copying clear the private report (till next direct call...)
   */
  soc_sand_64cnt_clear(&(Soc_petra_direct_counters[unit][counter_type].counter));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_counter_get_unsafe()",0,0);
}

/*****************************************************
* see remarks & definitions in the soc_petra_api_statistics.h
*****************************************************/
uint32
  soc_petra_stat_all_counters_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_STAT_PRINT_LEVEL              print_level,
    SOC_SAND_IN  uint32                            sampling_period,
    SOC_SAND_OUT SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS   *all_statistic_counters
  )
{
  uint32
    egq_sampling_error = 0,
    epni_sampling_error = 0,
    iqm_sampling_error = 0,
    ipt_sampling_error = 0,
    sch_sampling_error = 0,
    sampling_period_value_reg = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_STAT_COUNTER_TYPE
    counter_type_i;
  uint8
    absence_gtimer = TRUE;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_cnt_relevant;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_ALL_COUNTERS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(all_statistic_counters);

  regs = soc_petra_regs();


  /*
   * With a non-zero general timer, activation of use of a timer in all the relevant
   * register blocks: we configure the timer mode, the period and the register reset
   * option, we put the trigger, and read after the period. Lastly, we deactivate the
   * timer mode. Besides, we read twice the other counters (before and after the period).
   */
  if (sampling_period)
  {
    absence_gtimer = FALSE;
    sampling_period_value_reg = sampling_period * soc_petra_chip_kilo_ticks_per_sec_get(unit);

    SOC_PETRA_FLD_SET(regs->egq.gtimer_config_cont_reg.clear_gtimer, 0x1, 20, exit);
    SOC_PETRA_FLD_SET(regs->egq.gtimer_config_reg.timer_config, sampling_period_value_reg, 22, exit);

    SOC_PETRA_FLD_SET(regs->epni.gtimer_config_cont_reg.clear_gtimer, 0x1, 25, exit);
    SOC_PETRA_FLD_SET(regs->epni.gtimer_config_reg.timer_config, sampling_period_value_reg, 27, exit);

    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_configuration_reg.iqc_cnt_by_gt, 0x1, 30, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_configuration_reg.iqc_prg_cnt_by_gt, 0x1, 31, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_configuration_reg.ste_cnt_by_gt, 0x1, 32, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_reg.gt_prd, sampling_period_value_reg, 33, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_reg.gt_rst_cntrs, 0x1, 34, exit);

    SOC_PETRA_FLD_SET(regs->ipt.gtimer_config_cont_reg.clear_gtimer, 0x1, 40, exit);
    SOC_PETRA_FLD_SET(regs->ipt.gtimer_config_reg.timer_config, sampling_period_value_reg, 42, exit);

    SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_2_reg.cnt_by_gtimer, 0x1, 45, exit);
    SOC_PETRA_FLD_SET(regs->sch.global_timer_configuration_reg.gtimer_cnt, sampling_period_value_reg, 46, exit);
    SOC_PETRA_FLD_SET(regs->sch.global_timer_configuration_reg.gtimer_clr_cnt, 0x1, 47, exit);


    SOC_PETRA_FLD_SET(regs->egq.gtimer_config_cont_reg.activate_gtimer, 0x1, 21, exit);
    SOC_PETRA_FLD_SET(regs->epni.gtimer_config_cont_reg.activate_gtimer, 0x1, 26, exit);
    SOC_PETRA_FLD_SET(regs->ipt.gtimer_config_cont_reg.activate_gtimer, 0x1, 41, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_trigger_reg.glb_cnt_trg, 0x1, 35, exit);
    SOC_PETRA_FLD_SET(regs->sch.global_timer_activation_reg.gtimer_act, 0x1, 48, exit);
  }

  /*
   * Before taking all counters, update them from the device.
   * It is also useful in case the TCM is not enabled.
   */
  res = soc_petra_stat_pkt_counters_collect_unsafe(
          unit,
          SOC_PETRA_NOF_COUNTER_TYPES
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  for(counter_type_i = 0; counter_type_i < SOC_PETRA_NOF_COUNTER_TYPES; ++counter_type_i)
  {
    res = soc_petra_stat_is_relevant_counter(
            unit,
            counter_type_i,
            &is_cnt_relevant
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(is_cnt_relevant == FALSE)
    {
      continue;
    }

    if (
        (SOC_PETRA_STAT_PARAMS_HAS_THIS_FLAVOR(Soc_petra_counters_info[counter_type_i].params, print_level))
        && ( absence_gtimer || (!SOC_PETRA_STAT_PARAMS_GET_HAS_GTIMER_ID(Soc_petra_counters_info[counter_type_i].params)) )
       )
    {
      all_statistic_counters->counters[counter_type_i] = Soc_petra_direct_counters[unit][counter_type_i].counter ;
    }
    else
    {
      soc_sand_64cnt_clear(&(all_statistic_counters->counters[counter_type_i]));
    }
    /*
     * after copying clear the private report (till next direct call...)
     */
    soc_sand_64cnt_clear(&(Soc_petra_direct_counters[unit][counter_type_i].counter));
  }

  /*
   * We wait, read a second time the counters without a gtimer,
   * then with a gtimer and deactivate the gtimer mode.
   */
  if (sampling_period)
  {
    sal_msleep(sampling_period);

    /*
     * Before taking all counters, update them from the device.
     * It is also useful in case the TCM is not enabled.
     */
    res = soc_petra_stat_pkt_counters_collect_unsafe(
            unit,
             SOC_PETRA_NOF_COUNTER_TYPES
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

    /*
     * Second reading of the counters non controlled by a general timer
     */

    for(counter_type_i = 0; counter_type_i < SOC_PETRA_NOF_COUNTER_TYPES; ++counter_type_i)
    {
      res = soc_petra_stat_is_relevant_counter(
              unit,
              counter_type_i,
              &is_cnt_relevant
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if(is_cnt_relevant == FALSE)
      {
        continue;
      }

      if (SOC_PETRA_STAT_PARAMS_HAS_THIS_FLAVOR(Soc_petra_counters_info[counter_type_i].params, print_level))
      {
        all_statistic_counters->counters[counter_type_i] = Soc_petra_direct_counters[unit][counter_type_i].counter ;
      }
      soc_sand_64cnt_clear(&(Soc_petra_direct_counters[unit][counter_type_i].counter));
    }

    /*
     * Reading of the counters controlled by a general timer
     */
    /*
     * Verification that the sampling period is finished
     */
    SOC_PETRA_FLD_GET(regs->egq.gtimer_config_cont_reg.gtimer_active, egq_sampling_error, 56, exit);
    SOC_PETRA_FLD_GET(regs->epni.gtimer_config_cont_reg.gtimer_active, epni_sampling_error, 562, exit);
    SOC_PETRA_FLD_GET(regs->ipt.gtimer_config_cont_reg.gtimer_active, ipt_sampling_error, 57, exit);
    SOC_PETRA_FLD_GET(regs->iqm.global_time_counter_trigger_reg.glb_cnt_trg, iqm_sampling_error, 58, exit);
    SOC_PETRA_FLD_GET(regs->sch.global_timer_activation_reg.gtimer_act, sch_sampling_error, 59, exit);

    /*
     * Marking the counters whose timer has not worked properly.
     */
    for(counter_type_i = 0; counter_type_i < SOC_PETRA_NOF_COUNTER_TYPES; ++counter_type_i)
    {
      res = soc_petra_stat_is_relevant_counter(
              unit,
              counter_type_i,
              &is_cnt_relevant
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if(is_cnt_relevant == FALSE)
      {
        continue;
      }

      switch(SOC_PETRA_STAT_PARAMS_GET_BLOCK_ID(Soc_petra_counters_info[counter_type_i].params))
      {
      case SOC_PETRA_EGQ_ID:
        if (egq_sampling_error)
        {
          all_statistic_counters->counters[counter_type_i].u64.arr[0] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
          all_statistic_counters->counters[counter_type_i].u64.arr[1] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
        }
        break;

      case SOC_PETRA_EPNI_ID:
        if (epni_sampling_error)
        {
          all_statistic_counters->counters[counter_type_i].u64.arr[0] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
          all_statistic_counters->counters[counter_type_i].u64.arr[1] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
        }
        break;

      case SOC_PETRA_IPT_ID:
        if (ipt_sampling_error)
        {
          all_statistic_counters->counters[counter_type_i].u64.arr[0] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
          all_statistic_counters->counters[counter_type_i].u64.arr[1] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
        }
        break;

      case SOC_PETRA_IQM_ID:
        if (iqm_sampling_error)
        {
          all_statistic_counters->counters[counter_type_i].u64.arr[0] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
          all_statistic_counters->counters[counter_type_i].u64.arr[1] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
        }
        break;

      case SOC_PETRA_SCH_ID:
        if (sch_sampling_error)
        {
          all_statistic_counters->counters[counter_type_i].u64.arr[0] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
          all_statistic_counters->counters[counter_type_i].u64.arr[1] = SOC_PETRA_STAT_COUNTER_TIMER_INVALID;
        }
        break;

      default:
        break;
      }

    }

    /*
     *  Getting back to the configuration without general timer
     */
    SOC_PETRA_FLD_SET(regs->egq.gtimer_config_cont_reg.clear_gtimer, 0x1, 60, exit);
    SOC_PETRA_FLD_SET(regs->egq.gtimer_config_cont_reg.activate_gtimer, 0x0, 61, exit);

    SOC_PETRA_FLD_SET(regs->epni.gtimer_config_cont_reg.clear_gtimer, 0x1, 60, exit);
    SOC_PETRA_FLD_SET(regs->epni.gtimer_config_cont_reg.activate_gtimer, 0x0, 61, exit);

    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_configuration_reg.iqc_cnt_by_gt, 0x0, 70, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_configuration_reg.iqc_prg_cnt_by_gt, 0x0, 71, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_configuration_reg.ste_cnt_by_gt, 0x0, 72, exit);
    SOC_PETRA_FLD_SET(regs->iqm.global_time_counter_trigger_reg.glb_cnt_trg, 0x0, 73, exit);

    SOC_PETRA_FLD_SET(regs->ipt.gtimer_config_cont_reg.clear_gtimer, 0x1, 80, exit);
    SOC_PETRA_FLD_SET(regs->ipt.gtimer_config_cont_reg.activate_gtimer, 0x0, 81, exit);

    SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_2_reg.cnt_by_gtimer, 0x0, 90, exit);
    SOC_PETRA_FLD_SET(regs->sch.global_timer_activation_reg.gtimer_act, 0x0, 91, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_all_counters_get_unsafe()",0,0);
}

/*****************************************************
* Clear specific device deferred counters
* This function is to be called only by 'soc_petra_register_device()'
*****************************************************/
uint32
  soc_petra_stat_statistics_module_initialize(
    SOC_SAND_IN int    unit
  )
{
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_TBLS
    *tables = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_STATISTICS_MODULE_INITIALIZE);

  regs = soc_petra_regs();
  soc_petra_tbls_get(&(tables));

  if (soc_petra_counters_info_init)
  {
    goto exit;
  }

  /*
   *	CAUTION: the following register has in practice a different
   *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
   */
  Soc_petra_counters_info[SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->idr.received_packets0_reg_0.received_packets0);                   /*0x299a*/
  Soc_petra_counters_info[SOC_PETRA_IDR_RECEIVED_PKT_PORT_1_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->idr.received_packets1_reg_0.received_packets1);                                     /*0x299c*/
  Soc_petra_counters_info[SOC_PETRA_IDR_RECEIVED_PKT_PORT_2_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->idr.received_packets2_reg_0.received_packets2);                                     /*0x299e*/
  Soc_petra_counters_info[SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->idr.received_packets3_reg_0.received_packets3);                                     /*0x29a0*/
  Soc_petra_counters_info[SOC_PETRA_IDR_REASSEMBLY_ERR_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->idr.reassembly_errors_reg.reassembly_errors);                                     /*0x2977*/
  Soc_petra_counters_info[SOC_PETRA_IQM_GLOBAL_RESOURCE_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.global_resource_counters_reg.free_bdb_count);                                     /*0x0552*/
  Soc_petra_counters_info[SOC_PETRA_IQM_VOQ_ENQ_PROG_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.queue_enqueue_packet_counter_reg.qenq_pkt_cnt);               /* 0x0560 */
  Soc_petra_counters_info[SOC_PETRA_IQM_VOQ_DEQ_PROG_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.queue_dequeue_packet_counter_reg.qdeq_pkt_cnt);               /* 0x0561 */
  Soc_petra_counters_info[SOC_PETRA_IQM_VSQ_ENQ_PROG_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.vsq_enqueue_packet_counter_reg.vsq_enq_pkt_cnt);              /* 0x056d */
  Soc_petra_counters_info[SOC_PETRA_IQM_VSQ_DEQ_PROG_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.vsq_dequeue_packet_counter_reg.vsq_deq_pkt_cnt);              /* 0x056e */
  Soc_petra_counters_info[SOC_PETRA_IQM_ENQUEUE_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.enqueue_packet_counter_reg.enq_pkt_cnt);                                     /*0x055b*/
  Soc_petra_counters_info[SOC_PETRA_IQM_DEQUEUE_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.dequeue_packet_counter_reg.deq_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_TOTAL_DISCARDED_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.total_discarded_packet_counter_reg.tot_dscrd_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_DELETED_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.deleted_packet_counter_reg.deq_delete_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_ISP_PACKET_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.isp_packet_counter_reg.isp_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_REJECTED_PACKET_LACK_RESOURCE_DBUFFS_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.free_resource_rejected_packet_counter1_reg.rjct_bdb_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_REJECTED_PACKET_LACK_RESOURCE_DBD_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.free_resource_rejected_packet_counter1_reg.rjct_bdb_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_REJECTED_BDB_PROTCT_PKT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.free_resource_rejected_packet_counter2_reg.rjct_bdb_protct_pkt_cnt);
  Soc_petra_counters_info[ SOC_PETRA_IQM_REJECTED_OC_BD_PKT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.free_resource_rejected_packet_counter2_reg.rjct_oc_bd_pkt_cnt);
  Soc_petra_counters_info[ SOC_PETRA_IQM_IRR_RJCT_SN_ERR_PKT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.irr_error_rejected_packet_counters_reg.rjct_sn_err_pkt_cnt);
  Soc_petra_counters_info[ SOC_PETRA_IQM_IRR_RJCT_MC_ERR_PKT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.irr_error_rejected_packet_counters_reg.rjct_mc_err_pkt_cnt);
  Soc_petra_counters_info[ SOC_PETRA_IQM_IDR_RJCT_RSRC_ERR_PKT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.idr_error_reject_packet_counters_reg.rjct_rsrc_err_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_IDR_RJCT_QNVALID_ERR_PKT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.idr_error_reject_packet_counters_reg.rjct_qnvalid_err_pkt_cnt);
  Soc_petra_counters_info[ SOC_PETRA_IPT_CRC_ERR_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->ipt.crc_error_counter_reg.crc_err_cnt);
  Soc_petra_counters_info[SOC_PETRA_EGQ_CPU_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.cpu_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_IPT_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.ipt_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_FDR_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.fdr_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_RQP_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.rqp_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_RQP_DISCARD_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.rqp_discard_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_EHP_UNICAST_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.ehp_unicast_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.ehp_multicast_high_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.ehp_multicast_low_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_EHP_DISCARD_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.ehp_discard_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_unicast_high_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_unicast_low_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_multicast_high_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_multicast_low_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[SOC_PETRA_EGQ_FQP_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->egq.fqp_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[ SOC_PETRA_EGQ_PACEKT_EPE_PACKET_CNT].indirect_addr =
    SOC_PETRA_TBL(tables->epni.epe_packet_counter_tbl.addr.base);
  Soc_petra_counters_info[ SOC_PETRA_EGQ_FABRIC_ROUTE_SOP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->egq.fabric_route_sop_counter.fsr_sop_cnt);
  Soc_petra_counters_info[ SOC_PETRA_EGQ_LOCAL_ROUTE_SOP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->egq.local_route_sop_counter.lsr_sop_cnt);
  Soc_petra_counters_info[ SOC_PETRA_EGQ_CPU_INTERFACE_SOP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->egq.cpu_interface_sop_counter.csr_sop_cnt);
  Soc_petra_counters_info[ SOC_PETRA_EGQ_PACEKT_REASSEMBLY_OUTPUT_SOP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->egq.packet_reassembly_sop_counter_0.prp_sop_cnt);

  switch(soc_petra_sw_db_ver_get(unit)) {
    case SOC_PETRA_DEV_VER_A:
#ifdef LINK_PA_LIBRARIES
      Soc_petra_counters_info[ SOC_PETRA_IRE_CPU_PACKET_CNT].indirect_addr =
        SOC_PA_TBL(tables->ire.cpu_packet_counter_tbl.addr.base);
      Soc_petra_counters_info[SOC_PETRA_IRE_OLP_PACKET_CNT].indirect_addr =
        SOC_PA_TBL(tables->ire.olp_packet_counter_tbl.addr.base);
      Soc_petra_counters_info[SOC_PETRA_IRE_RCYCLE_PACKET_CNT].indirect_addr =
        SOC_PA_TBL(tables->ire.rcy_packet_counter_tbl.addr.base);
      Soc_petra_counters_info[SOC_PETRA_IRE_NIFA_PACKET_CNT].indirect_addr =
        SOC_PA_TBL(tables->ire.nifa_packet_counter_tbl.addr.base);
      Soc_petra_counters_info[SOC_PETRA_IRE_NIFB_PACKET_CNT].indirect_addr =
        SOC_PA_TBL(tables->ire.nifb_packet_counter_tbl.addr.base);
#endif /* LINK_PA_LIBRARIES */
      break;

    case SOC_PETRA_DEV_VER_B:
#ifdef LINK_PB_LIBRARIES
      Soc_petra_counters_info[ SOC_PETRA_IRE_CPU_PACKET_CNT].field =
        SOC_PB_REG_DB_ACC_REF(regs->ire.cpu_packet_counter_reg[0].cpu_packet_counter);
      Soc_petra_counters_info[SOC_PETRA_IRE_OLP_PACKET_CNT].field =
        SOC_PB_REG_DB_ACC_REF(regs->ire.olp_packet_counter_reg[0].olp_packet_counter);
      Soc_petra_counters_info[SOC_PETRA_IRE_RCYCLE_PACKET_CNT].field =
        SOC_PB_REG_DB_ACC_REF(regs->ire.rcy_packet_counter_reg[0].rcy_packet_counter);
#endif /* LINK_PB_LIBRARIES */
      break;

    default:
      break;
  }
  
  Soc_petra_counters_info[SOC_PETRA_FCR_PROGRAMMABLE_CELLS_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.programmable_cells_counter_reg.prg_ctrl_cell_cnt);
  Soc_petra_counters_info[SOC_PETRA_FCR_CREDIT_CELLS_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.credit_cells_counter_reg.credit_cells_counter);
  Soc_petra_counters_info[SOC_PETRA_FCT_FLOW_STATUS_CELLS_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.flow_status_cells_counter_reg.fs_cells_counter);
  Soc_petra_counters_info[SOC_PETRA_FCT_REACHABILITY_CELLS_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.reachability_cells_counter_reg.reach_cells_counter);
  Soc_petra_counters_info[SOC_PETRA_FCT_TOTAL_CELLS_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.total_cells_counter_reg.total_cells_counter);
  Soc_petra_counters_info[SOC_PETRA_FCT_CREDIT_DROP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.cell_drop_counter0_reg.credit_drop_count);
  Soc_petra_counters_info[SOC_PETRA_FCT_FS_DROP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.cell_drop_counter0_reg.fs_drop_count);
  Soc_petra_counters_info[SOC_PETRA_FCT_REACH_DROP_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fcr.cell_drop_counter1_reg.reach_drop_count);
  Soc_petra_counters_info[SOC_PETRA_FDT_DATA_CELL_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fdt.transmitted_data_cells_counter_reg.data_cell_cnt);
  Soc_petra_counters_info[SOC_PETRA_FDT_TDM_DATA_CELL_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fdt.tdm_cells_counter_reg.tdm_data_cell_cnt);
  Soc_petra_counters_info[SOC_PETRA_FDT_LOCAL_TDM_DATA_CELL_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->fdt.local_tdm_cells_counter_reg.local_tdm_data_cell_cnt);
  Soc_petra_counters_info[SOC_PETRA_SCH_CREDIT_CNT].field =
    SOC_PETRA_REG_DB_ACC_REF(regs->sch.credit_counter_reg.credit_cnt);

#ifdef LINK_PB_LIBRARIES
  Soc_petra_counters_info[SOC_PETRA_IQM_CNM_PKT_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.cnm_pckt_cnt_reg.cnm_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_REJECTED_CNM_PKT_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.rjct_cnm_pckt_cnt_reg.rjct_cnm_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_DRAM_DYNAMIC_REJECT_PKT_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.dram_dyn_reject_packet_counters_reg.rjct_dram_dyn_pkt_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_DRAM_DYNAMIC_SIZE_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.dram_dyn_size_cnt_reg.dram_dyn_size);
  Soc_petra_counters_info[SOC_PETRA_IQM_BDB_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.global_resource_counters2_reg.oc_bdb_count);
  Soc_petra_counters_info[SOC_PETRA_IQM_CNTING_CMD_REC_PROC_A_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts_reg[0].crps_cmd_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_CNTING_CMD_REC_PROC_B_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts_reg[1].crps_cmd_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_CPU_RD_REQ_PROC_A_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts_reg[0].crps_cpu_req_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_CPU_RD_REQ_PROC_B_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts_reg[1].crps_cpu_req_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_NOF_CNTERS_OVER_TH_A_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts1_reg[0].crps_ovth_cntrs_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_NOF_CNTERS_OVER_TH_B_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts1_reg[1].crps_ovth_cntrs_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_NOF_NON_ZERO_CNTERS_A_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts1_reg[0].crps_act_cntrs_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_NOF_NON_ZERO_CNTERS_B_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.crps_counter_sts1_reg[1].crps_act_cntrs_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_METER_A_CIR_LB_NOF_BYTES_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.mtrpa_cbl_cnt_reg.mtrpa_cbl_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_METER_B_CIR_LB_NOF_BYTES_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.mtrpb_cbl_cnt_reg.mtrpb_cbl_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_METER_A_EIR_LB_NOF_BYTES_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.mtrpa_ebl_cnt_reg.mtrpa_ebl_cnt);
  Soc_petra_counters_info[SOC_PETRA_IQM_METER_B_EIR_LB_NOF_BYTES_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->iqm.mtrpb_ebl_cnt_reg.mtrpb_ebl_cnt);
  Soc_petra_counters_info[SOC_PETRA_FCR_CREDIT_CELLS_TX_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->fcr.credit_cells_counter_reg.credit_cells_counter);
  Soc_petra_counters_info[SOC_PETRA_FCT_FLOW_STATUS_CELLS_TX_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->fcr.flow_status_cells_counter_reg.fs_cells_counter);
  Soc_petra_counters_info[SOC_PETRA_EGQ_PACKET_AGED_CNT].field =
    SOC_PB_REG_DB_ACC_REF(regs->egq.packet_aged_counter_reg.pkt_aged_cnt);
#endif /* LINK_PB_LIBRARIES */

  soc_petra_counters_info_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_stat_statistics_module_initialize()",0,0);
}

/*****************************************************
* Clear specific device deferred counters
* This function is to be called only by 'soc_petra_register_device()'
*****************************************************/
uint32
  soc_petra_stat_statistics_device_initialize(
    SOC_SAND_IN  int    unit
  )
{
  SOC_PETRA_STAT_COUNTER_TYPE
    counter_i;
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_STATISTICS_DEVICE_INITIALIZE);

  for(counter_i = 0; counter_i < SOC_PETRA_NOF_COUNTER_TYPES; ++counter_i)
  {
    ret = SOC_SAND_OK; sal_memset(
            &Soc_petra_direct_counters[unit][counter_i],
            0x0,
            sizeof(SOC_PETRA_COUNTER)
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

    ret = SOC_SAND_OK; sal_memset(
            &Soc_petra_deferred_counters[unit][counter_i],
            0x0,
            sizeof(SOC_PETRA_COUNTER)
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_stat_statistics_device_initialize()",0,0);
}
/*****************************************************
* Clear specific device deferred counters
* This function is to be called only by 'soc_petra_register_device()'
*****************************************************/
uint32
  soc_petra_stat_statistics_device_finalize(
    SOC_SAND_IN  int    unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_stat_statistics_device_finalize()",0,0);
}

/*****************************************************
* Clear specific device deferred counter
* This function is to be called only by 'soc_petra_callback_function_register()'
*****************************************************/
uint32
  soc_petra_stat_statistics_deferred_counter_clear(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN SOC_PETRA_STAT_COUNTER_TYPE  counter_type
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_STATISTICS_DEFERRED_COUNTER_CLEAR);

  if (counter_type >= SOC_PETRA_NOF_COUNTER_TYPES)
  {
    goto exit;
  }

  ret = SOC_SAND_OK; sal_memset(
          &Soc_petra_deferred_counters[unit][counter_type],
          0x0,
          sizeof(SOC_PETRA_COUNTER)
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_set_programmable_port()",0,0);
}

/*****************************************************
*NAME
*  soc_petra_get_pkt_cnt_callback
*TYPE:
*  PROC
*DATE:
*  11-Jan-04
*FUNCTION:
*  The function that get registered in the TCM in
*  order to periodically reports on the
*  packet counters.
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32    * callback_buff -
*       Points to buffer of type SOC_PETRA_PKT_CNT_CALLBACK_STRUCT
*    uint32      size -
*       size in bytes of the buffer 'callback_buff' points to.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32     -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not SOC_PETRA_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_stat_pkt_cnt_callback_get(
    SOC_SAND_INOUT  uint32     *buffer,
    SOC_SAND_OUT    uint32     size
  )
{
  uint32
    *new_buf;
  uint32
    unit;
  SOC_PETRA_STAT_PKT_CNT_CALLBACK
    *callback_struct;
  SOC_PETRA_STAT_COUNTER_TYPE
    pkt_cnt_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_PKT_CNT_CALLBACK_GET);

  if (sizeof(SOC_PETRA_STAT_PKT_CNT_CALLBACK) != size)
  {
    /*
     * We received a buffer of incompatible size
     */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR_8015, 10, exit);
  }

  callback_struct = (SOC_PETRA_STAT_PKT_CNT_CALLBACK*) buffer;

  unit = callback_struct->unit;

  pkt_cnt_type = callback_struct->pkt_cnt_type;

  *(callback_struct->result_counter) = Soc_petra_deferred_counters[unit][pkt_cnt_type].counter;
  soc_sand_64cnt_clear_ov(&Soc_petra_deferred_counters[unit][pkt_cnt_type].counter);

  if (callback_struct->user_callback_func)
  {
    callback_struct->user_callback_func(
                       callback_struct->user_callback_id,
                       (uint32*)callback_struct->result_counter,
                       &new_buf,
                       sizeof(SOC_SAND_64CNT),
                       ex,
                       callback_struct->unit,
                       callback_struct->soc_sand_tcm_callback_id,
                       pkt_cnt_type,
                       0
                     );
    if (new_buf)
    {
      callback_struct->result_counter =  (SOC_SAND_64CNT*)new_buf;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_stat_pkt_cnt_callback_get()",0,0);
}

uint32
  soc_petra_stat_counter_info_get(
    SOC_SAND_OUT SOC_PETRA_COUNTER_INFO  *counters[SOC_PETRA_NOF_COUNTER_TYPES]
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_COUNTER_INFO_GET);

  SOC_SAND_CHECK_NULL_INPUT(counters);

  *counters = Soc_petra_counters_info;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_stat_counter_info_get()",0,0);
}

#if SOC_PETRA_DEBUG

SOC_SAND_RET
  soc_petra_stat_pkt_user_callback_get(
    SOC_SAND_IN    uint32       user_callback_id,
    SOC_SAND_INOUT uint32       *buff,
    SOC_SAND_OUT   uint32       **new_buff,
    SOC_SAND_IN    uint32       buff_size,
    SOC_SAND_IN    uint32       service_ex,
    SOC_SAND_IN    int       unit,
    SOC_SAND_IN    uint32       driver_callback_id,
    SOC_SAND_IN    uint32       meaningless_01,
    SOC_SAND_IN    uint32       meaningless_02
  )
{
  uint32
    seconds;
  const SOC_SAND_64CNT
    *counter;

  counter = (const SOC_SAND_64CNT*) buff;

  soc_sand_os_printf("in soc_petra_stat_ber_counters_user_callback_get() \n\r");
  soc_sand_os_get_time(&seconds, NULL);
  soc_sand_os_printf("soc_sand_os_get_time(): seconds(%u) \n\r", seconds);
  soc_sand_os_printf("service_ex(%u) \n\r", service_ex);
  soc_sand_os_printf("unit(%u) \n\r", unit);
  soc_sand_os_printf("driver_callback_id(%u) \n\r", driver_callback_id);

  soc_sand_64cnt_print(counter, TRUE);
  soc_sand_os_printf("\n\r");

  *new_buff = buff;

  return SOC_SAND_OK;
}


/*
 * Convert counter type to const string.
 */
const char*
  soc_petra_PETRA_STAT_COUNTER_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_STAT_COUNTER_TYPE counter_type,
    SOC_SAND_IN uint32                short_format
  )
{
  const char
    *str = NULL;

  if(counter_type < SOC_PETRA_NOF_COUNTER_TYPES)
  {
    if(short_format)
    {
      str = Soc_petra_counters_info[counter_type].short_name;
    }
    else
    {
      str = Soc_petra_counters_info[counter_type].long_name;
    }
  }
  else
  {
    str = "soc_petra_PETRA_COUNTER_TYPE_to_str() got Invalid 'counter_type'";
  }

  return str;
}

SOC_PETRA_COUNTER_INFO* soc_petra_stat_counter_info_ptr_get(void)
{
  return Soc_petra_counters_info;
}

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
