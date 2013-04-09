/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename ping.c
*
* @purpose Provide "ping" functionality as defined in RFC 2925
*
* @component ping
*
* @comments
*       The Ping application will not support the MIB defined in RFC-2925
*       directly. It will however provide the API's and data structures needed
*       to support the MIB. The SNMP implementation can use the APIs to
*       provide the semantics as needed by the RFC. The implementation can
*       infact be used by both CLI and HTTP as well.
*
*       The implementsion is split across 2 files, ping.c and ping_api.c
*       ping_api.c : Provides implementation for interfaces to ping application
*       ping.c : Contains actual implementation of the ping session
*
* @create 08/16/2005
*
* @author Praveen K V
*
* @end
*
**********************************************************************/
#include "l7_socket.h"
#include "l7_icmp.h"
#include "osapi.h"
#include "osapi_support.h"
#include "string.h"
#include "log.h"
#include "ping_api.h"
#include "ping_private.h"
#include "nimapi.h"

/* Local function definitions */
static L7_RC_t pingRxErr( pingSessionTable_t *pingEntry );
static L7_RC_t pingRxPdu( pingSessionTable_t *pingEntry );
static L7_RC_t pingSendProbe( pingSessionTable_t *pingEntry );
static L7_RC_t pingASyncSetFd(L7_uint32 *maxFd, fd_set *rFds, fd_set *eFds);
static L7_uint32 pingASyncGetTimeout( void );
static void *pingASyncTask( void *param );
static L7_RC_t pingValidate( pingSessionTable_t *pingEntry, L7_uchar8 *pingBuf,
        L7_uint32 dataLen, L7_ushort16 *respIndex, L7_ushort16 *respType, L7_uchar8 *src_ipAddress );
static L7_RC_t pingTimeUSecElapsed( L7_clocktime *end, L7_clocktime *start,
        L7_uint32 *elapsed );
static L7_RC_t pingElapsedTime( pingSessionTable_t *pingEntry,
        L7_uint32 elapsedTime );
static L7_RC_t pingInetChksum( L7_uchar8 *buff, L7_ushort16 len );

/* Number of outstanding ping requests */
static L7_uint32 pingAsyncSessionCount;

/* Task-ID for the task handling ASYNC requests */
static L7_int32 pingAsyncTaskId;

/* Table containing ping sessions */
pingSessionTable_t   *pingSessionTable_g;

/* Lock to control access to pingSessionTable_g*/
osapiRWLock_t        pingSessionLock_g;

/* Variable to hold trace level */

