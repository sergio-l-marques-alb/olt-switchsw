/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename traceroute.h
*
* @purpose     Header file to be included by traceroute.c
*
* @component  Traceroute
*
* @comments   This file contains all the defines, function prototypes
*             and include files required by traceroute.c
*
* @create     05/19/2004
*
* @author     ssharma
* @end
*
**********************************************************************/
#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "l7_icmp.h"
#include "traceroute_map_api.h"
#include "l7_resources.h"
#include "traceroute_exports.h"

#ifdef _L7_OS_VXWORKS_
#include <vxWorks.h>
#include <sys/types.h>
#endif /* _L7_OS_VXWORKS */

#ifdef _L7_OS_LINUX_
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#endif /* _L7_OS_LINUX */

/* Some defines used internally */
#define TRACEROUTE_INVALID_RTT      0xFFFFFFFF
#define TRACEROUTE_INTERVAL_MULTIPLIER 1000000

/* Max Buff size is traceroute payload + other headers as IP,ICMP/UDP */
#define TRACEROUTE_MAX_BUFF (TRACEROUTE_MAX_PROBE_SIZE + 64)
#define TRACEROUTE_MAX_WAIT_TIME (10 * TRACEROUTE_INTERVAL_MULTIPLIER)
#define TRACEROUTE_INDEX_MASK ((L7_ushort16) 0xF)
#define TRACEROUTE_HANDLE_INCREMENT ((L7_ushort16) 0x0010)

/* Max hop infos stored. Allow enough space for default maxTtl at least*/
#define TRACEROUTE_MAX_HOP_INFO       \
    (TRACEROUTE_DEFAULT_MAX_TTL - TRACEROUTE_DEFAULT_INIT_TTL)

#define TRACEROUTE_DEFAULT_MAX_TIME       (60 * TRACEROUTE_INTERVAL_MULTIPLIER)

/* Signature to verify ICMP packet is response to our packet. Make sure this 
 * string of odd size. */
#define TRACEROUTE_SIGNATURE "RFC2925-TRACEROUTE "
/*********************************************************************
*
* @structures  traceRouteAddrType_t
*
* @purpose     Defines address types supported for traceroute probes
*
* @notes       none
*
* @end
*********************************************************************/
typedef enum {
    TRACEROUTE_ADDR_IPV4 = 0,   /* IPv4 */
    TRACEROUTE_ADDR_INVALID     /* Nothing else supported */
} traceRouteAddrType_t;

/*********************************************************************
*
* @structures  traceRouteMode_t
*
* @purpose     Defines modes of operation
*
* @notes       Mode can be sync or async
*              sync mode:  Complete operation done before returning. No need to 
*                          call traceRouteSessionEnd() API.
*              async mode: Traceroute initiated and function returns. Response 
*                          must query with api traceRouteSessionQuery later.
*
* @end
*********************************************************************/
typedef enum {
    TRACEROUTE_MODE_SYNC,
    TRACEROUTE_MODE_ASYNC
} traceRouteMode_t;

/*********************************************************************
*
* @structures  traceRouteState_t
*
* @purpose     Defines state of the traceroute session
*
* @notes       
*   TRACEROUTE_STATE_FREE          - Entry is free and not yet used
*   TRACEROUTE_STATE_IN_PROGRESS   - Means traceroute initiated and in progress
*   TRACEROUTE_STATE_DONE          - Means traceroute operation is complete
*
*   When a new traceroute is initiated, we will try to allocate an entry from 
*   table that is free. If no entry free, then we will try to use an entry in 
*   TRACEROUTE_STATE_DONE state. If there is no entry in this state, 
*   then no entry can be allocated and traceroute cannot be initiated.
* @end
*********************************************************************/
typedef enum {
    TRACEROUTE_STATE_FREE,            /* Free entry */
    TRACEROUTE_STATE_ALLOCATED,       /* Entry allocated, session not started */
    TRACEROUTE_STATE_IN_PROGRESS,     /* Traceroute in progress */
    TRACEROUTE_STATE_DONE,            /* Traceroute done, results available */
    TRACEROUTE_STATE_EXCEED_FAILURES, /* Traceroute failed. Too many failures */
    TRACEROUTE_STATE_EXCEED_TTL,      /* Traceroute failed. TTL Exceeded */
    TRACEROUTE_STATE_UNREACHABLE,     /* Destination Unreachable */
    TRACEROUTE_STATE_FRAG_ERR,        /* Needs Fragmentation */
    TRACEROUTE_STATE_INTERNAL_ERR,    /* Internal Error */
} traceRouteState_t;

/*********************************************************************
*
* @structures  traceRouteHops_t
*
* @purpose     Structure defined to store hop information for traceroute
*
* @notes       
*
*   Structure defining the different hops for traceroute operation.
*
*   The response does not contain all fields of the request, for example IP-DA. 
*   It is expected that initiator of request will maintain the information if 
*   needed. (The fields can be added to response if needed by any module).
*
*   Note, there is only on hopDa. If there are multiple probes sent and
*   response for probe arrives from different destinations, it will contain
*   response only from the last valid response.
* @end
*********************************************************************/

