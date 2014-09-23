/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmuictrl.h
*
* @purpose  
*
* @component 
*
* @comments 
*
* @create 01/01/2006
*
* @author  dsatyanarayana
* @end
*
******************************************************************************/
#ifndef _PIMSM_CTRL_H_
#define _PIMSM_CTRL_H_

#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "intf_bitset.h"
#include "mfc_api.h"


/* eventdata w.r.t event MCAST_EVENT_PIMSM_INTF_OPER_MODE_CHANGE */
typedef struct pimsmIntfStatus_s {
   L7_uint32 rtrIfNum;
   L7_uint32 intIfNum;
   L7_inet_addr_t *intfAddr;
   L7_BOOL  isEnabled;   
} pimsmIntfStatus_t;


/* eventdata w.r.t event MCAST_EVENT_PIMSM_STATIC_RP_SET */
typedef struct pimsmStaticRPdata_s {
   L7_BOOL operMode; /* TRUE - add, FALSE - delete */
   L7_inet_addr_t *pPimsmStaticRPGrpAddress;
   L7_uchar8 pimsmStaticRPGrpPrefixLength;
   L7_inet_addr_t *pPimsmStaticRPRPAddress;
   L7_BOOL pimsmStaticRPOverrideDynamic;
} pimsmStaticRPdata_t;

/* eventdata w.r.t event PIMSM_CAND_RP_SET */
typedef struct pimsmCandRPdata_s {
    L7_BOOL        operMode; /* TRUE - add, FALSE - delete */
   L7_uint32      intIfNum;  /* router interface number on which cand-Rp is configured */
   L7_inet_addr_t *pCandRPAddress;   
   L7_inet_addr_t *pGrpAddress;
   L7_uint32      grpPrefixLength;
    L7_BOOL empty;
} pimsmCandRPdata_t;

/* eventdata w.r.t event PIMSM_CAND_BSR_SET */
typedef struct pimsmCandBSRdata_s {
   L7_BOOL operMode; /* TRUE - add, FALSE - delete */
   L7_uint32      intIfNum;  /* router interface number on which cand-Bsr is configured */
   L7_inet_addr_t *pCandBSRAddress;   
   L7_uint32          candBSRPriority;       
   L7_uint32          candBSRHashMaskLength;
   L7_uint32      candBSRScope; 
} pimsmCandBSRdata_t;

extern L7_RC_t pimsmDefaultHashMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);

/* following functions are defined in pimsmMain.c */
extern L7_RC_t pimsmMemoryInit (L7_uchar8 addrFamily);
extern L7_RC_t pimsmMemoryDeInit (L7_uchar8 addrFamily);

extern MCAST_CB_HNDL_t pimsmCtrlBlockInit(L7_uchar8 family, L7_BOOL warmRestart);
extern L7_RC_t pimsmCtrlBlockDeInit(MCAST_CB_HNDL_t pimsmCbHandle);

extern L7_RC_t pimsmDeRegisterEventChanges();
extern L7_RC_t pimsmRegisterEventChanges();
extern L7_RC_t pimsmEventProcess(L7_uchar8 familyType, L7_uint32 event,
                                 void *pMsg);
