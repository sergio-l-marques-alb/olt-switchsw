#ifndef L7_UNDEFS_H
#define L7_UNDEFS_H

#undef I_ifOutQLen

#undef I_ifTestId
#undef I_ifTestStatus
#undef I_ifTestType
#undef I_ifTestResult
#undef I_ifTestCode
#undef I_ifTestOwner

#undef I_ifStackLastChange
#undef I_ifStackStatus

#undef I_ifRcvAddressStatus
#undef I_ifRcvAddressType

#undef I_dot1dStaticAllowedToGoTo
#undef I_dot1dStaticStatus

/* Made obsolete in RFC 1907 */
#undef I_snmpInTooBigs
#undef I_snmpInNoSuchNames
#undef I_snmpInBadValues
#undef I_snmpInReadOnlys
#undef I_snmpInGenErrs
#undef I_snmpInTotalReqVars
#undef I_snmpInTotalSetVars
#undef I_snmpInGetRequests
#undef I_snmpInGetNexts
#undef I_snmpInSetRequests
#undef I_snmpInGetResponses
#undef I_snmpInTraps
#undef I_snmpOutTooBigs
#undef I_snmpOutNoSuchNames
#undef I_snmpOutBadValues
#undef I_snmpOutGenErrs
#undef I_snmpOutGetRequests
#undef I_snmpOutGetNexts
#undef I_snmpOutSetRequests
#undef I_snmpOutGetResponses
#undef I_snmpOutTraps
#undef I_snmpOutPkts



/* lvl7_@p0361 start */
#undef I_dot1dBasePortDelayExceededDiscards
/* lvl7_@p0361 end */

/* lvl7_@p0362 start */
#undef I_dot1dBasePortMtuExceededDiscards
/* lvl7_@p0362 end */

/* lvl7_@p0616 start */
#undef I_dot1dRegenUserPriority
#undef I_dot1dUserPriority
/* lvl7_@p0616 end */

/* lvl7_@p0618 start */
#undef I_dot1dPortOutboundAccessPriority
/* lvl7_@p0618 end */

/* lvl7_@p0635 end */
#undef I_dot1qTpGroupAddress
#undef I_dot1qTpGroupEgressPorts
#undef I_dot1qTpGroupLearnt
/* lvl7_@p0635 end */

/* lvl7_@p0636 end */
#undef I_dot1qForwardAllPorts
#undef I_dot1qForwardAllForbiddenPorts
#undef I_dot1qForwardAllStaticPorts
/* lvl7_@p0636 end */

/* lvl7_@p0637 start */
#undef I_dot1qForwardUnregisteredPorts
#undef I_dot1qForwardUnregisteredStaticPorts
#undef I_dot1qForwardUnregisteredForbiddenPorts
/* lvl7_@p0637 end */

/* lvl7_@p0638 start */
#undef I_dot1qStaticUnicastAddress
#undef I_dot1qStaticUnicastReceivePort
#undef I_dot1qStaticUnicastAllowedToGoTo
#undef I_dot1qStaticUnicastStatus
/* lvl7_@p0638 end */

/* lvl7_@p0639 start */
#undef I_dot1qStaticMulticastAddress
#undef I_dot1qStaticMulticastReceivePort
#undef I_dot1qStaticMulticastStaticEgressPorts
#undef I_dot1qStaticMulticastForbiddenEgressPorts
#undef I_dot1qStaticMulticastStatus
/* lvl7_@p0639 end */

/* lvl7_@p0809 start */
#undef I_dot1qConstraintType
#undef I_dot1qConstraintStatus
/* lvl7_@p0809 end */

#undef I_dot1qConstraintSetDefault
#undef I_dot1qConstraintTypeDefault 

/* lvl7_@p2069 start */
#undef I_dot1qTpVlanPortInFrames
#undef I_dot1qTpVlanPortOutFrames
#undef I_dot1qTpVlanPortInDiscards
#undef I_dot1qTpVlanPortInOverflowFrames
#undef I_dot1qTpVlanPortOutOverflowFrames
#undef I_dot1qTpVlanPortInOverflowDiscards
#undef I_dot1qPortVlanStatisticsEntryIndex_dot1dBasePort
#undef I_dot1qPortVlanStatisticsEntryIndex_dot1qVlanIndex

