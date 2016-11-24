/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_lldp_api.c
*
* @purpose SNMP specific value conversion for LLDP MIB
*
* @component LLDP
*
* @comments
*
* @create 04/15/2005
*
* @author ikiran
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#include "usmdb_lldp_api.h"
#include "k_mib_lldp_api.h"

L7_RC_t
snmpLldpIntfAdminModeGet( L7_uint32 intIfNum, L7_int32 *adminStatus)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 rxmode, txmode;

  if(((rc = usmDbLldpIntfTxModeGet(intIfNum, &txmode)) == L7_SUCCESS) && 
     ((rc = usmDbLldpIntfRxModeGet(intIfNum, &rxmode)) == L7_SUCCESS))
  {
    if(rxmode == L7_ENABLE && txmode == L7_ENABLE)
    {
      *adminStatus = D_lldpPortConfigAdminStatus_txAndRx;
    }
    else if(rxmode == L7_DISABLE && txmode == L7_DISABLE)
    {
      *adminStatus = D_lldpPortConfigAdminStatus_disabled;
    }
    else if(rxmode == L7_ENABLE && txmode == L7_DISABLE)
    {
      *adminStatus = D_lldpPortConfigAdminStatus_rxOnly;
    }
    else if(rxmode == L7_DISABLE && txmode == L7_ENABLE)
    {
      *adminStatus = D_lldpPortConfigAdminStatus_txOnly;
    }
  }

  return rc;
}