/*extern L7_RC_t pimsmMRouteDeleteAll(L7_VOIDPTR handle);*/
extern void pimsmRtoStartTimer (MCAST_CB_HNDL_t pimsmCbHandle);
extern void pimsmRTOTimerExpiresHandler (void* param);
/*
***** draft-ietf-pim-mib-v2-03.txt *****
--
-- The PIM Interface Table
--

PimInterfaceEntry ::= SEQUENCE {
    pimInterfaceIfIndex              InterfaceIndex,
    pimInterfaceIPVersion            InetVersion,
    pimInterfaceAddressType          InetAddressType,
    pimInterfaceAddress              InetAddress,
    pimInterfaceNetMaskLength        InetAddressPrefixLength,
    pimInterfaceDR                   InetAddress,
    pimInterfaceHelloInterval        Unsigned32,
    pimInterfaceTrigHelloInterval    Unsigned32,
    pimInterfaceJoinPruneInterval    Unsigned32,
    pimInterfaceDFElectionRobustness Unsigned32,
    pimInterfaceHelloHoldtime        Unsigned32,
    pimInterfaceJoinPruneHoldtime    Unsigned32,
    pimInterfaceUseLanPruneDelay     TruthValue,
    pimInterfacePropagationDelay     Unsigned32,
    pimInterfaceOverrideInterval     Unsigned32,
    pimInterfaceUseGenerationID      TruthValue,
    pimInterfaceGenerationIDValue    Unsigned32,
    pimInterfaceUseDRPriority        TruthValue,
    pimInterfaceDRPriority           Unsigned32,
    pimInterfaceLanDelayEnabled      TruthValue,
    pimInterfaceEffectPropagDelay    Unsigned32,
    pimInterfaceEffectOverrideIvl    Unsigned32,
    pimInterfaceSuppressionEnabled   TruthValue,
    pimInterfaceBidirCapable         TruthValue,
    pimInterfaceDRPriorityEnabled    TruthValue,
    pimInterfaceBSRBorder            TruthValue,
    pimInterfaceStatus               RowStatus
}
    INDEX      { pimInterfaceIfIndex,
                 pimInterfaceIPVersion }



--                 
-- The PIM Neighbor Table  (pimsmNeighborEntry_t)
--
PimNeighborEntry ::= SEQUENCE {
    pimNeighborIfIndex              InterfaceIndex,
    pimNeighborAddressType          InetAddressType,
    pimNeighborAddress              InetAddress,
    pimNeighborUpTime               TimeTicks,
    pimNeighborExpiryTime           TimeTicks,
    pimNeighborLanPruneDelayPresent TruthValue,
    pimNeighborPropagationDelay     Unsigned32,
    pimNeighborOverrideInterval     Unsigned32,
    pimNeighborTBit                 TruthValue,
    pimNeighborGenerationIDPresent  TruthValue,
    pimNeighborGenerationIDValue    Unsigned32,
    pimNeighborBidirCapable         TruthValue,
    pimNeighborDRPriorityPresent    TruthValue,
    pimNeighborDRPriority           Unsigned32
}

    INDEX      { pimNeighborIfIndex,
                 pimNeighborAddressType,
                 pimNeighborAddress }
--
-- The PIM (*,G) State Table
--                 
                 
PimStarGEntry ::= SEQUENCE {
    pimStarGAddressType             InetAddressType,
    pimStarGGrpAddress              InetAddress,
    pimStarGUpTime                  TimeTicks,
    pimStarGPimMode                 PimMode,
    pimStarGRPAddress               InetAddress,
    pimStarGRPOrigin                PimGroupMappingOriginType,
    pimStarGRPIsLocal               TruthValue,
    pimStarGUpstreamJoinState       INTEGER,
    pimStarGUpstreamJoinTimer       TimeTicks,
    pimStarGUpstreamNeighbor        InetAddress,
    pimStarGRPFIfIndex              InterfaceIndexOrZero,
    pimStarGRPFNextHop              InetAddress,
    pimStarGRPFRouteProtocol        IANAipRouteProtocol,
    pimStarGRPFRouteAddress         InetAddress,
    pimStarGRPFRoutePrefixLength    InetAddressPrefixLength,
    pimStarGRPFRouteMetricPref      Unsigned32,
    pimStarGRPFRouteMetric          Unsigned32
}
    INDEX      { pimStarGAddressType,
                 pimStarGGrpAddress }

--
-- The PIM (*,G,I) State Table
--

PimStarGIEntry ::= SEQUENCE {
    pimStarGIIfIndex                 InterfaceIndex,
    pimStarGIUpTime                  TimeTicks,
    pimStarGILocalMembership         TruthValue,
    pimStarGIJoinPruneState          INTEGER,
    pimStarGIPrunePendingTimer       TimeTicks,
    pimStarGIJoinExpiryTimer         TimeTicks,
    pimStarGIAssertState             INTEGER,
    pimStarGIAssertTimer             TimeTicks,
    pimStarGIAssertWinnerAddress     InetAddress,
    pimStarGIAssertWinnerMetricPref  Unsigned32,
    pimStarGIAssertWinnerMetric      Unsigned32
}
   INDEX      { pimStarGAddressType,
                 pimStarGGrpAddress,
                 pimStarGIIfIndex }                 

--
-- The PIM (S,G) State Table
--

PimSGEntry ::= SEQUENCE {
    pimSGAddressType             InetAddressType,
    pimSGGrpAddress              InetAddress,
    pimSGSrcAddress              InetAddress,
    pimSGUpTime                  TimeTicks,
    pimSGPimMode                 PimMode,
    pimSGUpstreamJoinState       INTEGER,
    pimSGUpstreamJoinTimer       TimeTicks,
    pimSGUpstreamNeighbor        InetAddress,
    pimSGRPFIfIndex              InterfaceIndexOrZero,
    pimSGRPFNextHop              InetAddress,
    pimSGRPFRouteProtocol        IANAipRouteProtocol,
    pimSGRPFRouteAddress         InetAddress,
    pimSGRPFRoutePrefixLength    InetAddressPrefixLength,
    pimSGRPFRouteMetricPref      Unsigned32,
    pimSGRPFRouteMetric          Unsigned32,
    pimSGSPTBit                  TruthValue,
    pimSGKeepaliveTimer          TimeTicks,
    pimSGDRRegisterState         INTEGER,
    pimSGDRRegisterStopTimer     TimeTicks,
    pimSGRPRegisterPMBRAddress   InetAddress
}
   INDEX      { pimSGAddressType,
                 pimSGGrpAddress,
                 pimSGSrcAddress }

--
-- The PIM (S,G,I) State Table
--
PimSGIEntry ::= SEQUENCE {
    pimSGIIfIndex                 InterfaceIndex,
    pimSGIUpTime                  TimeTicks,
    pimSGILocalMembership         TruthValue,
    pimSGIJoinPruneState          INTEGER,
    pimSGIPrunePendingTimer       TimeTicks,
    pimSGIJoinExpiryTimer         TimeTicks,
    pimSGIAssertState             INTEGER,
    pimSGIAssertTimer             TimeTicks,
    pimSGIAssertWinnerAddress     InetAddress,
    pimSGIAssertWinnerMetricPref  Unsigned32,
    pimSGIAssertWinnerMetric      Unsigned32
}
    INDEX      { pimSGAddressType,
                 pimSGGrpAddress,
                 pimSGSrcAddress,
                 pimSGIIfIndex }                 
--
-- The PIM (S,G,rpt) State Table
--
PimSGRptEntry ::= SEQUENCE {
    pimSGRptSrcAddress             InetAddress,
    pimSGRptUpTime                 TimeTicks,
    pimSGRptUpstreamPruneState     INTEGER,
    pimSGRptUpstreamOverrideTimer  TimeTicks
}
   INDEX      { pimStarGAddressType,
                 pimStarGGrpAddress,
                 pimSGRptSrcAddress }

--
-- The PIM (S,G,rpt,I) State Table
--

PimSGRptIEntry ::= SEQUENCE {
    pimSGRptIIfIndex            InterfaceIndex,
    pimSGRptIUpTime             TimeTicks,
    pimSGRptILocalMembership    TruthValue,
    pimSGRptIJoinPruneState     INTEGER,
    pimSGRptIPrunePendingTimer  TimeTicks,
    pimSGRptIPruneExpiryTimer   TimeTicks
}
    INDEX      { pimStarGAddressType,
                 pimStarGGrpAddress,
                 pimSGRptSrcAddress,
                 pimSGRptIIfIndex }
--
-- The PIM Group Mapping Table(pimsmGroupMappingEntry_t)
-- this was called The PIM RP-Set Table in old RFC

PimGroupMappingEntry ::= SEQUENCE {
    pimGroupMappingOrigin           PimGroupMappingOriginType,
    pimGroupMappingAddressType      InetAddressType,
    pimGroupMappingGrpAddress       InetAddress,
    pimGroupMappingGrpPrefixLength  InetAddressPrefixLength,
    pimGroupMappingRPAddress        InetAddress,
    pimGroupMappingPimMode          PimMode,
    pimGroupMappingPrecedence       Unsigned32,
    pimGroupMappingActive           TruthValue
}
    INDEX      { pimGroupMappingOrigin,
                 pimGroupMappingAddressType,
                 pimGroupMappingGrpAddress,
                 pimGroupMappingGrpPrefixLength,
                 pimGroupMappingRPAddress }


--
-- The BSR RP-Set Table
-- this was part of The PIM RP-Set Table in old RFC

BsrRPSetEntry ::= SEQUENCE {
    bsrRPSetPriority          Unsigned32,
    bsrRPSetHoldtime          Unsigned32,
    bsrRPSetExpiryTime        TimeTicks
}
    INDEX      { pimGroupMappingOrigin,
                 pimGroupMappingAddressType,
                 pimGroupMappingGrpAddress,
                 pimGroupMappingGrpPrefixLength,
                 pimGroupMappingRPAddress }

--
-- The BSR Elected BSR Table
-- ( this was called The PIM Component Table  in old RFC)

BsrElectedBSREntry ::= SEQUENCE {
    bsrElectedBSRAddressType      InetAddressType,
    bsrElectedBSRAddress          InetAddress,
    bsrElectedBSRPriority         Unsigned32,
    bsrElectedBSRHashMaskLength   Unsigned32,
    bsrElectedBSRExpiryTime       TimeTicks,
    bsrElectedBSRCRPAdvTimer      TimeTicks
}
INDEX      { bsrElectedBSRAddressType }
--
-- The PIM SSM Range Table(should be in mapping layer)
--
PimSsmRangeEntry ::= SEQUENCE {
    pimSsmRangeAddressType   InetAddressType,
    pimSsmRangeAddress       InetAddress,
    pimSsmRangePrefixLength  InetAddressPrefixLength,
    pimSsmRangeRowStatus     RowStatus
}
    INDEX      { pimSsmRangeAddressType,
                 pimSsmRangeAddress,
                 pimSsmRangePrefixLength }

--
-- The PIM Static RP Table(should be in mapping layer)
--
PimStaticRPEntry ::= SEQUENCE {
    pimStaticRPAddressType      InetAddressType,
    pimStaticRPGrpAddress       InetAddress,
    pimStaticRPGrpPrefixLength  InetAddressPrefixLength,
    pimStaticRPRPAddress        InetAddress,
    pimStaticRPPimMode          PimMode,
    pimStaticRPOverrideDynamic  TruthValue,
    pimStaticRPRowStatus        RowStatus
}
INDEX      { pimStaticRPAddressType,
                 pimStaticRPGrpAddress,
                 pimStaticRPGrpPrefixLength }

--
-- The BSR Candidate-RP Table(should be in mapping layer)
-- ( this was called PIM Candidate-RP Table in old RFC)
--
BsrCandidateRPEntry ::= SEQUENCE {
    bsrCandidateRPAddressType       InetAddressType,
    bsrCandidateRPAddress           InetAddress,
    bsrCandidateRPGroupAddress      InetAddress,
    bsrCandidateRPGroupPrefixLength InetAddressPrefixLength,
    bsrCandidateRPBidir             TruthValue,
    bsrCandidateRPStatus            RowStatus
}
INDEX      { bsrCandidateRPAddressType,
                 bsrCandidateRPAddress,
                 bsrCandidateRPGroupAddress,
                 bsrCandidateRPGroupPrefixLength }

--
-- The BSR Candidate-BSR Table(should be in mapping layer)
--
BsrCandidateBSREntry ::= SEQUENCE {
    bsrCandidateBSRAddressType      InetAddressType,
    bsrCandidateBSRAddress          InetAddress,
    bsrCandidateBSRPriority         Unsigned32,
    bsrCandidateBSRHashMaskLength   Unsigned32,
    bsrCandidateBSRElectedBSR       TruthValue,
    bsrCandidateBSRBootstrapTimer   TimeTicks,
    bsrCandidateBSRStatus           RowStatus
}
INDEX      { bsrCandidateBSRAddressType }
*
*
*/

