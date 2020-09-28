/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot3adlac.h
* @purpose     802.3ad link aggregation, LAC private functions
* @component   dot3ad
* @comments    none
* @create      06/04/2001
* @author      djohnson
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef INCLUDE_DOT3AD_LAC_H
#define INCLUDE_DOT3AD_LAC_H

#include "nimapi.h"
#include "l7_cnfgr_api.h"
#include "dot3ad_exports.h"
#include "dot3ad_db.h"

#define DOT3AD_MSG_COUNT 1024


#define DOT3AD_MSG_SIZE   sizeof(dot3adMsg_t)


typedef struct
{
  L7_uint32 lcpdu;  /* to be determined */
} dot3adLACPDU_t;


/*********************************************************************
* @purpose  structure which will be used as an overlay on the received LACPDU 
* @purpose  
*
* @notes    none     
* 
*********************************************************************/
typedef struct dot3ad_pdu_s
{
  L7_enetMacAddr_t  da;                     /* 6 bytes*/
  L7_enetMacAddr_t  sa;                     /* 6 bytes*/

  L7_ushort16       lengthType;             /* 2 bytes*/
  L7_uchar8         subType;                /* 1 byte */
  L7_uchar8         verNum;                 /* 1 byte */

  L7_uchar8         tvlTypeActor;         /* 1 byte */
  L7_uchar8         actorInfoLength;      /* 1 byte */
  L7_ushort16       actorSysPri;          /* 2 bytes*/
  L7_enetMacAddr_t  actorSys;              /* 6 bytes*/
  L7_ushort16       actorKey;              /* 2 bytes*/
  L7_ushort16       actorPortPri;         /* 2 bytes*/
  L7_ushort16       actorPort;             /* 2 bytes*/
  L7_uchar8         actorState;            /* 1 byte */
  L7_uchar8         actorReserved[3];      /* 3 bytes*/

  L7_uchar8         tvlTypePartner;       /* 1 byte */
  L7_uchar8         partnerInfoLength;    /* 1 byte */
  L7_ushort16       partnerSysPri;        /* 2 bytes*/
  L7_enetMacAddr_t  partnerSys;            /* 6 bytes*/
  L7_ushort16       partnerKey;            /* 2 bytes*/
  L7_ushort16       partnerPortPri;       /* 2 bytes*/
  L7_ushort16       partnerPort;           /* 2 bytes*/
  L7_uchar8         partnerState;          /* 1 byte */
  L7_uchar8         partnerReserved[3];    /* 3 bytes*/

  L7_uchar8         tvlTypeCollector;     /* 1 byte */
  L7_uchar8         collectorInfoLength;  /* 1 byte */
  L7_ushort16       collectorMaxDelay;    /* 2 bytes*/
  L7_uchar8         collectorReserved[12]; /* 12 bytes*/

  L7_uchar8         tvlTypeTerminator;    /* 1 byte */
  L7_uchar8         terminatorLength;      /* 1 byte */

  L7_uchar8         reserved[50];           /* 50 bytes*/
  L7_uchar8         fcs[4];                 /* 4 bytes*/


} dot3ad_pdu_t;

typedef struct
{
  L7_uint32 status;
  L7_BOOL   updateConfig;
} dot3adAdminMode_t;

typedef struct
{
  L7_uint32 event;
  L7_uint32 intf;
  union
  {
    dot3adLACPDU_t   lacpdu;
    L7_uint32        p;    /* or key or priority */
    dot3adAdminMode_t adminMode;
    L7_uint32        blockedState;  /* PTin added: Blocked state */
    L7_enetMacAddr_t mac;
    L7_uchar8        state;
    void            *bufHandle;
    L7_uchar8        name[L7_DOT3AD_MAX_NAME];
    NIM_EVENT_COMPLETE_INFO_t nimInfo;
    L7_CNFGR_CMD_DATA_t cnfgrInfo;
  } intfData;
} dot3adMsg_t;

