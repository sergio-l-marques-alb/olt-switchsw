/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename ping_private.h
*
* @purpose Provides private difinitions used for "ping" functionality
*
* @component ping
*
* @comments
*
* @create 08/16/2005
*
* @author Praveen
*
* @end
*
**********************************************************************/
#ifndef _PING_PRIVATE_H_
#define _PING_PRIVATE_H_

#include "ping_api.h"
#include "ping_exports.h"

/* Limits on the data structures */
#define PING_MAX_SESSIONS       16    /* Number of simultaneous sessions */
#define PING_MAX_PROBE_INFO     16    /* Number of responses stored */

/* Some defines used internally */
#define PING_INDEX_MASK       (L7_ushort16)0xF    /* Last 4 bits of handle */
#define PING_HANDLE_INCREMENT (L7_ushort16)0x0010 /*Incr upper part of handle*/
#define PING_IP_HEADER_LEN    20                  /* Length of IP header */
#define PING_MAX_BUFF         (PING_MAX_SIZE+ICMP_MINLEN+PING_IP_HEADER_LEN+1)

#define PING_ITERATION_COUNT        3  /* Iterations to wait for response to
                                          last probe request */

/* Some constants used in program */
#define PING_INTERVAL_MULTIPLIER 1000000
/* Max wait time per call to select fn  - 10 seconds*/
#define PING_MAX_WAIT_TIME     10 * PING_INTERVAL_MULTIPLIER
#define PING_INVALID_RTT      (L7_uint32)0xFFFFFFFF /* Invalid RTT */

/* Some of the definitions below must actually be in ping_api.h, but they are
 * added here since applications are not using them and hence interfaces are
 * not using these structures
 */

/*********************************************************************
*
* @structures  pingProbeResult_t
*
* @purpose     Contains enumerations to describe the result of ping
*
* @notes       Enumeration to define the status of probes.
*              Contains values defined by RFC 2925 and proprietary values
*              Not all values defined in RFC 2925 are supported.
*
* @end
*********************************************************************/
typedef enum {
    PING_PROBE_SENT = 0,        /* Probe sent - response not known yet */
    PING_PROBE_SUCC,            /* Probe succeeded */
    PING_PROBE_UNKNOWN,         /* unknown error */
    PING_PROBE_INTERNAL_ERR,    /* Internal Error */
    PING_PROBE_TIMEOUT,         /* timeout */
    PING_PROBE_UNKNOWN_DA,      /* Invalid Destination Address */
    PING_PROBE_NO_ROUTE,        /* no route to IP-DA */
    PING_PROBE_INTF_ERR,        /* error in intf to be used for probe*/
    PING_PROBE_ARP_FAIL,        /* ARP failure */
    PING_PROBE_MAX_SESSION_REACHED, /* Max number of probe session reached */
    PING_PROBE_DNS_ERR,         /* Error in DNS resolution of IP-DA */
    PING_PROBE_INVALID_DA,      /* Invalid IP-DA, ex MC Addr, BC Addr */
    PING_PROBE_NO_INSTANCE      /* No such instance of probe */
} pingProbeResult_t;

/*********************************************************************
*
* @structures  pingProbeRespCode_t
*
* @purpose     Defines values for rc field of Ping Response
*
* @notes       Response code in the ICMP error message received for the request.
*
* @end
*********************************************************************/

typedef enum {
    PING_PROBE_RC_SUCC = 0,        /* Probe succeeded */
    PING_PROBE_RC_FAIL             /* Probe failed */
} pingProbeRespCode_t;

/*********************************************************************
*
* @structures  pingProbeAddrType_t
*
* @purpose     Defines address types supported for ping probes
*
* @notes       none
*
* @end
*********************************************************************/
typedef enum {
    PING_PROBE_ADDR_IPV4 = 0,   /* IPv4 */
    PING_PROBE_ADDR_INVALID     /* Nothing else supported */
} pingProbeAddrType_t;

