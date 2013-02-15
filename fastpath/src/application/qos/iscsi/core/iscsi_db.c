/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_db.c
*
* @purpose ISCSI component session and connection database functions
*
* @component ISCSI
*
* @comments This file contains all the functions to maintain and query
*           the table of known iSCSI sessions and their connections.
*           All functions in this file are considered private to the 
*           iSCSI component and must be called within appropriate critical
*           section locks to protect the table contents.  These locks are
*           managed at level above these utility functions.
*
*           Many of these functions are designed to insulate the calling
*           layers from the details of the underlying data structures 
*           implementing this data store.
*
* @create 04/18/2008
*
* @end
*
**********************************************************************/
#include "dtlapi.h"
#include "dtl_iscsi.h"
#include "iscsi_db.h"

static iscsiSessionRecord_t     *sessionTable;
static iscsiConnectionRecord_t  *connectionTable;

/*********************************************************************
* @purpose  Finds the entry in the session table the matches
*           the specified data and returns its identifier.
*
* @param  L7_uchar8 *targetName  
* @param  L7_uchar8 *initiatorName
* @param  L7_uchar8 *isid              
*
* @returns  >=0 matching entry found
* @returns  -1  entry not found 
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbSessionRecordLookup(L7_uchar8 *targetName,
                                    L7_uchar8 *initiatorName,
                                    L7_uchar8 *isid)
{
  L7_int32 i = -1;
  L7_BOOL  found = L7_FALSE;

  while ((found != L7_TRUE) && (iscsiDbSessionIterate(i, &i) == L7_SUCCESS))
  {
    if ((sessionTable[i].inUse == L7_TRUE) &&
        (memcmp(sessionTable[i].isid, isid, sizeof(sessionTable[i].isid)) == 0) &&
        (osapiStrncmp(sessionTable[i].targetName, targetName, sizeof(sessionTable[i].targetName)) == 0) &&
        (osapiStrncmp(sessionTable[i].initiatorName, initiatorName, sizeof(sessionTable[i].initiatorName)) == 0))
    {
      found = L7_TRUE;
    }
  }
  if (found == L7_TRUE)
    return(i);
  else
    return(-1);
}

/*********************************************************************
* @purpose  Creates an entry in the Session Record table for
*           the specified data.  If successful, returns the identifer
*           of the new entry.
*
* @param  L7_uchar8 *targetName  
* @param  L7_uchar8 *initiatorName    
* @param  L7_uchar8 *isid
*
* @returns  >=0 entry created
* @returns  -1  entry not created/table full 
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbSessionRecordCreate(L7_uchar8 *targetName,
                                    L7_uchar8 *initiatorName,
                                    L7_uchar8 *isid)
{
  L7_uint32 i;

  for (i=0; i<L7_ISCSI_MAX_SESSIONS; i++)
  {
    if (connectionTable[i].inUse != L7_TRUE)
    {
      break;
    }
  }

  if (i < L7_ISCSI_MAX_SESSIONS)
  {
    sessionTable[i].inUse = L7_TRUE;
    (void)osapiStrncpySafe(sessionTable[i].targetName, targetName, sizeof(sessionTable[i].targetName));
    (void)osapiStrncpySafe(sessionTable[i].initiatorName, initiatorName, sizeof(sessionTable[i].initiatorName));
    memcpy(sessionTable[i].isid, isid, sizeof(sessionTable[i].isid));
    osapiClockTimeRaw(&sessionTable[i].startTime);
    memcpy(&sessionTable[i].lastActivityTime, &sessionTable[i].startTime, sizeof(L7_clocktime));
    return(i);
  }
  else
    return(-1);
}

/*********************************************************************
* @purpose  Internal function to delete an entry in the Session Record table for
*           the specified identifer.
*
* @param    L7_uint32 sessionId  
*
* @returns  void
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiDbSessionRecordDelete(L7_uint32 sessionId)
{
  if (sessionTable[sessionId].inUse != L7_TRUE)
  {
    ISCSI_TRACE(ISCSI_TRACE_DATABASE, "Attempted to delete an empty session record: sessionId=%d\n", sessionId);
  }

  memset(&sessionTable[sessionId], 0, sizeof(iscsiSessionRecord_t));
}

/*********************************************************************
* @purpose  Deletes an entry in the Session Record table for
*           the specified identifer. Includes range checking.
*
* @param    L7_uint32 sessionId  
*
* @returns  L7_ERROR  sessionID not valid
*           L7_SUCCESS
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionDelete(L7_uint32 sessionId)
{
  if (sessionId > L7_ISCSI_MAX_SESSIONS)
  {
    return L7_ERROR;
  }

  iscsiDbSessionRecordDelete(sessionId);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Finds the entry in the connection table the matches
*           the specified data.
*
* @param  L7_uint32 targetIpAddress  
* @param  L7_uint32 targetTcpPort    
* @param  L7_uint32 initiatorIpAddress
* @param  L7_uint32 initiatorTcpPort  
* @param  L7_uint32 cid              
*
* @returns  >=0  entry found with no errors 
* @returns  -1   no matching entry found
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbConnectionRecordLookup(L7_uint32 targetIpAddress,
                                       L7_uint32 targetTcpPort,
                                       L7_uint32 initiatorIpAddress,
                                       L7_uint32 initiatorTcpPort)
{
  L7_int32 i = -1;
  L7_BOOL   found = L7_FALSE;

  while ((found != L7_TRUE) && (iscsiDbConnectionIterate(i, &i) == L7_SUCCESS))
  {
    if ((connectionTable[i].inUse == L7_TRUE) &&
        (connectionTable[i].targetIpAddress == targetIpAddress) &&
        (connectionTable[i].targetTcpPort == targetTcpPort) &&
        (connectionTable[i].initiatorIpAddress == initiatorIpAddress) &&
        (connectionTable[i].initiatorTcpPort == initiatorTcpPort))
    {
      found = L7_TRUE;
    }
  }
  if (found == L7_TRUE)
    return(i);
  else
    return(-1);
}

/*********************************************************************
* @purpose  Creates an entry in the connection table the matches
*           the specified data.  If successful, returns the identifier
*           of the connection table record.
*
* @param  L7_uint32 targetIpAddress  
* @param  L7_uint32 targetTcpPort    
* @param  L7_uint32 initiatorIpAddress
* @param  L7_uint32 initiatorTcpPort  
* @param  L7_uint32 cid              
*
* @returns  >=0  entry created with no errors 
* @returns  -1   no matching entry found
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbConnectionRecordCreate(L7_uint32 targetIpAddress,
                                       L7_uint32 targetTcpPort,
                                       L7_uint32 initiatorIpAddress,
                                       L7_uint32 initiatorTcpPort,
                                       L7_uint32 cid,
                                       L7_uint32 sessionTableIndex,
                                       L7_uint32 targetTcpPortTableId)
{
  L7_uint32 i;

  for (i=0; i<L7_ISCSI_MAX_CONNECTIONS; i++)
  {
    if (connectionTable[i].inUse != L7_TRUE)
    {
      break;
    }
  }

  if (i < L7_ISCSI_MAX_CONNECTIONS)
  {
    connectionTable[i].inUse = L7_TRUE;
    connectionTable[i].targetIpAddress = targetIpAddress;
    connectionTable[i].targetTcpPort = targetTcpPort;
    connectionTable[i].initiatorIpAddress = initiatorIpAddress;
    connectionTable[i].initiatorTcpPort = initiatorTcpPort;
    connectionTable[i].cid = cid;
    connectionTable[i].targetPortTableId = targetTcpPortTableId;
    connectionTable[i].sessionIndex = sessionTableIndex;
    sessionTable[sessionTableIndex].connectionCount++;
    iscsiDbSessionActivityTimeUpdate(sessionTableIndex);    /* adding a connection counts as activity */
    return(i);
  }
  else
    return(-1);
}

