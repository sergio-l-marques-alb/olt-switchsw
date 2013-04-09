/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot3ad_api.h
* @purpose     802.3ad link aggregation API definitions and functions
* @component   dot3ad
* @comments    none
* @create      05/30/2001
* @author      djohnson
* @end
*             
**********************************************************************/

#ifndef _DOT3AD_API_H_
#define _DOT3AD_API_H_

#include <l7_common.h>
#include <osapi.h>


/* DOT3AD Link Aggregation Statistics */

#define L7_PLATFORM_CTR_LAC_PDUS_RX                         ((L7_DOT3AD_COMPONENT_ID << 16) | 0x0000)
#define L7_PLATFORM_CTR_MARKER_PDUS_RX                      ((L7_DOT3AD_COMPONENT_ID << 16) | 0x0001)
#define L7_PLATFORM_CTR_UNKNOWN_RX                          ((L7_DOT3AD_COMPONENT_ID << 16) | 0x0002)
#define L7_PLATFORM_CTR_ILLEGAL_RX                          ((L7_DOT3AD_COMPONENT_ID << 16) | 0x0003)
#define L7_PLATFORM_CTR_LACP_PDUS_TX                        ((L7_DOT3AD_COMPONENT_ID << 16) | 0x0004)
#define L7_PLATFORM_CTR_MARKER_RESPONSE_PDUS_TX             ((L7_DOT3AD_COMPONENT_ID << 16) | 0x0005)


/*--------------------------------------*/
/*  LAG Public Structures               */
/*--------------------------------------*/
#define DOT3AD_MAX_NAME          (15 +1)
#include "osapi.h"
/* LAC Event List */
typedef enum
{
  AGG_ACTOR_SYSTEM_PRIORITY = 1000,
  AGG_ACTOR_ADMIN_KEY,
  AGG_COLLECTOR_MAX_DELAY,
  AGG_NAME,
  PORT_ACTOR_SYSTEM_PRIORITY,
  PORT_ACTOR_ADMIN_KEY,
  PORT_ACTOR_OPER_KEY,
  PORT_PARTNER_ADMIN_SYSTEM_PRIORITY,
  PORT_PARTNER_ADMIN_SYSTEM_ID,
  PORT_PARTNER_OPER_SYSTEM_ID,
  PORT_PARTNER_ADMIN_KEY,
  PORT_ACTOR_PORT_PRIORITY,
  PORT_PARTNER_ADMIN_PORT,
  PORT_PARTNER_ADMIN_PORT_PRIORITY,
  PORT_ACTOR_ADMIN_STATE,
  PORT_PARTNER_ADMIN_STATE,
  LACPDU_RECEIVE,
  AGG_INUSE,
  AGG_ADMIN_MODE,
  /* change LAG_STATIC_MODE to AGG_STATIC_MODE as static mode is per lag */
  AGG_STATIC_MODE,
  AGG_HASHMODE_SET,
  NIM_INTF_CHANGE
} lac_event_t;

/* 43.4.2.2 */
  #define DOT3AD_STATE_EXPIRED          0x80
  #define DOT3AD_STATE_DEFAULTED        0x40
  #define DOT3AD_STATE_DISTRIBUTING     0x20
  #define DOT3AD_STATE_COLLECTING       0x10
  #define DOT3AD_STATE_SYNCHRONIZATION  0x8
  #define DOT3AD_STATE_AGGREGATION      0x4
  #define DOT3AD_STATE_LACP_TIMEOUT     0x2
  #define DOT3AD_STATE_LACP_ACTIVITY    0x1

/* Non-SNMP gets/sets */
L7_RC_t dot3adLACPDUReceive(L7_uint32 port, void *data);


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
L7_RC_t dot3adTablesLastChangedGet(L7_timespec *time);

