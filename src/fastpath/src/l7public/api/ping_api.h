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
#ifndef PING_API_H
#define PING_API_H

/*********************************************************************
*
* @purpose  Phase-1 Init of ping application
*
* @param    None
*
* @returns  Void
*
* @notes    Allocates memory for ping session
*
* @end
*********************************************************************/
L7_RC_t pingCnfgrInitPhase1Process( void );

/*********************************************************************
*
* @purpose  Phase-3 Init of ping application
*
* @param    None
*
* @returns  Void
*
* @notes    Initializes the ping session table
*
* @end
*********************************************************************/
L7_RC_t pingCnfgrInitPhase3Process( void );

/*********************************************************************
*
* @purpose  Phase-2 De-Initiates of ping application
*
* @param    None
*
* @returns  Void
*
* @notes    Stops all active sessions
*
* @end
*********************************************************************/
L7_RC_t pingCnfgrUconfigPhase2Process( void );

/*********************************************************************
* @purpose  Allocates a ping session 
*
* @param  L7_uchar8 *index  @b{(input)} Field for pingCtlOwnerIndex
* @param  L7_uchar8 *name   @b{(input)} Name of the session
* @param  L7_BOOL syncFlag  @b{(input)} Operation mode (TRUE=Sync, FALSE=ASync)
* @param  void    *callback @b{(input)} Callback function called for each probe 
*               response. Pass NULL if callback is not needed.
* @param  void       *finishCallback @b{(input)} Callback function called after
*                                                completing the ping operation
* @param  L7_ushort16 *userHandle @b{(input)} User parameter passed in callback.
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
*       passed, then session cannot be queried using pingSessionGet.
*       The application will have to store the handle in such case
*
*       The handle returned must be given in further calls to ping APIs
*       Ping application will pass back userHandle in callback routine
*
*       In either case, the API pingSessionFree must be called to delete 
*       the session.
*
*       At most PING_MAX_SESSIONS simultaneous sessions are supported.
*       The calling API must call pingSessionFree to free a session.
* @end
*********************************************************************/
typedef L7_RC_t (*pingSessionCallback)(void *userParam, L7_ushort16 seqNum, 
        L7_uint32 rtt, L7_uint32 type, L7_uchar8 *src_ipAddress );
typedef L7_RC_t (*pingSessionFinishCallback)(void *userParam);
extern L7_RC_t pingSessionAlloc( L7_uchar8 *index, L7_uchar8 *name, 
        L7_BOOL syncFlag, pingSessionCallback *callback, pingSessionFinishCallback *finishCallback,
        void *userHandle, L7_ushort16 *handle );

/*********************************************************************
*
* @purpose  Frees a ping session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
*
* @returns  L7_SUCCESS   If session could be freed
* @returns  L7_FAILURE   If address could not be freed
*
* @notes
*       Fails if session is not allocated
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionFree( L7_ushort16 handle );

/*********************************************************************
*
* @purpose  Ends a ping session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
*
* @returns  L7_SUCCESS   If session could be end
* @returns  L7_FAILURE   If address could not be end
*
* @notes
*       Fails if session is not allocated
* @end
*********************************************************************/
extern L7_RC_t pingSessionEnd( L7_ushort16 handle );

/*********************************************************************
* @purpose  Provide destination address for the session
*
* @param  L7_ushort16 handle @b{(input)} Handle for session
* @parame L7_uint32 ipDa     @b{(input)} IP Destination address

* @returns  L7_SUCCESS   If address could be updated
* @returns  L7_FAILURE   If address could not be updated
*
* @notes
*       Fails if session is not allocated or already started
*
* @end
*********************************************************************/
extern L7_RC_t pingDestAddrSet( L7_ushort16 handle, L7_uint32 ipDa );

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
extern L7_RC_t pingVrIdSet( L7_ushort16 handle, L7_ushort16 vrId );

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
extern L7_RC_t pingProbeCountSet( L7_ushort16 handle, L7_ushort16 count );

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
extern L7_RC_t pingProbeCountGet( L7_ushort16 handle, L7_uint32 *count );
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
extern L7_RC_t pingDSCPSet( L7_ushort16 handle, L7_uint32 dscpVal );
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
extern L7_RC_t pingDSCPGet( L7_ushort16 handle, L7_uint32 *dscpVal );
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
extern L7_RC_t pingProbeIntervalSet(L7_ushort16 handle, L7_ushort16 interval);

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
extern L7_RC_t pingProbeIntervalGet( L7_ushort16 handle, L7_uint32 *interval );

/*********************************************************************
* @purpose  To set source IP address of a ping
*
* @param  handle    @b{(input)} Handle for ping session
* @param  srcIpAddr @b{(input)} Source IP address
*
* @returns  L7_SUCCESS   If session update is success
* @returns  L7_FAILURE   If session update fails
*
* @notes
*       Fails if session is not allocated or session already started.
*
* @end
*********************************************************************/
extern L7_RC_t pingProbeSrcIpAddrSet(L7_ushort16 handle, L7_uint32 srcIpAddr);

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
extern L7_RC_t pingProbeSizeSet( L7_ushort16 handle, L7_ushort16 size );

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
extern L7_RC_t pingProbeSizeGet( L7_ushort16 handle, L7_uint32 *size );

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
*       If the mode is SYNC, then complete operation is executed before
*       returning from the API. Also, response will be returned in resp.
*
*       If the mode is ASYNC, then it will initiate the session and return.
*       The response must be queried later with pingSessionQuery API.
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionStart( L7_ushort16 handle );

