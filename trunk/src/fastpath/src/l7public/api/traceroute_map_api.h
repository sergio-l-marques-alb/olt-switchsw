/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   traceroute_map_api.h
*
* @component  Traceroute
*
* @comments   Header file for mapping traceroute function.
*
* @create  05/21/2004
*
* @author  ssharma
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef TRACEROUTE_MAP_API_H
#define TRACEROUTE_MAP_API_H

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
extern L7_RC_t traceRouteCnfgrInitPhase1Process( void );

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
extern L7_RC_t traceRouteCnfgrInitPhase3Process( void );

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
extern L7_RC_t traceRouteCnfgrUconfigPhase2Process( void );

/*********************************************************************
* @purpose  Allocates a traceroute session 
*
* @param L7_uchar8   *index    @b{(input)} Field for traceRouteCtlOwnerIndex
* @param L7_uchar8   *name     @b{(input)} Name of the session
* @param L7_BOOL     syncFlag  @b{(input)} Operation mode(TRUE=Sync, FALSE=ASync)
* @param void        *callback @b{(input)} Callback function called for each 
*           probe response. Pass NULL if callback is not needed.
* @param void        *userHandle @b{(input)}  User parameter passed in callback.
* @param L7_ushort16 *handle     @b{(output)} Handle for the traceroute session.
*
* @returns L7_SUCCESS   If session could be allocated
* @returns L7_FAILURE   If session could not be could be allocated
*
* @notes
*
*       Index and Name act as index into the session table.
*
*       index of NULL or name = NULL are reserved. If these values are 
*       passed, then session cannot be queried using traceRouteGet.
*       The application will have to store the handle in such case
*
*       The handle returned must be given in further calls to traceroute APIs
*       Traceroute application will pass back userHandle in callback routine
*
*       In either case, the API traceRouteFree must be called to delete 
*       the session.
*
*       At most TRACEROUTE_MAX_SESSIONS simultaneous sessions are supported.
*
* @end
*********************************************************************/
typedef L7_RC_t (*traceRouteCallback)( void *param, L7_ushort16 handle, 
        L7_uint32 ttl, L7_uint32 hopDa, L7_ushort16 hopCount, 
        L7_ushort16 probeCount, L7_uint32 rtt, L7_char8 errSym );

extern L7_RC_t traceRouteAlloc( L7_uchar8 *index, L7_uchar8 *name, 
        L7_BOOL syncFlag, traceRouteCallback callback, void *userHandle, 
        L7_ushort16 *handle );

/*********************************************************************
* @purpose  Frees a Traceroute session
*
* @param    L7_ushort16 handle @b{(input)} Handle for session
*
* @returns  L7_SUCCESS   If session could be freed
* @returns  L7_FAILURE   If address could not be freed
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteFree( L7_ushort16 handle );

/*********************************************************************
* @purpose  Ends a Traceroute session
*
* @param  L7_ushort16 handle @b{(input)} Handle for Traceroute session
*
* @returns  L7_SUCCESS   If session could be end
* @returns  L7_FAILURE   If address could not be end
*
* @notes
*       Fails if session is not allocated
*
* @end
*********************************************************************/
extern L7_RC_t  traceRouteSessionEnd( L7_ushort16 handle );

/*********************************************************************
* @purpose  Queries handle for a Traceroute session 
*
* @param  L7_uchar8   *index  @b{(input)} Field for traceRouteCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(input)} Name of the session
* @param  L7_ushort16 *handle @b{(output)} Handle for the Traceroute session
* @param  L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_ushort16 *vrfId  @b{(output)} VRF Id for the session
* @param  L7_uint32   *ipDa   @b{(output)} IP Address of the destination
* @param  L7_ushort16 *probeSize     @b{(output)} Probe Size
* @param  L7_ushort16 *probePerHop   @b{(output)} Probes per hop
* @param  L7_ushort16 *probeInterval @b{(output)} Interval between probes(secs)
* @param  L7_BOOL     *dontFag       @b{(output)} Dont Fragment Flag
* @param  L7_ushort16 *port          @b{(output)} UDP destination port
* @param  L7_ushort16 *maxTtl        @b{(output)} Max TTL for session
* @param  L7_ushort16 *initTtl       @b{(output)} Init TTL for session
* @param  L7_ushort16 *maxFail       @b{(output)} Max failures allowed in session

* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*       Cannot query sessions created with index = NULL or name = NULL
*       The application will have to store the handle in such case
*
* @end
*********************************************************************/
L7_RC_t traceRouteResultGet( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_ushort16 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail);

