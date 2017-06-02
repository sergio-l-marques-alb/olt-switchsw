/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename  usmdb_isdp_api.h
*
* @purpose   Provide interface to API for usmdb isdp
*
* @component mgmt
*
* @comments
*
* @create    28/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/

#ifndef USMDB_ISDP_API_H
#define USMDB_ISDP_API_H

/*********************************************************************
*
* @purpose  Sets the isdp global mode
*
* @param    mode  @b((input)) Admin mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the isdp global mode
*
* @param    *mode @b((output)) Admin mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Sets the isdp global timer
*
* @param    timer  @b((input)) timer in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpTimerSet(L7_uint32 timer);

/*********************************************************************
*
* @purpose  Gets the isdp global timer
*
* @param    *timer @b((output)) timer in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpTimerGet(L7_uint32 *timer);

/*********************************************************************
*
* @purpose  Sets the isdp global holdtime
*
* @param    holdtime  @b((input)) holdtime in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpHoldTimeSet(L7_uint32 holdTime);

/*********************************************************************
*
* @purpose  Gets the isdp global holdtime
*
* @param    *holdtime @b((output)) holdtime in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpHoldTimeGet(L7_uint32 *holdTime);

/*********************************************************************
*
* @purpose  Sets the isdp global v2Mode
*
* @param    v2Mode  @b((input)) V2 advertisement mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpV2ModeSet(L7_uint32 v2Mode);

/*********************************************************************
*
* @purpose  Gets the isdp global v2Mode
*
* @param    *v2Mode @b((output)) V2 advertisement mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpV2ModeGet(L7_uint32 *v2Mode);

/*********************************************************************
*
* @purpose  Sets the isdp mode for the specified interface
*
* @param    intIfNum  @b((input)) internal interface number
* @param    mode      @b((input)) isdp intf mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the isdp mode for the specified interface
*
* @param    intIfNum  @b((input))  internal interface number
* @param    *mode     @b((output)) isdp intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Clears all isdp traffic counters
*
* @param    none
*
* @returns  L7_SUCCESS, if data cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpTrafficCountersClear(void);

/*********************************************************************
*
* @purpose  Clears all isdp neighbor entries
*
* @param    none
*
* @returns  L7_SUCCESS, if data cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpNeighborTableClear(void);

/*********************************************************************
* @purpose  Given a deviceId, get the next one,
*           if it exists
*
* @param    intIfNum     @b((input))   Interface number
* @param    *deviceId     @b((input))   deviceId, start
* @param    *nextDeviceId @b((output))  deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborGetNext(L7_uint32 intIfNum,
    L7_uchar8 *deviceId, L7_uchar8 *nextDeviceId);

/*********************************************************************
* @purpose  Get next neighbor with specific deviceId
*
*
* @param    intIfNum      @b((input))   Interface number, start
* @param    *deviceId      @b((input))   deviceId, start
* @param    *nextIntIfNum @b((output))  Interface number, next
* @param    *nextDeviceId  @b((output))  deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDeviceIdNeighborGetNext(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 *nextIntIfNum,
    L7_uchar8 *nextDeviceId);

/*********************************************************************
* @purpose  Given a deviceId, get the next one,
*           if it exists
*
* @param    intIfNum       @b((input))  Interface number
* @param    *deviceId       @b((input))  deviceId, start
* @param    *nextIntIfNum  @b((output)) Next interface number
* @param    *nextDeviceId   @b((output)) deviceId, next
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
* @notes   if the start is NULL_PTR, the first will be returned
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpNeighborGetNext(
    L7_uint32 intIfNum, L7_uchar8 *deviceId,
    L7_uint32 *nextIntIfNum, L7_uchar8 *nextDeviceId);

/*********************************************************************
*
* @purpose  Gets the platform of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  Device identifier
* @param    *platform  @b((output)) Platform string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborPlatformGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uchar8 *platform);

/*********************************************************************
*
* @purpose  Gets the version of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId @b((input))    Device identifier
* @param    *version   @b((output)) Version string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborVersionGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uchar8 *version);

/*********************************************************************
*
* @purpose  Gets the portId of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  Device identifier
* @param    *portId    @b((output)) Port identifier string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborPortIdGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uchar8 *portId);

/*********************************************************************
*
* @purpose  Get the next ip adress index of the neighbor
*
* @param    intIfNum     @b((input))  Interface number
* @param    *deviceId     @b((input))  Device identifier
* @param    index       @b((input))  Previous index
* @param    *nextIndex   @b((output)) Next index
*
*
* @returns  L7_SUCCESS if there is next address
* @returns  L7_FAILURE if there is no next address
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborAddressGetNext(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 index,
    L7_uint32 *nextIndex);

/*********************************************************************
*
* @purpose  Gets the ip address of neighbor
*
* @param    intIfNum      @b((input))  Interface number
* @param    *deviceId      @b((input))  Device identifier
* @param    index         @b((input))  ip address index
* @param    *ipAddress    @b((output)) ip address
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborIpAddressGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 index,
    L7_uint32 *ipAddress);

/*********************************************************************
*
* @purpose  Gets the capabilities of neighbor
*
* @param    intIfNum      @b((input))  Interface number
* @param    *deviceId      @b((input))  Device identifier
* @param    *capabilities @b((output)) Device capabilities
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborCapabilitiesGet(L7_uint32 intIfNum,
    L7_uchar8 *deviceId, L7_uint32 *capabilities);

/*********************************************************************
*
* @purpose  Gets the holdTime of neighbor
*
* @param    intIfNum   @b((input))  Interface number
* @param    *deviceId   @b((input))  Device identifier
* @param    *holdTime  @b((output)) Holdtime
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborHoldTimeGet(L7_uint32 intIfNum,
    L7_uchar8 *deviceId, L7_uchar8 *holdTime);

/*********************************************************************
*
* @purpose  Gets the protocol version of neighbor
*
* @param    intIfNum      @b((input))  Interface number
* @param    *deviceId      @b((input))  Device identifier
* @param    *protoVersion @b((output)) ISDP protocol version
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborProtocolVersionGet(L7_uint32 intIfNum,
    L7_uchar8 *deviceId, L7_uchar8 *protoVersion);

/*********************************************************************
*
* @purpose  Gets the time when entry last changed
*
* @param    intIfNum        @b((input))  Interface number
* @param    *deviceId        @b((input))  Device identifier
* @param    *lastChangeTime @b((output)) time added/modified
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpIntfNeighborLastChangeTimeGet(
    L7_uint32 intIfNum,
    L7_uchar8 *deviceId,
    L7_uint32 *lastChangeTime);

/*********************************************************************
* @purpose  Get the number of all isdp pdu received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficPduReceivedGet(void);

/*********************************************************************
* @purpose  Get the number of v1 isdp pdu received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficV1PduReceivedGet(void);

/*********************************************************************
* @purpose  Get the number of v2 isdp pdu received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficV2PduReceivedGet(void);

/*********************************************************************
* @purpose  Get the number of isdp pdu with bad header received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficBadHeaderPduReceivedGet(void);

/*********************************************************************
* @purpose  Get the number of isdp pdu with chksum error received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficChkSumErrorPduReceivedGet(void);

/*********************************************************************
* @purpose  Get the number of isdp pdu in invalid format received
*
*
* @returns the current value of received pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficInvalidFormatPduReceivedGet(void);

/*********************************************************************
* @purpose  Get the number of all isdp pdu transmitted
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficPduTransmitGet(void);

/*********************************************************************
* @purpose  Get the number of v1 isdp pdu transmitted
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficV1PduTransmitGet(void);

/*********************************************************************
* @purpose  Get the number of v2 isdp pdu transmitted
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficV2PduTransmitGet(void);

/*********************************************************************
* @purpose  Get the number of isdp pdu transmition failures
*
*
* @returns the current value of transmitted pdu
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficFailurePduTransmitGet(void);

/*********************************************************************
* @purpose  Get the number of isdp entry table was full
*
*
* @returns the number
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficTableFullGet(void);

/*********************************************************************
* @purpose  Get the number of ip address table was full
*
*
* @returns the number
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpTrafficIpAddressTableFullGet(void);

/*********************************************************************
* @purpose  Get the time when isdp entry table was modified last time
*
*
* @returns the number
*
* @comments
*
*
* @end
*********************************************************************/
L7_uint32 usmdbIsdpNeighborsTableLastChangeTimeGet(void);