/*********************************************************************
* @purpose  Queries a ping session
*
* @param  L7_ushort16 *handle       @b{(input)}   Handle for the ping session
* @param  L7_BOOL     *operStatus   @b{(output)}  Operational status of session
* @param  L7_ushort16 *probeSent    @b{(output)}  Number of probes sent
* @param  L7_ushort16 *probeSuccess @b{(output)}  Number of probes succeeded
* @param  L7_ushort16 *probeFail    @b{(output)}  Number of probes failed
* @param  L7_uint32   *minRtt       @b{(output)}  Minimum RTT
* @param  L7_uint32   *maxRtt       @b{(output)}  Max RTT
* @param  L7_uint32   *avgRtt       @b{(output)}  Avg RTT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionQuery( L7_ushort16 handle, L7_BOOL *operStatus,
        L7_ushort16 *probeSent, L7_ushort16 *probeSuccess, 
        L7_ushort16 *probeFail, L7_uint32 *minRtt, L7_uint32 *maxRtt, 
        L7_uint32 *avgRtt );

/*********************************************************************
* @purpose  Queries a ping session
*
* @param L7_ushort16 handle       @b{(input)}   Handle for session
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionQueryTargetAddrs( L7_ushort16 handle, L7_uint32 *ipDa);

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
extern L7_RC_t pingSessionQueryOperStatus( L7_ushort16 handle, L7_BOOL *operStatus);
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
extern L7_RC_t pingSessionQuerySessionState( L7_ushort16 handle, L7_uint32 *operState);
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
extern L7_RC_t pingSessionQueryMinRtt( L7_ushort16 handle, L7_uint32 *minRtt);

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
extern L7_RC_t pingSessionQueryMaxRtt( L7_ushort16 handle, L7_uint32 *maxRtt);

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
extern L7_RC_t pingSessionQueryAvgRtt( L7_ushort16 handle, L7_uint32 *avgRtt);

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
extern L7_RC_t  pingSessionQueryProbeSent(L7_ushort16 handle, L7_uint32 *probeSent);

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
extern L7_RC_t pingSessionQueryProbeResponse( L7_ushort16 handle, L7_uint32 *probeResponse);
/*********************************************************************
* @purpose  Queries a ping session to get handle and request parameters
*
* @param  L7_uchar8   *index  @b{(input)}  Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(input)}  Name of the session
* @param  L7_ushort16 *handle @b{(output)} Handle for the ping session started.
* @parame L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_uint32   *vrfId  @b{(output)) VRF-ID of the session
* @param  L7_uint32   *ipDa   @b{(output)) IP-DA of the session
* @param  L7_ushort16 *count  @b{(output)) Number of probe request to send
* @param  L7_ushort16 *size   @b{(output)) Size of the probe request
* @param  L7_ushort16 *interval @b{(output)) Interval between probes
*
* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*       Cannot query sessions created with index = NULL or name = NULL
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionHandleGet( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_uint32 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *count, L7_ushort16 *size, 
        L7_ushort16 *interval );
/*********************************************************************
* @purpose  Queries a ping session to get handle and request parameters
*
* @param  L7_uchar8   *index  @b{(input)}  Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(input)}  Name of the session
* @param  L7_ushort16 *handle @b{(output)} Handle for the ping session started.
*
* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*       Cannot query sessions created with index = NULL or name = NULL
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionHandleFromIndexTestNameGet( L7_uchar8 *index,
        L7_uchar8 *name, L7_ushort16 *handle);
/*********************************************************************
* @purpose  Get first ping session allocated
*
* @param  L7_uchar8   *index      @b{(output)} Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name       @b{(output)} Name of the session
* @param  L7_ushort16 *handle     @b{(output)} Handle for the ping session
* @parame L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_uint32   *vrfId      @b{(output)) VRF-ID of the session
* @param  L7_uint32   *ipDa       @b{(output)) IP-DA of the session
* @param  L7_ushort16 *count      @b{(output)) Number of probe to send
* @param  L7_ushort16 *size       @b{(output)) Size of the probes
* @param  L7_ushort16 *interval   @b{(output)) Interval between probes
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionGetFirst ( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_uint32 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *count, L7_ushort16 *size, 
        L7_ushort16 *interval );

/*********************************************************************
* @purpose  Get next ping session allocated
*
* @param  L7_uchar8   *index      @b{(inout)} Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name       @b{(inout)} Name of the session
* @param  L7_ushort16 *handle     @b{(inout)} Handle for the ping session
* @parame L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_uint32   *vrfId      @b{(output)) VRF-ID of the session
* @param  L7_uint32   *ipDa       @b{(output)) IP-DA of the session
* @param  L7_ushort16 *count      @b{(output)) Number of probes to send
* @param  L7_ushort16 *size       @b{(output)) Size of the probes
* @param  L7_ushort16 *interval   @b{(output)) Interval between probes
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionGetNext( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_uint32 *vrfId
        , L7_uint32 *ipDa, L7_ushort16 *count, L7_ushort16 *size, 
        L7_ushort16 *interval );

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
extern L7_RC_t pingGetNextHandle( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle );

/*********************************************************************
* @purpose  Get Maximum number of ping  sessions allowed
*
* @param  L7_uint32  *maxSessions @b{(output)} maximum number of concurrent
*                                              ping requests
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t pingMaxSessionsGet(L7_uint32 *maxSessions);

/*********************************************************************
* @purpose  Flushes all entries that are in DONE state.
*
* @param    None
*
* @returns  L7_SUCCESS, if success
*
* @notes    This API is provided only for debugging purpose. Can be handy
*           if ping mis-behaves and there are entries in DONE state.
*
* @end
*********************************************************************/
extern L7_RC_t pingSessionFlushDone( void );

#endif /* PING_API_H */
