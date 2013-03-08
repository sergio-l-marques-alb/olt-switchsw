/******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
*******************************************************************************
*
* @filename broad_l3_int.h
*
* @purpose   Contains internal declarations for Broadcom HAPI L3 module
*
* @component HAPI
*
* @comments  This file is internal to L3 module in HAPI and must not be
*            included else where.
*
* @create 10/01/2007
*
* @author sdoke
*
* @end
*
******************************************************************************/
#ifndef BROAD_L3_INT_H_INCLUDED
#define BROAD_L3_INT_H_INCLUDED

#include "broad_common.h"
#include "broad_l3.h"
#include "broad_l3_debug.h"
#include "avl_api.h"
#include "osapi_support.h"
#include "comm_mask.h"

#include "bcmx/l3.h"
#include "l7_usl_bcmx_l3.h"

/* AVL Tree heap and data sizes - derived from size of tables */
#define HAPI_BROAD_L3_NEXT_HOP_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                               HAPI_BROAD_L3_NH_TBL_SIZE)
#define HAPI_BROAD_L3_NEXT_HOP_DATA_HEAP_SIZE (sizeof (BROAD_L3_NH_ENTRY_t) * \
                                               HAPI_BROAD_L3_NH_TBL_SIZE)

#define HAPI_BROAD_L3_ECMP_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                           HAPI_BROAD_L3_MAX_ECMP_GROUPS)
#define HAPI_BROAD_L3_ECMP_DATA_HEAP_SIZE (sizeof (BROAD_L3_ECMP_ENTRY_t) * \
                                           HAPI_BROAD_L3_MAX_ECMP_GROUPS)

#define HAPI_BROAD_L3_HOST_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                           HAPI_BROAD_L3_HOST_TBL_SIZE)
#define HAPI_BROAD_L3_HOST_DATA_HEAP_SIZE (sizeof (BROAD_L3_HOST_ENTRY_t) * \
                                           HAPI_BROAD_L3_HOST_TBL_SIZE)

#define HAPI_BROAD_L3_MAC_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                          HAPI_BROAD_L3_MAC_TBL_SIZE)
#define HAPI_BROAD_L3_MAC_DATA_HEAP_SIZE (sizeof (BROAD_L3_MAC_ENTRY_t) * \
                                          HAPI_BROAD_L3_MAC_TBL_SIZE)

#define HAPI_BROAD_L3_ROUTE_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                            HAPI_BROAD_L3_ROUTE_TBL_SIZE)
#define HAPI_BROAD_L3_ROUTE_DATA_HEAP_SIZE (sizeof (BROAD_L3_ROUTE_ENTRY_t) * \
                                            HAPI_BROAD_L3_ROUTE_TBL_SIZE)

#define HAPI_BROAD_L3_TUNNEL_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                             HAPI_BROAD_L3_MAX_TUNNELS)
#define HAPI_BROAD_L3_TUNNEL_DATA_HEAP_SIZE (sizeof (BROAD_L3_TUNNEL_ENTRY_t) * \
                                            HAPI_BROAD_L3_MAX_TUNNELS)

#define HAPI_BROAD_L3_INVALID_EGR_ID       (-1)


/* We use reserved 127.0.0.1 address to represent
** cpu next-hop egress object.
*/
#define HAPI_BROAD_L3_CPU_EGR_NHOP_IP      (0x7f000001)

#define HAPI_BROAD_L3_LOG_ERROR(e)                                            \
  {                                                                           \
    sysapiPrintf("Log error: %s, %d, code %d\n", __FILE__, __LINE__, e);      \
    hapiBroadL3StatsShow();                                                   \
    hapiBroadL3DebugStackTrace();                                             \
    LOG_ERROR((e));                                                           \
  }

/* Macros for improving readability */
#define BROAD_L3_ROUTE_FAMILY(r) ((r)->key.family)

#define BROAD_L3_ROUTE_SET_FAMILY(r,f) (((r)->key.family) = (f))

#define BROAD_L3_ROUTE_ADDR_REF(r) (&((r)->key.ipAddr)

#define BROAD_L3_ROUTE_NHOP_ADDR_REF(r, i) (&((r)->nextHopTable[(i)].addrUsp.addr))

#define BROAD_L3_IS_ROUTE_ECMP(p)  (((p)->numNextHops > 1) ? L7_TRUE : L7_FALSE)
#define BROAD_L3_IS_ROUTE_NON_ECMP(p)  (((p)->numNextHops == 1) ? L7_TRUE : L7_FALSE)


/* L3 cache flags */
#define BROAD_L3_ENTRY_CACHE    0
#define BROAD_L3_ENTRY_COMMIT   1

/* Retry failures after 60 secs (not too long and not too short). If this
 * interval is long, we may be dropping traffic for a long time. If this
 * interval is short, we may be retrying too many times without success
 */
#define  HAPI_BROAD_L3_RETRY_INTERVAL   (60 * 1000) /* 60 secs */


