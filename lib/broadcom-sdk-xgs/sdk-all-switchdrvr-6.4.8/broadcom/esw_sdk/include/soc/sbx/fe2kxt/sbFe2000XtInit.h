/* -*-  Mode:C; c-basic-offset:4 -*- */
/*****************************************************************************
 * $Id: sbFe2000XtInit.h,v 1.34 Broadcom SDK $
 *
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
 *
 * ******************************************************************************/
#ifndef _SB_FE_2000XT_INIT_H_
#define _SB_FE_2000XT_INIT_H_

#include <soc/sbx/sbTypes.h>
#include <soc/types.h>
#include <soc/sbx/fe2kxt/sbFe2000Xt.h>

#define SB_G2P3_MAX_BATCH_SUPPORTED              1

#if 1
#define SB_FE2000XT_NUM_XG_INTERFACES 4

#define SB_FE2000XT_XGXS0_PHY_ADDR 0x10
#define SB_FE2000XT_XGXS1_PHY_ADDR 0x11
#define SB_FE2000XT_XGXS2_PHY_ADDR 0x12
#define SB_FE2000XT_XGXS3_PHY_ADDR 0x13
#define SB_FE2000XT_AG_PORTS_TIED_TO_X8SERDES 8 /*jk 11/02*/

/* rgf - Oct 19 2006 - Defines for QM drop queues */
#define SB_FE2000XT_QM_BASE_DROP_QUEUE_DEFAULT 0xFC
#define SB_FE2000XT_QM_MAX_DROP_QUEUES 4

/* rgf - Jan 10 2007 - Defines for QM lock queue */
#define SB_FE2000XT_QM_LOCKED_QUEUE_DEFAULT 0xFB
#define SB_FE2000XT_QM_LOCKED_PAGES_PER_PB 1

/* OAM page has to be taken from PB0 */ 
/* need two pages, in order to get an even page number for Microcode use */
#define SB_FE2000XT_QM_OAM_FREEPAGES_DEFAULT 2
#define SB_FE2000XT_QM_OAM_SRC_BUFFER    ((SB_FE2000XT_PB_PAGES_PER_PACKET_BUFFER) - (SB_FE2000XT_QM_OAM_FREEPAGES_DEFAULT))

/* rgf - Oct 17 2006 - For even batchgroups (ingress) the default size is 160 */
/* rgf - Oct 17 2006 - For odd batchgroups (egress) the default is 96 */
/* rgf - Oct 17 2006 - These match up with the default init params for LR for words0/words1 so if you */
/* rgf - Oct 17 2006 - change these upward you will need to change those as well and will not be full */
/* rgf - Oct 17 2006 - linerate through the HPP. */
/* jts - Jan 19 2007 - modified so INGRESS/EGRESS are both 160 for FE2KXT fix to bug 24043 */
#define C2_PP_DEFAULT_INGRESS_RECORD_SIZE      160
#define C2_PP_DEFAULT_EGRESS_RECORD_SIZE       160
#define C2_PP_MAX_NUM_ENTRIES_PER_CAM          128
#define C2_PP_ETHERNET_TYPE_EXCPT_MIN          1501
#define C2_PP_ETHERNET_TYPE_EXCPT_MAX          1535
#define C2_PP_CAM_VALID_ENTRY                  0x3

/* these are randomly picket labels */
#define C2_PP_DEFAULT_MAX_MPLS_RTLBL        ((1 << 20)-1)
#define C2_PP_DEFAULT_MAX_MPLS_STLBL        ((1 << 20)-1)
#define C2_PP_DEFAULT_MIN_MPLS_STLBL        (0)

