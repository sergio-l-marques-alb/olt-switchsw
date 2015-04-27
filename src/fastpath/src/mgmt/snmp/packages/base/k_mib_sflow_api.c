/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_sflow_api.c
*
* @purpose    Wrapper functions for sFlow  MIB
*
* @component  SNMP
*
* @comments
*
* @create     10/23/2007
*
* @author     Rajesh G
* @end
*
**********************************************************************/
#include "k_private_base.h"

#include "commdefs.h"

#include "k_mib_sflow_api.h"
#include "usmdb_sflow.h"
#include "usmdb_util_api.h"
#include "sflow_exports.h"

/*********************************************************************
*
* @purpose  Get the Receiver table enrtry 
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     index value of receiver table
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpSflowRcvrEntryGet(L7_uint32 UnitIndex, L7_uint32 index)
{
  return usmdbsFlowRcvrEntryGet(UnitIndex, index);
}
/*********************************************************************
*
* @purpose  Get the next Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((output))     next index value of receiver table
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpSflowRcvrEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *index)
{
 return usmdbsFlowRcvrEntryNextGet(UnitIndex,index);

}
/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value 
* @param    buffer @b((output))   owner string of the index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrOwnerGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buffer) 
{
  return usmdbsFlowRcvrOwnerGet(UnitIndex, index, buffer);
}
/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    time @b((output))     Pointer to the receiver time out value 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrTimeoutGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time)
{
   return usmdbsFlowRcvrTimeoutGet(UnitIndex, index, time);
}

/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    dSize @b((output))    Pointer to the maximum datagram size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpsFlowRcvrMaximumDatagramSizeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *dSize)
{
   return usmdbSflowRcvrMaxDatagramSizeGet(UnitIndex, index, dSize);

}
/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    addType @b((output))    Pointer to the Address type of receiver
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *addType)
{
   return usmdbsFlowRcvrAddressTypeGet(UnitIndex, index, addType);
}

/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    ipAddr @b((output))   Pointer to the address of receiver
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrAddressGet(L7_uint32 UnitIndex, L7_uint32 index,  L7_inet_addr_t *ipAddr)
{
 
  return  usmdbsFlowRcvrAddressGet(UnitIndex, index, ipAddr);
}

/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    port @b((output))   Pointer to the port
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpsFlowRcvrPortGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *port)
{
   return usmdbsFlowRcvrPortGet(UnitIndex, index, port);

}
/*********************************************************************
*
* @purpose  Get the Receiver table enrtry
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    version @b((output))  Pointer to the sFlow Datagram Version
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes    Datagram version will be allways 5
*    
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrDatagramVersionGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *version)
{
 
  return usmdbsFlowRcvrDatagramVersionGet(UnitIndex, index, version);
}




/* SNMP set functions */
/*********************************************************************
*
* @purpose  Set the owner string for the Receiver 
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    buffer @b((input))   owner string of the index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrOwnerSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *ownerBuffer, 
                               L7_uint32 timeout)
{
  return usmdbsFlowRcvrOwnerSet(UnitIndex, index, ownerBuffer, timeout);  
}
/*********************************************************************
*
* @purpose  Set the timeout value for the receiver
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    time @b((input))      Receiver time out value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrTimeoutSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 timeout)
{
  
  return usmdbsFlowRcvrTimeoutSet(UnitIndex, index, timeout);
}

/*********************************************************************
*
* @purpose  Set the maxmum datagram size of the Receiver 
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    dSize @b((input))     Maximum datagram size 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrMaximumDatagramSizeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 dSize)
{
  return usmdbsFlowRcvrMaximumDatagramSizeSet(UnitIndex, index, dSize);
}
/*********************************************************************
*
* @purpose  Set  the Receiver Address Type (IPv4 or IPV6)
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    addType @b((input))   Address type 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrAddressTypeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 addressType)
{
  
  return usmdbsFlowRcvrAddressTypeSet(UnitIndex, index, addressType);
}
/*********************************************************************
*
* @purpose  Set the Receiver Address
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    ipAddr @b((input))    Address of receiver
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrAddressSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *addressBuffer)
{
  return usmdbsFlowRcvrAddressSet(UnitIndex, index, addressBuffer);
}
/*********************************************************************
*
* @purpose  Set the receiver port
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    port @b((input))      Port value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrPortSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 port)
{
  
  return usmdbsFlowRcvrPortSet(UnitIndex, index, port); 
}
/*********************************************************************
*
* @purpose  St the sFlow datagram version
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    index @b((input))     Index value
* @param    version @b((input))   sFlow Datagram Version
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes    Datagram version will be allways 5
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowRcvrDatagramVersionSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 version)
{
  
  return usmdbsFlowRcvrDatagramVersionSet(UnitIndex, index, version);
}



/* SNMP Agent get Functions */