/*********************************************************************
*
* @purpose  Set the tracing mode for tx packets
*
* @param    txTraceMode  @b((input)) Trace mode for tx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDebugTracePacketTxModeSet(L7_uint32 txTraceMode);

/*********************************************************************
*
* @purpose  Get the tracing mode for tx packets
*
* @param    *txTraceMode @b((output)) Trace mode for tx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDebugTracePacketTxModeGet(L7_uint32 *txTraceMode);

/*********************************************************************
*
* @purpose  Set the tracing mode for rx packets
*
* @param    rxTraceMode  @b((input)) Trace mode for rx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDebugTracePacketRxModeSet(L7_uint32 rxTraceMode);

/*********************************************************************
*
* @purpose  Get the tracing mode for tx packets
*
* @param    *rxTraceMode @b((output)) Trace mode for rx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDebugTracePacketRxModeGet(L7_uint32 *rxTraceMode);

/*********************************************************************
*
* @purpose  Set the tracing mode for events
*
* @param    eventsTraceMode  @b((input)) Trace mode for events
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDebugTraceEventsModeSet(L7_uint32 eventsTraceMode);

/*********************************************************************
*
* @purpose  Get the tracing mode for events
*
* @param    *eventsTraceMode @b((output)) Trace mode for events
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDebugTraceEventsModeGet(L7_uint32 *eventsTraceMode);

/*********************************************************************
*
* @purpose  Gets the isdp deviceId name format capability
*
* @param    *deviceIdFormatCpb @b((output)) DeviceId format Capability
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDeviceIdFormatCapabilityGet(L7_uchar8 *deviceIdFormatCpb);

/*********************************************************************
*
* @purpose  Gets the isdp deviceId name format
*
* @param    *deviceIdFormatCpb @b((output)) DeviceId format
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpDeviceIdFormatGet(L7_uint32 *deviceIdFormat);

/*********************************************************************
* @purpose    Translates input integer capability code to its char
*             equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
L7_RC_t usmdbIsdpCapabilityCharGet(L7_uint32 cap, L7_char8 *capCh, L7_uint32 size);

/*********************************************************************
*
* @purpose    Translates input integer capability code to its
*             string equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
L7_RC_t usmdbIsdpCapabilityStringGet(L7_uint32 cap, L7_char8 *capCh, L7_uint32 size);

/*********************************************************************
*
* @purpose    Translates input integer device id capability code
*             to its string equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
L7_RC_t usmdbIsdpDevIdFormatCapabilityStringGet(L7_char8 cap, L7_char8 *capCh, L7_uint32 size);

/*********************************************************************
*
* @purpose    Translates input integer device id code
*             to its string equivalent
*
* @param      L7_uint32 cap
* @param      L7_char8 *capCh
* @param      L7_uint32 size
*
* @returntype void
*
* @end
*
*********************************************************************/
L7_RC_t usmdbIsdpDevIdFormatStringGet(L7_uint32 cap, L7_char8 *capCh, L7_uint32 size);

