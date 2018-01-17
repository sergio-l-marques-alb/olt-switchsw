/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_ripv2_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 05/15/2001
*
* @author soma
*
* @end
*             
**********************************************************************/

#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "usmdb_iputil_api.h"
#include "usmdb_mib_ripv2_api.h"


/**************************@null{*************************************
                     
 *******************************}*************************************}
 **********************************************************************/

/* lvl7_@p1707 start */
L7_RC_t
snmpRip2IfStatEntryGet( L7_uint32 UnitIndex, L7_uint32 rip2IfStatAddress)
{
  L7_uint32 temp_intIfNum;
  L7_uint32 temp_val;

  /* check that it's a valid index, and that the interface is up */
  if (usmDbRip2IfStatEntryGet(UnitIndex, rip2IfStatAddress) == L7_SUCCESS &&
      usmDbRip2IfIPAddrToIntIf(USMDB_UNIT_CURRENT, rip2IfStatAddress, &temp_intIfNum) == L7_SUCCESS &&
      usmDbRip2IntfStateGet(UnitIndex, temp_intIfNum, &temp_val) == L7_SUCCESS && temp_val == L7_UP)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpRip2IfStatEntryNextGet( L7_uint32 UnitIndex, L7_uint32 *rip2IfStatAddress)
{
  while (usmDbRip2IfStatEntryNext(UnitIndex, rip2IfStatAddress) == L7_SUCCESS)
  {
    if (snmpRip2IfStatEntryGet(UnitIndex, *rip2IfStatAddress) == L7_SUCCESS)
      return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/* lvl7_@p1707 end */

/* lvl7_@p0971 start */
L7_RC_t
snmpRip2IfStatStatusGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbRip2IfStatStatusGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE :
      *val = D_rip2IfStatStatus_active;
      break;
    
    case L7_DISABLE : /* should never reach here */
      *val = D_rip2IfStatStatus_notInService;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpRip2IfStatStatusSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc;

  rc = L7_SUCCESS;

  switch (val)
  {
  case D_rip2IfStatStatus_active :
  case D_rip2IfStatStatus_createAndGo :
  case D_rip2IfStatStatus_createAndWait :
    temp_val = L7_ENABLE;
    break;

  case D_rip2IfStatStatus_destroy :
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS &&
      usmDbValidateRtrIntf(UnitIndex, intIfNum) == L7_SUCCESS )
  {
    rc = usmDbRip2IfStatStatusSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}
/* lvl7_@p0971 end */

L7_RC_t
snmpRip2IfConfEntryGet( L7_uint32 UnitIndex, L7_uint32 rip2IfConfAddress)
{
  L7_uint32 temp_intIfNum;
  L7_uint32 temp_val;

  /* check that it's a valid index, and that the interface is up */
  if (usmDbRip2IfConfEntryGet(UnitIndex, rip2IfConfAddress) == L7_SUCCESS &&
      usmDbRip2IfIPAddrToIntIf(USMDB_UNIT_CURRENT, rip2IfConfAddress, &temp_intIfNum) == L7_SUCCESS &&
      usmDbRip2IntfStateGet(UnitIndex, temp_intIfNum, &temp_val) == L7_SUCCESS && temp_val == L7_UP)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpRip2IfConfAuthTypeGet ( L7_uint32 UnitIndex, L7_uint32 Address, L7_int32 *val )
{
 
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbRip2IfConfAuthTypeGet(UnitIndex, Address, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_AUTH_TYPE_NONE :         /* L7_NOAUTHENTICATION: */
      *val = D_rip2IfConfAuthType_noAuthentication;
      break;
    
    case L7_AUTH_TYPE_SIMPLE_PASSWORD :         /* L7_SIMPLEPASSWORD:   */
      *val = D_rip2IfConfAuthType_simplePassword;
      break;
    
    case L7_AUTH_TYPE_MD5 :         /* L7_MD5:   */
      *val = D_rip2IfConfAuthType_md5;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t          
snmpRip2IfConfAuthTypeSet ( L7_uint32 UnitIndex, L7_uint32 Address, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_rip2IfConfAuthType_noAuthentication:
      temp_val = L7_AUTH_TYPE_NONE;     /* L7_NOAUTHENTICATION; */
      break;

    case D_rip2IfConfAuthType_simplePassword:
      temp_val = L7_AUTH_TYPE_SIMPLE_PASSWORD;     /*  L7_SIMPLEPASSWORD;  */
      break;

    case D_rip2IfConfAuthType_md5:
      temp_val = L7_AUTH_TYPE_MD5;     /* L7_MD5;      */
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS &&
        usmDbValidateRtrIntf(UnitIndex, Address) == L7_SUCCESS)
    {
        rc = usmDbRip2IfConfAuthTypeSet (UnitIndex, Address, temp_val);
    }

  return rc;
}


L7_RC_t
snmpRip2IfConfSendGet ( L7_uint32 UnitIndex, L7_uint32 Address, L7_int32 *val )
{
 
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbRip2IfConfSendGet(UnitIndex, Address, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_RIP_CONF_SEND_DO_NOT_SEND :   
      *val = D_rip2IfConfSend_doNotSend;
      break;
    
    case L7_RIP_CONF_SEND_RIPV1 :         
      *val = D_rip2IfConfSend_ripVersion1;
      break;
    
    case L7_RIP_CONF_SEND_RIPV1_COMPAT :  
      *val = D_rip2IfConfSend_rip1Compatible;
      break;
    
    case L7_RIP_CONF_SEND_RIPV2 :         
      *val = D_rip2IfConfSend_ripVersion2;
      break;
    
    case L7_RIP_CONF_SEND_RIPV1_DEMAND :      
      *val = D_rip2IfConfSend_ripV1Demand;
      break;
    
    case L7_RIP_CONF_SEND_RIPV2_DEMAND :      
      *val = D_rip2IfConfSend_ripV2Demand;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t          
snmpRip2IfConfSendSet ( L7_uint32 UnitIndex, L7_uint32 Address, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_rip2IfConfSend_doNotSend:
      temp_val = L7_RIP_CONF_SEND_DO_NOT_SEND;
      break;

    case D_rip2IfConfSend_ripVersion1:
      temp_val = L7_RIP_CONF_SEND_RIPV1;
      break;

    case D_rip2IfConfSend_rip1Compatible:
      temp_val = L7_RIP_CONF_SEND_RIPV1_COMPAT;
      break;

    case D_rip2IfConfSend_ripVersion2:
      temp_val = L7_RIP_CONF_SEND_RIPV2;
      break;

/* lvl7_@p0974 start */
#ifdef NOT_SUPPORTED
    case D_rip2IfConfSend_ripV1Demand:
      temp_val = L7_RIP_CONF_SEND_RIPV1_DEMAND;
      break;

    case D_rip2IfConfSend_ripV2Demand:
      temp_val = L7_RIP_CONF_SEND_RIPV2_DEMAND;      
      break;
#endif /* NOT_SUPPORTED */
/* lvl7_@p0974 end */

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS &&
        usmDbValidateRtrIntf(UnitIndex, Address) == L7_SUCCESS )
    {
        rc = usmDbRip2IfConfSendSet (UnitIndex, Address, temp_val);
    }

  return rc;
}


L7_RC_t
snmpRip2IfConfReceiveGet ( L7_uint32 UnitIndex, L7_uint32 Address, L7_int32 *val )
{
 
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbRip2IfConfReceiveGet(UnitIndex, Address, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    
    case L7_RIP_CONF_RECV_RIPV1 :  
      *val = D_rip2IfConfReceive_rip1;
      break;
    
    case L7_RIP_CONF_RECV_RIPV2 :         
      *val = D_rip2IfConfReceive_rip2;
      break;
    
    case L7_RIP_CONF_RECV_RIPV1_RIPV2 :       
      *val = D_rip2IfConfReceive_rip1OrRip2;
      break;
    
    case L7_RIP_CONF_RECV_DO_NOT_RECEIVE : 
      *val = D_rip2IfConfReceive_doNotRecieve;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t          
snmpRip2IfConfReceiveSet ( L7_uint32 UnitIndex, L7_uint32 Address, L7_int32 val )
{
    L7_uint32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_rip2IfConfReceive_rip1:
      temp_val = L7_RIP_CONF_RECV_RIPV1;     
      break;

    case D_rip2IfConfReceive_rip2:
      temp_val = L7_RIP_CONF_RECV_RIPV2;     
      break;

    case D_rip2IfConfReceive_rip1OrRip2:
      temp_val = L7_RIP_CONF_RECV_RIPV1_RIPV2;           
      break;

    case D_rip2IfConfReceive_doNotRecieve:
      temp_val = L7_RIP_CONF_RECV_DO_NOT_RECEIVE;    
      break;
    
    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS &&
        usmDbValidateRtrIntf(UnitIndex, Address) == L7_SUCCESS )
    {
        rc = usmDbRip2IfConfReceiveSet (UnitIndex, Address, temp_val);
    }

  return rc;
}

/* lvl7_@p0971 start */
L7_RC_t
snmpRip2IfConfStatusGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbRip2IfConfStatusGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE :
      *val = D_rip2IfConfStatus_active;
      break;
    
    case L7_DISABLE : /* should never reach here */
      *val = D_rip2IfConfStatus_notInService;
      break;
    
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpRip2IfConfStatusSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc;

  rc = L7_SUCCESS;

  switch (val)
  {
  case D_rip2IfConfStatus_active :
  case D_rip2IfConfStatus_createAndGo :
  case D_rip2IfConfStatus_createAndWait :
    temp_val = L7_ENABLE;
    break;

  case D_rip2IfConfStatus_destroy :
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2IfConfStatusSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}
/* lvl7_@p0971 end */


L7_RC_t
snmpAgentRouterRipAdminStateGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbRip2AdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentRouterRipAdminState_enable;
      break;
    case L7_DISABLE:
      *val = D_agentRouterRipAdminState_disable;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/* lvl7_@p1515 start */
L7_RC_t
snmpAgentRouterRipAdminStateSet(L7_uint32 UnitIndex, L7_uint32 val)
{
/* lvl7_@p1515 end */
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentRouterRipAdminState_enable:
    temp_val = L7_ENABLE;
    break;
  case D_agentRouterRipAdminState_disable:
    temp_val = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2AdminModeSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipSplitHorizonModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbRip2SplitHorizonModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_RIP_SPLITHORIZON_NONE:
      *val = D_agentRouterRipSplitHorizonMode_none;
      break;
    case L7_RIP_SPLITHORIZON_SIMPLE:
      *val = D_agentRouterRipSplitHorizonMode_simple;
      break;
    case L7_RIP_SPLITHORIZON_POISONREVERSE:
      *val = D_agentRouterRipSplitHorizonMode_poisonReverse;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipSplitHorizonModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentRouterRipSplitHorizonMode_none:
    temp_val = L7_RIP_SPLITHORIZON_NONE;
    break;
  case D_agentRouterRipSplitHorizonMode_simple:
    temp_val = L7_RIP_SPLITHORIZON_SIMPLE;
    break;
  case D_agentRouterRipSplitHorizonMode_poisonReverse:
    temp_val = L7_RIP_SPLITHORIZON_POISONREVERSE;
    break;
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2SplitHorizonModeSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipAutoSummaryModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbRip2AutoSummarizationModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentRouterRipAutoSummaryMode_enable;
      break;
    case L7_DISABLE:
      *val = D_agentRouterRipAutoSummaryMode_disable;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipAutoSummaryModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentRouterRipAutoSummaryMode_enable:
    temp_val = L7_ENABLE;
    break;
  case D_agentRouterRipAutoSummaryMode_disable:
    temp_val = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2AutoSummarizationModeSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipHostRoutesAcceptModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbRip2HostRoutesAcceptModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentRouterRipHostRoutesAcceptMode_enable;
      break;
    case L7_DISABLE:
      *val = D_agentRouterRipHostRoutesAcceptMode_disable;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipHostRoutesAcceptModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentRouterRipHostRoutesAcceptMode_enable:
    temp_val = L7_ENABLE;
    break;
  case D_agentRouterRipHostRoutesAcceptMode_disable:
    temp_val = L7_DISABLE;
    break;
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2HostRoutesAcceptModeSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentRouterRipRoutePrefGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpRouterPreferenceGet(USMDB_UNIT_CURRENT, ROUTE_PREF_RIP, val);
}

L7_RC_t
snmpAgentRouterRipRoutePrefSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpRouterPreferenceSet(USMDB_UNIT_CURRENT, ROUTE_PREF_RIP, val);
}

L7_RC_t
snmpAgentRipDefaultMetricGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;

  rc = usmDbRip2DefaultMetricGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;

  else 
  {
    *val = 0;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

L7_RC_t
snmpAgentRipDefaultMetricSet(L7_uint32 UnitIndex, L7_uint32 mode, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  rc = usmDbRip2DefaultMetricSet(USMDB_UNIT_CURRENT, mode, val);
  return rc; 
}


L7_RC_t
snmpAgentRipDefaultMetricConfiguredGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val = L7_NULL;

  rc = usmDbRip2DefaultMetricGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
     *val = D_agentRouterRipDefaultMetricConfigured_true;

  else if (rc == L7_ERROR)
  {
    *val = D_agentRouterRipDefaultMetricConfigured_false;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

/* Setting of 'DefaultMetricConfigured' will only support setting the MetricConfigured 
   parameter to 'false', so it 'll be used for unconfiguring the metric */
L7_RC_t
snmpAgentRipDefaultMetricConfiguredSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  

  switch (val)
  {
    case D_agentRouterRipDefaultMetricConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentRouterRipDefaultMetricConfigured_false:
      rc = usmDbRip2DefaultMetricClear(USMDB_UNIT_CURRENT);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
    
  return rc;

}


L7_RC_t
snmpAgentRipDefaultInfoOriginateGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val = L7_NULL;

  rc = usmDbRip2DefaultRouteAdvertiseModeGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentRouterRipDefaultInfoOriginate_true;
      break;

    case L7_FALSE:
      *val = D_agentRouterRipDefaultInfoOriginate_false;
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
snmpAgentRipDefaultInfoOriginateSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = L7_NULL;

  switch (val)
  {
  case D_agentRouterRipDefaultInfoOriginate_true:
    temp_val = L7_TRUE;
    break;

  case D_agentRouterRipDefaultInfoOriginate_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2DefaultRouteAdvertiseModeSet(USMDB_UNIT_CURRENT, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentRipRouteRedistributeSourceGet(L7_uint32 UnitIndex, L7_uint32 redistSource)
{
  L7_RC_t rc;
  L7_uint32 redistSrcProto = L7_NULL;

  if( redistSource <   D_agentRipRouteRedistSource_connected)
        redistSrcProto = REDIST_RT_FIRST;

  if( redistSource >   D_agentRipRouteRedistSource_bgp)
        redistSrcProto = REDIST_RT_LAST;

  switch (redistSource)
    {
    case D_agentRipRouteRedistSource_connected :
      redistSrcProto = REDIST_RT_LOCAL;
      break;

    case D_agentRipRouteRedistSource_static :
      redistSrcProto = REDIST_RT_STATIC;
      break;

    case D_agentRipRouteRedistSource_ospf :
      redistSrcProto = REDIST_RT_OSPF;
      break;

    case D_agentRipRouteRedistSource_bgp :
      redistSrcProto = REDIST_RT_BGP;
      break;              

    default:
      break;
    }                     

  rc = usmDbRip2RouteRedistributeSourceGet(USMDB_UNIT_CURRENT,redistSrcProto); 
  return rc;
}

L7_RC_t
snmpAgentRipRouteRedistributeSourceGetNext(L7_uint32 UnitIndex, L7_uint32 *redistSource)
{
    L7_RC_t rc;
    L7_uint32 redistSrcProto = L7_NULL;

    if( *redistSource <   D_agentRipRouteRedistSource_connected)
        redistSrcProto = REDIST_RT_FIRST;

    if( *redistSource >   D_agentRipRouteRedistSource_bgp)
        redistSrcProto = REDIST_RT_LAST;

    switch (*redistSource)
    {
      case D_agentRipRouteRedistSource_connected :
        redistSrcProto = REDIST_RT_LOCAL;
        break;

      case D_agentRipRouteRedistSource_static :
        redistSrcProto = REDIST_RT_STATIC;
        break;

      case D_agentRipRouteRedistSource_ospf :
        redistSrcProto = REDIST_RT_OSPF;
        break;

      case D_agentRipRouteRedistSource_bgp :
        redistSrcProto = REDIST_RT_BGP;
        break;              

      default:
        break;
    }                
    
  rc = usmDbRip2RouteRedistributeSourceGetNext(USMDB_UNIT_CURRENT,&redistSrcProto);
  *redistSource = redistSrcProto;
  return rc;
}

L7_RC_t
snmpAgentRipRedistributeModeGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val = L7_NULL;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  rc = usmDbRip2RouteRedistributeModeGet(USMDB_UNIT_CURRENT,redistSource,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE :
      *val = D_agentRipRouteRedistMode_enable;
      break;

    case L7_FALSE :
      *val = D_agentRipRouteRedistMode_disable;
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
snmpAgentRipRedistributeSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = L7_NULL;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  switch (val)
  {
  case D_agentRipRouteRedistMode_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentRipRouteRedistMode_disable:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRip2RouteRedistributeModeSet(USMDB_UNIT_CURRENT, redistSource, temp_val);
    
    if(val == D_agentRipRouteRedistMode_disable)
        usmDbRip2RouteRedistributeParmsClear(USMDB_UNIT_CURRENT,redistSource);
  }

  return rc;
}

L7_RC_t
snmpAgentRipRouteRedistributeMetricGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;
  
  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;
  

  rc = usmDbRip2RouteRedistributeMetricGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;

  else if (rc == L7_ERROR)
  {
    *val = 0;
    rc = L7_SUCCESS;
  }
   
  return rc;
}

L7_RC_t
snmpAgentRipRouteRedistributeMetricSet(L7_uint32 UnitIndex, L7_uint32 redistSource, 
                                       L7_uint32 mode, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
 
  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  rc = usmDbRip2RouteRedistributeMetricSet(USMDB_UNIT_CURRENT, redistSource,mode, val);
  return rc; 
}



L7_RC_t
snmpAgentRipRedistMetricConfiguredGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val = L7_NULL;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  rc = usmDbRip2RouteRedistributeMetricGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = D_agentRipRouteRedistMetricConfigured_true;

  else if (rc == L7_ERROR)
  {
    *val = D_agentRipRouteRedistMetricConfigured_false;
    rc = L7_SUCCESS;
  }
   
  return rc;
}


/* Setting of 'RedistMetricConfigured' will only support setting the MetricConfigured 
   parameter to 'false', so it 'll be used for unconfiguring the metric */
L7_RC_t
snmpAgentRipRedistMetricConfiguredSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  
  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  switch (val)
  {
    case D_agentRipRouteRedistMetricConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentRipRouteRedistMetricConfigured_false:
      rc = usmDbRip2RouteRedistributeMetricClear(USMDB_UNIT_CURRENT, redistSource);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }

    return rc;
}



L7_RC_t
snmpAgentRipRedistMatchInternalGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;

  L7_uint32 temp_val;


  if(redistSource != D_agentRipRouteRedistSource_ospf)
    {
      *val =  D_agentRipRouteRedistMatchInternal_not_applicable;
      rc = L7_SUCCESS;
  }
  else {
      
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */
    rc = usmDbRip2RouteRedistributeMatchTypeGet(USMDB_UNIT_CURRENT,++redistSource,
                                                &temp_val);
    /* If the matchType is not configured, show it as 'false' */
    if(rc == L7_ERROR)
    {
       *val = D_agentRipRouteRedistMatchInternal_false;
       return L7_SUCCESS;
    }

    /* Doing bitwise operation on temp_val to check, whether MatchInternal is true or false */
    if(L7_OSPF_METRIC_TYPE_INTERNAL == (L7_OSPF_METRIC_TYPE_INTERNAL & temp_val))
        *val = D_agentRipRouteRedistMatchInternal_true;
    else
        *val = D_agentRipRouteRedistMatchInternal_false;
  }
  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchInternalSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  if(redistSource != D_agentRipRouteRedistSource_ospf)
      rc =  L7_FAILURE;

  switch (val)
  {
  case D_agentRipRouteRedistMatchInternal_true:
    temp_val = L7_ENABLE;
    break;

  case D_agentRipRouteRedistMatchInternal_false:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS )
  {
    /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
       matches REDIST_RT_OSPF(4) */
    rc = usmDbRip2RouteRedistributeMatchTypeSet(USMDB_UNIT_CURRENT, 
                                                ++redistSource, temp_val,
                                                L7_OSPF_METRIC_TYPE_INTERNAL);
  }

  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchExternal1Get(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;

  L7_uint32 temp_val;
    

  if(redistSource != D_agentRipRouteRedistSource_ospf)
    {
      *val =  D_agentRipRouteRedistMatchExternal1_not_applicable;
      rc = L7_SUCCESS;
  }
  else {
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */ 
    rc = usmDbRip2RouteRedistributeMatchTypeGet(USMDB_UNIT_CURRENT,++redistSource,
                                                &temp_val);
    
    /* If the matchType is not configured, show it as 'false' */
    if(rc == L7_ERROR)
    {
      *val = D_agentRipRouteRedistMatchInternal_false;
      return L7_SUCCESS;
    }


    /* Doing bitwise operation on temp_val to check, whether MatchExternal1 is true or false */
    if(L7_OSPF_METRIC_TYPE_EXT1 == (L7_OSPF_METRIC_TYPE_EXT1 & temp_val))
        *val = D_agentRipRouteRedistMatchExternal1_true;
    else
        *val = D_agentRipRouteRedistMatchExternal1_false;
  }
  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchExternal1Set(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  if(redistSource != D_agentRipRouteRedistSource_ospf)
      rc =  L7_FAILURE;

  switch (val)
  {
  case D_agentRipRouteRedistMatchExternal1_true:
    temp_val = L7_ENABLE;
    break;

  case D_agentRipRouteRedistMatchExternal1_false:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS )
  {
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */
    rc = usmDbRip2RouteRedistributeMatchTypeSet(USMDB_UNIT_CURRENT, 
                                                ++redistSource, temp_val,
                                                L7_OSPF_METRIC_TYPE_EXT1);
  }

  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchExternal2Get(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;

  L7_uint32 temp_val;

  if(redistSource != D_agentRipRouteRedistSource_ospf)
    {
      *val =  D_agentRipRouteRedistMatchExternal2_not_applicable;
      rc = L7_SUCCESS;
  }
  else {
      
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */
    rc = usmDbRip2RouteRedistributeMatchTypeGet(USMDB_UNIT_CURRENT,++redistSource,
                                                &temp_val);

    /* If the matchType is not configured, show it as 'false' */
    if(rc == L7_ERROR)
    {
      *val = D_agentRipRouteRedistMatchInternal_false;
      return L7_SUCCESS;
    }

    /* Doing bitwise operation on temp_val to check, whether MatchExternal2 is true or false */
    if(L7_OSPF_METRIC_TYPE_EXT2 == (L7_OSPF_METRIC_TYPE_EXT2 & temp_val))
        *val = D_agentRipRouteRedistMatchExternal2_true;
    else
        *val = D_agentRipRouteRedistMatchExternal2_false;
  }
  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchExternal2Set(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  if(redistSource != D_agentRipRouteRedistSource_ospf)
      rc =  L7_FAILURE;

  switch (val)
  {
  case D_agentRipRouteRedistMatchExternal2_true:
    temp_val = L7_ENABLE;
    break;

  case D_agentRipRouteRedistMatchExternal2_false:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS )
  {
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */ 
    rc = usmDbRip2RouteRedistributeMatchTypeSet(USMDB_UNIT_CURRENT, 
                                                ++redistSource, temp_val,
                                                L7_OSPF_METRIC_TYPE_EXT2);
  }

  return rc;
}

L7_RC_t
snmpAgentRipRedistMatchNSSAExternal1Get(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;

  L7_uint32 temp_val;

  if(redistSource != D_agentRipRouteRedistSource_ospf)
    {
      *val =  D_agentRipRouteRedistMatchNSSAExternal1_not_applicable;
      rc = L7_SUCCESS;
  }
  else {
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */
    rc = usmDbRip2RouteRedistributeMatchTypeGet(USMDB_UNIT_CURRENT,++redistSource,
                                                &temp_val);

    /* If the matchType is not configured, show it as 'false' */
    if(rc == L7_ERROR)
    {
      *val = D_agentRipRouteRedistMatchInternal_false;
      return L7_SUCCESS;
    }


    /* Doing bitwise operation on temp_val to check, whether MatchNSSAExternal1 is true or false */
    if(L7_OSPF_METRIC_TYPE_NSSA_EXT1 == (L7_OSPF_METRIC_TYPE_NSSA_EXT1 & temp_val))
        *val = D_agentRipRouteRedistMatchNSSAExternal1_true;
    else
        *val = D_agentRipRouteRedistMatchNSSAExternal1_false;
  }
  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchNSSAExternal1Set(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  if(redistSource != D_agentRipRouteRedistSource_ospf)
      rc =  L7_FAILURE;

  switch (val)
  {
  case D_agentRipRouteRedistMatchNSSAExternal1_true:
    temp_val = L7_ENABLE;
    break;

  case D_agentRipRouteRedistMatchNSSAExternal1_false:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS )
  {
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */ 
    rc = usmDbRip2RouteRedistributeMatchTypeSet(USMDB_UNIT_CURRENT, 
                                                ++redistSource, temp_val,
                                                L7_OSPF_METRIC_TYPE_NSSA_EXT1);
  }

  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchNSSAExternal2Get(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc;

  L7_uint32 temp_val;


  if(redistSource != D_agentRipRouteRedistSource_ospf)
    {
      *val =  D_agentRipRouteRedistMatchNSSAExternal2_not_applicable;
      rc = L7_SUCCESS;
  }
  else { 
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */ 
    rc = usmDbRip2RouteRedistributeMatchTypeGet(USMDB_UNIT_CURRENT,++redistSource,
                                                &temp_val);


    /* If the matchType is not configured, show it as 'false' */
    if(rc == L7_ERROR)
    {
      *val = D_agentRipRouteRedistMatchInternal_false;
      return L7_SUCCESS;
    }

    /* Doing bitwise operation on temp_val to check, whether MatchNSSAExternal2 is true or false */
    if(L7_OSPF_METRIC_TYPE_NSSA_EXT2 == (L7_OSPF_METRIC_TYPE_NSSA_EXT2 & temp_val))
        *val = D_agentRipRouteRedistMatchNSSAExternal2_true;
    else
        *val = D_agentRipRouteRedistMatchNSSAExternal2_false;
  }
  return rc;
}


L7_RC_t
snmpAgentRipRedistMatchNSSAExternal2Set(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;


  if(redistSource != D_agentRipRouteRedistSource_ospf)
      rc =  L7_FAILURE;

  switch (val)
  {
  case D_agentRipRouteRedistMatchNSSAExternal2_true:
    temp_val = L7_ENABLE;
    break;

  case D_agentRipRouteRedistMatchNSSAExternal2_false:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS )
  {
      /* Increasing redistSource by 1, s.t.,   D_agentRipRouteRedistSource_ospf(3) 
         matches REDIST_RT_OSPF(4) */
    rc = usmDbRip2RouteRedistributeMatchTypeSet(USMDB_UNIT_CURRENT, 
                                                ++redistSource, temp_val,
                                                L7_OSPF_METRIC_TYPE_NSSA_EXT2);
  }

  return rc;
}

L7_RC_t
snmpAgentRipRouteRedistributeFilterGet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 *val)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 temp_val;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  rc = usmDbRip2RouteRedistributeFilterGet(USMDB_UNIT_CURRENT, redistSource, &temp_val);

  if (rc == L7_SUCCESS)
     *val = temp_val;
  else
    {
      *val = 0;
      rc = L7_SUCCESS;
    }
      
  
  return rc;
}

L7_RC_t
snmpAgentRipRouteRedistributeFilterSet(L7_uint32 UnitIndex, L7_uint32 redistSource, 
                                       L7_uint32 mode, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  rc = usmDbRip2RouteRedistributeFilterSet(USMDB_UNIT_CURRENT, redistSource, mode, val);
  return rc; 
}


L7_RC_t
snmpRipRouteRedistDistListConfiguredGet(L7_uint32 UnitIndex, L7_uint32 redistSource, 
                                        L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  rc = usmDbRip2RouteRedistributeFilterGet(USMDB_UNIT_CURRENT, redistSource, 
                                           &temp_val);

  if (rc == L7_SUCCESS)
     *val = D_agentRipRouteRedistDistListConfigured_true;

  else if (rc == L7_ERROR)
  {
    *val = D_agentRipRouteRedistDistListConfigured_false;
    rc = L7_SUCCESS;
  }
   
  return rc;
}


/* Setting of 'RedistDistListConfigured' will only support setting the DistListConfigured 
   parameter to 'false', so it 'll be used for unconfiguring the access-list */
L7_RC_t
snmpRipRouteRedistDistListConfiguredSet(L7_uint32 UnitIndex, L7_uint32 redistSource, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  if( (redistSource ==   D_agentRipRouteRedistSource_ospf) ||
      (redistSource ==   D_agentRipRouteRedistSource_bgp))
      redistSource++;

  switch (val)
  {
    case D_agentRipRouteRedistDistListConfigured_true:
      rc = L7_FAILURE; /* setting to 'true' is not supported */
      break;

    case D_agentRipRouteRedistDistListConfigured_false:
      rc = usmDbRip2RRouteRedistributeFilterClear(USMDB_UNIT_CURRENT, redistSource);
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }

    return rc;
}


