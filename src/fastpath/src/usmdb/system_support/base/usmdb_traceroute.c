/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  usmdb_traceroute.c
*
* @purpose   Provide interface to API's for traceroute UI components
*
* @component Traceroute
*
* @comments  This file provides usmdb API for traceroute, which will
*            be called by User Interface.
*
* @create    05/21/2004
*
* @author    ssharma
*
* @end
*       Following are 2 example sequence of operations for traceroute
*       Option 1.
*           1. Allocate an entry by calling usmDbTraceRouteAlloc
*           2. Configure the session by calling usmDbTraceRouteDestAddrSet etc..
*           3. Call usmDbTraceRouteStart() to start the traceroute session
*           4. Query the result by calling usmDbTraceRouteResultGet
*           5. Free the session by calling usmDbTraceRouteFree
*       Option 2.
*           1. Call usmDbTraceRouteStart() to start TraceRoute session 
*              with all required params
*           2. Query the result by calling usmDbTraceRouteResultGet
*           3. Free the session by calling usmDbTraceRouteFree
*
*
**********************************************************************/
#include "osapi.h"
#include "osapi_support.h"
#include "string.h"
#include "l7_common.h"
#include "traceroute_map_api.h"
#include "usmdb_traceroute_api.h"

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
*       passed, then session cannot be queried using usmDbTraceRouteGet.
*       The application will have to store the handle in such case
*
*       The handle returned must be given in further calls to traceroute APIs
*       Traceroute application will pass back userHandle in callback routine
*
*       In either case, the API usmDbTraceRouteFree must be called to delete 
*       the session.
*
*       At most TRACEROUTE_MAX_SESSIONS simultaneous sessions are supported.
*
* @end
*********************************************************************/
L7_RC_t 
usmDbTraceRouteAlloc( L7_uchar8 *index, L7_uchar8 *name, L7_BOOL syncFlag, 
        void *callback, void *userHandle, L7_ushort16 *handle )
{
    return traceRouteAlloc( index, name, syncFlag, callback, userHandle, 
            handle );
}

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
L7_RC_t 
usmDbTraceRouteFree( L7_ushort16 handle )
{
    return traceRouteFree( handle );
}

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
L7_RC_t usmDbTraceRouteSessionEnd( L7_ushort16 handle )
{
  return traceRouteSessionEnd( handle );
}

/*********************************************************************
* @purpose  Queries handle for a Traceroute session 
*
* @param  L7_uchar8   *index  @b{(input)}  Field for traceRouteCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(input)}  Name of the session
* @param  L7_ushort16 *handle @b{(output)} Handle for the session.
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
* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*       Cannot query sessions created with index = NULL or name = NULL
*       The application will have to store the handle in such case
*
* @end
*********************************************************************/
L7_RC_t 
usmDbTraceRouteResultGet( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_ushort16 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    return traceRouteResultGet( index, name, handle, operStatus, vrfId, ipDa, 
            probeSize, probePerHop, probeInterval, dontFrag, port, 
            maxTtl, initTtl, maxFail );
}

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
L7_RC_t
usmDbTraceRouteHandleGet( L7_uchar8 *index, L7_uchar8 *name,
            L7_ushort16 *handle)
{
   return traceRouteHandleGet(index, name, handle);
}

