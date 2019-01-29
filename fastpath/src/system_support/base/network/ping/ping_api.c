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
#include "commdefs.h"
#include "l3_commdefs.h"
#include "l7_socket.h"
#include "l7_icmp.h"
#include "osapi.h"
#include "osapi_support.h"
#include "string.h"
#include "log.h"
#include "ping_api.h"
#include "ping_private.h"
#include "nimapi.h"
#include "acl_exports.h"

/* Extern definitions */

/* Table containing ping sessions */
extern pingSessionTable_t   *pingSessionTable_g;

/* Lock to control access to pingSessionTable_g*/
extern osapiRWLock_t        pingSessionLock_g;

/*********************************************************************
* @purpose  Returns pointer to session table for given handle
*
* @param  L7_ushort16 *handle @b{(input)} Handle for the session
* @param  pingSessionTable_t **entry @b{(output)} Will contain address of entry
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
pingHandleToEntry( L7_ushort16 handle, pingSessionTable_t **entry )
{
    L7_uint32 index = handle & PING_INDEX_MASK;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
            __FUNCTION__);

    if ( index >= PING_MAX_SESSIONS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid handle.");
        return L7_FAILURE;
    }

    pingGetEntry( index, entry );

    if ( (*entry)->handle != handle ||
            (*entry)->state == PING_SESSION_STATE_FREE )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid handle.");
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copies the fields needed for pingCtlEntry from session entry
*
* @param  pingSessionTable_t *entry @b{(input)} Address of the entry
* @param  L7_ushort16 *handle @b{(output)} Handle for the session
* @param  L7_uint32 *vrfId @b{(output)) VRF-ID of the session
* @param  L7_uint32 *ipDa  @b{(output)) IP-DA of the session
* @param  L7_BOOL     *operStatus   @b{(output)} Oper status of session
* @param  L7_ushort16 *count  @b{(output)) Number of probe requests
* @param  L7_ushort16 *size  @b{(output)) Size of probe requests
* @param  L7_ushort16 *interval  @b{(output)) Interval between probes in secs
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
void
pingSessionCopy( pingSessionTable_t *entry, L7_ushort16 *handle,
        L7_BOOL *operStatus, L7_uint32 *vrfId, L7_uint32 *ipDa,
        L7_ushort16 *count, L7_ushort16 *size, L7_ushort16 *interval )
{
    *handle = entry->handle;
    /* Oper Status is TRUE only if session is in progress. */
    if ( entry->state == PING_SESSION_STATE_IN_PROGRESS)
    {
        *operStatus = L7_TRUE;
    }
    else
    {
        *operStatus = L7_FALSE;
    }

    *vrfId = entry->vrfId;
    *ipDa = entry->ipDa;
    *count = entry->probeCount;
    *size = entry->probeSize;
    *interval = entry->probeInterval / PING_INTERVAL_MULTIPLIER;
    return;
}

/*********************************************************************
* @purpose  Copies the response from session entry
*
* @param  pingSessionTable_t *entry @b{(input)} Handle for the ping session
* @param  L7_BOOL     *operStatus   @b{(output)} Oper status of session
* @param  L7_uint32   *probeSent    @b{(output)) Number of probes to send
* @param  L7_uint32   *probeSuccess @b{(output)) Number of successfull probes
* @param  L7_ushort16 *probeFail    @b{(output)) Number of failed probes
* @param  L7_uint32   *minRtt       @b{(output)) Minimum RTT in usec
* @param  L7_uint32   *maxRtt       @b{(output)) Maximum RTT in usec
* @param  L7_uint32   *avgRtt       @b{(output)) Average RTT in usec
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
void
pingSessionRespCopy( pingSessionTable_t *entry, L7_BOOL *operStatus,
        L7_ushort16 *probeSent, L7_ushort16 *probeSuccess,
        L7_ushort16 *probeFail, L7_uint32 *minRtt, L7_uint32 *maxRtt,
        L7_uint32 *avgRtt )
{
    /* Oper Status is TRUE only if session is in progress. */
    if ( entry->state == PING_SESSION_STATE_IN_PROGRESS)
    {
        *operStatus = L7_TRUE;
    }
    else
    {
        *operStatus = L7_FALSE;
    }

    *probeSent = entry->probeSent;
    *probeSuccess = entry->probeSuccess;
    *probeFail = entry->probeSent - entry->probeSuccess;

    /* minRtt, maxRtt and avgRtt valid if atleast one probe succeeded */
    if ( entry->probeSuccess > 0 )
    {
        *avgRtt = entry->aggrRtt / entry->probeSuccess;
        *minRtt = entry->minRtt;
        *maxRtt = entry->maxRtt;
    }
    else
    {
        *avgRtt = 0;
        *minRtt = 0;
        *maxRtt = 0;
    }

    return;
}

