/* -*-  Mode:C; c-basic-offset:4 -*- */
/*****************************************************************************
 *
 * $Id: sbFe2000Init.h,v 1.23 Broadcom SDK $
 *
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
 *
 *****************************************************************************/
#ifndef _SB_FE_2000_INIT_H_
#define _SB_FE_2000_INIT_H_

#include <soc/sbx/sbTypes.h>
#include <soc/types.h>
#include <soc/sbx/fe2k/sbFe2000.h>
#include <soc/sbx/fe2k_common/sbFeISupport.h>

/* forward declarations */

/* rgf - Oct 19 2006 - Defines for QM drop queues */
#define SB_FE2000_QM_BASE_DROP_QUEUE_DEFAULT 0xFC
#define SB_FE2000_QM_MAX_DROP_QUEUES 4

/* rgf - Jan 10 2007 - Defines for QM lock queue */
#define SB_FE2000_QM_LOCKED_QUEUE_DEFAULT 0xFB
#define SB_FE2000_QM_LOCKED_PAGES_PER_PB 1

/* OAM page has to be taken from PB0 */           
#define SB_FE2000_QM_OAM_FREEPAGES_DEFAULT 1
#define SB_FE2000_QM_OAM_SRC_BUFFER    ((SB_FE2000_QM_FREEPAGES_DEFAULT) - (SB_FE2000_QM_OAM_FREEPAGES_DEFAULT))

/* rgf - Oct 17 2006 - For even batchgroups (ingress) the default size is 160 */
/* rgf - Oct 17 2006 - For odd batchgroups (egress) the default is 96 */
/* rgf - Oct 17 2006 - These match up with the default init params for LR for words0/words1 so if you */
/* rgf - Oct 17 2006 - change these upward you will need to change those as well and will not be full */
/* rgf - Oct 17 2006 - linerate through the HPP. */
/* jts - Jan 19 2007 - modified so INGRESS/EGRESS are both 128 as workaround for bug 24043 */
#define CA_PP_DEFAULT_INGRESS_RECORD_SIZE      128
#define CA_PP_DEFAULT_EGRESS_RECORD_SIZE       128
#define CA_PP_MAX_NUM_ENTRIES_PER_CAM          128
#define CA_PP_ETHERNET_TYPE_EXCPT_MIN          1501
#define CA_PP_ETHERNET_TYPE_EXCPT_MAX          1535
#define CA_PP_CAM_VALID_ENTRY                  0x3

/* these are randomly picket labels */
#define CA_PP_DEFAULT_MAX_MPLS_RTLBL        ((1 << 20)-1)
#define CA_PP_DEFAULT_MAX_MPLS_STLBL        ((1 << 20)-1)
#define CA_PP_DEFAULT_MIN_MPLS_STLBL        (0)

/* default configuration for the PD block */
/* these are local to this .cpp file only. */
/* Native Header Ids */
#define CA_PD_DEFAULT_SBROUTE_HDRID          0
#define CA_PD_DEFAULT_PPP_HDRID              1
#define CA_PD_DEFAULT_ETH_HDRID              2
#define CA_PD_DEFAULT_LLC_HDRID              3
#define CA_PD_DEFAULT_SNAP_HDRID             4
#define CA_PD_DEFAULT_VLAN_HDRID             5
#define CA_PD_DEFAULT_MPLS_HDRID             6
#define CA_PD_DEFAULT_IPV4_HDRID             7
#define CA_PD_DEFAULT_IPV6_HDRID             8
#define CA_PD_DEFAULT_GRE_HDRID              9
#define CA_PD_DEFAULT_TCP_HDRID              10
#define CA_PD_DEFAULT_UDP_HDRID              11
#define CA_PD_DEFAULT_HIGIG_HDRID            12
#define CA_PD_DEFAULT_EMBEDDED_LENGTH_HDRID  14
#define CA_PD_DEFAULT_UNKNOWN_HDRID          0xF
/* Native Header Lengths */
#define CA_PD_DEFAULT_SBROUTE_HDRLEN         12
#define CA_PD_DEFAULT_PPP_HDRLEN             4
#define CA_PD_DEFAULT_ETH_HDRLEN             14
#define CA_PD_DEFAULT_LLC_HDRLEN             3
#define CA_PD_DEFAULT_SNAP_HDRLEN            (5+3)  /* we consider llc+snap as one "snap" header */
#define CA_PD_DEFAULT_VLAN_HDRLEN            4
#define CA_PD_DEFAULT_MPLS_HDRLEN            4
#define CA_PD_DEFAULT_IPV4_HDRLEN            0
#define CA_PD_DEFAULT_IPV6_HDRLEN            40
#define CA_PD_DEFAULT_GRE_HDRLEN             4
#define CA_PD_DEFAULT_TCP_HDRLEN             20
#define CA_PD_DEFAULT_UDP_HDRLEN             8
#define CA_PD_DEFAULT_HIGIG_HDRLEN           12

