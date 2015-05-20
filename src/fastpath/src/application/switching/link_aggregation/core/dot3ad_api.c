/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename    dot3ad_api.c
 * @purpose     802.3ad link aggregation API implementation
 * @component   dot3ad
 * @comments    none
 * @create      05/30/2001
 * @author      djohnson
 * @end
 *             
 **********************************************************************/

#include "dot3ad_include.h"
#include "dot3ad_lac.h"
#include "dot3ad_api.h"

#include "simapi.h"
#include "dot3ad_exports.h"
#include "usmdb_nim_api.h"
#include "dot1ad_l2tp_api.h"
#include "cnfgr.h"


extern dot3ad_agg_t    dot3adAgg[L7_MAX_NUM_LAG_INTF];
extern L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
extern void * dot3adQueueSyncSema;

/* get the per interface statistic to reset the RxLACPDUsDropped everytime a member is added 
   to a static lag */
extern dot3ad_stats_t  dot3ad_stats[L7_MAX_INTERFACE_COUNT];
extern dot3ad_LagCnt_t dot3adLagCnt;
extern dot3adCfg_t dot3adCfg;    /* Configuration File Overlay */
extern dot3ad_system_t dot3adSystem; 



/* SNMP object get/sets */
/*********************************************************************
 * @purpose  Gets the most recent change to the dot3adAggTable,
 *           dot3adAggPortListTable or the dot3adAggPortTable
 *
 * @param    *time      time of last change
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adTablesLastChangedGet(L7_timespec *time)
{
  return tablesLastChangedGet(time);
}

/*********************************************************************
 * @purpose  Check the validity of an aggregator interface
 *
 * @param    intf       internal interface number
 *
 * @returns  L7_SUCCESS, if a the first was found
 * @returns  L7_FAILURE, if a the first doesn't exist
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggEntryGet(L7_uint32 intf)
{
  dot3ad_agg_t *p;

  p = dot3adAggIntfFind(intf);
  if (p != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Given an aggregator interface, the get the next one,
 *           if it exists
 *
 * @param    intf       internal interface number, start
 * @param    *next      internal interface number, next
 *
 * @returns  L7_SUCCESS, if a next was found
 * @returns  L7_FAILURE, if a next doesn't exist
 *
 * @notes   if the start doesnt exits, the closest next will be returned 
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggEntryGetNext(L7_uint32 intf, L7_uint32 *next)
{
  L7_uint32 i;
  dot3ad_agg_t *p;
  L7_uint32 highestIntfNumber;
  L7_RC_t rc;

  rc = nimGetHighestIntfNumber(&highestIntfNumber);

  for (i=intf+1;i<=highestIntfNumber;i++)
  {
    if (nimCheckIfNumber(i) != L7_SUCCESS) 
      continue;
    p = dot3adAggIntfFind(i);
    if (p != L7_NULLPTR)
    {
      /* if (p->ready == L7_TRUE) */
      *next = i;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Given LAG index, get the intIfNum of the LAG interface
