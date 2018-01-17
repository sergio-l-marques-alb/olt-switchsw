/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_portsecurity_api.c
*
* @purpose    Wrapper functions for Fastpath Port Security MIB
*
* @component  SNMP
*
* @comments
*
* @create     06/09/2004
*
* @author     kmanish
* @end
*
**********************************************************************/
#include "k_private_base.h"

#include "k_mib_fastpath_portsecurity_api.h"
#include "usmdb_pml_api.h"
#include "dot1q_exports.h"
#include "snmp_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"


/* Begin Function Definitions: k_mib_fastpath_portsecurity_api.c */

/****************************************************************************************/

L7_RC_t snmpAgentGlobalPortSecurityModeGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbPmlAdminModeGet(USMDB_UNIT_CURRENT,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
      case L7_ENABLE :
        *val = D_agentGlobalPortSecurityMode_enable;
      break;

      case L7_DISABLE :
        *val = D_agentGlobalPortSecurityMode_disable;
      break;

      default:
        /* unknown value */
        rc = L7_FAILURE;
      break;
    }
  }
  
  return rc;
}

L7_RC_t
snmpAgentGlobalPortSecurityModeSet(L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentGlobalPortSecurityMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentGlobalPortSecurityMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  rc = usmDbPmlAdminModeSet(USMDB_UNIT_CURRENT, temp_val);

  return rc;
}

L7_RC_t
snmpAgentPortSecurityEntryGet(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{

  L7_uint32 intIfNum;
  L7_RC_t rc = L7_FAILURE;

  if (extIfNum == 0)
    rc = L7_FAILURE;

  else
    {
      /* check to see if this is a visible external interface number */
      if (usmDbVisibleExtIfNumberCheck(UnitIndex,extIfNum) == L7_SUCCESS)
        {
          /* convert external interface number into internal interface number*/

          if(usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
            {
              /*validate the interface number*/
              
              if ( usmDbPmlIsValidIntf(UnitIndex, intIfNum)  ==  L7_TRUE)
                rc = L7_SUCCESS;

            }
        }

    }

 return (rc);

}


L7_RC_t
snmpAgentPortSecurityEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *extIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum;

  /* find the next visible external interface */
  if (usmDbGetNextVisibleExtIfNumber(*extIfNum, extIfNum) == L7_SUCCESS)
  {
    /* convert to internal interface */
    if (usmDbIntIfNumFromExtIfNum(*extIfNum, &intIfNum) == L7_SUCCESS)
    {
      /* if this is a valid index for the table */
      if ( usmDbPmlIsValidIntf(UnitIndex, intIfNum)  ==  L7_TRUE)
      {
        rc = L7_SUCCESS;
      }
      else
      {
        if (usmDbPmlNextValidIntfGet(intIfNum,&intIfNum) == L7_SUCCESS)
        {
          /* convert internal interface to external */
          if (usmDbExtIfNumFromIntIfNum(intIfNum, extIfNum) 
                                         == L7_SUCCESS)
          {
            rc =  L7_SUCCESS;
          }
        }
      }
    }
  }
  return(rc);
}


L7_RC_t snmpAgentPortSecurityModeGet(L7_uint32 intfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  rc = usmDbPmlIntfModeGet(USMDB_UNIT_CURRENT,intfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
      case L7_ENABLE :
        *val = D_agentPortSecurityMode_enable;
      break;

      case L7_DISABLE :
        *val = D_agentPortSecurityMode_disable;
      break;

      default:
        /* unknown value */
        rc = L7_FAILURE;
      break;
    }
  }
  
  return rc;
}

L7_RC_t
snmpAgentPortSecurityModeSet(L7_uint32 intfNum, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentPortSecurityMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortSecurityMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  rc = usmDbPmlIntfModeSet(USMDB_UNIT_CURRENT,intfNum, temp_val);

  return rc;
}

L7_RC_t snmpAgentPortSecurityViolationTrapModeGet(L7_uint32 intfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbPmlIntfViolationTrapModeGet(USMDB_UNIT_CURRENT,intfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
      case L7_ENABLE :
        *val = D_agentPortSecurityViolationTrapMode_enable;
      break;

      case L7_DISABLE :
        *val = D_agentPortSecurityViolationTrapMode_disable;
      break;

      default:
        /* unknown value */
        rc = L7_FAILURE;
      break;
    }
  }
  
  return rc;
}

L7_RC_t
snmpAgentPortSecurityViolationTrapModeSet(L7_uint32 intfNum, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentPortSecurityViolationTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortSecurityViolationTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  rc = usmDbPmlIntfViolationTrapModeSet(USMDB_UNIT_CURRENT,intfNum, temp_val);

  return rc;
}