/*********************************************************************
* @purpose  Queries next handle for a Traceroute session
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
L7_RC_t
usmDbTraceRouteSessionHandleGetNext( L7_uchar8 *index, L7_uchar8 *name,
                L7_ushort16 *handle)
{
     return traceRouteSessionHandleGetNext(index, name, handle);
}

/*********************************************************************
* @purpose  Provide destination address for the session
*
* @param  L7_ushort16 handle   @b{(input)} Handle for Traceroute session
* @parame L7_uint32   ipDa     @b{(input)} IP Destination address

* @returns  L7_SUCCESS   If address could be updated
* @returns  L7_FAILURE   If address could not be updated
*
* @notes
*       Fails if session is not allocated or already started
*
* @end
*********************************************************************/
L7_RC_t 
usmDbTraceRouteDestAddrSet( L7_ushort16 handle, L7_uint32 ipDa )
{
    return traceRouteDestAddrSet( handle, ipDa );
}

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
L7_RC_t 
usmDbTraceRouteVrIdSet( L7_ushort16 handle, L7_ushort16 vrId )
{
    return traceRouteVrIdSet( handle, vrId );
}

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
L7_RC_t 
usmDbTraceRouteProbePerHopSet( L7_ushort16 handle, L7_ushort16 count )
{
    return traceRouteProbePerHopSet( handle, count );
}

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
L7_RC_t 
usmDbTraceRouteProbeIntervalSet( L7_ushort16 handle, L7_ushort16 interval )
{
    return traceRouteProbeIntervalSet( handle, interval );
}

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
L7_RC_t 
usmDbTraceRouteProbeSizeSet( L7_ushort16 handle, L7_ushort16 size )
{
    return traceRouteProbeSizeSet( handle, size );
}

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
L7_RC_t 
usmDbTraceRouteDestPortSet(L7_ushort16 handle, L7_ushort16 port)
{
    return traceRouteDestPortSet( handle, port );
}

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
L7_RC_t 
usmDbTraceRouteMaxTtlSet( L7_ushort16 handle, L7_ushort16 maxTtl )
{
    return traceRouteMaxTtlSet( handle, maxTtl );
}

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
L7_RC_t 
usmDbTraceRouteInitTtlSet( L7_ushort16 handle, L7_ushort16 initTtl )
{
    return traceRouteInitTtlSet( handle, initTtl );
}

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
L7_RC_t 
usmDbTraceRouteMaxFailSet( L7_ushort16 handle, L7_ushort16 maxFail )
{
    return traceRouteMaxFailSet( handle, maxFail );
}

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
L7_RC_t 
usmDbTraceRouteDontFragSet( L7_ushort16 handle, L7_BOOL dfFlag )
{
    return traceRouteDontFragSet( handle, dfFlag );
}
        
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
* @end
*********************************************************************/
L7_RC_t 
usmDbTraceRouteStart( L7_ushort16 handle )
{
    return traceRouteStart( handle );
}
        
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
L7_RC_t 
usmDbTraceRouteGetFirst(L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle,
        L7_BOOL *operStatus, L7_ushort16 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    return traceRouteGetFirst( index, name, handle, operStatus, vrfId, ipDa, 
            probeSize, probePerHop, probeInterval, dontFrag, port, 
            maxTtl, initTtl, maxFail );
}

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
L7_RC_t 
usmDbTraceRouteGetNext( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle,
        L7_BOOL *operStatus, L7_ushort16 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    return traceRouteGetNext( index, name, handle, operStatus, vrfId, ipDa, 
            probeSize, probePerHop, probeInterval, dontFrag, port, 
            maxTtl, initTtl, maxFail );
}

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
L7_RC_t
usmDbTraceRouteDestAddrGet(L7_ushort16 handle,L7_uint32 *ipDa)
{
  return traceRouteDestAddrGet(handle,ipDa);
}

/*********************************************************************
* @purpose  Execute a traceroute session
*
* @param  L7_uchar8  *index @b{(input)} Field for traceRouteCtlOwnerIndex
* @param  L7_uchar8  *name  @b{(input)} Name of the session
* @param  L7_BOOL     syncFlag @b{(input)} Operation mode(TRUE=Sync, FALSE=ASync)
* @param  L7_ushort16 vrfId  @b{(input)} VRF Id for the session
* @param  L7_uint32   ipDa   @b{(input)} IP Address of the destination
* @param  L7_ushort16 probeSize     @b{(input)} Probe Size
* @param  L7_ushort16 probePerHop   @b{(input)} Probes per hop
* @param  L7_ushort16 probeInterval @b{(output)} Probes per hop
* @param  L7_BOOL     dontFrag  @b{(input)} Dont Fragment Flag
* @param  L7_ushort16 port      @b{(input)} UDP destination port
* @param  L7_ushort16 maxTtl    @b{(input)} Max TTL for session
* @param  L7_ushort16 initTtl   @b{(input)} Init TTL for session
* @param  L7_ushort16 maxFail   @b{(input)} Max failures allowed in session
* @param  void        *callback  @b{(input)}  Callback function called for 
*           each probe response. Pass NULL if callback is not needed.
* @param  void        *userParam @b{(input)}  User parameter passed in callback.
* @param  L7_ushort16 *handle    @b{(output)} Handle for the session started.
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
*       Only index and name are index into the session table.
*
*       index = NULL or name = NULL are reserved. If these values are 
*       passed, u cannot do a usmDbTraceRouteGet on session later with 
*       these values. The application will have to store the handle in 
*       such case
* @end
*********************************************************************/