#define CA_PD_DEFAULT_IPV4_LEN_POS           4
#define CA_PD_DEFAULT_IPV4_LEN_SIZE          3
#define CA_PD_DEFAULT_IPV4_LEN_UNITS         2
#define CA_PD_DEFAULT_LEN_POS                0x1FF

#define SB_FE2000_PP_NUM_CAMS                4
#define SB_FE2000_PP_NUM_CAM_ENTRIES         128
#define SB_FE2000_MIN_PACKET_SIZE_IN_BYTES (64)
#define SB_FE2000_MAX_PACKET_SIZE_IN_BYTES (9216)

/* Default PP block configuration */
#define CA_PP_DEFAULT_ETH_HDRID              2
#define CA_PP_DEFAULT_VLAN_TYPE              0x9100
#define CA_PP_DEFAULT_VLAN_TYPE_MASK         0xFFFF
#define CA_PP_DEFAULT_LLC_MAX_LEN            0x05DC
#define CA_PP_DEFAULT_DEBUG_MODE             TRUE
#define CA_PP_DEFAULT_DEBUG_PED_HDR_MIRRORINDEX 0
#define CA_PP_DEFAULT_DEBUG_PED_HDR_COPYCOUNT   0
#define CA_PP_DEFAULT_DEBUG_PED_HDR_HDRCOPY     0
#define CA_PP_DEFAULT_DEBUG_PED_HDR_DROP        0
#define SB_FE2000_PP_UNKNOWN_HDR_TYPE              0xF


#define SB_FE2000_DEFAULT_BKT_SIZE 15
#define SB_FE2000_LR_20G_DUPLEX_EPOCH 403

#if 0  
typedef enum _sbCaPpCheckerType {
  CA_PP_CHECKER_TYPE_PPP  = 0,
  CA_PP_CHECKER_TYPE_GRE  = 1,
  CA_PP_CHECKER_TYPE_IPV4 = 2,
  CA_PP_CHECKER_TYPE_IPV6 = 3,
  CA_PP_CHECKER_TYPE_ETH = 4,
  CA_PP_CHECKER_TYPE_ONE_VLAN = 5,
  CA_PP_CHECKER_TYPE_TWO_VLAN = 6,
  CA_PP_CHECKER_TYPE_THREE_VLAN = 7,
  CA_PP_CHECKER_TYPE_ONE_MPLS = 8,
  CA_PP_CHECKER_TYPE_TWO_MPLS  = 9,
  CA_PP_CHECKER_TYPE_THREE_MPLS = 10,
  CA_PP_CHECKER_TYPE_TCP  = 11,
  CA_PP_CHECKER_TYPE_UDP  = 12,
  CA_PP_CHECKER_TYPE_NOCHECKER  = 15
} sbCaPpCheckerType_t;
#endif



typedef struct _sbFe2000InitParamsXgmCommon {

  uint8 bBringUp;
  uint8 bLocalLoopback;
  uint8 bRemoteLoopback;
  uint8 bXactorLoopback;
  uint8 bXactor12gpbs ; 
} sbFe2000InitParamsXgmCommon_t;

typedef struct _sbFe2000InitParamsXgmTx {

    int32 nHeaderMode;
    int32 nCrcMode;
    int32 nAvgIpg;
    int32 nThrotNumer;
    int32 nThrotDenom;
    uint32 bPauseEnable;
    uint32 bDiscard;
    uint32 bAnyStart;
    int32 nHiGig2Mode;
    uint32 uMacSaLo;
    uint32 uMacSaHi;
    int32 nMaxSize;
    int32 nXoff;
    int32 nXon;

} sbFe2000InitParamsXgmTx_t;

typedef struct _sbFe2000InitParamsXgmRx {

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

} sbFe2000InitParamsXgmRx_t;

