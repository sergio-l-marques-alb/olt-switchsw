
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3addb.h
*
* @purpose defines data structures and prototypes for the Data Base
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @comments none
*
* @create 6/5/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DOT3AD_DB_H
#define INCLUDE_DOT3AD_DB_H

#include "osapi.h"
#include "dot3ad_exports.h"
/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/
typedef struct dot3ad_usp_s
{
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
} dot3ad_usp_t;

typedef struct dot3ad_port_s
{
  struct dot3ad_port_s *next;
  struct dot3ad_port_s  *prev;

  L7_uint32         actorPortNum;
  L7_uint32         actorPortPri;
  L7_uint32         actorPortAggId;         /* intIfNum of aggregator when attached */
  L7_uint32         actorPortSelectedAggId; /* intIfNum of aggregator when selected */
  L7_uint32         actorPortWaitSelectedAggId;
  L7_BOOL           ntt;


  L7_uint32         actorAdminPortKey;      /* intIfNum */
  L7_uchar8         actorAdminPortState;

  L7_uint32         actorOperPortKey;       /* intIfNum */
  L7_uchar8         actorOperPortState;

  L7_enetMacAddr_t  partnerAdminSys;
  L7_uint32         partnerAdminSysPri;
  L7_uint32         partnerAdminKey;
  L7_uint32         partnerAdminPortNumber;
  L7_uint32         partnerAdminPortPri;
  L7_uchar8         partnerAdminPortState;

  L7_enetMacAddr_t  partnerOperSys;
  L7_uint32         partnerOperSysPri;
  L7_uint32         partnerOperKey;
  L7_uint32         partnerOperPortNumber;
  L7_uint32         partnerOperPortPri;
  L7_uchar8         partnerOperPortState;


  L7_BOOL           portEnabled;         

  L7_BOOL           begin;
  L7_BOOL           lacpEnabled;            /* to forward or drop LACPDUs */
  L7_BOOL           readyN;
  L7_uint32         selected;
  L7_BOOL           aggPortMoved;
  L7_BOOL           portIndividualAgg; /* L7_TRUE-aggregatable, L7_FALSE-individual, default */
  L7_BOOL           portAttached;
  L7_uint32         txCount; /*to maintain max of 3 tx pdus per fast periodic time*/

  L7_uint32         rxState;
  L7_uint32         perState;
  L7_uint32         muxState;

  osapiTimerDescr_t        *currentWhileTimer;
  osapiTimerDescr_t        *periodicTimer;
  osapiTimerDescr_t        *waitWhileTimer;
  osapiTimerDescr_t        *nttTimer;

} dot3ad_port_t;

typedef struct dot3adOperPort_s
{
  L7_uint32         linkSpeed;
  L7_BOOL           fullDuplex;

}dot3adOperPort_t;

typedef struct dot3ad_agg_s
{
  struct dot3ad_agg_s     *next;
  struct dot3ad_agg_s     *prev;

  L7_enetMacAddr_t    aggMacAddr;       /* MAC address associated with this logical interface */
  L7_uint32           aggId;            /* intIfNum of this logical interface */
  L7_BOOL             individualAgg;    /* L7_TRUE: default, aggregator represents an aggregate */
  L7_uint32           actorAdminAggKey; /* intIfNum of this logical interface */
  L7_uint32           actorOperAggKey;  /* intIfNum of this logical interface */

  L7_enetMacAddr_t    partnerSys;
  L7_uint32           partnerSysPri;
  L7_uint32           partnerOperAggKey;

  L7_uint32           collectorMaxDelay;
  L7_uint32           rxState;          /* enabled - disabled */
  L7_uint32           txState;

  /*ports actively participating in aggregation*/
  L7_uint32           aggActivePortList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32           activeNumMembers;
  /* ports attached to this aggregator */
  L7_uint32           aggPortList[L7_MAX_MEMBERS_PER_LAG]; 
  L7_uint32           currNumMembers;

  /* ports that have selected this aggregator */
  L7_uint32           aggSelectedPortList[L7_MAX_MEMBERS_PER_LAG]; 
  L7_uint32           currNumSelectedMembers;

  /* ports waiting to meet all selection requirements */
  L7_uint32           aggWaitSelectedPortList[L7_MAX_MEMBERS_PER_LAG]; 
  L7_uint32           currNumWaitSelectedMembers;
  dot3ad_usp_t        aggPortListUsp[L7_MAX_MEMBERS_PER_LAG];

  L7_BOOL             ready;

  L7_uchar8           name[L7_DOT3AD_MAX_NAME];
  L7_BOOL             inuse;  /* aggregator has been assigned */
  L7_uint32           adminMode; /*L7_ENABLE enabled, L7_DISABLE disabled*/
  L7_uint32           unused;  /*Was linktrapstate. Since that value was always with nim. removed to avoid confusion*/
  L7_uint32           stpMode;
  L7_BOOL             isStatic;  /* This LAG is static, i.e. no protocol partner
                                  * Using default values to aggregate
								  */
  L7_int32            hashMode;

} dot3ad_agg_t;


typedef struct dot3ad_system_s
{
  L7_enetMacAddr_t  actorSys;
  L7_uint32         actorSysPriority;
  L7_uint32         traceHandle;
  L7_timespec		timeOfLastTableChange;
  L7_uint32			unused;/*system wide static capability is unused */
  L7_uint32			hashMode;/*system wide hash mode */

} dot3ad_system_t;

