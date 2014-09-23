/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmp_api.c
*
* Purpose: API interface for SNMP Agent
*
* Created by: Colin Verne 02/16/2001
*
* Component: SNMP
*
*********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "dtlapi.h"
#include "snmp_exports.h"
#include "defaultconfig.h"
#include "snmpconfig.h"
#include "snmpconfigv3.h"
#include "snmptrap.h"
#include "basetrap.h"
#include "snmp_api.h"
#include "snmp_util.h"
#include "l7_getvar.h"
#include "snmp_trap_api.h"
#include "osapi_support.h"

extern snmpState_t		   snmpState;
extern snmpCfgData_t snmpCfgData;
extern snmpSupportedMib_t supportedMibs[L7_SNMP_SUPPORTED_MIB_COUNT];
static L7_uint32 supportedMibsLastChanged = 0;

L7_uint32 SnmpEnable = L7_DISABLE;
L7_uint32 SnmpResetConfig = L7_DISABLE;

static L7_uint32 SnmpTrapsFlag = FD_NIM_TRAP_STATE;



/*********************************************************************
*
* @purpose  Sets the current SNMP state
*
* @returns  nothing
*
* @end
*********************************************************************/
void SnmpSetState(snmpState_t state)
{
  snmpState = state;

  if (state >= L7_SNMP_STATE_CONFIG)
    SnmpInitialize();
}


/*********************************************************************
*
* @purpose  Waits until the SNMP component is in the specified state
*
* @returns  nothing
*
* @end
*********************************************************************/
void SnmpWaitUntilState(snmpState_t state)
{
  if (snmpState < L7_SNMP_STATE_LAST)
  {
    while (snmpState < state)
    {
      osapiSleep(1);
    }
  }
}

/*********************************************************************
*
* @purpose  Waits until the SNMP component is in the RUN state
*
* @returns  nothing
*
* @notes    This should be called by all snmp tasks before any USMDB calls are
*           issued.
* @end
*********************************************************************/
void SnmpWaitUntilRun()
{
  SnmpWaitUntilState(L7_SNMP_STATE_RUN);
}

/*********************************************************************
*
* @purpose  Initialize SNMP agent communities and trap receivers.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    This should only be called once per bootup, and only by
* @notes    the SNMP agent after it's completed it's initialization.
*
* @end
*********************************************************************/
L7_RC_t
SnmpInitialize()
{
  if ( SnmpEnable != L7_ENABLE )
  {
    /* allow configuration to procede */
    SnmpEnable = L7_ENABLE;

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SNMP Agent Table Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Agent Community Table
*
* @param    index    Index of community to add/set
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpCommunityTableSet(L7_uint32 index)
{
  L7_RC_t rc;

  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index < 0 || index >= L7_MAX_SNMP_COMM )
    return L7_FAILURE;

  if ( snmpCfgData.snmpComm[index].agentCommunityStatus == L7_SNMP_COMMUNITY_STATUS_DELETE )
    return L7_FAILURE;

  /* ignore if in config mode */
  if ( snmpCfgData.snmpComm[index].agentCommunityStatus == L7_SNMP_COMMUNITY_STATUS_CONFIG )
    return L7_SUCCESS;

  (void)snmpSemaTake();
  rc = CommunityTableSet(index, 
                         snmpCfgData.snmpComm[index].agentCommunityName, 
                         snmpCfgData.snmpComm[index].agentCommunityAccessLevel, 
                         snmpCfgData.snmpComm[index].agentCommunityClientIpAddr,
                         snmpCfgData.snmpComm[index].agentCommunityClientIpMask,
                         snmpCfgData.snmpComm[index].agentCommunityStatus);
  (void)snmpSemaGive();

  return rc;
}


/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Agent Trap Receiver table
*
* @param    index    Index of Trap Receiver to add/set
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapReceiverTableSet ( L7_uint32 index )
{
  L7_RC_t rc;
  L7_sockaddr_union_t addr;

  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index < 0 || index >= L7_MAX_SNMP_COMM )
    return L7_FAILURE;

  if ( snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus == L7_SNMP_TRAP_MGR_STATUS_DELETE )
    return L7_FAILURE;

  /* ignore if in config mode */
  if ( snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus == L7_SNMP_TRAP_MGR_STATUS_CONFIG )
    return L7_SUCCESS;

  if (!L7_IP6_IS_ADDR_UNSPECIFIED(&snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr))
  {
    addr.u.sa6.sin6_family = L7_AF_INET6;
    memcpy(addr.u.sa6.sin6_addr.in6.addr8, &snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr,
           sizeof(L7_in6_addr_t));
  }
  else
  {
    addr.u.sa4.sin_family = L7_AF_INET;
    addr.u.sa4.sin_addr.s_addr = osapiHtonl( snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddr);
  }

  (void)snmpSemaTake();
  rc = TrapReceiverTableSet(index,
                            snmpCfgData.snmpTrapMgr[index].agentTrapMgrCommunityName,
                            &addr,
                            snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus,
                            snmpCfgData.snmpTrapMgr[index].agentTrapMgrVersion);
  (void)snmpSemaGive();

  return rc;
}