typedef struct _sbFe2000InitParamsPr {

  uint8 bBringUp;
  /* rgf - Nov 13 2006 - Control free page FIFO prefetching per PRE and per PB */
  uint8 bEnablePbPrefetch[SB_FE2000_PR_NUM_PRE][SB_FE2000_PB_NUM_PACKET_BUFFERS];
  uint8 bPrEnable[SB_FE2000_PR_NUM_PRE];
  uint32 uPbPrefetchPages[SB_FE2000_PR_NUM_PRE][SB_FE2000_PB_NUM_PACKET_BUFFERS];
  /* bms - May 03 2006 - select which packet buffer to use, the PED does not use this */
  uint32 nPre0_PacketBufferSelect[SB_FE2000_MAX_PORTS_PER_SPI];
  uint32 nPre1_PacketBufferSelect[SB_FE2000_MAX_PORTS_PER_SPI];
  uint32 nPre2_PacketBufferSelect[SB_FE2000_MAX_AG_PORTS];
  uint32 nPre3_PacketBufferSelect[SB_FE2000_MAX_AG_PORTS];
  uint32 nPre4_PacketBufferSelect;
  uint32 nPre5_PacketBufferSelect;
  uint32 nPre6_PacketBufferSelect;
  sbFe2000InitParamsPrPipelinePriority_t PipelinePriority;

} sbFe2000InitParamsPr_t;


typedef struct _sbFe2000InitParamsPt {
  uint8 bBringUp;

  /* rgf - Nov 13 2006 - PB prefetch only applicable to PTE7 */
  uint8 bEnablePbPrefetch[SB_FE2000_PB_NUM_PACKET_BUFFERS];
  uint8 bUsePtEnable;
  uint8 bPtEnable[SB_FE2000_PT_NUM_PTE];
  sbFe2000InitParamsPtPipelinePriority_t PipelinePriority;
  uint32 uExpandedFifoSize;

} sbFe2000InitParamsPt_t;

typedef struct _sbFe2000InitParamsQm {
  uint8 bBringUp;
  uint32 uNumberOfFreePages[SB_FE2000_PB_NUM_PACKET_BUFFERS];
  /* rgf - Jan 17 2007 - used for Thresh1 for SPI interfaces */
  uint32 uGlobalInterfaceFlowControlThresh[SB_FE2000_PR_NUM_PRE][SB_FE2000_PB_NUM_PACKET_BUFFERS];
  /* rgf - Jan 17 2007 - These only apply to the SPI interfaces */
  uint32 uGlobalInterfaceFlowControlThresh2[SB_FE2000_NUM_SPI_INTERFACES][SB_FE2000_PB_NUM_PACKET_BUFFERS];
  uint32 uBaseDropQueue;
  uint32 uNumberOfDropQueues;
  uint8 bLockedQueueEnable;
  uint32 uLockedQueue;
} sbFe2000InitParamsQm_t;

typedef struct _sbFe2000InitParamsPb {
    uint8 bBringUpPb0;
    uint8 bBringUpPb1;
} sbFe2000InitParamsPb_t;

typedef struct _sbFe2000InitParamsUnimacSerdesCommon {

  uint8 bBringUp;
  uint8 bMacLocalLoopback;
  uint8 bMacRemoteLoopback;
  uint8 bSerGlobalLoopback;
  uint8 bSpeed1000Mbps;
  uint8 bSpeed100Mbps;
  uint8 bFullDuplex;
  uint8 b1000Xmode;
  uint8 bEarlyCrsMode;
  uint8 bSignalDetectMode;
  uint8 bXactorLoopback;
  uint64 uuMacAddr;

} sbFe2000InitParamsUnimacSerdesCommon_t;

typedef struct _sbFe2000InitParamsUnimacSerdesTx {

    uint32 bPadEnable;
    uint32 bPauseIgnore;
    uint32 bMacSaInsert;
    int32  nPauseQuanta;
    int32  nIpg;
    uint32 uCrcMode;

} sbFe2000InitParamsUnimacSerdesTx_t;

typedef struct _sbFe2000InitParamsUnimacSerdesRx {

    uint8 bPromiscuousEnable;
    uint8 bCrcForward;
    uint8 bPauseForward;
    uint8 bMacControlEnable;
    uint8 bPayloadLengthCheck;
    uint8 bRxErrFrameDiscard;
    uint32 uMaxTransferUnit;

} sbFe2000InitParamsUnimacSerdesRx_t;

