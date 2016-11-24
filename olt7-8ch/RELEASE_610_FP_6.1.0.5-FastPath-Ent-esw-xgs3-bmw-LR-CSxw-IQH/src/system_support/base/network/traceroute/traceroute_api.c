/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  traceroute_api.c
*
* @purpose   Provide interface to API's for traceroute UI components
*
* @component Traceroute
*
* @comments  This file provides usmdb API for traceroute, which will
*            be called by User Interface.
*
* @create    08/16/2005
*
* @author    Praveen K V
*
* @end
*       Following are 2 example sequence of operations for traceroute
*       Option 1.
*           1. Allocate an entry by calling traceRouteAlloc
*           2. Configure the session by calling traceRouteDestAddrSet etc..
*           3. Call traceRouteStart() to start the traceroute session
*           4. Query the result by calling traceRouteResultGet
*           5. Free the session by calling traceRouteFree
*       Option 2.
*           1. Call traceRouteStart() to start TraceRoute session 
*              with all required params
*           2. Query the result by calling traceRouteResultGet
*           3. Free the session by calling traceRouteFree
*
*
**********************************************************************/
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "string.h"
#include "l3_commdefs.h"
#include "traceroute.h"
#include "traceroute_map_api.h"

/* Extern definitions */

/* Table containing traceroute sessions */
extern traceRouteTable_t   *traceRouteTable_g;

/* Lock to control access to traceRouteTable_g*/
extern osapiRWLock_t        traceRouteLock_g;

/*********************************************************************
* @purpose  Returns pointer to session table for given handle
*
* @param  L7_ushort16 *handle @b{(input)} Handle for the session
* @param  traceRouteTable_t **entry @b{(output)} Will contain address of entry
*
* @returns  L7_SUCCESS   If session could be found
* @returns  L7_FAILURE   If session could not be found
*
* @notes
*   OperState is UP(TRUE) if state of session is IN_PROGRESS else its
*   down(FALSE)
*
* @end
*********************************************************************/
L7_RC_t
traceRouteHandleToEntry( L7_ushort16 handle, traceRouteTable_t **entry )
{
    L7_uint32 index = handle & TRACEROUTE_INDEX_MASK;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);
    if ( index >= TRACEROUTE_MAX_SESSIONS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        return L7_FAILURE;
    }

    traceRouteGetEntry( index, entry );

    if ( (*entry)->handle != handle || 
            (*entry)->state == TRACEROUTE_STATE_FREE )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copies the fields needed for traceRouteCtlEntry from session entry
*
* @param  traceRouteTable_t *entry @b{(input)} Address of the entry
* @param  L7_ushort16 *handle @b{(output)} Handle for the Traceroute session
* @param  L7_BOOL     *operStatus @b{(output)} Operational status
* @param  L7_ushort16 *vrfId  @b{(output)} VRF Id for the session
* @param  L7_uint32   *ipDa   @b{(output)} IP Address of the destination
* @param  L7_ushort16 *probeSize     @b{(output)} Probe Size
* @param  L7_ushort16 *probePerHop   @b{(output)} Probes per hop
* @param  L7_ushort16 *probeInterval @b{(output)} Probes per hop in secs
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
*   OperState is UP(TRUE) if state of session is IN_PROGRESS else its
*   down(FALSE)
*
* @end
*********************************************************************/
void
traceRouteReqCopy( traceRouteTable_t *entry, L7_ushort16 *handle, 
        L7_BOOL *operStatus, L7_ushort16 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    *handle = entry->handle;
    /* Oper Status is TRUE only if session is in progress. */
    if ( entry->state == TRACEROUTE_STATE_IN_PROGRESS)
    {
        *operStatus = L7_TRUE;
    }
    else
    {
        *operStatus = L7_FALSE;
    }

    *handle = entry->handle;
    *vrfId = entry->vrfId;
    *ipDa = entry->ipDa;
    *probeSize = entry->probeSize;
    *probePerHop = entry->probePerHop;
    *probeInterval = entry->probeInterval / TRACEROUTE_INTERVAL_MULTIPLIER;
    *dontFrag = entry->dontFrag;
    *port = entry->port;
    *maxTtl = entry->maxTtl;
    *initTtl = entry->initTtl;
    *maxFail = entry->maxFail;

    return;
}