/*********************************************************************
* @purpose  Check the validity of an aggregator interface
*
* @param    intf        internal interface number
*
* @returns  L7_SUCCESS, if a the first was found
* @returns  L7_FAILURE, if a the first doesn't exist
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggEntryGet(L7_uint32 intf);

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
L7_RC_t dot3adAggEntryGetNext(L7_uint32 intf, L7_uint32 *next);

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
L7_RC_t dot3adAggIfNumFromIndexGet(L7_uint32 index, L7_uint32 *intIfNum);

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
L7_RC_t dot3adAggIndexFromIfNumGet(L7_uint32 intIfNum, L7_uint32 *index);

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
L7_RC_t dot3adAggMACAddressGet(L7_uint32 intf, L7_uchar8 *mac);

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
L7_RC_t dot3adAggInuseSet(L7_uint32 agg_intf, L7_BOOL status);

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
L7_RC_t dot3adAggInuseGet(L7_uint32 agg_intf, L7_BOOL *status);

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
L7_RC_t dot3adAggActorSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggActorSystemPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t dot3adAggActorSystemIDGet(L7_uint32 intf, L7_char8 *mac);

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
L7_RC_t dot3adAggActorAggregateOrIndividualGet(L7_uint32 intf, L7_BOOL *link);

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
L7_RC_t dot3adAggActorAdminKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggActorAdminKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t dot3adAggActorOperKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggPartnerSystemIDGet(L7_uint32 intf, L7_uint32 *mac);

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
L7_RC_t dot3adAggPartnerSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggPartnerOperKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggCollectorMaxDelayGet(L7_uint32 intf, L7_uint32 *delay);

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
L7_RC_t dot3adAggCollectorMaxDelaySet(L7_uint32 intf, L7_uint32 delay);


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
L7_RC_t dot3adAggPortIndexGet(L7_uint32 intf);

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
L7_RC_t dot3adAggPortIndexGetNext(L7_uint32 intf, L7_uint32 *next);

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
L7_RC_t dot3adAggPortActorSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggPortActorSystemPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t dot3adAggPortActorSystemIDGet(L7_uint32 intf, L7_uint32 *mac);

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
L7_RC_t dot3adAggPortActorAdminKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggPortActorAdminKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t dot3adAggPortActorOperKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggPortActorOperKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t dot3adAggPortPartnerAdminSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);


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
L7_RC_t dot3adAggPortPartnerAdminSystemPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t dot3adAggPortPartnerOperSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggPortPartnerAdminSystemIDGet(L7_uint32 intf, L7_enetMacAddr_t *mac);

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
L7_RC_t dot3adAggPortPartnerAdminSystemIDSet(L7_uint32 intf, L7_enetMacAddr_t mac);

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
L7_RC_t dot3adAggPortPartnerOperSystemIDGet(L7_uint32 intf, L7_enetMacAddr_t *mac);

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
L7_RC_t dot3adAggPortPartnerOperSystemIDSet(L7_uint32 intf, L7_uint32 mac);

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
L7_RC_t dot3adAggPortPartnerAdminKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggPortPartnerAdminKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t dot3adAggPortPartnerOperKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t dot3adAggPortSelectedAggIDGet(L7_uint32 intf, L7_uint32 *agg);

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
L7_RC_t dot3adAggPortAttachedAggIDGet(L7_uint32 intf, L7_uint32 *id);

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
L7_RC_t dot3adAggPortActorPortGet(L7_uint32 intf, L7_uint32 *port);

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
L7_RC_t dot3adAggPortActorPortPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggPortActorPortPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t dot3adAggPortPartnerAdminPortGet(L7_uint32 intf, L7_uint32 *port);

/*********************************************************************
* @purpose  Get the administrative value of the port number for the partner
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
L7_RC_t dot3adAggPortPartnerAdminPortSet(L7_uint32 intf, L7_uint32 port);

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
L7_RC_t dot3adAggPortPartnerOperPortGet(L7_uint32 intf, L7_uint32 *port);

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
L7_RC_t dot3adAggPortPartnerAdminPortPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggPortPartnerAdminPortPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t dot3adAggPortPartnerOperPortPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t dot3adAggPortActorAdminStateGet(L7_uint32 intf, L7_uchar8 *state);

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
L7_RC_t dot3adAggPortActorAdminStateSet(L7_uint32 intf, L7_uchar8 state);

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
L7_RC_t dot3adAggPortActorOperStateGet(L7_uint32 intf, L7_uchar8 *state); 

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
L7_RC_t dot3adAggPortPartnerOperStateGet(L7_uint32 intf, L7_uchar8 *state);

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
L7_RC_t dot3adAggPortAggregateOrIndividualGet(L7_uint32 intf, L7_BOOL *state);

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
L7_RC_t dot3adAggPortStatsLACPDUsRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsMarkerPDUsRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsMarkerResponsePDUsRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsUnknownRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsIllegalRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsLACPDUsTxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsMarkerPDUsTxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t dot3adAggPortStatsMarkerResponsePDUsTxGet(L7_uint32 port, L7_uint32 *stat);

/*********************************************************************
* @purpose  Clears the port statistics. 
*          
* @param    intIfNum @b{(input)} Internal Interface Number of the port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Clears all the LACPDU stats associated with this port. 
*
* @end
*********************************************************************/
L7_RC_t dot3adPortStatsClear(L7_uint32 intIfNum);

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
L7_RC_t dot3adAggPortListGet(L7_uint32 agg_intf, L7_uchar8 *list, L7_uint32 *length);

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
L7_RC_t dot3adAggPortDebugRxStateGet(L7_uint32 port, L7_uchar8 *state);

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
L7_RC_t dot3adAggPortDebugLastRxTimeGet(L7_uint32 port, L7_timespec *time);

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
L7_RC_t dot3adAggPortDebugMuxStateGet(L7_uint32 port, L7_uchar8 *state);

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
L7_RC_t dot3adAggPortDebugMuxReasonGet(L7_uint32 port, L7_char8 *reason, L7_uint32 *length);

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
L7_RC_t dot3adAggPortDebugActorChurnStateGet(L7_uint32 port, L7_uchar8 *churnstate);

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
L7_RC_t dot3adAggPortDebugPartnerChurnStateGet(L7_uint32 port, L7_uchar8 *churnstate);

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
L7_RC_t dot3adAggPortDebugActorChurnCountGet(L7_uint32 port, L7_uint32 *churncount);

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
L7_RC_t dot3adAggPortDebugPartnerChurnCountGet(L7_uint32 port, L7_uint32 *churncount);

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
L7_RC_t dot3adAggPortDebugActorSyncTransitionCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t dot3adAggPortDebugPartnerSyncTransitionCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t dot3adAggPortDebugActorChangeCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t dot3adAggPortDebugPartnerChangeCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t dot3adAggPortNameGet(L7_uint32 agg_intf, L7_uchar8 *name);

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
L7_RC_t dot3adAggPortNameSet(L7_uint32 agg_intf, L7_uchar8 *name);

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
L7_RC_t dot3adAggPortHashModeSet(L7_uint32 agg_intf, L7_uint32 hashMode);