/*********************************************************************
* @purpose  Compare two session entries
*
* @param  L7_uchar8 *index1 @b{input} Index of first entry
* @param  L7_uchar8 *name1  @b{input} Name of first entry
* @param  L7_ushort16 handle1 @b{input} Handle of first entry
* @param  L7_uchar8 *index2 @b{input} Index of second entry
* @param  L7_uchar8 *name2  @b{input} Name of second entry
* @param  L7_ushort16 handle2 @b{input} Handle of second entry
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
pingSessionCompareKey( L7_uchar8 *index1, L7_uchar8 *name1, L7_ushort16 handle1,
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
* @purpose  Finds chronologically next entry in ping session table
*
* @param  L7_uchar8 *index  @b{(inout)} index of logically next entry
* @param  L7_uchar8 *name   @b{(inout)} name of logically next entry
* @param  L7_ushort16 **handle @b{(inout)} Handle of logically next entry
*
* @returns  L7_SUCCESS   If session could be allocated
* @returns  L7_FAILURE   If session could not be allocated
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
pingSessionGetNextHandle( L7_uchar8 *index, L7_uchar8 *name,
        L7_ushort16 *handle )
{
    L7_uint32 i;
    pingSessionTable_t *currEntry = NULL;
    pingSessionTable_t *entry = NULL;
    L7_RC_t rc = L7_FAILURE;
    L7_int32 compareResult;

    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &entry );
        /* See if entry is allocated or not */
        if ( entry->state == PING_SESSION_STATE_FREE )
        {
            continue;
        }

        /* First check if the entry is greater than one in parameter */
        if ( pingSessionCompareKey( index, name, *handle, entry->index,
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
                compareResult = pingSessionCompareKey( currEntry->index,
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
* @purpose  Allocates a ping session
*
* @param  L7_uchar8 *index  @b{(input)} Field for pingCtlOwnerIndex
* @param  L7_uchar8 *name   @b{(input)} Name of the session
* @param  L7_BOOL syncFlag  @b{(input)} Operation mode (TRUE=Sync, FALSE=ASync)
* @param  void    *callback @b{(input)} Callback function called for each probe
*               response. Pass NULL if callback is not needed.
* @param  void       *finishCallback @b{(input)} Callback function called after
*                                                completing the ping operation
* @param  L7_ushort16 *userHandle @b{(input)}  User parameter passed in callback.
* @param  L7_ushort16 *handle     @b{(output)} Handle for the session allocated
*
* @returns  L7_SUCCESS   If session could be allocated
* @returns  L7_FAILURE   If session could not be allocated
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
L7_RC_t
pingSessionAlloc( L7_uchar8 *index, L7_uchar8 *name,
        L7_BOOL syncFlag, pingSessionCallback *callback,
        pingSessionFinishCallback *finishCallback, void *userHandle,
        L7_ushort16 *handle )
{
    L7_uint32 i;
    pingSessionTable_t *pingEntry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
                    __FUNCTION__);

    if ( index == NULL )
        index = (L7_uchar8 *)"";

    if ( name == NULL )
        name = (L7_uchar8 *)"";

    /* Verify length of name and index is within limits */
    if ( strlen( (L7_char8 *)index ) >= PING_MAX_NAME_LEN ||
            strlen( (L7_char8 *)name ) >= PING_MAX_NAME_LEN )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Cannot allocate pingSessionEntry - index or name too long.");
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );

    /* Find if there is any entry with same name and index */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );
        if ( pingEntry->state == PING_SESSION_STATE_FREE )
        {
            continue;
        }

        if ( strcmp( pingEntry->index, index ) == 0 &&
                strcmp( pingEntry->name, name ) == 0 )
        {
            osapiWriteLockGive( pingSessionLock_g );
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Cannot allocate entry - duplicate name and index\n");
            return L7_FAILURE;
        }
    }

    /* Find an entry in FREE State */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );
        if ( pingEntry->state == PING_SESSION_STATE_FREE )
        {
            break;
        }
    }

    if ( i >= PING_MAX_SESSIONS )
    {
        /* Session table full */
        osapiWriteLockGive( pingSessionLock_g );
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

    *handle = pingEntry->handle + PING_HANDLE_INCREMENT;
    memset( pingEntry, 0, sizeof( *pingEntry ) );
    pingEntry->handle = *handle;

    strcpy( pingEntry->index, index );
    strcpy( pingEntry->name, name );

    pingEntry->probeInterval = PING_DEFAULT_PROBE_INTERVAL;
    pingEntry->probeSize = PING_DEFAULT_PROBE_SIZE;
    pingEntry->probeCount = PING_DEFAULT_PROBE_COUNT;
    pingEntry->probeSrcIpAddr = PING_DEFAULT_SRC_IP_ADDR;
    pingEntry->dscp       = PING_DEFAULT_DSCP;

    /* Set state to allocated. We do not process probe request/response in
     * this state */
    pingEntry->state = PING_SESSION_STATE_ALLOCATED;

    /* Leave probeIntervalResidue to 0 so that first probe is sent ASAP */

    /* probeIterations is set to value > probeCount so that we wait for
     * result from last probe request we have sent. We will will wait till
     * PING_ITERATION_COUNT*probeCount times probeInterval after sending last
     * probe request is sent.
     */
    pingEntry->probeIterations = pingEntry->probeCount + PING_ITERATION_COUNT;
    pingEntry->callback = (pingSessionCallback)callback;
    pingEntry->finishCallback = (pingSessionFinishCallback)finishCallback;
    pingEntry->userParam = userHandle;

    if ( syncFlag == L7_TRUE )
    {
        pingEntry->mode = PING_PROBE_MODE_SYNC;
    }
    else
    {
        pingEntry->mode = PING_PROBE_MODE_ASYNC;
    }

    /* Initialize default minRtt for convinience */
    pingEntry->minRtt = PING_INVALID_RTT;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
}

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
* @end
*********************************************************************/
L7_RC_t
pingSessionFree( L7_ushort16 handle )
{
    L7_ushort16 index = handle & PING_INDEX_MASK;
    pingSessionTable_t *pingEntry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
                    __FUNCTION__);

    if ( index >= PING_MAX_SESSIONS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle %u passed to pingSessionFree.", handle );
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    pingGetEntry( index, &pingEntry );

    if ( pingEntry->handle != handle )
    {
        osapiWriteLockGive( pingSessionLock_g );
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle %u passed to pingSessionFree.", handle );
        return L7_FAILURE;
    }
    /* Close the connection forcefully if its still open */
    pingEnd( pingEntry, PING_SESSION_STATE_DONE );

    /* Free the entry */
    pingEntry->state = PING_SESSION_STATE_FREE;

    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
}

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
L7_RC_t
pingSessionEnd( L7_ushort16 handle )
{
  L7_ushort16 index = handle & PING_INDEX_MASK;
  pingSessionTable_t *pingEntry;

  PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
      __FUNCTION__);

  if ( index >= PING_MAX_SESSIONS )
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
        "Invalid handle %u passed to pingSessionFree.", handle );
    return L7_FAILURE;
  }

  osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
  pingGetEntry( index, &pingEntry );

  if ( pingEntry->handle != handle )
  {
    osapiWriteLockGive( pingSessionLock_g );
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
        "Invalid handle %u passed to pingSessionFree.", handle );
    return L7_FAILURE;
  }
  /* Close the connection forcefully if its still open */
  pingEnd( pingEntry, PING_SESSION_STATE_DONE );

  osapiWriteLockGive( pingSessionLock_g );
  return L7_SUCCESS;
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
pingDestAddrSet( L7_ushort16 handle, L7_uint32 ipDa )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
        __FUNCTION__);
    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle to pingDestAddrSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }
    entry->ipDa = ipDa;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