/*********************************************************************
*
* @purpose  Initializes the ping application
*
* @param    None
*
* @returns  L7_SUCCESS, if success
*
* @notes    Initializes the Ping Session Table
*
* @end
*********************************************************************/
L7_RC_t
pingCnfgrInitPhase1Process( void )
{

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    pingSessionTable_g = osapiMalloc( L7_SIM_COMPONENT_ID,
            PING_MAX_SESSIONS * sizeof( *pingSessionTable_g) );
    if ( pingSessionTable_g == NULL )
    {
        L7_LOGF( L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "%s %d: %s : osapiMalloc failed\n",
                __FILE__, __LINE__, __FUNCTION__);
        return L7_FAILURE;
    }

    if ( osapiRWLockCreate( &pingSessionLock_g, OSAPI_RWLOCK_Q_FIFO )
            != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Failure creating RWLock for pingSessionTable." );
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Phase-3 initialization routing
*
* @param    None
*
* @returns  L7_SUCCESS, if success
*
* @notes    Initializes the state of table
* @end
*********************************************************************/
L7_RC_t
pingCnfgrInitPhase3Process( void )
{
    L7_ushort16 i;
    pingSessionTable_t *pingEntry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    /* Initialize the ping session entries */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );
        bzero( (void *)pingEntry, sizeof ( pingSessionTable_t ) );
        pingEntry->state = PING_SESSION_STATE_FREE;
        pingEntry->handle = i;
    }

    pingAsyncSessionCount = 0;
    osapiWriteLockGive( pingSessionLock_g );

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Phase2 Unconfig processing
*
* @param    None
*
* @returns  L7_SUCCESS, if success
*
* @notes    Closes all the session allocated
* @end
*********************************************************************/
L7_RC_t
pingCnfgrUconfigPhase2Process( void )
{

    L7_ushort16 i;
    pingSessionTable_t *pingEntry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );

    if ( pingAsyncTaskId != 0 )
    {
        osapiTaskDelete( pingAsyncTaskId );
        pingAsyncTaskId = 0;
    }

    /* Stops any active ping sessions */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );

        /* Close the connection forcefully if its still open. Closing socket
         * will ensure that even SYNC sessions will stop since select call
         * returns error */
        pingEnd( pingEntry, PING_SESSION_STATE_DONE );

        /* Free the entry */
        pingEntry->state = PING_SESSION_STATE_FREE;
    }

    osapiWriteLockGive( pingSessionLock_g );

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Execute ping for SYNC mode
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
*
* @returns  L7_SUCCESS
*
* @notes    Sends the probes according to configuration in the entry.
*           Also, listens for responses parallely and accounts the response.
* @end
*********************************************************************/
L7_RC_t
pingSessionSync( pingSessionTable_t *pingEntry )
{
    fd_set rFds;
    fd_set eFds;
    L7_clocktime startTime;
    L7_clocktime endTime;
    L7_uint32 elapsedTime;
    L7_long32 timeoutSecs;
    L7_long32 timeoutUSecs;
    L7_int32  error = 0;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    /*
     * Wait for probeInterval time, and then send next probe.
     * In the mean time, handle any probe response pdus also
     */
    while ( 1 )
    {

        osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
        /* Set Fds to poll */
        FD_ZERO( &rFds );
        FD_ZERO( &eFds );
        FD_SET( pingEntry->sock, &rFds );
        FD_SET( pingEntry->sock, &eFds );

        /* Wait for probeIntervalResidue time */
        timeoutSecs = 0;
        timeoutUSecs = pingEntry->probeIntervalResidue;
        osapiReadLockGive( pingSessionLock_g );

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
            timeoutUSecs = 1;

        if ((error = osapiSelect( pingEntry->sock + 1, &rFds, NULL, &eFds, timeoutSecs,
                timeoutUSecs )) < 0 )
        {
           L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "osapiSelect Returns error %d in %s \n", error,  __FUNCTION__);
           osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
           (void)pingEnd( pingEntry, PING_SESSION_STATE_DONE );
           osapiWriteLockGive( pingSessionLock_g );
           return L7_FAILURE;
        }
        /* Get time after select returns and compute elapsed time */
        osapiUTCTimeGet( &endTime );
        pingTimeUSecElapsed( &endTime, &startTime, &elapsedTime );

        osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
        /* Process any PDU received or any error */
        if ( FD_ISSET( pingEntry->sock, &rFds ) )
        {
            pingRxPdu( pingEntry );
        }

        if ( FD_ISSET( pingEntry->sock, &eFds ) )
        {
            pingRxErr( pingEntry );
        }

        /* Check if end of processing. Processing can end in two cases,
         * 1. We have received all probe responses.
         * 2. Last probe is sent and probeInterval has expired
         * In both cases, the state is changed to DONE
         */

        /* Break if Rx or Exception processing says its end of processing */
        if ( pingEntry->state != PING_SESSION_STATE_IN_PROGRESS )
        {
            osapiWriteLockGive( pingSessionLock_g );
            break;
        }

        pingElapsedTime( pingEntry, elapsedTime );

        /* Break-out in case timer processing finds its end of processing */
        if ( pingEntry->state != PING_SESSION_STATE_IN_PROGRESS )
        {
            osapiWriteLockGive( pingSessionLock_g );
            break;
        }

        osapiWriteLockGive( pingSessionLock_g );
    }

    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates the ping session table at the end of ping session
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
* @param    pingSessionState_t state      @b{(input)} New state of the session
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Sets the state of probe session to value given.
*           For probes that have not received response, sets state to timeout
*
* @end
*********************************************************************/
L7_RC_t
pingEnd(pingSessionTable_t *pingEntry, pingSessionState_t state)
{
    L7_ushort16 i;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    /* Return if session is not active. */
    if ( pingEntry->state != PING_SESSION_STATE_IN_PROGRESS )
    {
        return L7_FAILURE;
    }

    pingEntry->state = state;

    /* End of probe. See if response is not received for any of the probes
     * in info. Set state for such entries to fail.
     */
    for ( i = 0; i < PING_MAX_PROBE_INFO; i++ )
    {
        if ( pingEntry->info[i].rxTime.seconds == 0 &&
                pingEntry->info[i].rxTime.nanoseconds == 0  )
        {
            pingEntry->info[i].result = PING_PROBE_TIMEOUT;
            pingEntry->info[i].rc = PING_PROBE_RC_FAIL;
        }
    }

    /* Decrement the Session Count, if mode is async */
    if ( pingEntry->mode == PING_PROBE_MODE_ASYNC )
    {
        pingAsyncSessionCount--;
    }

    if (pingEntry->finishCallback)
    {
      pingEntry->finishCallback(pingEntry->userParam);
    }
    /* Close the socket */
    osapiSocketClose( pingEntry->sock );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handles a new ASYNC ping request.
*
* @param    None
*
* @returns  L7_SUCCESS Session was started succesfuly.
* @returns  L7_FAILURE If starting the session fails.
*
* @notes    If this is first ASYNC request, then spaws a new task to handle
*           ASYNC requests. Increments the number of simultaneous ASYNC ping
*           request in pingAsyncSessionCount
*
* @end
*********************************************************************/
L7_RC_t
pingSessionAsync( void )
{
    void *rxArgs[1];

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    pingAsyncSessionCount++;
    /* Spwan a new task if this is the first session */
    if ( pingAsyncSessionCount == 1 )
    {
        rxArgs[0] = 0;
        if ((pingAsyncTaskId =  osapiTaskCreate( (L7_uchar8 *)"pingAsync",
                        (void *)pingASyncTask, 1, &rxArgs,
                        L7_DEFAULT_STACK_SIZE, L7_DEFAULT_TASK_PRIORITY,
                        L7_DEFAULT_TASK_SLICE)) == L7_FAILURE)
        {
            L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Could not spawn pingAsync task\n" );
            return L7_FAILURE;
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process ASYNC ping requests.
*
* @param    None
*
* @returns  NULL
*
* @notes    Sends probe requests for all the ASYNC requests. Also listens for
*           response and processes them.
*
*           Runs on a infinite loop until processing for all sessions are done.
*           a. Sends a probe for every probe interval configured. Atmost
*              probeCount number of probes are sent
*           b. Also reads ICMP responses and updates the session table
*
*           Repeats (a) and (b) till all ASYNC sessions end. An ASYNC sesion
*           ends under following conditions,
*           1. Receives response for all the probeCount requests
*           2. Timeout happens after sending the last probe request.
* @end
*********************************************************************/
static void *
pingASyncTask( void *param )
{
    fd_set rFds;
    fd_set eFds;
    L7_clocktime startTime;
    L7_clocktime endTime;
    L7_long32 timeoutSecs;
    L7_long32 timeoutUSecs;
    L7_ushort16 i;
    L7_uint32 elapsedTime;
    L7_uint32 maxFd;
    pingSessionTable_t *pingEntry;
    L7_uint32 tmpTaskId;
    L7_int32  error = 0;
    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    /* Be in the loop until all the ASYNC ping sessions are done */
    while ( 1 )
    {

        /* Exit out of loop if there are no more sessions */
        if ( pingAsyncSessionCount == 0 )
        {
            break;
        }

        /* Scan through the table and add all the sockets to poll */
        pingASyncSetFd( &maxFd, &rFds, &eFds );

        /* Compute the time to wait in select */
        timeoutSecs = 0;
        timeoutUSecs = pingASyncGetTimeout( );

        osapiWriteLockGive( pingSessionLock_g );

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
        pingTimeUSecElapsed( &endTime, &startTime, &elapsedTime );

        osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
        /*
         * Loop through session table and perform following for each session
         * 1. Process any PDU recieved on the socket
         * 2. Process any Exception on the socket
         * 3. Send a probe if probeInterval is elapsed
         */
        for ( i = 0; i < PING_MAX_SESSIONS; i++ )
        {
            pingGetEntry( i, &pingEntry );

            /* Only ASYNC entries processed here */
            if ( pingEntry->mode != PING_PROBE_MODE_ASYNC )
            {
                continue;
            }

            /* Ignore the entry if session is not active */
            if ( pingEntry->state != PING_SESSION_STATE_IN_PROGRESS )
            {
                continue;
            }

            /* Process any PDU received or any error */
            if ( FD_ISSET( pingEntry->sock, &rFds ) )
            {
                pingRxPdu( pingEntry );
            }

            if ( FD_ISSET( pingEntry->sock, &eFds ) )
            {
                pingRxErr( pingEntry );
            }

            /* Ignore entry if Rx processing finds its end of processing */
            if ( pingEntry->state != PING_SESSION_STATE_IN_PROGRESS )
            {
                continue;
            }

            /* Check if we need to send new probes due to elapsed time */
            pingElapsedTime( pingEntry, elapsedTime );
        }
    }

    /* If control comes out of loop, it means all ping sessions are complete.
     * Destroy the thread */
    if ( pingAsyncTaskId != 0 )
    {
        tmpTaskId = pingAsyncTaskId;
        pingAsyncTaskId = 0;
        osapiWriteLockGive( pingSessionLock_g );
        osapiTaskDelete( tmpTaskId );
    }
    else
    {
        osapiWriteLockGive( pingSessionLock_g );
    }


    return NULL;
}

/*********************************************************************
* @purpose  Create a socket for ping session
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
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
pingCreateSock( pingSessionTable_t *pingEntry )
{
   L7_int32 one = 1;
   L7_sockaddr_in_t srcaddr;

   PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
             __FUNCTION__);
    /* Create a socket of type ICMP */
    if ( osapiSocketCreate( L7_AF_INET, L7_SOCK_RAW, IPPROTO_ICMP,
                &(pingEntry->sock)) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
                "Error opening ICMP RAW socket. Error = %d \n",
                (L7_int32)pingEntry->sock );
        return L7_FAILURE;
    }

    if (osapiSetsockopt(pingEntry->sock, L7_SOL_SOCKET, L7_SO_BROADCAST,(L7_char8 *) &one, sizeof(one))== L7_FAILURE)
    {
       L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "pingCreateSock(): osapiSetsockopt SO_BROADCAST failed\n");

    }
#ifdef _L7_OS_LINUX_
    /* For large pings, ensure the receive buffer is large enough     */
    /* to receive 2 copies of the ping.(The broadcast & the response) */
    if (pingEntry->probeSize > 40*1024)
    {
        int sockSize;
        sockSize = (pingEntry->probeSize + PING_IP_HEADER_LEN) * 2;
        if (osapiSetsockopt(pingEntry->sock, L7_SOL_SOCKET, L7_SO_RCVBUF,
                            (L7_uchar8 *)&sockSize, sizeof(sockSize) ))
        {
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                       "%s: setsockopt FAILED\n", __FUNCTION__);
            return L7_FAILURE;
        }
    }
#else /* Vxworks IPNET stack supports 10000 bytes BUFF size */
     if(pingEntry->probeSize > 9972)
     {
       L7_uint32 sndRcvBuffSize = 65535;
       if (osapiSetsockopt(pingEntry->sock, L7_SOL_SOCKET, L7_SO_SNDBUF,(L7_char8 *)
                      &sndRcvBuffSize, sizeof(sndRcvBuffSize))== L7_FAILURE)
       {
         L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                       "%s: setsockopt FAILED\n", __FUNCTION__);
         return L7_FAILURE;
       }
       if (osapiSetsockopt(pingEntry->sock, L7_SOL_SOCKET, L7_SO_RCVBUF,(L7_char8 *)
                      &sndRcvBuffSize, sizeof(sndRcvBuffSize))== L7_FAILURE)
       {

         L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                       "%s: setsockopt FAILED\n", __FUNCTION__);
         return L7_FAILURE;
       }
    }