typedef enum {
    I_pimsmNeighborIfIndex,
    I_pimsmNeighborAddressType,
    I_pimsmNeighborAddress,
    I_pimsmNeighborUpTime,
    I_pimsmNeighborExpiryTime,
    I_pimsmNeighborLanPruneDelayPresent,
    I_pimsmNeighborPropagationDelay,
    I_pimsmNeighborOverrideInterval,
    I_pimsmNeighborTBit,
    I_pimsmNeighborGenerationIDPresent,
    I_pimsmNeighborGenerationIDValue,
    I_pimsmNeighborBidirCapable,
    I_pimsmNeighborDRPriorityPresent,
    I_pimsmNeighborDRPriority
} pimsmNeighborObjectId_t;


typedef enum {
    I_pimsmInterfaceIfIndex,
    I_pimsmInterfaceIPVersion,
    I_pimsmInterfaceAddressType,
    I_pimsmInterfaceAddress,
    I_pimsmInterfaceNetMaskLength,
    I_pimsmInterfaceDR,
    I_pimsmInterfaceHelloInterval,
    I_pimsmInterfaceTrigHelloInterval,
    I_pimsmInterfaceJoinPruneInterval,
    I_pimsmInterfaceDFElectionRobustness,
    I_pimsmInterfaceHelloHoldtime,
    I_pimsmInterfaceJoinPruneHoldtime,
    I_pimsmInterfaceUseLanPruneDelay,
    I_pimsmInterfacePropagationDelay,
    I_pimsmInterfaceOverrideInterval,
    I_pimsmInterfaceUseGenerationID,
    I_pimsmInterfaceGenerationIDValue,
    I_pimsmInterfaceUseDRPriority,
    I_pimsmInterfaceDRPriority,
    I_pimsmInterfaceLanDelayEnabled,
    I_pimsmInterfaceEffectPropagDelay,
    I_pimsmInterfaceEffectOverrideIvl,
    I_pimsmInterfaceSuppressionEnabled,
    I_pimsmInterfaceBidirCapable,
    I_pimsmInterfaceDRPriorityEnabled,
    I_pimsmInterfaceBSRBorder
}pimsmInterfaceObjectId_t;

                 
typedef enum {
    I_pimsmStarGAddressType,
    I_pimsmStarGGrpAddress,
    I_pimsmStarGUpTime,
    I_pimsmStarGpimMode,
    I_pimsmStarGRPAddress,
    I_pimsmStarGRPOrigin,
    I_pimsmStarGRPIsLocal,
    I_pimsmStarGUpstreamJoinState,
    I_pimsmStarGUpstreamJoinTimer,
    I_pimsmStarGUpstreamNeighbor,
    I_pimsmStarGRPFIfIndex,
    I_pimsmStarGRPFNextHop,
    I_pimsmStarGRPFRouteProtocol,
    I_pimsmStarGRPFRouteAddress,
    I_pimsmStarGRPFRoutePrefixLength,
    I_pimsmStarGRPFRouteMetricPref,
    I_pimsmStarGRPFRouteMetric
} pimsmStarGObjectId_t;

