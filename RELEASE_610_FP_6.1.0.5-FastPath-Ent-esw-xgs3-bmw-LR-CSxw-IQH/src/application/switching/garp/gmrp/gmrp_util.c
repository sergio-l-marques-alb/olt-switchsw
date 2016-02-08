/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    gmrp_util.c
* @purpose     GMRP Utility
* @component   GMRP-GARP
* @comments    none
* @create      02/25/2002
* @author      Shekhar Kalyanam
* @end
*             
**********************************************************************/
/*************************************************************

*************************************************************/


#include "string.h"
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "defaultconfig.h"
#include "garp.h"
#include "garpcfg.h"
#include "gidapi.h"
#include "gipapi.h"
#include "gmrp_api.h"
#include "gmrapi.h"
#include "garpapi.h"
#include "gmd.h"
#include "dot1q_api.h"
#include "dot1s_api.h"
#include "usmdb_util_api.h"

#include "garp_dot1q_api.h"

extern gmrpInstance_t *gmrpInstance;
/*********************************************************************
* @purpose  connects a port to all vlans gips that the port is a member of    
*
* @param    L7_uint32 port 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpConnectPort(L7_uint32 intIfNum)
{
  dot1qVidList_t *list;
  L7_uint32 numOfVlans;
  L7_uint32 i;
  L7_uint32 index;
  L7_uint32 instId = 0;
  L7_uint32 portState;
  GARPPort port;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 gmrpMode;

  /* if gmrp mode is disabled, we do not really care */
  if ((garpGMRPEnableGet(&gmrpMode) == L7_SUCCESS) && (gmrpMode==L7_FALSE))
    return L7_SUCCESS;

  unit = usmDbThisUnitGet();

  port = (GARPPort) intIfNum;
  
  /*get a list of vlans this port is a member of*/
  list = (dot1qVidList_t*)osapiMalloc( L7_GMRP_COMPONENT_ID, (sizeof(dot1qVidList_t)*(L7_MAX_VLANS+1)) );
  
  if (list == L7_NULLPTR)
    return L7_FAILURE;

  rc = dot1qVIDListGet(port,1,(L7_uint32 *)list,&numOfVlans);

  for(i=0; i<numOfVlans; i++)
  {
	if(gmrpInstanceIndexFind(list[i].vid,&index) != L7_SUCCESS)
	{
      osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
      return L7_FAILURE;
    }
    
	if (dot1sModeGet() == L7_TRUE)
	{
	  /*for mstp need to check if this port if forwarding 
        in the instance the particular vlan is associated with*/
      /*find the instance associated with this vlan*/
      instId = dot1sVlanToMstiGet(list[i].vid);
      /*if (instId == mstID)*/
	  {
	     portState = dot1sMstiPortStateGet(instId,intIfNum);
		 if (portState == L7_DOT1S_FORWARDING ||
			 portState == L7_DOT1S_MANUAL_FWD)
		 {
		   /*connect this in the ring only if this port is forwarding in this isntance*/
		   gip_connect_port_to_active_topology(&gmrpInstance[index].gmr->g, port);
		 }
	  }
	}
	else
      gip_connect_port_to_active_topology(&gmrpInstance[index].gmr->g, port);
    
  }
  
  osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disconnects a port from all vlans gips that it is a member of    
*
* @param    L7_uint32 port 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpDisconnectPort(L7_uint32 intIfNum)
{
  dot1qVidList_t *list;
  L7_uint32 numOfVlans;
  L7_uint32 i;
  L7_uint32 index;
  GARPPort port;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 gmrpMode;

  /* if gmrp mode is disabled, we do not really care */
  if ((garpGMRPEnableGet(&gmrpMode) == L7_SUCCESS) && (gmrpMode==L7_FALSE))
	return L7_SUCCESS;

  unit = usmDbThisUnitGet();
  
  port = (GARPPort) intIfNum;
  
  /*get a list of vlans this port is a member of*/
  list = (dot1qVidList_t*)osapiMalloc( L7_GMRP_COMPONENT_ID, (sizeof(dot1qVidList_t)*(L7_MAX_VLANS+1)) );
  
  if (list == L7_NULLPTR)
    return L7_FAILURE;
  
  rc = dot1qVIDListGet(port,1,(L7_uint32 *)list,&numOfVlans);

  for(i=0; i<numOfVlans; i++)
  {
	if(gmrpInstanceIndexFind(list[i].vid,&index) != L7_SUCCESS)
	{
      osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
      return L7_FAILURE;
    }
	gip_disconnect_port_from_active_tolpolgy(&gmrpInstance[index].gmr->g, port);

  }
  
  osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
  return L7_SUCCESS;	 
}