/*********************************************************************
*
* @structures  pingProbeMode_t
*
* @purpose     Defines modes of operation
*
* @notes       Mode can be sync or async
*              sync mode:  Complete operation done before returning. No need to
*                          call pingSessionFree() API.
*              async mode: Ping initiated and function returns. Response must
*                          be queried using api pingSessionQuery API later.
*
* @end
*********************************************************************/
typedef enum {
    PING_PROBE_MODE_SYNC,
    PING_PROBE_MODE_ASYNC
} pingProbeMode_t;

/*********************************************************************
*
* @structures  pingProbeProtoType_t
*
* @purpose     Defines protocol type of probe being used for session.
*
* @notes       Currently supports ICMP only.
*
* @end
*********************************************************************/
typedef enum {
    PING_PROBE_PROTO_ICMP = 0,   /* Probe with ICMP packets */
    PING_PROBE_PROTO_INVALID     /* Nothing else supported */
} pingProbeProtoType_t;

/*********************************************************************
*
* @structures  pingSessionState_t
*
* @purpose     Defines state of the ping session
*
* @notes
*   PING_SESSION_STATE_FREE          - Entry is free and not yet used
*   PING_SESSION_STATE_IN_PROGRESS   - Means Ping initiated and in progress
*   PING_SESSION_STATE_DONE          - Means Ping operation is complete
*
*   When a new ping is initiated, we will try to allocate an entry from table
*   that is free. If no entry free, then we will try to use an entry in
*   PING_SESSION_STATE_DONE state. If there is no entry in this state,
*   then no entry can be allocated and ping cannot be initiated.
* @end
*********************************************************************/
typedef enum {
    PING_SESSION_STATE_FREE,            /* Free entry */
    PING_SESSION_STATE_ALLOCATED,       /* Entry allocated and being built.
                                           Ping not yet started */
    PING_SESSION_STATE_IN_PROGRESS,     /* Ping in progress */
    PING_SESSION_STATE_DONE             /* Ping done - resp already read */
} pingSessionState_t;


/*********************************************************************
*
* @structures  pingProbeInfo_t
*
* @purpose     Structure containing result of individual probes
*
* @notes       txTime contains time when probe was sent.
*              rxTime contains time when result of probe was determined.
*              The delay between sending the probe and response can be got
*              by subtracting rxTime and txTime.
*
* @end
*********************************************************************/
/* Structure containing result of individual probes */
typedef struct {
    L7_clocktime        txTime; /* Time when probe was sent */
    L7_clocktime        rxTime; /* Time when result of probe was determined */
    pingProbeResult_t   result;/* Result of the probe */
    pingProbeRespCode_t rc;    /* Reason for failure */
} pingProbeInfo_t;