typedef enum {
    I_pimsmStarGIIfIndex,
    I_pimsmStarGIUpTime,
    I_pimsmStarGILocalMembership,
    I_pimsmStarGIJoinPruneState,
    I_pimsmStarGIPrunePendingTimer,
    I_pimsmStarGIJoinExpiryTimer,
    I_pimsmStarGIAssertState,
    I_pimsmStarGIAssertTimer,
    I_pimsmStarGIAssertWinnerAddress,
    I_pimsmStarGIAssertWinnerMetricPref,
    I_pimsmStarGIAssertWinnerMetric
} pimsmStarGIObjectId_t;

typedef enum {
    I_pimsmSGAddressType,
    I_pimsmSGGrpAddress,
    I_pimsmSGSrcAddress,
    I_pimsmSGUpTime,
    I_pimsmSGpimMode,
    I_pimsmSGUpstreamJoinState,
    I_pimsmSGUpstreamJoinTimer,
    I_pimsmSGUpstreamNeighbor,
    I_pimsmSGRPFIfIndex,
    I_pimsmSGRPFNextHop,
    I_pimsmSGRPFRouteProtocol,
    I_pimsmSGRPFRouteAddress,
    I_pimsmSGRPFRoutePrefixLength,
    I_pimsmSGRPFRouteMetricPref,
    I_pimsmSGRPFRouteMetric,
    I_pimsmSGSPTBit,
    I_pimsmSGKeepaliveTimer,
    I_pimsmSGDRRegisterState,
    I_pimsmSGDRRegisterStopTimer,
    I_pimsmSGRPRegisterPMBRAddress
} pimsmSGObjectId_t;