L7_RC_t 
usmDbTraceRoute( L7_uchar8 *index, L7_uchar8 *name, L7_BOOL syncFlag, 
        L7_ushort16 vrfId, L7_uint32 ipDa, L7_ushort16 probeSize,
        L7_ushort16 probePerHop, L7_uint32 probeInterval, L7_BOOL dontFrag,
        L7_ushort16 port, L7_ushort16 maxTtl, L7_ushort16 initTtl, 
        L7_ushort16 maxFail, void *callback, void *userParam, 
        L7_ushort16 *handle )
{
    L7_RC_t rc;

    if ((rc = traceRouteAlloc( index, name, syncFlag, callback, userParam, 
            handle )) != L7_SUCCESS )
        return rc;

    /* Single iteration loop to simplify handling error cases */
    do 
    {

        if (( rc = traceRouteVrIdSet( *handle, vrfId ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteDestAddrSet( *handle, ipDa ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteProbeSizeSet( *handle, probeSize ) )!=L7_SUCCESS)
        {
            break;
        }

        if (( rc = traceRouteProbePerHopSet( *handle, probePerHop ) ) 
                != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteProbeIntervalSet( *handle, probeInterval ) ) 
                != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteDontFragSet( *handle, dontFrag ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteDestPortSet( *handle, port ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteMaxTtlSet( *handle, maxTtl ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteInitTtlSet( *handle, initTtl ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteMaxFailSet( *handle, maxFail ) ) != L7_SUCCESS )
        {
            break;
        }

        if (( rc = traceRouteStart( *handle ) ) != L7_SUCCESS )
        {
            break;
        }

    } while ( 0 );

    /* Close session if there was any error */
    if ( rc != L7_SUCCESS )
    {
        traceRouteFree( *handle );
    }

    return rc;
}

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
L7_RC_t 
usmDbTraceRouteQuery( L7_ushort16 handle, L7_BOOL *operStatus, 
        L7_ushort16 *currTtl, L7_ushort16 *currHopCount, 
        L7_ushort16 *currProbeCount, L7_ushort16 *testAttempt, 
        L7_ushort16 *testSuccess )
{
    return traceRouteQuery( handle, operStatus, currTtl, currHopCount,
            currProbeCount, testAttempt, testSuccess );
}

/*********************************************************************
* @purpose  Queries first hop in a given traceroute session
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
L7_RC_t 
usmDbTraceRouteHopGetFirst( L7_ushort16 handle, L7_ushort16 *hopIndex, 
        L7_ushort16 *ttl, L7_uint32 *hopDa, L7_uint32 *minRtt, 
        L7_uint32 *maxRtt, L7_uint32 *avgRtt, L7_ushort16 *probeSent, 
        L7_ushort16 *probeRecvd )
{
    return traceRouteHopGetFirst( handle, hopIndex, ttl, hopDa, minRtt, maxRtt, 
            avgRtt, probeSent, probeRecvd );
}

/*********************************************************************
* @purpose  Queries next hop in a traceroute session
*
* @param L7_ushort16 handle    @b{(inout)}  Handle for the session
* @param L7_ushort16 *hopIndex  @b{(output)} Index of first hop
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
L7_RC_t 
usmDbTraceRouteHopGetNext( L7_ushort16 handle, L7_ushort16 *hopIndex, 
        L7_ushort16 *ttl, L7_uint32 *hopDa, L7_uint32 *minRtt, 
        L7_uint32 *maxRtt, L7_uint32 *avgRtt, L7_ushort16 *probeSent, 
        L7_ushort16 *probeRecvd )
{
    return traceRouteHopGetNext( handle, hopIndex, ttl, hopDa, minRtt, maxRtt, 
            avgRtt, probeSent, probeRecvd );
}

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
L7_RC_t 
usmDbTraceRouteHopGet( L7_ushort16 handle, L7_ushort16 hopIndex, 
        L7_ushort16 *ttl, L7_uint32 *hopDa, L7_uint32 *minRtt, 
        L7_uint32 *maxRtt, L7_uint32 *avgRtt, L7_ushort16 *probeSent, 
        L7_ushort16 *probeRecvd )
{

    return traceRouteHopGet( handle, hopIndex, ttl, hopDa, minRtt, maxRtt, 
            avgRtt, probeSent, probeRecvd );
}

/*********************************************************************
* @purpose  Get Maximum number of traceroute sessions allowed
*
* @param  L7_uint32  *maxSessions @b{(output)} max no of concurrent traceroute requests
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbTraceRouteMaxSessionsGet(L7_uint32 * maxSessions)
{
  return traceRouteMaxSessionsGet(maxSessions);
}  