/* default configuration for the PD block */
/* these are local to this .cpp file only. */
/* Native Header Ids */
#define C2_PD_DEFAULT_SBROUTE_HDRID          0
#define C2_PD_DEFAULT_PPP_HDRID              1
#define C2_PD_DEFAULT_ETH_HDRID              2
#define C2_PD_DEFAULT_LLC_HDRID              3
#define C2_PD_DEFAULT_SNAP_HDRID             4
#define C2_PD_DEFAULT_VLAN_HDRID             5
#define C2_PD_DEFAULT_MPLS_HDRID             6
#define C2_PD_DEFAULT_IPV4_HDRID             7
#define C2_PD_DEFAULT_IPV6_HDRID             8
#define C2_PD_DEFAULT_GRE_HDRID              9
#define C2_PD_DEFAULT_TCP_HDRID              10
#define C2_PD_DEFAULT_UDP_HDRID              11
#define C2_PD_DEFAULT_HIGIG_HDRID            12
#define C2_PD_DEFAULT_EMBEDDED_LENGTH_HDRID  14
#define C2_PD_DEFAULT_UNKNOWN_HDRID          0xF
/* Native Header Lengths */
#define C2_PD_DEFAULT_SBROUTE_HDRLEN         12
#define C2_PD_DEFAULT_PPP_HDRLEN             4
#define C2_PD_DEFAULT_ETH_HDRLEN             14
#define C2_PD_DEFAULT_LLC_HDRLEN             3
#define C2_PD_DEFAULT_SNAP_HDRLEN            (5+3)  /* we consider llc+snap as one "snap" header */
#define C2_PD_DEFAULT_VLAN_HDRLEN            4
#define C2_PD_DEFAULT_MPLS_HDRLEN            4
#define C2_PD_DEFAULT_IPV4_HDRLEN            0
#define C2_PD_DEFAULT_IPV6_HDRLEN            40
#define C2_PD_DEFAULT_GRE_HDRLEN             4
#define C2_PD_DEFAULT_TCP_HDRLEN             20
#define C2_PD_DEFAULT_UDP_HDRLEN             8
#define C2_PD_DEFAULT_HIGIG_HDRLEN           12

#define C2_PD_DEFAULT_IPV4_LEN_POS           4
#define C2_PD_DEFAULT_IPV4_LEN_SIZE          3
#define C2_PD_DEFAULT_IPV4_LEN_UNITS         2
#define C2_PD_DEFAULT_LEN_POS                0x1FF

#define SB_FE2000XT_PP_NUM_CAMS                4
#define SB_FE2000XT_PP_NUM_CAM_ENTRIES         256
#define SB_FE2000XT_MIN_PACKET_SIZE_IN_BYTES (64)
#define SB_FE2000XT_MAX_PACKET_SIZE_IN_BYTES (9216)

/* Default PP block configuration */
#define C2_PP_DEFAULT_ETH_HDRID              2
#define C2_PP_DEFAULT_VLAN_TYPE              0x9100
#define C2_PP_DEFAULT_VLAN_TYPE_MASK         0xFFFF
#define C2_PP_DEFAULT_LLC_MAX_LEN            0x05DC
#define C2_PP_DEFAULT_DEBUG_MODE             TRUE
#define C2_PP_DEFAULT_DEBUG_PED_HDR_MIRRORINDEX 0
#define C2_PP_DEFAULT_DEBUG_PED_HDR_COPYCOUNT   0
#define C2_PP_DEFAULT_DEBUG_PED_HDR_HDRCOPY     0
#define C2_PP_DEFAULT_DEBUG_PED_HDR_DROP        0
#define SB_FE2000XT_PP_UNKNOWN_HDR_TYPE              0xF

#define SB_FE2000XT_DEFAULT_BKT_SIZE 15
#define SB_FE2000XT_LR_20G_DUPLEX_EPOCH 403

/* DDR2 memory types */
#define SB_FE2000XT_DDR2            1
#define SB_FE2000XT_DDR2_PLUS_20    2
#define SB_FE2000XT_DDR2_PLUS_25    4
#define SB_FE2000XT_DDR_PORTS_PER_MMU 3

/* HPP Frequency properties */
#define SB_FE2000XT_HPP_FREQ_400000    400000
#define SB_FE2000XT_HPP_FREQ_375000    375000
#define SB_FE2000XT_HPP_FREQ_300000    300000
#define SB_FE2000XT_HPP_FREQ_262500    262500
#define SB_FE2000XT_HPP_FREQ_258333    258333
#define SB_FE2000XT_HPP_FREQ_250000    250000
/* SWS Frequency properties */
#define SB_FE2000XT_SWS_FREQ_360000    360000
#define SB_FE2000XT_SWS_FREQ_340000    340000
#define SB_FE2000XT_SWS_FREQ_295000    295000
#define SB_FE2000XT_SWS_FREQ_275000    275000
#define SB_FE2000XT_SWS_FREQ_254167    254167
#define SB_FE2000XT_SWS_FREQ_245000    245000
/* SPI Frequency properties */
#define SB_FE2000XT_SPI_FREQ_500000    500000
#define SB_FE2000XT_SPI_FREQ_400000    400000