typedef enum {
    I_pimsmSGIIfIndex,
    I_pimsmSGIUpTime,
    I_pimsmSGILocalMembership,
    I_pimsmSGIJoinPruneState,
    I_pimsmSGIPrunePendingTimer,
    I_pimsmSGIJoinExpiryTimer,
    I_pimsmSGIAssertState,
    I_pimsmSGIAssertTimer,
    I_pimsmSGIAssertWinnerAddress,
    I_pimsmSGIAssertWinnerMetricPref,
    I_pimsmSGIAssertWinnerMetric
} pimsmSGIObjectId_t;

typedef enum {
    I_pimsmSGRptSrcAddress,
    I_pimsmSGRptUpTime,
    I_pimsmSGRptUpstreamPruneState,
    I_pimsmSGRptUpstreamOverrideTimer
} pimsmSGRptObjectId_t;


typedef enum {
    I_pimsmSGRptIIfIndex,
    I_pimsmSGRptIUpTime,
    I_pimsmSGRptILocalMembership,
    I_pimsmSGRptIJoinPruneState,
    I_pimsmSGRptIPrunePendingTimer,
    I_pimsmSGRptIPruneExpiryTimer
} pimsmSGRptIObjectId_t;

typedef enum {
    I_pimsmGroupMappingOrigin,
    I_pimsmGroupMappingAddressType,
    I_pimsmGroupMappingGrpAddress,
    I_pimsmGroupMappingGrpPrefixLength,
    I_pimsmGroupMappingRPAddress,
    I_pimsmGroupMappingpimMode,
    I_pimsmGroupMappingPrecedence,
    I_pimsmGroupMappingActive
} pimsmGroupMappingObjectId_t;