/* Begin Function Declarations: c:\work\esa201\lvl7dev\src\l7public\api\snmp_api.h */

/*********************************************************************
*
* @purpose  Create/Update all present Communities in the SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpCommunityTableUpdate()
{
  L7_uint32 index;

  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  for ( index=0; index < L7_MAX_SNMP_COMM; index++ )
  {
    if ( snmpCfgData.snmpComm[index].agentCommunityStatus != L7_SNMP_COMMUNITY_STATUS_DELETE )
    {
      if ( SnmpCommunityTableSet(index) == L7_FAILURE )
        return L7_FAILURE;
    }
    else
    {
      SnmpCommunityTableDelete(index);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deletes an entry in the SNMP Agent Community Table
*
* @param    index    Index of community to add/set
* @param    comm     SNMP Community structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCommunityTableDelete(L7_uint32 index)
{
  L7_RC_t rc;
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index < 0 || index >= L7_MAX_SNMP_COMM )
    return L7_FAILURE;

  (void)snmpSemaTake();
  rc = CommunityTableDelete(index, snmpCfgData.snmpComm[index].agentCommunityName);
  (void)snmpSemaGive();
  return rc;
}

/*********************************************************************
*
* @purpose  Create/Update all present Trap Receivers in the SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapMgrTableUpdate()
{
  L7_uint32 index;

  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  for ( index=0; index < L7_MAX_SNMP_COMM; index++ )
  {
    if ( snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus != L7_AGENT_TRAP_MGR_STATUS_DELETE )
    {
      if ( SnmpTrapReceiverTableSet(index) == L7_FAILURE )
        return L7_FAILURE;
    }
    else
    {
      SnmpTrapReceiverTableDelete(index);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Agent Trap Receiver table
*
* @param    index    Index of Trap Receiver to add/set
* @param    trap     SNMP Trap Receiver structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapReceiverTableDelete(L7_uint32 index)
{
  L7_RC_t rc;
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index < 0 || index >= L7_MAX_SNMP_COMM )
    return L7_FAILURE;

  (void)snmpSemaTake();
  rc = TrapReceiverTableDelete(index);
  (void)snmpSemaGive();
  return rc;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Community Table Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************************
* @purpose  Returns table index of SNMP community name   
*
* @param    commName    pointer to Community name   
* @param    index       pointer to table index   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIndexGet(L7_char8 *commName, L7_uint32 *index)
{
  L7_uint32 i, length;

  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  length = strlen(commName);

  for ( i=0; i < L7_MAX_SNMP_COMM; i++ )
  {
    if ( length == strlen(snmpCfgData.snmpComm[i].agentCommunityName) )
      if ( 0 == memcmp (commName, snmpCfgData.snmpComm[i].agentCommunityName, length) )
        break;
  }

  if ( i == L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *index = i;

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Returns SNMP community name per index  
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityNameGet(L7_uint32 index, L7_char8 *commName)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  strncpy(commName, snmpCfgData.snmpComm[index].agentCommunityName, L7_SNMP_NAME_SIZE+1);

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Sets a SNMP community name per index if not a duplicate 
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityNameSet(L7_uint32 index, L7_char8 *commName)
{
  L7_uint32 temp_index;

  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpComm[index].agentCommunityStatus != L7_SNMP_COMMUNITY_STATUS_CONFIG)
    return(L7_FAILURE);

  if (SnmpCommunityIndexGet(commName, &temp_index) == L7_SUCCESS && temp_index != index)
    return(L7_ALREADY_CONFIGURED);

  bzero( snmpCfgData.snmpComm[index].agentCommunityName, L7_SNMP_NAME_SIZE+1 );
  strncpy( snmpCfgData.snmpComm[index].agentCommunityName, commName, L7_SNMP_NAME_SIZE);

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Return a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      pointer to IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpAddrGet(L7_uint32 index, L7_uint32 *ipAddr)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *ipAddr = snmpCfgData.snmpComm[index].agentCommunityClientIpAddr;

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Sets a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpAddrSet(L7_uint32 index, L7_uint32 ipAddr)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpComm[index].agentCommunityStatus != L7_SNMP_COMMUNITY_STATUS_CONFIG)
    return(L7_FAILURE);

  snmpCfgData.snmpComm[index].agentCommunityClientIpAddr = ipAddr;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

  return SnmpCommunityTableSet(index);
}


/*********************************************************************
* @purpose  Return a SNMP IP mask per index  
*
* @param    index       table index   
* @param    ipMask      pointer to IP mask   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpMaskGet(L7_uint32 index, L7_uint32 *ipMask)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *ipMask = snmpCfgData.snmpComm[index].agentCommunityClientIpMask;

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Sets a SNMP IP mask per index  
*
* @param    index       table index   
* @param    ipMask      IP mask   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpMaskSet(L7_uint32 index, L7_uint32 ipMask)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpComm[index].agentCommunityStatus != L7_SNMP_COMMUNITY_STATUS_CONFIG)
    return(L7_FAILURE);

  snmpCfgData.snmpComm[index].agentCommunityClientIpMask = ipMask;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

  return SnmpCommunityTableSet(index);
}


/*********************************************************************
* @purpose  Return a SNMP access level per index  
*
* @param    index    table index   
* @param    level    pointer to access level   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityAccessLevelGet(L7_uint32 index, L7_uint32 *level)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *level = snmpCfgData.snmpComm[index].agentCommunityAccessLevel;

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Sets a SNMP access level per index  
*
* @param    index    table index   
* @param    level    access level   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityAccessLevelSet(L7_uint32 index, L7_uint32 level)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpComm[index].agentCommunityStatus != L7_SNMP_COMMUNITY_STATUS_CONFIG)
    return(L7_FAILURE);

  snmpCfgData.snmpComm[index].agentCommunityAccessLevel = level;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

  return SnmpCommunityTableSet(index);
}


/*********************************************************************
* @purpose  Return a SNMP access status per index  
*
* @param    index    table index   
* @param    status   pointer to access statu 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityStatusGet(L7_uint32 index, L7_uint32 *status)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *status = snmpCfgData.snmpComm[index].agentCommunityStatus;

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Sets a SNMP access status per index  
*
* @param    index    table index   
* @param    status   access status   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityStatusSet(L7_uint32 index, L7_uint32 status)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if ( snmpCfgData.snmpComm[index].agentCommunityStatus == L7_SNMP_COMMUNITY_STATUS_DELETE &&
       status != L7_SNMP_COMMUNITY_STATUS_CONFIG)
    return(L7_FAILURE);

  if ( status != L7_SNMP_COMMUNITY_STATUS_VALID )
  {
    /* set disabled first */
    snmpCfgData.snmpComm[index].agentCommunityStatus = L7_SNMP_COMMUNITY_STATUS_INVALID;
    SnmpCommunityTableSet(index);
  }

  if ( status == L7_SNMP_COMMUNITY_STATUS_DELETE )
  {
    /* clear configuration for this entry */
    snmpCfgData.snmpComm[index].agentCommunityStatus = status;
    memset( snmpCfgData.snmpComm[index].agentCommunityName, 0, L7_SNMP_NAME_SIZE );
    memset( snmpCfgData.snmpComm[index].agentCommunityOwner, 0, L7_SNMP_NAME_SIZE );
    snmpCfgData.snmpComm[index].agentCommunityClientIpAddr = 0;
    snmpCfgData.snmpComm[index].agentCommunityClientIpMask = 0;
    snmpCfgData.snmpComm[index].agentCommunityAccessLevel = L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY; 

    SnmpCommunityTableDelete(index);
    snmpCfgData.cfgHdr.dataChanged = L7_TRUE; 
    return L7_SUCCESS;
  }
  else
  {
    snmpCfgData.snmpComm[index].agentCommunityStatus = status;

    snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

    return SnmpCommunityTableSet(index);
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Return a SNMP access owner per index  
*
* @param    index    table index   
* @param    owner    pointer to access owner   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityOwnerGet(L7_uint32 index, L7_char8 *owner)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  memcpy(owner, snmpCfgData.snmpComm[index].agentCommunityOwner, L7_SNMP_NAME_SIZE);

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Sets a SNMP access owner per index  
*
* @param    index    table index   
* @param    owner    pointer to access owner   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityOwnerSet(L7_uint32 index, L7_char8 *owner)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpComm[index].agentCommunityStatus != L7_SNMP_COMMUNITY_STATUS_CONFIG)
    return(L7_FAILURE);

  bzero(snmpCfgData.snmpComm[index].agentCommunityOwner, L7_SNMP_NAME_SIZE+1);

  if ( strlen(owner) <= L7_SNMP_NAME_SIZE )
  {
    strcpy( snmpCfgData.snmpComm[index].agentCommunityOwner, owner );
  }
  else
  {
    strncpy( snmpCfgData.snmpComm[index].agentCommunityOwner, owner, L7_SNMP_NAME_SIZE);
  }

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

  return SnmpCommunityTableSet(index);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Trap Receiver Table Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************************