/*********************************************************************
*
* @purpose  Get the sFlow Agent Version
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    buffer    @b((output)) sFlow Agent version
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpSflowAgentVersionGet(L7_uint32 UnitIndex, L7_char8 *buffer)
{
  return usmdbsFlowAgentVersionGet(UnitIndex, buffer);
}
/*********************************************************************
*
* @purpose  Get the sFlow Agent Address type 
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    type    @b((output))   Pointer to sFlow Agent address type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpSflowAgentAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 *type)
{
  return usmdbsFlowAgentAddressTypeGet( UnitIndex, type);
}
/*********************************************************************
*
* @purpose  Get the sFlow Agent Address type
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    ipAddr    @b((output)) Pointer to sFlow Agent address 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpSflowAgentAddressGet(L7_uint32 UnitIndex, L7_uchar8 *strIpAddr)
{
  return usmdbsFlowAgentAddressGet(UnitIndex, strIpAddr); 
}



/* snmp sflow FS get functions */

/*********************************************************************
*
* @purpose  Get the sFlow sampler table entry
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowFsEntryGet (L7_uint32 UnitIndex,char *index, L7_uint32 instance)
{
  if (usmdbsFlowFsEntryGet(UnitIndex, index, instance) == L7_SUCCESS )
  {
     return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next sFlow sampler table entry
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((output))   OID of sFlowFs Table
* @param    instance @b((output))   Instance number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowFsEntryGetNext(L7_uint32 UnitIndex, L7_char8 *index, L7_int32 *instance)
{
  if (usmdbsFlowFsEntryGetNext(UnitIndex, index, instance) == L7_SUCCESS )
  {
     return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Get the sFlow sampler receiver
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
* @param    receiver @b((output))  Pointer to the sflow receiver index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowFsReceiverGet(L7_uint32 UnitIndex, L7_char8 *strOID, L7_uint32 instance,
             L7_uint32 *receiver)
{
  L7_uint32 intIfIndex;
  L7_RC_t   rc = L7_FAILURE;
  if(usmdbsFlowintIfIndexGet(UnitIndex, strOID, &intIfIndex) == L7_SUCCESS)
  {
    rc = usmdbsFlowFsReceiverGet( UnitIndex, intIfIndex, instance, receiver);
    if ( rc == L7_FAILURE)
    {
      *receiver = L7_NULL;
      rc = L7_SUCCESS;
    }
  }
  return rc;
}
/*********************************************************************
*
* @purpose  Get the sFlow sampler packet sampling rate
*
* @param    UnitIndex @b((input))      the unit for this operation
* @param    index    @b((input))       OID of sFlowFs Table
* @param    instance @b((input))       Instance number
* @param    samplingRate @b((output))  Pointer to the packet sampling rate
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowFsPacketSamplingRateGet(L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
                  L7_uint32 *samplingRate)
{
  L7_uint32 intIfIndex;
  L7_RC_t   rc = L7_FAILURE;
  if(usmdbsFlowintIfIndexGet(UnitIndex, strOID, &intIfIndex) == L7_SUCCESS)
  {
    rc = usmdbsFlowFsPacketSamplingRateGet( UnitIndex, intIfIndex, instance, samplingRate);
    if ( rc == L7_FAILURE)  
    {
      *samplingRate = FD_SFLOW_SAMPLING_RATE;
      rc = L7_SUCCESS;
    }
  }
  return rc;
}
/*********************************************************************
*
* @purpose  Get the sFlow sampler maximum header size
*
* @param    UnitIndex @b((input))       the unit for this operation
* @param    index    @b((input))        OID of sFlowFs Table
* @param    instance @b((input))        Instance number
* @param    maxHeaderSize @b((output))  Pointer to the maximum header size
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  snmpsFlowFsMaximumHeaderSizeGet( L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
                   L7_uint32 *maxHeaderSize)
{
  L7_uint32 intIfIndex;
  L7_RC_t   rc = L7_FAILURE;

  if(usmdbsFlowintIfIndexGet(UnitIndex, strOID, &intIfIndex) == L7_SUCCESS)
  {
    rc = usmdbsFlowFsMaximumHeaderSizeGet(UnitIndex, intIfIndex, instance, maxHeaderSize);
    if ( rc == L7_FAILURE)
    {
      *maxHeaderSize = FD_SFLOW_DEFAULT_HEADER_SIZE;
      rc = L7_SUCCESS;
    }
  }
  return rc;
}

/* snmp sflow FS set functions */
/*********************************************************************
*
* @purpose  Set the sFlow sampler receiver
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
* @param    receiver @b((output))  pointer to the sflow receiver index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowFsReceiverSet(L7_uint32 UnitIndex,L7_uint32 instance, L7_uint32 ifIndex,
             L7_uint32 receiver)
{
  L7_uint32 intIfIndex;

  if((usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex) != L7_SUCCESS) ||
                (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfIndex) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }
  return usmdbsFlowFsReceiverSet( UnitIndex, intIfIndex, instance, receiver);
}
/*********************************************************************
*
* @purpose  Set the sFlow sampler packet sampling rate
*
* @param    UnitIndex @b((input))      the unit for this operation
* @param    index    @b((input))       OID of sFlowFs Table
* @param    instance @b((input))       Instance number
* @param    samplingRate @b((output))  packet sampling rate
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowFsPacketSamplingRateSet(L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex, 
                  L7_uint32 samplingRate)
{
  L7_uint32 intIfIndex;
  if((usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex) != L7_SUCCESS) ||
                (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfIndex) != L7_SUCCESS)) 
  {
    return L7_FAILURE;
  }
  return usmdbsFlowFsPacketSamplingRateSet( UnitIndex, intIfIndex, instance, samplingRate);
}
/*********************************************************************
*
* @purpose  Set the sFlow sampler maximum header size
*
* @param    UnitIndex @b((input))       the unit for this operation
* @param    index    @b((input))        OID of sFlowFs Table
* @param    instance @b((input))        Instance number
* @param    maxHeaderSize @b((output))  Maximum header size
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t  snmpsFlowFsMaximumHeaderSizeSet( L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
                   L7_uint32 maxHeaderSize)
{
  L7_uint32 intIfIndex;
  if((usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex) != L7_SUCCESS) ||
                (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfIndex) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }
  return usmdbsFlowFsMaximumHeaderSizeSet(UnitIndex, intIfIndex, instance, maxHeaderSize);
}


/** sflow cp get functions */