/*********************************************************************
* @purpose  Copies the response from session entry
*
* @param  traceRouteTable_t *entry    @b{(input)} Address of the entry
* @param  L7_BOOL     *operStatus     @b{(output)} Operational status
* @param  L7_ushort16 *currTtl        @b{(output)} Current TTL being sent
* @param  L7_ushort16 *currHopCount   @b{(output)} Current Hop count processed
* @param  L7_ushort16 *currProbeCount @b{(output)} Current probe count for hop
* @param  L7_ushort16 *testAttempt    @b{(output)} Total probes sent
* @param  L7_ushort16 *testSuccess    @b{(output)} Total successfull probes
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*   OperState is UP(TRUE) if state of session is IN_PROGRESS else its
*   down(FALSE)
*
* @end
*********************************************************************/
void
traceRouteRespCopy( traceRouteTable_t *entry, L7_ushort16 *handle, 
        L7_BOOL *operStatus, L7_ushort16 *currTtl, L7_ushort16 *currHopCount, 
        L7_ushort16 *currProbeCount, L7_ushort16 *testAttempt, 
        L7_ushort16 *testSuccess )
{
    *handle = entry->handle;
    /* Oper Status is TRUE only if session is in progress. */
    if ( entry->state == TRACEROUTE_STATE_IN_PROGRESS)
    {
        *operStatus = L7_TRUE;
    }
    else
    {
        *operStatus = L7_FALSE;
    }

    *currTtl = entry->currTtl;
    *currHopCount = entry->hopCount;
    *currProbeCount = entry->currProbeCount;
    *testAttempt = entry->testAttempts;
    *testSuccess = entry->testSuccess;
    return;
}

/*********************************************************************
* @purpose  Compare two session entries
*
* @param  L7_uchar8 *index1 @b{(input)} Index of first entry
* @param  L7_uchar8 *name1  @b{(input)} Name of first entry
* @param  L7_ushort16 handle1 @b{(input)} Handle of first entry
* @param  L7_uchar8 *index2 @b{(input)} Index of second entry
* @param  L7_uchar8 *name2  @b{(input)} Name of second entry
* @param  L7_ushort16 handle2 @b{(input)} Handle of second entry
*
* @returns  <0  if entry1 < entry2
* @returns   0  if  entry1 == entry2
* @returns  >0  if  entry1 > entry2
*
* @notes
*       Key consists of <index, name, handle> in increasing order
*
* @end
*********************************************************************/
L7_int32
traceRouteCompareKey( L7_uchar8 *index1, L7_uchar8 *name1, L7_ushort16 handle1,
        L7_uchar8 *index2, L7_uchar8 *name2, L7_ushort16 handle2 )
{
    L7_int32 result;

    /* If any of string params are NULL, treat them as blank string */
    if ( index1 == NULL )
        index1 = (L7_uchar8 *)"";

    if ( index2 == NULL )
        index2 = (L7_uchar8 *)"";

    if ( name1 == NULL )
        name1 = (L7_uchar8 *)"";

    if ( name2 == NULL )
        name2 = (L7_uchar8 *)"";

    /* First compare index */
    result = strcmp( (L7_char8 *)index1, (L7_char8 *)index2 );
    if ( result != 0 )
        return result;

    /* Index is same, compare name */
    result = strcmp( (L7_char8 *)name1, (L7_char8 *)name2 );
    if ( result != 0 )
        return result;

    /* Both Index and Name are same - Compare handle */
    return handle1 - handle2;

}

