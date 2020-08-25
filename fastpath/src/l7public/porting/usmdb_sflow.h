/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_sflow.c
*
* @purpose externs for USMDB layer
*
* @component sflow
*
* @comments tba
*
* @create 21/10/2007
*
* @author Rajesh G
* @end
*
**********************************************************************/

#ifndef USMDB_SFLOW_H
#define USMDB_SFLOW_H

#include <string.h>
#include <l7_common.h>
#include <l7_packet.h>

#include "comm_structs.h"


/*************************************************************
*   Begin Function Prototypes
* *************************************************************/

/*********************************************************************
* @purpose  Returns number of sFlow Receivers
*
* @param    UnitIndex unit index
*
* 
*
* @returns  numIndex
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usmdbsFlowNumRcvrIndex(L7_uint32 unit);
/*********************************************************************
* @purpose  Returns number of valid sFlow pollers
*
* @param    UnitIndex unit index
*
*
*
* @returns  count  Number of pollers
*
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_int32 usmDbCpIntIfNumCountGet(L7_uint32 unit);
/*********************************************************************
* @purpose  Returns number of valid sFlow samplers
*
* @param    UnitIndex unit index
*
*
*
* @returns  count  Number of samplers
*
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_int32 usmDbFsIntIfNumCountGet(L7_uint32 unit);
/*********************************************************************
* @purpose  Returns sFlow Datagram version 
*
* @param    UnitIndex  unit index
*
* @param    version    pointer to the version
*
* @param    index      datasource index of sflow component
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrDatagramVersionGet(L7_uint32 unit, L7_uint32 index,
                                         L7_uint32 *version);
/*********************************************************************
* @purpose  Returns sFlow Receiver Port
*
* @param    UnitIndex   unit index
* @param    port        Pointer to the port
* @param    index       index of sFlow Receiver
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrPortGet(L7_uint32 uintindex, L7_uint32 index,
                              L7_uint32 *port);


/*********************************************************************
* @purpose  Returns sFlow Receiver Entry
*
* @param    UnitIndex unit index 
*
* @param    index  datasource index of sflow component
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* 
* @notes    none
* 
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrEntryGet(L7_uint32 UnitIndex,L7_uint32 index);

/*********************************************************************
* @purpose  Returns first sFlow Receiver Entry
*
* @param    UnitIndex unit index
*
* @param    index  datasource index of sflow component
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowRcvrEntryFirstGet(L7_uint32 UnitIndex,L7_uint32 *index);

/*********************************************************************
* @purpose  Returns next sFlow Receiver Entry
*
* @param    UnitIndex unit index 
*
* @param    index  datasource index of sflow component
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* 
* @notes    none
* 
* @end
*********************************************************************/

L7_RC_t usmdbsFlowRcvrEntryNextGet(L7_uint32 UnitIndex,L7_uint32 *index);