/*********************************************************************
*
* @purpose  Get the sFlow poller table entry
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpsFlowCpEntryGet(L7_uint32 UnitIndex, L7_char8 *index, L7_uint32 instance)
{

  return usmdbsFlowCpEntryGet(UnitIndex, index, instance);
}
/*********************************************************************
*
* @purpose  Get the sFlow next poller table entry
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowCpEntryGetNext(L7_uint32 UnitIndex, L7_char8 *index, L7_int32 *instance)
{

  return usmdbsFlowCpEntryGetNext(UnitIndex, index, instance);

}

/*********************************************************************
*
* @purpose  Get the sFlow poller receiver
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
* @param    receiver @b((output))  Pointer to the sflow receiver index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowCpReceiverGet(L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
             L7_uint32 *receiver)
{
  L7_uint32 intIfIndex;
  L7_RC_t   rc = L7_FAILURE;

  if(usmdbsFlowintIfIndexGet(UnitIndex, strOID, &intIfIndex) == L7_SUCCESS)
  {
    rc =  usmdbsFlowCpReceiverGet(UnitIndex, intIfIndex, instance, receiver);
    if ( rc == L7_FAILURE)
    {
      *receiver = L7_NULL;
      rc = L7_SUCCESS;
    }
  }
  return rc;
}
/*********************************************************************
*
* @purpose  Get the sFlow poller interval
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
* @param    interval @b((output))  Pointer to the sflow poller interval
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowCpIntervalGet( L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
                   L7_uint32 *interval)
{
  L7_uint32 intIfIndex;
  L7_RC_t   rc = L7_FAILURE;
  
  if(usmdbsFlowintIfIndexGet(UnitIndex, strOID, &intIfIndex) == L7_SUCCESS)
  {
    rc = usmdbsFlowCpIntervalGet(UnitIndex, intIfIndex, instance, interval);
    if ( rc == L7_FAILURE)
    {
      *interval = FD_SFLOW_POLL_INTERVAL;
      rc = L7_SUCCESS;
    }
  }
  return rc;
}

/** sflow cp set functions */
/*********************************************************************
*
* @purpose  Set the sFlow sampler receiver
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
* @param    receiver @b((output))  sFlow receiver index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t snmpsFlowCpReceiverSet(L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
             L7_uint32 receiver)
{
  L7_uint32 intIfIndex;
  if((usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex) != L7_SUCCESS) ||
                (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfIndex) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }
  return usmdbsFlowCpReceiverSet(UnitIndex, intIfIndex, instance, receiver);
}
/*********************************************************************
*
* @purpose  Set the sFlow poller interval
*
* @param    UnitIndex @b((input))  the unit for this operation
* @param    index    @b((input))   OID of sFlowFs Table
* @param    instance @b((input))   Instance number
* @param    interval @b((output))  sFlow poller interval
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowCpIntervalSet( L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
                   L7_uint32 interval)
{
  L7_uint32 intIfIndex;
  if((usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex) != L7_SUCCESS) ||
                (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfIndex) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }
  return usmdbsFlowCpIntervalSet(UnitIndex, intIfIndex, instance, interval);
}

/*********************************************************************
*
* @purpose  Set the sFlow poller interval
*
* @param    UnitIndex @b((input))    the unit for this operation
* @param    index    @b((input))     str format of ifIdex
* @param    intIfIndex @b((output))  internal interface number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @notes
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t snmpsFlowIntIfNumGet(L7_uint32 UnitIndex, L7_char8 *index, L7_uint32 *intIfIndex)
{
  L7_uint32 ifIndex;
  if( strcmp(L7_SFLOW_IFINDEX, index) < 0)
  {
    ifIndex = atoi( index+(sizeof("ifIndex")));
    if((usmDbVisibleExtIfNumberCheck(UnitIndex,ifIndex) == L7_SUCCESS)&&
                (usmDbIntIfNumFromExtIfNum(ifIndex, intIfIndex)== L7_SUCCESS))
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}