#endif

#if 0
    /* Set the VRF for the socket*/
    if ( osapiIpStackSocketVrAttach( pingEntry->sock, pingEntry->vrfId )
            != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Error attaching socket to vr = %d \n", pingEntry->vrfId );
        osapiSocketClose( pingEntry->sock );
        return L7_FAILURE;
    }
#endif

    if (pingEntry->probeSrcIpAddr)
    {
      /* Set source IP address for echo requests */
      srcaddr.sin_family = L7_AF_INET;
      srcaddr.sin_addr.s_addr = pingEntry->probeSrcIpAddr;
      if (osapiSocketBind(pingEntry->sock, (L7_sockaddr_t*) &srcaddr, sizeof(srcaddr)) != L7_SUCCESS)
      {
        L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(pingEntry->probeSrcIpAddr, srcAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                  "Failed to bind ping socket to IPv4 address %s.",
                  srcAddrStr);
        osapiSocketClose(pingEntry->sock);
        return L7_FAILURE;
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
* @notes    Scans pingSessionTable and sets bits in fd_set for socket of
*           ASYNC session.
*
* @end
*********************************************************************/
static L7_RC_t
pingASyncSetFd( L7_uint32 *maxFd, fd_set *rFds, fd_set *eFds )
{
    pingSessionTable_t *pingEntry;
    L7_ushort16 i;
    L7_ushort16 flag = 0;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    FD_ZERO( rFds );
    FD_ZERO( eFds );

    /* Scan through the table, and add socket for requests of type ASYNC */
    *maxFd = 0;
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );

        if ( pingEntry->mode == PING_PROBE_MODE_ASYNC &&
            pingEntry->state == PING_SESSION_STATE_IN_PROGRESS )
        {
            if ( *maxFd < pingEntry->sock )
            {
                *maxFd = pingEntry->sock;
            }
            FD_SET( pingEntry->sock, rFds );
            FD_SET( pingEntry->sock, eFds );
            flag = 1;
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
* @notes    Scans pingSessionTable and finds the minimum residue time.
*           Returns residue as time to wait.
*
* @end
*********************************************************************/
static L7_uint32
pingASyncGetTimeout( )
{
    L7_ushort16 i;
    L7_uint32 residue = PING_INVALID_RTT;
    pingSessionTable_t *pingEntry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    /* Mode is ASYNC. Timeout is min of residue in all the ASYNC sessions */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );
        if ( pingEntry->state != PING_SESSION_STATE_IN_PROGRESS )
        {
            continue;
        }

        if ( pingEntry->mode == PING_PROBE_MODE_SYNC )
        {
            continue;
        }

        if ( pingEntry->probeIntervalResidue < residue )
        {
            residue = pingEntry->probeIntervalResidue;
        }
    }

    /* If a new ping request arrives when thread is waiting in select, then
     * the ping will not be executed until select returns. To ensure, the new
     * ping is not delayed very long, put a limit on the max time select can
     * wait for.
     * TODO: This might not be a good idea, since the task will wake after time
     * given below even if there are no new requests. Ideally, we want to
     * interrupt the select when a new ping request arrives. Some of the
     * alternatives,
     * a. Send a dummy packet on any of the sockets in select call.
     * b. On Linux send a dummy signal to the thread and select will come out.
     */
    if ( residue > PING_MAX_WAIT_TIME )
    {
        residue = PING_MAX_WAIT_TIME;
    }

    return residue;
}

/*********************************************************************
* @purpose  Handle elapsing of time for ping requests.
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Entry for the session
* @param    L7_uint32 elapsedTime @b{(input)} Number of usec elapsed.
*
* @returns  L7_SUCCESS
*
* @notes    Sends a probe request if time for entry expires.
*           If probeInterval number of iterations are done, then changes
*           state to DONE.
*
* @end
*********************************************************************/
static L7_RC_t
pingElapsedTime( pingSessionTable_t *pingEntry, L7_uint32 elapsedTime )
{
    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    /* Check if its time to send next probe */
    if ( elapsedTime >= pingEntry->probeIntervalResidue )
    {
        pingEntry->probeIterations--;

        /* End session if number of iterations configured are expired */
        if ( pingEntry->probeIterations == 0 )
        {
            pingEnd( pingEntry, PING_SESSION_STATE_DONE );
        }

        /* Do we need to send any more probes? */
        if ( pingEntry->probeSent < pingEntry->probeCount )
        {
            pingSendProbe( pingEntry );
        }

        /* Reset probeIntervalResidue */
        pingEntry->probeIntervalResidue = pingEntry->probeInterval;
    }
    else
    {
        /* Update probeIntervalResidue */
        pingEntry->probeIntervalResidue -= elapsedTime;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sends a probe request.
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
*
* @returns  L7_SUCCESS If probe was sent
* @returns  L7_FAILURE If probe could not be sent
*
* @notes    Sends a probe request. Allocates one response info entry and
*           initializes the fields.
*
* @end
*********************************************************************/
static L7_RC_t
pingSendProbe( pingSessionTable_t *pingEntry )
{
    L7_uint32 i;
    L7_uchar8 j;
    L7_ushort16 respIndex;
    pingProbeInfo_t *probeInfoEntry;
    struct icmp icmpMsg;
    L7_uchar8 *pingBuff;
    L7_sockaddr_t to;
    L7_sockaddr_in_t *toPtr;

   PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
            __FUNCTION__);
   pingBuff = osapiMalloc( L7_SIM_COMPONENT_ID,
        PING_MAX_BUFF * sizeof( *pingBuff) );
    if (  pingBuff == NULL )
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "%s %d: %s : osapiMalloc failed\n",
          __FILE__, __LINE__, __FUNCTION__);
      return L7_FAILURE;
    }


    /* Get entry to be used in resp information table. */
    respIndex = pingEntry->probeSent % PING_MAX_PROBE_INFO;
    probeInfoEntry = &(pingEntry->info[respIndex]);

    /*
     * Note, we might be overwriting an entry used earlier in current session.
     * But, that does not have any implication for us. If the response comes
     * it is treated as delayed response. The active window in resp info
     * is always between (probeSent - PING_MAX_pktLen) and probeSent.
     */
    /* Init the fields in response info entry */
    osapiUTCTimeGet( &(probeInfoEntry->txTime) );
    bzero( (void *)&(probeInfoEntry->rxTime), sizeof( L7_clocktime ) );
    probeInfoEntry->result = PING_PROBE_SENT;
    probeInfoEntry->rc = 0;

    /* Initialize the ICMP fields */
    bzero ( (L7_uchar8 *) &icmpMsg, sizeof (struct icmp) );
    icmpMsg.icmp_type = ICMP_ECHO;
    icmpMsg.icmp_id = osapiHtons(pingEntry->handle);
    icmpMsg.icmp_seq = osapiHtons(pingEntry->probeSent);

    j = 0;
    /* Initialize the data portion with 0,1,2,... */
    for ( i = ICMP_MINLEN; i < (pingEntry->probeSize + ICMP_MINLEN); i++ )
    {
        pingBuff[i] = (L7_uchar8)j++;
    }

    /* Compute size of the PDU */
    i = pingEntry->probeSize + ICMP_MINLEN;

    /* Copy the ICMP Header for checksum calculation */
    memcpy( pingBuff, &icmpMsg, ICMP_MINLEN );
    /* Compute ICMP checksum */
    icmpMsg.icmp_cksum = pingInetChksum( pingBuff, i );
    /* Copy the header updated with checksum */
    memcpy( pingBuff, &icmpMsg, ICMP_MINLEN );

    bzero( (void *)&to, sizeof( to ) );
    toPtr = (L7_sockaddr_in_t *)&to;
    toPtr->sin_family = L7_AF_INET;
    toPtr->sin_addr.s_addr = osapiHtonl( pingEntry->ipDa );

    pingEntry->probeSent++;
    if ( osapiSocketSendto( pingEntry->sock, (L7_uchar8 *)pingBuff, i, 0, &to,
                sizeof(to), &i ) == L7_SUCCESS )
    {
#ifdef _L7_OS_LINUX_
        (void) osapiL7IcmpOutEchosInc();
#endif
        osapiFree(L7_SIM_COMPONENT_ID, pingBuff);
        return L7_SUCCESS;
    }
    else
    {
        osapiFree(L7_SIM_COMPONENT_ID, pingBuff);
        return L7_FAILURE;
    }
}