/*********************************************************************
* @purpose  Finds chronologically next entry in traceroute session table
*
* @param  L7_uchar8 *index  @b{(input-ouput)} index of logically next entry
* @param  L7_uchar8 *name   @b{(input-ouput)} name of logically next entry
* @param  L7_ushort16 **handle @b{(inout)} Handle of logically next entry
*
* @returns  L7_SUCCESS   If session could be allocated
* @returns  L7_FAILURE   If session could not be could be allocated
*
* @notes
*
*       Finds the next key in the table. The key consists of (index, name,
*       handle) in that order.
*
*       Any entry in table with key greater than the one in argument is
*       potential next entry. We must choose the least of such entries.
*
*       After each iteration, currEntry holds the entry currently known next
*       entry.
* @end
*********************************************************************/
L7_RC_t
traceRouteGetNextHandle( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle )
{
    L7_uint32 i;
    traceRouteTable_t *currEntry = NULL;
    traceRouteTable_t *entry = NULL;
    L7_RC_t rc = L7_FAILURE;
    L7_int32 compareResult;

    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &entry );
        /* See if entry is allocated or not */
        if ( entry->state == TRACEROUTE_STATE_FREE )
        {
            continue;
        }

        /* First check if the entry is greater than one in parameter */
        if ( traceRouteCompareKey( index, name, *handle, entry->index,
                    entry->name, entry->handle ) < 0 ) 
        {
            /* entry is greater than key given. Now compare if it is less than
             * last known least entry */
            if ( currEntry == NULL ) 
            {
                /* There was no entry earlier, make this as least */
                compareResult = 1;
            }
            else
            {
                compareResult = traceRouteCompareKey( currEntry->index,
                        currEntry->name, currEntry->handle,
                        entry->index, entry->name, entry->handle );
            }
            /* entry is smaller than currEntry, replace it */
            if ( compareResult > 0 )
            {
                currEntry = entry;
                rc = L7_SUCCESS;
            }
        }
    }

    if ( rc == L7_SUCCESS )
    {
        strcpy( (L7_char8 *)index, (L7_char8 *)currEntry->index );
        strcpy( (L7_char8 *)name, (L7_char8 *)currEntry->name );
        *handle = currEntry->handle;
    }

    return rc;
}

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
L7_RC_t 
traceRouteAlloc( L7_uchar8 *index, L7_uchar8 *name, L7_BOOL syncFlag, 
        traceRouteCallback callback, void *userHandle, L7_ushort16 *handle )
{
    L7_uint32 i;
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    if ( index == NULL )
        index = (L7_uchar8 *)"";

    if ( name == NULL )
        name = (L7_uchar8 *)"";

    /* Verify length of name and index is within limits */
    if ( strlen( (L7_char8 *)index ) >= TRACEROUTE_MAX_NAME_LEN || 
            strlen( (L7_char8 *)name ) >= TRACEROUTE_MAX_NAME_LEN )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, 
                "Cannot allocate traceRouteEntry - index or name too long.");
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    /* Find if there is any entry with same name and index */
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &entry );
        if ( entry->state == TRACEROUTE_STATE_FREE )
        {
            continue;
        }

        if ( strcmp( entry->index, index ) == 0 && 
                strcmp( entry->name, name ) == 0 )
        {
            osapiWriteLockGive( traceRouteLock_g );
            L7_LOGFNC( L7_LOG_SEVERITY_WARNING, 
                "Cannot allocate entry - duplicate name and index\n");
            return L7_FAILURE;
        }
    }

    /* Find an entry in FREE State */
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &entry );
        if ( entry->state == TRACEROUTE_STATE_FREE )
        {
            break;
        }
    }

    if ( i >= TRACEROUTE_MAX_SESSIONS )
    {
        /* Session table full */
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, 
                   "Traceroute Session table full."
                   " The maximum number of simultaneous traceroutes is already in progress");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Initialize the entry to default values. They can be overwritten with
     * valid values later by users */

    /*
     * Compute new handle. 
     * Handle consists of two parts, lower 4 bits consisting of index into 
     * the session table and upper 12 bits a running number incrementing 
     * everytime an entry is re-used.
     */

    *handle = entry->handle + TRACEROUTE_HANDLE_INCREMENT;
    bzero( (void *)entry, sizeof( traceRouteTable_t ) );
    entry->handle = *handle;

    strcpy( entry->index, index );
    strcpy( entry->name, name );
    /* Set default values if they are not specified */
    entry->probeSize = TRACEROUTE_DEFAULT_PROBE_SIZE;
    entry->probePerHop = TRACEROUTE_DEFAULT_PROBE_PER_HOP;
    entry->probeInterval = TRACEROUTE_DEFAULT_PROBE_INTERVAL;
    entry->port = TRACEROUTE_DEFAULT_PORT;
    entry->maxTtl = TRACEROUTE_DEFAULT_MAX_TTL;
    entry->initTtl = TRACEROUTE_DEFAULT_INIT_TTL;
    entry->currTtl = entry->initTtl;
    entry->maxFail = TRACEROUTE_DEFAULT_MAX_FAIL;

    entry->callback = callback;
    entry->userParam = userHandle;
    /* Leave probeIntervalResidue to 0 so that first probe is sent ASAP */
    if ( syncFlag == L7_TRUE )
    {
        entry->mode = TRACEROUTE_MODE_SYNC;
    }
    else
    {
        entry->mode = TRACEROUTE_MODE_ASYNC;
    }

    /* Initialize dynamic data for the session */
    for ( i = 0; i < TRACEROUTE_MAX_HOP_INFO; i++ )
    {
        entry->hopInfo[i].minRtt = TRACEROUTE_INVALID_RTT;
        entry->hopInfo[i].state = TRACEROUTE_STATE_FREE;
    }

    /* Set state to allocated. We do not process probe request/response in 
     * this state */
    entry->state = TRACEROUTE_STATE_ALLOCATED;

    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;
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
traceRouteFree( L7_ushort16 handle )
{
    L7_ushort16 index = handle & TRACEROUTE_INDEX_MASK;
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    if ( index >= TRACEROUTE_MAX_SESSIONS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, 
                "Invalid handle %u passed to traceRouteFree.", handle );
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    traceRouteGetEntry( index, &entry );
    if ( entry->handle != handle )
    {
        osapiWriteLockGive( traceRouteLock_g );
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, 
                "Invalid handle %u passed to traceRouteFree.", handle );
        return L7_FAILURE;
    }

    if ( entry->state == TRACEROUTE_STATE_IN_PROGRESS )
    {
        traceRouteDone( entry, TRACEROUTE_STATE_DONE );
    }

    entry->state = TRACEROUTE_STATE_FREE;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;
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
L7_RC_t  traceRouteSessionEnd( L7_ushort16 handle )
{
    L7_ushort16 index = handle & TRACEROUTE_INDEX_MASK;
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__,
        __FUNCTION__);

    if ( index >= TRACEROUTE_MAX_SESSIONS )
    {
      L7_LOGFNC( L7_LOG_SEVERITY_WARNING,
          "Invalid handle %u passed to traceRouteFree.", handle );
      return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    traceRouteGetEntry( index, &entry );
    if ( entry->handle != handle )
    {
      osapiWriteLockGive( traceRouteLock_g );
      L7_LOGFNC( L7_LOG_SEVERITY_WARNING,
          "Invalid handle %u passed to traceRouteFree.", handle );
      return L7_FAILURE;
    }

    if ( entry->state == TRACEROUTE_STATE_IN_PROGRESS )
    {
      traceRouteDone( entry, TRACEROUTE_STATE_DONE );
    }

    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
* @param  L7_ushort16 *probeInterval @b{(output)} Probes per hop in secs
* @param  L7_BOOL     *dontFag       @b{(output)} Dont Fragment Flag
* @param  L7_ushort16 *port          @b{(output)} UDP destination port
* @param  L7_ushort16 *maxTtl        @b{(output)} Max TTL for session
* @param  L7_ushort16 *initTtl       @b{(output)} Init TTL for session
* @param  L7_ushort16 *maxFail       @b{(output)} Max failures allowed 
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
L7_RC_t traceRouteResultGet( L7_uchar8 *index, L7_uchar8 *name, 
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_ushort16 *vrfId, 
        L7_uint32 *ipDa, L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    int i;
    L7_RC_t rc = L7_FAILURE;
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    if ( index == NULL || name == NULL )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid params - index or name is NULL.");
        return L7_FAILURE;
    }

    osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    /* Search for entry with given index and name in traceroute session table */
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &entry );
        if ( entry->state == TRACEROUTE_STATE_FREE )
        {
            continue;
        }
        if ( strcmp( (L7_char8 *)index, (L7_char8 *)entry->index ) == 0 && 
                strcmp( (L7_char8 *)name, (L7_char8 *)entry->name ) == 0 )
        {
            rc = L7_SUCCESS;
            /* Copy the values needed for traceRouteCtrlTable entry */
            traceRouteReqCopy( entry, handle, operStatus, vrfId, ipDa, 
                    probeSize, probePerHop, probeInterval, dontFrag,
                    port, maxTtl, initTtl, maxFail );
            break;
        }
    }

    osapiReadLockGive( traceRouteLock_g );
    return rc;
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
traceRouteHandleGet( L7_uchar8 *index, L7_uchar8 *name,
    L7_ushort16 *handle)
{
  int i;
  L7_RC_t rc = L7_FAILURE;
  traceRouteTable_t *entry;

  L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__,
      __FUNCTION__);

  if ( index == NULL || name == NULL )
  {
    L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid params - index or name is NULL.");
    return L7_FAILURE;
  }

  osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

  /* Search for entry with given index and name in traceroute session table */
  for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
  {
    traceRouteGetEntry( i, &entry );
    if ( entry->state == TRACEROUTE_STATE_FREE )
    {
      continue;
    }
    if ( strcmp( (L7_char8 *)index, (L7_char8 *)entry->index ) == 0 &&
        strcmp( (L7_char8 *)name, (L7_char8 *)entry->name ) == 0 )
    {
      rc = L7_SUCCESS;
      *handle = entry->handle;
      break;
    }
  }

  osapiReadLockGive( traceRouteLock_g );
  return rc;

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
traceRouteDestAddrSet( L7_ushort16 handle, L7_uint32 ipDa )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->ipDa = ipDa;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

}