/*********************************************************************
* @purpose  This function is used to get the  owner string of  
*           the sFlow receiver
*
* @param    index        Receiver Index
* @param    owner        owner string of the sflow receiver
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowRcvrOwnerGet(L7_uint32 UnitIndex, L7_uint32 index, 
                                L7_uchar8 *buffer);
 
/*********************************************************************
* @purpose  This function is used to set  owner string  to  
*           sFlow Receiver
*
* @param    index        receiver index
* @param    owner        owner of the sflow instance
* @paran    timeout      Timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*          Wheb ever we are setting owner string we should also 
*          pass timeout value to avoid race conditions
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowRcvrOwnerSet(L7_uint32 UnitIndex, L7_uint32 index,
                                L7_uchar8 *owner, L7_uint32 timeout);

/*********************************************************************
* @purpose returns sFlow Reciever Address Type
*
* @param    index        index of the sFlowRcvr Table
* @param    version      version of the reciever address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 index,
                                     L7_uint32 *version);

/*********************************************************************
* @purpose returns sFlow Reciever Address 
*
* @param    index        index of the sFlowRcvr Table
* @param    address      Reciever address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrAddressGet(L7_uint32 UnitIndex, L7_uint32 index,
                                 L7_inet_addr_t *ipAddr );



/*********************************************************************
* @purpose  Returns sflowFsEntry status
*
* @param    UnitIndex   unit index
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
L7_RC_t usmdbsFlowFsEntryGet(L7_uint32 UnitIndex,L7_char8 *index, L7_uint32 instance);



/*********************************************************************
* @purpose  Returns next sflowFsEntry index
*
* @param    UnitIndex   unit index
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
L7_RC_t usmdbsFlowFsEntryGetNext(L7_uint32 UnitIndex,L7_char8 *index, L7_uint32 *instance );



/*********************************************************************
* @purpose  Returns sflowCpEntry status
*
* @param    UnitIndex   unit index
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
L7_RC_t usmdbsFlowCpEntryGet(L7_uint32 UnitIndex,L7_uchar8 *index,
                                L7_uint32 instance);



/*********************************************************************
* @purpose  Returns next sflowCpEntry index
*
* @param    UnitIndex   unit index
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
L7_RC_t usmdbsFlowCpEntryGetNext(L7_uint32 UnitIndex,L7_uchar8 *index,
                                L7_uint32 *instance);
/*********************************************************************
* @purpose  This function is used to get the time out value of 
*           the Receiver
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    time        pointer to Remaining time before receiver expires
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowRcvrTimeoutGet(L7_uint32 UnitIndex,L7_uint32 index,
                                L7_uint32 *time);
/*********************************************************************
* @purpose  This function is used to get the time out value of
*           the Receiver.
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    size        pointer to maximum datagram size of the 
*                       receiver
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbSflowRcvrMaxDatagramSizeGet(L7_uint32 UnitIndex,L7_uint32 index,
                                L7_uint32 *size);
/*********************************************************************
* @purpose  This function is used to set the maximum datagram size of 
*           the Receiver.
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    size        Maximum datagram size of the
*                       receiver
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrMaximumDatagramSizeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 dSize);
/*********************************************************************
* @purpose  This function is used to set the Receiver Address type
*
* @param    UnitIndex    unit index
* @param    index        Receiver index
* @param    addressType  Address Type 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrAddressTypeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 addressType);

/*********************************************************************
* @purpose  This function is used to set Receiver IP Address
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    strIPaddr   IP Address string 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrAddressSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *strIPaddr);
/*********************************************************************
* @purpose  This function is used to set the port number of Receiver.
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    port        Port Number 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrPortSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 port);

/*********************************************************************
* @purpose  This function is used to set the time out value of
*           the Receiver.
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    timeout     Timeout value of the receiver
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowRcvrTimeoutSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 timeout);
/*********************************************************************
* @purpose  This function is used to set the Datagram version of 
*           the receiver.
*
* @param    UnitIndex   unit index
* @param    index       Receiver index
* @param    version     version of the receiver
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowRcvrDatagramVersionSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 version);



/******** FS table functions*********/
/*********************************************************************
* @purpose  This function is used to get the receiver index
*           associated with a sampler.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    receiver    Pointer to the  Receiver index associated with
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowFsReceiverGet(L7_uint32 UnitIndex,L7_uint32 intIfIndex, L7_uint32 instance,
             L7_uint32 *receiver);
/*********************************************************************
* @purpose  This function is used to get sampling rate
*           associated with a sampler.
*
* @param    UnitIndex     unit index
* @param    samplingRate  Pointer to the  sampling rate associated With
*                         this sampler
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowFsPacketSamplingRateGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                  L7_uint32 *samplingRate);
/*********************************************************************
* @purpose  This function is used to get the maximum header size
*           associated with a sampler.
*
* @param    UnitIndex      unit index
* @param    intIfIndex     internal interface number
* @param    instance       instance id of the sampler
* @param    maxHeaderSize  Pointer to the  maximum header size associated With
*                          this sampler
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  usmdbsFlowFsMaximumHeaderSizeGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                   L7_uint32 *maxHeaderSize);

/* snmp sflow FS set functions */
/*********************************************************************
* @purpose  This function is used to set the receiver index
*           associated with a sampler.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    receiver    Receiver index associated with
*                       this sampler
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowFsReceiverSet(L7_uint32 UnitIndex,L7_uint32 intIfIndex, L7_uint32 instance,
             L7_uint32 receiver);
/*********************************************************************
* @purpose  This function is used to set sampling rate
*           associated with a sampler.
*
* @param    UnitIndex     unit index
* @param    intIfIndex    internal interface number
* @param    instance      instance id of the sampler
* @param    samplingRate  Sampling rate associated With
*                         this sampler
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowFsPacketSamplingRateSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                  L7_uint32 samplingRate);
/*********************************************************************
* @purpose  This function is used to set the maximum header size
*           associated with a sampler.
*
* @param    UnitIndex      unit index
* @param    intIfIndex     internal interface number
* @param    instance       instance id of the sampler
* @param    maxHeaderSize  Maximum header size associated With
*                          this sampler
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  usmdbsFlowFsMaximumHeaderSizeSet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                   L7_uint32 maxHeaderSize);


/****** END OF SF FUNCTIONS**********/


/******* CP FUNCTIONS **********/
/*********************************************************************
* @purpose  This function is used to get the receiver index
*           associated with a counter poller.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    receiver    Pointer to the  Receiver index associated with
*                       this counter poller 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowCpReceiverGet(L7_uint32 UnitIndex,L7_uint32 intIfIndex, L7_uint32 instance,
             L7_uint32 *receiver);
/*********************************************************************
* @purpose  This function is used to get the counter poller interval 
*           associated with a counter poller.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    interval    Pointer to the  interval associated with
*                       this samplep
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowCpIntervalGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                   L7_uint32 *interval);

/** sflow cp set functions */
/*********************************************************************
* @purpose  This function is used to set the receiver index
*           associated with a counter poller.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    receiver    Receiver index associated with
*                       this counter poller
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowCpReceiverSet(L7_uint32 UnitIndex,L7_uint32 intIfIndex, L7_uint32 instance,
             L7_uint32 receiver);
/*********************************************************************
* @purpose  This function is used to set the counter poller interval
*           associated with a counter poller.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    interval    The Interval associated with
*                       this samplep
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbsFlowCpIntervalSet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                   L7_uint32 interval);

/********* END OF CP FUNCTIONS ****/

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
L7_RC_t usmdbsFlowAgentVersionGet(L7_uint32 unit, L7_char8 *version);
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
L7_RC_t usmdbsFlowAgentAddressTypeGet(L7_uint32 unit, L7_uint32 *type);
/*********************************************************************
* @purpose  This function is used to get the Agent Address.
*
* @param    UnitIndex   unit index
* @param    strIpAddr   Printable Agent Address
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowAgentAddressGet(L7_uint32 unit, L7_uchar8 *strIpAddr);
/*********************************************************************
* @purpose  Returns internal ifIndex to index
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    index
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowintIfIndexGet(L7_uint32 unit,  L7_uchar8 *index, L7_uint32 *intIfIndex);
/*********************************************************************
* @purpose  Get the current status of displaying sFlow packet debug info
*
* @param    *flag          value of the Packet Debug flag
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbsFlowPacketDebugTraceFlagGet(L7_BOOL *transmitFlag);
/*********************************************************************
* @purpose  Turns on/off the displaying of sFlow packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbsFlowPacketDebugTraceFlagSet(L7_BOOL transmitFlag);

#endif /* USMDB_SFLOW_H */