/*********************************************************************
* @purpose  Internal function to delete an entry in the Connection Record table for
*           the specified identifer.
*
* @param    L7_uint32 sessionId  
*
* @returns  void
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiDbConnectionRecordDelete(L7_uint32 connectionId)
{
  if (connectionTable[connectionId].inUse != L7_TRUE)
  {
    ISCSI_TRACE(ISCSI_TRACE_DATABASE, "Attempted to delete an empty connection record: connectionId=%d\n", connectionId);
  }

  if (sessionTable[connectionTable[connectionId].sessionIndex].connectionCount > 0)
  {
    sessionTable[connectionTable[connectionId].sessionIndex].connectionCount--;
  }
  else
  {
    ISCSI_TRACE(ISCSI_TRACE_DATABASE, "Attempted to delete connection record from session with 0 connections: connectionId=%d, sessionId=%d\n", 
                connectionId, connectionTable[connectionId].sessionIndex);
  }
  memset(&connectionTable[connectionId], 0, sizeof(iscsiConnectionRecord_t));
}

/*********************************************************************
* @purpose  Creates a connection table entry for the data provided.
*
* @param  L7_uchar8 *targetName,        
* @param  L7_uchar8 *initiatorName     
* @param  L7_uchar8 *isid
* @param  L7_uint32 targetIpAddress
* @param  L7_uint32 targetTcpPort     
* @param  L7_uint32 initiatorIpAddress
* @param  L7_uint32 initiatorTcpPort  
* @param  L7_uint32 cid    
* @param  L7_uint32 targetTcpPortTableId
*
* @returns  L7_SUCCESS  entry created with no errors 
* @returns  L7_FAILURE  entry not created
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionCreate(L7_uchar8 *targetName,
                                L7_uchar8 *initiatorName,
                                L7_uchar8 *isid,
                                L7_uint32 targetIpAddress,
                                L7_uint32 targetTcpPort,
                                L7_uint32 initiatorIpAddress,
                                L7_uint32 initiatorTcpPort,
                                L7_uint32 cid,
                                L7_int32  *connectionId,
                                L7_int32  *sessionId,
                                L7_uint32 targetTcpPortTableId)
{
  L7_int32                 sessionIndex;
  L7_int32                 connectionIndex;
  L7_BOOL                  createdNewSession = L7_FALSE;

  if ((connectionIndex = iscsiDbConnectionRecordLookup(targetIpAddress, targetTcpPort, initiatorIpAddress, initiatorTcpPort)) >= 0)
  {
    (void)iscsiDbSessionActivityTimeUpdate(connectionTable[connectionIndex].sessionIndex);    
    *connectionId = connectionIndex;
    *sessionId = connectionTable[connectionIndex].sessionIndex;
    return L7_SUCCESS;
  }
  else
  {
    if ((sessionIndex = iscsiDbSessionRecordLookup(targetName, initiatorName, isid)) < 0)
    {
      if ((sessionIndex = iscsiDbSessionRecordCreate(targetName, initiatorName, isid)) < 0)
      {
        /* session table is full */
        return L7_FAILURE;
      }
      else
      {
        createdNewSession = L7_TRUE;
      }
    }
    if ((connectionIndex = iscsiDbConnectionRecordCreate(targetIpAddress, targetTcpPort, initiatorIpAddress, initiatorTcpPort, cid, 
                                                         sessionIndex, targetTcpPortTableId)) < 0)
    {
      /* connection table is full */
      if (createdNewSession == L7_TRUE)
      {
        iscsiDbSessionRecordDelete(sessionIndex);
      }
      return L7_FAILURE;
    }
    else
    {
      *sessionId = connectionTable[connectionIndex].sessionIndex;
      *connectionId = connectionIndex;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deletes an entry in the Connection Record table for
*           the specified identifer. Includes range checking.
*
* @param    L7_uint32 connectionId  
*
* @returns  L7_ERROR  connectionID not valid
*           L7_SUCCESS
*
* @notes    None
*
* @end
*********************************************************************/
void iscsiDbConnectionDelete(L7_uint32 connectionId)
{
  L7_int32                 sessionIndex;
  L7_int32                 connectionCount;

  sessionIndex = iscsiDbConnectionSessionIdGet(connectionId);
  iscsiDbConnectionRecordDelete(connectionId);
  if (sessionIndex >= 0)
  {
    if ((iscsiDbSessionConnectionCountGet(sessionIndex, &connectionCount) == L7_SUCCESS) && (connectionCount == 0))
    {
      iscsiDbSessionRecordDelete(sessionIndex);
    }
  }
  return;
}

/*********************************************************************
* @purpose  Returns whether one session table record is less than or
*           greater than for the purpose of walking the table.  The 
*           ordering criteria is target name, initiator name, isid.
*
* @param    iscsiSessionRecord_t *r1 first record 
*           iscsiSessionRecord_t *r2 second record
*
* @returns  -1  first record is less than second
*            0  records are equal
*            1  first record is greater than second
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbSessionRecordCompare(iscsiSessionRecord_t *r1, iscsiSessionRecord_t *r2)
{
  L7_int32 result;
  L7_int32 targetNameResult;
  L7_int32 initiatorNameResult;

  targetNameResult = osapiStrncmp(r1->targetName, r2->targetName, sizeof(r1->targetName));
  if (targetNameResult < 0)
  {
    result = -1;
  }
  else if (targetNameResult > 0)
  {
    result = 1;
  }
  else  /* targetName is equal, look at next precendence field */
  {
    initiatorNameResult = osapiStrncmp(r1->initiatorName, r2->initiatorName, sizeof(r1->initiatorName));
    if (initiatorNameResult < 0)
    {
      result = -1;
    }
    else if (initiatorNameResult > 0)
    {
      result = 1;
    }
    else  /* initiatorName is equal, look at next precendence field */
    {
      result = memcmp(r1->isid, r2->isid, sizeof(r1->isid));
    }
  }
  return(result);
}