typedef enum
{
 PERIODIC_EXPIRED,
 NTT_EXPIRED,
 CURRENT_EXPIRED,
 WAIT_EXPIRED

}dot3adTimer_t;

typedef struct
{
  L7_uint32 msgId;
  L7_uint32 parm1;
  L7_uint32 parm2;

} dot3adTimerMsg_t;



#ifdef DOT3AD_STATS
static L7_uint32 dot3ad_counters_port[] =
{
  L7_PLATFORM_CTR_LAC_PDUS_RX,
  L7_PLATFORM_CTR_MARKER_PDUS_RX,
  L7_PLATFORM_CTR_UNKNOWN_RX,
  L7_PLATFORM_CTR_ILLEGAL_RX,
  L7_PLATFORM_CTR_LACP_PDUS_TX,
  L7_PLATFORM_CTR_MARKER_RESPONSE_PDUS_TX
};
#endif

/* bytes in PortList */
#define DOT3AD_PORT_INDICES ((L7_MAX_INTERFACE_COUNT - 1) / (sizeof(L7_uchar8) * 8) + 1)

typedef struct
{
  L7_uchar8 value[DOT3AD_PORT_INDICES];
} DOT3AD_PORTLIST_t;

/* SETMASKBIT turns on bit index # k in mask j. */
#define DOT3AD_PORT_SETMASKBIT(j, k)                     \
            ((j).value[(((k)-1)/(8*sizeof(L7_uchar8)))]    \
                         |= 1 << (((k)-1) % (8*sizeof(L7_uchar8))))



/* CLRMASKBIT turns off bit index # k in mask j. */
#define DOT3AD_PORT_CLRMASKBIT(j, k)                   \
           ((j).value[(((k)-1)/(8*sizeof(L7_uchar8)))]   \
                        &= ~(1 << (((k)-1) % (8*sizeof(L7_uchar8)))))

/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define DOT3AD_PORT_ISMASKBITSET(j, k)             \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << (((k)-1) % (8*sizeof(L7_char8)))) )


/* Majority of functions get or set information in the LAC Database */


/*********************************************************************
* @purpose  Send a command to LAC
*
* @param    event       Event type
* @param    *data       pointer to data
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Command will be queued for service
*
* @end
*********************************************************************/
L7_RC_t LACIssueCmd(L7_uint32 event, L7_uint32 intf, void* data);

/*********************************************************************
* @purpose  Save the data in a message to a shared memory
*
* @param    event       Event type
* @param    *data       pointer to data
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Once the message is serviced, this variable size data will
*           be retrieved
*
* @end
*********************************************************************/
L7_RC_t dot3adFillMessage(void *data, dot3adMsg_t *msg);

/*********************************************************************
* @purpose  Get data pointed to by *data of size size from shared mem
*
* @param    event       Event type
* @param    *data       pointer to data
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adGetMessage(void *data, L7_uchar8 *dest);


/*********************************************************************
* @purpose  Handles events generated by NIM
*
* @param    intIfNum      interface number
* @param    event         event
* @param    correlator    correlator for the event
* @returns  L7_SUCCESS    if successful
*           L7_FAILURE    otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Process the startup callback notification for all the interfaces from NIM
*
* @param    NIM_STARTUP_PHASE_t the startup phase NIM_INTERFACE_CREATE_STARTUP for create
*           NIM_INTERFACE_ACTIVATE_STARTUP for activate.
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dot3adIntfStartupCallback(NIM_STARTUP_PHASE_t startup_phase);

/*********************************************************************
* @purpose  dot3ad task which serves the request queue
*
* @param
*
* @returns  void
*
* @notes    User-interface writes and LACPDUs are serviced off
*           of the dot3ad_queue
*
* @end
*********************************************************************/
void dot3ad_lac_task();

/*********************************************************************
* @purpose  Process an ACTIVATE callback during startup
*
* @param    
*
* @returns  L7_SUCCESS   if succesful
*
* @notes    Get a list of valid interfaces from NIM, for each interface
*           If the restart is warm use check pointed data and reconcile
*           if not perform a cold restart on that interface.
*           notify nim about interested events to begin receiving those notifications
*       
* @end
*********************************************************************/
L7_RC_t dot3adNimActivateStartup();

