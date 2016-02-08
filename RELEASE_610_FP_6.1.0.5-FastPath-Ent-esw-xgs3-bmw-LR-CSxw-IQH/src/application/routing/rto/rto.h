#ifndef RTO_H
#define RTO_H
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rto.h
*
* @purpose Internal definitions for the Route Table Object component.
*
* @component 
*
* @create 03/22/01
*
* @author alt
* @end
*
*
*********************************************************************/
/*************************************************************

The unicast routing table is maintained as a tree of lists. Each node in the tree
is the head of a list of routes to a specific destination. The list of 
routes is ordered by route preference, with the lowest preference at the
head of the list. Thus, the most preferred route is at the head of the list.

A route with a preference of 255 is considered unusable. It will be stored 
in the routing table, but will not be considered a best route, even if it 
is the only route to a destination.

Currently, all routes of the same type from a given dynamic routing protocol 
have the same preference. (This could change if we add routing policy to 
set the preference of specific routes.) OSPF has a different default preference
for intra-area, inter-area, external type 1 and external type 2 routes. BGP 
has a different default preference for iBGP and eBGP routes. If OSPF learns 
different types of routes to the same destination, it will give both to RTO and
have RTO select the preferred route. BGP on the other hand will not give RTO 
an iBGP and an eBGP route to the same destination. 

The user can specify the preference of individual static routes and can 
configure multiple default routes that have different preferences. Static 
and default routes with different preferences must have different next hops.
RTO will not accept two static (or default) routes with the same set of 
next hops and different preferences. 

A "route" includes a set of next hops to a given destination. Since preference
is an attribute of the route, not of a next hop, it follows that the same 
preference applies to all next hops
in the route. The user configures static and default routes one next hop at
a time. Next hops with the same preference are considered part of the same route.
Next hops with different preferences are considered members of different routes.

Whenever the operator configures an IPv4 address, a local route must be 
added to RTO. If RTO is full, the local route cannot be added, potentially 
leaving the interface in an inconsistent state. Our goal is to only accept 
the addition of an address if a local route can be added. Even if a local 
route is not added immediately, either because routing is not enabled or 
the link is down, the system should guarantee that when the routing 
interface comes up, RTO has space for the local route. If a link goes down, 
RTO should guarantee that if the link comes back up there will be space for 
the local routes associated with the link. To satisfy this goal, RTO allows 
IP MAP to reserve space for a local route. IP MAP makes a reservation 
whenever a primary or secondary IPv4 address is configured. The reservation 
is released when an address is removed. RTO maintains a count of the number 
of IPv4 addresses configured and a count of local routes. RTO reserves space 
for the difference.

                    
*************************************************************/



#include "l7_common.h"
#include "radix_api.h"

/* Maximum number of applications that can register with RTO.
*/

/* Only IP MAP now registers for ARP GW callbacks. */
#define RTO_MAX_CALLBACKS 5        /* ARP GW table changes */
#define RTO_MAX_BR_CLIENTS 8       /* best route clients */

/* Max length of the name of a best route client. Used for debugging. */
#define RTO_CLIENT_NAME_LEN 16

/* maximum length of an RTO trace message */
#define RTO_TRACE_LEN_MAX 255   

/* RTO trace flags */
typedef enum {

    /* Trace additions to the routing table */
    RTO_TRACE_ROUTE_ADD = 0x1,

    /* Trace deletions from routing table */
    RTO_TRACE_ROUTE_DEL = 0x2,

    /* Trace changes to the routing table */
    RTO_TRACE_ROUTE_CHANGE = 0x4,

    /* Trace best route changes */
    RTO_TRACE_BEST = 0x8,

    /* Trace interface events reported to RTO */
    RTO_TRACE_IF = 0x10,

    /* Trace changes to protocol preferences */
    RTO_TRACE_PREF = 0x20,

    /* Trace best route notifications */
    RTO_TRACE_NOTIF = 0x40,

    /* Trace RTO change list actions */
    RTO_TRACE_CHANGE_LIST = 0x80,

    /* Trace memory allocation at init time */
    RTO_TRACE_ALLOC = 0x100,

    /* next hop resolve registration */
    RTO_TRACE_NHRES_ADD = 0x200,

    RTO_TRACE_NHRES_DEL = 0x400

} rtoTraceFlag_t;


/* Mapping from L7_RTO_PROTOCOL_INDICES_t to L7_REDIST_RT_INDICES_t */
/* Defined in rto.c. */
extern L7_REDIST_RT_INDICES_t protocolMapping[RTO_LAST_PROTOCOL];

extern L7_uint32 rtoEcmpRouteCount;
extern L7_uint32 rtoEcmpRouteMax;

/* Storage structure for a callback.
*/
typedef struct rto_callback
{
  L7_VOIDFUNCPTR_t callback;
  void * passthrough;
  L7_uint32 notification_type;
} RtoCallbackElement;

