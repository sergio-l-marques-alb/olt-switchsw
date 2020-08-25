/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sflow_api.h
* 
* @purpose  sFlow Component API header file
* 
* @component sflow
* 
* @create  11/21/2007
*   
* @author  Rajesh G
* @end
*
**********************************************************************/
#ifndef SFLOW_API_H
#define SFLOW_API_H

#include "comm_mask.h"
#include "comm_structs.h"
#include "l7_common.h"
#include "l3_addrdefs.h"

/*********************************************************************
* @purpose  This function is used to get the Agent Version.
*
* @param    UnitIndex   unit index
* @param    version     Agennt version string
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowAgentVersionGet(L7_char8 *version);
 

/*********************************************************************
* @purpose  This function is used to get the Agent Address Type.
*
* @param    UnitIndex   unit index
* @param    type        Pointer to the Agent Address Type
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowAgentAddressTypeGet(L7_uint32 *type);

/*********************************************************************
* @purpose  This function is used to get the Agent Address .
*
* @param    UnitIndex   unit index
* @param    strIpAddr   Agent Address in Printable format
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t sFlowAgentAddressGet(L7_uchar8 *strIpAddr);
/*********************************************************************
* @purpose  Checks if the specified receiver index is valid or not
*
* @param    index  @b{(input)} receiver index
*
* @returns  L7_SUCCESS - Valid receiver index
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrGet(L7_uint32 index);
/*********************************************************************
* @purpose  Gets next valid receiver index if it exists
*
* @param    index      @b{(input)}  receiver index
* @param   *nextIndex  @b{(output)} receiver index
*
* @returns  L7_SUCCESS - No next valid receiver present
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrNextGet(L7_uint32 index, L7_uint32 *nextIndex);
/*********************************************************************
* @purpose  This function is used to get the timeout value
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    time       @b{(input)}  Pointer to Receiver Timeout value
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrTimeoutGet(L7_uint32 index, L7_uint32 *time);
/*********************************************************************
* @purpose  This function is used to set the timeout value
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    time       @b{(input)}  Timeout value
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrTimeoutSet(L7_uint32 index, L7_uint32 time);
/*********************************************************************
* @purpose  This function is used to get the IP Address Type
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    ipaddr     @b{(input)}  Pointer to Receiver IP Address
*                                   type
*
* @returns  L7_SUCCESS
*
* @notes
*   1 Represents IPv4 
*   2 Represents Ipv6
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressTypeGet(L7_uint32 index, L7_uint32 *version);
/*********************************************************************
* @purpose  This function is used to set address type
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    type       @b{(input)}  L7_AF_INET or L7_AF_INET6
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressTypeSet(L7_uint32 index, L7_uint32 type);
/*********************************************************************
* @purpose  This function is used to get the IP Address 
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    ipaddr     @b{(input)}  Pointer to Receiver IP Address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressGet(L7_uint32 index, L7_inet_addr_t *ipAddr);
/*********************************************************************
* @purpose  This function is used to set the IP Address
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    ipaddr     @b{(input)}  IP Address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrAddressSet(L7_uint32 index, L7_inet_addr_t *inetAddr);
/*********************************************************************
* @purpose  This function is used to get the port
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    port       @b{(input)}  Pointer to port
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrPortGet(L7_uint32 index, L7_uint32 *port);
/*********************************************************************
* @purpose  This function is used to set the port
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    port       @b{(input)}  Port
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrPortSet(L7_uint32 index, L7_uint32 port);
/*********************************************************************
* @purpose  This function is used to get the sFlow Datagram Version
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    version    @b{(input)}  Pointer to Datagram Version
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrDatagramVersionGet(L7_uint32 index, L7_uint32 *version);
/*********************************************************************
* @purpose  This function is used to get the sFlow receiver snmp mode
*           associated with the Receiver Index
*
* @param    index    @b{(input)}  Receiver Index
* @param    mode     @b{(input)}  L7_ENABLE - Collector maintains
*                                 the receiver record
*                                 L7_DISABLE - Receiver record is
*                                 persistent
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrSnmpModeGet(L7_uint32 index, L7_uint32 *mode);
/*********************************************************************
* @purpose  This function is used to set the sFlow receiver snmp mode
*           associated with the Receiver Index
*
* @param    index    @b{(input)}  Receiver Index
* @param    mode     @b{(input)}  L7_ENABLE - Collector maintains
*                                 the receiver record
*                                 L7_DISABLE - Receiver record is
*                                 persistent
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowRcvrSnmpModeSet(L7_uint32 index, L7_uint32 mode);
/*********************************************************************
* @purpose  This function is used to set the sFlow Datagram Version
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    version    @b{(input)}  Datagram Version
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowRcvrDatagramVersionSet(L7_uint32 index, L7_uint32 version);
/*********************************************************************
* @purpose  This function is used to get the Datagram Size
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    dSize      @b{(input)}  Pointer to Datagram size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrMaximumDatagramSizeGet(L7_uint32 index, L7_uint32 *dSize);
/*********************************************************************
* @purpose  This function is used to set the Datagram Size
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    dSize      @b{(input)}  Datagram size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrMaximumDatagramSet(L7_uint32 index, L7_uint32 dSize);
/*********************************************************************
* @purpose  This function is used to get the Owner String
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    buffer     @b{(input)}  Pointer to Receiver Buffer
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrOwnerGet(L7_uint32 index, L7_uchar8 *buffer);
/*********************************************************************
* @purpose  This function is used to set the Owner String
*           associated with the Receiver Index
*
* @param    index      @b{(input)}  Receiver Index
* @param    buffer     @b{(input)}  Receiver Buffer
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowRcvrOwnerSet(L7_uint32 index, L7_uchar8 *buffer, 
                          L7_uint32 timeout);
/*********************************************************************
* @purpose  Returns sflowFsEntry status
*   
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    instance will be all ways 1 
*            
* @end
*********************************************************************/
L7_RC_t sFlowFsEntryGet(L7_uint32 UnitIndex, L7_char8 *index, 
                        L7_uint32 instance);

