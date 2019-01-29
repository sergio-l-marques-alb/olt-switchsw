
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  traceroute.c
*
* @purpose Provide "traceroute" functionality as defined in RFC 2925
*
* @component traceroute
*
* @comments
*       This file contains implementation of traceroute as defined in RFC 2925.
*       Implementation of traceroute as in RFC 2925 can be split into two parts.
*       1. Support for RFC-2925. Contains tables, configuration etc... needed
*          to support the RFC
*       2. Packet Handling. Contains functionality needed to generate packets
*          or to handle the packet received.
*       This splitting will enable traceroute implementation to work both when
*       we use sockets for packet i/o or if we use our own implementation for
*       i/o.
*
*       The traceroute application will not support the MIB defined in RFC-2925
*       directly. It will however provide the API's and data structures needed
*       to support the MIB. The SNMP implementation can use the APIs to
*       provide the semantics as needed by the RFC. The implementation can
*       infact be used by both CLI and HTTP as well.
*
* @create 08/16/2005
*
* @author Praveen
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#include "l7_socket.h"
#include "l7_icmp.h"
#include "simapi.h"
#include "l7_ip_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "traceroute_map_api.h"
#include "traceroute.h"

static L7_RC_t traceRouteProcessTimer( L7_uint32 elapsedTime,
        traceRouteTable_t *traceEntry );
static void *traceRouteASyncTask( void *param );
static L7_RC_t traceRouteASyncSetFd( L7_ushort16 *maxFd, fd_set *rFds,
        fd_set *eFds );
static L7_uint32 traceRouteASyncGetTimeout( );
static L7_RC_t traceRouteSendProbe( traceRouteTable_t *traceEntry );
static L7_RC_t traceRouteRxPdu( traceRouteTable_t *traceEntry );
static L7_RC_t traceRouteRxErr( traceRouteTable_t *traceEntry );
static L7_RC_t traceRouteNextHop( traceRouteTable_t *traceEntry );

static L7_uint32 waitForICMPReply(traceRouteTable_t *traceEntry, L7_uint32
        icmpSockfd, L7_uint32 *ipAddr, L7_ipHeader_t *icmpPacket,
        L7_uint32 icmpPacketLen, L7_uint32 port, L7_uint32 *icmpType);
#if 0
/* TODO: The function was used in earlier implementation, but does not seem to
 * be needed anymore
 */
static L7_BOOL isLocalAddr(L7_IP_ADDR_t ipAddr);
#endif
static L7_RC_t traceRouteProbeEnd( traceRouteTable_t *traceEntry,
        L7_BOOL timeOut, L7_ushort16 icmpCode, L7_uint32 rtt );
static L7_RC_t traceRouteTimeUSecElapsed( L7_clocktime *end,
        L7_clocktime *start, L7_uint32 *elapsedTime );

/* Number of outstanding traceroute requests */
static L7_uint32 traceRouteCount;

/* Task-Id for task processing ASYNC reqeusts */
static L7_uint32 traceRouteAsyncTaskId;

/* Table containing traceroute sessions */
traceRouteTable_t   *traceRouteTable_g;

/* Lock to control access to traceRouteTable_g*/
osapiRWLock_t        traceRouteLock_g;


/*********************************************************************
*
* @purpose  Phase-1 Init of traceroute application
*
* @param    None
*
* @returns  Void
*
* @notes    Allocates memory for traceroute table
*
* @end
*********************************************************************/
L7_RC_t
traceRouteCnfgrInitPhase1Process( void )
{
  traceRouteTable_g = osapiMalloc( L7_SIM_COMPONENT_ID,
            TRACEROUTE_MAX_SESSIONS * sizeof( *traceRouteTable_g) );
    if ( traceRouteTable_g == NULL )
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "%s %d: %s : osapiMalloc failed\n",
                __FILE__, __LINE__, __FUNCTION__);
        return L7_FAILURE;
    }

    if ( osapiRWLockCreate( &traceRouteLock_g, OSAPI_RWLOCK_Q_FIFO )
            != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Failure creating RWLock for traceRouteTable_g." );
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Phase-3 Init of traceroute application
*
* @param    None
*
* @returns  Void
*
* @notes    Initializes the session table
*
* @end
*********************************************************************/
L7_RC_t
traceRouteCnfgrInitPhase3Process( void )
{

    L7_ushort16 i;
    traceRouteTable_t *traceEntry;

    /* Initialize the traceroute session entries */
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &traceEntry );
        memset( traceEntry, 0, sizeof ( traceRouteTable_t ) );
        traceEntry->state = TRACEROUTE_STATE_FREE;
        traceEntry->handle = i;
    }

    traceRouteCount = 0;
    traceRouteAsyncTaskId = 0;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Phase-2 De-Init of traceroute application