/*********************************************************************
* @purpose  Handles an ICMP message received.
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
*
* @returns  L7_SUCCESS If the PDU was processed
* @returns  L7_FAILURE If probe was ignored
*
* @notes    Handles only ICMP REPLIES.
*           Lower 4 bits of icmp_id identifies the entry in pingSessionTable.
*           sequnce-num % PING_PROBE_MAX_INFO identifies the entry in resp
*           info table.
*           Updates the result, rc and rxTime fields.
*
* @end
*********************************************************************/
static L7_RC_t
pingRxPdu( pingSessionTable_t *pingEntry )
{
    L7_uint32           remAddrLen;
    L7_sockaddr_t       remAddr;
    L7_uchar8           *pingBuf;
    L7_ushort16         seqNum = L7_NULL;
    L7_ushort16         type;
    L7_ushort16         respIndex;
    L7_uint32           dataLen;
    L7_uint32           rtt = 0;
    L7_uchar8 src_ipAddress[OSAPI_INET_NTOA_BUF_SIZE];

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
             __FUNCTION__);
    pingBuf = osapiMalloc( L7_SIM_COMPONENT_ID,
        PING_MAX_BUFF * sizeof( *pingBuf) );
    if (  pingBuf == NULL )
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "%s %d: %s : osapiMalloc failed\n",
          __FILE__, __LINE__, __FUNCTION__);
      return L7_FAILURE;
    }


    /* Read the PDU from socket */
    remAddrLen = sizeof(remAddr) ;
    if ( osapiSocketRecvfrom(pingEntry->sock, pingBuf, PING_MAX_BUFF,
                    0, &remAddr, &remAddrLen, &dataLen ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
                "Error in receiving probe response.\n" );
        osapiFree(L7_SIM_COMPONENT_ID, pingBuf);
        return L7_FAILURE;
    }

    /* Check if the packet belongs to this session and must be processed */
    if ( pingValidate( pingEntry, pingBuf, dataLen, &seqNum, &type, src_ipAddress) != L7_SUCCESS )
    {
      osapiFree(L7_SIM_COMPONENT_ID, pingBuf);
        return L7_FAILURE;
    }

    /* Compute index in probe info table */
    respIndex = seqNum % PING_MAX_PROBE_INFO;

    /* Update the rxTime in response */
    osapiUTCTimeGet( &(pingEntry->info[respIndex].rxTime) );
    if (type != ICMP_UNREACH)
    {
      pingEntry->info[respIndex].result = PING_PROBE_SUCC;

      /* Update maxRtt and minRtt for the session */

      /* Compute the RTT for this probe */
      pingTimeUSecElapsed(&(pingEntry->info[respIndex].rxTime),
          &(pingEntry->info[respIndex].txTime), &rtt);

      if ( pingEntry->minRtt > rtt )
      {
        pingEntry->minRtt = rtt;
      }

      if ( pingEntry->maxRtt < rtt )
      {
        pingEntry->maxRtt = rtt;
      }

      /* Add this RTT for aggrRtt. Used to compute avgRtt later on */
      pingEntry->aggrRtt += rtt;

    pingEntry->info[respIndex].rc = PING_PROBE_SUCC;
    pingEntry->probeSuccess++;
    }
    else
    {
      pingEntry->info[respIndex].result = PING_PROBE_NO_ROUTE;
      pingEntry->minRtt = 0;
      pingEntry->maxRtt = 0;
      pingEntry->aggrRtt = 0;
      rtt = 0;
    }
    /* Stop if we have already received all probe responses */
    if ( pingEntry->probeSuccess >= pingEntry->probeCount )
    {
        pingEnd( pingEntry, PING_SESSION_STATE_DONE );
    }

    if ( pingEntry->callback != NULL )
    {
        pingEntry->callback( pingEntry->userParam, seqNum, rtt, type, src_ipAddress );
    }

    osapiFree(L7_SIM_COMPONENT_ID, pingBuf);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle exception on ICMP sockets.
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
*
* @returns  L7_SUCESS
*
* @notes    Closes the session
*
* @end
*********************************************************************/
static L7_RC_t
pingRxErr( pingSessionTable_t *pingEntry )
{
    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    /* Close the session if any error on socket */
    pingEnd( pingEntry, PING_SESSION_STATE_DONE );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate if packet is okay to be processed
*
* @param    pingSessionTable_t *pingEntry @b{(input)} Pointer to ping session
* @param    L7_char *pingBuf              @b{(input)} Buffer containing the PDU
* @param    L7_ushort16 dataLen           @b{(input)} Length of the data
* @param    L7_ushrot *seqNum             @b{(output)} Seq Num in the recvd PDU
*
* @returns  L7_FAILURE If packet is to be ignored
* @returns  L7_SUCCESS If packet is to be processed
*
* @notes
*           Verifies ICMP Checksum and also checks if the packet is of
*           interest and can be processed or not.
*
* @end
*********************************************************************/
static L7_RC_t
pingValidate( pingSessionTable_t *pingEntry, L7_uchar8 *pingBuf,
        L7_uint32 dataLen, L7_ushort16 *seqNum, L7_ushort16 *icmp_type, L7_uchar8 *ipAddr )
{
    L7_ushort16         ipLen;
    struct icmp         *icmpPtr;
    L7_ipHeader_t       *ipHdr;
    L7_uint32           bytesRemained;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);

    bytesRemained = dataLen;
    /* Compute length of IP Header */
    ipHdr = (L7_ipHeader_t *)pingBuf;
    ipLen = (ipHdr->iph_versLen & 0xF) * 4;
    bytesRemained -= ipLen;
    icmpPtr = (struct icmp *)( pingBuf + ipLen );
    *icmp_type = icmpPtr->icmp_type;
     osapiInetNtoa((L7_uint32)osapiNtohl(ipHdr->iph_src),ipAddr);
    bytesRemained -= ICMP_MINLEN;

    /* Verify ICMP checksum */
    if ( pingInetChksum((L7_uchar8 *)icmpPtr, (dataLen - ipLen) ) != 0 )
    {
        return L7_FAILURE;
    }


    /* Only ECHOREPLY and Destination Unreachable handled. There is no need to handle other types */
    if ( (icmpPtr->icmp_type != ICMP_ECHOREPLY) && (icmpPtr->icmp_type != ICMP_UNREACH) )

    {
        return L7_FAILURE;
    }
    if (icmpPtr->icmp_type == ICMP_UNREACH)
    {
       if (bytesRemained == 0)
       {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
          "Invalid ICMP Unreachable Packet Received. ");
          return L7_FAILURE;
       }
       else
       {
          L7_ipHeader_t       *hip;
          L7_ushort16         hlen;
          struct icmp         *icmpHeader;

          hip = (L7_ipHeader_t *)&(icmpPtr->icmp_ip);

          /* Get the header length from the received packet */
          hlen = (hip->iph_versLen & 0x0f);

          /* Check if packet contains original IP and ICMP Headers */
          if (bytesRemained <  ((hlen*4) + ICMP_MINLEN))
          {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
              "Received ICMP Unreachable Packet Length is too small to verify the pkt belongs to this session or not");
              return L7_FAILURE;
          }
          icmpHeader = (struct icmp *) (((L7_uint32 *) hip) + hlen);
          /* The handle in entry must have been used as ID field in ICMP */
          if ( pingEntry->handle != osapiNtohs(icmpHeader->icmp_id ) )
          {
            return L7_FAILURE;
          }
       }
       *seqNum = L7_NULL;
       return L7_SUCCESS;
    }

    /* Verify if size of packet received is same as one sent. Maybe some
     * implementations do not send the data back. Log message and continue */
    if (pingEntry->probeSize != (dataLen - ipLen - ICMP_MINLEN) &&
        (icmpPtr->icmp_type == ICMP_ECHOREPLY))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
          "Invalid pkt len in ICMP Response . Expected = %u, Got = %u."
          " Ignoring error and continuing\n",
          pingEntry->probeSize, (dataLen - ipLen) );
    }

    if (icmpPtr->icmp_type == ICMP_ECHOREPLY)
    {

      /* The handle in entry must have been used as ID field in ICMP */
      if ( pingEntry->handle != osapiNtohs(icmpPtr->icmp_id ) )
      {
        return L7_FAILURE;
      }

      /* Get the sequence number in ICMP Response */
      *seqNum = osapiNtohs( icmpPtr->icmp_seq );

      /* Ignore if received sequence number > last we have sent */
      if ( *seqNum >= pingEntry->probeSent )
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "Invalid sequence number in ICMP Response."
            " Expected < %u, Got = %u\n",
            pingEntry->probeSent, *seqNum );
        return L7_FAILURE;
      }

      /* A PDU is delayed, if its sequence number is less than the min
       * sequence number in our info array */
      if ( *seqNum <= (pingEntry->probeSent - PING_MAX_PROBE_INFO) )
      {
        if ( *seqNum < (pingEntry->probeSent - PING_MAX_PROBE_INFO) )
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
              "Received delayed ICMP response\n" );
          pingEntry->probeDelayed++;
        }
        return L7_FAILURE;
      }
      /* A PDU is duplicated, when ping to a Broadcast address there is a chance to get
        more than one ECHO Reply packet for an Echo request. Ignore the rest of the echo replies for
        the transmitted  ECHO Request packet */

      if ( (*seqNum < pingEntry->probeSent) &&
           (pingEntry->probeSuccess == pingEntry->probeSent))
      {
         L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
              "Received duplicated ICMP response\n" );
         return L7_FAILURE;
      }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns pingSessionTable entry for index specified