/*********************************************************************
* @purpose  connects a port to all vlans gips that the port is a member of    
*
* @param    L7_uint32 mstId
* @param	L7_uint32 intIfNum 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpDot1sConnectPort(L7_uint32 mstID, L7_uint32 intIfNum)
{
  dot1qVidList_t *list;
  L7_uint32 numOfVlans;
  L7_uint32 i;
  L7_uint32 index;
  GARPPort port;
  L7_uint32 instId = 0;
  L7_uint32 portState;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 gmrpMode;

  /* if gmrp mode is disabled, we do not really care */
  if ((garpGMRPEnableGet(&gmrpMode) == L7_SUCCESS) && (gmrpMode==L7_FALSE))
	return L7_SUCCESS;


  unit = usmDbThisUnitGet();

  port = (GARPPort) intIfNum;
  
  /*get a list of vlans this port is a member of*/
  list = (dot1qVidList_t*)osapiMalloc( L7_GMRP_COMPONENT_ID, (sizeof(dot1qVidList_t)*(L7_MAX_VLANS+1)) );
  
  if (list == L7_NULLPTR)
    return L7_FAILURE;
  
  rc = dot1qVIDListGet(port,1,(L7_uint32 *)list,&numOfVlans);

  for(i=0; i<numOfVlans; i++)
  {
	if(gmrpInstanceIndexFind(list[i].vid,&index) != L7_SUCCESS)
	{
      osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
      return L7_FAILURE;
    }

      /*for mstp need to check if this port if forwarding 
        in the instance the particular vlan is associated with*/
      /*find the instance associated with this vlan*/
      instId = dot1sVlanToMstiGet(list[i].vid);
      if (instId == mstID)
	  {
	     portState = dot1sMstiPortStateGet(instId,intIfNum);
		 if (portState == L7_DOT1S_FORWARDING ||
			 portState == L7_DOT1S_MANUAL_FWD)
		 {
		   /*connect this in the ring only if this port is forwarding in this isntance*/
		   gip_connect_port_to_active_topology(&gmrpInstance[index].gmr->g, port);
		 }
	  }
  }
  
  osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disconnects a port from all vlans gips that it is a member of    
*
* @param    L7_uint32 mstId
* @param	L7_uint32 intIfNum 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpDot1sDisconnectPort(L7_uint32 mstID, L7_uint32 intIfNum)
{
  dot1qVidList_t *list;
  L7_uint32 numOfVlans;
  L7_uint32 i, instanceId;
  L7_uint32 index;
  GARPPort port;
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 gmrpMode;

  /* if gmrp mode is disabled, we do not really care */
  if ((garpGMRPEnableGet(&gmrpMode) == L7_SUCCESS) && (gmrpMode==L7_FALSE))
	return L7_SUCCESS;

  unit = usmDbThisUnitGet();

  port = (GARPPort) intIfNum;
  
  /*get a list of vlans this port is a member of*/
  list = (dot1qVidList_t*)osapiMalloc( L7_GMRP_COMPONENT_ID, (sizeof(dot1qVidList_t)*(L7_MAX_VLANS+1)) );
  
  if (list == L7_NULLPTR)
    return L7_FAILURE;
  
  rc = dot1qVIDListGet(port,1,(L7_uint32 *)list,&numOfVlans);

  for(i=0; i<numOfVlans; i++)
  {
	if(gmrpInstanceIndexFind(list[i].vid,&index) != L7_SUCCESS)
	{
      osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
      return L7_FAILURE;
    }

	instanceId = dot1sVlanToMstiGet(list[i].vid);
	if (instanceId == mstID)
	  gip_disconnect_port_from_active_tolpolgy(&gmrpInstance[index].gmr->g, port);

  }

  osapiFree(L7_GMRP_COMPONENT_ID, (void*)list);
  return L7_SUCCESS;	 
}