/* Various statistics for the Route Table Object Radix tree.
*/
typedef struct rto_stats_s
{
  L7_uint32 tot_adds;      /* Total number of adds                          */
  L7_uint32 tot_dels;      /* Total number of deletes                       */
  L7_uint32 bad_dels;      /* attempts to delete non-existant entries       */
  L7_uint32 dup_adds;      /* additions of existent entries                 */

  /* add attempted when RTO full, or destination invalid, or no next hops
   * are on a local subnet. */
  L7_uint32 bad_adds;    

  /* Modify fails because original route not found */
  L7_uint32 bad_mods;

  /* Number of route lookups with invalid destination. */
  L7_uint32 bad_lookups;

  /* current number of best and non-best routes in RTO. Does not include
   * destinations that are still in the radix tree, but are marked for deletion 
   * pending notification of all best route clients of the deletion. 
   * Equals the number of routeInfo objects in use. */
  L7_uint32 route_entries;   

  /* The number of destinations in the radix tree. Includes destinations
   * marked for deletion. Also includes destinations whose only routes 
   * have a pref of 255 and therefore are not best routes. Equals the number
   * of routeData objects in use. */
  L7_uint32 radix_entries; 

  /* The number of best routes in the routing table. Should equal the number of
   * routes given to the forwarding table. Used to determine when RTO is full. */
  L7_uint32 best_entries;
  
  /* Number of primary and secondary IPv4 addresses configured on the box. */
  L7_uint32 reserved_locals;  

  /* Number of local routes in RTO. */
  L7_uint32 active_locals;

  L7_uint32 nhres_radix_entries;

} rto_stats_t;

/* Next hop structure */
typedef struct rtoNextHop_s
{
  L7_uint32 intIfNum;  /* outgoing interface */
  L7_uint32 ifUp;      /* Set to L7_TRUE when IP interface for this route is up. */
  L7_uint32 nextHopIP; /* next hop ip address */

} rtoNextHop_t;

/* This structure describes a route to a given destination. */
typedef struct rtoRouteInfo_s 
{
  L7_RTO_PROTOCOL_INDICES_t protocol;   /* one of L7_RTO_PROTOCOL_INDICES_t */

  /* a route with a preference1 of 255 cannot be selected as a best route. */
  L7_uint32 preference1;     /* Primary Preference */

  /* preference2 is not configurable. It is automatically set to a low value 
   * for static and default routes and to a high value for other routes so 
   * that configured routes are preferred to dynamic routes if they have 
   * the same preference1. */
  L7_uint32 preference2;     /* Secondary Preference */

  /* flag to indicate whether this route is the best route to its destination. 
   * Normally the first route in the list is the best route (since the routes
   * to a destination are sorted by preference. If the only route to a destination
   * has a preference of 255, then the first route in the list has this flag 
   * cleared and there is no best route. */
#define RTO_BEST_ROUTE_NODE     0x01  
#define RTO_ECMP_TRUNC          0x02
#define RTO_REJECT_ROUTE        0x04
  L7_uint32 flags;           /* Used to describe this route node */
  L7_uint32 metric;	         /* route metric */
  L7_uint32 updateTime;     /*Specifies the last time the route was updated (in hours:minutes:seconds)*/ 
                              
  /* The number of valid next hops in this route. A next hop is considered
   * valid if the next hop IP address is on a connected subnet. A valid
   * next hop is considered "inactive" if the interface to that subnet is
   * down. */
  L7_uint32 numNextHops; 

  /* Array of next hops to the destination. There are no holes in the 
   * array. That is, there cannot be a nonzero array element beyond
   * an empty array element. */
  rtoNextHop_t nextHops[L7_RT_MAX_EQUAL_COST_ROUTES];  

  /*... Additional information about the route.... */
  struct rtoRouteInfo_s * next;

} rtoRouteInfo_t;

/* the RTO key structure. Use rtoSetKey to set 
 * the key properly. The first byte of the key 
 * gets set to the key length. The other three
 * bytes are not used, and are zeroed out. They 
 * ensure proper byte alignment is maintained.
 */
typedef struct rtoTreeKey_s
{
  L7_uchar8 reserved[4];
  L7_uint32 addr;
} rtoTreeKey_t;

typedef struct rtoRouteData_s
{
  struct l7_radix_node nodes[2]; /* Must be the first element in the structure */
  /* ipaddr (key) and mask must follow */
  rtoTreeKey_t      network;  /* Only the significant portion of the network is stored
                               * in this field.
                               */
  rtoTreeKey_t      netmask;  /* Destination network mask. */      

  /* List of routes to this destination. Each route is from a different source
   * (e.g., OSPF, RIP), or in the case of static routes has a unique preference.
   * May be NULL if all routes to this destination have been deleted, but some
   * best route clients have not yet been notified of the delete. */
  rtoRouteInfo_t *nextRouteInfo; 

  /* next and previous entries on change list */
  struct rtoRouteData_s *prevChange;
  struct rtoRouteData_s *nextChange;

  /* Change sequence number. Increases from head to tail of change list. */
  L7_uint32 changeSeqNo;

  /* May not be RTO_NOOP. */
  e_RTO_RT_CHANGE_TYPE changeType;

  /* client-specific change type for each element on the change list.  */
  L7_uchar8 clientChType[RTO_MAX_BR_CLIENTS];

  void *next;   /* Need this for radix */

} rtoRouteData_t;