*       Note, the API allows setting of VR-ID to 0 such that ping can be done
*       for management interfaces also
*
* @end
*********************************************************************/
L7_RC_t
pingVrIdSet( L7_ushort16 handle, L7_ushort16 vrId )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( vrId >= L7_VR_MAX_VIRTUAL_ROUTERS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid vrId to pingVrIdSet" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid handle to pingVrIdSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }
    entry->vrfId = vrId;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
pingProbeCountSet( L7_ushort16 handle, L7_ushort16 count )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( count > PING_MAX_PROBES || count < PING_MIN_PROBES )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid probeCount to pingProbeCountSet" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle to pingProbeCountSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }
    entry->probeCount = count;
    entry->probeIterations = count + PING_ITERATION_COUNT;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
pingProbeCountGet( L7_ushort16 handle, L7_uint32 *count )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }
    *count = entry->probeCount;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;

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
L7_RC_t pingDSCPSet( L7_ushort16 handle, L7_uint32 dscpVal )
{
    pingSessionTable_t *entry;
    L7_uint32 byteVal;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( dscpVal > L7_ACL_MAX_DSCP || dscpVal < L7_ACL_MIN_DSCP )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid DSCP value to pingDSCPSet" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle to pingDSCPSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }

    if (entry->state == PING_SESSION_STATE_IN_PROGRESS)
    {
      byteVal = dscpVal;
      byteVal = byteVal << 2;

      if (osapiSetsockopt(entry->sock, IPPROTO_IP, L7_IP_TOS,
                          (L7_uchar8 *)&byteVal, sizeof(byteVal)) != L7_SUCCESS)
      {
         L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Unable to set IP_TOS option" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
      }
    }

    entry->dscp = dscpVal;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t pingDSCPGet( L7_ushort16 handle, L7_uint32 *dscpVal )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }

    *dscpVal = entry->dscp;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;

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
pingProbeIntervalSet( L7_ushort16 handle, L7_ushort16 interval )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( interval > PING_MAX_INTERVAL || interval < PING_MIN_INTERVAL )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid interval to pingProbeIntervalSet" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle to pingProbeIntervalSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }
    entry->probeInterval = interval * PING_INTERVAL_MULTIPLIER;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
