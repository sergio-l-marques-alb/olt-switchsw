/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_vrrp_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 11/01/2001(Nov. 1, 2001)
*
* @author Kumar Manish
*
* @end
*             
**********************************************************************/

/**************************@null{*************************************
                     
 *******************************}*************************************}
 **********************************************************************/

 #include "usmdb_mib_vrrp_api.h"
 #include "usmdb_common.h"
 #include "usmdb_util_api.h"
 L7_uint32 usmSnmpMaskLength(L7_uint32 mask); 
L7_RC_t          
snmpVrrpNotificationCntlGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uchar8 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpNotificationCntlGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_vrrpNotificationCntl_enabled;
        break;

      case L7_DISABLE:
        *val = D_vrrpNotificationCntl_disabled;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}


L7_RC_t          
snmpVrrpNotificationCntlSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_uchar8 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_vrrpNotificationCntl_enabled:
      temp_val = L7_ENABLE;
      break;

    case D_vrrpNotificationCntl_disabled:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
        rc = usmDbVrrpNotificationCntlSet(UnitIndex, temp_val);
    }

  return rc;
}


L7_RC_t          
snmpVrrpOperEntryGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 ifIndex)
 
{
    L7_uint32 intIfNum;

    if(ifIndex == 0)
        intIfNum =0;
    else if(usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS)
         return L7_FAILURE;
    if(usmDbVrrpOperEntryGet(UnitIndex, vrId, intIfNum) != L7_SUCCESS)
        return L7_FAILURE;
    return L7_SUCCESS;
}


L7_RC_t          
snmpVrrpOperEntryNextGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 ifIndex,
                                              L7_uint32 *vrIdNext, L7_uint32 *ifIndexNext)
 
{
    L7_uint32 intIfNum,intIfNumNext;
    L7_uchar8 ucharVrId, ucharVrIdNext;
    
    /*
     * since converting to a char,
     * make sure 32 value is not greater than 255
     */
    if (vrId > 255)
         return L7_FAILURE;

    ucharVrId = (L7_uchar8)vrId;
    
    if(ifIndex == 0)
        intIfNum =0;

    else if(usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS)
         return L7_FAILURE;

    if(usmDbVrrpOperEntryNextGet(UnitIndex, ucharVrId, intIfNum, 
                                 &ucharVrIdNext, &intIfNumNext) != L7_SUCCESS)
        return L7_FAILURE;

    *vrIdNext = (L7_uint32)ucharVrIdNext;
    
    if(usmDbExtIfNumFromIntIfNum(intIfNumNext, ifIndexNext) != L7_SUCCESS)
        return L7_FAILURE;
    
    return L7_SUCCESS;
}

L7_RC_t          
snmpVrrpOperStateGet( L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, 
                      L7_int32 *val )
{
  L7_vrrpState_t operState;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpOperStateGet (UnitIndex, vrId, intIfNum, &operState);

  if (rc == L7_SUCCESS)
  {
      switch (operState)
      {
      case L7_VRRP_STATE_INIT:
        *val = D_vrrpOperState_initialize;
        break;

      case L7_VRRP_STATE_MASTER:
        *val = D_vrrpOperState_master;
        break;

      case L7_VRRP_STATE_BACKUP:
        *val = D_vrrpOperState_backup;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}


L7_RC_t          
snmpVrrpOperAdminStateGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                          L7_uint32 *val)
 
{
  L7_vrrpStartState_t adminState;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpOperVrrpModeGet(UnitIndex, vrId, intIfNum, &adminState);

  if (rc == L7_SUCCESS)
  {
      switch (adminState)
      {
      case L7_VRRP_UP:
        *val = D_vrrpOperAdminState_up;
        break;

      case L7_VRRP_DOWN:
        *val = D_vrrpOperAdminState_down;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
 
}


L7_RC_t          
snmpVrrpOperAdminStateSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                          L7_uint32 adminState)
 
{
    return usmDbVrrpOperVrrpModeSet(UnitIndex, vrId, intIfNum, adminState);
    
}

L7_RC_t          
snmpVrrpConfigPriorityGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                        L7_uint32 *val)
 
{
  L7_uchar8 Priority;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpConfigPriorityGet(UnitIndex, vrId, intIfNum, &Priority);
  
  *val = (L7_uint32)Priority;

  return rc;
 
}

  L7_RC_t
snmpVrrpOperPriorityGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
    L7_uint32 *val)