#define SB_FE2000XT_LR_MAX_NUMBER_OF_CONTEXTS      16
#define SB_FE2000XT_PIPELINE_CLOCK_RATE (3.75e08)
#define SB_FE2000XT_NSEC_PER_PIPELINE_CLOCK ((1.0/SB_FE2000XT_PIPELINE_CLOCK_RATE) * 1.0e09)
#define SB_FE2000XT_CORE_CLOCK_RATE (3.4e08)
#define SB_FE2000XT_LR_24G_DUPLEX_EPOCH 503      /*dgm - Dec 4 2007 - Fred said this is the number! (was 502)*/

#define SB_FE2000XT_MIN_VALID_DELAY 5
#define SB_FE2000XT_MAX_VALID_DELAY 7

#define SB_FE2000XT_TO_QUEUE_OFFSET 128
#define SB_FE2000XT_EGRESS_QUEUE_OFFSET 64
#define SB_FE2000XT_XGM_EXPANDED_FIFO_SIZE 63

#define SB_FE2000XT_SWS_QUEUES_PER_GROUP 16
#define SB_FE2000XT_SWS_INGRESS_QUEUE_GROUPS \
 ((SB_FE2000_SWS_INIT_MAX_CONNECTIONS / 2) / SB_FE2000XT_SWS_QUEUES_PER_GROUP)

#define SB_FE2000XT_SWS_PAGES_PER_PB 4094
#define SB_FE2000XT_SWS_BYTES_PER_PAGE 192
#define SB_FE2000XT_SPI_MAX_LINES 768
#define SB_FE2000XT_SPI_MAX_LINES_PER_PORT 127
#define SB_FE2000XT_SPI_MIN_LINES_PER_PORT 7
#define SB_FE2000XT_MAX_PACKET 9216
#define SB_FE2000XT_FRAMES_PER_EPOCH 48

/* rgf - Nov 16 2007 - PT can overwrite portions of the header when
 * rgf - Nov 16 2007 - mirroring based on the mirror index.  What portion
 * rgf - Nov 16 2007 - of the header is overwritten is controlled by top
 * rgf - Nov 16 2007 - level mask words of which there are 4 (since the 
 * rgf - Nov 16 2007 - depth of overwrite reach is 16B) */
#define SB_FE2000XT_PT_NUM_HDR_WORDS (4)
#endif
typedef struct _sbFe2000XtInitParamsXgmCommon {

  uint8 bBringUp;
  uint8 bSerdesLocalLoopback;
  uint8 bLocalLoopback;
  uint8 bRemoteLoopback;
  uint8 bXactorLoopback;
  int32  nXgmiiSpeed; 
  int32  nE2eccBitmap;
} sbFe2000XtInitParamsXgmCommon_t;

typedef struct _sbFe2000XtInitParamsXgmTx {

  uint8 bE2eccEnable;
  uint32 uE2eccPeriod;
  uint32 uE2eccNumPorts;
  uint32 uE2eccEtherType;
  uint32 uE2eccCtlOp;
  uint32 uE2eccFrameLength;
  uint32 uE2eccPortEnable;
  uint64 uuE2eccMacDa;
  uint64 uuE2eccMacSa;
  int32 nHeaderMode;
  int32 nCrcMode;
  int32 nAvgIpg;
  int32 nThrotNumer;
  int32 nThrotDenom;
  uint8 bPauseEnable;
  uint8 bDiscard;
  uint8 bAnyStart;
  int32 nHiGig2Mode;
  uint32 uMacSaLo;
  uint32 uMacSaHi;
  int32 nMaxSize;
  int32 nXoff;
  int32 nXon;

} sbFe2000XtInitParamsXgmTx_t;