typedef enum {
    I_bsrRPSetPriority,
    I_bsrRPSetHoldtime,
    I_bsrRPSetExpiryTime
} pimsmBsrRPSetObjectId_t;

typedef enum {
    I_bsrElectedBSRAddressType,
    I_bsrElectedBSRAddress,
    I_bsrElectedBSRPriority,
    I_bsrElectedBSRHashMaskLength,
    I_bsrElectedBSRExpiryTime,
    I_bsrElectedBSRCRPAdvTimer
}  pimsmBsrElectedBSRObjectId_t;

/* (S,G) Table */
L7_RC_t pimsmSGEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr);
L7_RC_t pimsmSGEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr);
L7_RC_t pimsmSGObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         pimsmSGObjectId_t objId,
                         L7_uint32  *output);

/* (S,G,I) Table */
L7_RC_t pimsmSGIEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 rtrIfNum);
L7_RC_t pimsmSGIEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 *rtrIfNum);
L7_RC_t pimsmSGIObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 rtrIfNum, 
                         pimsmSGIObjectId_t objId,
                         L7_uint32  *output);

/* (S,G,rpt) Table */
L7_RC_t pimsmSGRptEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr);
L7_RC_t pimsmSGRptEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr);
L7_RC_t pimsmSGRptObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         pimsmSGRptObjectId_t objId,
                         L7_uint32  *output);

/* (S,G,rpt,I) Table */
L7_RC_t pimsmSGRptIEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 rtrIfNum);
L7_RC_t pimsmSGRptIEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 *rtrIfNum);
L7_RC_t pimsmSGRptIObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 rtrIfNum, 
                         pimsmSGRptIObjectId_t objId,
                         L7_uint32  *output);

/* (*,G) Table */
L7_RC_t pimsmStarGEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr);
L7_RC_t pimsmStarGEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr);
L7_RC_t pimsmStarGObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         pimsmStarGObjectId_t objId,
                         L7_uint32  *output);

/* (*,G,I) Table */
L7_RC_t pimsmStarGIEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_uint32 rtrIfNum);
L7_RC_t pimsmStarGIEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_uint32 *rtrIfNum);
L7_RC_t pimsmStarGIObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_uint32 rtrIfNum, 
                         pimsmStarGIObjectId_t objId,
                         L7_uint32  *output);

/* Interface Table */
L7_RC_t pimsmInterfaceEntryGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum);
L7_RC_t pimsmInterfaceEntryNextGet(L7_VOIDPTR handle, L7_uint32 *rtrIfNum);
L7_RC_t pimsmInterfaceObjectGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                        pimsmInterfaceObjectId_t objId,
                        L7_uint32  *output);
L7_RC_t pimsmInterfaceNeighborCountGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                                          L7_uint32 *pNbrCount);
/* Neighbor Table */
L7_RC_t pimsmNeighborEntryGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                        L7_inet_addr_t *pNbrAddr);
L7_RC_t pimsmNeighborEntryNextGet(L7_VOIDPTR handle, L7_uint32 *outRtrIfNum,
                        L7_inet_addr_t *pNbrAddr);
L7_RC_t pimsmNeighborObjectGet(L7_VOIDPTR handle, L7_uint32 rtrIfNum,
                        L7_inet_addr_t *pNbrAddr, 
                        pimsmNeighborObjectId_t objId,
                        L7_uint32  *output);

/* GroupMapping Table */
L7_RC_t pimsmRpGrpMappingEntryGet(L7_VOIDPTR handle, 
            L7_uchar8 origin,
            L7_inet_addr_t *pGrpAddrIn,
            L7_uchar8 grpPrefixLength,
            L7_inet_addr_t *pRpAddrIn);
L7_RC_t pimsmRpGrpMappingEntryNextGet(L7_VOIDPTR handle, 
            L7_uchar8 *origin,
            L7_inet_addr_t *pGrpAddrIn,
            L7_uchar8 *grpPrefixLength,
            L7_inet_addr_t *pRpAddrIn);
L7_RC_t pimsmRpGrpMappingEntryObjectGet(L7_VOIDPTR handle, 
            L7_uchar8 origin,
            L7_inet_addr_t *pGrpAddrIn,
            L7_uchar8 grpPrefixLength,
            L7_inet_addr_t *pRpAddrIn,
            pimsmGroupMappingObjectId_t objId,
            L7_uint32  *output);