/*********************************************************************
* @purpose  Returns next sflowFsEntry index
*
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
* 
* @end
*********************************************************************/
L7_RC_t sFlowFsEntryGetNext(L7_uint32 UnitIndex, L7_char8 *index, 
                            L7_uint32 *instance );

/*********************************************************************
* @purpose  This function is used get the maximum header size
*           associated with the sFlow Sampler
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    maxHeaderSize   @b{(input)}  Pointer to Maximum header 
                                         size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowFsMaximumHeaderSizeGet(L7_uint32 intIfIndex, L7_uint32 instance,
                                     L7_uint32 *maxHeaderSize);
/*********************************************************************
* @purpose  This function is used set the maximum header size
*           associated with the sFlow Sampler
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    maxHeaderSize   @b{(input)}  Maximun Header size
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
***********************************************************************/
L7_RC_t sFlowFsMaximumHeaderSizeSet(L7_uint32 intIfIndex, L7_uint32 instance,
                                    L7_uint32 maxHeaderSize);
/*********************************************************************
* @purpose  This function is used get the Receiver Index
*           associated with the sFlow Sampler
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Pointer to receiver index
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowFsReciverGet(L7_uint32 intIfIndex, L7_uint32 instance,
                          L7_uint32 *receiver);
/*********************************************************************
* @purpose  This function is used to set the Receiver for 
*           sFlow Sampler
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Receiver Index
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowFsReceiverSet(L7_uint32 intIfIndex, L7_uint32 instance,
                           L7_uint32 receiver);
/*********************************************************************
* @purpose  This function is used get the Packet Sampling Rate
*           associated with the sFlow Sampler
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    samplingRate    @b{(input)}  Pointer to sampling rate
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowFsPacketSamplingRateGet(L7_uint32 intIfIndex, L7_uint32 instance,
                                     L7_uint32 *samplingRate);
/*********************************************************************
* @purpose  This function is used to set the sampling rate
*           associated with the sFlow Sampler
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    samplingRate    @b{(input)}  Sampling rate
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowFsPacketSamplingRateSet(L7_uint32 intIfIndex, L7_uint32 instance,
                                     L7_uint32 samplingRate);
/** END OF SF FUNCTIONS **/
/******* CP FUNCTIONS **********/
/*********************************************************************
* @purpose  Returns sflowCpEntry status
*
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowCpEntryGet(L7_uint32 UnitIndex, L7_uchar8 *index, 
                              L7_uint32 instance);

/*********************************************************************
* @purpose  Returns next sflowFsEntry index
*
* @param    intIfIndex  internal interface number
* @param    instance    sflow instance id
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
* 
* @end
*********************************************************************/
L7_RC_t sFlowCpEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *index,
                            L7_uint32 *instance );
/*********************************************************************
* @purpose  This function is used get the Receiver Idex
*           associated with the sFlow Counter Poller
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Pointer to Receiver Index
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
***********************************************************************/
L7_RC_t sFlowCpReciverGet(L7_uint32 intIfIndex, L7_uint32 instance,
                          L7_uint32 *receiver);
/*********************************************************************
* @purpose  This function is used to set the Receiver index
*          associated with the sFlow Counter Poller
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    receiver        @b{(input)}  Receiver index
*
* @returns  L7_SUCCESS
*
* @notes    Check if a poller is already present. If present return
*            error.
*            failure.
*
* @end
***********************************************************************/
L7_RC_t sFlowCpReceiverSet(L7_uint32 intIfIndex, L7_uint32 instance,
                           L7_uint32 receiver);
/*********************************************************************
* @purpose  This function is used get the Poller Interval
*           associated with the sFlow Counter Poller
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    interval        @b{(input)}  Pointer to poller interval
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowCpIntervalGet(L7_uint32 intIfIndex, L7_uint32 instance,
                           L7_uint32 *interval);
/*********************************************************************
* @purpose  This function is used set the counter poller interval
*           associated with the sFlow Counter Poller
*
* @param    intIfIndex      @b{(input)}  Internal Interface number
* @param    instance        @b{(input)}  instance id of sampler(1)
* @param    interval        @b{(input)}  Poller Interval
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t sFlowCpIntervalSet(L7_uint32 intIfIndex, L7_uint32 instance,
                           L7_uint32 interval);
/********* END OF CP FUNCTIONS ****/
#endif /* SFLOW_API_H */