L7_RC_t
snmpLldpIntfAdminModeSet( L7_uint32 intIfNum, L7_int32 adminStatus)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  switch (adminStatus)
  {
    case D_lldpPortConfigAdminStatus_txOnly:
      mode = L7_DISABLE;
      rc = usmDbLldpIntfRxModeSet(intIfNum, mode);
      if(rc == L7_SUCCESS)
      {
        /* Enable Tx mode */
        mode = L7_ENABLE;
        rc = usmDbLldpIntfTxModeSet(intIfNum, mode);
      }
      break;
    case D_lldpPortConfigAdminStatus_rxOnly:
      /* Disable Tx mode */
      mode = L7_DISABLE;
      rc = usmDbLldpIntfTxModeSet(intIfNum, mode);
      if(rc == L7_SUCCESS)
      {
        /* Enable Rx mode */
        mode = L7_ENABLE;
        rc = usmDbLldpIntfRxModeSet(intIfNum, mode);
      }
      break;
    case D_lldpPortConfigAdminStatus_txAndRx:
      mode = L7_ENABLE;
      rc = usmDbLldpIntfTxModeSet(intIfNum, mode);
      if(rc == L7_SUCCESS)
        rc = usmDbLldpIntfRxModeSet(intIfNum, mode);
      break;
    case D_lldpPortConfigAdminStatus_disabled:
      mode = L7_DISABLE;
      rc = usmDbLldpIntfTxModeSet(intIfNum, mode);
      if(rc == L7_SUCCESS)
        rc = usmDbLldpIntfRxModeSet(intIfNum, mode);
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

L7_RC_t
snmpLldpIntfNoticationEnableGet( L7_uint32 intIfNum, 
                                 L7_int32 *notification_enable)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;
 
  rc = usmDbLldpIntfNotificationModeGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *notification_enable = D_lldpPortConfigNotificationEnable_true;
      break;
    case L7_DISABLE:
      *notification_enable = D_lldpPortConfigNotificationEnable_false;
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
snmpLldpIntfNoticationEnableSet( L7_uint32 intIfNum, 
                                 L7_int32 notification_enable)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (notification_enable)
  {
  case D_lldpPortConfigNotificationEnable_true:
    temp_val = L7_ENABLE;
    break;
  case D_lldpPortConfigNotificationEnable_false:
    temp_val = L7_DISABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if(rc == L7_SUCCESS)
    rc = usmDbLldpIntfNotificationModeSet(intIfNum, temp_val);
  return rc;
}

L7_RC_t
snmpLldpIntfTLVsTxEnableGet( L7_uint32 intIfNum, 
                             L7_uchar8 *tlvstxenable)
{
  L7_RC_t rc;
  L7_BOOL portDesc, sysName, sysDesc, sysCap;
                                                                                

  rc = usmDbLldpIntfTxTLVsGet(intIfNum, &portDesc, &sysName, &sysDesc, &sysCap); 
  if (rc == L7_SUCCESS)   
  {
    /* zero out bits in the bitmask */
    if(portDesc == L7_TRUE)
    {
      *tlvstxenable = (0x80 >> D_lldpPortConfigTLVsTxEnable_portDesc);
    }
                                                                                
    if (sysName == L7_TRUE)
    {
      *tlvstxenable |= (0x80 >> D_lldpPortConfigTLVsTxEnable_sysName);     
    }
                                                                                
    if (sysDesc == L7_TRUE)
    {
      *tlvstxenable |= (0x80 >> D_lldpPortConfigTLVsTxEnable_sysDesc);
    }
                                                                                
    if (sysCap == L7_TRUE)
    {
      *tlvstxenable |= (0x80 >> D_lldpPortConfigTLVsTxEnable_sysCap);
    }
  }
                                                                                
  return rc;
}

L7_RC_t
snmpLldpIntfTLVsTxEnableSet( L7_uint32 intIfNum, 
                             L7_uchar8 *tlvstxenable)
{
  L7_RC_t rc;
  L7_BOOL portDesc, sysName, sysDesc, sysCap;


  portDesc = L7_FALSE;
  sysName  = L7_FALSE;
  sysDesc  = L7_FALSE;
  sysCap   = L7_FALSE;

  if (tlvstxenable[0] & 0x80)
  {
    portDesc = L7_TRUE;
  }

  if (tlvstxenable[0] & 0x40)
  {
    sysName = L7_TRUE;
  }

  if (tlvstxenable[0] & 0x20)
  {
    sysDesc = L7_TRUE;
  }

  if (tlvstxenable[0] & 0x10)
  {
    sysCap = L7_TRUE;
  }

  rc = usmDbLldpIntfTxTLVsSet(intIfNum, portDesc, sysName, sysDesc, sysCap); 
                                                                                
  return rc;
}

L7_RC_t
snmpLldpAddrFamilyGet(L7_int32  subtype, 
                       lldpIANAAddrFamilyNumber_t *addr_family)
{
  L7_RC_t rc = L7_SUCCESS;
 
  switch(subtype)
  {
  case D_lldpLocManAddrSubtype_other:
   /**addr_family = LLDP_IANA_ADDR_FAMILY_NUMBER_OTHER;*/
   *addr_family = LLDP_IANA_ADDR_FAMILY_NUMBER_RESERVED;
   break;

  case D_lldpLocManAddrSubtype_ipV4:
   *addr_family = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4;
   break;

  case D_lldpLocManAddrSubtype_ipV6:
   *addr_family = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6;
   break;

  case D_lldpLocManAddrSubtype_all802:
   *addr_family = LLDP_IANA_ADDR_FAMILY_NUMBER_802;
   break;

   default:
    rc = L7_FAILURE;
   break;
  }

  return rc;
}

L7_RC_t
snmpLldpAddrSubTypeGet(lldpIANAAddrFamilyNumber_t addr_family,
                      L7_int32  *subtype)
{
  L7_RC_t rc = L7_SUCCESS;
 
  switch(addr_family)
  {
  /*case LLDP_IANA_ADDR_FAMILY_NUMBER_OTHER:
   *subtype = D_lldpLocManAddrSubtype_other;
   break; */

  case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4:
   *subtype = D_lldpLocManAddrSubtype_ipV4;
   break;

  case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6:
   *subtype = D_lldpLocManAddrSubtype_ipV6;
   break;

  case LLDP_IANA_ADDR_FAMILY_NUMBER_802:
   *subtype = D_lldpLocManAddrSubtype_all802;
   break;

   default:
    rc = L7_FAILURE;
   break;
  }
  return rc;
}

L7_RC_t
snmpLldpLocManAddrGet(L7_int32 lldpLocManAddrSubtype,
                      L7_uchar8 *address, L7_int32 *lldpLocManAddrLen)
{
  lldpIANAAddrFamilyNumber_t subtype;
  lldpIANAAddrFamilyNumber_t family;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 length[SNMP_BUFFER_LEN];

  if (snmpLldpAddrFamilyGet(lldpLocManAddrSubtype, &subtype) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memset(length, 0, SNMP_BUFFER_LEN);

  if (usmDbLldpLocManAddrGet(&family, snmp_buffer, length) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if ((family == subtype) && (memcmp(snmp_buffer, address, length[0]) == 0))
  {
    *lldpLocManAddrLen = length[0];
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpLldpLocManAddrGetNext(L7_int32 *lldpLocManAddrSubtype,
                          L7_uchar8 *address, L7_int32 *lldpLocManAddrLen)
{
  lldpIANAAddrFamilyNumber_t subtype;
  lldpIANAAddrFamilyNumber_t family;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 length[SNMP_BUFFER_LEN];

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memset(length, 0, SNMP_BUFFER_LEN);

  if(*lldpLocManAddrSubtype == 0)
  {
    *lldpLocManAddrSubtype = D_lldpLocManAddrSubtype_other;
  }

  if (snmpLldpAddrFamilyGet(*lldpLocManAddrSubtype, &subtype) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (usmDbLldpLocManAddrGet(&family, snmp_buffer, length) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (family >= subtype)
  {
    if ((family == subtype) && (memcmp(snmp_buffer, address, length[0]) < 0))
    {
      return L7_FAILURE;
    }
    if(snmpLldpAddrSubTypeGet(family, lldpLocManAddrSubtype) != L7_SUCCESS)
      return L7_FAILURE;
    memcpy(address, snmp_buffer, SNMP_BUFFER_LEN);
    *lldpLocManAddrLen = length[0];
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpLldpNextExtIfNumber(L7_uint32 UnitIndex, L7_uint32 *extIfNum)
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
      if (usmDbLldpIsValidIntf(intIfNum)  ==  L7_TRUE)
      {
        rc = L7_SUCCESS;
      }
      else
      {
        if (usmDbLldpValidIntfNextGet(intIfNum,&intIfNum) == L7_SUCCESS)
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

L7_RC_t
snmpLldpRemEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 lldpRemLocalPortNum,
                    L7_int32 lldpRemIndex)
{
  if(usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                   lldpRemLocalPortNum) != L7_SUCCESS)
    return L7_FAILURE;

  return usmDbLldpRemTimestampIndexEntryGet(lldpRemTimeMark,lldpRemLocalPortNum, lldpRemIndex);
}

L7_RC_t
snmpLldpRemEntryGetNext(L7_uint32 *lldpRemTimeMark, L7_int32 *lldpRemLocalPortNum,
                        L7_int32 *lldpRemIndex)
{
  return usmDbLldpRemTimestampIndexEntryGetNext(lldpRemTimeMark,lldpRemLocalPortNum,lldpRemIndex);
}

L7_RC_t
snmpLldpRemManAddrEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfNum,
                    L7_int32 lldpRemIndex, 
                    lldpIANAAddrFamilyNumber_t lldpRemManAddrSubtype,
                    L7_uchar8 *lldpRemManAddr, L7_int32 length)
{
  L7_uchar8 len = 0;

  len = length;
  
  if(usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                   intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  if(snmpLldpRemEntryGet(lldpRemTimeMark,intIfNum, lldpRemIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if(usmDbLldpRemManAddrEntryGet(intIfNum, lldpRemIndex, lldpRemTimeMark,
                                 lldpRemManAddrSubtype, 
                                 lldpRemManAddr, len) != L7_SUCCESS)
    return L7_FAILURE;
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpRemManAddrEntryGetNext(L7_uint32 *lldpRemTimeMark, L7_int32 *intIfNum,
                              L7_int32 *lldpRemIndex, 
                              lldpIANAAddrFamilyNumber_t *lldpRemManAddrSubtype,
                              L7_uchar8 *lldpRemManAddr, L7_int32 *length)
{
  L7_uchar8 len;

  len = *length;
  if(snmpLldpRemEntryGet(*lldpRemTimeMark,*intIfNum, *lldpRemIndex) == L7_SUCCESS)
  {
    if(usmDbLldpRemManAddrEntryNextGet(*intIfNum, *lldpRemIndex, 
                                       *lldpRemTimeMark, lldpRemManAddrSubtype,
                                       lldpRemManAddr, &len) == L7_SUCCESS)
    {
      *length = len;
      return L7_SUCCESS;
    }
  }
  while(snmpLldpRemEntryGetNext(lldpRemTimeMark, intIfNum, 
                                lldpRemIndex) == L7_SUCCESS)
  {
    *lldpRemManAddrSubtype = LLDP_IANA_ADDR_FAMILY_NUMBER_RESERVED;
    len = 0;
    if(usmDbLldpRemManAddrEntryNextGet(*intIfNum, *lldpRemIndex,
                                       *lldpRemTimeMark, lldpRemManAddrSubtype,
                                       lldpRemManAddr, &len ) == L7_SUCCESS)
    {
      *length = len;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpLldpRemUnknownTLVEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfNum,
                              L7_int32 lldpRemIndex, 
                              L7_int32 lldpRemUnknownTLVType)
{
  
  if(usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                   intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  if(snmpLldpRemEntryGet(lldpRemTimeMark, intIfNum, lldpRemIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if(usmDbLldpRemUnknownTLVEntryGet(intIfNum, lldpRemIndex, lldpRemTimeMark,
                                    lldpRemUnknownTLVType)!= L7_SUCCESS)
    return L7_FAILURE;
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpRemUnknownTLVEntryGetNext(L7_uint32 *lldpRemTimeMark, 
                                  L7_int32 *intIfNum, L7_int32 *lldpRemIndex,
                                  L7_int32 *lldpRemUnknownTLVType)
{
  if(snmpLldpRemEntryGet(*lldpRemTimeMark, *intIfNum, *lldpRemIndex) == L7_SUCCESS)
  {
    if(usmDbLldpRemUnknownTLVEntryNextGet(*intIfNum, *lldpRemIndex, 
                                          *lldpRemTimeMark, 
                                          lldpRemUnknownTLVType) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  while(snmpLldpRemEntryGetNext(lldpRemTimeMark, intIfNum, 
                                lldpRemIndex) == L7_SUCCESS)
  {
    *lldpRemUnknownTLVType = 0;
    if(usmDbLldpRemUnknownTLVEntryNextGet(*intIfNum, *lldpRemIndex,
                                          *lldpRemTimeMark, 
                                          lldpRemUnknownTLVType) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpLldpRemOrgDefInfoEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfNum,
                              L7_int32 lldpRemIndex, 
                              L7_int32 lldpRemOrgDefInfoIndex,
                              L7_uchar8* oui,
                              L7_uint32* subtype,
                              L7_uchar8* info,
                              L7_uint32* length)
{
  
  if(usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                   intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  if(snmpLldpRemEntryGet(lldpRemTimeMark, intIfNum, lldpRemIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if(usmdbLldpRemOrgDefEntryInfoGet(intIfNum, lldpRemIndex, lldpRemTimeMark,
                                    lldpRemOrgDefInfoIndex, oui, subtype, info, length)!= L7_SUCCESS)
    return L7_FAILURE;
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpRemOrgDefInfoEntryGetNext(L7_uint32 *lldpRemTimeMark, 
                                  L7_int32 *intIfNum, L7_int32 *lldpRemIndex,
                                  L7_int32 *lldpRemOrgDefInfoIndex,L7_uchar8* oui,
                                  L7_uint32* subtype,
                                  L7_uchar8* info,
                                  L7_uint32* length)
{
  if(snmpLldpRemEntryGet(*lldpRemTimeMark, *intIfNum, *lldpRemIndex) == L7_SUCCESS)
  {
    if(usmdbLldpRemOrgDefEntryInfoGetNext(*intIfNum, *lldpRemIndex, 
                                          *lldpRemTimeMark, 
                                          lldpRemOrgDefInfoIndex,
                                          oui,
                                          subtype,
                                          info,
                                          length) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  while(snmpLldpRemEntryGetNext(lldpRemTimeMark, intIfNum, 
                                lldpRemIndex) == L7_SUCCESS)
  {
    *lldpRemOrgDefInfoIndex = 0;
    if(usmdbLldpRemOrgDefEntryInfoGetNext(*intIfNum, *lldpRemIndex,
                                          *lldpRemTimeMark, 
                                          lldpRemOrgDefInfoIndex,
                                          oui,
                                          subtype,
                                          info,
                                          length) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_ushort16 snmpSysCapReverse(L7_ushort16 org)
{
  int i = 0;
  L7_ushort16 new = 0;
  for (i = 0; i < (sizeof(org)*8);i++)
  {
    if((org & (0x8000 >> i)) != 0)
      new |= (0x0001 << i);
  }
  return new;
} 

L7_RC_t
snmpLldpConfigManAddrPortsTxEnableGet(L7_uchar8 *portList, L7_uint32 *length)
{
  L7_INTF_MASK_t temp_val;
  L7_uint32 k, j;
  L7_BOOL mgt_mode;

  memset(temp_val.value, 0, L7_INTF_INDICES);
  for(k = 0; k < L7_INTF_INDICES; k++)
  {
    /* interfaces 1 to 8 within a byte */
    for (j = 1; j <= 8; j++)
    {
      if((usmDbLldpIntfTxMgmtAddrGet(8*k + j, &mgt_mode) == L7_SUCCESS) &&
         (mgt_mode == L7_TRUE))
      {
        L7_INTF_SETMASKBIT(temp_val, 8*k + j);
      }
    }
  }
  L7_INTF_MASKREV(temp_val);
  *length = L7_INTF_INDICES;
  memcpy(portList, temp_val.value, L7_INTF_INDICES);
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpConfigManAddrPortsTxEnableSet(L7_uchar8 *buf, L7_uint32 buf_len )
{
  L7_INTF_MASK_t temp_val;
  L7_uint32 k, j;
  L7_BOOL mgt_mode;

  if (buf_len > L7_INTF_INDICES)
    return L7_FAILURE;

  memset(temp_val.value, 0, L7_INTF_INDICES);
  memcpy(temp_val.value, buf, buf_len);

  /* reverse the bits from the SNMP mode to internal mode */
  L7_INTF_MASKREV(temp_val);

  for (k = 0; k < L7_INTF_INDICES && k < buf_len; k++)
  {
    /* interfaces 1 to 8 within a byte */
    for (j = 1; j <= 8; j++)
    {
      if(usmDbLldpIntfTxMgmtAddrGet(8*k + j, &mgt_mode) == L7_SUCCESS)
      {
        if((L7_INTF_ISMASKBITSET(temp_val, 8*k + j) == 0) &&
           (mgt_mode == L7_TRUE))
        {
          if(usmDbLldpIntfTxMgmtAddrSet(8*k + j, L7_FALSE) != L7_SUCCESS)
            return L7_FAILURE;
        }
        else if((L7_INTF_ISMASKBITSET(temp_val, 8*k + j) != 0) &&
                (mgt_mode == L7_FALSE))
        {
          if(usmDbLldpIntfTxMgmtAddrSet(8*k + j, L7_TRUE) != L7_SUCCESS)
            return L7_FAILURE;
        }
      }
      else
      {
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXdot3LocPortAutoNegSupportedGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXdot3LocPortAutoNegSupportedGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXdot3LocPortAutoNegSupported_true;
      break;
    case L7_FALSE:
      *val = D_lldpXdot3LocPortAutoNegSupported_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXdot3LocPortAutoNegEnabledGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXdot3LocPortAutoNegEnabledGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXdot3LocPortAutoNegEnabled_true;
      break;
    case L7_FALSE:
      *val = D_lldpXdot3LocPortAutoNegEnabled_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXdot3RemPortAutoNegSupportedGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXdot3RemPortAutoNegSupportedGet(intIfNum, lldpRemIndex, lldpRemTimeMark, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXdot3RemPortAutoNegSupported_true;
      break;
    case L7_FALSE:
      *val = D_lldpXdot3RemPortAutoNegSupported_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* unknown, assume false */
    *val = D_lldpXdot3RemPortAutoNegSupported_false;
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpLldpXdot3RemPortAutoNegEnabledGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXdot3RemPortAutoNegEnabledGet(intIfNum, lldpRemIndex, lldpRemTimeMark, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXdot3LocPortAutoNegEnabled_true;
      break;
    case L7_FALSE:
      *val = D_lldpXdot3LocPortAutoNegEnabled_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  else
  {
    /* unknown, assume false */
    *val = D_lldpXdot3LocPortAutoNegEnabled_false;
    rc = L7_SUCCESS;
  }


  return rc;
}

L7_RC_t
snmpLldpXdot3RemPortAutoNegAdvertisedCapGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_uchar8 temp_buf[2];
  L7_uint32 i, j;

  rc = usmDbLldpXdot3RemPortAutoNegAdvertizedCapGet(intIfNum, lldpRemIndex, lldpRemTimeMark, temp_buf);

  if (rc == L7_SUCCESS)
  {
    /* swap bits for SNMP */
    for (i=0; i>= 1; i--)
    {
      for (j=0; j<8; j++)
      {
        if (temp_buf[i] & (0x1 << j))
        {
          buf[1-i] |= (0x80 >> j);
        }
      }
    }
  }
  else
  {
    /* unknown */
    buf[0] = 0x80;
    buf[1] = 0x00;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXdot3RemPortOperMauTypeGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uint32 *val)
{
  if (usmDbLldpXdot3RemPortAutoNegOperMauTypeGet(intIfNum, lldpRemIndex, lldpRemTimeMark, val) != L7_SUCCESS)
  {
    /* unknown */
    *val = 0;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedPortCapSupportedGet(L7_uint32 intIfNum, L7_uchar8 *buf)
{
  L7_RC_t rc;
  lldpXMedCapabilities_t temp_val;

  rc = usmDbLldpXMedPortCapSupportedGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (temp_val.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortCapSupported_capabilities);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortCapSupported_networkPolicy);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortCapSupported_location);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortCapSupported_extendedPSE);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortCapSupported_extendedPD);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortCapSupported_inventory);
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXMedPortConfigTLVsTxEnableGet(L7_uint32 intIfNum, L7_uchar8 *buf)
{
  L7_RC_t rc;
  lldpXMedCapabilities_t temp_val;

  rc = usmDbLldpXMedPortConfigTLVsEnabledGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (temp_val.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_capabilities);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_networkPolicy);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_location);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_extendedPSE);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_extendedPD);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_inventory);
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXMedPortConfigTLVsTxEnableSet(L7_uint32 intIfNum, L7_uchar8 *buf)
{
  lldpXMedCapabilities_t temp_val;

  memset(temp_val.bitmap, 0, sizeof(temp_val.bitmap));
  if (buf[0] & (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_capabilities))
  {
    temp_val.bitmap[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
    buf[0] &= ~(0x80 >> D_lldpXMedPortConfigTLVsTxEnable_capabilities);
  }
  if (buf[0] & (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_networkPolicy))
  {
    temp_val.bitmap[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
    buf[0] &= ~(0x80 >> D_lldpXMedPortConfigTLVsTxEnable_networkPolicy);
  }
  if (buf[0] & (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_location))
  {
    temp_val.bitmap[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
    buf[0] &= ~(0x80 >> D_lldpXMedPortConfigTLVsTxEnable_location);
  }
  if (buf[0] & (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_extendedPSE))
  {
    temp_val.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
    buf[0] &= ~(0x80 >> D_lldpXMedPortConfigTLVsTxEnable_extendedPSE);
  }
  if (buf[0] & (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_extendedPD))
  {
    temp_val.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
    buf[0] &= ~(0x80 >> D_lldpXMedPortConfigTLVsTxEnable_extendedPD);
  }
  if (buf[0] & (0x80 >> D_lldpXMedPortConfigTLVsTxEnable_inventory))
  {
    temp_val.bitmap[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
    buf[0] &= ~(0x80 >> D_lldpXMedPortConfigTLVsTxEnable_inventory);
  }

  /* check that no other bits were turned on */
  if (buf[0] != 0x00)
    return L7_FAILURE;

  return usmDbLldpXMedPortConfigTLVsEnabledSet(intIfNum, &temp_val);
}

L7_RC_t
snmpLldpXMedPortConfigNotifEnableGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXMedPortConfigNotifEnableGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXMedPortConfigNotifEnable_true;
      break;
    case L7_FALSE:
      *val = D_lldpXMedPortConfigNotifEnable_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedPortConfigNotifEnableSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;

  switch (val)
  {
  case D_lldpXMedPortConfigNotifEnable_true:
    temp_val = L7_TRUE;
    break;
  case D_lldpXMedPortConfigNotifEnable_false:
    temp_val = L7_FALSE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbLldpXMedPortConfigNotifEnableSet(intIfNum, temp_val);
  }
  return rc;
}

L7_RC_t
snmpLldpXMedLocDeviceClassGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedLocDeviceClassGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case notDefined:
      *val = D_lldpXMedLocDeviceClass_notDefined;
      break;
    case endpointClass1:
      *val = D_lldpXMedLocDeviceClass_endpointClass1;
      break;
    case endpointClass2:
      *val = D_lldpXMedLocDeviceClass_endpointClass2;
      break;
    case endpointClass3:
      *val = D_lldpXMedLocDeviceClass_endpointClass3;
      break;
    case networkConnectivity:
      *val = D_lldpXMedLocDeviceClass_networkConnectivity;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedLocXPoeDeviceTypeGet(L7_uint32 *val)
{
#if 0
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedLocXPoeDeviceTypeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_poedevicetype:
      *val = D_lldpXMedLocXPoEDeviceType_unknown;
      break;
    case pseDevice:
      *val = D_lldpXMedLocXPoEDeviceType_pseDevice;
      break;
    case pdDevice:
      *val = D_lldpXMedLocXPoEDeviceType_pdDevice;
      break;
    case none_poedevicetype:
      *val = D_lldpXMedLocXPoEDeviceType_none;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
#endif
  *val = D_lldpXMedLocXPoEDeviceType_unknown;
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocXPoEPDPowerSourceGet(L7_uint32 *val)
{
#if 0
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedLocXPoePDPowerSouceGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_pdsrc:
      *val = D_lldpXMedLocXPoEPDPowerSource_unknown;
      break;
    case fromPSE_pdsrc:
      *val = D_lldpXMedLocXPoEPDPowerSource_fromPSE;
      break;
    case local_pdsrc:
      *val = D_lldpXMedLocXPoEPDPowerSource_local;
      break;
    case localAndPSE_pdsrc:
      *val = D_lldpXMedLocXPoEPDPowerSource_localAndPSE;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
#endif 
  *val = D_lldpXMedLocXPoEPDPowerSource_unknown;
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocXPoEPSEPowerSourceGet(L7_uint32 *val)
{
#if 0
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedLocXPoePSEPowerSourceGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_psesrc:
      *val = D_lldpXMedLocXPoEPSEPowerSource_unknown;
      break;
    case primary_psesrc:
      *val = D_lldpXMedLocXPoEPSEPowerSource_primary;
      break;
    case backup_psesrc:
      *val = D_lldpXMedLocXPoEPSEPowerSource_backup;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
#endif
  *val = D_lldpXMedLocXPoEPSEPowerSource_unknown;
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocXPoEPDPowerPriorityGet(L7_uint32 *val)
{
#if 0
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedLocXPoePDPowerPriorityGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_pwpri:
      *val = D_lldpXMedLocXPoEPDPowerPriority_unknown;
      break;
    case critical_pwpri:
      *val = D_lldpXMedLocXPoEPDPowerPriority_critical;
      break;
    case high_pwpri:
      *val = D_lldpXMedLocXPoEPDPowerPriority_high;
      break;
    case low_pwpri:
      *val = D_lldpXMedLocXPoEPDPowerPriority_low;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
#endif
  *val = D_lldpXMedLocXPoEPDPowerPriority_unknown;
  return L7_SUCCESS;
}


L7_RC_t
snmpLldpXMedLocMediaPolicyAppTypeNextGet(L7_uchar8 *buf, L7_uint32 *buf_len)
{
  if (*buf_len < 1)
  {
    buf[0] = 0x01;
    *buf_len = 1;
  }
  else if (*buf_len == 1)
  {
    if (buf[0] == 0x00)
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_streamingVideo));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_streamingVideo)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoconferencing)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoconferencing));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoconferencing)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_softPhoneVoice)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_softPhoneVoice));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_softPhoneVoice)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoiceSignaling)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoiceSignaling));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoiceSignaling)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoice)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoice));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoice)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voiceSignaling)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voiceSignaling));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voiceSignaling)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voice)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voice));
    }
    else if (buf[0] >= (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voice)) &&
             buf[0] <  (0x80 >> (D_lldpXMedLocMediaPolicyAppType_unknown)))
    {
      buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_unknown));
    }
    else
    {
      buf[0] = 0x00;
      buf[1] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoSignaling-8));
      *buf_len = 2;
    }
  }
  else if (*buf_len == 2)
  {
    if (buf[0] == 0x00 && buf[1] == 0x00)
    {
      buf[1] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoSignaling-8));
    }
    else
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(L7_uchar8 *buf, L7_uint32 buf_len, lldpXMedPolicyAppTypeValue_t *appType)
{
  if (buf_len == 1)
  {
    if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_unknown)))
    {
      *appType = unknown_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voice)))
    {
      *appType = voice_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voiceSignaling)))
    {
      *appType = voiceSignaling_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoice)))
    {
      *appType = guestVoice_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoiceSignaling)))
    {
      *appType = guestVoiceSignaling_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_softPhoneVoice)))
    {
      *appType = softPhoneVoice_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoconferencing)))
    {
      *appType = videoConferencing_policyapptype;
    }
    else if (buf[0] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_streamingVideo)))
    {
      *appType = streamingVideo_policyapptype;
    }
    else
    {
      return L7_FAILURE;
    }
  }
  /* bit 9 is high bit on second octet */
  else if (buf[0] == 0x00 && buf[1] == (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoSignaling-8)))
  {
    *appType = videoSignaling_policyapptype;
  }
  else
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocMediaPolicyAppTypeToBitmask(lldpXMedPolicyAppTypeValue_t val, L7_uchar8 *buf, L7_uint32 *buf_len)
{
  switch (val)
  {
  case unknown_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_unknown));
    *buf_len = 1;
    break;
  case voice_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voice));
    *buf_len = 1;
    break;
  case voiceSignaling_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_voiceSignaling));
    *buf_len = 1;
    break;
  case guestVoice_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoice));
    *buf_len = 1;
    break;
  case guestVoiceSignaling_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_guestVoiceSignaling));
    *buf_len = 1;
    break;
  case softPhoneVoice_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_softPhoneVoice));
    *buf_len = 1;
    break;
  case videoConferencing_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoconferencing));
    *buf_len = 1;
    break;
  case streamingVideo_policyapptype:
    buf[0] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_streamingVideo));
    *buf_len = 1;
    break;
  case videoSignaling_policyapptype:
    buf[0] = 0x00;
    buf[1] = (0x80 >> (D_lldpXMedLocMediaPolicyAppType_videoSignaling-sizeof(L7_uchar8)));
    *buf_len = 2;
    break;
  default:
    /* unknown value */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