/* bsr RP Set Table */
L7_RC_t pimsmBsrRPSetEntryGet(L7_VOIDPTR handle, 
            L7_uchar8 origin,
            L7_inet_addr_t *pGrpAddrIn,
            L7_uchar8 grpPrefixLength,
            L7_inet_addr_t *pRpAddrIn);
L7_RC_t pimsmBsrRPSetEntryNextGet(L7_VOIDPTR handle, 
            L7_uchar8 *origin,
            L7_inet_addr_t *pGrpAddrIn,
            L7_uchar8 *grpPrefixLength,
            L7_inet_addr_t *pRpAddrIn);
L7_RC_t pimsmBsrRPSetEntryObjectGet(L7_VOIDPTR handle, 
            L7_uchar8 origin,
            L7_inet_addr_t *pGrpAddrIn,
            L7_uchar8 grpPrefixLength,
            L7_inet_addr_t *pRpAddrIn,
            pimsmBsrRPSetObjectId_t objId,
            L7_uint32  *output);

/* bsr ElectedBSR Table */
L7_RC_t pimsmBsrElectedBSREntryGet(L7_VOIDPTR handle);
L7_RC_t pimsmBsrElectedBSREntryNextGet(L7_VOIDPTR handle);
L7_RC_t pimsmBsrElectedBSRObjectGet(L7_VOIDPTR handle,
                           pimsmBsrElectedBSRObjectId_t objId,
                           L7_uint32  *output);

#ifdef PIMSM_NOT_SUPPORTED

/*
 *
 * --------------------------
Following tables are taken from RFC2934 only
to support the existing common CLI & USMDB layer
 "show ip mcast mroute summary/detail"

--
-- The PIM Next Hop Table
--
PimMRouteNextHopEntry ::= SEQUENCE {
    pimMRouteNextHopAddressType        InetAddressType,
    pimMRouteNextHopGroup              InetAddress,
    pimMRouteNextHopSource             InetAddress,
    pimMRouteNextHopSourcePrefixLength InetAddressPrefixLength,
    pimMRouteNextHopIfIndex            InterfaceIndex,
    pimMRouteNextHopUpTime             TimeTicks,
    pimMRouteNextHopForwarding         TruthValue,
    pimMRouteNextHopJoinPruneTimer     TimeTicks,
    pimMRouteNextHopAssertWinner       InetAddress,
    pimMRouteNextHopAssertTimer        TimeTicks,
    pimMRouteNextHopAssertRPTBit       TruthValue,
    pimMRouteNextHopAssertMetricPref   Unsigned32,
    pimMRouteNextHopAssertMetric       Unsigned32
}
INDEX      { pimMRouteNextHopAddressType,
                 pimMRouteNextHopGroup,
                 pimMRouteNextHopSource,
                 pimMRouteNextHopSourcePrefixLength,
                 pimMRouteNextHopIfIndex }                 


-- The PIM IP Multicast Route Table
PimMRouteEntry ::= SEQUENCE {
    pimMRouteAddressType           InetAddressType,
    pimMRouteGroup                 InetAddress,
    pimMRouteSource                InetAddress,
    pimMRouteSourcePrefixLength    InetAddressPrefixLength,
    pimMRouteUpTime                TimeTicks,
    pimMRouteExpiryTime            TimeTicks,
    pimMRouteType                  INTEGER,
    pimMRouteRPAddress             InetAddress,
    pimMRouteRPFIfIndex            InterfaceIndexOrZero,
    pimMRouteRPFNeighbor           InetAddress,
    pimMRouteUpstreamAssertTimer   TimeTicks,
    pimMRouteAssertRPTBit          TruthValue,
    pimMRouteAssertMetricPref      Unsigned32,
    pimMRouteAssertMetric          Unsigned32,
    pimMRouteFlags                 BITS
}
INDEX      { pimMRouteAddressType,
                 pimMRouteGroup,
                 pimMRouteSource,
                 pimMRouteSourcePrefixLength }
                 
*/