{
  L7_uchar8 Priority;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbVrrpOperPriorityGet(UnitIndex, vrId, intIfNum, &Priority);

  *val = (L7_uint32)Priority;

  return rc;

}

L7_RC_t          
snmpVrrpOperPrioritySet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                        L7_uint32 priority)
{
  L7_uchar8 oldPriority;

  if (usmDbVrrpOperPriorityGet(UnitIndex, vrId, intIfNum, &oldPriority) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else if (oldPriority == 255)
  {
    return L7_FAILURE;
  }

  if (priority > 0 && priority < 255)
  {
    if (usmDbVrrpOperPrioritySet(UnitIndex, vrId, intIfNum, (L7_uchar8)priority) != L7_SUCCESS)
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
snmpVrrpOperIpAddrCountGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                           L7_uint32 *val)
 
{
  L7_uchar8 IpAddrCount;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpOperIpAddrCountGet(UnitIndex, vrId, intIfNum, &IpAddrCount);
  
  *val = (L7_uint32)IpAddrCount;

  return rc;
 
}


L7_RC_t          
snmpVrrpOperMasterIpAddrGet(L7_uint32  UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                           L7_uint32* ipAddress)
 
{
    return usmDbVrrpOperMasterIpAddressGet(UnitIndex, vrId, intIfNum, ipAddress);
    
}


L7_RC_t          
snmpVrrpOperPrimaryIpAddrGet(L7_uint32  UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                           L7_uint32* ipAddress)
 
{
    return usmDbVrrpOperMasterIpAddressGet(UnitIndex, vrId, intIfNum, ipAddress);

}


L7_RC_t          
snmpVrrpOperPrimaryIpAddrSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                           L7_uint32 ipAddress)
 
{
  L7_uint32 masterIpAddr;


  if (usmDbVrrpOperMasterIpAddressGet(UnitIndex, vrId, intIfNum, &masterIpAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  if ((ipAddress != 0) || (ipAddress != masterIpAddr))
  {
    return L7_NOT_SUPPORTED;
  }
  else
  {
    return L7_SUCCESS;
  }

}

L7_RC_t          
snmpVrrpOperAuthTypeGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                        L7_uint32 *val)
 
{
  L7_AUTH_TYPES_t authType;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpOperAuthTypeGet(UnitIndex, vrId, intIfNum, &authType);

  if (rc == L7_SUCCESS)
  {
      switch (authType)
      {
      case L7_AUTH_TYPE_NONE:
        *val = D_vrrpOperAuthType_noAuthentication;
        break;

      case L7_AUTH_TYPE_SIMPLE_PASSWORD:
        *val = D_vrrpOperAuthType_simpleTextPassword;
        break;

      case L7_AUTH_TYPE_MD5:
        *val = D_vrrpOperAuthType_ipAuthenticationHeader;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
 
}

L7_RC_t          
snmpVrrpOperAuthTypeSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                        L7_uint32 val)
 
{
  L7_AUTH_TYPES_t authType = 0;
  L7_RC_t rc;

  rc = L7_SUCCESS;
  
  switch (val)
  {
  case D_vrrpOperAuthType_noAuthentication:
    authType = L7_AUTH_TYPE_NONE;
    break;

  case D_vrrpOperAuthType_simpleTextPassword:
    authType = L7_AUTH_TYPE_SIMPLE_PASSWORD;
    break;

  case D_vrrpOperAuthType_ipAuthenticationHeader:
    authType = L7_AUTH_TYPE_MD5;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbVrrpOperAuthTypeSet(UnitIndex, vrId, intIfNum, authType);
  }

  return rc;
}

L7_RC_t 
snmpVrrpOperAuthKeySet(L7_uint32 unitIndex, L7_uchar8 vrId, L7_uint32 intIfNum,
                       L7_uchar8* authKey)
{
  if (strlen(authKey) > L7_VRRP_MAX_AUTH_DATA)
    return L7_FAILURE;

  if (usmDbVrrpOperAuthKeySet(unitIndex, vrId, intIfNum, authKey) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t          
snmpVrrpOperAdvertisementIntervalGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                                     L7_uint32 *val)
 
{
  L7_uchar8 AdvertInterval;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpOperAdvertisementIntervalGet(UnitIndex, vrId, intIfNum, &AdvertInterval);
  
  *val = (L7_uint32)AdvertInterval;

  return rc;
 
}


L7_RC_t          
snmpVrrpOperPreemptModeGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                           L7_uint32 *val)
 
{
  L7_BOOL preemptMode;
  L7_RC_t rc;

  rc = L7_FAILURE;
  
  rc = usmDbVrrpOperPreemptModeGet(UnitIndex, vrId, intIfNum, &preemptMode);

  if (rc == L7_SUCCESS)
  {
      switch (preemptMode)
      {
      case L7_FALSE:
        *val = D_vrrpOperPreemptMode_false;
        break;

      case L7_TRUE:
        *val = D_vrrpOperPreemptMode_true;
        break;
      
      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
 
}

L7_RC_t          
snmpVrrpOperPreemptModeSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                           L7_uint32 val)
 
{
  L7_BOOL preemptMode = L7_FALSE;
  L7_RC_t rc;

  rc = L7_SUCCESS;
  
  switch (val)
  {
  case D_vrrpOperPreemptMode_false:
    preemptMode = L7_FALSE;
    break;

  case D_vrrpOperPreemptMode_true:
    preemptMode = L7_TRUE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbVrrpOperPreemptModeSet(UnitIndex, vrId, intIfNum, preemptMode);
  }

  return rc;
}

L7_RC_t          
snmpVrrpOperUpTimeGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                      L7_uint32 *val)
 
{
  L7_uint32 upTime;
  L7_RC_t rc = L7_FAILURE;
  L7_vrrpState_t operState;

  if (usmDbVrrpOperStateGet(UnitIndex, vrId, intIfNum, &operState) == L7_SUCCESS) 
  { 
    *val = 0;
    rc = L7_SUCCESS;
    if (operState != L7_VRRP_STATE_INIT)
    {
      rc = usmDbVrrpOperUpTimeGet(UnitIndex, vrId, intIfNum, &upTime);
      *val = osapiUpTimeRaw() - upTime;
    }
  } 

  return rc;
 
}

L7_RC_t          
snmpVrrpOperProtocolGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                        L7_uint32 *val)
 
{
  L7_vrrpProtocolType_t protocolType;
  L7_RC_t rc;

  rc = L7_SUCCESS;
  
  rc = usmDbVrrpOperProtocolGet(UnitIndex, vrId, intIfNum, &protocolType);

  if (rc == L7_SUCCESS)
  {
      switch (protocolType)
      {
      case L7_VRRP_IP:
        *val = D_vrrpOperProtocol_ip;
        break;

      case L7_VRRP_BRIDGE:
        *val = D_vrrpOperProtocol_bridge;
        break;

      case L7_VRRP_DECNET:
        *val = D_vrrpOperProtocol_decnet;
        break;

      case L7_VRRP_OTHER:
        *val = D_vrrpOperProtocol_other;
        break;
      
      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
 
}

L7_RC_t          
snmpVrrpOperProtocolSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                        L7_uint32 val)
 
{
  L7_vrrpProtocolType_t protocolType = 0;
  L7_RC_t rc;

  rc = L7_SUCCESS;
  
  switch (val)
  {
  case D_vrrpOperProtocol_ip:
    protocolType = L7_VRRP_IP;
    break;

  case D_vrrpOperProtocol_bridge:
    protocolType = L7_VRRP_BRIDGE;
    break;

  case D_vrrpOperProtocol_decnet:
    protocolType = L7_VRRP_DECNET;
    break;

  case D_vrrpOperProtocol_other:
    protocolType = L7_VRRP_OTHER;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbVrrpOperProtocolSet(UnitIndex, vrId, intIfNum, protocolType);
  }

  return rc;
 
}

L7_RC_t          
snmpVrrpOperRowStatusSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                         L7_uint32 operRowStatus)
 
{
    L7_RC_t rc = L7_SUCCESS;

    switch (operRowStatus)
    {
    case D_vrrpOperRowStatus_active:
    case D_vrrpOperRowStatus_createAndGo:
    case D_vrrpOperRowStatus_createAndWait:

      break;

    case D_vrrpOperRowStatus_destroy:

      rc = usmDbVrrpRouterIdDelete(UnitIndex, vrId, intIfNum);
      break;

    default:
      rc = L7_FAILURE;
    }

  return rc;

    
}



L7_RC_t          
snmpVrrpAssoIpAddrEntryGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 ifIndex, 
                           L7_uint32 ipAddress)
 
{
    L7_uint32 intIfNum;

    if(usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS)
        return L7_FAILURE;
    if(usmDbVrrpAssoIpAddrEntryGet(UnitIndex, vrId, intIfNum, ipAddress) != L7_SUCCESS)
        return L7_FAILURE;
    return L7_SUCCESS;
}


L7_RC_t          
snmpVrrpAssoIpAddrEntryNextGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 ifIndex,
                               L7_uint32 ipAddress, L7_uint32 *vrIdNext, 
                               L7_uint32 *ifIndexNext, L7_uint32 *ipAddressNext)
 