/* Macros for OSAPI operations */
#define HAPI_BROAD_L3_SEMA_CREATE(sema, opt, state)                 \
  {                                                                 \
    HAPI_BROAD_L3_DEBUG(broadL3SemaDebug,                           \
                        "sema create %s (%s, %s)\n",                \
                        #sema, #opt, #state);                       \
    (sema) = osapiSemaBCreate((opt), (state));                      \
    if ((sema) == NULL) HAPI_BROAD_L3_LOG_ERROR(0);                 \
  }

#define HAPI_BROAD_L3_SEMA_DELETE(sema)                             \
  {                                                                 \
    HAPI_BROAD_L3_DEBUG(broadL3SemaDebug,                           \
                        "sema delete %s\n",                         \
                        #sema);                                     \
    if (osapiSemaDelete((sema)) != L7_SUCCESS)                      \
      HAPI_BROAD_L3_LOG_ERROR(0);                                   \
  }

#define HAPI_BROAD_L3_SEMA_TAKE(sema, opt)                          \
  {                                                                 \
    L7_RC_t rc;                                                     \
    HAPI_BROAD_L3_DEBUG(broadL3SemaDebug, "sema take %s\n",         \
                        #sema);                                     \
    rc = osapiSemaTake((sema), (opt));                              \
    if (rc != L7_SUCCESS) HAPI_BROAD_L3_LOG_ERROR(rc);              \
    HAPI_BROAD_L3_DEBUG(broadL3SemaDebug, "sema have %s\n",         \
                        #sema);                                     \
  }

#define HAPI_BROAD_L3_SEMA_GIVE(sema)                               \
  {                                                                 \
    L7_RC_t rc;                                                     \
    HAPI_BROAD_L3_DEBUG(broadL3SemaDebug, "sema give %s\n",         \
                        #sema);                                     \
    rc = osapiSemaGive((sema));                                     \
    if (rc != L7_SUCCESS) HAPI_BROAD_L3_LOG_ERROR(rc);              \
  }

#define NTOP(f,a,s,l)   ((L7_uchar8 *)osapiInetNtop((L7_uint32)(f),     \
                                                    (L7_uchar8 *)(a),   \
                                                    (L7_uchar8 *)(s),   \
                                                    (L7_uint32)(l)))

/* Macros for Work lists */
#define HAPI_BROAD_L3_NH_WLIST_ADD(p)                                          \
  {                                                                            \
    if ((hapiBroadL3NhopListHead == L7_NULL) &&                                \
        (hapiBroadL3NhopListTail == L7_NULL))                                  \
    {                                                                          \
      hapiBroadL3NhopListHead = hapiBroadL3NhopListTail = p;                   \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      hapiBroadL3NhopListTail->wl.next = p;                                    \
      p->wl.prev = hapiBroadL3NhopListTail;                                    \
      hapiBroadL3NhopListTail = p;                                             \
    }                                                                          \
    broadL3NhopWlistCurrDepth++;                                               \
    if (broadL3NhopWlistCurrDepth > broadL3NhopWlistMaxDepth)                  \
    {                                                                          \
      broadL3NhopWlistMaxDepth = broadL3NhopWlistCurrDepth;                    \
    }                                                                          \
  }                                                                            \

#define HAPI_BROAD_L3_NH_WLIST_DEL(p)                                          \
  {                                                                            \
    if (p->wl.prev == L7_NULL)                                                 \
      hapiBroadL3NhopListHead = p->wl.next;                                    \
    else                                                                       \
      p->wl.prev->wl.next = p->wl.next;                                        \
                                                                               \
    if (p->wl.next == L7_NULL)                                                 \
      hapiBroadL3NhopListTail = p->wl.prev;                                    \
    else                                                                       \
      p->wl.next->wl.prev = p->wl.prev;                                        \
                                                                               \
    p->wl.cmd  = BROAD_L3_NH_CMD_NO_OP;                                        \
    p->wl.prev = L7_NULL;                                                      \
    p->wl.next = L7_NULL;                                                      \
    broadL3NhopWlistCurrDepth--;                                               \
  }                                                                            \


#define HAPI_BROAD_L3_ECMP_WLIST_ADD(p)                                        \
  {                                                                            \
    if ((hapiBroadL3EcmpListHead == L7_NULL) &&                                \
        (hapiBroadL3EcmpListTail == L7_NULL))                                  \
    {                                                                          \
      hapiBroadL3EcmpListHead = hapiBroadL3EcmpListTail = p;                   \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      hapiBroadL3EcmpListTail->wl.next = p;                                    \
      p->wl.prev = hapiBroadL3EcmpListTail;                                    \
      hapiBroadL3EcmpListTail = p;                                             \
    }                                                                          \
  }                                                                            \

#define HAPI_BROAD_L3_ECMP_WLIST_DEL(p)                                        \
  {                                                                            \
    if (p->wl.prev == L7_NULL)                                                 \
      hapiBroadL3EcmpListHead = p->wl.next;                                    \
    else                                                                       \
      p->wl.prev->wl.next = p->wl.next;                                        \
                                                                               \
    if (p->wl.next == L7_NULL)                                                 \
      hapiBroadL3EcmpListTail = p->wl.prev;                                    \
    else                                                                       \
      p->wl.next->wl.prev = p->wl.prev;                                        \
                                                                               \
    p->wl.cmd  = BROAD_L3_ECMP_CMD_NO_OP;                                      \
    p->wl.prev = L7_NULL;                                                      \
    p->wl.next = L7_NULL;                                                      \
  }                                                                            \


#define HAPI_BROAD_L3_HOST_WLIST_ADD(p)                                        \
  {                                                                            \
    if ((hapiBroadL3HostListHead == L7_NULL) &&                                \
        (hapiBroadL3HostListTail == L7_NULL))                                  \
    {                                                                          \
      hapiBroadL3HostListHead = hapiBroadL3HostListTail = p;                   \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      hapiBroadL3HostListTail->wl.next = p;                                    \
      p->wl.prev = hapiBroadL3HostListTail;                                    \
      hapiBroadL3HostListTail = p;                                             \
    }                                                                          \
    broadL3HostWlistCurrDepth++;                                               \
    if (broadL3HostWlistCurrDepth > broadL3HostWlistMaxDepth)                  \
    {                                                                          \
      broadL3HostWlistMaxDepth = broadL3HostWlistCurrDepth;                    \
    }                                                                          \
  }                                                                            \
 
#define HAPI_BROAD_L3_HOST_WLIST_DEL(p)                                        \
  {                                                                            \
    if (p->wl.prev == L7_NULL)                                                 \
      hapiBroadL3HostListHead = p->wl.next;                                    \
    else                                                                       \
      p->wl.prev->wl.next = p->wl.next;                                        \
                                                                               \
    if (p->wl.next == L7_NULL)                                                 \
      hapiBroadL3HostListTail = p->wl.prev;                                    \
    else                                                                       \
      p->wl.next->wl.prev = p->wl.prev;                                        \
                                                                               \
    p->wl.cmd  = BROAD_L3_HOST_CMD_NO_OP;                                      \
    p->wl.prev = L7_NULL;                                                      \
    p->wl.next = L7_NULL;                                                      \
    broadL3HostWlistCurrDepth--;                                               \
  }

#define HAPI_BROAD_L3_ROUTE_WLIST_ADD(p)                                       \
  {                                                                            \
    if ((hapiBroadL3RouteListHead == L7_NULL) &&                               \
        (hapiBroadL3RouteListTail == L7_NULL))                                 \
    {                                                                          \
      hapiBroadL3RouteListHead = hapiBroadL3RouteListTail = p;                 \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      hapiBroadL3RouteListTail->wl.next = p;                                   \
      p->wl.prev = hapiBroadL3RouteListTail;                                   \
      hapiBroadL3RouteListTail = p;                                            \
    }                                                                          \
    broadL3RouteWlistCurrDepth++;                                              \
    if (broadL3RouteWlistCurrDepth > broadL3RouteWlistMaxDepth)                \
    {                                                                          \
      broadL3RouteWlistMaxDepth = broadL3RouteWlistCurrDepth;                  \
    }                                                                          \
  }                                                                            \

#define HAPI_BROAD_L3_ROUTE_WLIST_DEL(p)                                       \
  {                                                                            \
    if (p->wl.prev == L7_NULL)                                                 \
      hapiBroadL3RouteListHead = p->wl.next;                                   \
    else                                                                       \
      p->wl.prev->wl.next = p->wl.next;                                        \
                                                                               \
    if (p->wl.next == L7_NULL)                                                 \
      hapiBroadL3RouteListTail = p->wl.prev;                                   \
    else                                                                       \
      p->wl.next->wl.prev = p->wl.prev;                                        \
                                                                               \
    p->wl.cmd  = BROAD_L3_ROUTE_CMD_NO_OP;                                     \
    p->wl.prev = L7_NULL;                                                      \
    p->wl.next = L7_NULL;                                                      \
   broadL3RouteWlistCurrDepth--;                                               \
  }                


#define HAPI_BROAD_L3_TUNNEL_WLIST_ADD(p)                                      \
  {                                                                            \
    if ((hapiBroadL3TunnelListHead == L7_NULL) &&                              \
        (hapiBroadL3TunnelListTail == L7_NULL))                                \
    {                                                                          \
      hapiBroadL3TunnelListHead = hapiBroadL3TunnelListTail = p;               \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      hapiBroadL3TunnelListTail->wl.next = p;                                  \
      p->wl.prev = hapiBroadL3TunnelListTail;                                  \
      hapiBroadL3TunnelListTail = p;                                           \
    }                                                                          \
  }                                                                            \

#define HAPI_BROAD_L3_TUNNEL_WLIST_DEL(p)                                       \
  {                                                                            \
    if (p->wl.prev == L7_NULL)                                                 \
      hapiBroadL3TunnelListHead = p->wl.next;                                   \
    else                                                                       \
      p->wl.prev->wl.next = p->wl.next;                                        \
                                                                               \
    if (p->wl.next == L7_NULL)                                                 \
      hapiBroadL3TunnelListTail = p->wl.prev;                                   \
    else                                                                       \
      p->wl.next->wl.prev = p->wl.prev;                                        \
                                                                               \
    p->wl.cmd  = BROAD_L3_TUNNEL_CMD_NO_OP;                                     \
    p->wl.prev = L7_NULL;                                                      \
    p->wl.next = L7_NULL;                                                      \
  }


#define HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK                                       \
  do                                                                           \
  {                                                                            \
    L7_uint32 _val32_=1;                                                       \
    osapiMessageSend(hapiBroadL3WakeUpQueue,                                   \
                     (void*)&_val32_,                                          \
                     sizeof (_val32_),                                         \
                      L7_NO_WAIT,                                              \
                      L7_MSG_PRIORITY_NORM);                                   \
  } while(0)                                                                   \

#define HAPI_BROAD_L3_ASYNC_WAIT                                               \
  do                                                                           \
  {                                                                            \
    L7_uint32 _val32_;                                                         \
    if (osapiMessageReceive(hapiBroadL3WakeUpQueue,                            \
                            (void *)&_val32_,                                  \
                            sizeof(_val32_),                                   \
                             L7_WAIT_FOREVER) != L7_SUCCESS)                   \
    {                                                                          \
      HAPI_BROAD_L3_LOG_ERROR(0);                                              \
    }                                                                          \
  } while(0)                                                                   \


/* IPv4/IPv6 Address structures */

typedef union BROAD_L3_ADDR_u
{
  bcm_ip_t      ipv4;  /* 32-bit IPv4 address */
  bcm_ip6_t     ipv6;  /* 128-bit IPv6 address */
} BROAD_L3_ADDR_t;

typedef struct BROAD_L3_ADRR_USP_s {
  BROAD_L3_ADDR_t addr;  /* IPv6 or IPv4 address */
  DAPI_USP_t usp;        /* USP of routing interface on which addr exists */
} BROAD_L3_ADDR_USP_t;


/* Data structure tags */
struct BROAD_L3_NH_ENTRY_s;
typedef struct BROAD_L3_NH_ENTRY_s BROAD_L3_NH_ENTRY_t;

struct BROAD_L3_MAC_ENTRY_s;
typedef struct BROAD_L3_MAC_ENTRY_s BROAD_L3_MAC_ENTRY_t;

struct BROAD_L3_ECMP_ENTRY_s;
typedef struct BROAD_L3_ECMP_ENTRY_s BROAD_L3_ECMP_ENTRY_t;

struct BROAD_L3_HOST_ENTRY_s;
typedef struct BROAD_L3_HOST_ENTRY_s BROAD_L3_HOST_ENTRY_t;

struct BROAD_L3_ROUTE_ENTRY_s;
typedef struct BROAD_L3_ROUTE_ENTRY_s BROAD_L3_ROUTE_ENTRY_t;

struct BROAD_L3_TUNNEL_ENTRY_s;
typedef struct BROAD_L3_TUNNEL_ENTRY_s BROAD_L3_TUNNEL_ENTRY_t;


/* NEXT HOP data structures - unipath(non-ECMP) & multipath(ECMP) */

/* Uni-Path next hops (non ECMP) */
typedef enum {
  BROAD_L3_NH_CMD_NO_OP = 0,
  BROAD_L3_NH_CMD_ADD,
  BROAD_L3_NH_CMD_MODIFY,
  BROAD_L3_NH_CMD_DELETE,
} BROAD_L3_NH_CMD_t;

typedef struct BROAD_L3_NH_WORK_LIST_s
{
  BROAD_L3_NH_CMD_t       cmd;          /* CMD for Async task */
  BROAD_L3_NH_ENTRY_t    *prev;         /* Link to prev NH entry in the WL */
  BROAD_L3_NH_ENTRY_t    *next;         /* Link to next NH entry in the WL */
} BROAD_L3_NH_WORK_LIST_t;


typedef struct BROAD_L3_NH_KEY_s
{
  L7_uint8    family;
  BROAD_L3_ADDR_USP_t addrUsp;
} BROAD_L3_NH_KEY_t;

/* Next hop flags */
#define BROAD_L3_NH_RESOLVED    0x01     /* Next hop is resolved */
#define BROAD_L3_NH_ZERO_COUNT  0x02     /* Decrement/Delete pending */
#define BROAD_L3_NH_LOCAL       0x04     /* Next hop is local */
#define BROAD_L3_NH_TUNNEL      0x08     /* Next hop goes over tunnel */

struct BROAD_L3_NH_ENTRY_s
{
  BROAD_L3_NH_KEY_t key;      /* Next hop key */

  L7_uint8          flags;    /* Next hop flags */

  bcm_if_t          l3_intf_id;        /* L3 intf ID of routing/tunnel intf */
  bcm_if_t          egressId;          /* Egress object ID of the next hop */

  L7_uint32         ref_count;         /* Num of hosts/routes linked to this NH */ 

  /* BCM Error code when creating next hop. BCM_E_NONE if success */
  L7_int32    rv;

  BROAD_L3_MAC_ENTRY_t       *pMac;   /* Link to Nhop's L2 information */
  BROAD_L3_NH_ENTRY_t  *pMacNhopNext; /* List of Nhops referencing a given MAC */

  BROAD_L3_NH_WORK_LIST_t     wl;    /* List of nexthops to be worked on */

  void   *reserved;                  /* This field is needed by AVL Library */
};


/* ECMP or Multipath next hops */
typedef struct BROAD_L3_ECMP_KEY_s
{
  L7_uint8    family;
  BROAD_L3_ADDR_USP_t addrUsp[L7_RT_MAX_EQUAL_COST_ROUTES];
} BROAD_L3_ECMP_KEY_t;

typedef enum {
  BROAD_L3_ECMP_CMD_NO_OP = 0,
  BROAD_L3_ECMP_CMD_ADD,
  BROAD_L3_ECMP_CMD_MODIFY,
  BROAD_L3_ECMP_CMD_DELETE,
} BROAD_L3_ECMP_CMD_t;

typedef struct BROAD_L3_ECMP_WORK_LIST_s
{
  BROAD_L3_ECMP_CMD_t       cmd;          /* CMD for Async task */
  BROAD_L3_ECMP_ENTRY_t    *prev;         /* Link to prev ECMP entry in the WL */
  BROAD_L3_ECMP_ENTRY_t    *next;         /* Link to next ECMP entry in the WL */
} BROAD_L3_ECMP_WORK_LIST_t;

typedef enum {
  BROAD_L3_ECMP_UNRESOLVED = 0, 
  BROAD_L3_ECMP_TO_CPU,   /* Resolved to CPU */
  BROAD_L3_ECMP_RESOLVED  /* Resolved to individual nexthops */
} BROAD_L3_ECMP_STATE_t;

/* ECMP flags */
#define BROAD_L3_ECMP_RESOLVED    0x01     /* ECMP is resolved */
#define BROAD_L3_ECMP_TO_CPU      0x02     /* ECMP is pointing to CPU */
#define BROAD_L3_ECMP_ZERO_COUNT  0x04     /* Decrement/Delete pending */

struct BROAD_L3_ECMP_ENTRY_s
{
  BROAD_L3_ECMP_KEY_t  key;       /* ECMP key */
 
  L7_uint8             flags;     /* ECMP flags */
  bcm_if_t             egressId;  /* Multipath egress object id */
  L7_uint32            ref_count; /* Num of routes linked to this ECMP */
 
  /* BCM Error code when creating ECMP next hop. BCM_E_NONE if success */
  L7_int32    rv;

  BROAD_L3_NH_ENTRY_t  *pNhop[L7_RT_MAX_EQUAL_COST_ROUTES]; /* Individual nhops */

  BROAD_L3_ECMP_WORK_LIST_t wl;

  void *reserved; /* AVL TREE requires this as last */
};


/* MAC data structures */
typedef struct BROAD_L3_MAC_KEY_s
{
  bcm_mac_t   macAddr;   /* MAC address of the host */
  L7_ushort16 vlanId;    /* VLAN ID of the host */
} BROAD_L3_MAC_KEY_t;


struct BROAD_L3_MAC_ENTRY_s
{
  BROAD_L3_MAC_KEY_t  key;  /* Key for MAC AVL tree */

  DAPI_USP_t    usp;        /* Actual physical port on which MAC is learnt */
  L7_BOOL       resolved;   /* Is Mac resolved or not */
  L7_BOOL       vlan_routing_intf; /* Is VLAN routing intf */

  L7_BOOL       target_is_trunk;   /* For trunks, maintain the trunk id */
  L7_uint32     trunkId;

  BROAD_L3_NH_ENTRY_t *nHopList;   /* Nhop(s) referencing this MAC entry */

  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry; /* Tunnel associated with MAC entry */

  /* List of hosts on this tunnel. Wireless/4o4 tunnels only */
  BROAD_L3_HOST_ENTRY_t *hostList;

  void   *reserved;                 /* This field is needed by AVL Library */
};


/* HOST data structures */
typedef enum {
  BROAD_L3_HOST_CMD_NO_OP = 0,
  BROAD_L3_HOST_CMD_ADD,
  BROAD_L3_HOST_CMD_MODIFY,
  BROAD_L3_HOST_CMD_DELETE,
  BROAD_L3_HOST_CMD_ASYNC_FAIL_HANDLE,
} BROAD_L3_HOST_CMD_t;

typedef struct BROAD_L3_HOST_WORK_LIST_s
{
  BROAD_L3_HOST_CMD_t       cmd;         /* CMD for Async task */
  BROAD_L3_HOST_ENTRY_t    *prev;        /* Link to prev Host entry in the WL */
  BROAD_L3_HOST_ENTRY_t    *next;        /* Link to next Host entry in the WL */
} BROAD_L3_HOST_WORK_LIST_t;


typedef struct BROAD_L3_HOST_KEY_s
{
  L7_uint8  family;            /* Is host v4 or v6 */
  BROAD_L3_ADDR_USP_t addrUsp; /* IP address + USP */
} BROAD_L3_HOST_KEY_t;

/* Host Flags */
#define  BROAD_L3_HOST_LOCAL   0x1  /* Host entry is local */
#define  BROAD_L3_HOST_TUNNEL  0x2  /* Host entry goes over tunnel */

struct BROAD_L3_HOST_ENTRY_s
{
  BROAD_L3_HOST_KEY_t   key;         /* Key for AVL tree */

  L7_BOOL               unnumbered;  /* Is host on unnumbered intf? */
  L7_BOOL               hwError;     /* Error adding this host to hardware */
  L7_uint8              flags;       /* Host flags - local, tunnel */

  /* BCM Error code when adding hosts. BCM_E_NONE if success */
  L7_int32    rv;

  BROAD_L3_NH_ENTRY_t   *pNhop;        /* Link to host's next hop */ 
  BROAD_L3_NH_ENTRY_t   *pTunNhop;     /* Link to tunnel's next hop */
  BROAD_L3_HOST_ENTRY_t *pMacHostNext; /* List of hosts on a tunnel */

  BROAD_L3_HOST_WORK_LIST_t wl;      /* List of hosts to be worked on */

  void   *reserved;                  /* This field is needed by AVL Library */
};


/* ROUTE data structures */
typedef enum {
  BROAD_L3_ROUTE_CMD_NO_OP = 0,
  BROAD_L3_ROUTE_CMD_ADD,
  BROAD_L3_ROUTE_CMD_MODIFY,
  BROAD_L3_ROUTE_CMD_DELETE,
} BROAD_L3_ROUTE_CMD_t;

typedef struct BROAD_L3_ROUTE_WORK_LIST_s
{
  BROAD_L3_ROUTE_CMD_t       cmd;       /* CMD for Async task */
  BROAD_L3_ROUTE_ENTRY_t    *prev;      /* Link to prev Route entry in the WL */
  BROAD_L3_ROUTE_ENTRY_t    *next;      /* Link to next Route entry in the WL */
} BROAD_L3_ROUTE_WORK_LIST_t;


typedef struct BROAD_L3_ROUTE_KEY_s {
  L7_uint8         family;              /* IPv4 or IPv6 family */
  BROAD_L3_ADDR_t  ipAddr;              /* IP address */ 
  BROAD_L3_ADDR_t  ipMask;              /* IP mask / prefix */
} BROAD_L3_ROUTE_KEY_t;


struct BROAD_L3_ROUTE_ENTRY_s
{
  BROAD_L3_ROUTE_KEY_t key;  /* family, v4 and v6 addr */
 
  /* BCM Error code when inserting the route. BCM_E_NONE if success */
  L7_int32    rv;

  L7_uint8    numNextHops;   /* Number of next hops specified. 1 if non-ECMP */ 

  /* 128-bit prefix routes are added as hosts. If hash full condition, retry */
  L7_BOOL host_conflict;    
   
  BROAD_L3_NH_ENTRY_t   *pNhop;     /* Link to next hop entry for non-ECMP  */
  BROAD_L3_ECMP_ENTRY_t  *pEcmp;    /* Link to ECMP next hop entry */
   
  BROAD_L3_ROUTE_WORK_LIST_t wl;    /* List of routes to be worked on */

  void   *reserved;                 /* This field is needed by AVL Library */
};


/* Tunnel Data structures */

/* Note: Tunnel struct represents various types of tunnels. Some fields are 
 * only applicable for one particular type of tunnels !
 */

typedef enum
{
  BROAD_L3_TUNNEL_CMD_NO_OP=0,
  BROAD_L3_TUNNEL_CMD_CREATE,
  BROAD_L3_TUNNEL_CMD_MODIFY,
  BROAD_L3_TUNNEL_CMD_NHOP_SET,  /* 6o4 only */
  BROAD_L3_TUNNEL_CMD_DELETE
} BROAD_L3_TUNNEL_CMD_t;

typedef struct BROAD_L3_TUNNEL_WORK_LIST_s
{
  BROAD_L3_TUNNEL_CMD_t       cmd;       /* CMD for Async task */
  BROAD_L3_TUNNEL_ENTRY_t    *prev;      /* Link to prev tunnel entry in the WL */
  BROAD_L3_TUNNEL_ENTRY_t    *next;      /* Link to next tunnel entry in the WL */
} BROAD_L3_TUNNEL_WORK_LIST_t;


/*  Assumes IPv4 tunnel initiator and terminator only as that is what hardware
 * currently supports. This should be extensible to IP6 mode of operation using
 * family based addressing (as done for route and host entries)
 */

typedef struct BROAD_L3_TUNNEL_KEY_s
{
  L7_TUNNEL_MODE_t   tunnelMode;     /* L7 tunnel mode */
  L7_sockaddr_union_t    localAddr;  /* Local address of tunnel */
  L7_sockaddr_union_t    remoteAddr; /* Remote address of tunnel */
  
} BROAD_L3_TUNNEL_KEY_t;

typedef struct BROAD_L3_6TO4_NHOP_s
{
  BROAD_L3_NH_ENTRY_t *pV6Nhop;    /* Nhop to go over tunnel */
  BROAD_L3_NH_ENTRY_t *pV4Nhop;    /* v4 nhop for this v6 nhop */
  L7_BOOL              update;     /* Tunnel next hop marked for update */

  /* If tunnel next hop is a relay router, an alternate tunnel is created */
  bcm_if_t             relayTunnelId;  /* L3 intf of tunnel to a relay */
} BROAD_L3_6TO4_NHOP_t;


struct BROAD_L3_TUNNEL_ENTRY_s
{
  BROAD_L3_TUNNEL_KEY_t       key;    /* Tunnel key */

  L7_sockaddr_union_t  nextHopAddr;   /* Tunnel's next hop address */
  DAPI_USP_t           nextHopUsp;    /* Tunnel's next hop USP */

  bcm_if_t  bcmL3IntfId;              /* BCM interface id of the tunnel */
  L7_BOOL   hasTerminator;            /* Hardware state for terminator */
  L7_BOOL   hasInitiator;             /* Hardware state for initiator */

  /* BCM Error code for the tunnel. BCM_E_NONE if success */
  L7_int32    rv;

  /* Link to tunnel's next hop. This Nhop uses L3 intf id of the outgoing
   * routing interface. Transport next hop.
   */
  BROAD_L3_NH_ENTRY_t  *pXportNhop;     
  /* Next hop for routes/hosts going over tunnel. Uses L3 intf id of tunnel's
   * L3 interface
   */
  BROAD_L3_NH_ENTRY_t  *pTunnelNhop;  /* 6o4 and 4o4 tunnels only */

  /* Nhop information for each 6to4 router reachable over this tunnel. Note
   * 6to4 tunnels are one-to-many and not point-to-point
   */
  BROAD_L3_6TO4_NHOP_t  tun6to4Nhops[HAPI_BROAD_L3_MAX_6TO4_NHOP];

  BROAD_L3_TUNNEL_WORK_LIST_t wl;     /* Tunnel work list */ 

  void *reserved;
};


/* Hardware stats */
typedef struct BROAD_L3_HW_STATS_s
{
  L7_uint32 total_adds;           /* Successful adds - cumulative */
  L7_uint32 total_mods;           /* Successful modifies - cumulative */
  L7_uint32 total_dels;           /* Successful deletes - cumulative */
  L7_uint32 total_add_failures;   /* Failed adds - cumulative */
  L7_uint32 total_mod_failures;   /* Failed modifies - cumulative */
  L7_uint32 total_del_failures;   /* Failed deletes - cumulative */
  L7_uint32 current_add_failures; /* Number of current failures */
} BROAD_L3_HW_STATS_t;

#define HAPI_BROAD_L3_INCR_HW_STATS(p, op, rv)                                 \
  {                                                                            \
    if ((op) == 0)                                                             \
    {                                                                          \
      if ((rv) == BCM_E_NONE)                                                  \
        (p)->total_adds++;                                                     \
      else                                                                     \
        (p)->total_add_failures++;                                             \
    }                                                                          \
    else if((op) == 1)                                                         \
    {                                                                          \
      if ((rv) == BCM_E_NONE)                                                  \
        (p)->total_mods++;                                                     \
      else                                                                     \
        (p)->total_mod_failures++;                                             \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      if ((rv) == BCM_E_NONE)                                                  \
        (p)->total_dels++;                                                     \
      else                                                                     \
        (p)->total_del_failures++;                                             \
    }                                                                          \
  }                                                                            \


/* Extern declarations for data structures global with in HAPI L3 component */

extern avlTree_t hapiBroadL3NhopTree;
extern avlTree_t hapiBroadL3EcmpTree;
extern avlTree_t hapiBroadL3HostTree;
extern avlTree_t hapiBroadL3MacTree;
extern avlTree_t hapiBroadL3RouteTree;
extern avlTree_t hapiBroadL3TunnelTree;

extern BROAD_L3_NH_ENTRY_t *hapiBroadL3NhopListHead;
extern BROAD_L3_NH_ENTRY_t *hapiBroadL3NhopListTail;
extern BROAD_L3_ECMP_ENTRY_t *hapiBroadL3EcmpListHead;
extern BROAD_L3_ECMP_ENTRY_t *hapiBroadL3EcmpListTail;
extern BROAD_L3_HOST_ENTRY_t *hapiBroadL3HostListHead;
extern BROAD_L3_HOST_ENTRY_t *hapiBroadL3HostListTail;
extern BROAD_L3_ROUTE_ENTRY_t *hapiBroadL3RouteListHead;
extern BROAD_L3_ROUTE_ENTRY_t *hapiBroadL3RouteListTail;
extern BROAD_L3_TUNNEL_ENTRY_t *hapiBroadL3TunnelListHead;
extern BROAD_L3_TUNNEL_ENTRY_t *hapiBroadL3TunnelListTail;

extern void *hapiBroadL3Sema;
extern osapiRWLock_t hapiBroadL3MacLock;
extern void *hapiBroadL3WakeUpQueue;
extern void *hapiBroadL3WlistWaitQueue;

extern bcm_mac_t hapiBroadL3CpuMac;
extern bcm_if_t  hapiBroadL3CpuIntfId;
extern L7_uint32 hapiBroadL3Ipv6Mask128[];
extern bcm_if_t  hapiBroadL3CpuEgrNhopId;

extern L7_BOOL hapiBroadL3ProcessZeroCountNhops;

extern osapiTimerDescr_t *pBroadL3RetryTimer;

extern BROAD_L3_HW_STATS_t  broadL3HwRouteStats;
extern BROAD_L3_HW_STATS_t  broadL3HwHostStats;
extern BROAD_L3_HW_STATS_t  broadL3HwNhopStats;
extern BROAD_L3_HW_STATS_t  broadL3HwEcmpStats;


/* Function prototypes */


/*******************************************************************************
*
* @purpose Task that handles all async L3 operations. Also updates host and
*          route entries to match the correct L2 state.
*
* @param   num_args - Number of arguments passed
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
void hapiBroadL3AsyncTask(L7_uint32 num_args, DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Adds a route to the work list and signals ASYNC task
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD or
*                DAPI_CMD_IPV6_ROUTE_ENTRY_ADD
* @param  data   Data associated with the command
* @param *dapi_g DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouteEntryAdd(DAPI_USP_t *usp,
                                 DAPI_CMD_t cmd,
                                 void *data,
                                 DAPI_t *dapi_g);

/******************************************************************************
*
* @purpose Deletes an existing route.
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE or
*                DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE
* @param  data   Data associated with the command.
* @param *dapi_g DAPI Driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouteEntryDelete(DAPI_USP_t *usp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Deletes all the existing IPv4 routes.
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL
* @param  data   Data associated with the command.
* @param *dapi_g DAPI Driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t
hapiBroadL3RouteEntryDeleteAll (DAPI_USP_t *usp,
                                DAPI_CMD_t cmd,
                                void *data,
                                DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Adds a IPv4/IPv6Host in the AVL tree and queues it to work list.
*
* @param  usp     unit/slot/port of routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_ADD or
*                 DAPI_CMD_IPV6_NEIGH_ENTRY_ADD
* @param  data    Data associate with host add command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t L7_SUCCESS or L7_FAILURE
*
* @notes  It is not out of question for the V6 stack to report changes to
*         IPv6 hosts as host-add requests (in fact, that seems to be the way
*         it is).
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostEntryAdd(DAPI_USP_t *usp,
                                DAPI_CMD_t cmd,
                                void *data,
                                DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Deletes a Host from the tables
*
* @param  usp     unit/slot/port of routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_DELETE or
*                 DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE
* @param  data    Data associated with host delete command.
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostEntryDelete(DAPI_USP_t *usp,
                                   DAPI_CMD_t cmd,
                                   void *data,
                                   DAPI_t *dapi_g);

/******************************************************************************
*
* @purpose Deletes all the IPv4 Host entries from the tables
*
* @param  usp     unit/slot/port of routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL
* @param  data    Data associated with host delete command.
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t
hapiBroadL3HostEntryDeleteAll (DAPI_USP_t *usp,
                               DAPI_CMD_t cmd,
                               void *data,
                               DAPI_t *dapi_g);

/******************************************************************************
* Get the CPU HAPI Port Pointer
*******************************************************************************/
BROAD_PORT_t * hapiBroadL3CpuHapiPortGet(DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Check if a given VLAN has routing enabled. If yes, return USP
*
* @param  vlanId    VLAN to be checked
* @param  vlanUsp   USP of the vlan routing interface (output param)
* @param *dapi_g    DAPI driver object
*
* @returns L7_BOOL
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_BOOL hapiBroadL3VlanRoutingEnabled(L7_ushort16 vlanId,
                                      DAPI_USP_t  *vlanUsp,
                                      DAPI_t *dapi_g);

/******************************************************************************
*
* @purpose Add local host MAC address rule to trap ARP replies to CPU.
*          Idea is to avoid tunneling ARP replies. If protocol_pkt_control
*          register is used, packets will be tunneled.
*
* @param   DAPI_USP_t *usp - USP of the L3 interface
* @param   mac - MAC Address for this rule.
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t
*
* @notes  The MAC rule is added when IP interface is created
*         and when new unit joins the stack.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostPolicyInstall (DAPI_USP_t *usp, L7_uchar8 *mac, DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Delete the local host MAC address policy.
*
* @param   DAPI_USP_t *usp - USP of the L3 interface
* @param   mac - MAC Address of the L3 interface.
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t
*
* @notes  The policy is deleted when L3 interface is deleted
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostPolicyRemove (DAPI_USP_t *usp, L7_uchar8 *mac, DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Remove all protocol VLAN associations for this port
*
* @param   DAPI_USP_t *usp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   Typically called when port based routing is enabled on a port.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3ProtocolVlanRemove(DAPI_USP_t *usp, DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Restore all protocol VLAN associations for this port
*
* @param   DAPI_USP_t *usp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   Typically called when port based routing is disabled on a port.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3ProtocolVlanAdd (DAPI_USP_t *usp, DAPI_t *dapi_g);


/*******************************************************************************
* @purpose  Insert an entry into AVL tree. Wrapper for avlInsertEntry()
*
* @param    tree - pointer to the AVL tree structure
* @param    item - pointer to the item to be inserted
*
* @returns  see avlInsertEntry() for return pointer.
*
* @comments
*
* @end
*******************************************************************************/
void *hapiBroadL3AvlInsert(avlTree_t *tree, void *item);


/******************************************************************************
*
* @purpose Update the state of an existing next hop entry.
*
* @param   Pointer to the next hop entry.
* @param   Next hop command.
*
* @returns none.
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
void hapiBroadL3NhopEntryUpdate (BROAD_L3_NH_ENTRY_t *pNhopEntry,
                                 BROAD_L3_NH_CMD_t   nextCmd);


/*******************************************************************************
*
* @purpose  Unlinks Nhop entry from a MAC entry
*
* @param  pHostEntry - Host entry
*
* @returns L7_RC_t result
*
* @notes  MAC entry is deleted if no one is referencing the entry.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3MacNhopUnlink (BROAD_L3_NH_ENTRY_t *pNhopEntry);


/******************************************************************************
*
* @purpose  Manage the special IPv6 route that sends link local traffic
*           (i.e. traffic with an FE80::/10 prefix) to the CPU port. Idea is
*           to use one entry for all link locals.
*
* @param install          If L7_TRUE, install the route, L7_FALSE remove it.
* @param *dapi_g          system information
*
* @returns void
*
* @notes   Only Link-local IPv6 traffic destined to router MAC Address reaches
*          CPU. Currently all v6 interfaces have same Link-local IPv6
*          address by default.
*          The Link local route will take one v6 route entry thereby reducing
*          max configurable v6 routes by 1.
*          Ideally, this should be done via route work list, but we don't want
*          to allow link local routes from application.
*
* @end
*
*******************************************************************************/
void hapiBroadL3v6LinkLocalRouteAddDel(L7_BOOL install, DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose  Check if v6 prefix lengths > 64 are supported or not. Helix/FB/FB2
*           devices supported prefixes upto 64bits and v6 routes with prefix
*           lengths /65 to /127 were not supported (routes with /128 prefix
*           were handled as v6 host entry instead of v6 route entry).
*
*           Some of the new XGS3 devices have support for upto 128bit prefix
*           lengths. This function checks the XGS3 family type to detect whether
*           128-bit prefix lengths are supported.
*
* @param    none
*
* @returns  L7_BOOL - True if 128bit prefix is supported. Else False.
*
* @notes
*
* @end
*
*******************************************************************************/
L7_BOOL hapiBroadL3Ipv6Prefix128Supported(void);

/******************************************************************************
*
* @purpose Update the Work list command of an existing ECMP entry.
*
* @param   Pointer to the ECMP entry.
* @param   ECMP command.
*
* @returns none.
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
void hapiBroadL3EcmpEntryUpdate (BROAD_L3_ECMP_ENTRY_t *pEcmpEntry,
                                 BROAD_L3_ECMP_CMD_t   nextCmd);


/******************************************************************************
*
* @purpose  Given L7 tunnel mode, return the BCM tunnel type
*
* @param    none
*
* @returns  L7_BOOL - True if 128bit prefix is supported. Else False.
*
* @notes
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3BcmTunnelType(L7_TUNNEL_MODE_t l7TunnelType,
                                 L7_uint32 *bcmTunnelType);


/******************************************************************************
*
* @purpose Add an L3-enabled MAC address entry to hardware
*
* @param  usp       unit slot port
* @param  pMacAddr  MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3MacAddressAdd(DAPI_USP_t *usp, L7_enetMacAddr_t *pMacAddr,
                                 DAPI_t *dapi_g);

/*******************************************************************************
*
* @purpose Delete an L3-enabled MAC address entry from hardware
*
* @param  usp       unit slot port
* @param  pMacAddr  MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3MacAddressDelete(DAPI_USP_t *usp, L7_enetMacAddr_t *pMacAddr,
                                    DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose  Send a message to ASYNC task and wait for Wlist processing to
*           complete
*
* @param    DAPI_CMD_t
*
* @returns  none
*
* @notes    L7_RC_t
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3AsyncWait (DAPI_USP_t *usp, DAPI_CMD_t cmd);


/*******************************************************************************
*
* @purpose Allocate and Initialize L3 cache
*
* @param   none
*
* @returns L7_RC_t result
*
* @notes   Called only once during L3 Init.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3CacheInit(void);


/*******************************************************************************
*
* @purpose Create/cache L3 next hop egress object in hardware
*
* @param   egrFlags      - Egress object flags
* @param   pNhop         - pointer to internal L3 nexthop struct
* @param   pBcmxInfo     - BCMX data assoicated with the L3 nexthop egress object.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 Nexthop cache is commited to hardware,
*            - when Nexthop cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwNhopAdd (L7_uint32 egrFlags, BROAD_L3_NH_ENTRY_t *pNhopEntry,
                           usl_bcm_l3_egress_t *pBcmInfo, L7_BOOL cacheOrCommit);


/*******************************************************************************
*
* @purpose Delete/cache L3 next hop egress object (s) from hardware
*
* @param   egressId      - BCMX data assoicated with the L3 egress object.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 Nhop cache is commited to hardware,
*            - when route cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwNhopDelete (usl_bcm_l3_egress_t *pBcmInfo, bcm_if_t egressId,
                              L7_BOOL  cacheOrCommit);

/*******************************************************************************
*
* @purpose Commit the Nexthop cache to hardware
*
* @param   none
*
* @returns none
*
* @notes   If cache command is set to ADD, then cached nexthop entries are added
*          to hardware. If cmd is DELETE, then entries are deleted from hardware.
*
* @end
*
*******************************************************************************/
void hapiBroadL3NhopCacheCommit (void);


/*******************************************************************************
*
* @purpose Add/cache L3 host entry to hardware
*
* @param   pHostEntry    - pointer to internal L3 host struct
* @param   pBcmxInfo     - BCMX data assoicated with the L3 host.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 host cache is commited to hardware,
*            - when host cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwHostAdd (BROAD_L3_HOST_ENTRY_t *pHostEntry,
                           usl_bcm_l3_host_t *pBcmInfo,
                           L7_BOOL          cacheOrCommit);

/*******************************************************************************
*
* @purpose Delete/cache L3 host entry from hardware
*
* @param   pBcmxInfo     - BCMX data assoicated with the L3 host.
* @param   cacheOrCommit - Cache/Commit
*
* @returns none
*
* @notes   L3 host cache is commited to hardware,
*            - when host cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwHostDelete (usl_bcm_l3_host_t *pBcmInfo,
                              L7_BOOL          cacheOrCommit);


/*******************************************************************************
*
* @purpose Commit the Host cache to hardware
*
* @param   none
*
* @returns none
*
* @notes   If cache command is set to ADD, then cached host entries are added
*          to hardware. If cmd is DELETE, then entries are deleted from hardware.
*
* @end
*
*******************************************************************************/
void hapiBroadL3HostCacheCommit (void);


/*******************************************************************************
*
* @purpose Add/cache L3 route entry to hardware
*
* @param   pRouteEntry - pointer to internal L3 route struct
* @param   pBcmxInfo   - BCMX data assoicated with the L3 route.
* @param   flag        - Cache/Commit
*
* @returns none
*
* @notes   L3 route cache is commited to hardware,
*            - when route cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwRouteAdd (BROAD_L3_ROUTE_ENTRY_t *pRouteEntry,
                            usl_bcm_l3_route_t      *pBcmInfo,
                            L7_BOOL                 flag);

/*******************************************************************************
*
* @purpose Delete/cache L3 route entry from hardware
*
* @param   pBcmxInfo   - BCMX data assoicated with the L3 route.
* @param   flag        - Cache/Commit
*
* @returns none
*
* @notes   L3 route cache is commited to hardware,
*            - when route cache is full
*            - when command associated with cache changes
*            - when caller explicity specifies to commit
*
* @end
*
*******************************************************************************/
void hapiBroadL3HwRouteDelete (usl_bcm_l3_route_t *pBcmInfo,
                               L7_BOOL            flag);

/*******************************************************************************
*
* @purpose Commit the route cache to hardware
*
* @param   none
*
* @returns none
*
* @notes   If cache command is set to ADD, then cached route entries are added
*          to hardware. If cmd is DELETE, then entries are deleted from hardware.
*
* @end
*
*******************************************************************************/
void hapiBroadL3RouteCacheCommit (void);

/*******************************************************************************
*
* @purpose Periodic timer to retry failures that have occured due to transient
*          error conditions that might have cleared overtime.
*
* @param   arg1
* @param   arg2
*
* @returns none
*
* @notes   Failed entries are retried either periodically or if any other entry
*          gets deleted
*
* @end
*
*******************************************************************************/
void hapiBroadL3RetryTimer(L7_uint32 arg1, L7_uint32 arg2);


/*******************************************************************************
* @purpose  Checks if there is room for a new host entry
*
* @param    cmd - if host is being added/modified
* @param    pHostInfo - Host IP address
* @param    pMac -  MAC Address
* @param    dapi_g - driver object pointer
*
*
* @returns  L7_TRUE - If host table is full
*           L7_FALSE - If there is room in host table.
*
* @comments
*
* @end
*******************************************************************************/
L7_BOOL hapiBroadL3HostTableFull(BROAD_L3_HOST_CMD_t cmd,
                                 BROAD_L3_HOST_KEY_t *pHostInfo,
                                 L7_enetMacAddr_t    *pMac,
                                 DAPI_t *dapi_g);


/*******************************************************************************
* @purpose  Checks if there is room for a route entry
*
* @param    cmd -  route is being added/modified
* @param    family - IPv4 or IPv6 family of route
* @param    pRtNhop - list of next hops for the route
* @param    dapi_g - driver object pointer
*
* @returns  L7_TRUE - If  route table is full.
*           L7_FALSE - If there is room in route table.
*
* @end
*******************************************************************************/
L7_BOOL hapiBroadL3RouteTableFull(BROAD_L3_ROUTE_CMD_t cmd, L7_uint8 family,
                                  DAPI_ROUTING_ROUTE_ENTRY_t * pRtNhop,
                                  DAPI_t *dapi_g);

/*******************************************************************************
* @purpose  Callback function from USL to indicate that a host could not
*           be inserted into the HW. HAPI acts on this by utilizing the
*           retry mechanism.
*
* @param    None
*
* @returns  
*
* @comments
*
* @end
*******************************************************************************/
void hapiBroadL3AsyncHostFailureCallback(void *data);

/******************************************************************************
*
* @purpose Set the IP MTU for the given routing interface
*
* @param  usp       USP of the routing interface
* @param  mtu       MTU of the routing interface
* @param *dapi_g    DAPI driver object
*
* @returns L7_RC_t
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3IntfMtuSet (DAPI_USP_t *usp, L7_ulong32  mtu, DAPI_t *dapi_g);


#endif  /* BROAD_L3_INT_H_INCLUDED */
