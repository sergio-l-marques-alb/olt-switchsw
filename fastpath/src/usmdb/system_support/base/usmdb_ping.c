/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  usmdb_ping.h
*
* @purpose   Provide interface to API's for ping functionality
*
* @component Ping
*
* @comments  USMDB Implementation of ping interfaces
*
* @create    09/08/2005
*
* @author    Praveen K V
*
* @end
*       Following are 2 example sequence of operations for ping
*       Option 1.
*           1. Allocate an entry by calling usmDbPingSessionAlloc
*           2. Configure the session by calling usmDbPingDestAddrSet etc...
*           3. Call usmDbPingSessionStart() to start the ping session
*           4. Query the result by calling usmDbPingQuery
*           5. Free the session by calling usmDbPingFree
*       Option 2.
*           1. Call usmDbPingStart() to start the ping session with all params
*           2. Query the result by calling usmDbPingQuery
*           3. Free the session by calling usmDbPingFree
*
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_ip_base_api.h"

#include "osapi.h"
#include "osapi_support.h"
#include "usmdb_ping_api.h"
#include "ping_api.h"


/*********************************************************************
* @purpose  Allocates a ping session 
*
* @param  L7_uchar8 *index     @b{(input)} Field for pingCtlOwnerIndex
* @param  L7_uchar8 *name      @b{(input)} Name of the session
* @param  L7_BOOL    syncFlag  @b{(input)} Operation mode(TRUE=Sync, FALSE=ASync)
* @param  void       *callback @b{(input)} Callback function called for each 
*                     probe response. Pass NULL if callback is not needed
* @param  void       *finishCallback @b{(input)} Callback function called after
*                                                completing the ping operation
* @param  L7_ushort16 *userHandle @b{(input)}  User parameter passed in callback
* @param  L7_ushort16 *handle     @b{(output)} Handle for the session allocated
*
* @returns  L7_SUCCESS   If session could be allocated
* @returns  L7_FAILURE   If session could not be could be allocated
*
* @notes
*
*       Index and Name act as index into the session table.
*
*       index of NULL or name = NULL are reserved. If these values are 
*       passed, then session cannot be queried using usmDbPingSessionGet.
*       The application will have to store the handle in such case
*
*       The handle returned must be given in further calls to ping APIs
*       Ping application will pass back userHandle in callback routine
*
*       In either case, the API pingSessionFree must be called to delete 
*       the session.
*
*       At most PING_MAX_SESSIONS simultaneous sessions are supported.
*       The calling API must call pingSessionEnd to free a session.
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingSessionAlloc( L7_uchar8 *index, L7_uchar8 *name, L7_BOOL syncFlag, 
        void *callback, void *finishCallback, void *userHandle, L7_ushort16 *handle )
{
    return pingSessionAlloc( index, name, syncFlag, callback, finishCallback, userHandle, 
            handle );
}

/*********************************************************************
* @purpose  Frees a ping session
*
* @param  L7_ushort16 handle @b{(input)} Handle for ping session
*
* @returns  L7_SUCCESS   If session could be freed
* @returns  L7_FAILURE   If address could not be freed
*
* @notes
*       Fails if session is not allocated
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingSessionFree( L7_ushort16 handle )
{
    return pingSessionFree( handle );
}

/*********************************************************************
* @purpose  Ends a ping session
*
* @param  L7_ushort16 handle @b{(input)} Handle for ping session
*
* @returns  L7_SUCCESS   If session could be end
* @returns  L7_FAILURE   If address could not be end
*
* @notes
*       Fails if session is not allocated
*
* @end
*********************************************************************/
L7_RC_t usmDbPingSessionEnd( L7_ushort16 handle )
{
  return pingSessionEnd( handle );
}  