{
  L7_uint32 intIfNum,intIfNumNext;
  L7_uchar8 ucharVrIdNext;

  ucharVrIdNext = *((L7_uchar8*)vrIdNext);

  if (ifIndex == 0)
    intIfNum = 0;
  else if(usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
 
  if(usmDbVrrpAssoIpAddrEntryNextGet(UnitIndex, vrId, intIfNum, ipAddress, 
                                     &ucharVrIdNext, &intIfNumNext, ipAddressNext) != L7_SUCCESS)
    return L7_FAILURE;
  
   
  if(usmDbExtIfNumFromIntIfNum(intIfNumNext, ifIndexNext) != L7_SUCCESS)
    return L7_FAILURE;
  
  *vrIdNext = ucharVrIdNext;
  return L7_SUCCESS;
  

}



L7_RC_t
snmpAgentSnmpVrrpNewMasterTrapFlagGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapVrrpSwGet ( UnitIndex, &temp_val, L7_VRRP_TRAP_NEW_MASTER );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpVRRPNewMasterTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpVRRPNewMasterTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpVrrpNewMasterTrapFlagSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpVRRPNewMasterTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpVRRPNewMasterTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapVrrpSwSet ( UnitIndex, temp_val, L7_VRRP_TRAP_NEW_MASTER );
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpVrrpAuthFailureTrapFlagGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapVrrpSwGet ( UnitIndex, &temp_val, L7_VRRP_TRAP_AUTH_FAILURE );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpVRRPAuthFailureTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpVRRPAuthFailureTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpVrrpAuthFailureTrapFlagSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpVRRPAuthFailureTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpVRRPAuthFailureTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapVrrpSwSet ( UnitIndex, temp_val, L7_VRRP_TRAP_AUTH_FAILURE );
  }

  return rc;
}