* @purpose  Returns SNMP Trap Receiver community name per index  
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrCommunityGet(L7_uint32 index, L7_char8 *commName)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  strncpy(commName, snmpCfgData.snmpTrapMgr[index].agentTrapMgrCommunityName, L7_SNMP_NAME_SIZE+1);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets a SNMP Trap Receiver community name by index
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrCommunitySet(L7_uint32 index, L7_char8 *commName)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus != L7_SNMP_TRAP_MGR_STATUS_CONFIG)
    return(L7_FAILURE);

  bzero( snmpCfgData.snmpTrapMgr[index].agentTrapMgrCommunityName, L7_SNMP_NAME_SIZE+1);
  strncpy( snmpCfgData.snmpTrapMgr[index].agentTrapMgrCommunityName, commName, L7_SNMP_NAME_SIZE);

  if ( snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus == L7_SNMP_TRAP_MGR_STATUS_DELETE )
    snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus = L7_SNMP_TRAP_MGR_STATUS_INVALID;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;

  return SnmpTrapReceiverTableSet(index);
}

/*********************************************************************
* @purpose  Return a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      pointer to IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIpAddrGet(L7_uint32 index, L7_uint32 *ipAddr)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *ipAddr = snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddr;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Return a SNMP IPv6 addr per index  
*
* @param    index       table index   
* @param    ipv6Addr    pointer to IPv6 address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIPv6AddrGet(L7_uint32 index, L7_in6_addr_t *ipv6Addr)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  memcpy(ipv6Addr, &snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr,
         sizeof(L7_in6_addr_t));

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIpAddrSet(L7_uint32 index, L7_uint32 ipAddr)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus != L7_SNMP_TRAP_MGR_STATUS_CONFIG)
    return(L7_FAILURE);

  /* Set IPv4 address and clear IPv6 definition. */
  snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddr = ipAddr;
  memset(&snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr, 0,
         sizeof(L7_in6_addr_t));

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
  return SnmpTrapReceiverTableSet(index);
}