L7_RC_t
snmpLldpXMedLocMediaPolicyEntryIndexNextGet(L7_uint32 *intIfNum, L7_uchar8 *buf, L7_uint32 *buf_len)
{
  lldpXMedPolicyAppTypeValue_t appType = D_lldpXMedLocMediaPolicyAppType_unknown;
  
  do
  {
    /* iterate through supported app types */
    while (snmpLldpXMedLocMediaPolicyAppTypeNextGet(buf, buf_len) == L7_SUCCESS)
    {
      if (snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(buf, *buf_len, &appType) != L7_SUCCESS)
      {
        break;
      }
      if (usmDbLldpXMedLocMediaPolicyAppSupportedGet(*intIfNum, appType) == L7_TRUE)
      {
        return L7_SUCCESS;
      }
    } 
    /* no matches, go to the next interface */
    /* reset to first bit */
    buf[0] = 0x00;
    buf[1] = 0x00;
    *buf_len = 0;
  } while (usmDbGetNextPhysicalIntIfNumber(*intIfNum, intIfNum) == L7_SUCCESS);

  return L7_FAILURE;
}

L7_RC_t snmpLldpXMedLocMediaPolicyEntryGet(lldpXMedLocMediaPolicyEntry_t *lldpXMedLocMediaPolicyEntryData,
                       int nominator)
{
  L7_uint32 intIfNum;
  lldpXMedPolicyAppTypeValue_t appType = videoSignaling_policyapptype;
  L7_uchar8 temp_buf[2];
  L7_uint32 temp_buf_len;

  if (usmDbIntIfNumFromExtIfNum(lldpXMedLocMediaPolicyEntryData->lldpLocPortNum,
                &intIfNum) != L7_SUCCESS)
  return(L7_FAILURE);

  memcpy(temp_buf, lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType->octet_ptr, lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType->length);
  temp_buf_len = lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType->length;

  snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(temp_buf, temp_buf_len, &appType);

  switch (nominator)
  {
  case -1:
  /* fallthrough all cases except default */
  case I_lldpXMedLocMediaPolicyEntryIndex_lldpLocPortNum:
  case I_lldpXMedLocMediaPolicyAppType:
  if (nominator != -1) break;
  case I_lldpXMedLocMediaPolicyVlanID:
  if (usmDbLldpXMedLocMediaPolicyVlanIDGet(intIfNum, appType, 
                       &lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyVlanID) == L7_SUCCESS)
    SET_VALID(I_lldpXMedLocMediaPolicyVlanID, lldpXMedLocMediaPolicyEntryData->valid);
  if (nominator != -1) break;
  case I_lldpXMedLocMediaPolicyPriority:
  if (usmDbLldpXMedLocMediaPolicyPriorityGet(intIfNum, appType, 
                         &lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyPriority) == L7_SUCCESS)
    SET_VALID(I_lldpXMedLocMediaPolicyPriority, lldpXMedLocMediaPolicyEntryData->valid);
  if (nominator != -1) break;
  case I_lldpXMedLocMediaPolicyDscp:
  if (usmDbLldpXMedLocMediaPolicyDscpGet(intIfNum, appType, 
                       &lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyDscp) == L7_SUCCESS)
    SET_VALID(I_lldpXMedLocMediaPolicyDscp, lldpXMedLocMediaPolicyEntryData->valid);
  if (nominator != -1) break;
  case I_lldpXMedLocMediaPolicyUnknown:
  if (snmpLldpXMedLocMediaPolicyUnknownGet(intIfNum, appType, 
                        &lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyUnknown) == L7_SUCCESS)
    SET_VALID(I_lldpXMedLocMediaPolicyUnknown, lldpXMedLocMediaPolicyEntryData->valid);
  if (nominator != -1) break;
  case I_lldpXMedLocMediaPolicyTagged:
  if (snmpLldpXMedLocMediaPolicyTaggedGet(intIfNum, appType, 
                       &lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyTagged) == L7_SUCCESS)
    SET_VALID(I_lldpXMedLocMediaPolicyTagged, lldpXMedLocMediaPolicyEntryData->valid);
  break;
  default:
  /* unknown nominator */
  return(L7_FAILURE);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedLocMediaPolicyEntryData->valid))
  return(L7_FAILURE);

  return(L7_SUCCESS);
}