L7_RC_t
snmpAgentPortSecurityStaticMACsGet( L7_uint32 intIfNum, L7_uchar8 *staticMACList)
{
  L7_RC_t rc = L7_FAILURE;

  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId, count = 0;
  
  L7_uchar8 staticMAC[SNMP_BUFFER_LEN];
  bzero(staticMAC, SNMP_BUFFER_LEN);
  
  rc = usmDbPmlIntfStaticEntryGetFirst(USMDB_UNIT_CURRENT,intIfNum,&macAddr, &vlanId);

  while (rc == L7_SUCCESS)
  {
    /* Storing in string staticMAC */
    sprintf(staticMAC,"%d %02x:%02x:%02x:%02x:%02x:%02x",
           vlanId,
           macAddr.addr[0],
           macAddr.addr[1],
           macAddr.addr[2],
           macAddr.addr[3],
           macAddr.addr[4],
           macAddr.addr[5]);


    strcat(staticMACList,staticMAC);
    count++;

    rc = usmDbPmlIntfStaticEntryGetNext(USMDB_UNIT_CURRENT, intIfNum, &macAddr, &vlanId);
    
    if(rc == L7_SUCCESS)
      strcat( staticMACList,",");
  }


  /* If there is no static vlan mac-id combination, staticMACList should be returned
     as null-string. */
  if(rc != L7_SUCCESS)
  {
    if(count == 0)
      strcpy(staticMACList, "");
    rc = L7_SUCCESS;
  }
  return rc;
}


L7_RC_t
snmpAgentPortSecurityLastDiscardedMACGet( L7_uint32 intIfNum, L7_uchar8 *discardedMAC)
{
  L7_RC_t rc = L7_FAILURE;

  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId;
  
  
  rc = usmDbPmlIntfLastViolationAddrGet(USMDB_UNIT_CURRENT,intIfNum,&macAddr, &vlanId);

  /* Storing in string discardedMAC */
  sprintf(discardedMAC,"%d %02x:%02x:%02x:%02x:%02x:%02x",
           vlanId,
           macAddr.addr[0],
           macAddr.addr[1],
           macAddr.addr[2],
           macAddr.addr[3],
           macAddr.addr[4],
           macAddr.addr[5]);

  if(rc != L7_SUCCESS)
  {
    strcpy(discardedMAC,"");
    rc = L7_SUCCESS;
  }
  return rc;
}