typedef struct _sbFe2000XtInitParamsXgmRx {

    int32 nHeaderMode;
    uint32 bStripCrc;
    uint32 bIgnoreCrc;
    uint32 bStrictPreamble;
    uint32 bPauseEnable;
    uint32 bPassCtrl;
    uint32 bAnyStart;
    int32 nHiGig2Mode;
    uint32 uMacSaLo;
    uint32 uMacSaHi;
    int32 nMaxSize;
    int32 nLocalFaultDisable;
    int32 nRemoteFaultDisable;

} sbFe2000XtInitParamsXgmRx_t;

typedef struct _sbFe2000XtInitParamsPr {
  uint8 bBringUp;

  /* rgf - Nov 13 2006 - Control free page FIFO prefetching per PRE and per PB*/
  uint8 bEnablePbPrefetch[SB_FE2000XT_PR_NUM_PRE][SB_FE2000XT_PB_NUM_PACKET_BUFFERS];
  uint8 bUsePrEnable;
  uint8 bPrEnable[SB_FE2000XT_PR_NUM_PRE];
  uint8 bCcEnable[SB_FE2000XT_PR_NUM_CC];
  /* rgf - Sep 04 2007 - If only one is true, the queue action in the CAM RAM will*/
  /* rgf - Sep 04 2007 - be set to use only that source.  If both are set the queue*/
  /* rgf - Sep 04 2007 - action will use the sum to arrive at the queue number.*/
  /* rgf - Sep 07 2007 - NOTE: Right now these are not actually used.  The provisioning always*/
  /* rgf - Sep 07 2007 - uses packet steering and never uses anything from the port queue default*/
  uint8 bCcUsePortSteering[SB_FE2000XT_PR_NUM_CC];
  uint8 bCcUsePacketSteering[SB_FE2000XT_PR_NUM_CC];
  uint32 uPbPrefetchPages[SB_FE2000XT_PR_NUM_PRE][SB_FE2000XT_PB_NUM_PACKET_BUFFERS];
  /* bms - May 03 2006 - select which packet buffer to use, the PED does not use this */
  int32 nPre0_PacketBufferSelect[SB_FE2000_MAX_PORTS_PER_SPI];
  int32 nPre1_PacketBufferSelect[SB_FE2000_MAX_PORTS_PER_SPI];
  int32 nPre2_PacketBufferSelect[SB_FE2000_MAX_AG_PORTS];
  int32 nPre3_PacketBufferSelect[SB_FE2000_MAX_AG_PORTS];
  int32 nPre4_PacketBufferSelect;
  int32 nPre5_PacketBufferSelect;
  int32 nPre6_PacketBufferSelect;
  sbFe2000InitParamsPrPipelinePriority_t PipelinePriority;

} sbFe2000XtInitParamsPr_t;

typedef struct _sbFe2000XtInitParamsPt {
  uint8 bBringUp;

  /* rgf - Nov 13 2006 - PB prefetch only applicable to PTE7*/
  uint8 bEnablePbPrefetch[SB_FE2000XT_PB_NUM_PACKET_BUFFERS];
  uint8 bUsePtEnable;
  uint8 bPtEnable[SB_FE2000XT_PT_NUM_PTE];
  sbFe2000InitParamsPtPipelinePriority_t PipelinePriority;
  uint32 uExpandedFifoSize;
  uint32 uMirrorHeaderMask[SB_FE2000XT_PT_NUM_HDR_WORDS];

} sbFe2000XtInitParamsPt_t;

typedef struct _sbFe2000XtInitParamsQm {
  uint8 bBringUp;
  uint32 uNumberOfFreePages[SB_FE2000XT_PB_NUM_PACKET_BUFFERS];
  /* rgf - Jan 17 2007 - used for Thresh1 for SPI interfaces*/
  uint32 uGlobalInterfaceFlowControlThresh[SB_FE2000XT_PR_NUM_PRE][SB_FE2000XT_PB_NUM_PACKET_BUFFERS];
  /* rgf - Jan 17 2007 - These only apply to the SPI interfaces*/
  /* rgf - Oct 03 2007 - These now apply to SPI, AG, and XG interfaces*/
  uint32 uGlobalInterfaceFlowControlThresh2[SB_FE2000XT_PR_NUM_PRE][SB_FE2000XT_PB_NUM_PACKET_BUFFERS];
  uint32 uBaseDropQueue;
  uint32 uNumberOfDropQueues;
  uint8 bLockedQueueEnable;
  uint32 uLockedQueue;
} sbFe2000XtInitParamsQm_t;