/*********************************************************************
* @purpose  Gets the Hash Mode from LAG.
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
L7_RC_t dot3adAggPortHashModeGet(L7_uint32 agg_intf, L7_uint32 * hashMode);

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
L7_RC_t dot3adAggSystemHashModeSet(L7_uint32 hashMode);

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
L7_RC_t dot3adAggSystemHashModeGet(L7_uint32 * hashMode);

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
L7_RC_t dot3adAggPortPartnerAdminStateGet(L7_uint32 intf, L7_uchar8 *state);


/*********************************************************************
* @purpose  Set 8 bits corresponding to the current administrative 
*           value of the actor_state for the protocol partner
*
* @param    intf            interface
* @param    state           state
* @param    hashMode        Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    activity,timeout,aggregation,synch,collecting,distributing,
*           defaulted,expired
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggPortPartnerAdminStateSet(L7_uint32 intf, L7_uchar8 state);

L7_RC_t dot3adLagCreate(L7_char8 *name, L7_uint32 members[],
                        L7_uint32 adminMode, L7_uint32 linkTrapMode,
                        L7_uint32 hashMode, L7_uint32 *pIntIfNum);

L7_RC_t dot3adLagModify(L7_uint32 intIfNum, L7_char8 *name,          
                        L7_uint32 members[],
                        L7_uint32 adminMode, L7_uint32 linkTrapMode);

L7_RC_t dot3adLagRemove(L7_uint32 intIfNum);

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
                            L7_uint32 *pMemIntf);

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
                              L7_uint32 *pMemIntf);

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
* @notes    Only the configured members
*           
*
* @end
*********************************************************************/
L7_RC_t dot3adMemberListGet(L7_uint32 intIfNum, L7_uint32 *pListCount, 
                            L7_uint32 memberList[]);

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
L7_RC_t dot3adAdminModeSet(L7_uint32 intIfNum, L7_uint32 status, L7_BOOL updateConfig);

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
L7_RC_t dot3adAdminModeGet(L7_uint32 intIfNum, L7_uint32 *status);

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
L7_RC_t dot3adAggPortLacpEnabledSet(L7_uint32 port, L7_BOOL status);

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
L7_RC_t dot3adAggPortLacpEnabledGet(L7_uint32 port, L7_BOOL *status);

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
L7_RC_t dot3adLinkTrapModeSet(L7_uint32 intIfNum, L7_uint32 val);

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
L7_RC_t dot3adLinkTrapModeGet(L7_uint32 intIfNum, L7_uint32 *val);

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
L7_BOOL dot3adIsLagConfigured(L7_uint32 intIfNum);

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
L7_BOOL dot3adIsLagMember(L7_uint32 intIfNum);

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
L7_RC_t dot3adAggGet(L7_uint32 intIfNum, L7_uint32 *agg_intf);

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
L7_RC_t dot3adWhoisOwnerLag(L7_uint32 intIfNum, L7_uint32 *pLagIntfNum);

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
L7_BOOL dot3adIsLag(L7_uint32 intIfNum);
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
L7_RC_t dot3adLagDataRateGet(L7_uint32 intIfNum, L7_uint32 *pLagDataRate);

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
L7_RC_t dot3adLagNumMembersGet(L7_uint32 intIfNum, L7_uint32 *numMembers);
/*********************************************************************
* @purpose  Checks to see if a LAG interface is static or not
*
* @param    agg_intf      aggregator interface number
* @param    *isStatic     
*           
* @returns  L7_SUCCESS      
* @returns  L7_FAILURE      
*
* @notes    A LAG with no active members will have the static variale set to false
*       
* @end
*********************************************************************/
L7_RC_t dot3adIsStaticLag(L7_uint32 agg_intf, L7_BOOL *isStatic);
/*********************************************************************
* @purpose  Get the list of active member interfaces for the specified LAG interface.
*            
* @param    intIfNum       internal interface number of the target LAG
* @param    *pListCount    pointer to array element count
*                            (@b{Input:} max number of elements in caller's 
*                             memberList[] array)
*                            (@b{Output:} current number of LAG members)
* @param    memberList[]   array to be filled in with densely-packed list of
*                            active LAG member interface numbers
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
L7_RC_t dot3adActiveMemberListGet(L7_uint32 intIfNum, L7_uint32 *pListCount, 
                                  L7_uint32 memberList[]);


/******************************************************************************
* @purpose  Set the mode (static or not,i.e, dynamic) for this LAG
*
* @param    intIfNum     internal interface number of the LAG being modified
* @param    isStatic     sets if the LAG is static or dynamic:L7_TRUE/L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is different from dot3adStaticCapabilitySet() which 
*           used to set the static capability for the entire system. That is 
*           why the first argument is the LAG's internal interface number. 
* @end
*******************************************************************************/
L7_RC_t dot3adLagStaticModeSet(L7_uint32 intIfNum, L7_uint32 isStatic);


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
L7_BOOL dot3adIsLagActiveMember(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Validate whether the interface can participate in dot3ad
*
* @param    intIfNum  @b({input}) The internal interface number being checked
*
* @returns  L7_BOOL
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adIsValidIntf(L7_uint32 intIfNum);

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
L7_RC_t dot3adLagCountGet(L7_uint32 *currNumLags);

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
L7_RC_t dot3adLagActiveDataRateGet(L7_uint32 intIfNum, L7_uint32 *pLagDataRate);

#endif /* _DOT3AD_API_H_*/