/*********************************************************************
*
* @param  L7_ushort16 handle @b{(input)}  Key for traceRouteCtlTable
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
traceRouteDestAddrGet(L7_ushort16 handle,L7_uint32 *ipDa)
{
  traceRouteTable_t *entry;

  L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__,
      __FUNCTION__);

  osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

  if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
  {
    osapiReadLockGive( traceRouteLock_g );
    return L7_FAILURE;
  }

  *ipDa = entry->ipDa;

  osapiReadLockGive( traceRouteLock_g );
  return L7_SUCCESS;

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
traceRouteVrIdSet( L7_ushort16 handle, L7_ushort16 vrId )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    if ( vrId >= L7_VR_MAX_VIRTUAL_ROUTERS )
    {
        L7_LOGFNC(L7_LOG_SEVERITY_WARNING, "Invalid vrId to traceRouteVrIdSet");
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->vrfId = vrId;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteProbePerHopSet( L7_ushort16 handle, L7_ushort16 count )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    if ( count > TRACEROUTE_MAX_PROBE_PER_HOP || 
            count < TRACEROUTE_MIN_PROBE_PER_HOP )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid probePerHop.");
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiWriteLockGive( traceRouteLock_g );
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        return L7_FAILURE;
    }
    entry->probePerHop = count;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  To set probe interval for a session
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
traceRouteProbeIntervalSet( L7_ushort16 handle, L7_ushort16 interval )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    if ( interval > TRACEROUTE_MAX_INTERVAL || 
            interval < TRACEROUTE_MIN_INTERVAL )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid probeInterval.");
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    entry->probeInterval = interval * TRACEROUTE_INTERVAL_MULTIPLIER;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteProbeSizeSet( L7_ushort16 handle, L7_ushort16 size )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    /* Validate range for size */
    if ( size > TRACEROUTE_MAX_PROBE_SIZE )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid probeSize - size "
                "less than minimum value" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    entry->probeSize = size;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteDestPortSet(L7_ushort16 handle, L7_ushort16 port)
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    /* Validate range for port number */
    if ( port < TRACEROUTE_MIN_PORT )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid probeSize - size "
                "less than minimum value" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->port = port;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteMaxTtlSet( L7_ushort16 handle, L7_ushort16 maxTtl )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    /* Validate maxTtl field */
    if ( maxTtl > TRACEROUTE_MAX_TTL || maxTtl < TRACEROUTE_MIN_TTL )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid maxTtl.");
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->maxTtl = maxTtl;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteInitTtlSet( L7_ushort16 handle, L7_ushort16 initTtl )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    /* Validate maxTtl field */
    if ( initTtl > TRACEROUTE_MAX_INIT_TTL  )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid initTtl.");
        return L7_FAILURE;
    }

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->initTtl = initTtl;
    /* Update fields dependent on iniTtl */
    entry->currTtl = initTtl;
    entry->hopInfo[0].ttl = initTtl;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteMaxFailSet( L7_ushort16 handle, L7_ushort16 maxFail )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    /* Validate maxFail field */
    if ( maxFail > TRACEROUTE_MAX_FAIL)
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid maxFail.");
        return L7_FAILURE;
    }