/*********************************************************************
* @purpose  Sets a SNMP IPv6 addr per index  
*
* @param    index       table index   
* @param    ipv6Addr    IPv6 address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIPv6AddrSet(L7_uint32 index, L7_in6_addr_t *ipv6Addr)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus != L7_SNMP_TRAP_MGR_STATUS_CONFIG)
    return(L7_FAILURE);

  /* Set IPv6 address and clear IPv4 definition. */
  memcpy(&snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr, ipv6Addr,
         sizeof(L7_in6_addr_t));
  snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddr = 0;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
  return SnmpTrapReceiverTableSet(index);
}

/*********************************************************************
* @purpose  Return a SNMP IP addr type per index
*
* @param    index       table index
* @param    ipAddrType    pointer to IP address type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapMgrIpAddrTypeGet(L7_uint32 index, L7_uint32 *ipAddrType)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *ipAddrType = snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddrType;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets a SNMP IP addr type per index
*
* @param    index       table index
* @param    ipAddrtype      IP address type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapMgrIpAddrTypeSet(L7_uint32 index, L7_uint32 ipAddrtype)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  /* Set Address type  */
  snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddrType = ipAddrtype;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
  return SnmpTrapReceiverTableSet(index);
}

/*********************************************************************
* @purpose  Return a SNMP Trap Manager status per index  
*
* @param    index    table index   
* @param    status   pointer to trap manager status entry  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrStatusGet(L7_uint32 index, L7_uint32 *status)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *status = snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets a SNMP Trap Manager per index  
*
* @param    index    table index   
* @param    status   trap manager status entry  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrStatusSet(L7_uint32 index, L7_uint32 status)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if ( status == snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus)
  {
    return L7_SUCCESS;
  }
  else if ( status == L7_SNMP_TRAP_MGR_STATUS_DELETE )
  {
    snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus = status;
    memset( snmpCfgData.snmpTrapMgr[index].agentTrapMgrCommunityName, 0, L7_SNMP_NAME_SIZE );
    snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddr = 0;
    memset(&snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr, 0, sizeof(L7_in6_addr_t));
    snmpCfgData.snmpTrapMgr[index].agentTrapMgrVersion = FD_SNMP_DEFAULT_TRAP_VERSION;

    snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
    SnmpTrapReceiverTableDelete(index);
    return L7_SUCCESS;
  }
/* @p0865 start */
  else if ( status != L7_SNMP_TRAP_MGR_STATUS_CONFIG &&
            snmpCfgData.snmpTrapMgr[index].agentTrapMgrIpAddr == 0 &&
            L7_IP6_IS_ADDR_UNSPECIFIED(&snmpCfgData.snmpTrapMgr[index].agentTrapMgrIPv6Addr))
  {
    return L7_FAILURE;
  }