/*********************************************************************
* @purpose  Queries a ping session 
*
* @param  L7_uchar8 *index  @b{(input)}  Field for pingCtlOwnerIndex
* @param  L7_uchar8 *name   @b{(input)}  Name of the session.
* @param  L7_ushort16 *handle @b{(output)} Handle for the ping session started.
* @param  L7_BOOL  *operStatus @b{(output)}  Operational status of the session
* @param  L7_uint32 *vrfId @b{(output)) VRF-ID of the session
* @param  L7_uint32 *ipDa  @b{(output)) IP-DA of the session
* @param  L7_ushort16 *count  @b{(output)) Number of probe request to send
* @param  L7_ushort16 *size  @b{(output)) Size of the probe request
* @param  L7_ushort16 *interval  @b{(output)) Interval between probes
*
* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*       Cannot query sessions created with index = NULL or name = NULL
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingSessionHandleGet( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle, 
        L7_BOOL *operStatus, L7_uint32 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *count, L7_ushort16 *size, L7_ushort16 *interval )
{
    return pingSessionHandleGet( index, name, handle, operStatus, vrfId, ipDa, count,
            size, interval );
}

/*********************************************************************
* @purpose  Queries a ping session handle
*
* @param  L7_uchar8 *index  @b{(input)}  Field for pingCtlOwnerIndex
* @param  L7_uchar8 *name   @b{(input)}  Name of the session.
* @param  L7_ushort16 *handle @b{(output)} Handle for the ping session started.
* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*       Cannot query sessions created with index = NULL or name = NULL
*
* @end
*********************************************************************/
L7_RC_t
usmDbPingSessionHandleFromIndexTestNameGet( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle)
{
    return pingSessionHandleFromIndexTestNameGet( index, name, handle);
}  

/*********************************************************************
* @purpose  Provide destination address for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @parame L7_uint32 ipDa     @b{(input)} IP Destination address
*
* @returns  L7_SUCCESS   If address could be updated
* @returns  L7_FAILURE   If address could not be updated
*
* @notes
*       Fails if session is not allocated or already started
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingDestAddrSet( L7_ushort16 handle, L7_uint32 ipDa )
{
    return pingDestAddrSet( handle, ipDa );
}

/*********************************************************************
* @purpose  Provide VR-ID for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @parame L7_ushort16 vrId   @b{(input)} VR-ID for the session

* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or already started
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingVrIdSet( L7_ushort16 handle, L7_ushort16 vrId )
{
    return pingVrIdSet( handle, vrId );
}

/*********************************************************************
* @purpose  To set number of probes for session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 count  @b{(input)} Number of probes to send
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingProbeCountSet( L7_ushort16 handle, L7_ushort16 count )
{
    return pingProbeCountSet( handle, count );
}

/*********************************************************************
* @purpose  To get number of probes for session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 count  @b{(output)} Number of probes to send
*
* @returns  L7_SUCCESS   If session get is success
* @returns  L7_FAILURE   If session get fails
*
* @notes
*       Fails if session is not allocated.
*
* @end
*********************************************************************/
 L7_RC_t
usmDbPingProbeCountGet( L7_ushort16 handle, L7_uint32 *count )
{
    return pingProbeCountGet( handle, count );
}
/*********************************************************************
* @purpose  To set IP DSCP value to be used while sending out probes
*
* @param  L7_ushort16 handle   @b{(input)} Handle for session
* @param  L7_uint32   dscpVal  @b{(input)} IP DSCP value
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
L7_RC_t usmDbPingDSCPSet( L7_ushort16 handle, L7_uint32 dscpVal )
{
  return pingDSCPSet(handle, dscpVal);
}
/*********************************************************************
* @purpose  To get DSCP value used in ping probe packets
*
* @param  L7_ushort16 handle   @b{(input)} Handle for session
* @param  L7_uint32   dscpVal  @b{(output)} DSCP Value
*
* @returns  L7_SUCCESS   If session get is success
* @returns  L7_FAILURE   If session get fails
*
* @notes
*       Fails if session is not allocated.
*
* @end
*********************************************************************/
L7_RC_t usmDbPingDSCPGet( L7_ushort16 handle, L7_uint32 *dscpVal )
{
  return pingDSCPGet(handle, dscpVal);
}
/*********************************************************************
* @purpose  To set interval between probes for a session
*
* @param  L7_ushort16 handle   @b{(input)} Handle for ping session
* @param  L7_ushort16 interval @b{(input)} Interval between probes in seconds
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingProbeIntervalSet( L7_ushort16 handle, L7_ushort16 interval )
{
    return pingProbeIntervalSet( handle, interval );
}

/*********************************************************************
* @purpose  To get interval between probes for a session
*
* @param  L7_ushort16 handle   @b{(input)} Handle for ping session
* @param  L7_ushort16 interval @b{(output)} Interval between probes in seconds
*
* @returns  L7_SUCCESS   If session get is success
* @returns  L7_FAILURE   If session get fails
*
* @notes
*       Fails if session is not allocated .
*
* @end
*********************************************************************/
L7_RC_t
usmDbPingProbeIntervalGet( L7_ushort16 handle, L7_uint32 *interval )
{
    return pingProbeIntervalGet( handle, interval );
}