L7_RC_t snmpLldpXMedLocMediaPolicyEntryNextGet(lldpXMedLocMediaPolicyEntry_t *lldpXMedLocMediaPolicyEntryData,
                           int nominator)
{                                           
  L7_uint32 intIfNum;
  lldpXMedPolicyAppTypeValue_t appType = videoSignaling_policyapptype;
  L7_uchar8 temp_buf[2];
  L7_uint32 temp_buf_len;

  if (usmDbIntIfNumFromExtIfNum(lldpXMedLocMediaPolicyEntryData->lldpLocPortNum,
                &intIfNum) != L7_SUCCESS)
  return(L7_FAILURE);

  memcpy(temp_buf, lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType->octet_ptr, lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType->length);
  temp_buf_len = lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType->length;

  snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(temp_buf, temp_buf_len, &appType);

  while (snmpLldpXMedLocMediaPolicyEntryIndexNextGet(&intIfNum, temp_buf, &temp_buf_len) == L7_SUCCESS)
  {
  snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(temp_buf, temp_buf_len, &appType);

  if ((SafeMakeOctetString(&lldpXMedLocMediaPolicyEntryData->lldpXMedLocMediaPolicyAppType,
               temp_buf, temp_buf_len) != L7_TRUE) ||
    usmDbExtIfNumFromIntIfNum(intIfNum, 
                  &lldpXMedLocMediaPolicyEntryData->lldpLocPortNum) != L7_SUCCESS)
    return(L7_FAILURE);

  if (snmpLldpXMedLocMediaPolicyEntryGet(lldpXMedLocMediaPolicyEntryData, nominator) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpLldpXMedLocMediaPolicyUnknownGet(L7_uint32 intIfNum, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXMedLocMediaPolicyUnknownGet(intIfNum, appType, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXMedLocMediaPolicyUnknown_true;
      break;
    case L7_FALSE:
      *val = D_lldpXMedLocMediaPolicyUnknown_false;
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
snmpLldpXMedLocMediaPolicyTaggedGet(L7_uint32 intIfNum, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXMedLocMediaPolicyTaggedGet(intIfNum, appType, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXMedLocMediaPolicyTagged_true;
      break;
    case L7_FALSE:
      *val = D_lldpXMedLocMediaPolicyTagged_false;
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
snmpLldpXMedLocLocationSubTypeGet(L7_uint32 val, L7_uint32 *subType)
{
  switch(val)
  {
  case D_lldpXMedLocLocationSubtype_unknown:
    *subType = unknown_locsubtype;
    break;
  case D_lldpXMedLocLocationSubtype_coordinateBased:
    *subType = coordinateBased_locsubtype;
    break;
  case D_lldpXMedLocLocationSubtype_civicAddress:
    *subType = civicAddress_locsubtype;
    break;
  case D_lldpXMedLocLocationSubtype_elin:
    *subType = elin_locsubtype;
    break;
  default:
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocLocationEntryGet(L7_uint32 intIfNum, L7_uint32 subType, L7_uchar8 *buf)
{
  L7_uint32 temp_subType;

  if(usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS ||
     snmpLldpXMedLocLocationSubTypeGet(subType, &temp_subType) != L7_SUCCESS ||
     usmDbLldpXMedLocLocationInfoGet(intIfNum, temp_subType, buf) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedLocLocationEntryNextGet(L7_uint32 *intIfNum, L7_uint32 *subType, L7_uchar8 *buf)
{
  do
  {
    while (*subType <= D_lldpXMedLocLocationSubtype_elin)
    {
      if (snmpLldpXMedLocLocationEntryGet(*intIfNum, *subType, buf) == L7_SUCCESS)
        return L7_SUCCESS;
      (*subType)++;
    }
    *subType = D_lldpXMedLocLocationSubtype_unknown;
  } while (usmDbGetNextPhysicalIntIfNumber(*intIfNum, intIfNum) == L7_SUCCESS);

  return L7_FAILURE;
}

L7_RC_t
snmpLldpXMedLocXPoePSEPortPDPriorityGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedLocXPoePSEPortPDPriorityGet(intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_pwpri:
      *val = D_lldpXMedLocXPoEPSEPortPDPriority_unknown;
      break;
    case critical_pwpri:
      *val = D_lldpXMedLocXPoEPSEPortPDPriority_critical;
      break;
    case high_pwpri:
      *val = D_lldpXMedLocXPoEPSEPortPDPriority_high;
      break;
    case low_pwpri:
      *val = D_lldpXMedLocXPoEPSEPortPDPriority_low;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXMedRemCapSupportedGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uchar8 *buf)
{
  L7_RC_t rc;
  lldpXMedCapabilities_t temp_val;

  rc = usmDbLldpXMedRemCapSupportedGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (temp_val.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapSupported_capabilities);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapSupported_networkPolicy);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapSupported_location);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapSupported_extendedPSE);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapSupported_extendedPD);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapSupported_inventory);
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXMedRemCapCurrentGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uchar8 *buf)
{
  L7_RC_t rc;
  lldpXMedCapabilities_t temp_val;

  rc = usmDbLldpXMedRemCapCurrentGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (temp_val.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapCurrent_capabilities);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapCurrent_networkPolicy);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapCurrent_location);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapCurrent_extendedPSE);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapCurrent_extendedPD);
    }
    if (temp_val.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
    {
      buf[0] |= (0x80 >> D_lldpXMedRemCapCurrent_inventory);
    }
  }

  return rc;
}

L7_RC_t
snmpLldpXMedRemDeviceClassGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedRemDeviceClassGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case notDefined:
      *val = D_lldpXMedRemDeviceClass_notDefined;
      break;
    case endpointClass1:
      *val = D_lldpXMedRemDeviceClass_endpointClass1;
      break;
    case endpointClass2:
      *val = D_lldpXMedRemDeviceClass_endpointClass2;
      break;
    case endpointClass3:
      *val = D_lldpXMedRemDeviceClass_endpointClass3;
      break;
    case networkConnectivity:
      *val = D_lldpXMedRemDeviceClass_networkConnectivity;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedRemLocationEntryGet(L7_uint32 timeMark, L7_uint32 intIfNum,
                                L7_uint32 remIndex, L7_uint32 subType, L7_uchar8 *buf)
{
  L7_uint32 temp_subType;

  if(snmpLldpRemEntryGet(timeMark, intIfNum, remIndex) != L7_SUCCESS ||
     snmpLldpXMedLocLocationSubTypeGet(subType, &temp_subType) != L7_SUCCESS ||
     usmDbLldpXMedRemLocationInfoGet(intIfNum, remIndex, timeMark, temp_subType, buf) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpLldpXMedRemLocationEntryNextGet(L7_uint32 *timeMark, L7_uint32 *intIfNum,
                                    L7_uint32 *remIndex, L7_uint32 *subType, L7_uchar8 *buf)
{
  do
  {
    while (*subType <= D_lldpXMedRemLocationSubtype_elin)
    {
      if (snmpLldpXMedRemLocationEntryGet(*timeMark, *intIfNum, *remIndex, *subType, buf) == L7_SUCCESS)
        return L7_SUCCESS;
      (*subType)++;
    }
    *subType = D_lldpXMedRemLocationSubtype_unknown;
  } while (snmpLldpRemEntryGetNext(timeMark, intIfNum, remIndex) == L7_SUCCESS);

  return L7_FAILURE;
}

L7_RC_t
snmpLldpXMedRemXPoEDeviceTypeGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedRemXPoeDeviceTypeGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_poedevicetype:
      *val = D_lldpXMedRemXPoEDeviceType_unknown;
      break;
    case pseDevice:
      *val = D_lldpXMedRemXPoEDeviceType_pseDevice;
      break;
    case pdDevice:
      *val = D_lldpXMedRemXPoEDeviceType_pdDevice;
      break;
    case none_poedevicetype:
      *val = D_lldpXMedRemXPoEDeviceType_none;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedRemXPoEPSEPowerSourceGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedRemXPoePSEPowerSrcGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_psesrc:
      *val = D_lldpXMedRemXPoEPSEPowerSource_unknown;
      break;
    case primary_psesrc:
      *val = D_lldpXMedRemXPoEPSEPowerSource_primary;
      break;
    case backup_psesrc:
      *val = D_lldpXMedRemXPoEPSEPowerSource_backup;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedRemXPoEPSEPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedRemXPoePSEPowerPriGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_pwpri:
      *val = D_lldpXMedRemXPoEPSEPowerPriority_unknown;
      break;
    case critical_pwpri:
      *val = D_lldpXMedRemXPoEPSEPowerPriority_critical;
      break;
    case high_pwpri:
      *val = D_lldpXMedRemXPoEPSEPowerPriority_high;
      break;
    case low_pwpri:
      *val = D_lldpXMedRemXPoEPSEPowerPriority_low;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedRemXPoEPDPowerSourceGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedRemXPoePDPowerSrcGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_pdsrc:
      *val = D_lldpXMedRemXPoEPDPowerSource_unknown;
      break;
    case fromPSE_pdsrc:
      *val = D_lldpXMedRemXPoEPDPowerSource_fromPSE;
      break;
    case local_pdsrc:
      *val = D_lldpXMedRemXPoEPDPowerSource_local;
      break;
    case localAndPSE_pdsrc:
      *val = D_lldpXMedRemXPoEPDPowerSource_localAndPSE;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedRemXPoEPDPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLldpXMedRemXPoePDPowerPriGet(intIfNum, remIndex, timestamp, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case unknown_pwpri:
      *val = D_lldpXMedRemXPoEPDPowerPriority_unknown;
      break;
    case critical_pwpri:
      *val = D_lldpXMedRemXPoEPDPowerPriority_critical;
      break;
    case high_pwpri:
      *val = D_lldpXMedRemXPoEPDPowerPriority_high;
      break;
    case low_pwpri:
      *val = D_lldpXMedRemXPoEPDPowerPriority_low;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpLldpXMedRemMediaPolicyEntryGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, 
                                   lldpXMedPolicyAppTypeValue_t appType)
{
  L7_uint32 temp_val;

  if(usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                   intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if(usmDbLldpRemTimestampIndexEntryGet(timestamp,intIfNum,remIndex) == L7_SUCCESS &&
     usmDbLldpXMedRemMediaPolicyVlanIdGet(intIfNum, remIndex, timestamp, appType, &temp_val) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpLldpXMedRemMediaPolicyEntryNextGet(L7_uint32 *intIfNum, L7_uint32 *remIndex, L7_uint32 *timestamp, 
                                       L7_uchar8 *buf, L7_uint32 *buf_len)
{
  lldpXMedPolicyAppTypeValue_t appType = D_lldpXMedLocMediaPolicyAppType_unknown;

  if (snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(buf, *buf_len, &appType) == L7_SUCCESS &&
      snmpLldpXMedRemMediaPolicyEntryGet(*intIfNum, *remIndex, *timestamp, appType) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  do
  {
    /* iterate through supported app types */
    while (snmpLldpXMedLocMediaPolicyAppTypeNextGet(buf, buf_len) == L7_SUCCESS)
    {
      if (snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(buf, *buf_len, &appType) != L7_SUCCESS)
      {
        break;
      }
      if (snmpLldpXMedRemMediaPolicyEntryGet(*intIfNum, *remIndex, *timestamp, appType) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
    } 
    /* no matches, go to the next interface */
    /* reset to first bit */
    buf[0] = 0x00;
    buf[1] = 0x00;
    *buf_len = 0;
  } while (snmpLldpRemEntryGetNext(timestamp, intIfNum, remIndex) == L7_SUCCESS);

  return L7_FAILURE;
}

L7_RC_t
snmpLldpXMedRemMediaPolicyUnknownGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXMedRemMediaPolicyUnknownGet(intIfNum, remIndex, timestamp, appType, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXMedRemMediaPolicyUnknown_true;
      break;
    case L7_FALSE:
      *val = D_lldpXMedRemMediaPolicyUnknown_false;
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
snmpLldpXMedRemMediaPolicyTaggedGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbLldpXMedRemMediaPolicyTaggedGet(intIfNum, remIndex, timestamp, appType, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_lldpXMedRemMediaPolicyTagged_true;
      break;
    case L7_FALSE:
      *val = D_lldpXMedRemMediaPolicyTagged_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
/*********************************************************************
*
* @purpose  Gets this devices PD power requirement
*
* @param    L7_uint32                  *pdPowerReq @b((output))  If this device is a PD then its requirement
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snmpLldpXMedLocXPoePDPowerReqGet(L7_uint32 *val)
{
#if 0
   return usmDbLldpXMedLocXPoePDPowerReqGet(L7_uint32 intIfNum,L7_uint32 *pdPowerReq);
#endif
  *val = FD_POE_POWER_LIMIT;
  return L7_SUCCESS;
}