/*********************************************************************
* @purpose  Given an index and interface, get the next neighbor,
*           if it exists
*
* @param    intIfNum        @b((input))   Interface number
* @param    index           @b((input))   index, start
* @param    *nextIntIfNum   @b((output))  Next interface number
* @param    *nextIndex      @b((output))  index, next
* @param    *deviceId       @b((output))  device identifier
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpNeighborIndexedGetNext(L7_uint32 intIfNum, L7_uint32 index,
                                   L7_uint32 *nextIntIfNum, L7_uint32 *nextIndex,
                                   L7_char8 *deviceId);

/*********************************************************************
* @purpose  Given an index and interface, get the neighbor device id,
*           if it exists
*
* @param    intIfNum        @b((input))   Interface number
* @param    index           @b((input))   index, start
* @param    *deviceId       @b((output))  device identifier
*
* @returns  L7_SUCCESS, if a next was found
* @returns  L7_FAILURE, if a next doesn't exist
*
*
* @end
*********************************************************************/
L7_RC_t usmdbIsdpNeighborIndexedGet(L7_uint32 intIfNum, L7_uint32 index,
                                    L7_char8 *deviceId);

/*********************************************************************
* @purpose  Determine if the interface is valid for isdp
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL usmdbIsdpIsValidIntf(L7_uint32 intIfNum);

/*****************************************************************
* @purpose  Determines the Device ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
****************************************************************/
L7_RC_t usmDbIsdpDeviceIdGet(L7_uchar8 *isdp_device_id);

#endif /*USMDB_ISDP_API_H*/