#if 0
    /* VR-FuncSpec does not address these values. So remove the change */
    /* Set to default if param is 255 (per RFC 2925)*/
    if ( maxFail == 0 || maxFail == 255 )
    {
        maxFail = TRACEROUTE_DEFAULT_MAX_FAIL;
    }
#endif /* 0 */

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->maxFail = maxFail;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteDontFragSet( L7_ushort16 handle, L7_BOOL dfFlag)
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid handle.");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }
    entry->dontFrag = dfFlag;
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;

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
traceRouteStart( L7_ushort16 handle )
{
    traceRouteTable_t *entry;
    L7_RC_t rc;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    if ( (entry->state == TRACEROUTE_STATE_IN_PROGRESS) || (entry->state == TRACEROUTE_STATE_FREE) )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid state - cannot start");
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Create the sockets. traceRouteFree API uses state to see if socket must
     * be closed or not. Dont change the state till socket is created */
    if ( traceRouteCreateSock( entry ) != L7_SUCCESS )
    {
        osapiWriteLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Set state to IN_PROGRESS. Socket can be closed if Free is called */
    entry->state = TRACEROUTE_STATE_IN_PROGRESS;

    /* Check mode of operation */
    if ( entry->mode == TRACEROUTE_MODE_ASYNC )
    {
        rc = traceRouteAsync( entry );
        osapiWriteLockGive( traceRouteLock_g );
        return rc;
    }

    osapiWriteLockGive( traceRouteLock_g );

    /* Must be sync mode */
    return traceRouteSync( entry );
}

/*********************************************************************
* @purpose  Get first Traceroute session allocated
*
* @param  L7_uchar8   *index  @b{(output)} Field for traceRouteCtlOwnerIndex
* @param  L7_uchar8   *name   @b{(output)} Name of the session
* @param  L7_ushort16 *handle @b{(output)} Handle for the Traceroute session
* @param  L7_ushort16 *vrfId  @b{(output)} VRF Id for the session
* @param  L7_uint32   *ipDa   @b{(output)} IP Address of the destination
* @param  L7_ushort16 *probeSize     @b{(output)} Probe Size
* @param  L7_ushort16 *probePerHop   @b{(output)} Probes per hop
* @param  L7_ushort16 *probeInterval @b{(output)} Probes per hop in secs
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
traceRouteGetFirst(L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle, 
        L7_BOOL *operStatus, L7_ushort16 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    *index = '\0';
    *name = '\0';
    *handle = 0;

    return traceRouteGetNext( index, name, handle, operStatus, vrfId, ipDa, 
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
* @param  L7_ushort16 *probeInterval @b{(output)} Probes per hop in secs
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
traceRouteGetNext( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle, 
        L7_BOOL *operStatus, L7_ushort16 *vrfId, L7_uint32 *ipDa, 
        L7_ushort16 *probeSize, L7_ushort16 *probePerHop, 
        L7_ushort16 *probeInterval, L7_BOOL *dontFrag, L7_ushort16 *port, 
        L7_ushort16 *maxTtl, L7_ushort16 *initTtl, L7_ushort16 *maxFail)
{
    L7_RC_t rc = L7_FAILURE;
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    if ( traceRouteGetNextHandle( index, name, handle ) == L7_SUCCESS )
    {
        rc = traceRouteHandleToEntry( *handle, &entry );
        if ( rc == L7_SUCCESS )
        {
            strcpy( (L7_char8 *)index, (L7_char8 *)entry->index );
            strcpy( (L7_char8 *)name, (L7_char8 *)entry->name );
            traceRouteReqCopy( entry, handle, operStatus, vrfId, ipDa, 
                probeSize, probePerHop, probeInterval, dontFrag, 
                port, maxTtl, initTtl, maxFail );
        }
    }

    osapiReadLockGive( traceRouteLock_g );
    return rc;
}

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
  L7_RC_t
traceRouteSessionHandleGetNext( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle)
{
  L7_RC_t rc = L7_FAILURE;
  traceRouteTable_t *entry;

  L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__,
      __FUNCTION__);

  osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

  if ( traceRouteGetNextHandle( index, name, handle ) == L7_SUCCESS )
  {
    rc = traceRouteHandleToEntry( *handle, &entry );
    if ( rc == L7_SUCCESS )
    {
      strcpy( (L7_char8 *)index, (L7_char8 *)entry->index );
      strcpy( (L7_char8 *)name, (L7_char8 *)entry->name );
      *handle = entry->handle;
    }
  }

  osapiReadLockGive( traceRouteLock_g );
  return rc;
}



/*********************************************************************
* @purpose  Queries first hop in a given traceroute session
*
* @param L7_ushort16 *handle    @b{(input)}  Handle for the traceroute session
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
traceRouteHopGetFirst( L7_ushort16 handle, L7_ushort16 *hopIndex, 
        L7_ushort16 *ttl, L7_uint32 *hopDa, L7_uint32 *minRtt, 
        L7_uint32 *maxRtt, L7_uint32 *avgRtt, L7_ushort16 *probeSent, 
        L7_ushort16 *probeRecvd )
{
    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    *hopIndex = (L7_ushort16)-1;
    return traceRouteHopGetNext( handle, hopIndex, ttl, hopDa, minRtt, maxRtt,
            avgRtt, probeSent, probeRecvd );
}

/*********************************************************************
* @purpose  Queries next hop in a traceroute session
*
* @param L7_ushort16 *handle    @b{(inout)}  Handle for the session
* @param L7_ushort16 *hopIndex  @b{(inout)} Index of first hop
* @param L7_ushort16 *ttl       @b{(output)} TTL for the hop
* @param L7_uint32   *hopDa      @b{(output)} IP Address of the hop
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
traceRouteHopGetNext( L7_ushort16 handle, L7_ushort16 *hopIndex, 
        L7_ushort16 *ttl, L7_uint32 *hopDa, L7_uint32 *minRtt, 
        L7_uint32 *maxRtt, L7_uint32 *avgRtt, L7_ushort16 *probeSent, 
        L7_ushort16 *probeRecvd )
{
    L7_ushort16 index = handle & TRACEROUTE_INDEX_MASK;
    traceRouteTable_t *entry;
    traceRouteHops_t *resp;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiReadLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Allow query only if state is DONE */
    if ( entry->state < TRACEROUTE_STATE_DONE )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid TraceRoute session." );
        osapiReadLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Find the first index to check. traceRouteHopGetFirst also uses same 
     * API and passes hop as -1. Pass first valid entry for GET */
    if ( *hopIndex == (L7_ushort16)-1 )
    {
        if ( entry->hopCount < TRACEROUTE_MAX_HOP_INFO )
        {
            *hopIndex = 0;
        }
        else
        {
            *hopIndex = entry->hopCount - TRACEROUTE_MAX_HOP_INFO;
        }
    }
    else 
    {
        /* Validate if hop requested is within the range of hopInfo table */
        if ( *hopIndex >= entry->hopCount )
        {
            L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid Hop Index." );
            osapiReadLockGive( traceRouteLock_g );
            return L7_FAILURE;
        }

        /* Check if hop is less than the minimum hop stored */
        if ( entry->hopCount >= TRACEROUTE_MAX_HOP_INFO )
        {
            if ( *hopIndex < (entry->hopCount - TRACEROUTE_MAX_HOP_INFO) )
            {
                /* The hop index has moved since last query. Just set hopIndex
                 * to first valid entry in table */
                *hopIndex = entry->hopCount - TRACEROUTE_MAX_HOP_INFO;
            }
            else
            {
                /* Increment hopIndex to point to next entry */
                (*hopIndex)++;
            }
        }
        else
        {
            /* Increment hopIndex to point to next entry */
            (*hopIndex)++;
        }
    }

    /* Copy the hop entry */
    index =  *hopIndex % TRACEROUTE_MAX_HOP_INFO;
    resp = &entry->hopInfo[index];


    *ttl = resp->ttl;
    *hopDa = resp->hopDa;
    *probeSent = resp->probeSent;
    *probeRecvd = resp->probeResponses;

    if ( resp->probeResponses > 0 )
    {
        *avgRtt = resp->avgRtt / resp->probeResponses;
        *minRtt = resp->minRtt;
        *maxRtt = resp->maxRtt;
    }
    else
    {
        *avgRtt = 0;
        *minRtt = 0;
        *maxRtt = 0;
    }

    osapiReadLockGive( traceRouteLock_g );
    
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Queries a specific hop in a traceroute session
*
* @param L7_ushort16 *handle    @b{(inout)}  Handle for the session
* @param L7_ushort16 hopIndex   @b{(input)} Index of first hop
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
traceRouteHopGet( L7_ushort16 handle, L7_ushort16 hopIndex, 
        L7_ushort16 *ttl, L7_uint32 *hopDa, L7_uint32 *minRtt, 
        L7_uint32 *maxRtt, L7_uint32 *avgRtt, L7_ushort16 *probeSent, 
        L7_ushort16 *probeRecvd )
{
    L7_ushort16 index = handle & TRACEROUTE_INDEX_MASK;
    traceRouteTable_t *entry;
    traceRouteHops_t *resp;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiReadLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Allow query only if state is DONE */
    if ( entry->state < TRACEROUTE_STATE_DONE )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid TraceRoute session." );
        osapiReadLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Validate if hop requested is within the range of hopInfo table */
    if ( hopIndex >= entry->hopCount )
    {
        L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid Hop Index." );
        osapiReadLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    /* Check if hop is less than the minimum hop stored */
    if ( entry->hopCount >= TRACEROUTE_MAX_HOP_INFO )
    {
        if ( hopIndex < (entry->hopCount - TRACEROUTE_MAX_HOP_INFO) )
        {
            /* The hop index has moved since last query. Return failure */
            L7_LOGFNC( L7_LOG_SEVERITY_WARNING, "Invalid Hop Index." );
            osapiReadLockGive( traceRouteLock_g );
            return L7_FAILURE;
        }
    }

    /* Copy the hop entry */
    index =  hopIndex % TRACEROUTE_MAX_HOP_INFO;
    resp = &entry->hopInfo[index];


    *ttl = resp->ttl;
    *hopDa = resp->hopDa;
    *probeSent = resp->probeSent;
    *probeRecvd = resp->probeResponses;

    if ( resp->probeResponses > 0 )
    {
        *avgRtt = resp->avgRtt / resp->probeResponses;
        *minRtt = resp->minRtt;
        *maxRtt = resp->maxRtt;
    }
    else
    {
        *avgRtt = 0;
        *minRtt = 0;
        *maxRtt = 0;
    }

    osapiReadLockGive( traceRouteLock_g );
    
    return L7_SUCCESS;
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
traceRouteQuery( L7_ushort16 handle, L7_BOOL *operStatus, 
        L7_ushort16 *currTtl, L7_ushort16 *currHopCount, 
        L7_ushort16 *currProbeCount, L7_ushort16 *testAttempt, 
        L7_ushort16 *testSuccess )
{
    traceRouteTable_t *entry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiReadLockTake( traceRouteLock_g, L7_WAIT_FOREVER );

    if ( traceRouteHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiReadLockGive( traceRouteLock_g );
        return L7_FAILURE;
    }

    traceRouteRespCopy( entry, &handle, operStatus, currTtl, currHopCount,
            currProbeCount, testAttempt, testSuccess );

    osapiReadLockGive( traceRouteLock_g );
    return L7_SUCCESS;
}
 
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
traceRouteMaxSessionsGet(L7_uint32 * maxSessions)
{
  *maxSessions = TRACEROUTE_MAX_SESSIONS;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Flushes all entries in DONE state.
*
* @param    None
*
* @returns  L7_SUCCESS, if success
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t 
traceRouteFlushDone( )
{
    L7_ushort16 i;
    traceRouteTable_t *traceEntry;

    L7_LOGFNC( L7_LOG_SEVERITY_DEBUG, "%s %d: %s : \n", __FILE__, __LINE__, 
            __FUNCTION__);

    osapiWriteLockTake( traceRouteLock_g, L7_WAIT_FOREVER );
    for ( i = 0; i < TRACEROUTE_MAX_SESSIONS; i++ )
    {
        traceRouteGetEntry( i, &traceEntry );
        if ( traceEntry->state >= TRACEROUTE_STATE_DONE )
            traceEntry->state = TRACEROUTE_STATE_FREE;
    }
    osapiWriteLockGive( traceRouteLock_g );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display all devshell commands.
*
* @param    None
*
* @returns  None
*
* @notes    
*       The linker does not link object files that do not contain any function
*       that are called. As a result, we dont see any devshell commands for
*       traceroute. Added to ensure traceroute_debug commands are linked in.
*
* @end
*********************************************************************/
extern void dbgTraceRouteShowCommands( void );
void
traceRouteDevShellHelp( void )
{
    dbgTraceRouteShowCommands( );
}