#undef I_dot1qTpVlanPortHCInFrames
#undef I_dot1qTpVlanPortHCOutFrames
#undef I_dot1qTpVlanPortHCInDiscards
#undef I_dot1qPortVlanHCStatisticsEntryIndex_dot1dBasePort
#undef I_dot1qPortVlanHCStatisticsEntryIndex_dot1qVlanIndex
/* lvl7_@p2069 end */

/* lvl7_@p1633 start */
#undef I_agentPortPhysicalMode
#undef I_agentPortPhysicalStatus
/* lvl7_@p1633 end */

#undef I_agentLagSummaryFlushTimer

/* unsupported dot3ad MIBs start */
#undef I_dot3adAggPortStatsMarkerResponsePDUsRx
#undef I_dot3adAggPortStatsMarkerPDUsTx
#undef I_dot3adAggPortDebugRxState
#undef I_dot3adAggPortDebugLastRxTime
#undef I_dot3adAggPortDebugMuxState
#undef I_dot3adAggPortDebugMuxReason
#undef I_dot3adAggPortDebugActorChurnState
#undef I_dot3adAggPortDebugPartnerChurnState
#undef I_dot3adAggPortDebugActorChurnCount
#undef I_dot3adAggPortDebugPartnerChurnCount
#undef I_dot3adAggPortDebugActorSyncTransitionCount
#undef I_dot3adAggPortDebugPartnerSyncTransitionCount
#undef I_dot3adAggPortDebugActorChangeCount
#undef I_dot3adAggPortDebugPartnerChangeCount
#undef I_dot3adAggPortDebugEntryIndex_dot3adAggPortIndex
/* unsupported dot3ad MIBs end */

/* unsupported rfc2925 MIBs start */
#undef I_pingCtlDataFill
#undef I_pingCtlFrequency
#undef I_pingCtlMaxRows
#undef I_pingCtlTrapGeneration
#undef I_pingCtlTrapProbeFailureFilter
#undef I_pingCtlTrapTestFailureFilter
#undef I_pingCtlDescr
#undef I_pingCtlSourceAddressType
#undef I_pingCtlSourceAddress
#undef I_pingCtlIfIndex
#undef I_pingCtlByPassRouteTable
#undef I_pingCtlDSField
#undef I_pingResultsRttSumOfSquares 
#undef I_pingResultsLastGoodProbe
#undef I_pingProbeHistoryTable
#undef I_pingProbeHistoryEntry
#undef I_pingProbeHistoryIndex        
#undef I_pingProbeHistoryResponse     
#undef I_pingProbeHistoryStatus       
#undef I_pingProbeHistoryLastRC       
#undef I_pingProbeHistoryTime        
#undef I_traceRouteProbeHistoryTable
#undef I_traceRouteCtlByPassRouteTable
#undef I_traceRouteCtlDSField
#undef I_traceRouteCtlSourceAddressType
#undef I_traceRouteCtlSourceAddress
#undef I_traceRouteCtlIfIndex
#undef I_traceRouteCtlMiscOptions
#undef I_traceRouteCtlFrequency
#undef I_traceRouteCtlDescr
#undef I_traceRouteCtlMaxRows
#undef I_traceRouteCtlTrapGeneration
#undef I_traceRouteCtlCreateHopsEntries
#undef I_traceRouteResultsLastGoodPath
#undef I_traceRouteHopsRttSumOfSquares
#undef I_traceRouteHopsLastGoodProbe
#undef I_traceRouteProbeHistoryIndex 
#undef I_traceRouteProbeHistoryHopIndex
#undef traceRouteProbeHistoryProbeIndex
#undef traceRouteProbeHistoryHAddrType
#undef traceRouteProbeHistoryHAddr
#undef traceRouteProbeHistoryResponse 
#undef traceRouteProbeHistoryStatus
#undef traceRouteProbeHistoryLastRC 
#undef traceRouteProbeHistoryTime 
/* unsupported rfc2925 MIBs end */ 

#endif /* L7_UNDEFS_H */