*
* @param    index      (input) LAG index
* @param    *intIfNum  (output) internal interface number
*
* @returns  L7_SUCCESS, if the LAG was found
* @returns  L7_FAILURE, if not
*
* @notes    index begins with one, but the array is zero-based.
*
* @end
*********************************************************************/
L7_RC_t dot3adAggIfNumFromIndexGet(L7_uint32 index, L7_uint32 *intIfNum)
{
  if ( ( 1 > index ) ||
       ( L7_MAX_NUM_LAG_INTF < index ) )
  {
    return L7_FAILURE;
  }

  *intIfNum = dot3adAgg[index - 1].aggId;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given the intIfNum of the LAG interface, return the index
*
* @param    index      (input) LAG index
* @param    *intIfNum  (output) internal interface number
*
* @returns  L7_SUCCESS, if the LAG was found
* @returns  L7_FAILURE, if not
*
* @notes    index begins with one, but the array is zero-based.
*
* @end
*********************************************************************/
L7_RC_t dot3adAggIndexFromIfNumGet(L7_uint32 intIfNum, L7_uint32 *index)
{
  if ( L7_NULLPTR == dot3adAggIntfFind( intIfNum ) )
  {
    return L7_FAILURE;
  }

  *index = dot3adAggIdx[intIfNum] + 1;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Sets the aggregator in use flag.  If an aggregator is 
 *           in-use, it has been created but may not be active or have
 *           members.
 *
 * @param    agg_intf     aggregator internal interface number
 * @param    *status      in use or not
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggInuseSet(L7_uint32 agg_intf, L7_BOOL status)
{
  return LACIssueCmd(AGG_INUSE, agg_intf, (void*)&status);
}

/*********************************************************************
 * @purpose  Gets the aggregator in use flag.  If an aggregator is 
 *           in-use, it has been created but may not be active or have
 *           members.
 *
 * @param    agg_intf     aggregator internal interface number
 * @param    *status      in use or not
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggInuseGet(L7_uint32 agg_intf, L7_BOOL *status)
{
  return aggInuseGet(agg_intf, status);
}

/*********************************************************************
 * @purpose  Gets the MAC address assigned to the aggregator
 *
 * @param    intf            interface
 * @param    *mac            MAC address, L7_MAC_ADDR_LEN in length
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggMACAddressGet(L7_uint32 intf, L7_uchar8 *mac)
{
  return aggMACAddressGet(intf, mac);
}

/*********************************************************************
 * @purpose  Gets the priority value associated with the system's actor ID
 *
 * @param    intf            interface
 * @param    *priority       priority, 0-64k
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggActorSystemPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Sets the priority value associated with the system's actor ID
 *
 * @param    intf            interface
 * @param    priority        priority, 0-64k
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorSystemPrioritySet(L7_uint32 intf, L7_uint32 priority)
{
  return LACIssueCmd(AGG_ACTOR_SYSTEM_PRIORITY, intf, (void*)&priority);
}

/*********************************************************************
 * @purpose  Gets the MAC address used as a unique identifier for the 
 *           system that contains this aggregator
 *
 * @param    intf            interface
 * @param    *mac            MAC identifier
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorSystemIDGet(L7_uint32 intf, L7_char8 *mac)
{
  return aggActorSystemIDGet(intf, (L7_enetMacAddr_t*)mac);
}

/*********************************************************************
 * @purpose  Determines if the aggregator represents an aggregate or 
 *           an individual link
 *
 * @param    intf            interface
 * @param    *link           L7_TRUE: aggregate
 *                           L7_FALISE: individual
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorAggregateOrIndividualGet(L7_uint32 intf, L7_BOOL *link)
{
  return aggActorAggregateOrIndividualGet(intf, link);
}

/*********************************************************************
 * @purpose  Gets the current administrative value of the Key for the
 *           aggregator 
 *
 * @param    intf            interface
 * @param    *key            key value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    admin and oper key may differ
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorAdminKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggActorAdminKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  Sets the current administrative value of the Key for the
 *           aggregator 
 *
 * @param    intf            interface
 * @param    key             key value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    admin and oper key may differ
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorAdminKeySet(L7_uint32 intf, L7_uint32 key)
{
  return LACIssueCmd(AGG_ACTOR_ADMIN_KEY, intf, (void*)&key);
}

/*********************************************************************
 * @purpose  Gets the current operational value of the Key for the
 *           aggregator 
 *
 * @param    intf            interface
 * @param    *key            key value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    admin and oper key may differ
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggActorOperKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggActorOperKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  MAC address of current partner of aggregator
 *
 * @param    intf            interface
 * @param    *mac            MAC address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    a value 0 indicates there is no known partner
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPartnerSystemIDGet(L7_uint32 intf, L7_uint32 *mac)
{
  return aggPartnerSystemIDGet(intf, mac);
}

/*********************************************************************
 * @purpose  Priority value associated with partner's system ID
 *
 * @param    intf            interface
 * @param    *priority       priority value, 16bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    If the aggregation is manually configured, it will be a
 *           value assigned by the local system
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPartnerSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPartnerSystemPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  The current operational value of the key for the aggregators
 *           current protocol partner. aggregator parser and either
 *           delivering it to a MAC client or discarding it
 *
 * @param    intf            interface
 * @param    *key            operational key
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    If the aggregation is manually configured, it will be a
 *           value assigned by the local system
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPartnerOperKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggPartnerOperKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  Delay that can be imposed on the frame collector between
 *           receving a frame from and aggregator parser and either
 *           delivering it to a MAC client or discarding it
 *
 * @param    intf            interface
 * @param    *delay          10s of usecs
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggCollectorMaxDelayGet(L7_uint32 intf, L7_uint32 *delay)
{
  return aggCollectorMaxDelayGet(intf, delay);
}


/*********************************************************************
 * @purpose  Delay that can be imposed on the frame collector between
 *           receving a frame from and aggregator parser and either
 *           delivering it to a MAC client or discarding it
 *
 * @param    intf            interface
 * @param    delay           10s of usecs
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggCollectorMaxDelaySet(L7_uint32 intf, L7_uint32 delay)
{
  return LACIssueCmd(AGG_COLLECTOR_MAX_DELAY, intf, (void*)&delay);

}

/*********************************************************************
 * @purpose  Check the validity of an interface
 *
 * @param    intf       internal interface number
 *
 * @returns  L7_SUCCESS, if a the first was found
 * @returns  L7_FAILURE, if a the first doesn't exist
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortIndexGet(L7_uint32 intf)
{
  if (dot3adPortIntfFind(intf) != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}
/*********************************************************************
 * @purpose  Given an internal interface, the get the next one,
 *           if it exists
 *
 * @param    intf       internal interface number, start
 * @param    *next      internal interface number, next
 *
 * @returns  L7_SUCCESS, if a next was found
 * @returns  L7_FAILURE, if a next doesn't exist
 *
 * @notes   if the start doesnt exits, the closest next will be returned 
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortIndexGetNext(L7_uint32 intf, L7_uint32 *next)
{
  L7_uint32 i;
  dot3ad_port_t *p;
  L7_uint32 highestIntfNumber;
  L7_RC_t rc;

  rc = nimGetHighestIntfNumber(&highestIntfNumber);

  for (i=intf+1;i<=highestIntfNumber;i++)
  {

    if (nimCheckIfNumber(i) != L7_SUCCESS) 
      continue;

    p = dot3adPortIntfFind(i);
    if (p != L7_NULLPTR)
    {
      /* if (p->ready == L7_TRUE) */
      *next = i;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}


/*********************************************************************
 * @purpose  Gets the priority value associated with the system actor's ID
 *
 * @param    intf            interface
 * @param    *priority       0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPortActorSystemPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Sets the priority value associated with the system actor's ID
 *
 * @param    intf            interface
 * @param    *priority       0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorSystemPrioritySet(L7_uint32 intf, L7_uint32 priority)
{
  return LACIssueCmd(PORT_ACTOR_SYSTEM_PRIORITY, intf, (void*)&priority);
}

/*********************************************************************
 * @purpose  Gets the system id for the system that contains this 
 *           aggregation port
 *
 * @param    intf            interface
 * @param    *mac            system id
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorSystemIDGet(L7_uint32 intf, L7_uint32 *mac)
{
  return aggPortActorSystemIDGet(intf, mac);
}

/*********************************************************************
 * @purpose  Gets current administrative value of the key for the
 *           aggregation port
 *
 * @param    intf            interface
 * @param    *key            key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorAdminKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggPortActorAdminKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  Sets current administrative value of the key for the
 *           aggregation port
 *
 * @param    intf            interface
 * @param    key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorAdminKeySet(L7_uint32 intf, L7_uint32 key)
{
  return LACIssueCmd(PORT_ACTOR_ADMIN_KEY, intf, (void*)&key);
}

/*********************************************************************
 * @purpose  Gets current operational value of the key for the
 *           aggregation port
 *
 * @param    intf            interface
 * @param    *key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorOperKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggPortActorOperKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  Sets current operational value of the key for the
 *           aggregation port
 *
 * @param    intf            interface
 * @param    key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorOperKeySet(L7_uint32 intf, L7_uint32 key)
{
  return LACIssueCmd(PORT_ACTOR_OPER_KEY, intf, (void*)&key);
}

/*********************************************************************
 * @purpose  Gets the current administrative priority associated
 *           with the partner's system ID
 *
 * @param    intf            interface
 * @param    priority        priority 0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPortPartnerAdminSystemPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Sets the current administrative priority associated
 *           with the partner's system ID
 *
 * @param    intf            interface
 * @param    priority        priority 0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminSystemPrioritySet(L7_uint32 intf, L7_uint32 priority)
{
  return LACIssueCmd(PORT_PARTNER_ADMIN_SYSTEM_PRIORITY, intf, (void*)&priority);
}

/*********************************************************************
 * @purpose  Sets the current operational priority associated
 *           with the partner's system ID
 *
 * @param    intf            interface
 * @param    priority        priority 0..255
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPortPartnerOperSystemPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner system ID (administrative)
 *
 * @param    intf            interface
 * @param    *mac            ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminSystemIDGet(L7_uint32 intf, L7_enetMacAddr_t *mac)
{
  return aggPortPartnerAdminSystemIDGet(intf, mac);
}

/*********************************************************************
 * @purpose  Sets the aggregation port's partner system ID (administrative)
 *
 * @param    intf            interface
 * @param    mac             ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminSystemIDSet(L7_uint32 intf, L7_enetMacAddr_t mac)
{
  return LACIssueCmd(PORT_PARTNER_ADMIN_SYSTEM_ID, intf, (void*)&mac);
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner system ID (administrative)
 *
 * @param    intf            interface
 * @param    *mac            ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperSystemIDGet(L7_uint32 intf, L7_enetMacAddr_t *mac)
{
  return aggPortPartnerOperSystemIDGet(intf, mac);
}

/*********************************************************************
 * @purpose  Sets the aggregation port's partner system ID (operational)
 *
 * @param    intf            interface
 * @param    mac             ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperSystemIDSet(L7_uint32 intf, L7_uint32 mac)
{
  return LACIssueCmd(PORT_PARTNER_OPER_SYSTEM_ID, intf, (void*)&mac);
}

/*********************************************************************
 * @purpose  Sets the aggregation port's partner system ID (administrative)
 *
 * @param    intf            interface
 * @param    *key            key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggPortPartnerAdminKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  Sets the aggregation port's partner system ID (administrative)
 *
 * @param    intf            interface
 * @param    key             key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminKeySet(L7_uint32 intf, L7_uint32 key)
{
  return LACIssueCmd(PORT_PARTNER_ADMIN_KEY, intf, (void*)&key);
}

/*********************************************************************
 * @purpose  Gets the aggregation port's partner system ID (operational)
 *
 * @param    intf            interface
 * @param    *key            key, 16 bit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperKeyGet(L7_uint32 intf, L7_uint32 *key)
{
  return aggPortPartnerOperKeyGet(intf, key);
}

/*********************************************************************
 * @purpose  The aggregrator this port has selected
 *
 * @param    intf            interface
 * @param    *agg            aggregrator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    0 means the port has not selected and aggregator
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortSelectedAggIDGet(L7_uint32 intf, L7_uint32 *agg)
{
  return aggPortSelectedAggIDGet(intf, agg);
}

/*********************************************************************
 * @purpose  The identifier value of the aggregrator this port is 
 *           currently attached to
 *
 * @param    intf            interface
 * @param    *id             id value of aggregator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    0 means the port is not attached to an aggregator
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortAttachedAggIDGet(L7_uint32 intf, L7_uint32 *id)
{
  return aggPortAttachedAggIDGet(intf, id);
}

/*********************************************************************
 * @purpose  The port number locally assigned to this aggregation port
 *
 * @param    intf            interface
 * @param    *port           port actor 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    0 means the port has not attached to an aggregrator
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorPortGet(L7_uint32 intf, L7_uint32 *port)
{
  return aggPortActorPortGet(intf, port);
}

/*********************************************************************
 * @purpose  Get the priority assigned to this aggregation port
 *
 * @param    intf            interface
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorPortPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPortActorPortPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Set the priority assigned to this aggregation port
 *
 * @param    intf            interface
 * @param    priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorPortPrioritySet(L7_uint32 intf, L7_uint32 priority)
{
  return LACIssueCmd(PORT_ACTOR_PORT_PRIORITY, intf, (void*)&priority);
}

/*********************************************************************
 * @purpose  Get the administrative value of the port number for the partner
 *
 * @param    intf            interface
 * @param    *port           port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminPortGet(L7_uint32 intf, L7_uint32 *port)
{
  return aggPortPartnerAdminPortGet(intf, port);
}

/*********************************************************************
 * @purpose  Set the administrative value of the port number for the partner
 *
 * @param    intf            interface
 * @param    port            port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminPortSet(L7_uint32 intf, L7_uint32 port)
{
  return LACIssueCmd(PORT_PARTNER_ADMIN_PORT, intf, (void*)&port);
}

/*********************************************************************
 * @purpose  Get the administrative value of the port number for the partner
 *
 * @param    intf            interface
 * @param    *port           port
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperPortGet(L7_uint32 intf, L7_uint32 *port)
{
  return aggPortPartnerOperPortGet(intf, port);
}

/*********************************************************************
 * @purpose  Get the administrative value of the port priority of the partner
 *
 * @param    intf            interface
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminPortPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPortPartnerAdminPortPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Set the administrative value of the port priority of the partner
 *
 * @param    intf            interface
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminPortPrioritySet(L7_uint32 intf, L7_uint32 priority)
{
  return LACIssueCmd(PORT_PARTNER_ADMIN_PORT_PRIORITY, intf, (void*)&priority);
}

/*********************************************************************
 * @purpose  Get the priority value assigned to this aggregation port
 *           by the partner
 *
 * @param    intf            interface
 * @param    *priority       priority
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperPortPriorityGet(L7_uint32 intf, L7_uint32 *priority)
{
  return aggPortPartnerOperPortPriorityGet(intf, priority);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the admin values of actor_state
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorAdminStateGet(L7_uint32 intf, L7_uchar8 *state)
{
  return aggPortActorAdminStateGet(intf, state);
}

/*********************************************************************
 * @purpose  Set 8 bits corresponding to the admin values of actor_state
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorAdminStateSet(L7_uint32 intf, L7_uchar8 state)
{
  return LACIssueCmd(PORT_ACTOR_ADMIN_STATE, intf, (void*)&state);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the admin values of actor_state
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortActorOperStateGet(L7_uint32 intf, L7_uchar8 *state)
{
  return aggPortActorOperStateGet(intf, state);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the current values of the
 *           actor_state from the most recently received LACPDU (by the partner)
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerOperStateGet(L7_uint32 intf, L7_uchar8 *state)
{
  return aggPortPartnerOperStateGet(intf, state);
}

/*********************************************************************
 * @purpose  Get 8 bits corresponding to the current administrative 
 *           value of the actor_state for the protocol partner
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminStateGet(L7_uint32 intf, L7_uchar8 *state)
{
  return aggPortPartnerAdminStateGet(intf, state);
}

/*********************************************************************
 * @purpose  Set 8 bits corresponding to the current administrative 
 *           value of the actor_state for the protocol partner
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    activity,timeout,aggregation,synch,collecting,distributing,
 *           defaulted,expired
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminStateSet(L7_uint32 intf, L7_uchar8 state)
{
  return LACIssueCmd(PORT_PARTNER_ADMIN_STATE, intf, (void*)&state);
}

/*********************************************************************
 * @purpose  Get whether an aggregation port is able to aggregate
 *
 * @param    intf            interface
 * @param    *state          state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortAggregateOrIndividualGet(L7_uint32 intf, L7_BOOL *state)
{
  return aggPortAggregateOrIndividualGet(intf, state);
}

/*********************************************************************
 * @purpose  Get the number of valid LACPDUs received on this aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsLACPDUsRxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsLACPDUsRxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker PDUs received on this aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsMarkerPDUsRxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsMarkerPDUsRxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker response PDUs received ont this 
 *           aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsMarkerResponsePDUsRxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsMarkerResponsePDUsRxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of unknown frames
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    type=slow protocol but unknown PDU or
 *           addressed to the slow protocols group but dont have the
 *           type set
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsUnknownRxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsUnknownRxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of valid LACPDUs received on this aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsIllegalRxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsIllegalRxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of valid LACPDUs transmitted on this 
 *           aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsLACPDUsTxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsLACPDUsTxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker PDUs transmitted on this 
 *           aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsMarkerPDUsTxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsMarkerPDUsTxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get the number of marker response PDUs transmitted on this 
 *           aggregation port
 *
 * @param    port            physical port
 * @param    *stat           value of statistic
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortStatsMarkerResponsePDUsTxGet(L7_uint32 port, L7_uint32 *stat)
{
  return aggPortStatsMarkerResponsePDUsTxGet(port, stat);
}

/*********************************************************************
 * @purpose  Get a bitwise list of ports included in an aggregator
 *
 * @param    agg_intf        internal interface number of aggregator
 * @param    *list           portlist, bits sent are included
 * @param    *length         length in bytes of the portlist
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortListGet(L7_uint32 agg_intf, L7_uchar8 *list, L7_uint32 *length)
{
  return aggPortListGet(agg_intf, list, length);
}

/*********************************************************************
 * @purpose  Get the current state of the state machine for this port 
 *
 * @param    port            physical port
 * @param    *state          one of 6 bits set representing a state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugRxStateGet(L7_uint32 port, L7_uchar8 *state)
{
  return aggPortDebugRxStateGet(port, state);
}

/*********************************************************************
 * @purpose  Get the last time a LACPDU was received on this port
 *
 * @param    port            physical port
 * @param    *time           time of reception
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugLastRxTimeGet(L7_uint32 port, L7_timespec *time)
{
  return aggPortDebugLastRxTimeGet(port, time);
}

/*********************************************************************
 * @purpose  Gets the state of the MUX state machine
 *
 * @param    port            physical port
 * @param    *state          one of 6 bits set representing state
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugMuxStateGet(L7_uint32 port, L7_uchar8 *state)
{
  return aggPortDebugMuxStateGet(port, state);
}

/*********************************************************************
 * @purpose  Gets a text string indicating the reason for the most
 *           recent state change in the MUX machine
 *
 * @param    port            physical port
 * @param    *reason         textual reason
 * @param    *length         length of string
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugMuxReasonGet(L7_uint32 port, L7_char8 *reason, L7_uint32 *length)
{
  return aggPortDebugMuxReasonGet(port, reason, length);
}

/*********************************************************************
 * @purpose  The state of the actor churn machine for this agg. port
 *
 * @param    port            physical port
 * @param    *churnstate     churn state, 3 bits
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugActorChurnStateGet(L7_uint32 port, L7_uchar8 *churnstate)
{
  return aggPortDebugActorChurnStateGet(port, churnstate);
}

/*********************************************************************
 * @purpose  The state of the partner churn machine for this agg. port
 *
 * @param    port            physical port
 * @param    *churnstate     churn state, 3 bits
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugPartnerChurnStateGet(L7_uint32 port, L7_uchar8 *churnstate)
{
  return aggPortDebugPartnerChurnStateGet(port, churnstate);
}

/*********************************************************************
 * @purpose  A count of the number of times the churn machine entered
 *           the ACTOR_CHURN state
 *
 * @param    port            physical port
 * @param    *churncount     churn count
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugActorChurnCountGet(L7_uint32 port, L7_uint32 *churncount)
{
  return aggPortDebugActorChurnCountGet(port, churncount);
}

/*********************************************************************
 * @purpose  A count of the number of times the churn machine entered
 *           the PARTNER_CHURN state
 *
 * @param    port            physical port
 * @param    *churncount     churn count
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugPartnerChurnCountGet(L7_uint32 port, L7_uint32 *churncount)
{
  return aggPortDebugPartnerChurnCountGet(port, churncount);
}

/*********************************************************************
 * @purpose  A count of the number of times the actor's mux machine entered
 *           the IN_SYNC state
 *
 * @param    port            physical port
 * @param    *count          transition count
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugActorSyncTransitionCountGet(L7_uint32 port, L7_uint32 *count)
{
  return aggPortDebugActorSyncTransitionCountGet(port, count);
}

/*********************************************************************
 * @purpose  A count of the number of times the actor's mux machine entered
 *           the IN_SYNC state
 *
 * @param    port            physical port
 * @param    *count          transition count
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugPartnerSyncTransitionCountGet(L7_uint32 port, L7_uint32 *count)
{
  return aggPortDebugPartnerSyncTransitionCountGet(port, count);
}

/*********************************************************************
 * @purpose  A count of the number of times the actor's perception of 
 *           the LAG ID for this aggregation port has changed
 *
 * @param    port            physical port
 * @param    *count          transition count
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugActorChangeCountGet(L7_uint32 port, L7_uint32 *count)
{
  return aggPortDebugActorChangeCountGet(port, count);
}

/*********************************************************************
 * @purpose  A count of the number of times the partner's perception of 
 *           the LAG ID for this aggregation port has changed
 *
 * @param    port            physical port
 * @param    *count          transition count
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortDebugPartnerChangeCountGet(L7_uint32 port, L7_uint32 *count)
{
  return aggPortDebugPartnerChangeCountGet(port, count);
}

/*********************************************************************
 * @purpose  Gets the text name associated with this aggregator
 *
 * @param    agg_intf     aggregator internal interface number
 * @param    *name        pointer to allocated storage
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    max length of L7_DOT3AD_MAX_NAME characters
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortNameGet(L7_uint32 agg_intf, L7_uchar8 *name)
{
  return aggPortNameGet(agg_intf, name);
}

/*********************************************************************
 * @purpose  Sets the text name for this aggregator
 *
 * @param    agg_intf     aggregator internal interface number
 * @param    *name        pointer to L7_DOT3AD_MAX_NAME characters
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortNameSet(L7_uint32 agg_intf, L7_uchar8 *name)
{
  return LACIssueCmd(AGG_NAME, agg_intf, (void*)name);
}

/*********************************************************************
* @purpose  Sets the Hash Mode per LAG.
*
* @param    agg_intf     aggregator internal interface number
* @param    hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_DEPENDENCY_NOT_MET, if the LAG interface is admin disabled
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adAggPortHashModeSet(L7_uint32 agg_intf, L7_uint32 hashMode)
{
  dot3ad_agg_t *a;
  L7_RC_t rc = L7_FAILURE;

  if ((hashMode <= L7_DOT3AD_HASH_MIN) || (hashMode >= L7_DOT3AD_HASH_MAX))
  {
     return rc;
  }

  a = dot3adAggIntfFind(agg_intf);
  if (a != L7_NULL)
  {
    /* Set the HASH Mode only if adminMode is enabled */
    if (L7_ENABLE == a->adminMode)
    {
      return LACIssueCmd(AGG_HASHMODE_SET, agg_intf, (void*)&hashMode);
    }
    else
    {
      return L7_DEPENDENCY_NOT_MET;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Gets the Hash Mode per LAG.
*
* @param    agg_intf     aggregator internal interface number
* @param    *hashMode    pointer to Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adAggPortHashModeGet(L7_uint32 agg_intf, L7_uint32 * hashMode)
{
  return aggPortHashModeGet (agg_intf, hashMode);
}
/*********************************************************************
* @purpose  Sets the Hash Mode of dot3ad system
*
* @param    hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adAggSystemHashModeSet(L7_uint32 hashMode)
{
  L7_uint32 lagIntIfNum, itype;
  L7_RC_t   rc;

  if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID,
                            L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID)
                           == L7_TRUE)
  {
    if (dot3adCfg.cfg.dot3adSystem.hashMode == hashMode)
    {
      return L7_SUCCESS;
    }
    dot3adCfg.cfg.dot3adSystem.hashMode = hashMode;
    dot3adSystem.hashMode = hashMode;
    dot3adCfg.hdr.dataChanged = L7_TRUE;
  }
  
  /* Apply the new config on all existing lag interfaces */
  rc = nimFirstValidIntfNumber(&lagIntIfNum);
  while (rc == L7_SUCCESS)
  {
    if (nimGetIntfType(lagIntIfNum, &itype) == L7_SUCCESS)
    {
      /* Interface should be LAG interface */
      if (itype == L7_LAG_INTF)
      {
        if (dot3adAggPortHashModeSet(lagIntIfNum, hashMode) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(lagIntIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT3AD_COMPONENT_ID,
                  "Failed to set hash mode %d for lag %s",
                   hashMode, ifName);
        }
      }
    }
    rc = nimNextValidIntfNumber(lagIntIfNum, &lagIntIfNum);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Hash Mode of the dot3ad system
*
* @param    *hashMode    pointer to Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adAggSystemHashModeGet(L7_uint32 * hashMode)
{
  if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID, 
                            L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID) 
                           == L7_TRUE)
  {
    *hashMode = dot3adCfg.cfg.dot3adSystem.hashMode;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/* BEGIN NON SNMP RELATED LAG FNS */

/*********************************************************************
 * @purpose  Creates a new LAG.
 * 
 * @param    lag_index   Suggested index
 * @param    *name       name string for the LAG (up to LAG_MAX_NAME-1 chars)
 *                         (@b{Input:} string of 0 to LAG_MAX_NAME-1 ASCII 
 *                          characters, terminated by a '\0' character)
 * @param    members[]   densely-packed array of L7_MAX_MEMBERS_PER_LAG elements
 *                         containing zero or more LAG member internal interface
 *                         numbers, with (@b{all unused array elements set to 0})
 *                         (@b{Input:} list of internal interface number(s))
 * @param    adminMode   administrative mode of the LAG interface
 *                         (@b{Input:} L7_ENABLE or L7_DISABLE)
 * @param    linkTrapMode link trap setting for the LAG interface
 *                         (@b{Input:} L7_ENABLE or L7_DISABLE)
 * @param    hashMode    Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
 *
 * @param    *pIntIfNum  pointer to output location
 *                         (@b{Output:} internal interface number of the LAG,
 *                          or 0 if this operation is not successful)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR    invalid config parameter
 * @returns  L7_FAILURE
 *
 * @notes    A LAG can be configured without any members.  When members are
 *           specified, they must all be full-duplex and the same speed. 
 *           Up to L7_MAX_MEMBERS_PER_LAG may be specified for a LAG.  The 
 *           @i{member } array parameter must be densely-packed, with all 
 *           unused array elements set to 0.
 *
 * @notes    None.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagCreate(L7_int lag_index /*PTin added*/, L7_char8 *name, L7_uint32 members[],
    L7_uint32 adminMode, L7_uint32 linkTrapMode,
                        L7_uint32 hashMode,
    L7_uint32 *pIntIfNum)
{

  L7_uint32 lagId,maxLagIntf,index;
  L7_RC_t rc = L7_SUCCESS;

  maxLagIntf = platIntfLagIntfMaxCountGet();

  /* find an available LAG ID */
  if (lag_index < 0)
  {
    for (index = 0; index < maxLagIntf; index++) 
    {
      if (dot3adAgg[index].inuse != L7_TRUE)
      {
        dot3adAgg[index].inuse = L7_TRUE;
        break;
      }
    }
  }
  else
  {
    index = lag_index;

    /* If index already exists, return error */
    if (dot3adAgg[index].inuse == L7_TRUE)
    {
      PT_LOG_ERR(LOG_CTX_TRUNKS, "LAG index %u already exists!", lag_index);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_TRUNKS, "LAG index %u is going to be used!", lag_index);
      dot3adAgg[index].inuse = L7_TRUE;
    }
  }

  if (index == maxLagIntf)
  {
    /* No available LAG IDs */
    rc =  L7_FAILURE;
  }
  else
  {
    /* the index is zero based and the lagId is 1 based */
    lagId = index + 1;

    /* create the LAG with the lag ID acquired */
    if (dot3adPrivateLagCreate(lagId,name,members,adminMode,linkTrapMode,
                                L7_FALSE,hashMode,pIntIfNum) != L7_SUCCESS)
    {
      dot3adAgg[index].inuse = L7_FALSE;
      rc =  L7_FAILURE;
    }
  }

  return rc;

}

/*********************************************************************
 * @purpose  Modifies the characteristics of an existing LAG.
 *
 * @param    intIfNum    internal interface number of the LAG being modified
 * @param    *name       name string for the LAG (up to LAG_MAX_NAME-1 chars)
 *                         (@b{Input:} string of 0 to LAG_MAX_NAME-1 ASCII 
 *                          characters, terminated by a '\0' character)
 * @param    members[]   densely-packed array of L7_MAX_MEMBERS_PER_LAG elements
 *                         containing zero or more LAG member internal interface
 *                         numbers, with (@b{all unused array elements set to 0})
 *                         (@b{Input:} list of internal interface number(s))
 * @param    adminMode   administrative mode of the LAG interface
 *                         (@b{Input:} L7_ENABLE or L7_DISABLE)
 * @param    linkTrapMode link trap setting for the LAG interface
 *                         (@b{Input:} L7_ENABLE or L7_DISABLE)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR    invalid config parameter
 * @returns  L7_FAILURE
 *
 * @notes    Only operates on an existing LAG.  Use lagapiLagCreate() to 
 *           create a new LAG.
 *
 * @notes    A LAG can be configured without any members.  When members are
 *           specified, they must all be full-duplex and the same speed. 
 *           Up to L7_MAX_MEMBERS_PER_LAG may be specified for a LAG.  The 
 *           @i{member } array parameter must be densely-packed, with all 
 *           unused array elements set to 0.
 *
 * @notes    None.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagModify(L7_uint32 intIfNum, L7_char8 *name,          
    L7_uint32 members[],   
    L7_uint32 adminMode, L7_uint32 linkTrapMode)
{
  L7_uint32 i,key;
  L7_RC_t rc;
  /* set admin mode   */
  /* set linktrapmode */

  for (i=0;i<L7_MAX_MEMBERS_PER_LAG;i++)
  {
    if (members[i] != 0)
    {
      if (L7_SUCCESS == dot3adAggPortActorOperKeyGet(intIfNum, &key))
      {
	if (key != members[i]) /* not already a member */
	  rc = dot3adAggPortActorOperKeySet(members[i], intIfNum);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Removes an existing LAG.
 *
 * @param    intIfNum    internal interface number of the LAG being removed
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none 
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagRemove(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t handle;

  rc = dot3adLihNotifySystem (intIfNum, L7_DOWN);


  /* successful event notification */
  eventInfo.component = L7_DOT3AD_COMPONENT_ID;
  eventInfo.intIfNum  = intIfNum;
  eventInfo.pCbFunc   = dot3adNimEventCompletionCallback;

  eventInfo.event     = L7_DETACH;
  rc = nimEventIntfNotify(eventInfo,&handle);

  return rc;
}

/*********************************************************************
 * @purpose  Add one or more interfaces to a LAG port.
 *            
 * @param    intIfNum    internal interface number of the target LAG
 * @param    count       number of members being added to the LAG 
 * @param    pMemIntf    pointer to list of LAG member intfs to add
 *                         
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR    if parameter checking fails 
 * @returns  L7_FAILURE
 *
 * @notes    This API command can only be used to update an existing LAG.
 *
 * @notes    New LAG members must be full-duplex and the same speed as 
 *           all existing members of this LAG.
 *
 * @notes    @i{pMemIntf} points to a @b{densely-packed} array of internal
 *           interface numbers.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagMemeberAdd(L7_uint32 intIfNum, L7_uint32 count, 
    L7_uint32 *pMemIntf)
{
  L7_uint32 i;
  dot3ad_agg_t *a;
  dot3ad_port_t *p;
  L7_RC_t rc = L7_SUCCESS;
  L7_SPECIAL_PORT_TYPE_t portType;

#if 0
  #if defined L7_DOT1AD_PACKAGE
  {
     L7_uint32 nniIntf;

     dot1adNniInterfaceGet(&nniIntf);
     for (i=0; i<count;i++)
     {
       if (pMemIntf[i] == nniIntf)
       {
        return L7_NOT_SUPPORTED;
       }
    }
  }
  #endif
#endif


  /* is the aggregator valid */
  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR || a->inuse == L7_FALSE || a->adminMode == L7_DISABLE)
  {
    return L7_FAILURE;
  }

  for (i=0;i<count;i++)
  {

    /* are we adding a physical interface */
    if (dot3adLihIsIntfTypePhy(pMemIntf[i]) == L7_FALSE)
    {
      return L7_FAILURE;
    }
    rc = usmDbIfSpecialPortTypeGet(simGetThisUnit(),pMemIntf[i],&portType);
    if (portType != L7_PORT_NORMAL)
    {
      return L7_FAILURE;
    }
    /* is there room to add another member */
    if (a->currNumWaitSelectedMembers == L7_MAX_MEMBERS_PER_LAG)
    {
      return L7_FAILURE;
    }

    /* get the port record */
    p = dot3adPortIntfFind(pMemIntf[i]);
    if (p == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    /* make sure its not already a member of an aggregation */
    if (p->actorOperPortKey == p->actorPortWaitSelectedAggId && p->actorOperPortKey != 0)
    {
      return L7_FAILURE; 
    }

    /* With pre configuration in mind we should allow ports to be configured
     * irrespective of their aggregation bit. With the bit set to individual 
     * they will never be active participants
     */
    /*
       if (p->portIndividualAgg == L7_FALSE)
       {
       return L7_FAILURE;
       }
     */
    /* check lif membership  */
    if (dot3adLihIsLifMember(pMemIntf[i]) == L7_TRUE)
    {
      return L7_FAILURE;
    }

    /* check mac filtering membership*/
    if (dot3adLihIsFilterMember(pMemIntf[i]) == L7_TRUE)
    {
      return L7_REQUEST_DENIED;
    }

    /*checked for all conditions for failure, this port can now be initiated to aggregate*/
    /*add this interface to the waiting to be selected list*/
    rc = dot3adAggWaitSelectedAdd(a->aggId,pMemIntf[i]);
    if (rc == L7_FAILURE)
    {
      return L7_FAILURE;
    }
    /*set the wait selected agg id to the chossen agg id*/
    p->actorPortWaitSelectedAggId = a->aggId;

    /* Reset the recieved LACPDUs dropped statistic*/
    if(a->isStatic == L7_TRUE)
    {
      dot3ad_stats[pMemIntf[i]].RxLACPDUsDropped =0;
    }


    /* set the oper key and notify LACP */
    rc = dot3adAggPortActorOperKeySet(pMemIntf[i], intIfNum);

    /* notify nim of aquire */
    /*
       rc = nimNotifyIntfChange(pMemIntf[i], L7_LAG_ACQUIRE);
     */
  }
  return rc;
}

/*********************************************************************
 * @purpose  Delete one or more member interfaces from a LAG port.
 *            
 * @param    intIfNum    internal interface number of the target LAG
 * @param    count       number of members being deleted from the LAG 
 * @param    pMemIntf    pointer to list of LAG member intfs to delete 
 *                         (@b{Input:} densely-packed array of internal intfs) 
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR    if parameter checking fails 
 * @returns  L7_FAILURE
 *
 * @notes    This API command can only be used to update an existing LAG.
 *
 * @notes    @i{pMemIntf} points to a @b{densely-packed} array of internal
 *           interface numbers.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagMemberDelete(L7_uint32 intIfNum, L7_uint32 count,
    L7_uint32 *pMemIntf)
{
  L7_RC_t rc, rc_exist;
  L7_uint32 i, j;
  dot3ad_port_t *p;
  dot3ad_agg_t *a;
  rc = L7_SUCCESS;
  for (i=0;i<count;i++)
  {

    /* remove only exisiting physical interfaces */
    if (dot3adLihIsIntfTypePhy(pMemIntf[i]) == L7_FALSE)
    {
      return L7_FAILURE;
    }

    /* save port pointer for later */
    p = dot3adPortIntfFind(pMemIntf[i]);
    if (p == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    /* find the ith member to be deleted in a list */
    rc_exist = L7_FAILURE;
    a = dot3adAggIntfFind(intIfNum);
    if (a == L7_NULLPTR || a->inuse == L7_FALSE || a->adminMode == L7_DISABLE)
    {
      return L7_FAILURE;
    }

    for (j=0;j<L7_MAX_MEMBERS_PER_LAG;j++)
    {
      if (a->aggPortList[j] == pMemIntf[i])
      {
	rc_exist = L7_SUCCESS;
      }
      if (a->aggSelectedPortList[j] == pMemIntf[i])
      {
	rc_exist = L7_SUCCESS;
      }
      if (a->aggWaitSelectedPortList[j] == pMemIntf[i])
      {
	rc_exist = L7_SUCCESS;
      }
    }

    if (rc_exist == L7_SUCCESS)
    {
      rc = LACIssueCmd(lacDeletePort, pMemIntf[i], L7_NULL);
      /*
	 rc = dot3adAggPortActorOperKeySet(intIfNum, pMemIntf[0]);
       */
      /* notify nim of release */
      /*
	 rc = nimNotifyIntfChange(pMemIntf[i], L7_LAG_RELEASE);
       */
      /*
	 rc = aggPortResetValues(p);
       */
    }
    else
    {
      /* currently, if one delete fails, all others after it
	 in pMemIntf fail */
      return L7_FAILURE;
    }

  }

  return rc;   

}

/*********************************************************************
 * @purpose  Get the list of member interfaces for the specified LAG interface.
 *            
 * @param    intIfNum       internal interface number of the target LAG
 * @param    *pListCount    pointer to array element count
 *                            (@b{Input:} max number of elements in caller's 
 *                             memberList[] array)
 *                            (@b{Output:} current number of LAG members)
 * @param    memberList[]   array to be filled in with densely-packed list of
 *                            LAG member interface numbers
 *                            (@b{Output:} LAG member intIfNum array)
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    This function outputs the current number of LAG members along
 *           with a densely-packed array of LAG member internal interface
 *           numbers.  Unused array entries are set to 0.
 *
 * @notes    To obtain all available LAG members, the caller should ensure the
 *           memberList[] array contains at least L7_MAX_MEMBERS_PER_LAG entries.
 *           This function limits itself to the smaller of L7_MAX_MEMBERS_PER_LAG
 *           and the caller's initial *pListCount value.
 *
 * @notes    Only the active members
 *           
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adMemberListGet(L7_uint32 intIfNum, L7_uint32 *pListCount, 
    L7_uint32 memberList[])
{
  dot3ad_agg_t *a;
  L7_uint32 i;

  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i=0;i<a->currNumWaitSelectedMembers;i++)
  {
    /*return the added members in waiting to be selected list*/
    memberList[i] = a->aggWaitSelectedPortList[i];
  }

  *pListCount = a->currNumWaitSelectedMembers; 

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Enable or disable a LAG port.
 *            
 * @param    intIfNum    internal interface number of the target LAG
 * @param    status      new admin mode setting
 * @param    updateConfig L7_FALSE if the mode is only operationally changed
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR    if parameter checking fails 
 * @returns  L7_FAILURE
 *
 * @notes    This API command can only be used to update an existing LAG.
 *
 * @notes    The allowed @i{status} values are L7_DISABLE or L7_ENABLE.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adAdminModeSet(L7_uint32 intIfNum, L7_uint32 status, L7_BOOL updateConfig)
{
  dot3adAdminMode_t mode;

  mode.status       = status;
  mode.updateConfig = updateConfig;

  return LACIssueCmd(AGG_ADMIN_MODE, intIfNum, &mode);
}

/*********************************************************************
 * @purpose  Get the admin mode value for the specified LAG interface.
 *            
 * @param    intIfNum       internal interface number of the target LAG
 * @param    *status        pointer to location where LAG admin mode is output
 *                            (@b{Output:} L7_DISABLE or L7_ENABLE)
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adAdminModeGet(L7_uint32 intIfNum, L7_uint32 *status)
{
  return aggAdminModeGet(intIfNum,status);
}

/*********************************************************************
 * @purpose  Set the LACP status on a per-port basis
 *
 * @param    port            physical port
 * @param    status         L7_TRUE/L7_FALSE 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE      if port not found
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortLacpEnabledSet(L7_uint32 port, L7_BOOL status)
{
  return aggPortLacpEnabledSet(port, status);
}

/*********************************************************************
 * @purpose  Get the LACP status on a per-port basis
 *
 * @param    port            physical port
 * @param    *status         L7_TRUE/L7_FALSE 
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE      if port not found
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adAggPortLacpEnabledGet(L7_uint32 port, L7_BOOL *status)
{
  return aggPortLacpEnabledGet(port, status);
}

/*********************************************************************
 * @purpose  Enable or disable Link Trap notification for the LAG.
 *            
 * @param    intIfNum    internal interface number of the target LAG
 * @param    status      new link trap mode setting
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR    if parameter checking fails 
 * @returns  L7_FAILURE
 *
 * @notes    This API command can only be used to update an existing LAG.
 *
 * @notes    The allowed @i{status} values are L7_DISABLE or L7_ENABLE.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLinkTrapModeSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc;
  rc = dot3adLihIntfLinkTrapSet(intIfNum, val);

  return rc;
}

/*********************************************************************
 * @purpose  Get the link trap mode value for the specified LAG interface.
 *            
 * @param    intIfNum       internal interface number of the target LAG
 * @param    *pLinkTrapMode pointer to location where LAG link trap mode is
 *                            output 
 *                            (@b{Output:} L7_DISABLE or L7_ENABLE)
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLinkTrapModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc;
  rc = dot3adLihIntfLinkTrapGet(intIfNum, val);
  return rc;
}

/*********************************************************************
 * @purpose  Determine if an interface is a configured LAG.
 *            
 * @param    intIfNum    internal interface number of interest
 *                            
 * @returns  L7_TRUE     interface is a configured LAG
 * @returns  L7_FALSE    interface neither a LAG nor a configured LAG
 *
 * @notes    A LAG that is in midst of a configuration change is not
 *           considered configured by this function.
 *
 * @end
 *********************************************************************/
L7_BOOL dot3adIsLagConfigured(L7_uint32 intIfNum)
{
  dot3ad_agg_t *a;

  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  return(a->inuse); 
}

/*********************************************************************
 * @purpose  Determine if an interface is a member of a LAG.
 *            
 * @param    intIfNum    internal interface number of interest
 *                            
 * @returns  L7_TRUE     interface is a LAG member
 * @returns  L7_FALSE    interface not a LAG member
 *
 * @notes    checks the aggWaitSelectedPortList
 *           
 *
 * @end
 *********************************************************************/
L7_BOOL dot3adIsLagMember(L7_uint32 intIfNum)
{
  dot3ad_port_t *p;

  dot3ad_agg_t *a;

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  if (p->actorPortWaitSelectedAggId != 0)
  {
    a = dot3adAggIntfFind(p->actorPortWaitSelectedAggId);
    if (a == L7_NULLPTR)
    {
      return L7_FALSE;
    }
  }
  else
    return L7_FALSE;

  return L7_TRUE;

}

/*********************************************************************
 * @purpose  Determine if an interface is a member of a LAG and
 get that LAG interface number
 *            
 * @param    intIfNum    internal interface number of interest
 * @param    *agg_intf   returned parent intf number
 *                            
 * @returns  L7_SUCCESS  interface is a LAG member
 * @returns  L7_FALSE    interface not a LAG member
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adAggGet(L7_uint32 intIfNum, L7_uint32 *agg_intf)
{
  dot3ad_port_t *p;

  if (dot3adLihIsIntfTypePhy(intIfNum) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (p->actorOperPortKey != p->actorPortNum)
  {
    *agg_intf = p->actorOperPortKey;
    return L7_SUCCESS;
  }

  return L7_FAILURE;

}

/*********************************************************************
 * @purpose  Determine to which LAG interface, if any, an interface belongs.
 *            
 * @param    intIfNum     internal interface number of possible LAG member
 * @param    pLagIntfNum    pointer to result 
 *                          (@b{Output:} internal interface number of 
 *                           LAG, if the specified interface is a member; 
 *                           otherwise, original @i{intIfNum} value is provided)
 *                            
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    A return code of L7_SUCCESS is used even when the interface is
 *           not a member of a LAG.  The L7_FAILURE return code designates
 *           a malfunction in the routine; it does not imply "not found".
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adWhoisOwnerLag(L7_uint32 intIfNum, L7_uint32 *pLagIntfNum)
{
  dot3ad_port_t *p;
  dot3ad_agg_t *a;

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (p->actorPortWaitSelectedAggId != 0)
    a = dot3adAggIntfFind(p->actorPortWaitSelectedAggId);
  else
    a = L7_NULLPTR;

  if (a == L7_NULLPTR)
  {
    *pLagIntfNum = intIfNum;
    return L7_SUCCESS;
  }
  else
    *pLagIntfNum = a->aggId;
  return L7_SUCCESS;

}

/*********************************************************************
 * @purpose  Determine if an interface is a LAG type of interface.
 *            
 * @param    intIfNum    internal interface number of interest
 *                            
 * @returns  L7_TRUE     interface is a LAG-type interface
 * @returns  L7_FALSE    interface not a LAG-type interface 
 *
 * @notes    This does not tell the caller anything about whether the
 *           LAG exists or not.  It only indicates whether the 
 *           specified interface is reserved for usage by the LAG
 *           component.  See dot3adIsLagConfigured() to determine if the
 *           interface is a configured LAG.
 *
 * @end
 *********************************************************************/
L7_BOOL dot3adIsLag(L7_uint32 intIfNum)
{
  dot3ad_agg_t *a;

  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
    return L7_FALSE;
  else
    return L7_TRUE;

}

/*********************************************************************
 * @purpose  Get the aggregate data rate of a LAG interface
 *            
 * @param    intIfNum       internal interface number of the target LAG
 * @param    *pLagDataRate  pointer to output location
 *                            (@b{Output:} LAG aggregate data rate in Mbps)
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Combines the full duplex data rates of all attached LAG members.
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagDataRateGet(L7_uint32 intIfNum, L7_uint32 *pLagDataRate)
{
  dot3ad_agg_t *a;
  L7_uint32 mem_speed, accum_speed = 0;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;

  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  for (i=0; ((i < a->activeNumMembers) && (rc == L7_SUCCESS)); i++)
  {
    if (a->aggActivePortList[i] != 0)
    {
      rc = dot3adLihIntfSpeedDataRateGet(a->aggActivePortList[i], &mem_speed);
      if (rc == L7_SUCCESS)
      {
	accum_speed += mem_speed;
      }
    }
  }
  *pLagDataRate = accum_speed;

  return rc;
}
/*********************************************************************
 * @purpose  Get the aggregate data rate of a LAG interface using the active members
 *            
 * @param    intIfNum       internal interface number of the target LAG
 * @param    *pLagDataRate  pointer to output location
 *                            (@b{Output:} LAG aggregate data rate in Mbps)
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagActiveDataRateGet(L7_uint32 intIfNum, L7_uint32 *pLagDataRate)
{
  dot3ad_agg_t *a;
  L7_uint32 activeMembersNum;
  L7_uint32 mem_speed;
  L7_RC_t rc = L7_FAILURE;

  a = dot3adAggIntfFind(intIfNum);

  if (a == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  activeMembersNum = a->activeNumMembers;

  if (activeMembersNum >= 1)
  {
    rc = nimGetIntfSpeedSimplexDataRate(a->aggActivePortList[0], &mem_speed);
    if (rc != L7_SUCCESS)
    {
      *pLagDataRate = 0;
      return rc;
    }
    *pLagDataRate = activeMembersNum *  mem_speed;
  }                   
  else
  {
    /* No Active members */
    *pLagDataRate = 0;
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  Get the configured number of members for a LAG
 *            
 * @param    intIfNum        internal interface number of the target LAG
 * @param    *numMembers pointer to output location
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagNumMembersGet(L7_uint32 intIfNum, L7_uint32 *numMembers)
{
  dot3ad_agg_t *a;
  L7_RC_t rc = L7_SUCCESS;

  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *numMembers = a->currNumWaitSelectedMembers ;

  return rc;
}

/******************************************************************************
 * @purpose  Set the mode (static or not,i.e, dynamic) for this LAG
 *
 * @param    intIfNum     internal interface number of the LAG being modified
 * @param    isStatic     sets if the LAG is static or dynamic:L7_TRUE/L7_FALSE
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_TABLE_IS_FULL if user is making static lag dynamic, and maximum 
 *                            number of dynamic lags already present in the system. 
 *
 * @notes    This function is different from dot3adStaticCapabilitySet() which 
 *           used to set the static capability for the entire system. That is 
 *           why the first argument is the LAG's internal interface number. 
 * @end
 *******************************************************************************/
L7_RC_t dot3adLagStaticModeSet(L7_uint32 intIfNum, L7_uint32 isStatic)
{
  dot3ad_agg_t *a;
  L7_uint32 maxDynamicLag;
  L7_RC_t rc= L7_FAILURE;

  /* check if lag interface present */
  a = dot3adAggIntfFind(intIfNum);
  if (a == L7_NULLPTR)
  {
    return rc;
  }

  if ((isStatic != L7_TRUE) && ( isStatic != L7_FALSE))
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT3AD_COMPONENT_ID,
            "Invalid mode for static lags %s: %u!\n", ifName, isStatic);
    return rc;
  
  }


  /* check if static lag is being converted to dynamic*/
  if ((a->isStatic== L7_TRUE) && (isStatic == L7_FALSE))
  {
    /* if max dynamic lags are present */
    maxDynamicLag= platIntfDynamicLagIntfMaxCountGet();
    if (dot3adLagCnt.DynamicLagCnt >= maxDynamicLag)
    {
      return L7_TABLE_IS_FULL;
    }
  }
  rc = LACIssueCmd(AGG_STATIC_MODE, intIfNum, (void*)&isStatic);


  (void)osapiSemaTake(dot3adQueueSyncSema, L7_WAIT_FOREVER);
  return rc;
}