/*********************************************************************
* @purpose  To set PDU size in probe request
*
* @param  L7_ushort16 handle @b{(input)} Handle for ping session
* @param  L7_ushort16 size   @b{(input)} Size of PDU in probe request
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingProbeSizeSet( L7_ushort16 handle, L7_ushort16 size )
{
    return pingProbeSizeSet( handle, size );
}
 
/*********************************************************************
* @purpose  To get PDU size in probe request
*
* @param  L7_ushort16 handle @b{(input)} Handle for ping session
* @param  L7_ushort16 size   @b{(output)} Size of PDU in probe request
*
* @returns  L7_SUCCESS   If session get is success
* @returns  L7_FAILURE   If session get fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
L7_RC_t
usmDbPingProbeSizeGet( L7_ushort16 handle, L7_uint32 *size )
{
    return pingProbeSizeGet( handle, size );
}

/*********************************************************************
* @purpose  To start a ping session
*
* @param  L7_ushort16 handle   @b{(input)} Handle for ping session
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingSessionStart( L7_ushort16 handle )
{
    return pingSessionStart( handle );
}

/*********************************************************************
* @purpose  To ping a destination with all parameters given at a time
*
* @param  L7_uchar8   *index   @b{(input)}  Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name    @b{(input)}  Name of the session
* @param  L7_BOOL     syncFlag @b{(input)}  Operation mode(TRUE=Sync,FALSE=ASync)
* @param  L7_ushort16 vrfId    @b{(input)}  VRF instance for the session
* @param  L7_uint32   ipDa     @b{(input)}  IP Address of the destination
* @param  L7_ushort16 probeCount @b{(input)} Number of ping probes to send
* @param  L7_ushort16 probeSize  @b{(input)} Size of the ping probe PDU
* @param  L7_ushort16 probeInterval @b{(input)} Interval between probes in secs
* @param             probeSrcIpAddr @b{(input)} Source IP address of Echo Request. 
*                                               If 0, the IP stack selects.
* @param  void        *callback  @b{(input)}  Callback function called for each 
*             probe response. Pass NULL if callback is not needed.
* @param  void       *finishCallback @b{(input)} Callback function called after
*                                                completing the ping operation
* @param  void        *userParam @b{(input)}  User parameter passed in callback.
* @param  L7_ushort16 *handle    @b{(output)}  Handle for the ping session started.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*       Only index and name are index into the session table.
*
*       index of NULL or name = NULL are reserved. If these values are 
*       passed, then session cannot be queried using usmDbPingSessionGet.
*       The application will have to store the handle in such case
* @end
*********************************************************************/
L7_RC_t 
usmDbPingStart(L7_uchar8 *index, L7_uchar8 *name, L7_BOOL syncFlag,
               L7_uint32 vrfId, L7_uint32 ipDa, L7_ushort16 probeCount, 
               L7_ushort16 probeSize, L7_ushort16 probeInterval, 
               L7_uint32 probeSrcIpAddr, void *callback, 
               void *finishCallback, void *userParam, L7_ushort16 *handle )
{

    L7_RC_t rc;

    if (( rc = pingSessionAlloc(index, name, syncFlag, callback, finishCallback, 
                                userParam, handle)) != L7_SUCCESS )
        return rc;

    /* Single iteration loop to simplify handling error cases */
    do 
    {
        /* Do the appropriate configurations and break out if any error */
        if (( rc = pingDestAddrSet( *handle, ipDa )) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = pingVrIdSet( *handle, vrfId )) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = pingProbeCountSet( *handle, probeCount )) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = pingProbeSizeSet( *handle, probeSize )) != L7_SUCCESS )
        {
            break;
        }

        if ((rc = pingProbeIntervalSet(*handle, probeInterval)) != L7_SUCCESS )
        {
            break;
        }

        if (probeSrcIpAddr != 0)
        {
          if ((rc = pingProbeSrcIpAddrSet(*handle, probeSrcIpAddr)) != L7_SUCCESS)
          {
            break;
          }
        }

        if (( rc = pingSessionStart( *handle )) != L7_SUCCESS )
        {
            break;
        }


    } while ( 0 );

    /* Close session if there was any error */
    if ( rc != L7_SUCCESS )
    {
        pingSessionFree( *handle );
    }
    return rc;
}