/*********************************************************************
 * @purpose  Queries handle for a Traceroute session
 *
 * @param  L7_uchar8   *index  @b{(input)}  Field for traceRouteCtlOwnerIndex
 * @param  L7_uchar8   *name   @b{(input)}  Name of the session
 * @param  L7_ushort16 *handle @b{(output)} Handle for the session.
 *
 * @returns  L7_SUCCESS   If session could be found
 * @returns  L7_FAILURE   If session could not be found
 *
 * @notes
 *       Cannot query sessions created with index = NULL or name = NULL
 *       The application will have to store the handle in such case
 *
 * @end
 *********************************************************************/
extern  L7_RC_t traceRouteHandleGet( L7_uchar8 *index, L7_uchar8 *name,
                                     L7_ushort16 *handle);

/*********************************************************************
 * @purpose  Get next Traceroute session allocated
 *
 * @param  L7_uchar8  *index @b{(inout)} Field for traceRouteCtlOwnerIndex
 * @param  L7_uchar8  *name  @b{(inout)} Name of the session
 * @param  L7_ushort16 *handle @b{(inout)} Handle for the session
 *
 * @returns  L7_SUCCESS   If any session could be found
 * @returns  L7_FAILURE   If no session could be found
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern  L7_RC_t traceRouteSessionHandleGetNext( L7_uchar8 *index, L7_uchar8 *name,
                                                L7_ushort16 *handle);


/*********************************************************************
* @purpose  Provide destination address for the session
*
* @param  L7_ushort16 handle   @b{(input)} Handle for Traceroute session
* @parame L7_uint32   ipDa     @b{(input)} IP Destination address
*
* @returns  L7_SUCCESS   If address could be updated
* @returns  L7_FAILURE   If address could not be updated
*
* @notes
*       Fails if session is not allocated or already started
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteDestAddrSet( L7_ushort16 handle, L7_uint32 ipDa );

/*********************************************************************
* @purpose Get the destination address for the session  
*
* @param  L7_ushort16 handle   @b{(input)} Handle for Traceroute session
* @param  L7_uint32   ipDa   @b{(output)} IP Address of the destination
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteDestAddrGet(L7_ushort16 handle,L7_uint32 *ipDa);

/*********************************************************************
* @purpose  Provide VR-ID for the session
*
* @param    L7_ushort16 handle @b{(input)} Handle for session
* @parame   L7_ushort16 vrId   @b{(input)} VR-ID for the session

* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or already started
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteVrIdSet( L7_ushort16 handle, L7_ushort16 vrId );

/*********************************************************************
* @purpose  To set number of probes per hop for session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 count  @b{(input)} Number of probes to send per hop
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteProbePerHopSet( L7_ushort16 handle, 
        L7_ushort16 count );

/*********************************************************************
* @purpose  To set probes interval for a session
*
* @param  L7_ushort16 handle   @b{(input)} Handle for session
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
extern L7_RC_t traceRouteProbeIntervalSet( L7_ushort16 handle, 
        L7_ushort16 interval );

/*********************************************************************
* @purpose  To set size of PDUs in probe request
*
* @param  L7_ushort16 handle @b{(input)} Handle for traceroute session
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
extern L7_RC_t traceRouteProbeSizeSet( L7_ushort16 handle, L7_ushort16 size );

/*********************************************************************
* @purpose  To set destination port in UDP packet
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 port   @b{(input)} Destination port in UDP packet
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteDestPortSet(L7_ushort16 handle, L7_ushort16 port);

/*********************************************************************
* @purpose  To set Max TTL for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 maxTtl @b{(input)} Max TTL for the session
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteMaxTtlSet( L7_ushort16 handle, L7_ushort16 maxTtl );

/*********************************************************************
* @purpose  To set Inital TTL for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 maxTtl @b{(input)} Initial TTL for the session
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteInitTtlSet( L7_ushort16 handle, L7_ushort16 initTtl );

/*********************************************************************
* @purpose  To set max failures for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_ushort16 maxFail @b{(input)} Max failures for the session
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteMaxFailSet( L7_ushort16 handle, L7_ushort16 maxFail );

/*********************************************************************
* @purpose  To set Dont Fragment for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @param  L7_BOOL     dfFlag @b{(input)} Dont Fragment flag 
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteDontFragSet( L7_ushort16 handle, L7_BOOL dfFlag);
        
/*********************************************************************
* @purpose  To start a traceroute session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
*       If the session mode is SYNC, then complete operation is executed before
*       returning from the API. Also, response will be returned in resp.
*
*       If the mode is ASYNC, then it will initiate the session and return.
*       The response must be queried later with traceRouteQuery API.
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteStart( L7_ushort16 handle );
        
/*********************************************************************
* @purpose  Get first Traceroute session allocated
*
* @param  L7_uchar8   *index  @b{(output)} Field for traceRouteCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(output)} Name of the session
* @param  L7_ushort16 *handle @b{(output)} Handle for the Traceroute session
* @param  L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_ushort16 *vrfId  @b{(output)} VRF Id for the session
* @param  L7_uint32   *ipDa   @b{(output)} IP Address of the destination
* @param  L7_ushort16 *probeSize     @b{(output)} Probe Size
* @param  L7_ushort16 *probePerHop   @b{(output)} Probes per hop
* @param  L7_ushort16 *probeInterval @b{(output)} Probes per hop
* @param  L7_BOOL     *dontFag       @b{(output)} Dont Fragment Flag
* @param  L7_ushort16 *port          @b{(output)} UDP destination port
* @param  L7_ushort16 *maxTtl        @b{(output)} Max TTL for session
* @param  L7_ushort16 *initTtl       @b{(output)} Init TTL for session
* @param  L7_ushort16 *maxFail       @b{(output)} Max failures allowed in session
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteGetFirst(L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_ushort16 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail);

/*********************************************************************
* @purpose  Get first Traceroute session allocated
*
* @param  L7_uchar8  *index @b{(inout)} Field for traceRouteCtlOwnerIndex
* @param  L7_uchar8  *name  @b{(inout)} Name of the session
* @param  L7_ushort16 *handle @b{(inout)} Handle for the session
* @param  L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_ushort16 *vrfId  @b{(output)} VRF Id for the session
* @param  L7_uint32   *ipDa   @b{(output)} IP Address of the destination
* @param  L7_ushort16 *probeSize     @b{(output)} Probe Size
* @param  L7_ushort16 *probePerHop   @b{(output)} Probes per hop
* @param  L7_ushort16 *probeInterval @b{(output)} Interval between probes(secs)
* @param  L7_BOOL     *dontFag       @b{(output)} Dont Fragment Flag
* @param  L7_ushort16 *port          @b{(output)} UDP destination port
* @param  L7_ushort16 *maxTtl        @b{(output)} Max TTL for session
* @param  L7_ushort16 *initTtl       @b{(output)} Init TTL for session
* @param  L7_ushort16 *maxFail       @b{(output)} Max failures allowed in session
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteGetNext( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_ushort16 *vrfId,
        L7_uint32 *ipDa, L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail);

/*********************************************************************
* @purpose  Queries a traceroute session
*
* @param L7_ushort16 handle          @b{(input)}  Handle for the session
* @param L7_BOOL     *operStatus     @b{(output)} Operational status
* @param L7_ushort16 *curTtl         @b{(output)} Current TTL being used
* @param L7_ushort16 *currHopCount   @b{(output)} Current hop being processed
* @param L7_ushort16 *currProbeCount @b{(output)} Current probe number for the 
*           hop being processed
* @param L7_ushort16 *testAttempt    @b{(output)} Current probe number for the 
*           hop being processed
* @param L7_ushort16 *testSuccess    @b{(output)} Current probe number for the 
*           hop being processed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteQuery( L7_ushort16 handle, L7_BOOL *operStatus, 
        L7_ushort16 *currTtl, L7_ushort16 *currHopCount, 
        L7_ushort16 *currProbeCount, L7_ushort16 *testAttempt, 
        L7_ushort16 *testSuccess );

/*********************************************************************
* @purpose  Queries next hop in a given traceroute session
*
* @param L7_ushort16 handle    @b{(input)}  Handle for the traceroute session
* @param L7_ushort16 *hopIndex  @b{(output)} Index of first hop
* @param L7_ushort16 *ttl       @b{(output)} TTL for the hop
* @param L7_uint32   *hopDa     @b{(output)} IP Address of the hop
* @param L7_uint32   *minRtt    @b{(output)} Min RTT for the hop
* @param L7_uint32   *maxRtt    @b{(output)} Max RTT for the hop
* @param L7_uint32   *avgRtt    @b{(output)} Avg RTT for the hop
* @param L7_ushort16 *probeSent @b{(output)} Probes sent for the hop
* @param L7_ushort16 *probeRcvd @b{(output)} Probes responses received for the hop
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteHopGetFirst( L7_ushort16 handle, 
        L7_ushort16 *hopIndex, L7_ushort16 *ttl, L7_uint32 *hopDa, 
        L7_uint32 *minRtt, L7_uint32 *maxRtt, L7_uint32 *avgRtt, 
        L7_ushort16 *probeSent, L7_ushort16 *probeRecvd );

/*********************************************************************
* @purpose  Queries next hop in a traceroute session
*
* @param L7_ushort16 handle     @b{(input)}  Handle for the session
* @param L7_ushort16 *hopIndex  @b{(inout)} Index of first hop
* @param L7_ushort16 *ttl       @b{(output)} TTL for the hop
* @param L7_uint32   *hopDa     @b{(output)} IP Address of the hop
* @param L7_uint32   *minRtt    @b{(output)} Min RTT for the hop
* @param L7_uint32   *maxRtt    @b{(output)} Max RTT for the hop
* @param L7_uint32   *avgRtt    @b{(output)} Avg RTT for the hop
* @param L7_ushort16 *probeSent @b{(output)} Probes sent for the hop
* @param L7_ushort16 *probeRcvd @b{(output)} Probe responses received for 
*               the hop
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteHopGetNext( L7_ushort16 handle, 
        L7_ushort16 *hopIndex, L7_ushort16 *ttl, L7_uint32 *hopDa, 
        L7_uint32 *minRtt, L7_uint32 *maxRtt, L7_uint32 *avgRtt, 
        L7_ushort16 *probeSent, L7_ushort16 *probeRecvd );

/*********************************************************************
* @purpose  Queries a specific hop in a traceroute session
*
* @param L7_ushort16 handle     @b{(input)}  Handle for the session
* @param L7_ushort16 hopIndex   @b{(inout)}  Hop to be queried
* @param L7_ushort16 *ttl       @b{(output)} TTL for the hop
* @param L7_uint32   *hopDa     @b{(output)} IP Address of the hop
* @param L7_uint32   *minRtt    @b{(output)} Min RTT for the hop
* @param L7_uint32   *maxRtt    @b{(output)} Max RTT for the hop
* @param L7_uint32   *avgRtt    @b{(output)} Avg RTT for the hop
* @param L7_ushort16 *probeSent @b{(output)} Probes sent for the hop
* @param L7_ushort16 *probeRcvd @b{(output)} Probe responses received for 
*               the hop
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t traceRouteHopGet( L7_ushort16 handle, 
        L7_ushort16 hopIndex, L7_ushort16 *ttl, L7_uint32 *hopDa, 
        L7_uint32 *minRtt, L7_uint32 *maxRtt, L7_uint32 *avgRtt, 
        L7_ushort16 *probeSent, L7_ushort16 *probeRecvd );

/*********************************************************************
* @purpose  Get Maximum number of traceroute sessions allowed
*
* @param  L7_uint32  *maxSessions @b{(output)} max no of concurrent traceroute requests
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
extern traceRouteMaxSessionsGet(L7_uint32 * maxSessions);
#endif /* TRACEROUTE_MAP_API_H */