/*********************************************************************
* @purpose  Returns whether a session table record is in use.
*
* @param    L7_uint32     sessionId  record identifier
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiDbSessionIdInUse(L7_uint32 sessionId)
{
  if (sessionId < L7_ISCSI_MAX_SESSIONS)
  {
    return(sessionTable[sessionId].inUse);
  }
  return(L7_FALSE);
}

/*********************************************************************
* @purpose  Returns the next entry in the session table, if any, after
*           the one specified.  There is no guarenteed order to how
*           the records are returned.  This function is used to iterate
*           through the table as efficiently as possible.
*
* @param    L7_int32      sessionId  record identifier or current entry, 
*                                    set to -1 for first
*           L7_int32      *nextSessionId set to identifier of next entry if
*                                        found
*
* @returns  L7_SUCCESS   next entry found
*           L7_FAILURE   no next entry found
*           L7_ERROR     problem with calling parameters
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionIterate(L7_int32 sessionId, L7_int32 *nextSessionId)
{
  L7_uint32                 i;

  /* check inputs */
  if ((sessionId >= L7_ISCSI_MAX_SESSIONS) || (nextSessionId == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  if (sessionId < 0)
    i = 0;
  else
    i = sessionId + 1;

  for (; i<L7_ISCSI_MAX_SESSIONS; i++)
  {
    if (sessionTable[i].inUse == L7_TRUE)
    {
      break;
    }
  }

  if (i < L7_ISCSI_MAX_SESSIONS)
  {
    *nextSessionId = i;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 *sessionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionFirstGet(L7_uint32 *sessionId)
{
  L7_uint32                 i;
  L7_int32                  bestMatchIndex;

  bestMatchIndex = -1;

  /* scan for first occupied record */
  for (i=0; i<L7_ISCSI_MAX_SESSIONS; i++)
  {
    if (sessionTable[i].inUse == L7_TRUE)
    {
      bestMatchIndex = i;
      break;
    }
  }
  /* if we found the first occupied record, scan the rest for one of lesser value */
  if (bestMatchIndex != -1)
  {
    for (; i<L7_ISCSI_MAX_SESSIONS; i++)
    {
      if (sessionTable[i].inUse == L7_TRUE)
      {
        if (iscsiDbSessionRecordCompare(&sessionTable[i], &sessionTable[bestMatchIndex]) < 0)
        {
          bestMatchIndex = i;
        }
      }
    }
  }

  if (bestMatchIndex != -1)
  {
    *sessionId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  L7_uint32                 i;
  L7_int32                  bestMatchIndex;

  /* check inputs */
  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || (nextSessionId == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  bestMatchIndex = -1;

  if (sessionTable[sessionId].inUse == L7_TRUE)
  {
    for (i=0; i<L7_ISCSI_MAX_SESSIONS; i++)
    {
      if ((i != sessionId) && (sessionTable[i].inUse == L7_TRUE))
      {
        if (iscsiDbSessionRecordCompare(&sessionTable[i], &sessionTable[sessionId]) > 0)
        {
          if (bestMatchIndex == -1)
          {
            bestMatchIndex = i;
          }
          else
          {
            if (iscsiDbSessionRecordCompare(&sessionTable[i], &sessionTable[bestMatchIndex]) < 0)
            {
              bestMatchIndex = i;
            }
          }
        }
      }
    }
  }

  if (bestMatchIndex != -1)
  {
    *nextSessionId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the target IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || 
      (name == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (sessionTable[sessionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(name, sessionTable[sessionId].targetName, sizeof(sessionTable[sessionId].targetName));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the initiator IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || 
      (name == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (sessionTable[sessionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(name, sessionTable[sessionId].initiatorName, sizeof(sessionTable[sessionId].initiatorName));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the start time for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_clocktime *time   (output) the system time when the session 
*                                was detected
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time)
{
  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || 
      (time == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (sessionTable[sessionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  memcpy(time, &sessionTable[sessionId].startTime, sizeof(L7_clocktime));
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the time since last data traffic for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uint32 *seconds   (output) seconds since last seen data for session 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds)
{
  L7_clocktime now;

  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || 
      (seconds == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (sessionTable[sessionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiClockTimeRaw(&now);
  *seconds = now.seconds - sessionTable[sessionId].lastActivityTime.seconds;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the ISID for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *isid      (output) the ISID associated with the specified
*                                 session
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid)
{
  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || 
      (isid == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (sessionTable[sessionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  memcpy(isid, sessionTable[sessionId].isid, sizeof(sessionTable[sessionId].isid));
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Updates the counter data for the session table entry
*           corresponding to the identifier given.
*
* @param  L7_uint32 sessionId     an identifier of the table entry
* @param  L7_ulong64 *countData   the new counter value
*
* @returns  L7_SUCCESS  counter updated 
* @returns  L7_FAILURE  entry not valid
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionCounterSet(L7_uint32 sessionId, L7_ulong64 *countData)
{
  L7_RC_t rc = L7_FAILURE;

  /* check inputs */
  if (sessionId < L7_ISCSI_MAX_SESSIONS)
  {
    if (sessionTable[sessionId].inUse == L7_TRUE)
    {
      sessionTable[sessionId].counterData.high = countData->high;
      sessionTable[sessionId].counterData.low = countData->low;
      rc = L7_SUCCESS;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the counter data for the session table entry
*           corresponding to the identifier given.
*
* @param  L7_uint32 sessionId     an identifier of the table entry
* @param  L7_ulong64 *countData   the counter value
*
* @returns  L7_SUCCESS  counter retrieved 
* @returns  L7_FAILURE  entry not valid
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionCounterGet(L7_uint32 sessionId, L7_ulong64 *countData)
{
  L7_RC_t rc = L7_FAILURE;

  /* check inputs */
  if (sessionId < L7_ISCSI_MAX_SESSIONS)
  {
    if (sessionTable[sessionId].inUse == L7_TRUE)
    {
      countData->low = sessionTable[sessionId].counterData.low;
      countData->high = sessionTable[sessionId].counterData.high;
      rc = L7_SUCCESS;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Updates the time since last activity for the session table entry
*           corresponding to the identifier given.
*
* @param  L7_uint32 sessionId     an identifier of the table entry
*
* @returns  L7_SUCCESS  time value cleared 
* @returns  L7_FAILURE  entry not valid
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionActivityTimeUpdate(L7_uint32 sessionId)
{
  L7_RC_t rc = L7_FAILURE;

  /* check inputs */
  if (sessionId < L7_ISCSI_MAX_SESSIONS)
  {
    if (sessionTable[sessionId].inUse == L7_TRUE)
    {
      osapiClockTimeRaw(&sessionTable[sessionId].lastActivityTime);
      rc = L7_SUCCESS;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Returns the number of connections associated with the session table entry
*           corresponding to the identifier given.
*
* @param  L7_uint32 sessionId     an identifier of the table entry
*         L7_uint32 *count        place to put the data
*
* @returns  L7_SUCCESS  data retrieved 
* @returns  L7_ERROR    entry not valid
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionConnectionCountGet(L7_uint32 sessionId, L7_uint32 *count)
{
  /* check inputs */
  if ((sessionId > L7_ISCSI_MAX_SESSIONS) || (count == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  *count = sessionTable[sessionId].connectionCount;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns whether one connection table record is less than or
*           greater than for the purpose of walking the table.  The 
*           ordering criteria is initiator IP address, initiator TCP port, 
*           target IP address, target TCP port, cid.
*
* @param    iscsiConnectionRecord_t *r1 first record 
*           iscsiConnectionRecord_t *r2 second record
*
* @returns  -1  first record is less than second
*            0  records are equal
*            1  first record is greater than second
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbConnectionRecordCompare(iscsiConnectionRecord_t *r1, iscsiConnectionRecord_t *r2)
{
  L7_int32 result;

  if (r1->initiatorIpAddress < r2->initiatorIpAddress)
  {
    result = -1;
  }
  else if (r1->initiatorIpAddress > r2->initiatorIpAddress)
  {
    result = 1;
  }
  else  /* initiatorIpAddress is equal, look at next precendence field */
  {
    if (r1->initiatorTcpPort < r2->initiatorTcpPort)
    {
      result = -1;
    }
    else if (r1->initiatorTcpPort > r2->initiatorTcpPort)
    {
      result = 1;
    }
    else  /* initiatorTcpPort is equal, look at next precendence field */
    {
      if (r1->targetIpAddress < r2->targetIpAddress)
      {
        result = -1;
      }
      else if (r1->targetIpAddress > r2->targetIpAddress)
      {
        result = 1;
      }
      else  /* initiatorTcpPort is equal, look at next precendence field */
      {
        if (r1->targetTcpPort < r2->targetTcpPort)
        {
          result = -1;
        }
        else if (r1->targetTcpPort > r2->targetTcpPort)
        {
          result = 1;
        }
        else
        {
          if (r1->cid < r2->cid)
          {
            result = -1;
          }
          else if (r1->cid > r2->cid)
          {
            result = 1;
          }
          else
          {
            result = 0;
          }
        }
      }
    }
  }
  return(result);
}

/*********************************************************************
* @purpose  Returns whether a connection table record is in use.
*
* @param    L7_uint32     connectionId  record identifier
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiDbConnectionIdInUse(L7_uint32 connectionId)
{
  if (connectionId < L7_ISCSI_MAX_CONNECTIONS)
  {
    return(connectionTable[connectionId].inUse);
  }
  return(L7_FALSE);
}


/*********************************************************************
* @purpose  Returns the next entry in the connection table, if any, after
*           the one specified.  There is no guarenteed order to how
*           the records are returned.  This function is used to iterate
*           through the table as efficiently as possible.
*
* @param    L7_int32     connectionId  record identifier or current entry, 
*                                       set to -1 for first
*           L7_int32     *nextConnectionId set to identifier of next entry if
*                                           found
*
* @returns  L7_SUCCESS   next entry found
*           L7_FAILURE   no next entry found
*           L7_ERROR     problem with calling parameters
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionIterate(L7_int32 connectionId, L7_int32 *nextConnectionId)
{
  L7_uint32                 i;

  /* check inputs */
  if ((connectionId >= L7_ISCSI_MAX_CONNECTIONS) || (nextConnectionId == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  if (connectionId < 0)
    i = 0;
  else
    i = connectionId + 1;

  for (; i<L7_ISCSI_MAX_CONNECTIONS; i++)
  {
    if (connectionTable[i].inUse == L7_TRUE)
    {
      break;
    }
  }

  if (i < L7_ISCSI_MAX_CONNECTIONS)
  {
    *nextConnectionId = i;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 *connectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId)
{
  L7_uint32                 i;
  L7_int32                  bestMatchIndex;

  bestMatchIndex = -1;

  /* scan for first occupied record */
  for (i=0; i<L7_ISCSI_MAX_CONNECTIONS; i++)
  {
    if ((connectionTable[i].inUse == L7_TRUE) &&
        (connectionTable[i].sessionIndex == sessionId))
    {
      bestMatchIndex = i;
      break;
    }
  }
  /* if we found the first occupied record, scan the rest for one of lesser value */
  if (bestMatchIndex != -1)
  {
    for (; i<L7_ISCSI_MAX_SESSIONS; i++)
    {
      if ((connectionTable[i].inUse == L7_TRUE) && 
          (connectionTable[i].sessionIndex == sessionId))
      {
        if (iscsiDbConnectionRecordCompare(&connectionTable[i], &connectionTable[bestMatchIndex]) < 0)
        {
          bestMatchIndex = i;
        }
      }
    }
  }
  if (bestMatchIndex != -1)
  {
    *connectionId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 connectionId  (input) an identifier of the current connection table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  L7_uint32                 i;
  L7_int32                  bestMatchIndex;

  /* check inputs */
  if ((connectionId > L7_ISCSI_MAX_CONNECTIONS) || (nextConnectionId == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  bestMatchIndex = -1;

  if (connectionTable[connectionId].inUse == L7_TRUE)
  {
    for (i=0; i<L7_ISCSI_MAX_CONNECTIONS; i++)
    {
      if ((i != connectionId) && 
          (connectionTable[i].inUse == L7_TRUE) && 
          (connectionTable[i].sessionIndex == sessionId))
      {
        if (iscsiDbConnectionRecordCompare(&connectionTable[i], &connectionTable[connectionId]) > 0)
        {
          if (bestMatchIndex == -1)
          {
            bestMatchIndex = i;
          }
          else
          {
            if (iscsiDbConnectionRecordCompare(&connectionTable[i], &connectionTable[bestMatchIndex]) < 0)
            {
              bestMatchIndex = i;
            }
          }
        }
      }
    }
  }

  if (bestMatchIndex != -1)
  {
    *nextConnectionId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Retrieves the target IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr)
{
  /* check inputs */
  if ((connectionId > L7_ISCSI_MAX_CONNECTIONS) || (ipAddr == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (connectionTable[connectionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  *ipAddr = connectionTable[connectionId].targetIpAddress;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the initiator IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_uint32 *ipAddr)
{
  /* check inputs */
  if ((connectionId > L7_ISCSI_MAX_CONNECTIONS) || (ipAddr == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (connectionTable[connectionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  *ipAddr = connectionTable[connectionId].initiatorIpAddress;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the target TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  /* check inputs */
  if ((connectionId > L7_ISCSI_MAX_CONNECTIONS) || (port == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (connectionTable[connectionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  *port = connectionTable[connectionId].targetTcpPort;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the initiator TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  /* check inputs */
  if ((connectionId > L7_ISCSI_MAX_CONNECTIONS) || (port == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (connectionTable[connectionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  *port = connectionTable[connectionId].initiatorTcpPort;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the iSCSI connection id (CID) for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *cid         (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiDbConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid)
{
  /* check inputs */
  if ((connectionId > L7_ISCSI_MAX_CONNECTIONS) || (cid == L7_NULLPTR))
  {
    return L7_ERROR;
  }
  if (connectionTable[connectionId].inUse != L7_TRUE)
  {
    return L7_FAILURE;
  }
  *cid = connectionTable[connectionId].cid;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Retrieves the session table id for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
*
* @returns  -1  if connectionId not valid 
* @returns  >=0 identifer of session table record
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbConnectionSessionIdGet(L7_uint32 connectionId)
{
  if ((connectionId < L7_ISCSI_MAX_CONNECTIONS) &&
      (connectionTable[connectionId].inUse == L7_TRUE))
  {
    return(connectionTable[connectionId].sessionIndex);
  }
  return(-1);
}

/*********************************************************************
* @purpose  Retrieves the target port table id for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
*
* @returns  -1  if connectionId not valid 
* @returns  >=0 identifer of target port table record
*
* @notes    None
*
* @end
*********************************************************************/
L7_int32 iscsiDbConnectionTargetTcpPortIdGet(L7_uint32 connectionId)
{
  if ((connectionId < L7_ISCSI_MAX_CONNECTIONS) &&
      (connectionTable[connectionId].inUse == L7_TRUE))
  {
    return(connectionTable[connectionId].targetPortTableId);
  }
  return(-1);
}

/*********************************************************************
* @purpose  Allocates and initializes all tables for the iSCSI session
*           database.
* @end
*********************************************************************/
L7_RC_t iscsiDbSessionDataStructureCreate(void)
{
  L7_RC_t rc = L7_FAILURE;

  connectionTable = (iscsiConnectionRecord_t *)osapiMalloc(L7_FLEX_QOS_ISCSI_COMPONENT_ID, sizeof(iscsiConnectionRecord_t) * L7_ISCSI_MAX_CONNECTIONS);
  sessionTable = (iscsiSessionRecord_t *)osapiMalloc(L7_FLEX_QOS_ISCSI_COMPONENT_ID, sizeof(iscsiSessionRecord_t) * L7_ISCSI_MAX_SESSIONS);

  if (connectionTable != L7_NULLPTR && sessionTable != L7_NULLPTR)
  {
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose  Frees and initializes all tables for the iSCSI session
*           database.
* @end
*********************************************************************/
void iscsiDbSessionDataStructureDestroy(void)
{
  /* free any memory allocated */
  if (connectionTable != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ISCSI_COMPONENT_ID, connectionTable);
  }
  if (sessionTable != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ISCSI_COMPONENT_ID, sessionTable);
  }
}

/* debug functions */
void iscsiDbDebugTableStorageShow(void)
{
  sysapiPrintf("iSCSI database storage:\n");
  sysapiPrintf("  Session table    = %u bytes\n", sizeof(iscsiSessionRecord_t) * L7_ISCSI_MAX_SESSIONS);
  sysapiPrintf("  Connection table = %u bytes\n", sizeof(iscsiConnectionRecord_t) * L7_ISCSI_MAX_CONNECTIONS);
}

