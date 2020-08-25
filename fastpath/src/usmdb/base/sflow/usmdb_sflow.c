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
#include "commdefs.h"
#include <string.h>
#include "osapi_support.h"
#include <l7_common.h>
#include <l7_packet.h>
#include "l3_addrdefs.h"

#include "usmdb_sflow.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include <stdlib.h>
#include "sflow_api.h"
#include "sflow_debug_api.h"
#include "sflow_defs.h"
#include "sflow_exports.h"

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
L7_int32 usmdbsFlowNumRcvrIndex(L7_uint32 unit)
{
  return L7_SFLOW_MAX_RECEIVERS;
}
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
L7_int32 usmDbCpIntIfNumCountGet(L7_uint32 unit)
{
  L7_uint32 count = 0, intIfNum, IfIndex, rcvr_index= 0;
  L7_uint32 instance = FD_SFLOW_INSTANCE;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 buf[20];
  
  rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF, 0, &intIfNum); 
  if (rc == L7_SUCCESS)
  {
    while(1)
    {
      memset(buf, 0, sizeof(buf));
      usmDbIfIndexGet(unit, intIfNum, &IfIndex);
      osapiSnprintf(buf,sizeof(buf), "ifIndex.%d",IfIndex);
    
      if( usmdbsFlowCpReceiverGet(unit, intIfNum, instance, &rcvr_index) == L7_SUCCESS)
      {
        count++;
      }
      if((usmdbsFlowCpEntryGetNext(unit, buf, &instance) != L7_SUCCESS )||
        (usmdbsFlowintIfIndexGet(unit, buf, &intIfNum) != L7_SUCCESS))
      {
         break;
      }
    }
    
  }
  return count;
}
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
L7_int32 usmDbFsIntIfNumCountGet(L7_uint32 unit)
{
  L7_uint32 count = 0, intIfNum, IfIndex, rcvr_index = 0;
  L7_uint32 instance = FD_SFLOW_INSTANCE;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 buf[20];

  rc = usmDbIntIfNumTypeFirstGet(unit, USM_PHYSICAL_INTF, 0, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    while(1)
    {
      memset(buf, 0, sizeof(buf));
      usmDbIfIndexGet(unit, intIfNum, &IfIndex);
      osapiSnprintf(buf,sizeof(buf), "ifIndex.%d",IfIndex);

      if( usmdbsFlowFsReceiverGet(unit, intIfNum, instance, &rcvr_index) == L7_SUCCESS)
      {
        count++;
      }
      if((usmdbsFlowFsEntryGetNext(unit, buf, &instance) != L7_SUCCESS )||
        (usmdbsFlowintIfIndexGet(unit, buf, &intIfNum) != L7_SUCCESS))
      {
         break;
      }
    }

  }
  return count;
}
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
                                         L7_uint32 *version)
{
  return sFlowRcvrDatagramVersionGet(index, version);
}
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
                              L7_uint32 *port)
{
  return sFlowRcvrPortGet(index, port);
}


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
L7_RC_t usmdbsFlowRcvrEntryGet(L7_uint32 UnitIndex,L7_uint32 index)
{
  return sFlowRcvrGet(index);  
}
/*********************************************************************
* @purpose  Returns sFlow First Receiver Entry
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
L7_RC_t usmdbsFlowRcvrEntryFirstGet(L7_uint32 UnitIndex,L7_uint32 *index)
{
  *index = SFLOW_FIRST_RCVR_INDEX ;
  return L7_SUCCESS;
}

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

L7_RC_t usmdbsFlowRcvrEntryNextGet(L7_uint32 UnitIndex,L7_uint32 *index)
{
  return sFlowRcvrNextGet(*index, index);
}



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
                                L7_uchar8 *buffer)
{

  return sFlowRcvrOwnerGet(index, buffer);
}
 
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
                                L7_uchar8 *owner, L7_uint32 timeout)
{
                                
  return sFlowRcvrOwnerSet(index, owner, timeout);
}

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
                                     L7_uint32 *version)
{
    return sFlowRcvrAddressTypeGet(index, version);
}

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
                                     L7_inet_addr_t *ipAddr)
{

  if(sFlowRcvrAddressGet(index, ipAddr) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}



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
* *            
* @end
*********************************************************************/
L7_RC_t usmdbsFlowFsEntryGet(L7_uint32 UnitIndex,L7_char8 *index, L7_uint32 instance)
{
  return sFlowFsEntryGet(UnitIndex, index, instance);
}

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
L7_RC_t usmdbsFlowFsEntryGetNext(L7_uint32 UnitIndex, L7_char8 *index, L7_uint32 *instance )
{
  return sFlowFsEntryGetNext(UnitIndex, index, instance );
}



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
L7_RC_t usmdbsFlowCpEntryGet(L7_uint32 UnitIndex, L7_uchar8 *index,
                                L7_uint32 instance)
{
  return sFlowCpEntryGet(UnitIndex, index, instance);
}



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
L7_RC_t usmdbsFlowCpEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *index,
                                L7_uint32 *instance)
{
  return sFlowCpEntryGetNext(UnitIndex, index, instance);
}

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