typedef struct _sbFe2000XtInitParamsPb {
  uint8 bBringUpPb;
} sbFe2000XtInitParamsPb_t;

typedef struct _sbFe2000XtInitParamsUnimacSerdesCommon {

  uint8 bBringUp;
  uint8 bMacLocalLoopback;
  uint8 bMacRemoteLoopback;
  uint8 bSerdesLocalLoopback;
  uint8 bSpeed1000Mbps;
  uint8 bSpeed100Mbps;
  uint8 bFullDuplex;
  uint8 b1000Xmode;
  uint8 bEarlyCrsMode;
  uint8 bSignalDetectMode;
  uint8 bXactorLoopback;
  uint64 uuMacAddr;

} sbFe2000XtInitParamsUnimacSerdesCommon_t;

typedef struct _sbFe2000XtInitParamsUnimacSerdesTx {

  uint8 bPadEnable;
  uint8 bPauseIgnore;
  uint8 bMacSaInsert;
  int32  nPauseQuanta;
  int32  nIpg;
  uint32 uCrcMode;

} sbFe2000XtInitParamsUnimacSerdesTx_t;

typedef struct _sbFe2000XtInitParamsUnimacSerdesRx {

  uint8 bPromiscuousEnable;
  uint8 bCrcForward;
  uint8 bPauseForward;
  uint8 bMacControlEnable;
  uint8 bPayloadLengthCheck;
  uint8 bRxErrFrameDiscard;
  uint32 uMaxTransferUnit;

} sbFe2000XtInitParamsUnimacSerdesRx_t;

typedef struct _sbFe2000XtInitParamsAg {

  uint8 bBringUp;
  uint8 bSwsLoopBackEnable;
  uint32 uSwsLoopBackCrcMode;
  uint8 bCrcCheck;
  uint32 uMaxStatCounterUnit;
  sbFe2000XtInitParamsUnimacSerdesCommon_t common[SB_FE2000_MAX_AG_PORTS];
  sbFe2000XtInitParamsUnimacSerdesRx_t     rx[SB_FE2000_MAX_AG_PORTS];
  sbFe2000XtInitParamsUnimacSerdesTx_t     tx[SB_FE2000_MAX_AG_PORTS];
    uint8 bTrainingByPass;
} sbFe2000XtInitParamsAg_t;

typedef struct _sbFe2000XtInitParamsPpCamConfigurationEntry
{
  uint32 uMask[7];
  uint32 uData[7];
  uint32 uValid;
} sbFe2000XtInitParamsPpCamConfigurationEntry_t;

typedef struct _sbFe2000XtInitParamsPpCamRamConfigurationEntry
{
  uint8 bStartHashEngine;
  uint32 uStateMask;
  uint32 uHashTemplate;
  uint32 uCheckerOffset;
  uint32 uCheckerType;
  uint32 uVariableMask;
  uint32 uVariableData;
  uint32 uState;
  uint8 bHeaderAValid;
  uint8 bHeaderBValid;
  uint32 uHeaderAType;
  uint32 uHeaderBType;
  uint32 uHeaderABaseLength;
  uint32 uHeaderBBaseLength;
  uint32 uShift;
  uint32 uLengthUnits;
  uint32 uLengthMask;
  uint32 uLengthPointer;
} sbFe2000XtInitParamsPpCamRamConfigurationEntry_t;

typedef struct _sbFe2000XtInitParamsPpCamDatabase
{
  sbFe2000XtInitParamsPpCamConfigurationEntry_t    cfgdb[SB_FE2000XT_PP_NUM_CAM_ENTRIES];
  sbFe2000XtInitParamsPpCamRamConfigurationEntry_t ramdb[SB_FE2000XT_PP_NUM_CAM_ENTRIES];
} sbFe2000XtInitParamsPpCamDatabase_t;