pingProbeIntervalGet( L7_ushort16 handle, L7_uint32 *interval )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
             __FUNCTION__);

    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }

    *interval = entry->probeInterval / PING_INTERVAL_MULTIPLIER;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
}

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
L7_RC_t pingProbeSrcIpAddrSet(L7_ushort16 handle, L7_uint32 srcIpAddr)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle to pingProbeSrcIpAddrSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }
    entry->probeSrcIpAddr = srcIpAddr;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
pingProbeSizeSet( L7_ushort16 handle, L7_ushort16 size )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( size > PING_MAX_SIZE )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid size to pingProbeSizeSet" );
        return L7_FAILURE;
    }

    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "Invalid handle to pingProbeSizeSet" );
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }
    entry->probeSize = size;
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
pingProbeSizeGet( L7_ushort16 handle, L7_uint32 *size )
{

    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }

    *size = entry->probeSize;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
*       If the mode is SYNC, then complete operation is executed before
*       returning from the API. Also, response will be returned in resp.
*
*       If the mode is ASYNC, then it will initiate the session and return.
*       The response must be queried later with pingSessionQuery API.
*
* @end
*********************************************************************/
L7_RC_t
pingSessionStart( L7_ushort16 handle )
{
    pingSessionTable_t *entry;
    L7_RC_t rc;
    L7_uint32 byteVal;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }

    if ( (entry->state == PING_SESSION_STATE_IN_PROGRESS)|| (entry->state == PING_SESSION_STATE_FREE) )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid state - cannot start session");
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }

    /* Create socket to use for ping */
    if ( pingCreateSock( entry ) != L7_SUCCESS )
    {
        osapiWriteLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }

    byteVal = entry->dscp;
    byteVal = byteVal << 2;

    if (osapiSetsockopt(entry->sock, IPPROTO_IP, L7_IP_TOS,
                        (L7_uchar8 *)&(byteVal), sizeof(byteVal)) != L7_SUCCESS)
    {
       L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
              "Unable to set IP_TOS option" );
    }

    /* Set state to IN_PROGRESS. Socket is closed when session is freed */
    entry->state = PING_SESSION_STATE_IN_PROGRESS;

    /* Execute the sessions */
    if ( entry->mode == PING_PROBE_MODE_ASYNC )
    {
        rc = pingSessionAsync();
        osapiWriteLockGive( pingSessionLock_g );
        return rc;
    }

    osapiWriteLockGive( pingSessionLock_g );

    /* Must be SYNC mode */
    return pingSessionSync( entry );
}