typedef struct _sbFe2000InitParamsAg {

    uint8 bBringUp;
    uint8 bSwsLoopBackEnable;
    uint8 uSwsLoopBackCrcMode;
    uint8 bCrcCheck;
    uint32 uMaxStatCounterUnit;
    sbFe2000InitParamsUnimacSerdesCommon_t common[SB_FE2000_MAX_AG_PORTS];
    sbFe2000InitParamsUnimacSerdesRx_t     rx[SB_FE2000_MAX_AG_PORTS];
    sbFe2000InitParamsUnimacSerdesTx_t     tx[SB_FE2000_MAX_AG_PORTS];
    uint8 bTrainingByPass;
} sbFe2000InitParamsAg_t;

typedef struct _sbFe2000InitParamsPpCamConfigurationEntry
{
  uint32 uMask[6];
  uint32 uData[6];
  uint32 uValid;
} sbFe2000InitParamsPpCamConfigurationEntry_t;

typedef struct _sbFe2000InitParamsPpCamRamConfigurationEntry
{
  uint8 bStartHashEngine;
  uint8 bStreamStateMask;
  uint8 bPreserveBit0;
  uint8 bPreserveBit1;
  uint8 bPreserveBit2;
  uint8 bPreserveBit3;
  uint8 bPreserveBit4;
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
} sbFe2000InitParamsPpCamRamConfigurationEntry_t;

typedef struct _sbFe2000InitParamsPpCamDatabase
{
  /* John I will change name if you want:) */
  sbFe2000InitParamsPpCamConfigurationEntry_t    cfgdb[SB_FE2000_PP_NUM_CAM_ENTRIES];
  sbFe2000InitParamsPpCamRamConfigurationEntry_t ramdb[SB_FE2000_PP_NUM_CAM_ENTRIES];
} sbFe2000InitParamsPpCamDatabase_t;

typedef struct _sbFe2000InitParamsPpInitialQueueState
{
  uint32 uHeaderType;
  uint32 uShift;
  uint32 uState;
  uint32 uVariable;
} sbFe2000InitParamsPpInitialQueueState_t;

typedef struct _sbFe2000InitParamsPpExceptionsTable
{
  uint8 bEnableAllExceptions;
}  sbFe2000InitParamsPpExceptionsTable_t;