*
* @param    L7_ushort16 index @b{(input)} Index for the entry
* @param    pingSessionTable_t **pingEntry @b{(output)} Pointer to entry where
*               address is to be stored
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void
pingGetEntry( L7_ushort16 index, pingSessionTable_t **pingEntry )
{
    *pingEntry = &pingSessionTable_g[index];
}

/**************************************************************************
 * *
 * * @purpose  Compute Inet Checksum over the buffer
 * *
 * * @param    buff         @b{(input)} Buffer containing data
 * * @param    len          @b{(input)} Lentgh of data
 * *
 * * @returns  Checksum
 * *
 * * @end
 * *
 * *************************************************************************/
static L7_RC_t
pingInetChksum( L7_uchar8 *buff, L7_ushort16 len )
{

    L7_ushort16 *data = (unsigned short *)buff;
    L7_ushort16 residue;
    L7_uint32 accum = 0;
    L7_uint32 i;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    for (i = 0; i < (len / 2); i++)
    {
        accum += osapiHtons(*data);
        data++;
    }

    if ((len % 2) == 1)
    {
        residue = (*((L7_ushort16 *)data));
        accum += osapiHtons(residue);
    }

    /* account for overflow */
    accum = (((accum & 0xffff0000) >> 16) + (accum & 0x0000ffff));

    /* do it twice, in case of second overflow */
    accum = (((accum & 0xffff0000) >> 16) + (accum & 0x0000ffff));
    return(osapiHtons(~((L7_ushort16)(accum & 0x0000ffff))));
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
pingTimeUSecElapsed( L7_clocktime *end, L7_clocktime *start,
        L7_uint32 *elapsedTime )
{
    *elapsedTime = (end->seconds - start->seconds) * 1000000;
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