typedef struct {
    traceRouteState_t state;/* State of the hop */
    L7_uint32   hopDa;      /* IP Address of destination responding for hop */
    L7_uint32   minRtt;     /* Min RTT for this hop in usec */
    L7_uint32   maxRtt;     /* Max RTT for this hop in usec */
    L7_uint32   avgRtt;     /* Avg RTT for this hop in usec */
    L7_uint32   probeSent;  /* Number of probes sent for the  hop */
    L7_uint32   probeResponses; /* Number of responses for this hop */
    L7_uint32   probeSuccess; /* Number of successfull probe responses */
    L7_ushort16 ttl;        /* TTL used for the probe */
} traceRouteHops_t;

/*********************************************************************
*
* @structures  traceRouteTable_t
*
* @purpose     Structure containing information for a traceroute session.
*
* @notes       Contains information about both request and response.
*
*              The structure contains response for last TRACEROUTE_MAX_HOP_INFO 
*              entries. If there are more hops, then earlier ones are 
*              overwritten. The array hopInfo is treated as a circular array. 
*              The start and end of the array are defined by fields hopCount.
*              The circular array must be treated as follows,
*                   If hopCount <= TRACEROUTE_MAX_HOP_INFO, then 
*                       indexes 0 through hopCount-1 are valid.
*                   If hopCount > TRACEROUTE_MAX_HOP_INFO, then 
*                       The start and ending entries in circular array are,
*                       Start entry = ((hopCount+1)%TRACEROUTE_MAX_HOP_INFO)
*                       Last Entry = hopCount % TRACEROUTE_MAX_HOP_INFO.
*
* @end
*********************************************************************/
typedef struct _traceRouteTable_s {
    L7_uchar8               index[ TRACEROUTE_MAX_INDEX_LEN ]; 
                                    /* Index string of session */
    L7_uchar8               name[ TRACEROUTE_MAX_NAME_LEN ]; 
                                    /* Name of the sesion */
    traceRouteState_t       state;      /* State of the session entry */
    L7_ushort16             handle;     /* handle for the session */
    L7_uint32               txSock;     /* Socket FD used to send PDUs */
    L7_uint32               rxSock;     /* Socket FD used to receive PDUs */
    /* Following two fields are intended to put a limit on how long a
     * traceroute sesion can run. They are not used in current version */
    /* L7_uint32               startTime;*//*Time at start of session in usec */
    /* L7_uint32               maxTime;*//* Max time for traceroute in usec */

    traceRouteMode_t        mode;       /* Mode sync/async */
    L7_uint32               vrfId;      /* VRF-ID of the session */
    traceRouteAddrType_t    addrType;   /* Addr type */
    L7_uint32               ipDa;       /* Destination IP Address */
    L7_ushort16             probeSize;  /* Size of the probe request */
    L7_ushort16             probePerHop;/* Number of probes per hop */
    L7_uint32               probeInterval;/* Interval between probes in usec */
    L7_ushort16             port;       /* Port number for the probe */
    L7_BOOL                 dontFrag;   /* Dont Fragment option */
    L7_ushort16             maxTtl;     /* Max value for TTL field */
    L7_ushort16             initTtl;    /* Initial value for TTL field */
    L7_ushort16             maxFail;    /* Maximum number of failures 
                                            allowed in a session */
    traceRouteCallback      callback;   /* User Callback function */
    void                    *userParam; /* User handle passed in callback */

    L7_ushort16             currTtl;       /*TTL value used for current probe*/
    L7_ushort16             currProbeCount;/* Current probe count */
    L7_ushort16             testAttempts;  /* Number of attempts done */
    L7_ushort16             testSuccess;   /* Number of successfull attempts */
    L7_ushort16             consecutiveFail; /* Number of consecutive fails */
    L7_uint32               probeIntervalResidue;/* Remaining time for expiry 
                                                    in usec */
    L7_clocktime            txTime;        /* Time last probe was sent */

    L7_ushort16             hopCount;      /* Number of hops processed */
    traceRouteHops_t        hopInfo[TRACEROUTE_MAX_HOP_INFO]; /* Max Hops */
} traceRouteTable_t;

/* Functions used in more than one file */
void traceRouteGetEntry( L7_ushort16 index, traceRouteTable_t **traceEntry );
L7_RC_t traceRouteDone(traceRouteTable_t *traceEntry, traceRouteState_t state);
L7_RC_t traceRouteSync( traceRouteTable_t *traceEntry );
L7_RC_t traceRouteAsync( traceRouteTable_t *traceEntry );
L7_RC_t traceRouteCreateSock( traceRouteTable_t *traceEntry );

#endif /* TRACEROUTE_H */ 