/**************************************************************************************************************/

/*********************************************************************
* @purpose  Gets the router VRRP Admin mode mode
*
* @param    adminMode            @b{(output)} Global Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentVrrpAdminStateGet(L7_uint32 *adminMode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempAdminMode;
 
  if ((rc = usmDbVrrpOperAdminStateGet(USMDB_UNIT_CURRENT, &tempAdminMode)) 
                                                                  == L7_SUCCESS)
  {
    switch(tempAdminMode)
    {
      case L7_TRUE:
        *adminMode = D_agentRouterVrrpAdminState_enable;
        break;

      case L7_FALSE:
        *adminMode = D_agentRouterVrrpAdminState_disable;
        break;
     
      default:
        rc = L7_FAILURE;
        return rc;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Sets the router VRRP Admin mode mode
*
* @param    adminMode            @b{(input)} Global Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This enables/disables the VRRP Global Admin Mode
*
* @end
*********************************************************************/
L7_RC_t 
snmpAgentVrrpAdminStateSet(L7_uint32 adminMode)
{
  L7_uint32 tempAdminMode;
  L7_RC_t rc = L7_FAILURE;
  
  switch(adminMode)
  {
    case D_agentRouterVrrpAdminState_enable:
      tempAdminMode = L7_TRUE;
      break;
    
    case D_agentRouterVrrpAdminState_disable:
      tempAdminMode = L7_FALSE;
      break;
   
    default:
      rc = L7_FAILURE;
      return rc;
  }
  
  return usmDbVrrpOperAdminStateSet(USMDB_UNIT_CURRENT, tempAdminMode);
}


