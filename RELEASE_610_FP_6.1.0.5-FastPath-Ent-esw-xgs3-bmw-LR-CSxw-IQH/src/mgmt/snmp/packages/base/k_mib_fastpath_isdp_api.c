/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename k_mib_fastpath_isdp_api.c
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
#include "k_private_base.h"
#include "k_mib_fastpath_isdp_api.h"
#include "compdefs.h"
#include "usmdb_isdp_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "osapi_support.h"

L7_RC_t snmpIsdpDeviceIdFormatCapabilityGet(L7_uchar8 *buf)
{
  L7_uchar8 ch;
  L7_RC_t rc;

  rc = usmdbIsdpDeviceIdFormatCapabilityGet(&ch);

  if(rc == L7_SUCCESS)
  {
    if (ch & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_SERIAL_BITMASK)
    {
      buf[0] |= (0x80 >> D_agentIsdpGlobalDeviceIdFormatCpb_serialNumber);
    }
    if (ch & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_MAC_BITMASK)
    {
      buf[0] |= (0x80 >> D_agentIsdpGlobalDeviceIdFormatCpb_macAddress);
    }
    if (ch & L7_ISDP_DEVICE_ID_FORMAT_CAPABILITY_OTHER_BITMASK)
    {
      buf[0] |= (0x80 >> D_agentIsdpGlobalDeviceIdFormatCpb_other);
    }
  }

  return rc;
}

L7_RC_t snmpAgentIsdpAddresseGet(L7_uint32 intIfNum, L7_uchar8 *deviceId, L7_uchar8 *addresse)
{
  L7_RC_t rc;
  L7_uint32 ipAddressIndex, ipAddress;

  if((rc = usmdbIsdpIntfNeighborAddressGetNext(intIfNum, deviceId, 0, &ipAddressIndex)) == L7_SUCCESS)
  {
    if((rc = usmdbIsdpIntfNeighborIpAddressGet(intIfNum, deviceId, ipAddressIndex, &ipAddress)) == L7_SUCCESS)
    {
      osapiInetNtop(L7_AF_INET, (L7_uchar8*)&ipAddress, addresse, IPV6_DISP_ADDR_LEN);
      return rc;
    }
  }
  return rc;
}

L7_RC_t snmpAgentIsdpCpbGet(L7_uint32 intIfNum, L7_uchar8 *deviceId, 
                            L7_uchar8 *capabilities, L7_uint32 capabilitiesLength)
{
  L7_RC_t rc;
  L7_uint32 cap;

  if((rc = usmdbIsdpIntfNeighborCapabilitiesGet(intIfNum, deviceId, &cap)) == L7_SUCCESS)
  {
    rc = usmdbIsdpCapabilityStringGet(cap, capabilities, capabilitiesLength);
  }
  return rc;
}

L7_RC_t snmpAgentIsdpProtocolVersion(L7_uint32 intIfNum, L7_uchar8 *deviceId, 
                          L7_uchar8 *protocolVersion, L7_uint32 length)
{
  L7_RC_t rc;
  L7_uchar8 protoVersion;

  if((rc=usmdbIsdpIntfNeighborProtocolVersionGet(intIfNum, deviceId,
                                                  &protoVersion)) == L7_SUCCESS)
  {
    osapiSnprintf(protocolVersion, length, "%d", protoVersion);
  }

  return rc;
}

L7_RC_t
snmpAgentIsdpTableEntryGet(L7_uint32 intIfIndex, L7_uint32 *intIfNum)
{

  return usmDbIntIfNumFromExtIfNum(intIfIndex, intIfNum);
}

L7_RC_t
snmpAgentIsdpTableEntryNextGet(L7_uint32 *intIfIndex, L7_uint32 *intIfNum)
{
  L7_uint32 nextIntIfNum, localIntIfNum;
  L7_RC_t rc = L7_FAILURE;

  if ( (rc = usmDbIntIfNumFromExtIfNum(*intIfIndex, &localIntIfNum)) == L7_SUCCESS)
  {
    if ( (rc = usmDbGetNextVisibleIntIfNumber(localIntIfNum, &nextIntIfNum)) == L7_SUCCESS)
    {
      if ( (rc = usmDbExtIfNumFromIntIfNum(nextIntIfNum, intIfIndex)) == L7_SUCCESS)
      {
        *intIfNum = nextIntIfNum;
        return snmpAgentIsdpTableEntryGet(*intIfIndex, intIfNum);
      }
    }
  }

  return rc;
}