typedef enum {
   I_pimsmMRouteGroup,
   I_pimsmMRouteSource,
   I_pimsmMRouteSourceMask,
   I_pimsmMRouteUpTime,
   I_pimsmMRouteExpiryTime,
   I_pimsmMRouteType,
   I_pimsmMRouteRPAddress,
   I_pimsmMRouteRPFIfIndex,
   I_pimsmMRouteRPFNeighbor,
   I_pimsmMRouteUpstreamAssertTimer,
   I_pimsmMRouteAssertMetric,   
   I_pimsmMRouteAssertMetricPref,
   I_pimsmMRouteAssertRPTBit,
   I_pimsmMRouteFlags,
} pimsmMRouteObjectId_t;

typedef enum {
   PIMSM_MROUTE_RPT = 0,
   PIMSM_MROUTE_SPT,
} pimsmMRouteFlags_t;


typedef struct pimsmMRouteEntry_s {
   L7_inet_addr_t      pimsmMRouteGroup;
   L7_inet_addr_t      pimsmMRouteSource;
   L7_inet_addr_t      pimsmMRouteSourceMask;
   L7_uint32         pimsmMRouteUpTime;
   L7_uint32         pimsmMRouteExpiryTime;
   pimMode_t         pimsmMRouteType;
   L7_inet_addr_t      pimsmMRouteRPAddress;
   L7_uint32         pimsmMRouteRPFIfIndex;
   L7_inet_addr_t      pimsmMRouteRPFNeighbor;
   L7_uint32         pimsmMRouteUpstreamAssertTimer;
   L7_uint32         pimsmMRouteAssertMetric;   
   L7_uint32         pimsmMRouteAssertMetricPref;
   L7_BOOL           pimsmMRouteAssertRPTBit;
   pimsmMRouteFlags_t  pimsmMRouteFlags;
} pimsmMRouteEntry_t;

L7_RC_t pimsmMRouteEntryGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_inet_addr_t *pSrcAddrMask);
L7_RC_t pimsmMRouteEntryObjectGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_inet_addr_t *pSrcAddrMask,
                         pimsmMRouteObjectId_t objId,
                         L7_uint32  *output);
L7_RC_t pimsmMRouteEntryNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_inet_addr_t *pSrcAddrMask);
L7_RC_t pimsmMRouteEntryOIFNextGet(L7_VOIDPTR handle,
                         L7_inet_addr_t *pGrpAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 *outIntIfNum);

#endif
L7_RC_t pimsmRpHashGet(L7_VOIDPTR handle,
                       L7_inet_addr_t *pGrpAddr,
                       L7_inet_addr_t *pRpAddr,
                       L7_uchar8 *origin);

extern L7_RC_t pimsmWholePacketEventQueue(L7_uchar8 familyType, 
                                          mfcEntry_t *pMfcEntry);

extern L7_RC_t pimsmGlobalAdminModeProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                                           L7_BOOL isPimsmEnabled);

extern L7_RC_t pimsmIntfStatusProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             pimsmIntfStatus_t *pimsmIntfStatus);

extern L7_RC_t pimsmStaticRPProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             pimsmStaticRPdata_t *pStaticRpData);

extern L7_RC_t pimsmCBSRProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             pimsmCandBSRdata_t *pCandBsrData);

extern L7_RC_t pimsmCRPProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             pimsmCandRPdata_t *pCandRpData);

extern L7_RC_t pimsmRegisterThresholdRateSet(MCAST_CB_HNDL_t pimsmCbHandle, 
                             L7_uint32 regThresholdRate);
extern L7_RC_t pimsmDataThresholdRateSet(MCAST_CB_HNDL_t pimsmCbHandle, 
                             L7_uint32 dataThresholdRate);
extern L7_RC_t pimsmDRPriorityProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             L7_uint32 rtrIfNum);
extern L7_RC_t pimsmHelloIntervalProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             L7_uint32 rtrIfNum, L7_uint32 helloIntvl);

extern L7_RC_t pimsmJoinPruneIntervalProcess(MCAST_CB_HNDL_t pimsmCbHandle, 
                             L7_uint32 rtrIfNum, L7_uint32 joinPruneIntvl);

extern L7_RC_t pimsmStarGEntryOIFGet(L7_VOIDPTR handle,
                                         L7_inet_addr_t *pGrpAddr,
                                         interface_bitset_t *pOif);
extern L7_RC_t pimsmSGEntryOIFGet(L7_VOIDPTR handle,
                                         L7_inet_addr_t *pGrpAddr,
                                         L7_inet_addr_t *pSrcAddr,
                                         interface_bitset_t *pOif);
extern L7_RC_t pimsmIpMrouteEntryCountGet(L7_VOIDPTR handle,
                                          L7_uint32 *entryCount);

#endif /* _PIMSM_CTRL_H_ */