/*********************************************************************
* @purpose  Queries a ping session
*
* @param  L7_ushort16 *handle       @b{(input)}   Handle for the ping session
* @param  L7_BOOL     *operStatus   @b{(output)}  Operational status of session
* @param  L7_ushort16 *probeSent    @b{(output)}  Number of probes sent
* @param  L7_ushort16 *probeSuccess @b{(output)}  Number of probes succeeded
* @param  L7_ushort16 *probeFail    @b{(output)}  Number of probes failed
* @param  L7_uint32   *minRtt       @b{(output)}  Minimum RTT in usec
* @param  L7_uint32   *maxRtt       @b{(output)}  Max RTT in usec
* @param  L7_uint32   *avgRtt       @b{(output)}  Avg RTT in usec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
pingSessionQuery( L7_ushort16 handle, L7_BOOL *operStatus,
        L7_ushort16 *probeSent, L7_ushort16 *probeSuccess,
        L7_ushort16 *probeFail, L7_uint32 *minRtt, L7_uint32 *maxRtt,
        L7_uint32 *avgRtt )
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
        osapiReadLockGive( pingSessionLock_g );
        return L7_FAILURE;
    }

    pingSessionRespCopy( entry, operStatus, probeSent, probeSuccess,
            probeFail, minRtt, maxRtt, avgRtt );

    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Queries a ping session  target address
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
L7_RC_t pingSessionQueryTargetAddrs( L7_ushort16 handle, L7_uint32 *ipDa)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }
    *ipDa = entry->ipDa;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;


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
L7_RC_t pingSessionQueryOperStatus( L7_ushort16 handle, L7_BOOL *operStatus)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }
    if ( entry->state == PING_SESSION_STATE_IN_PROGRESS)
    {
      *operStatus = L7_TRUE;
    }
    else
    {
      *operStatus = L7_FALSE;
    }

    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t pingSessionQuerySessionState( L7_ushort16 handle, L7_uint32 *operState)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }
    *operState = entry->state;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t pingSessionQueryMinRtt( L7_ushort16 handle, L7_uint32 *minRtt)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }
    if ( entry->probeSuccess > 0 )
    {
      *minRtt = (entry->minRtt)/PING_RTT_MULTIPLIER;

    }
    else
    {
      *minRtt = 0;
    }

    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t pingSessionQueryMaxRtt( L7_ushort16 handle, L7_uint32 *maxRtt)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }
    if ( entry->probeSuccess > 0 )
    {
      *maxRtt = (entry->maxRtt)/PING_RTT_MULTIPLIER;
    }
    else
    {
      *maxRtt = 0;
    }

    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t pingSessionQueryAvgRtt( L7_ushort16 handle, L7_uint32 *avgRtt)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }

    if ( entry->probeSuccess > 0 )
    {
      *avgRtt = (entry->aggrRtt / entry->probeSuccess)/PING_RTT_MULTIPLIER;
    }
    else
    {
      *avgRtt = 0;
    }

    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t  pingSessionQueryProbeSent(L7_ushort16 handle, L7_uint32 *probeSent)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }

    *probeSent= entry->probeSent;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
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
L7_RC_t pingSessionQueryProbeResponse( L7_ushort16 handle, L7_uint32 *probeResponse)
{
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    if ( pingHandleToEntry( handle, &entry ) != L7_SUCCESS )
    {
      osapiReadLockGive( pingSessionLock_g );
      return L7_FAILURE;
    }

    *probeResponse= entry->probeSuccess;
    osapiReadLockGive( pingSessionLock_g );
    return L7_SUCCESS;
}

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
* @param  L7_ushort16 *interval @b{(output)) Interval between probes in secs
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
pingSessionHandleGet( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle,
        L7_BOOL *operStatus, L7_uint32 *vrfId, L7_uint32 *ipDa,
        L7_ushort16 *count, L7_ushort16 *size, L7_ushort16 *interval )
{

    int i;
    L7_RC_t rc = L7_FAILURE;
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( index == NULL || name == NULL )
    {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid params - index or name is NULL.");
        return L7_FAILURE;
    }

    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );

    /* Search for entry with given index and name in ping session table */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &entry );
        if ( entry->state == PING_SESSION_STATE_FREE )
        {
            continue;
        }
        if ( strcmp( (L7_char8 *)index, (L7_char8 *)entry->index ) == 0 &&
                strcmp( (L7_char8 *)name, (L7_char8 *)entry->name ) == 0 )
        {
            rc = L7_SUCCESS;
            /* Copy the values needed for pingCtrlTable entry */
            pingSessionCopy( entry, handle, operStatus, vrfId, ipDa,
                    count, size, interval );
            break;
        }
    }

    osapiReadLockGive( pingSessionLock_g );
    return rc;
}

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
L7_RC_t
pingSessionHandleFromIndexTestNameGet( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle)
{

    int i;
    L7_RC_t rc = L7_FAILURE;
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    if ( index == NULL || name == NULL )
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "Invalid params - index or name is NULL.");
      return L7_FAILURE;
    }

    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );

    /* Search for entry with given index and name in ping session table */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
      pingGetEntry( i, &entry );
      if ( entry->state == PING_SESSION_STATE_FREE )
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

    osapiReadLockGive( pingSessionLock_g );
    return rc;
}


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
* @param  L7_ushort16 *interval   @b{(output)) Interval between probes in secs
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
pingSessionGetFirst ( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle,
        L7_BOOL *operStatus, L7_uint32 *vrfId, L7_uint32 *ipDa,
        L7_ushort16 *count, L7_ushort16 *size, L7_ushort16 *interval )
{
    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    *index = '\0';
    *name = '\0';
    *handle = 0;
    return pingSessionGetNext( index, name, handle, operStatus, vrfId, ipDa,
            count, size, interval );

}