typedef struct _sbFe2000InitParamsPp {

  uint8 bBringUp;
  uint8 bAddRouteHeader[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint8 bDeleteFirstHeader[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint8 bSourceBufferInsert[SB_FE2000_PP_NUM_BATCHGROUPS];
  uint8 bEnablePpExceptions;
  uint8 bHashRequired;
  sbFe2000InitParamsPpIpv4Filter_t Ipv4Filter[SB_FE2000_PP_NUM_IPV4_FILTERS];
  sbFe2000InitParamsPpIpv6Filter_t Ipv6Filter[SB_FE2000_PP_NUM_IPV6_FILTERS];
  sbFe2000InitParamsPpDebugMode_t       PpByPassLrpModeConfiguration;
  sbFe2000InitParamsPpQueueConfiguration_t QueueConfiguration[SB_FE2000_PP_NUM_QUEUES];
  sbFe2000InitParamsPpInitialQueueState_t  QueueInitialState[SB_FE2000_PP_NUM_QUEUES];
  sbFe2000InitParamsPpCamDatabase_t        cam[SB_FE2000_PP_NUM_CAMS];
  sbFe2000InitParamsPpExceptionsTable_t    exceptions;

  uint32 uQueueInitialHeaderType[SB_FE2000_PP_NUM_QUEUES];
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

} sbFe2000InitParamsPp_t;

typedef struct _sbFe2000InitParamsMmRamConfiguration
{
    sbFe2000InitParamsMmInternalRamConfiguration_t InternalRamConfiguration;
    sbFe2000InitParamsNarrowPortRamConfiguration_t NarrowPortRam0Configuration;
    uint32 NarrowPortRam0Words;
    sbFe2000InitParamsNarrowPortRamConfiguration_t NarrowPortRam1Configuration;
    uint32 NarrowPortRam1Words;
    sbFe2000InitParamsWidePortRamConfiguration_t   WidePortRamConfiguration;
    uint32 WidePortRamWords;
} sbFe2000InitParamsMmRamConfiguration_t;

typedef struct _sbFe2000InitParamsMmRamConnections
{
  uint8 bPmuPrimaryClientOnWideport;
  sbFe2000InitParamsMmLrp0MemoryConnection_t Lrp0MemoryConnection;
  sbFe2000InitParamsMmLrp1MemoryConnection_t Lrp1MemoryConnection;
  sbFe2000InitParamsMmLrp2MemoryConnection_t Lrp2MemoryConnection;
  sbFe2000InitParamsMmLrp3MemoryConnection_t Lrp3MemoryConnection;
  sbFe2000InitParamsMmLrp4MemoryConnection_t Lrp4MemoryConnection;
  sbFe2000InitParamsMmPmuMemoryConnection_t  PmuMemoryConnection;
  sbFe2000InitParamsMmCmu0MemoryConnection_t Cmu0MemoryConnection;
  sbFe2000InitParamsMmCmu1MemoryConnection_t Cmu1MemoryConnection;
} sbFe2000InitParamsMmRamConnections_t;

#if 0 /* obsolete type */
typedef struct _sbFe2000InitParamsMmRamSegmentConfiguration
{
    sbFe2000InitParamsMmProtectionSchemes_t ProtectionScheme;
    uint32 uWriteAccess;
    uint32 uReadAccess;
    uint8 bTrapNullPointer;
    uint8 bWrap;
    uint32 uAddressLimitMask;
    uint32 uBaseAddress;
} sbFe2000InitParamsMmRamSegmentConfiguration_t;

typedef struct _sbFe2000InitParamsMmInstanceConfig
{
    uint32 uNumberofInternalRam0Segments;
    uint32 uNumberofInternalRam1Segments;
    uint32 uNumberofNarrowPort0Segments;
    uint32 uNumberofNarrowPort1Segments;
    uint32 uNumberofWidePortSegments;

    sbFe2000InitParamsMmRamConfiguration_t    ramconfiguration;
    sbFe2000InitParamsMmRamConnections_t      ramconnections;
    sbFe2000InitParamsMmRamSegmentConfiguration_t InternalRam0[SB_FE2000_MM_MAX_NUM_MEMORY_SEGMENTS];
    sbFe2000InitParamsMmRamSegmentConfiguration_t InternalRam1[SB_FE2000_MM_MAX_NUM_MEMORY_SEGMENTS];
    sbFe2000InitParamsMmRamSegmentConfiguration_t NarrowPortRam0[SB_FE2000_MM_MAX_NUM_MEMORY_SEGMENTS];
    sbFe2000InitParamsMmRamSegmentConfiguration_t NarrowPortRam1[SB_FE2000_MM_MAX_NUM_MEMORY_SEGMENTS];
    sbFe2000InitParamsMmRamSegmentConfiguration_t WidePortRam[SB_FE2000_MM_MAX_NUM_MEMORY_SEGMENTS];
} sbFe2000InitParamsMmInstanceConfig_t;
#endif
typedef struct _sbFe2000InitParamsMmRamProtection
{
  sbFe2000InitParamsMmProtectionSchemes_t InternalRam0ProtectionScheme;
  sbFe2000InitParamsMmProtectionSchemes_t InternalRam1ProtectionScheme;
  sbFe2000InitParamsMmProtectionSchemes_t WidePortRamProtectionScheme;

  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort0ProcessorInterface;
  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort0DmaAccess;
  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort0SecondaryClient;
  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort0PrimaryClient;

  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort1ProcessorInterface;
  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort1DmaAccess;
  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort1SecondaryClient;
  sbFe2000InitParamsMmProtectionSchemes_t NarrowPort1PrimaryClient;

} sbFe2000InitParamsMmRamProtection_t;

typedef struct _sbFe2000InitParamsMm
{
  uint8 bBringUp;
  uint32 uNumberofInternalRam0Segments;
  uint32 uNumberofInternalRam1Segments;
  uint32 uNumberofNarrowPort0Segments;
  uint32 uNumberofNarrowPort1Segments;
  uint32 uNumberofWidePortSegments;

  sbFe2000InitParamsMmRamConfiguration_t ramconfiguration;
  sbFe2000InitParamsMmRamConnections_t   ramconnections;
  sbFe2000InitParamsMmRamProtection_t    ramprotection;

} sbFe2000InitParamsMm_t;

typedef struct _sbFe2000InitParamsLr {

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
  uint8 bStreamOnline[SB_FE2000_LR_NUMBER_OF_STREAMS];
} sbFe2000InitParamsLr_t;


typedef struct _sbFe2000InitParams {

  uint8 bVerify; /*< \brief When TRUE, various chip assumptions are verified  */
  uint8 bBringUp; /*< \brief Global for entire chip  */
  uint8 bBringUpPciOnly; /* < \brief When TRUE, only the PCI bus is brougt up.  Other blocks are out of soft reset, but not enabled */
  uint8 bSimulation; /* set to true running with LcModel, set to false for real hw -- see sbFe2000Init.c where used */

  sbFe2000ResetSramDllFunc_t resetDll;
  sbFe2000TrainDdrFunc_t trainDdr;

  sbFe2000InitParamsPci_t pci;

  sbFe2000Queues_t queues; /* < \brief SWS queues configuration database */

  sbFe2000InitParamsSpiRx_t sr[SB_FE2000_NUM_SPI_INTERFACES];
  sbFe2000InitParamsSpiTx_t st[SB_FE2000_NUM_SPI_INTERFACES];
  sbFe2000InitParamsXgmRx_t xgr[SB_FE2000_NUM_XG_INTERFACES];
  sbFe2000InitParamsXgmTx_t xgt[SB_FE2000_NUM_XG_INTERFACES];
  sbFe2000InitParamsXgmCommon_t xg[SB_FE2000_NUM_XG_INTERFACES];
  sbFe2000InitParamsAg_t ag[SB_FE2000_NUM_AG_INTERFACES];
  sbFe2000InitParamsPr_t pr;
  sbFe2000InitParamsPt_t pt;
  sbFe2000InitParamsQm_t qm;
  sbFe2000InitParamsPb_t pb;
  sbFe2000InitParamsPp_t pp;
  sbFe2000InitParamsPd_t pd;
  sbFe2000InitParamsPm_t pm;
  sbFe2000InitParamsMm_t mm[SB_FE2000_NUM_MM_INSTANCES];
  sbFe2000InitParamsCm_t cm;
  sbFe2000InitParamsLr_t lr;
  sbFe2000InitParamsRc_t rc[SB_FE2000_NUM_RC_INSTANCES];
} sbFe2000InitParams_t;
#define SB_G2P3_MAX_BATCH_SUPPORTED              1

typedef struct sbFe2000InitPortBatchMap_s {
  uint8 uBatchNumber;  /* Batch Group Numer */
  uint8 uValid;        /* If this batch to port map is valid */
  soc_pbmp_t  pbm;       /* port bitmaps of ports belonging to this batch */
} sbFe2000InitPortBatchMap_t;

uint32 sbFe2000InitConvertPbEnables(sbFe2000InitParams_t *pInitParams, 
				       uint8 bPre, uint32 uPre);
uint32 sbFe2000Init(sbhandle userDeviceHandle,
                      sbFe2000InitParams_t *pInitParams);
sbStatus_t sbFe2000InitQueues(sbhandle userDeviceHandle,
                              sbFe2000Queues_t *queues);

sbStatus_t sbFe2000InitClearMems(sbhandle userDeviceHandle,
                                 sbFe2000InitParams_t *pInitParams);
void sbFe2000InitDefaultParams(sbFe2000InitParams_t *pInitParams);
sbStatus_t sbFe2000InitXgmRxTx(sbhandle userDeviceHandle, uint16 nMac,
                         sbFe2000InitParams_t *pInitParams);
void sbFe2000StartBubbleTimer(uint32 unit, uint32 startBubble, uint32 size);
sbStatus_t sbFe2000SetQueueParams(int unit, sbFe2000InitParams_t *ip);
sbStatus_t sbFe2000SetPortPages(sbhandle userDeviceHandle,
                                sbFe2000InitParams_t *ip,
                                int port, int mbps, int mtu);
sbStatus_t 
sbFe2000InitBatch(sbhandle userDeviceHandle,
                  uint32 unit,
                  sbFe2000InitParams_t *ip,
                  sbFe2000InitPortBatchMap_t *pBatchMap,
                  uint32 *pMaxBatch);
int 
sbFe2000IsBatchSupported(uint32 unit);

uint8
sbFe2000MaxBatch(uint32 unit);

#endif /* _SB_FE_2000_INIT_H_ */