typedef struct _sbFe2000XtInitParamsPpInitialQueueState
{
  uint32 uHeaderType;
  uint32 uShift;
  uint32 uState;
  uint32 uVariable;
} sbFe2000XtInitParamsPpInitialQueueState_t;

typedef struct _sbFe2000XtInitParamsPpExceptionsTable
{
  uint8 bEnableAllExceptions;
}  sbFe2000XtInitParamsPpExceptionsTable_t;

typedef struct _sbFe2000XtInitParamsPp {

  uint8 bBringUp;
  uint8 bAddRouteHeader[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint8 bDeleteFirstHeader[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint8 bSourceBufferInsert[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint8 bEnablePpExceptions;
  uint8 bHashRequired;
  sbFe2000InitParamsPpIpv4Filter_t Ipv4Filter[SB_FE2000XT_PP_NUM_IPV4_FILTERS];
  sbFe2000InitParamsPpIpv6Filter_t Ipv6Filter[SB_FE2000XT_PP_NUM_IPV6_FILTERS];
  sbFe2000InitParamsPpDebugMode_t       PpByPassLrpModeConfiguration;
  sbFe2000InitParamsPpQueueConfiguration_t QueueConfiguration[SB_FE2000XT_PP_NUM_QUEUES];
  sbFe2000XtInitParamsPpInitialQueueState_t  QueueInitialState[SB_FE2000XT_PP_NUM_QUEUES];
  sbFe2000XtInitParamsPpCamDatabase_t        cam[SB_FE2000XT_PP_NUM_CAMS];
  sbFe2000XtInitParamsPpExceptionsTable_t    exceptions;

  uint32 uQueueInitialHeaderType[SB_FE2000XT_PP_NUM_QUEUES];
  uint32 uHeaderRecordSize[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint32 uDefaultNumberofQueuesToConfigure;
  uint32 uMaxMplsRouteLabel;
  uint32 uMaxMplsStatsLabel;
  uint32 uMinMplsStatsLabel;
  uint32 uEthernetTypeExceptionMin;
  uint32 uEthernetTypeExceptionMax;
  uint32 uDefaultVlanTypeValue;
  uint32 uDefaultVlanTypeMaskValue;
  uint32 uDefaultLlcMaxLength;
  uint32 uDefaultGlobalMask;

  uint32 uExceptionStreamEnable;
  uint32 uExceptionStream;

} sbFe2000XtInitParamsPp_t;

typedef struct _sbFe2000XtInitParamsMmRamConfiguration
{
  sbFe2000XtInitParamsMmInternalRamConfiguration_t InternalRamConfiguration;
  
  sbFe2000XtInitParamsMmInternalRamDmaControllerConfig_t IntRam0DmaControl;
  sbFe2000XtInitParamsMmInternalRamDmaControllerConfig_t IntRam1DmaControl;
  sbFe2000XtInitParamsNarrowPortRamConfiguration_t NarrowPortRam0Configuration;
  uint32 NarrowPortRam0Words;
  sbFe2000XtInitParamsNarrowPortRamConfiguration_t NarrowPortRam1Configuration;
  uint32 NarrowPortRam1Words;
  sbFe2000XtInitParamsWidePortRamConfiguration_t   WidePortRamConfiguration;
  uint32 WidePortRamWords;
} sbFe2000XtInitParamsMmRamConfiguration_t;

typedef struct _sbFe2000XtInitParamsMmRamConnections
{
  uint8 bPmuPrimaryClientOnWideport;
  sbFe2000XtInitParamsMmLrp0MemoryConnection_t Lrp0MemoryConnection;
  sbFe2000XtInitParamsMmLrp1MemoryConnection_t Lrp1MemoryConnection;
  sbFe2000XtInitParamsMmLrp2MemoryConnection_t Lrp2MemoryConnection;
  sbFe2000XtInitParamsMmLrp3MemoryConnection_t Lrp3MemoryConnection;
  sbFe2000XtInitParamsMmLrp4MemoryConnection_t Lrp4MemoryConnection;
  sbFe2000XtInitParamsMmPmuMemoryConnection_t  PmuMemoryConnection;
  sbFe2000XtInitParamsMmCmu0MemoryConnection_t Cmu0MemoryConnection;
  sbFe2000XtInitParamsMmCmu1MemoryConnection_t Cmu1MemoryConnection;
} sbFe2000XtInitParamsMmRamConnections_t;

typedef struct _sbFe2000XtInitParamsMmRamProtection
{
  sbFe2000XtInitParamsMmProtectionSchemes_t InternalRam0ProtectionScheme;
  sbFe2000XtInitParamsMmProtectionSchemes_t InternalRam1ProtectionScheme;
  sbFe2000XtInitParamsMmProtectionSchemes_t WidePortRamProtectionScheme;

  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort0ProcessorInterface;
  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort0DmaAccess;
  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort0SecondaryClient;
  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort0PrimaryClient;

  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort1ProcessorInterface;
  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort1DmaAccess;
  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort1SecondaryClient;
  sbFe2000XtInitParamsMmProtectionSchemes_t NarrowPort1PrimaryClient;

} sbFe2000XtInitParamsMmRamProtection_t;

typedef struct sbFe2000XtInitParamsMmDdrConfig {
  uint8 bValid;
  uint8 uValidDelay[SB_FE2000XT_DDR_PORTS_PER_MMU];
  uint8 uPhaseSelect[SB_FE2000XT_DDR_PORTS_PER_MMU];
  uint8 uQkMidRange[SB_FE2000XT_DDR_PORTS_PER_MMU];
  uint8 uQknMidRange[SB_FE2000XT_DDR_PORTS_PER_MMU];
  uint8 uXMidRange[SB_FE2000XT_DDR_PORTS_PER_MMU];
} sbFe2000XtInitParamsMmDdrConfig_t;

typedef int (*sbFe2000DdrConfigRwFunc_t)(int unit, int write, sbFe2000XtInitParamsMmDdrConfig_t *ddrconfig);

typedef struct _sbFe2000XtInitParamsMm {
  uint8 bBringUp;
  uint32 uNumberofInternalRam0Segments;
  uint32 uNumberofInternalRam1Segments;
  uint32 uNumberofNarrowPort0Segments;
  uint32 uNumberofNarrowPort1Segments;
  uint32 uNumberofWidePortSegments;

  sbFe2000XtInitParamsMmRamConfiguration_t ramconfiguration;
  sbFe2000XtInitParamsMmRamConnections_t   ramconnections;
  sbFe2000XtInitParamsMmRamProtection_t    ramprotection;
  
  uint32 uMemDiagConfigMm;
  uint32 uMemDiagRamConfigSave;
  uint32 uMemDiagClientConfigSave;
  uint32 uMemDiagSeed;

} sbFe2000XtInitParamsMm_t;

typedef struct _sbFe2000XtInitParamsLr {

  uint8 bBringUp;
  uint32 uEpochLengthInCycles;
  uint32 uNumberOfContexts;
  uint8 bLrpBypass;
  uint8 bPairedMode;
  uint8 bHeaderProcessingMode;
  uint32 uPpeRequestPaceInCycles;
  uint32 uFramesPerContext;
  uint32 uLoadCount;
  uint32 uWords0;
  uint32 uWords1;
  uint8 bStreamOnline[SB_FE2000XT_LR_NUMBER_OF_STREAMS];
  /* dgm - Sep 14 2006 - The need for StreamOffset config unclear at this time*/
  /* uint32 uStreamOffset[SB_FE2000XT_LR_NUMBER_OF_STREAMS];*/
} sbFe2000XtInitParamsLr_t;


typedef void (*sbFe2000XtResetSramDllFunc_t)(sbhandle sbh);
typedef void (*sbFe2000XtTrainDdrFunc_t)(sbhandle sbh);

typedef struct _sbFe2000XtInitParams {

  uint8 bVerify; /**< \brief When TRUE, various chip assumptions are verified */
  uint8 bBringUp; /**< \brief Global for entire chip */
  uint8 bBringUpPciOnly; /**< \brief When TRUE, only the PCI bus is brougt up.  Other blocks are out of soft reset, but not enabled */
  uint8 bSimulation; /* set to true running with LcModel, set to false for real hw -- see sbFe2000Init.c where used */
  uint8 bMemDiagLrpEnable; /* set to false to force MemDiag to reinit the LRP */
#ifdef BROADCOM_SVK
  uint8 bDVTboard;   /* skip sections of bringup with C2 DVT board */
#endif

  uint32 uDdr2MemorySelect;

  uint32 uHppFreq;
  uint32 uSwsFreq;
  uint32 uSpiFreq;
  uint32 uEpoch;

  sbFe2000DdrConfigRwFunc_t ddrConfigRw;

  sbFe2000InitParamsPci_t pci;

  sbFe2000Queues_t queues; /* < \brief SWS queues configuration database */

  sbFe2000InitParamsSpiRx_t sr[SB_FE2000_NUM_SPI_INTERFACES];
  sbFe2000InitParamsSpiTx_t st[SB_FE2000_NUM_SPI_INTERFACES];
  sbFe2000XtInitParamsXgmRx_t xgr[SB_FE2000XT_NUM_XG_INTERFACES];
  sbFe2000XtInitParamsXgmTx_t xgt[SB_FE2000XT_NUM_XG_INTERFACES];
  sbFe2000XtInitParamsXgmCommon_t xg[SB_FE2000XT_NUM_XG_INTERFACES];
  sbFe2000XtInitParamsAg_t ag[SB_FE2000_NUM_AG_INTERFACES];
  sbFe2000XtInitParamsPr_t pr;
  sbFe2000XtInitParamsPt_t pt;
  sbFe2000XtInitParamsQm_t qm;
  sbFe2000XtInitParamsPb_t pb;
  sbFe2000XtInitParamsPp_t pp;
  sbFe2000InitParamsPd_t pd;
  sbFe2000InitParamsPm_t pm;
  sbFe2000XtInitParamsMm_t mm[SB_FE2000_NUM_MM_INSTANCES];
  sbFe2000XtInitParamsMmDdrConfig_t ddrconfig[SB_FE2000_NUM_MM_INSTANCES];
  sbFe2000InitParamsCm_t cm;
  sbFe2000XtInitParamsLr_t lr;
  sbFe2000InitParamsRc_t rc[SB_FE2000_NUM_RC_INSTANCES];
} sbFe2000XtInitParams_t;

uint32 sbFe2000XtInitConvertPbEnables(sbFe2000XtInitParams_t *pInitParams, 
				       uint8 bPre, uint32 uPre);
uint32 sbFe2000XtInit(sbhandle userDeviceHandle,
                      sbFe2000XtInitParams_t *pInitParams);
sbStatus_t sbFe2000XtInitQueues(sbhandle userDeviceHandle,
                              sbFe2000Queues_t *queues);

sbStatus_t sbFe2000XtInitClearMems(sbhandle userDeviceHandle,
                                 sbFe2000XtInitParams_t *pInitParams);
void sbFe2000XtInitDefaultParams(int unit, sbFe2000XtInitParams_t *pInitParams);
sbStatus_t sbFe2000XtInitXgmRxTx(sbhandle userDeviceHandle, uint16 nMac,
                         sbFe2000XtInitParams_t *pInitParams);
void sbFe2000XtStartBubbleTimer(uint32 unit, uint32 startBubble, uint32 size);
sbStatus_t sbFe2000XtSetQueueParams(int unit, sbFe2000XtInitParams_t *ip);
sbStatus_t sbFe2000SetXtPortPages(sbhandle userDeviceHandle,
                                sbFe2000XtInitParams_t *ip,
                                int port, int mbps, int mtu);

sbStatus_t 
sbFe2000XtInitBatch(sbhandle userDeviceHandle,
                  uint32 unit,
                  sbFe2000XtInitParams_t *ip,
                  sbFe2000InitPortBatchMap_t *pBatchMap,
                  uint32 *pMaxBatch);
int 
sbFe2000XtIsBatchSupported(uint32 unit);

uint8
sbFe2000XtMaxBatch(uint32 unit);

int sbFe2000XtEccClear(int unit);

uint32
sbFe2000XtEnablePrPorts(sbhandle userDeviceHandle,
                  sbFe2000XtInitParams_t *ip);

#endif /* _SB_FE_2000_INIT_H_ */