/*********************************************************************
* @purpose  Queries a ping session
*
* @param L7_ushort16 handle       @b{(input)}   Handle for session
* @param L7_BOOL     *operStatus  @b{(output)}  Operational status of session
* @param L7_ushort16 probeSent    @b{(output)}  Number of probes sent
* @param L7_ushort16 probeSuccess @b{(output)}  Number of probes succeeded
* @param L7_ushort16 probeFail    @b{(output)}  Number of probes failed
* @param L7_uint32   minRtt       @b{(output)}  Minimum RTT
* @param L7_uint32   maxRtt       @b{(output)}  Max RTT
* @param L7_uint32   avgRtt       @b{(output)}  Avg RTT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingQuery( L7_ushort16 handle, L7_BOOL *operStatus, 
        L7_ushort16 *probeSent, L7_ushort16 *probeSuccess, 
        L7_ushort16 *probeFail, L7_uint32 *minRtt, L7_uint32 *maxRtt, 
        L7_uint32 *avgRtt )
{
    return pingSessionQuery( handle, operStatus, probeSent, probeSuccess, 
            probeFail, minRtt, maxRtt, avgRtt );
}

/*********************************************************************
* @purpose  Queries a ping session
*
* @param L7_ushort16 handle       @b{(input)}   Handle for session
* @param L7_uint32   ipDa         @b{(output)}  target address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryTargetAddrs( L7_ushort16 handle, L7_uint32 *ipDa)
{
    return pingSessionQueryTargetAddrs (handle,ipDa); 
}

/*********************************************************************
* @purpose  Queries a ping session status
*
* @param L7_ushort16 handle       @b{(input)}   Handle for session
* @param L7_BOOL   operStatus   @b{(output)}  oper status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryOperStatus( L7_ushort16 handle, L7_BOOL *operStatus)
{
    return pingSessionQueryOperStatus(handle,operStatus);
}
/*********************************************************************
* @purpose  Queries a ping session state
*
* @param L7_ushort16 handle      @b{(input)}   Handle for session
* @param L7_BOOL     operState   @b{(output)}  oper state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingSessionQuerySessionState( L7_ushort16 handle, L7_uint32 *operState)
{
  return pingSessionQuerySessionState(handle, operState);
}
/*********************************************************************
* @purpose  Queries a ping session minimum RTT
*
* @param L7_ushort16 handle @b{(input)}   Handle for session
* @param L7_uint32 minRtt  @b{(output)}  minRtt
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryMinRtt( L7_ushort16 handle, L7_uint32 *minRtt)
{
    return pingSessionQueryMinRtt(handle,minRtt);
}

/*********************************************************************
* @purpose  Queries a ping session maximum RTT
*
* @param L7_ushort16 handle @b{(input)}   Handle for session
* @param L7_uint32 maxRtt  @b{(output)}  maxRtt
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryMaxRtt( L7_ushort16 handle, L7_uint32 *maxRtt)
{
    return pingSessionQueryMaxRtt(handle,maxRtt);
}

/*********************************************************************
* @purpose  Queries a ping session Average RTT
*
* @param L7_ushort16 handle @b{(input)}   Handle for session
* @param L7_uint32 avgRtt  @b{(output)} avgRtt
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryAvgRtt( L7_ushort16 handle, L7_uint32 *avgRtt)
{
    return pingSessionQueryAvgRtt(handle,avgRtt);
}

/*********************************************************************
* @purpose  Queries a ping session Sent Probes
*
* @param L7_ushort16 handle @b{(input)}   Handle for session
* @param L7_uint32  probeSent @b{(output)}  probeSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryProbeSent( L7_ushort16 handle, L7_uint32 *probeSent)
{
    return pingSessionQueryProbeSent(handle,probeSent);
}

/*********************************************************************
* @purpose  Queries a ping session Probes Responses
*
* @param L7_ushort16 handle @b{(input)}   Handle for session
* @param L7_uint32  probeResponse @b{(output)}  probeResponse
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPingQueryProbeResponse( L7_ushort16 handle, L7_uint32 *probeResponse)
{
    return pingSessionQueryProbeResponse(handle,probeResponse);
}

/*********************************************************************
* @purpose  Get first ping session allocated
*
* @param  L7_uchar8   *index  @b{(output)} Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(output)} Name of the session. 
* @param  L7_ushort16 *handle @b{(output)} Handle for the session
* @param  L7_BOOL     *operStatus @b{(output)}  Operational status of session
* @param  L7_uint32   *vrfId @b{(output)) VRF-ID of the session
* @param  L7_uint32   *ipDa  @b{(output)) IP-DA of the session
* @param  L7_ushort16 *count  @b{(output)) Number of probe request to send
* @param  L7_ushort16 *size  @b{(output)) Size of the probe request
* @param  L7_ushort16 *interval  @b{(output)) Interval between probes
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingSessionGetFirst( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_uint32 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *count, L7_ushort16 *size, 
        L7_ushort16 *interval )
{
    return pingSessionGetFirst( index, name, handle, operStatus, vrfId, ipDa,
            count, size, interval );
}

/*********************************************************************
* @purpose  Get next ping session allocated
*
* @param  L7_uint32   *index     @b{(inout)} Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name      @b{(inout)} Name of the session
* @param  L7_ushort16 *handle    @b{(inout)} Handle for the ping session
* @param  L7_BOOL     *operStats @b{(output)}  Operational status of session
* @param  L7_uint32   *vrfId     @b{(output)) VRF-ID of the session
* @param  L7_uint32   *ipDa      @b{(output)) IP-DA of the session
* @param  L7_ushort16 *count     @b{(output)) Number of probe request to send
* @param  L7_ushort16 *size      @b{(output)) Size of the probe request
* @param  L7_ushort16 *interval  @b{(output)) Interval between probes
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t 
usmDbPingSessionGetNext( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle,
        L7_BOOL *operStatus, L7_uint32 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *count, L7_ushort16 *size, L7_ushort16 *interval )
{
    return pingSessionGetNext( index, name, handle, operStatus, vrfId, ipDa,
            count, size, interval );
}

/*********************************************************************
* @purpose  Get next ping session Handle 
*
* @param  L7_uint32   *index     @b{(inout)} Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name      @b{(inout)} Name of the session
* @param  L7_ushort16 *handle    @b{(inout)} Handle for the ping session
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPingGetNextHandle( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle )
{
    return pingGetNextHandle( index, name, handle );
}

/*********************************************************************
* @purpose  Get Maximum number of ping  sessions allowed
*
* @param  L7_uint32  *maxSessions @b{(output)} maximum number of concurrent ping requests
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbMaxPingSessionsGet(L7_uint32 * maxSessions)
{
    return pingMaxSessionsGet(maxSessions);
}