/*********************************************************************
* @purpose  Get next ping session allocated
*
* @param  L7_uchar8   *index      @b{(inout)} Field for pingCtlOwnerIndex
* @param  L7_uchar8   *name       @b{(inout)} Name of the session
* @param  L7_ushort16 *handle     @b{(inout)} Handle for the ping session
* @parame L7_BOOL     *operStatus @b{(inout)} Operational status
* @param  L7_uint32   *vrfId      @b{(inout)) VRF-ID of the session
* @param  L7_uint32   *ipDa       @b{(inout)) IP-DA of the session
* @param  L7_ushort16 *count      @b{(inout)) Number of probes to send
* @param  L7_ushort16 *size       @b{(inout)) Size of the probes
* @param  L7_ushort16 *interval   @b{(inout)) Interval between probes in secs
*
* @returns  L7_SUCCESS   If any session could be found
* @returns  L7_FAILURE   If no session could be found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pingSessionGetNext( L7_uchar8 *index, L7_uchar8 *name,
        L7_ushort16 *handle, L7_BOOL *operStatus, L7_uint32 *vrfId
        , L7_uint32 *ipDa, L7_ushort16 *count, L7_ushort16 *size,
        L7_ushort16 *interval )
{

    L7_RC_t rc = L7_FAILURE;
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );

    if ( pingSessionGetNextHandle( index, name, handle ) == L7_SUCCESS )
    {
        rc = pingHandleToEntry( *handle, &entry );
        if ( rc == L7_SUCCESS )
        {

            strcpy( (L7_char8 *)index, (L7_char8 *)entry->index );
            strcpy( (L7_char8 *)name, (L7_char8 *)entry->name );
            /* Copy the values needed for pingCtrlTable entry */
            pingSessionCopy( entry, handle, operStatus, vrfId, ipDa, count,
                    size, interval );
        }
    }

    osapiReadLockGive( pingSessionLock_g );
    return rc;
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
pingGetNextHandle( L7_uchar8 *index, L7_uchar8 *name, L7_ushort16 *handle )
{

    L7_RC_t rc = L7_FAILURE;
    pingSessionTable_t *entry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiReadLockTake( pingSessionLock_g, L7_WAIT_FOREVER );

    if ( pingSessionGetNextHandle( index, name, handle ) == L7_SUCCESS )
    {
      rc = pingHandleToEntry( *handle, &entry );
      if ( rc == L7_SUCCESS )
      {

        strcpy( (L7_char8 *)index, (L7_char8 *)entry->index );
        strcpy( (L7_char8 *)name, (L7_char8 *)entry->name );
        *handle = entry->handle;
      }
    }

    osapiReadLockGive( pingSessionLock_g );
    return rc;

}

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
L7_RC_t
pingSessionFlushDone( void )
{

    L7_ushort16 i;
    pingSessionTable_t *pingEntry;

    PING_PATH_TRACE( "%s %d: %s : \n", __FILE__, __LINE__,
              __FUNCTION__);
    osapiWriteLockTake( pingSessionLock_g, L7_WAIT_FOREVER );
    /* Look for all sessions in DONE state and stop them */
    for ( i = 0; i < PING_MAX_SESSIONS; i++ )
    {
        pingGetEntry( i, &pingEntry );
        if ( pingEntry->state == PING_SESSION_STATE_DONE )
        {
            /* Close the connection forcefully if its still open */
            pingEnd( pingEntry, PING_SESSION_STATE_DONE );
            pingEntry->state = PING_SESSION_STATE_FREE;
        }
    }
    osapiWriteLockGive( pingSessionLock_g );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get Maximum number of ping  sessions allowed
*
* @param  L7_uint32  *maxSessions @b{(output)} maximum number of concurrent ping requests
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t
pingMaxSessionsGet(L7_uint32 *maxSessions)
{
  *maxSessions = PING_MAX_SESSIONS;
  return(L7_SUCCESS);
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
*       ping. Added just to ensure ping_debug commands are linked in.
*
* @end
*********************************************************************/
extern void dbgPingShowCommands( void );
void
pingDevShellHelp( void )
{
    dbgPingShowCommands( );
}