typedef struct dot3ad_stats_s
{
  L7_uint32 LACPDUsRx;
  L7_uint32 MarkerPDUsRx;
  L7_uint32 MarkerResponsePDUsRx;
  L7_uint32 UnknownRx;
  L7_uint32 IllegalRx;
  L7_uint32 LACPDUsTx;
  L7_uint32 MarkerPDUsTx;
  L7_uint32 MarkerResponsePDUsTx;
  L7_uint32 RxLACPDUsDropped; /* statistic added to keep track of number of received 
                                 LACPDUs dropped for static lags */
} dot3ad_stats_t;

typedef struct dot3ad_LagCnt_s
{
  L7_uint32 StaticLagCnt;  /* number of static lags in the system */
  L7_uint32 DynamicLagCnt; /* number of dynamic lags in the system */
}dot3ad_LagCnt_t;

typedef struct  dot3adMutantCtr_s
{
  L7_uint32      mutling;             /* ID of member counter */
  L7_uint32      mutant;              /* ID of LAG composite counter */
} dot3adMutantCtr_t;



typedef enum dot3ad_nsf_opcode_e
{
  DOT3AD_NSF_OPCODE_NONE=0,
  DOT3AD_NSF_OPCODE_ADD,
  DOT3AD_NSF_OPCODE_MODIFY,
  DOT3AD_NSF_OPCODE_DELETE

}DOT3AD_NSF_OPCODE_t;

typedef struct dot3ad_nsfFuncPtr_s
{
  L7_RC_t (*dot3adCallCheckpointService)(L7_uint32 intIfNum, DOT3AD_NSF_OPCODE_t opcode);

} dot3ad_nsfFuncPtr_t;
/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DOT3AD INTERNAL USE ONLY
***********************************************************************
*/
/*********************************************************************
* @purpose  Find an aggregator record based on an aggregator key
*
* @param    agg_key         aggregator key
* @param    *p              pointer to record
*
* @returns  L7_SUCCESS      if retrieved
* @returns  L7_FAILURE      if key not found
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_agg_t *dot3adAggKeyFind(L7_uint32 agg_key);

/*********************************************************************
* @purpose  Find an aggregator record based on an internal interface
*
* @param    agg_intf        internal interface
*
* @returns  L7_NULL         if interface not found
* @returns  dot3ad_agg_t pointer to record if found
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_agg_t *dot3adAggIntfFind(L7_uint32 agg_intf);

/*********************************************************************
* @purpose  Find an aggregator record based on an aggregator port
*
* @param    agg_key         aggregator port
*
* @returns  L7_NULL         if port not a member of an aggregator
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_agg_t *dot3adAggPortFind(L7_uint32 agg_port);

/*********************************************************************
* @purpose  Find an port record based on a port key
*
* @param    port_key        port key
* @param    *p              pointer to record
*
* @returns  L7_SUCCESS      if retrieved
* @returns  L7_FAILURE      if key not found or not a member
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adPortKeyFind(L7_uint32 port_key);

/*********************************************************************
* @purpose  Find a port record based on a port interface number
*
* @param    intf            aggregator key
* @param    *p              pointer to record
*
* @returns  L7_SUCCESS      if retrieved
* @returns  L7_FAILURE      if intf not a member of an aggregator
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adPortIntfFind(L7_uint32 intf);

/*********************************************************************
* @purpose  Get the first port attached to a particular aggregator
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggAttachedGetFirst(L7_uint32 agg_intf);

/*********************************************************************
* @purpose  Given a first port attached to a particular aggregator
*           find the next
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggAttachedGetNext(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Add an interface to the attached list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggAttachedAdd(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Delete an interface from the attached list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggAttachedDelete(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Get the first port selected to a particular aggregator
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggSelectedGetFirst(L7_uint32 agg_intf);

/*********************************************************************
* @purpose  Given a first port selected by a particular aggregator
*           find the next
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggSelectedGetNext(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Add an interface to the selected list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggSelectedAdd(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Delete an interface from the selected list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggSelectedDelete(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Get the first port waiting to be selected
*
* @param    agg_intf        aggregator interface number
*
* @returns  dot3ad_port_t*  if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggWaitSelectedGetFirst(L7_uint32 agg_intf);

/*********************************************************************
* @purpose  Given a port in the wait list, find the next
*
* @param    agg_intf        aggregator interface number
* @param    intf            starting intf
*
* @returns  dot3ad_port_t*  next port record, if retrieved
* @returns  L7_NULLPTR      if the aggregator has no attached ports
*
* @notes    none
*       
* @end
*********************************************************************/
dot3ad_port_t *dot3adAggWaitSelectedGetNext(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Add an interface to the wait list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to add
*
* @returns  L7_SUCCESS      if successfully added or if already present
* @returns  L7_FAILURE      addition failed, possibly a full list
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggWaitSelectedAdd(L7_uint32 agg_intf, L7_uint32 intf);

/*********************************************************************
* @purpose  Delete an interface from the wait list for an aggregator
*
* @param    agg_intf        aggregator interface number
*           intf            interface to delete
*
* @returns  L7_SUCCESS      if successfully removed
* @returns  L7_FAILURE      deletion failed, doesnt exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot3adAggWaitSelectedDelete(L7_uint32 agg_intf, L7_uint32 intf);

dot3ad_port_t *dot3adAggActivePortGetFirst(L7_uint32 agg_intf);
dot3ad_port_t *dot3adAggActivePortGetNext(L7_uint32 agg_intf, L7_uint32 intf);
L7_RC_t dot3adAggActivePortAdd(L7_uint32 agg_intf, L7_uint32 intf);
L7_RC_t dot3adAggActivePortDelete(L7_uint32 agg_intf, L7_uint32 intf);


#endif /*INCLUDE_DOT3AD_DB_H */