/* @p0865 end */
  else
  {
    snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus = status;

    snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
    return SnmpTrapReceiverTableSet(index);
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Return a SNMP Version per index  
*
* @param    index       table index   
* @param    version     pointer to SNMP Version
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrVersionGet(L7_uint32 index, snmpTrapVer_t *version)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  *version = snmpCfgData.snmpTrapMgr[index].agentTrapMgrVersion;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets a SNMP IP addr per index  
*
* @param    index       table index   
* @param    version     SNMP Version
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrVersionSet(L7_uint32 index, snmpTrapVer_t version)
{
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  if ( index >= L7_MAX_SNMP_COMM )
    return(L7_FAILURE);

  if (snmpCfgData.snmpTrapMgr[index].agentTrapMgrStatus != L7_SNMP_TRAP_MGR_STATUS_CONFIG)
    return(L7_FAILURE);

  /* check that only v1 or v2c traps are selected */
  if ((version != L7_SNMP_TRAP_VER_SNMPV1) && (version != L7_SNMP_TRAP_VER_SNMPV2C))
    return(L7_FAILURE);

  snmpCfgData.snmpTrapMgr[index].agentTrapMgrVersion = version;

  snmpCfgData.cfgHdr.dataChanged = L7_TRUE;
  return SnmpTrapReceiverTableSet(index);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * User Table Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************************
* @purpose  Sets/Creates an SNMP User
*
* @param    userName        pointer to user name @b((input))
* @param    accessLevel     L7_SNMP_USER_ACCESS_LEVEL_t @b((input))
* @param    authProtocol    L7_SNMP_USER_AUTH_PROTO_t @b((input))
* @param    privProtocol    L7_SNMP_USER_PRIV_PROTO_t @b((input))
* @param    authKey         pointer to user's authentication password @b((input))
* @param    privKey         pointer to user's privacy password @b((input))
*
* @param    restore     L7_TRUE - Restore of the users from CLI, so shouldn't
*                       overwrite existing SNMP users
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpUserSet(L7_uchar8 *userName, L7_uint32 accessLevel, L7_uint32 authProtocol, L7_uint32 privProtocol, L7_uchar8 *authKey, L7_uchar8 *privKey, L7_BOOL restore)
{
  L7_RC_t rc;
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;


  (void)snmpSemaTake();
  rc = UserTableSet(userName, accessLevel, authProtocol, privProtocol, authKey, privKey, restore);
  (void)snmpSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Deletes an SNMP User
*
* @param    userName        pointer to user name @b((input))
* @param    accessLevel     L7_SNMP_USER_ACCESS_LEVEL_t @b((input))
* @param    authProtocol    L7_SNMP_USER_AUTH_PROTO_t @b((input))
* @param    privProtocol    L7_SNMP_USER_PRIV_PROTO_t @b((input))
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpUserDelete(L7_uchar8 *userName, L7_uint32 accessLevel, L7_uint32 authProtocol, L7_uint32 privProtocol)
{
  L7_RC_t rc;
  if ( SnmpEnable != L7_ENABLE )
    return L7_FAILURE;

  (void)snmpSemaTake();
  rc = UserTableDelete(userName, accessLevel, authProtocol, privProtocol);
  (void)snmpSemaGive();
  return rc;
}

/*********************************************************************
*
* @purpose  Returns status of encryption code in the SNMPv3 User Table
*
* @param    void
*
* @returns  L7_TRUE     if encryption code is present
* @returns  L7_FALSE    if not
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL
SnmpUserTableHasEncryption()
{
  return UserTableHasEncryption();
}

/*********************************************************************
* @purpose  Gets the current status of SNMP
*
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32
SnmpStatusGet()
{
  return SnmpEnable;
}


/*********************************************************************
* @purpose  Retrieve an unsigned integer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *val          unsigned integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an unsigned integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
SnmpGetVarUInt( L7_char8 *oid_buf, L7_uint32 *val)
{
  if (SnmpEnable != L7_ENABLE)
    return L7_FAILURE;

  return L7GetVarUInt(oid_buf, val);
}

/*********************************************************************
* @purpose  Retrieve a signed integer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *val          signed integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not a signed integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
SnmpGetVarSInt( L7_char8 *oid_buf, L7_int32 *val)
{
  if (SnmpEnable != L7_ENABLE)
    return L7_FAILURE;

  return L7GetVarSInt(oid_buf, val);
}

/*********************************************************************
* @purpose  Retrieve a character buffer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *buf          character buffer to place the value into
*           *buf_len      unsigned integer to place the buffer length into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an octet string
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
SnmpGetVarCharBuf( L7_char8 *oid_buf, L7_char8 *buf, L7_uint32 *buf_len)
{
  if (SnmpEnable != L7_ENABLE)
    return L7_FAILURE;

  return L7GetVarCharBuf(oid_buf, buf, buf_len);
}


/*********************************************************************
* @purpose  Checks to see if an entry in the Supported Mibs table exists
*
* @parms    index         index to check
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableEntryGet( L7_uint32 index )
{
  if ( index >= L7_SNMP_SUPPORTED_MIB_COUNT)
    return L7_FAILURE;

  if ( supportedMibs[index].mibStatus == L7_ENABLE)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Finds the next valid index of an entry in the Supported Mibs table
*
* @parms    *index        pointer to hold the next valid index
*
* @returns  L7_SUCCESS    if annother entry exists
*           L7_FAILURE    if there are no more entries
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableEntryNextGet( L7_uint32 *index )
{
  L7_uint32 mibIndex;

  for (mibIndex = *index; mibIndex < L7_SNMP_SUPPORTED_MIB_COUNT; mibIndex++)
  {
    if ( supportedMibs[mibIndex].mibStatus == L7_ENABLE)
    {
      *index = mibIndex;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Creates an entry in the Supported Mibs Table
*
* @parms    mibName         name of the mib (RFC number or short name)
*           mibDescription  string to describe the mib
*           mibOID          string buffer containing the OID to retreive
*
* @returns  L7_SUCCESS    if creation was successful
*           L7_FAILURE    if there are no empty positions available or
*                         if the provided strings are too long
*
* @notes    OID is in the form of "0.1.2.3.4.5"
* @notes    This function is used only by the SNMP Agent to add entries 
*           during initialization
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableEntryCreate( L7_char8 mibName[], L7_char8 mibDescription[], L7_char8 mibOID[] )
{
  L7_uint32 index;

  if (strlen(mibName) >= L7_SNMP_SUPPORTED_MIB_NAME_SIZE ||
      strlen(mibDescription) >= L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE ||
      strlen(mibOID) >= L7_SNMP_SUPPORTED_MIB_OID_SIZE)
    return L7_FAILURE;

  for (index = 1; index < L7_SNMP_SUPPORTED_MIB_COUNT; index++)
  {
    if ( supportedMibs[index].mibStatus == L7_DISABLE)
    {
      bzero(supportedMibs[index].mibName, L7_SNMP_SUPPORTED_MIB_NAME_SIZE);
      strncpy(supportedMibs[index].mibName, mibName, L7_SNMP_SUPPORTED_MIB_NAME_SIZE-1);
      
      bzero(supportedMibs[index].mibDescription, L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE);
      strncpy(supportedMibs[index].mibDescription, mibDescription, L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE-1);

      bzero(supportedMibs[index].mibOID, L7_SNMP_SUPPORTED_MIB_OID_SIZE);
      strncpy(supportedMibs[index].mibOID, mibOID, L7_SNMP_SUPPORTED_MIB_OID_SIZE-1);

      supportedMibs[index].mibEntryCreationTime = osapiUpTimeRaw() * 100; /* centiseconds */
      
      supportedMibs[index].mibStatus = L7_ENABLE;

      supportedMibsLastChanged = supportedMibs[index].mibEntryCreationTime;

      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the name of the Supported Mib entry
*
* @parms    index         index to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    buffer should be large enough to hold a string of size
*           L7_SNMP_SUPPORTED_MIB_NAME_SIZE-1
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibNameGet( L7_uint32 index, L7_char8 *buf)
{
  if ( SnmpSupportedMibTableEntryGet(index) == L7_SUCCESS)
  {
    strncpy(buf, supportedMibs[index].mibName, L7_SNMP_SUPPORTED_MIB_NAME_SIZE-1);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the description of the Supported Mib entry
*
* @parms    index         index to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    buffer should be large enough to hold a string of size
*           L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE-1
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibDescriptionGet( L7_uint32 index, L7_char8 *buf)
{
  if ( SnmpSupportedMibTableEntryGet(index) == L7_SUCCESS)
  {
    strncpy(buf, supportedMibs[index].mibDescription, L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE-1);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the OID of the Supported Mib entry
*
* @parms    index         index into supported MIBs table
* @parms    *buf          OID to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    buffer should be large enough to hold a string of size
*           L7_SNMP_SUPPORTED_MIB_OID_SIZE-1
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibOIDGet(L7_uint32 index, L7_char8 *buf)
{
  if ( SnmpSupportedMibTableEntryGet(index) == L7_SUCCESS)
  {
    strncpy(buf, supportedMibs[index].mibOID, L7_SNMP_SUPPORTED_MIB_OID_SIZE-1);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the entry creation time of the Supported Mib entry
*
* @parms    index         index into supported MIBs table
* @parms    *createTime   creation time to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibEntryCreationTimeGet(L7_uint32 index, L7_uint32 *createTime)
{
  if ( SnmpSupportedMibTableEntryGet(index) == L7_SUCCESS)
  {
    *createTime = supportedMibs[index].mibEntryCreationTime;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the time of the last changed to the Supported Mib table
*
* @parms    *lastChangeTime       last change time to return
*
* @returns  L7_SUCCESS    
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableLastChangeTimeGet(L7_uint32 *lastChangeTime)
{
  *lastChangeTime = supportedMibsLastChanged;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the current status of Trap master flag
*
* @param    UnitIndex      The unit for this operation
* @param    val   Status of Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpEnableTrapsGet(L7_uint32 *val)
{
  *val = SnmpTrapsFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the current status of Trap master flag
*
* @param    L7_uint32 UnitIndex      The unit for this operation
* @param    val   Status of Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpEnableTrapsSet(L7_uint32 val)
{
  if (val == L7_ENABLE || val == L7_DISABLE)
  {
    SnmpTrapsFlag = val;
    return L7_SUCCESS;
  }
  return L7_ERROR;
}