/* Defines a best route client */
typedef struct RtoBestRouteClient
{
  /* for debugging */
  L7_uchar8 clientName[RTO_CLIENT_NAME_LEN + 1];

  /* Index to the BestRouteClientList array */
  L7_uint32 clientIndex;

  /* callback used to notify client that "something has changed." */
  L7_VOIDFUNCPTR_t callback;

  /* L7_TRUE if client has not yet retrieved all outstanding changes. */
  L7_BOOL changePending;

  /* Client's place in the change list. NULL if no changes are pending
     for this client. */
  rtoRouteData_t *nextChange;

} RtoBestRouteClient;

typedef struct rtoCallbackData_s
{
  L7_uint32     routerIP;  
  L7_uint32     ifNum;
  L7_RTO_PROTOCOL_INDICES_t     protocol;      /* one of L7_RTO_PROTOCOL_INDICES_t */
  L7_uint32 	preference1;   /* Primary Preference */
  L7_uint32 	preference2;   /* Secondary Preference */
  L7_uint32 	metric;		/*route metric */
  L7_uint32 	tag;		/*FUTURE_FUNC route tag */

} rtoCallbackData_t;

/* info about a protocol that requires next hop resolution notify about a dest */
typedef struct rtoProtoInfo_s
{
  void   (*funcPtr)(L7_uint32 ipAddr,void *passthru);
  void   *passthru;
  struct rtoProtoInfo_s *next;
} rtoProtoInfo_t;

/* for notification of next hop changes to specific destinations */
/* mask always all 1's */
typedef struct rtoNhopData_s
{
  struct l7_radix_node nodes[2]; /* Must be the first element in the structure */
  /* ipaddr (key) and mask must follow */
  rtoTreeKey_t      network;  /* Only the significant portion of the network is stored
                               * in this field.
                               */
  rtoTreeKey_t      netmask;  /* Destination network mask. */      

  /* registered clients for notification */
  rtoProtoInfo_t *nextProtoInfo; 

  L7_int32        resolvingPrefixLen;       /* route used to this dest (MB signed) */

  void *next;   /* Need this for radix */

} rtoNhopData_t;

/* Utility Functions */

/* rto.c */
void rtoSelectNewBestRoute(rtoRouteData_t *pData, rtoRouteInfo_t *oldBestRouteInfo, 
                           L7_BOOL forceNotify);
rtoRouteInfo_t* rtoRouteInfoListSort(rtoRouteInfo_t *head);
void rtoNextHopsSort(rtoNextHop_t A[], L7_uint32 N);
void rtoNextHopsLeftShift(L7_uint32 index, rtoRouteInfo_t *routeInfo);
L7_BOOL rtoIsDuplicateNextHop(rtoNextHop_t A[], L7_uint32 N, L7_uint32 nextHopIP);
L7_int32 rtoCompareRoutes (rtoRouteInfo_t * firstRouteInfo, 
                               rtoRouteInfo_t * secondRouteInfo);
L7_int32 rtoComparePreferences(rtoRouteInfo_t *firstRouteInfo,
                               rtoRouteInfo_t *secondRouteInfo);
L7_int32 rtoCompareNextHops (rtoNextHop_t * firstNextHop, 
                             rtoNextHop_t * secondNextHop);
void rtoCleanupAfterDelete (rtoRouteData_t *pData, rtoRouteInfo_t *routeInfo);
L7_RC_t rtoNextBestRouteGet (L7_routeEntry_t *nextRoute);
L7_BOOL rtoIsFull(void);
void rtoTraceWrite(L7_uchar8 *traceMsg);
void rtoSetKey(rtoTreeKey_t * p_key, L7_uint32 addr);
L7_RC_t rtoNHResNotify (rtoRouteData_t *routeEntry, L7_uint32 command );


/* rto_porting.c */
rtoRouteInfo_t *rtoSelectPreferredRoute(rtoRouteInfo_t  *route1, 
                                        rtoRouteInfo_t  *route2);

void rtoDebugInit(void);

/* rto_ch_list.c */
L7_RC_t rtoClientListAllocate(void);
L7_RC_t rtoClientListDeallocate(void);
L7_RC_t rtoChangeListAdd(rtoRouteData_t *pData);
L7_RC_t rtoChangeListMod(rtoRouteData_t *pData);
L7_RC_t rtoChangeListDel(rtoRouteData_t *pData);

#endif /* RTO_H */