L7_BOOL snmpPmlConvertMac(L7_uchar8 *buf, L7_enetMacAddr_t *macAddr)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 mac_address[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];

  if (strlen(buf) != 17)
  {      /* test string length */
    return L7_FALSE;
  }

  for ( i=0,j=0;i<17;i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }

    /* Check digit count to be less than 3. */
    if (digit_count > 2) 
    {
        return L7_FALSE;
    }
  }

  for ( i = 0; i < 6; i++ )
  {
    (*macAddr).addr[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_TRUE;
}



L7_RC_t
snmpAgentPortSecurityMACAddressAdd(L7_uint32 intIfNum, L7_uchar8* VidMACAddress)
{
  L7_RC_t rc = L7_FAILURE;

  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId;

  L7_char8 *pbuf = NULL;

  L7_uchar8 strVID[SNMP_BUFFER_LEN];
  bzero(strVID, SNMP_BUFFER_LEN);

  /* The VLAN ID - MAC Address combination is entered in the following format :
     01 a1:b2:c3:d4:e5:f6 */
  pbuf = strchr (VidMACAddress, ' ');

  /* Check pbuf for NULL */
  if (pbuf == NULL)
  {
      return rc;
  }

  /* Getting VLAN id */
  strncpy (strVID, VidMACAddress, strlen(VidMACAddress) - strlen(pbuf));

  vlanId = atoi (strVID);
  if (vlanId > L7_DOT1Q_MAX_VLAN_ID)
  {
      return rc;
  }

  /* Getting MAC Address */
  if (!snmpPmlConvertMac (pbuf+1, &macAddr))
  {
      return rc;
  }

  rc = usmDbPmlIntfStaticEntryAdd (USMDB_UNIT_CURRENT, intIfNum, macAddr, vlanId);

  return rc;
}

L7_RC_t
snmpAgentPortSecurityMACAddressRemove(L7_uint32 intIfNum, L7_uchar8* VidMACAddress)
{
  L7_RC_t rc = L7_FAILURE;

  L7_enetMacAddr_t macAddr;
  L7_ushort16 vlanId;

  L7_char8 *pbuf = NULL;

  L7_uchar8 strVID[SNMP_BUFFER_LEN];
  bzero(strVID, SNMP_BUFFER_LEN);

  /* The VLAN ID - MAC Address combination is entered in the following format :
     01-a1:b2:c3:d4:e5:f6 */

  pbuf = strchr (VidMACAddress, ' ');

  /* Check pbuf for NULL */
  if (pbuf == NULL)
  {
      return rc;
  }

  strncpy (strVID, VidMACAddress, strlen(VidMACAddress) - strlen(pbuf));

  /* Getting VLAN id */
  vlanId = atoi (strVID);
  if (vlanId > L7_DOT1Q_MAX_VLAN_ID)
  {
      return rc;
  }

  /* Getting MAC Address */
  if (!snmpPmlConvertMac (pbuf+1, &macAddr))
  {
      return rc;
  }

  rc = usmDbPmlIntfStaticEntryDelete(USMDB_UNIT_CURRENT,intIfNum,macAddr,vlanId);

  return rc;
}

L7_RC_t
snmpAgentPortSecurityMACAddressMove(L7_uint32 intfNum, L7_uint32 val)
{
  L7_RC_t rc;
  
  switch (val)
  {
  case D_agentPortSecurityMACAddressMove_enable:
    rc = usmDbPmlIntfDynamicToStaticMove(USMDB_UNIT_CURRENT,intfNum);
    break;

  case D_agentPortSecurityMACAddressMove_disable:
    rc = L7_SUCCESS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentPortSecurityDynamicEntryGet(L7_uint32 UnitIndex, L7_uint32 extIfNum,
                                    L7_uchar8* macAddress, L7_ushort16 vlanId)
{

  L7_uint32 intIfNum;
  L7_RC_t rc = L7_FAILURE;
  L7_enetMacAddr_t temp_macAddr;
  L7_ushort16 temp_vlanId = 0;


  if(L7_FAILURE == snmpAgentPortSecurityEntryGet(UnitIndex,extIfNum))
      return (rc);

  if(L7_FAILURE == usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum))
      return (rc);

  memset(&temp_macAddr, 0, sizeof(temp_macAddr));

  rc = usmDbPmlIntfDynamicEntryGetFirst(USMDB_UNIT_CURRENT, intIfNum, &temp_macAddr, &temp_vlanId);

  while (rc == L7_SUCCESS)
  {
    if((memcmp(temp_macAddr.addr, macAddress, L7_MAC_ADDR_LEN) == 0) && (temp_vlanId == vlanId))
    {
      rc = L7_SUCCESS;
      break;
    }
    rc = usmDbPmlIntfDynamicEntryGetNext(USMDB_UNIT_CURRENT, intIfNum, &temp_macAddr, &vlanId);
   
  }
    
  return (rc);
}


L7_RC_t
snmpAgentPortSecurityDynamicEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *extIfNum,
                                        L7_uchar8* macAddress, L7_uint32 *vlanId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum; 

  L7_ushort16 vlanid16 = (L7_ushort16)(*vlanId);
  
  L7_enetMacAddr_t macAddr;
    
  if(L7_FAILURE == usmDbIntIfNumFromExtIfNum(*extIfNum, &intIfNum))
    return (rc);

  memset(&macAddr, 0, sizeof(macAddr));
  memcpy(macAddr.addr,macAddress,6);

  /* Trying to get next entry for this interface */
  rc = usmDbPmlIntfDynamicEntryGetNext(USMDB_UNIT_CURRENT, intIfNum, &macAddr, &vlanid16);

  /* Trying to get first entry for next interface having port-security configured */
  while (rc != L7_SUCCESS) 
  {
    if(L7_FAILURE == snmpAgentPortSecurityEntryGetNext(USMDB_UNIT_CURRENT,extIfNum))
        return (rc);
    
    if(L7_FAILURE == usmDbIntIfNumFromExtIfNum(*extIfNum, &intIfNum))
        return (rc);

    rc = usmDbPmlIntfDynamicEntryGetFirst(USMDB_UNIT_CURRENT,intIfNum,&macAddr, &vlanid16);

    if(L7_SUCCESS == rc)
        break;
    
  }

  *vlanId = (L7_uint32)vlanid16;

  if(L7_FAILURE == usmDbExtIfNumFromIntIfNum(intIfNum, extIfNum))
    return (rc);

  memcpy(macAddress,macAddr.addr,6); 

  return(rc);
}


/****************************************************************************************/

/* End Function Definitions */