/*********************************************************************
*
* @structures  pingSessionTable_t
*
* @purpose     Structure containing information for a ping session.
*
* @notes       Contains information about both request and response.
*              Also maintains information about each individual probes.
*              The information is needed to find minRtt/maxRtt even though
*              history table is not yet supported.
*
*              The structure contains response for last PING_MAX_PROBE_INFO
*              entries. If more probes are sent, then earlier ones are
*              overwritten. The array respInfo is treated as a circular array.
*              The start and end of the array are defined by fields probeSent.
*              The circular array must be treated as follows,
*                   If probeSent <= PING_MAX_PROBE_INFO_PER_SESSION, then
*                       indexes 0 through probeSent-1 are valid.
*                   If probeSent > PING_MAX_PROBE_INFO_PER_SESSION, then
*                       The start and ending entries in circular array are,
*                       Start entry = ((probeSent + 1) % PING_MAX_PROBE_INFO)
*                       Last Entry = probeSent % PING_MAX_PROBE_INFO.
*              Since the response info maintains only last
*              PING_MAX_PROBE_INFO_PER_SESSION entries, if response for a
*              probe is delayed more than PING_MAX_INFO_PER_SESSION *
*              pingInterval, it is treated as a delayed response and
*              probeDelayed counter is incremented. If probe arrives before
*              PING_MAX_INFO_PER_SESSION*pingInterval, it is treated as
*              successfull probe and probeSuccess is incremented.
*
*              The table is accessed using two keys from the APIs
*              1. Using the handle as key. In this case the lower bits are
*                 directly used as index into the table.
*              2. Using index and name fields as key. In this case, the table
*                 is to be searched to find matching entry. Additionally, Get
*                 and GetNext APIs must access the sessions in chronological
*                 order.
*                 Currently, using linear search on table (since table is
*                 quite small) to find the next chronological entry.
*                 An optimal implementation is to have secondary indexing into
*                 the table with index and name as keys.
* @end
*********************************************************************/
typedef struct _pingSessionTable_s {
    pingSessionState_t  state;      /* State of the session entry */
    L7_uchar8           index[ PING_MAX_INDEX_LEN ];
                                    /* Index string of session */
    L7_uchar8           name[ PING_MAX_NAME_LEN ]; /* Name of the sesion */
    L7_ushort16         handle;     /* handle is also used as Identifier
                                       in ping packets */
    pingProbeMode_t     mode;       /* Mode sync/async */
    L7_uint32           sock;       /* Socket FD  used for session */
    L7_ushort16         probeCount; /* Number of probes to send */
    L7_ushort16         probeSize;  /* Size of each probe */
    L7_uint32           probeInterval;  /* Time between probes */
    L7_uint32           probeIntervalResidue;/* Time in usec for next probe */
    L7_ushort16         probeIterations;/* Number of iterations probe must be
                                           executed */
    L7_uint32           probeSrcIpAddr;   /* source IP address of Echo Request */
    L7_uint32           vrfId;          /* VRF-ID of the session */
    pingProbeProtoType_t proto;     /* Protocol used for probes */
    pingProbeAddrType_t addrType;   /* Addr type */
    L7_uint32           ipDa;       /* IP-DA for the probe session */
    L7_uint32           minRtt;     /* Min RTT in the current session in usec*/
    L7_uint32           maxRtt;     /* Max RTT in the current session in usec*/
    L7_uint32           aggrRtt;    /* Sum of RTT for successfull probes.
                                     * Used to calculate avgRTT in usec*/
    L7_ushort16         probeSuccess;   /* Number of probes succeeded */
    L7_ushort16         probeSent;      /* Number of probes sent */
    L7_ushort16         probeDelayed;   /* Number of delayed probes responses */
    pingProbeInfo_t     info[ PING_MAX_PROBE_INFO ];
    pingSessionCallback callback;      /* Callback function for the session */
    pingSessionFinishCallback finishCallback;  /* Completion Callback function for the session*/
    L7_uint32           dscp; /* IP DSCP value to be used for this session */
    void                *userParam;     /* Parameter returned in callback */
} pingSessionTable_t;

/* Functions used across files */
extern void pingGetEntry(L7_ushort16 index, pingSessionTable_t **pingEntry);
extern L7_RC_t pingEnd(pingSessionTable_t *pingEntry, pingSessionState_t state);
extern L7_RC_t pingCreateSock( pingSessionTable_t *pingEntry );
extern L7_RC_t pingSessionSync( pingSessionTable_t *pingEntry );
extern L7_RC_t pingSessionAsync( void );
extern L7_BOOL pingDebugPathTraceFlagGet();
extern void pingDebugPathTraceFlagSet(L7_BOOL flag);
#define PING_PATH_TRACE(__fmt__, __args__... )                              \
  if (pingDebugPathTraceFlagGet() == L7_TRUE)                  \
{                                                                 \
 L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SIM_COMPONENT_ID, __fmt__, __args__); \
}

#endif /* _PING_PRIVATE_H_ */