/*********************************************************************
 * @purpose  Determine if an interface is an active member of a LAG.
 *            
 * @param    intIfNum    internal interface number of interest
 *                            
 * @returns  L7_TRUE     interface is an active LAG member
 * @returns  L7_FALSE    interface not an active LAG member
 *
 * @notes    checks the aggWaitSelectedPortList
 *           
 *
 * @end
 *********************************************************************/
L7_BOOL dot3adIsLagActiveMember(L7_uint32 intIfNum)
{
  dot3ad_port_t *p;
  L7_uint32     i;
  dot3ad_agg_t *a;

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  if (p->actorPortWaitSelectedAggId != 0)
  {
    a = dot3adAggIntfFind(p->actorPortWaitSelectedAggId);
    if (a == L7_NULLPTR)
    {
      return L7_FALSE;
    }
    else
    {
      for (i = 0; i < a->activeNumMembers; i++)
      {
	if (intIfNum == a->aggActivePortList[i])
	{
	  return L7_TRUE;
	}
      }
      return L7_FALSE;
    }
  }
  else
    return L7_FALSE;

}

/*********************************************************************
 * @purpose  Get the current number of LAGs in the system (static and dynamic) 
 *            
 * @param    *currNumLags pointer to output location
 *                      
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    This is the number Lags present in the system
 *
 * @end
 *********************************************************************/
L7_RC_t dot3adLagCountGet(L7_uint32 *currNumLags)
{
  if (currNumLags != L7_NULLPTR)
  {
    *currNumLags= dot3adLagCnt.DynamicLagCnt + dot3adLagCnt.StaticLagCnt;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Sets the Port channel System Priority
 *
 * @param    priority        priority, 0-64k
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *       
 * @end
 *********************************************************************/
L7_RC_t dot3adPortChannelSystemPrioritySet( L7_uint32 priority)
{
  return aggActorSystemPrioritySet(0, priority);
}
