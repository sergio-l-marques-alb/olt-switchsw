/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_sflow_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_sflow_api.c
*
* @create 10/23/2007
*
* @author Rajesh G
*
* @end
*
**********************************************************************/
extern L7_char8 *SNMP_SFLOW_IFINDEX;
extern L7_char8 *SNMP_SFLOW_VINDEX;
 
L7_RC_t snmpSflowDataSourceGet(L7_uint32 UnitIndex, L7_uint32 index);

L7_RC_t snmpSflowRcvrEntryGet(L7_uint32 UnitIndex, L7_uint32 index);

L7_RC_t snmpSflowRcvrEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *index);

L7_RC_t snmpsFlowRcvrOwnerGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buffer);

L7_RC_t snmpsFlowRcvrTimeoutGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time);

L7_RC_t snmpsFlowRcvrMaximumDatagramSizeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *dSize);

L7_RC_t snmpsFlowRcvrAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *addType);
 
L7_RC_t snmpsFlowRcvrAddressGet(L7_uint32 UnitIndex, L7_uint32 index, L7_inet_addr_t *ipAddr);

L7_RC_t snmpsFlowRcvrPortGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *port);

L7_RC_t snmpsFlowRcvrDatagramVersionGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *version);


/* SNMP SET FUNCTIONS */

L7_RC_t snmpsFlowRcvrOwnerSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *ownerBuffer,
                              L7_uint32 timeout);

L7_RC_t snmpsFlowRcvrTimeoutSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 timeout);

L7_RC_t snmpsFlowRcvrMaximumDatagramSizeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 dSize);

L7_RC_t snmpsFlowRcvrAddressTypeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 addressType);

L7_RC_t snmpsFlowRcvrAddressSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *addressBuffer);

L7_RC_t snmpsFlowRcvrPortSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 port);

L7_RC_t snmpsFlowRcvrDatagramVersionSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 version);


/* SNMP Agent Get Functions */

L7_RC_t snmpSflowAgentVersionGet(L7_uint32 UnitIndex, L7_char8 *buffer);

L7_RC_t snmpSflowAgentAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 *type);

L7_RC_t snmpSflowAgentAddressGet(L7_uint32 UnitIndex, L7_uchar8 *strIpAddr);


/* snmp sflow FS get functions */

L7_RC_t snmpsFlowFsEntryGet (L7_uint32 UnitIndex, L7_char8 *strOID, L7_uint32 instance);

L7_RC_t snmpsFlowFsEntryGetNext (L7_uint32 UnitIndex, L7_char8 *strOID, L7_int32 *instance);


L7_RC_t snmpsFlowFsReceiverGet(L7_uint32 UnitIndex, L7_char8 *strOID, L7_uint32 instance, L7_uint32 *receiver);

L7_RC_t snmpsFlowFsPacketSamplingRateGet(L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
                  L7_uint32 *samplingRate);

L7_RC_t  snmpsFlowFsMaximumHeaderSizeGet( L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
                   L7_uint32 *maxHeaderSize);

/* snmp sflow FS set functions */

L7_RC_t snmpsFlowFsReceiverSet(L7_uint32 UnitIndex,L7_uint32 instance, L7_uint32 ifIndex,
             L7_uint32 receiver);

L7_RC_t snmpsFlowFsPacketSamplingRateSet(L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
                  L7_uint32 samplingRate);

L7_RC_t  snmpsFlowFsMaximumHeaderSizeSet( L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
                   L7_uint32 maxHeaderSize);


/** sflow cp get functions */

L7_RC_t snmpsFlowCpEntryGet(L7_uint32 UnitIndex, L7_char8 *strOID, L7_uint32 instance);

L7_RC_t snmpsFlowCpEntryGetNext(L7_uint32 UnitIndex, L7_char8 *strOID, L7_int32 *instance);

L7_RC_t snmpsFlowIntIfNumGet(L7_uint32 UnitIndex, L7_char8 *index, L7_uint32 *intIfIndex);

L7_RC_t snmpsFlowCpReceiverGet(L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
             L7_uint32 *receiver);
L7_RC_t snmpsFlowCpIntervalGet( L7_uint32 UnitIndex, L7_uchar8 *strOID, L7_uint32 instance,
                   L7_uint32 *intervali);

/** sflow cp set functions */


L7_RC_t snmpsFlowCpReceiverSet(L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
             L7_uint32 receiver);

L7_RC_t snmpsFlowCpIntervalSet( L7_uint32 UnitIndex, L7_uint32 instance, L7_uint32 ifIndex,
                   L7_uint32 interval);