/*********************************************************************
* @purpose  Perform a cold start on an interface
*
* @param    NIM_STARTUP_PHASE_t the startup phase NIM_INTERFACE_CREATE_STARTUP for create
*           NIM_INTERFACE_ACTIVATE_STARTUP for activate.
*
* @returns  void
*
* @notes    Clear out all the known information for an interface (if any)
*           and initiate the structure with default values.
*       
* @end
*********************************************************************/
void dot3adIntfColdRestart(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Route the event to a handling function and grab the parms
*
* @param    event       Event type
* @param    intf        Interface
* @param    *data       pointer to data
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t LACDispatchCmd(dot3adMsg_t msg);

/*********************************************************************
* @purpose  Enables collection distribution on an interface
*
* @param    intf         internal interface that was changed
*
* @returns  L7_SUCCESS
*           L7_FALIURE
*
* @notes    Refreshes the active list and sends it to DTL
*           --adds this interface from the active list
*
* @end
*********************************************************************/
L7_RC_t aggCollDistEnable(L7_uint32 intf);
/*********************************************************************
* @purpose  Disable collection distribution on an interface
*
* @param    intf         internal interface that was changed
*
* @returns  L7_SUCCESS
*           L7_FALIURE
*
* @notes    Refreshes the active list and sends it to DTL
*           --removes this interface from the active list
*
*
* @end
*********************************************************************/
L7_RC_t aggCollDistDisable(L7_uint32 intf);

/*********************************************************************
* @purpose  Given a ports interface number, attach it to its selected
*           aggregator
*
* @param    intf         aggregator interface
*
* @returns  L7_SUCCESS   if all members attached
*           L7_FALIURE   if failure (agg doesn't exist)
*
* @notes    If attached list has members (configured) that aren't
*           yet functioning in the aggregation, this will add them in
*
* @end
*********************************************************************/
L7_RC_t aggLACAttach(L7_uint32 agg_intf);

/*********************************************************************
* @purpose  Decodes and services an incoming request from the stats
*           manager for a dot3ad statistic
*
* @param    statHandle   Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*
* @end
*********************************************************************/
void dot3adStatGet(void *statHandle);

/*********************************************************************
* @purpose  Gets the aggIndex from agg internal Interface number
*
* @param    agg_intf     aggregator internal interface number
* @param    *aggIndex    pointer to aggIndex value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    AggIndex ranges from 0 to L7_MAX_NUM_LAG_INTF -1.
*
* @end
*********************************************************************/
L7_RC_t aggIndexFromIntfNumGet(L7_uint32 aggIntIfNum, L7_uint32 *aggIndex);

/*********************************************************************
* @purpose  Gets the internal Interface number given an agg Index
*
* @param    aggIndex    the index of the aggregator 
* @param    *agg_intf    pointer to aggregator internal interface number value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    AggIndex ranges from 0 to L7_MAX_NUM_LAG_INTF -1. 
*           (It is different from the aggId 1 - L7_MAX_NUM_LAG_INTF.)
*
* @end
*********************************************************************/
L7_RC_t aggIfNumFromIndexGet(L7_uint32 aggIndex, L7_uint32 * aggIntIfNum);
/*L7_RC_t dot3adPortGet(L7_uint32 port, dot3ad_port_t *p);*/

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
L7_RC_t tablesLastChangedGet(L7_timespec *time);

L7_RC_t aggEntryGet(void);
L7_RC_t aggEntryGetNext(void);

/*********************************************************************
* @purpose  Sets the aggregator in use flag.  If an aggregator is
*           in-use, it has been created but may not be active or have
*           members.
*
* @param    agg_intf     aggregator internal interface number
* @param    status       in use or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggInuseSet(L7_uint32 agg_intf, L7_BOOL status);

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
L7_RC_t aggInuseGet(L7_uint32 agg_intf, L7_BOOL *status);

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
L7_RC_t aggPortNameGet(L7_uint32 agg_intf, L7_uchar8 *name);

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
L7_RC_t aggPortNameSet(L7_uint32 agg_intf, L7_uchar8 *name);

/*********************************************************************
* @purpose  Sets the Hash Mode for a LAG.
*
* @param    agg_intf     aggregator internal interface number
* @param    hashMode     Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggPortHashModeSet(L7_uint32 agg_intf, L7_uint32 hashMode);

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
L7_RC_t aggPortHashModeGet(L7_uint32 agg_intf, L7_uint32 * hashMode);

/*********************************************************************
* @purpose  Gets the MAC address assigned to the aggregator
*
* @param    InterfaceIndex  interface
* @param    *mac            MAC address, L7_MAC_ADDR_LEN in length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggMACAddressGet(L7_uint32 InterfaceIndex, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Gets the priority value associated with the system's actor ID
*
* @param    InterfaceIndex  interface
* @param    *priority       priority, 0-64k
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggActorSystemPriorityGet(L7_uint32 InterfaceIndex, L7_uint32 *priority);

/*********************************************************************
* @purpose  Sets the priority value associated with the system's actor ID
*
* @param    InterfaceIndex  interface
* @param    priority        priority, 0-64k
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggActorSystemPrioritySet(L7_uint32 InterfaceIndex, L7_uint32 priority);

/*********************************************************************
* @purpose  Gets the MAC address used as a unique identifier for the
*           system that contains this aggregator
*
* @param    InterfaceIndex  interface
* @param    *mac            MAC identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggActorSystemIDGet(L7_uint32 InterfaceIndex, L7_enetMacAddr_t *mac);

/*********************************************************************
* @purpose  Determines if the aggregator represents an aggregate or
*           an individual link
*
* @param    InterfaceIndex  interface
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
L7_RC_t aggActorAggregateOrIndividualGet(L7_uint32 InterfaceIndex, L7_BOOL *link);

/*********************************************************************
* @purpose  Gets the current administrative value of the Key for the
*           aggregator
*
* @param    InterfaceIndex  interface
* @param    *key            key value, 16bit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    admin and oper key may differ
*
* @end
*********************************************************************/
L7_RC_t aggActorAdminKeyGet(L7_uint32 InterfaceIndex, L7_uint32 *key);

/*********************************************************************
* @purpose  Sets the current administrative value of the Key for the
*           aggregator
*
* @param    InterfaceIndex  interface
* @param    key             key value, 16bit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    admin and oper key may differ
*
* @end
*********************************************************************/
L7_RC_t aggActorAdminKeySet(L7_uint32 InterfaceIndex, L7_uint32 key);

/*********************************************************************
* @purpose  Gets the current operational value of the Key for the
*           aggregator
*
* @param    InterfaceIndex  interface
* @param    *key            key value, 16bit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    admin and oper key may differ
*
* @end
*********************************************************************/
L7_RC_t aggActorOperKeyGet(L7_uint32 InterfaceIndex, L7_uint32 *key);

/*********************************************************************
* @purpose  MAC address of current partner of aggregator
*
* @param    InterfaceIndex  interface
* @param    *mac            MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    a value 0 indicates there is no known partner
*
* @end
*********************************************************************/
L7_RC_t aggPartnerSystemIDGet(L7_uint32 InterfaceIndex, L7_uint32 *mac);

/*********************************************************************
* @purpose  Priority value associated with partner's system ID
*
* @param    InterfaceIndex  interface
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
L7_RC_t aggPartnerSystemPriorityGet(L7_uint32 InterfaceIndex, L7_uint32 *priority);

/*********************************************************************
* @purpose  The current operational value of the key for the aggregators
*           current protocol partner. aggregator parser and either
*           delivering it to a MAC client or discarding it
*
* @param    InterfaceIndex  interface
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
L7_RC_t aggPartnerOperKeyGet(L7_uint32 InterfaceIndex, L7_uint32 *key);

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
L7_RC_t aggCollectorMaxDelayGet(L7_uint32 intf, L7_uint32 *delay);

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
L7_RC_t aggCollectorMaxDelaySet(L7_uint32 intf, L7_uint32 delay);

L7_RC_t aggPortIndexGet(void);
L7_RC_t aggPortIndexGetNext(void);

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
L7_RC_t aggPortActorSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t aggPortActorSystemPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t aggPortActorSystemIDGet(L7_uint32 intf, L7_uint32 *mac);

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
L7_RC_t aggPortActorAdminKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t aggPortActorAdminKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t aggPortActorOperKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t aggPortActorOperKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t aggPortPartnerAdminSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);


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
L7_RC_t aggPortPartnerAdminSystemPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t aggPortPartnerOperSystemPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t aggPortPartnerAdminSystemIDGet(L7_uint32 intf, L7_enetMacAddr_t *mac);

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
L7_RC_t aggPortPartnerAdminSystemIDSet(L7_uint32 intf, L7_enetMacAddr_t mac);

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
L7_RC_t aggPortPartnerOperSystemIDGet(L7_uint32 intf, L7_enetMacAddr_t *mac);

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
L7_RC_t aggPortPartnerOperSystemIDSet(L7_uint32 intf, L7_enetMacAddr_t mac);

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
L7_RC_t aggPortPartnerAdminKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t aggPortPartnerAdminKeySet(L7_uint32 intf, L7_uint32 key);

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
L7_RC_t aggPortPartnerOperKeyGet(L7_uint32 intf, L7_uint32 *key);

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
L7_RC_t aggPortSelectedAggIDGet(L7_uint32 intf, L7_uint32 *agg);

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
L7_RC_t aggPortAttachedAggIDGet(L7_uint32 intf, L7_uint32 *id);

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
L7_RC_t aggPortActorPortGet(L7_uint32 intf, L7_uint32 *port);

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
L7_RC_t aggPortActorPortPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t aggPortActorPortPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t aggPortPartnerAdminPortGet(L7_uint32 intf, L7_uint32 *port);

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
L7_RC_t aggPortPartnerAdminPortSet(L7_uint32 intf, L7_uint32 port);

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
L7_RC_t aggPortPartnerOperPortGet(L7_uint32 intf, L7_uint32 *port);

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
L7_RC_t aggPortPartnerAdminPortPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t aggPortPartnerAdminPortPrioritySet(L7_uint32 intf, L7_uint32 priority);

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
L7_RC_t aggPortPartnerOperPortPriorityGet(L7_uint32 intf, L7_uint32 *priority);

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
L7_RC_t aggPortActorAdminStateGet(L7_uint32 intf, L7_uchar8 *state);

/* PTin added: Blocked state */
#if 1
/**
 * Set a new select state for a dynamic LAG
 * 
 * @param intf : intIfNum
 * @param state : UNSELECTED/SELECTED/STANDBY
 * 
 * @return L7_RC_t 
 */
L7_RC_t aggPortActorStandby(L7_uint32 intf, L7_uchar8 state);
#endif

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
L7_RC_t aggPortActorAdminStateSet(L7_uint32 intf, L7_uchar8 state);

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
L7_RC_t aggPortPartnerAdminStateGet(L7_uint32 intf, L7_uchar8 *state);

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
L7_RC_t aggPortPartnerAdminStateSet(L7_uint32 intf, L7_uchar8 state);

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
L7_RC_t aggPortActorOperStateGet(L7_uint32 intf, L7_uchar8 *state);

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
L7_RC_t aggPortPartnerOperStateGet(L7_uint32 intf, L7_uchar8 *state);

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
L7_RC_t aggPortAggregateOrIndividualGet(L7_uint32 intf, L7_BOOL *state);

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
L7_RC_t aggPortStatsLACPDUsRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortStatsMarkerPDUsRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortStatsMarkerResponsePDUsRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortStatsUnknownRxGet(L7_uint32 port, L7_uint32 *stat);

/*********************************************************************
* @purpose  Get the number badly formed PDUs
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
L7_RC_t aggPortStatsIllegalRxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortStatsLACPDUsTxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortStatsMarkerPDUsTxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortStatsMarkerResponsePDUsTxGet(L7_uint32 port, L7_uint32 *stat);

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
L7_RC_t aggPortListGet(L7_uint32 agg_intf, L7_uchar8 *list, L7_uint32 *length);


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
L7_RC_t aggPortDebugRxStateGet(L7_uint32 port, L7_uchar8 *state);

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
L7_RC_t aggPortDebugLastRxTimeGet(L7_uint32 port, L7_timespec *time);

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
L7_RC_t aggPortDebugMuxStateGet(L7_uint32 port, L7_uchar8 *state);

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
L7_RC_t aggPortDebugMuxReasonGet(L7_uint32 port, L7_char8 *reason, L7_uint32 *length);

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
L7_RC_t aggPortDebugActorChurnStateGet(L7_uint32 port, L7_uchar8 *churnstate);

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
L7_RC_t aggPortDebugPartnerChurnStateGet(L7_uint32 port, L7_uchar8 *churnstate);

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
L7_RC_t aggPortDebugActorChurnCountGet(L7_uint32 port, L7_uint32 *churncount);

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
L7_RC_t aggPortDebugPartnerChurnCountGet(L7_uint32 port, L7_uint32 *churncount);

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
L7_RC_t aggPortDebugActorSyncTransitionCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t aggPortDebugPartnerSyncTransitionCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t aggPortDebugActorChangeCountGet(L7_uint32 port, L7_uint32 *count);

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
L7_RC_t aggPortDebugPartnerChangeCountGet(L7_uint32 port, L7_uint32 *count);



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
L7_RC_t aggPortLacpEnabledGet(L7_uint32 port, L7_BOOL *status);

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
L7_RC_t aggPortLacpEnabledSet(L7_uint32 port, L7_BOOL status);

/*********************************************************************
* @purpose  Forwards a LACPDU onto LACP
*
* @param    intf            interface number received on
* @param    bufHandle       handle to the PDU location in DTL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if port not found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t LACPDUReceive(L7_uint32 intf, void * bufHandle);
/*********************************************************************
* @purpose  Reset all default values in the port entry
*
* @param    p            pointer to the port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if port not found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggPortResetValues(dot3ad_port_t *p);
/*********************************************************************
* @purpose  Deletes Port from a LAG
*
* @param    intIfNum            Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if port not found
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t aggPortDelete(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Sets the aggregator admin mode flag.  If an aggregator is
*           disabled then all the member ports will not be forwarding
*           traffic. lacpdus will be exchanged to maitain the aggregation.
*
* @param    agg_intf     aggregator internal interface number
* @param    status       L7_ENABLE or L7_DISABLE
* @param    updateConfig L7_FALSE if the mode is only operationally changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggAdminModeSet(L7_uint32 agg_intf, L7_uint32 status, L7_BOOL updateConfig);

/*********************************************************************
* @purpose  Gets the aggregator admin mode flag.
*
* @param    agg_intf     aggregator internal interface number
* @param    *status      admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggAdminModeGet(L7_uint32 agg_intf, L7_uint32 *status);

/* PTin added: Blocked state */
#if 1
/*********************************************************************
* @purpose  Sets the aggregator Blocked state flag.
*
* @param    agg_intf     aggregator internal interface number
* @param    status       L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t aggBlockedStateSet(L7_uint32 agg_intf, L7_int status);

/*********************************************************************
* @purpose  Gets the aggregator blocked state flag.
*
* @param    agg_intf     aggregator internal interface number
* @param    *status      blocked state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggBlockedStateGet(L7_uint32 agg_intf, L7_uint32 *status);
#endif

/*********************************************************************
* @purpose  Deletes configured LAGs
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t aggConfiguredLagsRemove();

/*********************************************************************
* @purpose  Records the current system time as the dot3adTablesLastChanged time
*
* @param    none
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void dot3adTablesLastChangedRecord(void);

/**************************************************************************
*
* @purpose    Actions to be performed when the current while timer expires
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes    Put this timer expiry message on a timer queue, thus returning
*           immediately to the timer task that called this routine. This will
*           avoid any processing on the timer thread.
*
* @end
*
*************************************************************************/
void dot3adCurrentWhileTimerExpiredToQueue(L7_uint64 portNum, L7_uint64 nullParm);

/**************************************************************************
*
* @purpose    Actions to be performed when the periodic timer expires
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes    Put this timer expiry message on a timer queue, thus returning
*           immediately to the timer task that called this routine. This will
*           avoid any processing on the timer thread.
*
* @end
*
*************************************************************************/
void dot3adPeriodicTimerExpiredToQueue(L7_uint64 portNum, L7_uint64 nullParm);

/**************************************************************************
*
* @purpose    Actions to be performed when the Wait While timer expires
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes    Put this timer expiry message on a timer queue, thus returning
*           immediately to the timer task that called this routine. This will
*           avoid any processing on the timer thread.
* @end
*
*************************************************************************/
void dot3adWaitWhileTimerExpiredToQueue(L7_uint64 portNum, L7_uint64 nullParm);

/**************************************************************************
*
* @purpose    Sets the txCount to zero
*
* @param      portNum      internal interface number of the port
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes    Put this timer expiry message on a timer queue, thus returning
*           immediately to the timer task that called this routine. This will
*           avoid any processing on the timer thread.
*
* @end
*
*************************************************************************/
void dot3adNttTimerExpiredToQueue(L7_uint64 portNum, L7_uint64 nullParm);

/*********************************************************************
* @purpose  dot3ad timer task which serves the expired timers
*
* @param
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void dot3ad_timer_task();


/*********************************************************************
* @purpose  dot3ad timer task which serves the expired timers
*
* @param    
*
* @returns  void
*
* @notes    
*           
*       
* @end
*********************************************************************/
void dot3ad_helper_timer_task();

/*********************************************************************
* @purpose  dot3ad core transmit task.
*
* @param    
*
* @returns  void
*
* @notes   This task serves as a conduit between the core and the helper(s).
*          It distributes the information from the core to the appropriate 
*          helper w/o blocking the core.
*           
*       
* @end
*********************************************************************/
void dot3ad_transport_task();

/**************************************************************************
*
* @purpose  Resets default values of admin in the operational values
*
* @param   *p   pointer to the agg. port structure
*
* @returns  L7_SUCCESS -always as this function copies default information
*
* @notes    only the partner values are defaulted
*
* @end
*
*************************************************************************/
L7_RC_t dot3adActivePortResetDefault(dot3ad_port_t *p);

/*********************************************************************
* @purpose  Set the Static or Dynamic mode for the LAG/aggregator.
*
* @param    agg_intf    Internal aggregator interface
*           mode        Static or non-Static/Dynamic mode for the LAG
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aggStaticModeSet(L7_uint32 agg_intf, L7_uint32 mode);

L7_RC_t dot3adConfiguredLagMemberDelete(L7_uint32 intIfNum,
                                        L7_uint32 count,
                                        L7_uint32 *pMemIntf);
L7_RC_t dot3adConfiguredLagRemove(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Create a lag for the specific LAG_ID
*
* @param    lagID         the lag ID internal to LAG
* @param    name          the name assigned to the LAG
* @param    members       the member list
* @param    adminMode     the admin state of the interface
* @param    linkTrapMode  the link trap state of the interface
* @param    hashMode      the Hash Mode value, enumerated by L7_DOT3AD_HASH_MODE_t
* @param    pIntfIfNum    the returned intIfNum of the interface
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot3adPrivateLagCreate(L7_uint32 lagId, L7_char8 *name, L7_uint32 members[],
                               L7_uint32 adminMode, L7_uint32 linkTrapMode,
                               L7_BOOL cfgCreated, L7_uint32 hashMode, L7_uint32 *pIntIfNum);

#endif /* INCLUDE_DOT3AD_LAC_H*/