/* VRRP TRACKING APIS */

   /*********************START OF VRRP INTERFACE TRACKING APIS *************************/

L7_RC_t 
snmpVrrpTrackIntfEntryGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 ifIndex, L7_uint32 trackIfIndex)

{
  L7_uint32 intIfNum, trackIntIfNum;

  if(ifIndex == 0)
    intIfNum =0;
  else if(usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if(trackIfIndex == 0)
    trackIntIfNum =0;
  else if (usmDbIntIfNumFromExtIfNum (trackIfIndex,&trackIntIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  
  if(usmDbVrrpVrIdIntfTrackIntfGet(UnitIndex, vrId, intIfNum, trackIntIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  
  return L7_SUCCESS;
}


L7_RC_t
snmpVrrpTrackIntfEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *vrId, L7_uint32 *ifIndex,
                             L7_uint32 *trackIfIndex)

{
  L7_uint32 intIfNum,trackIntIfNum;
  L7_uchar8 ucharVrId;
  L7_RC_t rc = L7_FAILURE;

  /*
   * since converting to a char,
   * make sure 32 value is not greater than 255
   */
  if (*vrId > 255)
    return L7_FAILURE;

  ucharVrId = (L7_uchar8)*vrId;

  rc = usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum);

  if (rc != L7_SUCCESS)
  {
    /* find the next valid internal interface by first finding the next valid external interface */
    if (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS &&
        usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) == L7_SUCCESS)
      rc = L7_SUCCESS;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIntIfNumFromExtIfNum(*trackIfIndex, &trackIntIfNum);
    if (rc != L7_SUCCESS)
    {
      /* find the next valid internal interface by first finding the next valid external interface */
      if (!(usmDbGetNextVisibleExtIfNumber(*trackIfIndex, trackIfIndex) == L7_SUCCESS &&
          usmDbIntIfNumFromExtIfNum(*trackIfIndex, &trackIntIfNum) == L7_SUCCESS))
      {
       /* In this case lets Application to handle this case by returning next valid entry */
        *trackIfIndex = 0;
        trackIntIfNum = 0;
        /* find the next valid internal interface by first finding the next valid external interface */
        if (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS &&
            usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) == L7_SUCCESS)
          rc = L7_SUCCESS;
      }
      else
      {
        rc = L7_SUCCESS;
      }
    }
  }
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if(usmDbVrrpVrIdIntfTrackIntfGet(UnitIndex, ucharVrId, intIfNum, trackIntIfNum) != L7_SUCCESS)
  { 
    if(usmDbVrrpOperTrackIntfNextGet(UnitIndex,&ucharVrId, &intIfNum, &trackIntIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  *vrId = (L7_uint32)ucharVrId;
  if(usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex)!= L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if(usmDbExtIfNumFromIntIfNum(trackIntIfNum, trackIfIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpVrrpTrackIntfAdd(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, L7_uint32 trackIfIndex,
        L7_uint32 prioDec)
{
  L7_uint32 trackIntIfNum;
  
  if(usmDbIntIfNumFromExtIfNum(trackIfIndex, &trackIntIfNum) != L7_SUCCESS)
        return L7_FAILURE; 
  if (usmDbVrrpOperTrackIntfAdd(UnitIndex, vrId,intIfNum,  trackIntIfNum, (L7_uchar8)prioDec) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

L7_RC_t
snmpVrrpTrackIfPrioSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, 
                         L7_uint32 trackIfIndex, L7_uint32 prioDec)
{
  L7_uint32 trackIntIfNum;

  if(usmDbIntIfNumFromExtIfNum(trackIfIndex, &trackIntIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  
  if (usmDbVrrpVrIdIntfTrackIntfGet(UnitIndex, vrId, intIfNum, trackIntIfNum ) == L7_SUCCESS)
  {
    if (usmDbVrrpOperTrackIntfAdd(UnitIndex, vrId, intIfNum, 
          trackIntIfNum,(L7_uchar8) prioDec) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;

}

  L7_RC_t
snmpVrrpTrackIfRowStatusSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, 
                          L7_uint32 trackIfIndex, L7_uint32 trackIfStatus)
{
  L7_uint32 trackIntIfNum;
  L7_RC_t rc = L7_SUCCESS;

  if(usmDbIntIfNumFromExtIfNum(trackIfIndex, &trackIntIfNum) != L7_SUCCESS)
        return L7_FAILURE;

  switch (trackIfStatus)
  {
    case D_agentRouterVrrpTrackIfStatus_active:
    case D_agentRouterVrrpTrackIfStatus_createAndGo:
    case D_agentRouterVrrpTrackIfStatus_createAndWait:

      break;

    case D_agentRouterVrrpTrackIfStatus_destroy:

      rc = usmDbVrrpOperTrackIntfDelete(UnitIndex, vrId, intIfNum, trackIntIfNum);
      break;

    default:
      rc = L7_FAILURE;
  }

  return rc;

}

L7_RC_t
snmpVrrpOperTrackIntfPrioGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum,
                             L7_uint32 trackIfIndex,L7_uint32 *val)
{
  L7_uchar8 priority;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 trackIntIfNum;
  
  if(usmDbIntIfNumFromExtIfNum(trackIfIndex, &trackIntIfNum) != L7_SUCCESS)
    return L7_FAILURE; 
 
  rc = usmDbVrrpOperTrackIntfPrioGet(UnitIndex, vrId, intIfNum, trackIntIfNum, &priority);

  *val = (L7_uint32)priority;

  return rc;

}

L7_RC_t 
snmpVrrpOperTrackIntfStateGet(L7_uint32 UnitIndex, L7_uint32 vrId,  L7_uint32 intIfNum,
                              L7_uint32 trackIfIndex,L7_uint32 *val)
{
  L7_BOOL state;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 trackIntIfNum;
  
  if(usmDbIntIfNumFromExtIfNum(trackIfIndex, &trackIntIfNum) != L7_SUCCESS)
    return L7_FAILURE; 

  rc = usmDbVrrpOperTrackIntfStateGet(UnitIndex, vrId, intIfNum, trackIntIfNum, &state);
  if (rc == L7_SUCCESS)
  {
    switch (state)
    {
      case L7_TRUE:
                  *val = D_agentRouterVrrpTrackIfState_true;
                  break;
      default :
                  *val = D_agentRouterVrrpTrackIfState_false;
                  break;
    }
  }
  return rc;
}

   /********************* END OF VRRP INTERFACE TRACKING APIS *************************/


   /*********************START OF VRRP ROUTE TRACKING APIS *************************/
L7_RC_t
snmpVrrpTrackRouteEntryGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 ifIndex, L7_uint32 pfx, L7_uint32 pfxLen)

{
  L7_uint32 intIfNum, mask;

  if(ifIndex == 0)
    intIfNum =0;
  else if(usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  
  if (pfxLen == 0)
  {
    mask = 0;
  }
  else
  {
    if (usmDbIpSubnetMaskGet(&mask,pfxLen)!= L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if(usmDbVrrpVrIdIntfTrackRouteGet(UnitIndex, vrId, intIfNum, pfx, mask) != L7_SUCCESS)
    return L7_FAILURE;
  
  return L7_SUCCESS;
}

L7_RC_t
snmpVrrpTrackRouteEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *vrId, L7_uint32 *ifIndex,
    L7_uint32 *pfx, L7_uint32 *pfxLen)

{
  L7_uint32 intIfNum;
  L7_uchar8 ucharVrId;
  L7_uint32 mask;
  L7_RC_t rc = L7_FAILURE;
  /*
   * since converting to a char,
   * make sure 32 value is not greater than 255
   */
  if (*vrId > 255)
  {
    return L7_FAILURE;
  }
  if (*pfxLen == 0)
  {
    mask = 0;
  }
  else 
  {
    if (usmDbIpSubnetMaskGet(&mask,*pfxLen)!= L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  ucharVrId = (L7_uchar8)*vrId;

  rc = usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum);

  if (rc != L7_SUCCESS)
  {
    /* find the next valid internal interface by first finding the next valid external interface */
    if (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS &&
        usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) == L7_SUCCESS)
      rc = L7_SUCCESS;
  }
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  if(usmDbVrrpOperTrackRouteNextGet(UnitIndex,&ucharVrId, &intIfNum, pfx, &mask) != L7_SUCCESS)
    return L7_FAILURE;

  *vrId = (L7_uint32)ucharVrId;
  *pfxLen = usmSnmpMaskLength (mask);

  if(usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex)!= L7_SUCCESS)
    return L7_FAILURE;


  return L7_SUCCESS;
}

L7_RC_t
snmpVrrpTrackRtAdd(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, L7_uint32 ipAddr, 
    L7_uint32 pfxLen, L7_uint32 prioDec)
{
  L7_uint32 mask;

  if (usmDbIpSubnetMaskGet(&mask,pfxLen)!= L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (usmDbVrrpOperTrackRouteAdd(UnitIndex, vrId, intIfNum, ipAddr, mask, (L7_uchar8)prioDec) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
    
}

L7_RC_t
snmpVrrpTrackRtPrioSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, L7_uint32 ipAddr,
        L7_uint32 pfxLen, L7_uint32 prioDec)
{
 L7_uint32 mask;
 
  if (usmDbIpSubnetMaskGet(&mask,pfxLen)!= L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (usmDbVrrpVrIdIntfTrackRouteGet(UnitIndex, vrId, intIfNum, ipAddr, mask) == L7_SUCCESS)
  {
    if (usmDbVrrpOperTrackRouteAdd(UnitIndex, vrId, intIfNum, ipAddr, mask,(L7_uchar8) prioDec) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
    
}

L7_RC_t
snmpVrrpTrackRtRowStatusSet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, L7_uint32 ipAddr,
                            L7_uint32 pfxLen, L7_uint32 RtStatus)
{
  L7_uint32 mask;
  L7_RC_t rc = L7_SUCCESS;

  if (usmDbIpSubnetMaskGet(&mask,pfxLen)!= L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  switch (RtStatus)
  {
    case D_agentRouterVrrpTrackRtStatus_active:
    case D_agentRouterVrrpTrackRtStatus_createAndGo:
    case D_agentRouterVrrpTrackRtStatus_createAndWait:

      break;

    case D_agentRouterVrrpTrackRtStatus_destroy:

      rc = usmDbVrrpOperTrackRouteDelete(UnitIndex, vrId, intIfNum, ipAddr, mask);
      break;

    default:
      rc = L7_FAILURE;
  }

  return rc;

}

  L7_RC_t
snmpVrrpOperTrackRoutReachabilityGet(L7_uint32 UnitIndex, L7_uint32 vrId,  L7_uint32 intIfNum,
    L7_uint32 ipAddr, L7_uint32 pfxLen, L7_uint32 *val)
{
  L7_BOOL state;
  L7_RC_t rc;
  L7_uint32 mask;

  rc = L7_FAILURE;
  if (usmDbIpSubnetMaskGet(&mask,pfxLen)!= L7_SUCCESS)
  {
     return L7_FAILURE;
  }

  rc = usmDbVrrpOperTrackRouteReachabilityGet(UnitIndex, vrId, intIfNum, ipAddr, mask, &state);
  if (rc == L7_SUCCESS)
  {
    switch (state)
    {
      case L7_TRUE:
        *val = D_agentRouterVrrpTrackRtReachable_true;
        break;
      default :
        *val = D_agentRouterVrrpTrackRtReachable_false;
        break;
    }
  }
  return rc;
}


  L7_RC_t
snmpVrrpOperTrackRoutePrioGet(L7_uint32 UnitIndex, L7_uint32 vrId, L7_uint32 intIfNum, 
                              L7_uint32 pfx, L7_uint32 pfxLen, L7_uint32 *val)
{
  L7_uchar8 Priority;
  L7_RC_t rc;
  L7_uint32 mask;

  rc = L7_FAILURE;
  if (usmDbIpSubnetMaskGet(&mask,pfxLen)!= L7_SUCCESS)
  {
     return L7_FAILURE;
  }
  rc = usmDbVrrpOperTrackRoutePrioGet(UnitIndex, vrId, intIfNum, pfx, mask, &Priority);

  *val = (L7_uint32)Priority;

  return rc;

}

L7_uint32 usmSnmpMaskLength(L7_uint32 mask)
{
  L7_uint32 length=0;
  L7_uint32 b;

  b = 0X80000000;

  while(mask & b)
  {
    length++;
    b = b >> 1;
  }

  return length;
}


   /*********************END OF VRRP ROUTE TRACKING APIS *************************/
