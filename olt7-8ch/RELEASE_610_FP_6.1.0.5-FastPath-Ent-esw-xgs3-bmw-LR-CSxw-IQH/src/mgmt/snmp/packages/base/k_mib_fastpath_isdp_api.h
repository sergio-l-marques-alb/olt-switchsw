/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename k_mib_fastpath_isdp_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 20/12/2007
*
* @author Rostyslav Ivasiv
*
* @end
*
**********************************************************************/
#ifndef K_MIB_FASTPATH_ISDP_API_H_
#define K_MIB_FASTPATH_ISDP_API_H_

#include "compdefs.h"
#include "l3_addrdefs.h"

L7_RC_t snmpIsdpDeviceIdFormatCapabilityGet(L7_uchar8 *buf);
L7_RC_t snmpAgentIsdpProtocolVersion(L7_uint32 intIfNum, L7_uchar8 *deviceId, L7_uchar8 *protocolVersion, L7_uint32 length);
L7_RC_t snmpAgentIsdpCpbGet(L7_uint32 intIfNum, L7_uchar8 *deviceId, L7_uchar8 *capabilities, L7_uint32 capabilitiesLength);
L7_RC_t snmpAgentIsdpAddresseGet(L7_uint32 intIfNum, L7_uchar8 *deviceId, L7_uchar8 *addresse);

L7_RC_t snmpAgentIsdpTableEntryGet(L7_uint32 intIfIndex, L7_uint32 *intIfNum);
L7_RC_t snmpAgentIsdpTableEntryNextGet(L7_uint32 *intIfIndex, L7_uint32 *intIfNum);

#endif /*K_MIB_FASTPATH_ISDP_API_H_*/