*
* @param    None
*
* @returns  Void
*
* @notes    Stops all active sessions
*
* @end
*********************************************************************/
L7_RC_t
traceRouteCnfgrUconfigPhase2Process( void )
{
    L7_ushort16 i;
    traceRouteTable_t *traceEntry;

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    /* Initialize the traceroute session entries */
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &traceEntry );
        /* Close the connection forcefully if its still open */
        traceRouteDone( traceEntry, TRACEROUTE_STATE_DONE );

        /* Free the entry */
        traceEntry->state = TRACEROUTE_STATE_FREE;
    }

    /* Kill the ASYNC task if it is still active */
    if ( traceRouteAsyncTaskId != 0 )
    {
        osapiTaskDelete( traceRouteAsyncTaskId );
        traceRouteAsyncTaskId = 0;
    }

    osapiWriteLockGive( traceRouteLock_g );

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Execute traceroute for SYNC mode
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
*
* @returns  L7_SUCCESS
*
* @notes    Sends the probes according to configuration in the entry.
*           Also, listens for responses parallely and accounts the response.
*
* @end
*********************************************************************/
L7_RC_t
traceRouteSync( traceRouteTable_t *traceEntry )
{
    fd_set rFds;
    fd_set eFds;
    L7_clocktime startTime;
    L7_clocktime endTime;
    L7_uint32 elapsedTime;
    L7_uint32 timeoutSecs;
    L7_uint32 timeoutUSecs;
    L7_int32  error = 0;

    /*
     * Wait for probeInterval time, and then send next probe.
     * In the mean time, handle any probe response pdus also
     */
    while ( 1 )
    {
        osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
        /* Set Fds to poll */
        FD_ZERO( &rFds );
        FD_ZERO( &eFds );
        FD_SET( traceEntry->rxSock, &rFds );
        FD_SET( traceEntry->rxSock, &eFds );

        /* Wait for probeIntervalResidue time */
        timeoutSecs = 0;
        timeoutUSecs = traceEntry->probeIntervalResidue;
        osapiReadLockGive( traceRouteLock_g );

        /* Get current time. Used to find time spent in select call. */
        osapiUTCTimeGet( &startTime );
        /* osapiSelect fix. osapiSelect passes NULL in timeout if both
         * timeoutSecs and timeoutUSecs is 0. in such case, select blocks
         * infinitely. Set timeoutUSecs to 1 if both secs and usecs is 0 */
        if ( timeoutSecs == 0 && timeoutUSecs == 0 )
        {
            timeoutUSecs = 1;
        }

        /* txSock not added for select. Assuming txSock is always ready */
        if ((error = osapiSelect( traceEntry->rxSock + 1, &rFds, NULL, &eFds,
                timeoutSecs, timeoutUSecs )) < 0 )
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "osapiSelect Returns error %d in %s \n", error,  __FUNCTION__);
          osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
          traceRouteDone( traceEntry, TRACEROUTE_STATE_DONE );
          osapiWriteLockGive( traceRouteLock_g );
        }
        /* Get time after select returns and compute elapsed time */
        osapiUTCTimeGet( &endTime );
        traceRouteTimeUSecElapsed( &endTime, &startTime, &elapsedTime );

        osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
        /* Process any PDU received or any error */
        if ( FD_ISSET( traceEntry->rxSock, &rFds ) )
        {
            traceRouteRxPdu( traceEntry );
        }

        if ( FD_ISSET( traceEntry->rxSock, &eFds ) )
        {
            traceRouteRxErr( traceEntry );
        }

        if ( traceEntry->state >= TRACEROUTE_STATE_DONE )
        {
            osapiWriteLockGive( traceRouteLock_g );
            break;
        }

        /* Process the elapsed time */
        if (traceRouteProcessTimer( elapsedTime, traceEntry ) != L7_SUCCESS)
        {
          traceRouteDone( traceEntry, TRACEROUTE_STATE_DONE );
          osapiWriteLockGive( traceRouteLock_g );
          break;
        }

        /* Check if end of processing. The state is set to DONE at the end */
        if ( traceEntry->state >= TRACEROUTE_STATE_DONE )
        {
            osapiWriteLockGive( traceRouteLock_g );
            break;
        }
        osapiWriteLockGive( traceRouteLock_g );
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process expiry of elapsedTime micro-seconds
*
* @param    L7_uint32 elapsedTime @b{(input)} Elapsed time in micro-seconds
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteProcessTimer( L7_uint32 elapsedTime, traceRouteTable_t *traceEntry )
{
    /* Check if probeInterval expired */
    if ( elapsedTime >= traceEntry->probeIntervalResidue )
    {

        /* If we are waiting for any response, treat request as timedout */
        if (traceEntry->txTime.seconds!=0 || traceEntry->txTime.nanoseconds!=0)
        {
            traceRouteProbeEnd( traceEntry, L7_TRUE, 0, 0 );
        }

        /* Return if its already end of session */
        if ( traceEntry->state > TRACEROUTE_STATE_IN_PROGRESS )
        {
            return L7_FAILURE;
        }

        /* Ignore any error in sending packet. It will be treated as timeout */
        if (traceRouteSendProbe( traceEntry ) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }

    }
    else
    {
        /* Update probeIntervalResidue */
        traceEntry->probeIntervalResidue -= elapsedTime;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Updates traceroute session table at the end of traceroute session
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
*
* @returns  L7_SUCCESS
*
* @notes    Changes the state of traceroute entry to DONE
*           Sets the state of probes that have not received to TIMEOUT.
*           If ASYNC mode, updates number active sessions in traceRouteCount
*
* @end
*********************************************************************/
L7_RC_t
traceRouteDone( traceRouteTable_t *traceEntry, traceRouteState_t state )
{
    if ( traceEntry->state != TRACEROUTE_STATE_IN_PROGRESS )
    {
        return L7_FAILURE;
    }

    /* Decrement the Session Count, if mode is async */
    if ( traceEntry->mode == TRACEROUTE_MODE_ASYNC )
    {
        traceRouteCount--;
    }

    traceEntry->state = state;

    /* Close the socket */
    osapiSocketClose( traceEntry->txSock );
    osapiSocketClose( traceEntry->rxSock );

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handles a new ASYNC traceroute request.
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
*
* @returns  L7_SUCCESS Session was started succesfuly.
* @returns  L7_FAILURE If starting the session fails.
*
* @notes    If this is first ASYNC request, then spaws a new task to handle
*           ASYNC requests. Increments number of simultaneous ASYNC traceroute
*           request in traceRouteCount
* @end
*********************************************************************/
L7_RC_t
traceRouteAsync( traceRouteTable_t *traceEntry )
{
    void *rxArgs[1];

    traceRouteCount++;
    /* Spawn a new task if this is the first session */
    if ( traceRouteCount == 1 )
    {
        rxArgs[0] = 0;
        if ((traceRouteAsyncTaskId =  osapiTaskCreate( "traceRouteAsync",
                        traceRouteASyncTask, 1, &rxArgs, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
        {
            L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Could not spawn traceRouteAsync task\n" );
            return L7_FAILURE;
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process ASYNC traceroute requests.
*
* @param    None
*
* @returns  NULL
*
* @notes    Scans through all the ASYNC requests present in traceRouteTable
*           and processes them.
*
* @end
*********************************************************************/
static void *
traceRouteASyncTask( void *param )
{
    fd_set rFds;
    fd_set eFds;
    L7_clocktime startTime;
    L7_clocktime endTime;
    L7_uint32 timeoutSecs;
    L7_uint32 timeoutUSecs;
    L7_ushort16 i;
    L7_uint32 elapsedTime;
    L7_ushort16 maxFd;
    traceRouteTable_t *traceEntry;
    L7_uint32 tmpTaskId;
    L7_int32 error = 0;

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    /* Be in the loop until all the ASYNC traceroute sessions are done */
    while ( 1 )
    {

        /* Exit out of loop if there are no more sessions */
        if ( traceRouteCount == 0 )
        {
            break;
        }

        /* Scan through the table and add all the sockets to poll */
        traceRouteASyncSetFd( &maxFd, &rFds, &eFds );

        /* Compute the time to wait in select */
        timeoutSecs = 0;
        timeoutUSecs = traceRouteASyncGetTimeout( );
        osapiWriteLockGive( traceRouteLock_g );

        /*
         * In case select returns error, we cannot rely on timestamp
         * being updated (as per Linux select documentation). Hence need to
         * compute elapsed time in select call ourselves.
         * Store the current time first
         */
        osapiUTCTimeGet( &startTime );
        /* osapiSelect fix. osapiSelect passes NULL in timeout if both
         * timeoutSecs and timeoutUSecs is 0. in such case, select blocks
         * infinitely. Set timeoutUSecs to 1 if both secs and usecs is 0 */
        if ( timeoutSecs == 0 && timeoutUSecs == 0 )
        {
            timeoutUSecs = 1;
        }

        if ((error = osapiSelect( maxFd + 1, &rFds, NULL, &eFds, timeoutSecs, timeoutUSecs )) < 0)
        {
          FD_ZERO( &rFds );
          FD_ZERO( &eFds );
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "osapiSelect Returns error %d in %s \n", error,  __FUNCTION__);
        }
        /* Get time after select returns and compute elapsed time */
        osapiUTCTimeGet( &endTime );
        traceRouteTimeUSecElapsed( &endTime, &startTime, &elapsedTime );

        osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

        /*
         * Loop through the session table and perform following for each session
         * 1. Process any PDU recieved on the socket
         * 2. Process any Exception on the socket
         * 3. Send a probe if probeInterval is elapsed
         */
        for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
        {
            traceRouteGetEntry( i, &traceEntry );

            /* Ignore if entry is not of type ASYNC */
            if ( traceEntry->mode != TRACEROUTE_MODE_ASYNC )
            {
                continue;
            }

            /* Ignore the entry if session is not active */
            if ( traceEntry->state != TRACEROUTE_STATE_IN_PROGRESS )
            {
                continue;
            }

            /* Process any PDU received or any error */
            if ( FD_ISSET( traceEntry->rxSock, &rFds ) )
            {
                traceRouteRxPdu( traceEntry );
            }

            if ( FD_ISSET( traceEntry->rxSock, &eFds ) )
            {
                traceRouteRxErr( traceEntry );
            }

            /* Skip timer processing if Rx processing ends processing */
            if ( traceEntry->state != TRACEROUTE_STATE_IN_PROGRESS )
            {
                continue;
            }
            traceRouteProcessTimer( elapsedTime, traceEntry );
        }
    }

    /* If control comes out of loop, it means all traceroute sessions are
     * complete. Destroy the thread */
    if ( traceRouteAsyncTaskId != 0 )
    {
        /* Need to free the lock before deleting the task. */
        tmpTaskId = traceRouteAsyncTaskId;
        traceRouteAsyncTaskId = 0;
        osapiWriteLockGive( traceRouteLock_g );
        osapiTaskDelete( tmpTaskId );
    }
    else
    {
        osapiWriteLockGive( traceRouteLock_g );
    }

    return NULL;
}

/*********************************************************************
* @purpose  Create a socket for traceroute session
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to traceroute
*               session
*
* @returns  L7_SUCCESS If socket could be created
* @returns  L7_FAILURE If socket could not be created.
*
* @notes    Creates a socket to be used for ICMP PDUs and associates it
*           to the VRF.
*
* @end
*********************************************************************/
L7_RC_t
traceRouteCreateSock( traceRouteTable_t *traceEntry )
{
    /* Create a socket of type ICMP */
    if ( osapiSocketCreate( L7_AF_INET, L7_SOCK_RAW, IPPROTO_ICMP,
                &(traceEntry->rxSock)) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Error opening ICMP RAW socket. Error = %d \n",
                (L7_int32)traceEntry->rxSock );
        return L7_FAILURE;
    }

    if ( osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &(traceEntry->txSock))
            == L7_FAILURE )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Error opening UDP socket. Error = %d \n",
                (L7_int32)traceEntry->txSock );
        osapiSocketClose( traceEntry->rxSock );
        return L7_FAILURE;
    }
#ifdef _L7_OS_VXWORKS_
    if(traceEntry->probeSize > 9972)
    {
      L7_uint32 sndBuffSize = 65535;
      if (osapiSetsockopt(traceEntry->txSock, L7_SOL_SOCKET, L7_SO_SNDBUF,(L7_char8 *)
                      &sndBuffSize, sizeof(sndBuffSize))== L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                      "%s: setsockopt FAILED\n", __FUNCTION__);
        return L7_FAILURE;
      }
    }
#endif

#if 0
    /* Set the VRF for the sockets*/
    if ( osapiIpStackSocketVrAttach( traceEntry->txSock, traceEntry->vrfId )
            != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Error attaching tx socket to vr = %d \n", traceEntry->vrfId );
        osapiSocketClose( traceEntry->txSock );
        osapiSocketClose( traceEntry->rxSock );
        return L7_FAILURE;
    }

    if ( osapiIpStackSocketVrAttach( traceEntry->rxSock, traceEntry->vrfId )
            != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Error attaching rx socket to vr = %d \n", traceEntry->vrfId );
        osapiSocketClose( traceEntry->txSock );
        osapiSocketClose( traceEntry->rxSock );
        return L7_FAILURE;
    }
#endif

    /* Set the DontFrag flag if configured */
    if ( traceEntry->dontFrag == L7_TRUE )
    {
        if ( osapiSetsockopt(traceEntry->txSock, IPPROTO_IP, L7_IP_DONTFRAG,
                (L7_uchar8 *)&traceEntry->dontFrag,
                sizeof(traceEntry->dontFrag)) != L7_SUCCESS )
        {
            /* Ignore the error its not really fatal. Our traceroute does not
             * depend of dont fragment function */
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                    "Error setting DontFrag flag - ignoring \n" );
        }
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set sockets on which select must be done.
*
* @param    L7_ushort16 *maxFd @b{(output)} max FD in select.
* @param    fd_set *rFds @b{(output)} Set of read FDs
* @param    fd_set *eFds @b{(output)} Set of exception FDs
*
* @returns  L7_SUCCESS If atleast one socket is added to fd_set
* @returns  L7_FAILURE If no socket is added to fd_set.
*
* @notes    Scans traceRouteTable and sets bits in fd_set for socket of
*           ASYNC session.
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteASyncSetFd( L7_ushort16 *maxFd, fd_set *rFds, fd_set *eFds )
{
    traceRouteTable_t *traceEntry;
    L7_ushort16 i;
    L7_ushort16 flag = 0;

    FD_ZERO( rFds );
    FD_ZERO( eFds );

    /* Scan through the table, and add socket for requests of type ASYNC */
    *maxFd = 0;
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &traceEntry );

        if ( traceEntry->mode == TRACEROUTE_MODE_ASYNC &&
            traceEntry->state == TRACEROUTE_STATE_IN_PROGRESS )
        {

            /* update maxFd if necessary */
            if ( *maxFd < traceEntry->rxSock )
            {
                *maxFd = traceEntry->rxSock;
            }

            /* Set socket in fd_set */
            FD_SET( traceEntry->rxSock, rFds );
            FD_SET( traceEntry->rxSock, eFds );
            /* Set a flag to say atleast one FD is set */
            flag++;
        }
    }

    if ( flag == 0 )
    {
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Computes timeout to wait for in select call for ASYNC sessions.
*
* @param    None
*
* @returns  Time to wait in usec in select call.
*
* @notes    Scans traceRouteTable and finds the minimum residue time.
*           Returns residue as time to wait.
*
* @end
*********************************************************************/
static L7_uint32
traceRouteASyncGetTimeout( )
{
    L7_ushort16 i;
    L7_uint32 residue = 0xFFFFFFFF;
    traceRouteTable_t *traceEntry;

    /* Mode is ASYNC. Timeout is min of residue in all the ASYNC sessions */
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &traceEntry );
        if ( traceEntry->state != TRACEROUTE_STATE_IN_PROGRESS )
        {
            continue;
        }

        if ( traceEntry->mode == TRACEROUTE_MODE_SYNC )
        {
            continue;
        }

        if ( traceEntry->probeIntervalResidue < residue )
        {
            residue = traceEntry->probeIntervalResidue;
        }
    }

    /* If a new traceroute request arrives when thread is waiting in select,
     * then the traceroute will not be executed until select returns. To
     * ensure, the new traceroute is not delayed very long, put a limit on
     * the max time select can wait for.
     * TODO: This might not be a good idea, since the task will wake after time
     * given below even if there are no new requests. Ideally, we want to
     * interrupt the select when a new traceroute request arrives. Some of the
     * alternatives,
     * a. Send a dummy packet on any of the sockets in select call.
     * b. On Linux send a dummy signal to the thread and select will come out.
     */
    if ( residue > TRACEROUTE_MAX_WAIT_TIME )
    {
        residue = TRACEROUTE_MAX_WAIT_TIME;
    }

    return residue;
}

/*********************************************************************
* @purpose  To send the udp probe packet to the destination-ip
*
* @param    traceRouteTable_t *traceEntry  Entry in traceRoute Session Table
*
* @returns  L7_SUCCESS if probe transmitted successfully
*           L7_FAILURE if sendto fails
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteSendProbe( traceRouteTable_t *traceEntry )
{
    L7_uint32 ttl = traceEntry->currTtl;
    L7_int32 charsSent;
    L7_uchar8 *buffer;
    L7_uchar8 *ptr;           /* next byte to write in probe */
    L7_ushort16 probeVal;     /* a value to write into the probe. network byte order. */
    traceRouteHops_t *hopEntry;
    L7_sockaddr_in_t to;
    L7_uint32 i;
    L7_uint32 bufferLen = 0;

    buffer = osapiMalloc( L7_SIM_COMPONENT_ID,
                    TRACEROUTE_MAX_BUFF* sizeof( *buffer) );
   if (buffer == NULL)
   {
     L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "%s %d: %s : osapiMalloc failed\n",
         __FILE__, __LINE__, __FUNCTION__);
     return L7_FAILURE;
   }
    hopEntry = &traceEntry->hopInfo[
        (traceEntry->hopCount % TRACEROUTE_MAX_HOP_INFO)];

    /* Set the TTL before sending the probe packet */
    if (osapiSetsockopt(traceEntry->txSock, IPPROTO_IP, L7_IP_TTL,
            (L7_uchar8 *)&ttl, sizeof(ttl)) == L7_FAILURE)
    {
       L7_uint32 eno = osapiErrnoGet();
       L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                  "Setting IP_TTL socket option to %u failed with error %s (%u).",
                  ttl, strerror(eno), eno);
       return L7_FAILURE;
    }

    /* Add the Traceroute signature into packet */
    if (traceEntry->probeSize >= (strlen(TRACEROUTE_SIGNATURE)+1+(2*sizeof(L7_ushort16))))
    {
      strcpy( buffer, TRACEROUTE_SIGNATURE );
      bufferLen = strlen( TRACEROUTE_SIGNATURE ) + 1;
      ptr = buffer + bufferLen;
      /*
       * Store testAttempt and handle number in PDU. It will be used in
       * matching response with requests
       */
      probeVal = osapiHtons(traceEntry->testAttempts + 1);
      memcpy(ptr, &probeVal, sizeof(L7_ushort16));
      bufferLen += sizeof( L7_ushort16 );
      ptr += sizeof( L7_ushort16 );

      probeVal = osapiHtons(traceEntry->handle);
      memcpy(ptr, &probeVal, sizeof(L7_ushort16));
      bufferLen += sizeof( L7_ushort16 );
    }

    for ( i = bufferLen; i < (traceEntry->probeSize) ; i++ )
    {
        buffer[i] = (L7_uchar8)i;
        bufferLen++;
    }


    /* Send the probe packet */
    to.sin_family = L7_AF_INET;
    to.sin_addr.s_addr = osapiHtonl( traceEntry->ipDa );
    to.sin_port = osapiHtons( traceEntry->port );

    traceEntry->currProbeCount++;
    traceEntry->testAttempts++;
    hopEntry->probeSent++;
    osapiUTCTimeGet( &traceEntry->txTime );

    /* Reset probeIntervalResidue */
    traceEntry->probeIntervalResidue = traceEntry->probeInterval;

    /* Send packet. Any error in sendto will be treated as timeout errors */
    if (osapiSocketSendto(traceEntry->txSock, buffer, bufferLen, 0,
                (L7_sockaddr_t *)&to, sizeof(to), &charsSent ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Failure in osapiSocketSendTo\n" );
        osapiFree(L7_SIM_COMPONENT_ID, buffer);
        return L7_FAILURE;
    }
    osapiFree(L7_SIM_COMPONENT_ID, buffer);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handles an ICMP message received.
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to traceroute session
*
* @returns  L7_SUCCESS If the PDU was processed
* @returns  L7_FAILURE If probe was ignored
*
* @notes    Handles only ICMP REPLIES.
*           Lower 4 bits of icmp_id identifies the entry in traceRouteTable.
*           sequnce-num % TRACEROUTE_MAX_HOP_INFO identifies the entry in resp
*           info table.
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteRxPdu( traceRouteTable_t *traceEntry )
{
    L7_uint32           icmpType;    /* Type of icmp reply received */
    traceRouteHops_t    *hopEntry;
    L7_clocktime        recvReplyTimeTicks;
    L7_ipHeader_t       *icmpPacket;     /* starting with IPv4 header */
    L7_uint32           rtt;
    L7_uint32           hopDa;

    icmpPacket = (L7_ipHeader_t*) osapiMalloc(L7_SIM_COMPONENT_ID, TRACEROUTE_MAX_BUFF);
   if (icmpPacket == NULL)
   {
     L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "%s %d: %s : osapiMalloc failed\n",
         __FILE__, __LINE__, __FUNCTION__);
     return L7_FAILURE;
   }

    hopEntry = &traceEntry->hopInfo[
        traceEntry->hopCount % TRACEROUTE_MAX_HOP_INFO];
    /* Wait for the ICMP packet from destination
     * This function will also update the IP-DA for the hop. Note, we only
     * have one IP-DA in the hopInfo structure. As a result, the IP-DA will be
     * overwritten if response for a particular hop is received from multiple
     * destinations. This is still okay for us since we dont maintain any
     * probe specific information for each  hop.
     */
    if ( waitForICMPReply( traceEntry, traceEntry->rxSock, &hopDa,
                icmpPacket, TRACEROUTE_MAX_BUFF, traceEntry->port, &icmpType)
            == L7_FAILURE )
    {
      osapiFree(L7_SIM_COMPONENT_ID, icmpPacket);
      return L7_FAILURE;
    }

    /* Ignore if we werent expecting any PDU */
    if (traceEntry->txTime.seconds == 0 && traceEntry->txTime.nanoseconds == 0)
    {
      osapiFree(L7_SIM_COMPONENT_ID, icmpPacket);
        return L7_FAILURE;
    }

    /* Compute RTT */
    osapiUTCTimeGet( &recvReplyTimeTicks );
    traceRouteTimeUSecElapsed( &recvReplyTimeTicks, &traceEntry->txTime, &rtt );

    /* Reset txTime to say we are not expecting any response */
    traceEntry->txTime.seconds = 0;
    traceEntry->txTime.nanoseconds = 0;
    hopEntry->hopDa = hopDa;

    if ( hopEntry->minRtt > rtt )
    {
        hopEntry->minRtt = rtt;
    }

    if ( hopEntry->maxRtt < rtt )
    {
        hopEntry->maxRtt = rtt;
    }

    /* Update Hop specific fields */
    hopEntry->avgRtt += rtt;
    hopEntry->probeResponses++;

    traceRouteProbeEnd( traceEntry, L7_FALSE, (icmpType - 1), rtt );

    osapiFree(L7_SIM_COMPONENT_ID, icmpPacket);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose Check if an address matches a locally configured address
*
* @param  ipAddr         ip address to test
*
* @returns  L7_TRUE      if the address matches a local interface
* @returns  L7_FALSE     if not
*
* @notes
*          Checks against the service port, network port and routing
*          interfaces (for a routing build).
*
*           TODO: This piece of code was persent in earlier impemenetation,
*           but does not seem to be necessary.
* @end
*********************************************************************/
#if 0
static L7_BOOL
isLocalAddr(L7_IP_ADDR_t ipAddr)
{
  L7_uint32 ipSvrAddr;

#ifdef L7_ROUTING_PACKAGE
  L7_uint32 mask;
  L7_uint32 tempIntIfNum;
  L7_uint32 i;
#endif

  /*
  ** Check if the specified ip address & subnet mask conflicts with the network
  ** or service port configuration
  */
  ipSvrAddr = simGetSystemIPAddr();

  if(ipAddr == ipSvrAddr)
  {
    return L7_TRUE;
  }

  ipSvrAddr = simGetServPortIPAddr();
  if(ipAddr == ipSvrAddr)
  {
    return L7_TRUE;
  }

#ifdef L7_ROUTING_PACKAGE

  /*
  ** Iterate through all existing interfaces to determine if this ip is
  ** already configured on another interface
  */
  for (i = 1; i <=  L7_RTR_MAX_RTR_INTERFACES; i++)
  {
    if (ipMapRtrIntfToIntIfNum(i, &tempIntIfNum) == L7_SUCCESS)
    {
      if ((ipMapIpIntfExists(tempIntIfNum) == L7_TRUE) &&
          (ipMapRtrIntfCfgIpAddressGet(tempIntIfNum, &ipSvrAddr, &mask)
               == L7_SUCCESS))
        {
          if(ipAddr == ipSvrAddr)
            return L7_TRUE;
        }
    }
  }
#endif

  return L7_FALSE;
  return L7_FALSE;
}
#endif /* 0 */

/*********************************************************************
* @purpose  Read ICMP PDU from the rx socket
*
* @param  traceRouteTable_t *traceEntry @b{(input)} Entry for the session
* @param  L7_uint32 icmpSockFd @b{(input)} Socket for receiving the PDU
* @param  L7_uint32 *ipAddr    @b{(output)} IP Destination sending the PDU
* @param  L7_uchar8 *icmpPacket @b{(input)} Input PDU, starting with IPv4 header
* @param  L7_uint32 icmpPacketLen  @b{(input)} Length of PDU reeived
* @param  L7_uint32 port           @b{(input)} Port for which PDU was sent
* @param  L7_uint32 *icmpType      @b{(output)} ICMP Type in the PDU received
*
* @returns  L7_SUCCESS If a valid ICMP PDU is received
* @returns  L7_FAILURE If no ICMP PDU was received
*
* @notes
*
* @end
*********************************************************************/
static L7_uint32
waitForICMPReply(traceRouteTable_t *traceEntry, L7_uint32 icmpSockfd,
        L7_uint32 *ipAddr, L7_ipHeader_t *icmpPacket, L7_uint32 icmpPacketLen,
        L7_uint32 port, L7_uint32 *icmpType)
{
    L7_uint32           bytesReceived = 0;
    struct icmp         *icp;
    L7_uchar8           type;
    L7_uchar8           code;
    L7_uint32           hlen;        /* header length. number of 32-bit words */
    L7_ipHeader_t       *ip;
    L7_ipHeader_t       *hip;
    L7_udp_header_t     *up;
    L7_sockaddr_in_t    remAddr;
    L7_uint32           remAddrLen;
    L7_char8            *udpPayload;     /* first byte of UDP payload */
    L7_uchar8           *pos;
    L7_ushort16         testAttempts;    /* network byte order */
    L7_ushort16         handle;          /* test entry handle. net byte order */

    remAddrLen = sizeof( remAddr );
    if ( osapiSocketRecvfrom(icmpSockfd, (L7_uchar8*) icmpPacket, icmpPacketLen, 0,
                             (L7_sockaddr_t*) &remAddr, &remAddrLen,
                             &bytesReceived) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Failure in osapiSocketRecvfrom\n" );
        return L7_FAILURE;
    }

    *ipAddr = (L7_uint32) osapiNtohl(remAddr.sin_addr.s_addr);
    ip = icmpPacket;

    /* Get the header length from the received packet */
    hlen = ip->iph_versLen & 0x0f;

    /* Check if packet recieved contains ICMP header */
    if (bytesReceived <= 4 * hlen + sizeof(struct icmp))
    {
        return L7_FAILURE;
    }

    bytesReceived -= (4 * hlen);
    icp = (struct icmp *)(((L7_uint32*) icmpPacket) + hlen);

    type = icp->icmp_type;
    code = icp->icmp_code;
    if ( ( type == ICMP_TIMXCEED && code == ICMP_TIMXCEED_INTRANS ) ||
                                          type == ICMP_UNREACH )
    {
        hip = (L7_ipHeader_t *)&icp->icmp_ip;

        /* Get the header length from the received packet */
        hlen = hip->iph_versLen & 0x0f;

        /* Check if packet contains original IP and UDP Headers */
        if (bytesReceived <= (4 * hlen) + sizeof(L7_udp_header_t))
        {
            return L7_FAILURE;
        }

        bytesReceived -= ((4 * hlen) + sizeof(L7_udp_header_t)+ ICMP_MINLEN);
        up = (L7_udp_header_t *)(((L7_uint32 *) hip) + hlen);

        /* Destination port in UDP header must be same as port in session */
        if ( osapiNtohs(up->destPort) != traceEntry->port )
        {
            return L7_FAILURE;
        }

        /* TODO: Should we check that UDP has our port as source port? */

        /* Some checks to ensure its response to our probe request */

        /* Our Traceroute packet had following as UDP Data
         * TRACEROUTE_SIGNATURE followed by testAttempt and handle.
         * See, if we have all of them
         */

        udpPayload = (L7_char8 *)((L7_uchar8 *)up + sizeof( L7_udp_header_t) );
        /* Some routers might not have added enough data in ICMP response.
           In such case, dont verify signature and hope for the best */
        if ( (osapiNtohs(up->length) >=
              (strlen(TRACEROUTE_SIGNATURE) + 2 * sizeof( L7_ushort16)+1+ sizeof( L7_udp_header_t)) ) &&
             (bytesReceived >= (strlen(TRACEROUTE_SIGNATURE) + 2 * sizeof( L7_ushort16)+1)))
        {
            /* There is enough data to verify signature */

            /* Verify the signature string */
            if (strcmp( udpPayload, TRACEROUTE_SIGNATURE ) != L7_SUCCESS)
            {
                return L7_FAILURE;
            }
            pos = udpPayload + strlen( TRACEROUTE_SIGNATURE ) + 1;

            /* See if testAttempts matches. Can't guarantee alignment, so use memcpy. */
            memcpy(&testAttempts, pos, 2);
            if (osapiNtohs(testAttempts) != traceEntry->testAttempts)
            {
                return L7_FAILURE;
            }

            pos += 2;
            /* See if handle matches. */
            memcpy(&handle, pos, 2);
            if (osapiNtohs(handle) != traceEntry->handle)
            {
                return L7_FAILURE;
            }
        }

        if (hip->iph_prot == IP_PROT_UDP && up->destPort == osapiHtons(port))
        {
            /* TODO: Following code was present in earlier implementation, but
             * does not seem to be necessary any more.
             */
#if 0
            /* Weed out unreach packets from local interfaces.
             * These get sent on linux boxes
             */
            if(type == ICMP_UNREACH && code == ICMP_UNREACH_HOST &&
                     L7_TRUE == isLocalAddr(*ipAddr))
            {
                return L7_FAILURE;
            }
#endif
            *icmpType=(type == ICMP_TIMXCEED? -1 : code+1);
            return L7_SUCCESS;
        }
        else
        {
            return L7_FAILURE;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Handle exception on ICMP sockets.
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
*
* @returns  L7_SUCCESS
*
* @notes    Ends the session
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteRxErr( traceRouteTable_t *traceEntry )
{
    traceRouteDone( traceEntry, TRACEROUTE_STATE_INTERNAL_ERR );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Moves traceroute to use next hop
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
*
* @returns  L7_SUCCESS If we can send probe for next hop
* @returns  L7_FAILURE If no more probes must be sent and its end of traceroute
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteNextHop( traceRouteTable_t *traceEntry )
{
    L7_ushort16 index;
    traceRouteHops_t *hopEntry;

    /* See if it is already decided to stop the processing */
    if ( traceEntry->state != TRACEROUTE_STATE_IN_PROGRESS )
    {
        return L7_FAILURE;
    }

    /* Check if we exceeded maxTTL */
    if ( traceEntry->currTtl >= traceEntry->maxTtl )
    {
        traceRouteDone( traceEntry, TRACEROUTE_STATE_EXCEED_TTL );
        return L7_FAILURE;
    }

    /* stop if the previous HOP processing identifies it as end */
    index = traceEntry->hopCount % TRACEROUTE_MAX_HOP_INFO;
    hopEntry = &traceEntry->hopInfo[ index ];
    if ( hopEntry->state != TRACEROUTE_STATE_FREE )
    {
        traceRouteDone( traceEntry, hopEntry->state );
        return L7_FAILURE;
    }

    /* Move to next hop */
    traceEntry->hopCount++;
    index = traceEntry->hopCount % TRACEROUTE_MAX_HOP_INFO;
    memset( &traceEntry->hopInfo[ index ], 0, sizeof( traceRouteHops_t ) );
    traceEntry->hopInfo[index].minRtt = TRACEROUTE_INVALID_RTT;
    traceEntry->currProbeCount = 0;
    traceEntry->currTtl++;
    traceEntry->hopInfo[index].ttl = traceEntry->currTtl;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Processes end of a probe for a particular hop
*
* @param    traceRouteTable_t *traceEntry @b{(input)} Pointer to session
* @param    L7_BOOL *timeOut @b{(input)} Flag to specify if probe timed out
* @param    L7_uint32 icmpCode @b{(input)} ICMP Code if probe did not time out
* @param    L7_uint32 rtt @b{(input)} RTT for the probe
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t
traceRouteProbeEnd( traceRouteTable_t *traceEntry, L7_BOOL timeOut,
        L7_ushort16 icmpCode, L7_uint32 rtt )
{
    L7_BOOL fail = L7_TRUE;
    traceRouteHops_t  *hopEntry;
    L7_uchar8         errorSymbol = ' ';

    hopEntry = &traceEntry->hopInfo[
        traceEntry->hopCount % TRACEROUTE_MAX_HOP_INFO ];

    if ( timeOut == L7_TRUE )
    {
        errorSymbol = '*';
    }
    else
    {
        switch (icmpCode )
        {
        case ICMP_UNREACH_PROTOCOL:
            hopEntry->state = TRACEROUTE_STATE_DONE;
            errorSymbol = 'P';
            fail = L7_FALSE;
            break;
        case ICMP_UNREACH_PORT:
            hopEntry->state = TRACEROUTE_STATE_DONE;
            fail = L7_FALSE;
            break;
        case ICMP_UNREACH_NET:
            errorSymbol = 'N';
            hopEntry->state = TRACEROUTE_STATE_UNREACHABLE;
            break;
        case ICMP_UNREACH_HOST:
            errorSymbol = 'H';
            hopEntry->state = TRACEROUTE_STATE_UNREACHABLE;
            break;
        case ICMP_UNREACH_NEEDFRAG:
            errorSymbol = 'F';
            hopEntry->state = TRACEROUTE_STATE_FRAG_ERR;
            break;
        case ICMP_UNREACH_SRCFAIL:
            errorSymbol = 'S';
            hopEntry->state = TRACEROUTE_STATE_UNREACHABLE;
            break;
        case ICMP_UNREACH_NET_PROHIB:
            errorSymbol = 'A';
            hopEntry->state = TRACEROUTE_STATE_UNREACHABLE;
            break;
        case ICMP_UNREACH_HOST_PROHIB:
            errorSymbol = 'C';
            hopEntry->state = TRACEROUTE_STATE_UNREACHABLE;
            break;
        default:
            fail = L7_FALSE;
            /* do nothing */
            break;
        }
    }

    /* Now that response is found, send next probe immediately */
    traceEntry->probeIntervalResidue = 0;

    if ( traceEntry->callback != NULL )
    {
        traceEntry->callback( traceEntry->userParam, traceEntry->handle,
                traceEntry->currTtl, hopEntry->hopDa, traceEntry->hopCount,
                traceEntry->currProbeCount, rtt, errorSymbol );
    }

    /* Check for max consecutive failures */
    if ( fail == L7_TRUE )
    {
        traceEntry->consecutiveFail++;
        /* Stop if max consecutive failure reached */
        if ( traceEntry->consecutiveFail >= traceEntry->maxFail )
        {
            traceRouteDone( traceEntry, TRACEROUTE_STATE_EXCEED_FAILURES );
            return L7_FAILURE;
        }
    }
    else
    {
        /* Reset max consecutive failures */
        traceEntry->consecutiveFail = 0;
        traceEntry->testSuccess++;
    }

    /* Should we move to next hop? */
    if ( traceEntry->currProbeCount >= traceEntry->probePerHop )
    {
        traceRouteNextHop( traceEntry );
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns traceRouteTable entry for index specified
*
* @param    L7_uint16 index @b{(input)} Index for the entry
* @param    traceRouteTable_t **traceEntry @b{(output)}Pointer address is to
*                   be stored
*
* @returns  L7_SUCCESS
*
* @notes
*           The function is defined so that structure of table can be changed
*           in later if necessary
*
* @end
*********************************************************************/
void
traceRouteGetEntry( L7_ushort16 index, traceRouteTable_t **traceEntry )
{
    *traceEntry = &traceRouteTable_g[index];
}

/**********************************************************************
 * * @purpose  Get the difference between two times in usecs
 * *
 * * @param    end @b{(input)} Ending time
 * * @param    start @b{(input)} Starting time
 * * @param    elapsedTime @b{(output)} Difference in usec
 * *
 * * @returns  Elapsed time in uSecs
 * *
 * * @notes
 * *
 * * @end
 * *********************************************************************/
static L7_RC_t
traceRouteTimeUSecElapsed( L7_clocktime *end, L7_clocktime *start,
        L7_uint32 *elapsedTime )
{
      *elapsedTime = (end->seconds - start->seconds)
          * TRACEROUTE_INTERVAL_MULTIPLIER;
      if ( start->nanoseconds > end->nanoseconds )
      {
          *elapsedTime -= ((start->nanoseconds - end->nanoseconds)/1000);
      }
      else
      {
          *elapsedTime += ((end->nanoseconds - start->nanoseconds)/1000);
      }
      return L7_SUCCESS;
}