L7_RC_t usmdbsFlowRcvrTimeoutGet(L7_uint32 UnitIndex, L7_uint32 index,
                                L7_uint32 *time)
{
  return  sFlowRcvrTimeoutGet(index, time);
}
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
                                L7_uint32 *size)
{
 return sFlowRcvrMaximumDatagramSizeGet(index, size);
}
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
L7_RC_t usmdbsFlowRcvrMaximumDatagramSizeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 dSize)
{
  return sFlowRcvrMaximumDatagramSet(index, dSize);
}
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
L7_RC_t usmdbsFlowRcvrAddressTypeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 addressType)
{
  return sFlowRcvrAddressTypeSet(index, addressType);
}

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
L7_RC_t usmdbsFlowRcvrAddressSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *strIPaddr)
{
  L7_inet_addr_t IPaddr;
  if(usmDbParseInetAddrFromStr(strIPaddr, &IPaddr) == L7_SUCCESS)
  {
     return sFlowRcvrAddressSet(index, &IPaddr);
  }
  return L7_FAILURE;
}
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
L7_RC_t usmdbsFlowRcvrPortSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 port)
{
  return sFlowRcvrPortSet(index, port);
}
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

L7_RC_t usmdbsFlowRcvrTimeoutSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 timeout)
{
  return sFlowRcvrTimeoutSet(index, timeout);
}
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
L7_RC_t usmdbsFlowRcvrDatagramVersionSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 version)
{
  return sFlowRcvrDatagramVersionSet(index, version);
}



/******** FS table functions*********/
/*********************************************************************
* @purpose  This function is used to get the receiver index
*           associated with a sampler.
*
* @param    UnitIndex   unit index
* @param    intIfIndex  internal interface number
* @param    instance    instance id of the sampler
* @param    receiver    Pointer to the  Receiver index associated with
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
L7_RC_t usmdbsFlowFsReceiverGet(L7_uint32 UnitIndex,L7_uint32 intIfIndex, L7_uint32 instance,
             L7_uint32 *receiver)
{
  return sFlowFsReciverGet(intIfIndex, instance, receiver);
}
/*********************************************************************
* @purpose  This function is used to get sampling rate
*           associated with a sampler.
*
* @param    UnitIndex     unit index
* @param    intIfIndex    internal interface number
* @param    instance      instance id of the sampler
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
                  L7_uint32 *samplingRate)
{
  return sFlowFsPacketSamplingRateGet(intIfIndex, instance, samplingRate);
}
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
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  usmdbsFlowFsMaximumHeaderSizeGet( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 instance,
                   L7_uint32 *maxHeaderSize)
{
   return sFlowFsMaximumHeaderSizeGet( intIfIndex, instance, maxHeaderSize);
}

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
             L7_uint32 receiver)
{
  return sFlowFsReceiverSet( intIfIndex, instance, receiver);
}
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
                  L7_uint32 samplingRate)
{
  return sFlowFsPacketSamplingRateSet( intIfIndex, instance, samplingRate);
}
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
                   L7_uint32 maxHeaderSize)
{
  return sFlowFsMaximumHeaderSizeSet(intIfIndex, instance, maxHeaderSize);
}


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
             L7_uint32 *receiver)
{
  return sFlowCpReciverGet(intIfIndex, instance, receiver);
}
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
                   L7_uint32 *interval)
{
   return sFlowCpIntervalGet(intIfIndex, instance, interval);
}

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
             L7_uint32 receiver)
{
  return sFlowCpReceiverSet(intIfIndex, instance, receiver);
}
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
                   L7_uint32 interval)
{
   return sFlowCpIntervalSet(intIfIndex, instance, interval);
}

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
L7_RC_t usmdbsFlowAgentVersionGet(L7_uint32 unit, L7_char8 *version)
{
  return sFlowAgentVersionGet(version);
}
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
L7_RC_t usmdbsFlowAgentAddressTypeGet(L7_uint32 unit, L7_uint32 *type)
{
  return sFlowAgentAddressTypeGet(type);
}
/*********************************************************************
* @purpose  This function is used to get the Agent Address.
*
* @param    UnitIndex   unit index
* @param    type        Printable Agent Addres
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmdbsFlowAgentAddressGet(L7_uint32 unit, L7_uchar8 *strIpAddr)
{
  return sFlowAgentAddressGet(strIpAddr);
}
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

L7_RC_t usmdbsFlowintIfIndexGet(L7_uint32 unit, L7_uchar8 *index, L7_uint32 *intIfIndex)
{
  L7_uint32 ifIndex;
  ifIndex = atoi( index+(sizeof(L7_SFLOW_IFINDEX)));
  if(usmDbIntIfNumFromExtIfNum(ifIndex, intIfIndex) == L7_SUCCESS )
  {
     return L7_SUCCESS;
  }
  return L7_FAILURE;
}
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
L7_RC_t usmDbsFlowPacketDebugTraceFlagGet(L7_BOOL *transmitFlag)
{
  return sFlowDebugPacketTraceFlagGet(transmitFlag);
}

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
L7_RC_t usmDbsFlowPacketDebugTraceFlagSet(L7_BOOL transmitFlag)
{
  return sFlowDebugPacketTraceFlagSet(transmitFlag);
}

