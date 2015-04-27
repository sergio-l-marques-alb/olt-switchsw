/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_event.c
*
* @purpose  this is the core file for RMON Event group
*           
* @component SNMP-RMON
*
*
* @create  08/13/2001
*
* @author  K Pragash 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


 
#include <rmon_common.h>
#include <rmon_event.h>
#include <rmon_index.h>
#include "usmdb_common.h"
#include "usmdb_trapmgr_api.h"

/* These have been declared in snmp_rmon.c */
extern rmonEventEntry_t *rmonEventEntryArray;


/* The SNMP to array inde mapping handle.
*/
static L7_uint32 eventMap;



/*********************************************************************
*
* @purpose    Initialize the RMON event group.
*
* @returntype L7_SUCCESS    Event group is initialized.
*             L7_FAILURE    Initialization failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventTableInitialize(void)
{
    L7_uint32 i;
    rmonEventEntry_t* evnt;
    eventMap = rmonMapCreate(RMON_EVENT_ENTRY_MAX_NUM);
    
    for(i = 0; i < RMON_EVENT_ENTRY_MAX_NUM; i++)
    {
        evnt = &rmonEventEntryArray[i];
        memset (evnt, 0, sizeof (rmonEventEntry_t));
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    Event Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventTableUnconfigure(void)
{
    L7_uint32 index = 0;
    L7_RC_t rc = L7_SUCCESS;

    /* iterate over all entries in the table */
    while (rmonEventEntryNextGet(&index) == L7_SUCCESS)
    {
      if (rmonEventEntryDelete(index) != L7_SUCCESS)
      {
        /* one of the removals failed for some reason */
        rc = L7_FAILURE;
      }

      /* set the index back to 0 to ensure we get the first entry every 
         time this ensures reindexing doesn't affect table navigation */
      index = 0;
    }
    return rc;
}

/*********************************************************************
*
* @purpose    Delete the EventEntry for  the specified index
*
* @param      Index         Index of EventEntry to be deleted
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventEntryDelete(L7_uint32 Index)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    /* Resolve array index from the SNMP index.
    */
    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    evnt = &rmonEventEntryArray[array_index];
    memset (evnt, 0, sizeof (rmonEventEntry_t));

    rc =  rmonMapRemove (eventMap, Index);

    if (rc != L7_SUCCESS) 
        return L7_FAILURE;
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose    Creates the default EventEntry for  the specified index
*
* @param      Index         Index of EventEntry
*             evnt           struct of EventEntry
*             status        L7_uint32 to be set as default status
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventEntryDefaultsSet(rmonEventEntry_t* evnt, L7_uint32 Index, L7_uint32 status)   
{
    char* tempDescription = FD_RMON_EVENT_DESCRIPTION;
    char* tempCommunity   = FD_RMON_EVENT_COMMUNITY;
    char* tempOwner       = FD_RMON_EVENT_OWNER;
    L7_uint32 logCount;
    bzero(evnt->description, RMON_EVENT_ENTRY_DESCRIPTION_MAX_LENGTH+1);
    bzero(evnt->owner, RMON_EVENT_ENTRY_OWNER_MAX_LENGTH+1);
    bzero(evnt->community, RMON_EVENT_ENTRY_COMMUNITY_MAX_LENGTH+1);
    if(evnt == L7_NULLPTR)
       return L7_FAILURE;
    evnt->index = Index;
    memcpy(evnt->description, tempDescription, strlen(tempDescription));
    evnt->type = RMON_EVENT_ENTRY_TYPE_NONE;
    memcpy(evnt->community, tempCommunity, strlen(tempCommunity));
	evnt->lastTimeSent = 0;
    memcpy(evnt->owner, tempOwner, strlen(tempOwner));
	evnt->status = status;
    evnt->lastLogIndex = 0;
    evnt->logQueueIndex = 0;
    evnt->validEntry = L7_TRUE;
    for(logCount = 1; logCount <= RMON_LOG_QUEUE_INDEX_MAX_NUM; logCount++)
    {
        evnt->logQueue[logCount].logEventIndex = evnt->index;
        evnt->logQueue[logCount].logIndex = 0;
        evnt->logQueue[logCount].logTime = 0;
        evnt->logQueue[logCount].validEntry = L7_FALSE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Checks index for the valid Event Entry
*
* @param      Index         Index of EventEntry to check
*             searchType    L7_uint32 (Get or GetNext or Set)
*             integerValue  L7_int32  (depends on get or set request)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventEntryGet(L7_uint32 searchType, L7_uint32 Index)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);

    if (rc == L7_SUCCESS)
    {
      evnt = &rmonEventEntryArray[array_index];

      if (evnt->index != Index)
        rc = L7_FAILURE;
    }

    return rc;
}
#ifdef OLD_CODE
L7_RC_t 
rmonEventEntryGet(L7_uint32 searchType, L7_int32 integerValue, L7_uint32 Index)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    if(searchType == L7_MATCH_RMON_EXACTSET && integerValue == RMON_EVENT_ENTRY_STATUS_CREATEREQUEST)
    {
        return L7_SUCCESS;
    }

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];

    /* integerValue is having the value of nominator if it is GET or GETNEXT operation */

    if(searchType == L7_MATCH_RMON_EXACTGET || searchType == L7_MATCH_RMON_GETNEXT)
    {
        if ((integerValue >= 0) && (evnt->index == Index) &&
            ((evnt->status == RMON_EVENT_ENTRY_STATUS_VALID) ||
             (evnt->status == RMON_EVENT_ENTRY_STATUS_UNDERCREATION)))
            return L7_SUCCESS;

        if ((integerValue < 0) && (evnt->index == Index) &&
            ((evnt->status == RMON_EVENT_ENTRY_STATUS_VALID) ||
             (evnt->status == RMON_EVENT_ENTRY_STATUS_UNDERCREATION) ||
             (evnt->status == RMON_EVENT_ENTRY_STATUS_INVALID)))
        {
            evnt->validEntry = L7_TRUE;
            return L7_SUCCESS;
        }

    }
    /* integerValue is having the value of STATUS of the input Event entry used for set */
    /* if it is SET operation */

    if(searchType == L7_MATCH_RMON_EXACTSET)
    {
        if ((integerValue != RMON_EVENT_ENTRY_STATUS_CREATEREQUEST) && 
            (evnt->index==Index) && 
            ((evnt->status == RMON_EVENT_ENTRY_STATUS_VALID)||
             (evnt->status == RMON_EVENT_ENTRY_STATUS_UNDERCREATION)))
            return L7_SUCCESS;
    }
    return L7_FAILURE;
}
#endif
/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      Index         Index of EventEntry to check
*             
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventEntryNextGet(L7_uint32 *Index)
{

    L7_RC_t rc;
    L7_uint32 next_snmp_index;

    rc =  rmonMapNextGet (eventMap, *Index, &next_snmp_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    *Index = next_snmp_index;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Generates Event as per the alarm raised
*
* @param      Index         index of EventEntry
*             almIndex      index of alarm entry
*             flag          determines the raising or falling event
*
* @returntype L7_SUCCESS  if member was generated
*             L7_FAILURE  if member was not generated
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventGenerate(L7_uint32 almIndex, L7_uint32 Index, L7_uint32 flag)
{
    rmonEventEntry_t* evnt;
    L7_uint32 logCount;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    evnt = &rmonEventEntryArray[array_index];
    if(evnt->status == RMON_EVENT_ENTRY_STATUS_VALID)
    {
        evnt->lastTimeSent = osapiUpTimeRaw();
        if((evnt->type == RMON_EVENT_ENTRY_TYPE_LOG) || (evnt->type == RMON_EVENT_ENTRY_TYPE_LOGTRAP))
        {
            evnt->lastLogIndex++;

            if(evnt->lastLogIndex > RMON_LOG_INDEX_MAX_NUM)
                return L7_FAILURE;
            evnt->logQueueIndex++;
            if(evnt->logQueueIndex > RMON_LOG_QUEUE_INDEX_MAX_NUM)
            {
                for(logCount = 1; logCount < RMON_LOG_QUEUE_INDEX_MAX_NUM; logCount++)
                {
                    evnt->logQueue[logCount].logIndex = evnt->logQueue[logCount+1].logIndex;
                    evnt->logQueue[logCount].logTime = evnt->logQueue[logCount+1].logTime;
                    evnt->logQueue[logCount].validEntry =
                                            evnt->logQueue[logCount+1].validEntry;
                    bzero(evnt->logQueue[logCount].description, RMON_LOG_ENTRY_DESCRIPTION_MAX_LENGTH+1);
                    memcpy(evnt->logQueue[logCount].description, evnt->logQueue[logCount+1].description, 
                                            strlen(evnt->logQueue[logCount+1].description));
                }
                evnt->logQueueIndex =  RMON_LOG_QUEUE_INDEX_MAX_NUM;
            }
            if((rmonLogEntryGenerate(evnt, evnt->logQueueIndex)) != L7_SUCCESS)
                return L7_FAILURE;
        }
        if((evnt->type == RMON_EVENT_ENTRY_TYPE_TRAP) || (evnt->type == RMON_EVENT_ENTRY_TYPE_LOGTRAP))
        {
            if((rmonTrapGenerate(almIndex, flag)) != L7_SUCCESS)
                return L7_FAILURE;
        }
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of Description for Event Entry
*
* @param      Index         index of EventEntry
*             buf(output)    pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventDescriptionGet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if((buf != L7_NULLPTR) && (evnt->validEntry != L7_FALSE))
    {
        bzero(buf, RMON_EVENT_ENTRY_DESCRIPTION_MAX_LENGTH+1);
        memcpy(buf, evnt->description, strlen(evnt->description));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of EventType for EventEntry
*
* @param      Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventTypeGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(evnt->validEntry != L7_FALSE)
    {
        *val = evnt->type;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of community for Event Entry
*
* @param      Index         index of EventEntry
*             buf(output)   pointer to L7_uchar8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventCommunityGet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if((buf != L7_NULLPTR) && (evnt->validEntry != L7_FALSE))
    {
        bzero(buf, RMON_EVENT_ENTRY_COMMUNITY_MAX_LENGTH+1);
        memcpy(buf, evnt->community, strlen(evnt->community));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of lastTimeSent for Event Entry
*
* @param      Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventLastTimeSentGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(evnt->validEntry != L7_FALSE)
    {
        *val = evnt->lastTimeSent * 100;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of Owner for Event Entry
*
* @param      Index         index of EventEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventOwnerGet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if((buf != L7_NULLPTR) && (evnt->validEntry != L7_FALSE))
    {
        bzero(buf, RMON_EVENT_ENTRY_OWNER_MAX_LENGTH+1);
        memcpy(buf, evnt->owner, strlen(evnt->owner));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of Status for Event Entry
*
* @param      Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventStatusGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(evnt->validEntry != L7_FALSE)
    {
        *val = evnt->status;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/* SET Requests for Alarm group*/


/*********************************************************************
*
* @purpose    Sets value of Description for Event entry
*
* @param      Index         index of the EventEntry
*             buf(input)    Pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventDescriptionSet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(strcmp(evnt->description, buf) == 0)
        return L7_SUCCESS;
    if(buf != L7_NULLPTR)
    {
        if(strlen(buf) > RMON_EVENT_ENTRY_DESCRIPTION_MAX_LENGTH)
            return L7_FAILURE;

        bzero(evnt->description, RMON_EVENT_ENTRY_DESCRIPTION_MAX_LENGTH+1);
        memcpy(evnt->description, buf, strlen(buf));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of EventType for Event entry
*
* @param      Index         index of the EventEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventTypeSet(L7_uint32 Index, L7_uint32 val)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(evnt->type == val)
        return L7_SUCCESS;
    if ((val == RMON_EVENT_ENTRY_TYPE_NONE) || (val == RMON_EVENT_ENTRY_TYPE_LOG) ||
        (val == RMON_EVENT_ENTRY_TYPE_TRAP) || (val == RMON_EVENT_ENTRY_TYPE_LOGTRAP)) 
    {
        evnt->type = val;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/*********************************************************************
*
* @purpose    Sets value of community for Event entry
*
* @param      Index         index of the EventEntry
*             buf(input)    Pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventCommunitySet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(strcmp(evnt->community, buf) == 0)
        return L7_SUCCESS;
    if(buf != L7_NULLPTR)
    {
        if(strlen(buf) > RMON_EVENT_ENTRY_COMMUNITY_MAX_LENGTH)
            return L7_FAILURE;

        bzero(evnt->community, RMON_EVENT_ENTRY_COMMUNITY_MAX_LENGTH+1);
        memcpy(evnt->community, buf, strlen(buf));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of Owner for Event entry
*
* @param      Index         index of the EventEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventOwnerSet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    evnt = &rmonEventEntryArray[array_index];
    if(strcmp(evnt->owner, buf) == 0)
        return L7_SUCCESS;
    if(buf != L7_NULLPTR)
    {
        if(strlen(buf) > RMON_EVENT_ENTRY_OWNER_MAX_LENGTH)
            return L7_FAILURE;
        bzero(evnt->owner, RMON_EVENT_ENTRY_OWNER_MAX_LENGTH+1);
        memcpy(evnt->owner, buf, strlen(buf));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/*********************************************************************
*
* @purpose    Sets value of Status for Event entry
*
* @param      Index         index of the EventEntry
*             val(input)    L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventStatusSet(L7_uint32 Index, L7_uint32 val)
{
    rmonEventEntry_t* evnt;
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 array_index;
    L7_RC_t rc1;

    if ((val == RMON_EVENT_ENTRY_STATUS_VALID) ||
        (val == RMON_EVENT_ENTRY_STATUS_UNDERCREATION) ||
        (val == RMON_EVENT_ENTRY_STATUS_INVALID))
    {
        rc1 = rmonMapMatch (eventMap, Index, &array_index);
        if (rc1 != L7_SUCCESS) 
            return L7_FAILURE;

        evnt = &rmonEventEntryArray[array_index];
        if (evnt->status == val)
                return L7_SUCCESS;
    }
    else {
      if (val == RMON_EVENT_ENTRY_STATUS_CREATEREQUEST) {
          rc1 = rmonMapMatch (eventMap, Index, &array_index);
          if (rc1 == L7_SUCCESS) {
              return L7_FAILURE;
          }

          rc1 = rmonMapInsert (eventMap, Index);
          if (rc1 != L7_SUCCESS) {
              return L7_FAILURE;
          }

          rc1 = rmonMapMatch (eventMap, Index, &array_index);
          if (rc1 != L7_SUCCESS) {
              return L7_FAILURE;
          }
          evnt = &rmonEventEntryArray[array_index];

          rmonEventEntryDefaultsSet(evnt, Index, RMON_EVENT_ENTRY_STATUS_UNDERCREATION);
          return L7_SUCCESS;

      } else {
          return L7_FAILURE;
      }
  }


  switch (evnt->status)
     {
      case RMON_EVENT_ENTRY_STATUS_VALID:
          switch (val) {
          case RMON_EVENT_ENTRY_STATUS_UNDERCREATION:
              evnt->status = val;
              evnt->validEntry = L7_TRUE;
              rc = L7_SUCCESS;
            break;
          case RMON_EVENT_ENTRY_STATUS_INVALID:
              memset (evnt, 0, sizeof (rmonEventEntry_t));
              rc1 =  rmonMapRemove (eventMap, Index);
              rc = L7_SUCCESS;
            break;
          default:
             rc = L7_FAILURE;
          }
      break;

       case RMON_EVENT_ENTRY_STATUS_UNDERCREATION:
           switch (val) 
           {
            case RMON_EVENT_ENTRY_STATUS_VALID:
                    evnt->status = val;
                    evnt->validEntry = L7_TRUE;
                    rc = L7_SUCCESS;
                    break;
           case RMON_EVENT_ENTRY_STATUS_INVALID:
                    memset (evnt, 0, sizeof (rmonEventEntry_t));
                    rc1 =  rmonMapRemove (eventMap, Index);
                    rc = L7_SUCCESS;
                    break;
            default:
                       rc = L7_FAILURE;
                    }
                    break;
            }
    return rc;
}

 
/***************** FOR LOG TABLE ********************************/


/*********************************************************************
*
* @purpose    Generates the LogEntry for  the specified index
*
* @param      evnt                      pointer to eventEntry
*             queueIndex                L7_uint32 (index of the log Queue)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonLogEntryGenerate(rmonEventEntry_t* evnt, L7_uint32 queueIndex)
{
    rmonLogEntry_t* log;
    log = &evnt->logQueue[queueIndex];

    if(log == L7_NULLPTR)
        return L7_FAILURE;
    log->logEventIndex = evnt->index;
    log->logIndex = evnt->lastLogIndex;
    bzero(log->description, RMON_LOG_ENTRY_DESCRIPTION_MAX_LENGTH+1);
    memcpy(log->description, evnt->description, strlen(evnt->description));
    log->logTime =  osapiUpTimeRaw();
    log->validEntry = L7_TRUE;
    return L7_SUCCESS; 
}


/*********************************************************************
*
* @purpose    Checks index for the valid Log Entry
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             searchType    L7_uint32 (Get or GetNext or Set)
*             integerValue  L7_int32  (depends on get or set request)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonLogEntryGet(L7_uint32 searchType, L7_uint32 logEventIndex, L7_uint32 queueIndex)
{
    rmonEventEntry_t* evnt;
    rmonLogEntry_t* log;
    L7_uint32 logQueueNumber;

    L7_uint32 array_index;
    L7_RC_t rc;


    rc = rmonMapMatch (eventMap, logEventIndex, &array_index);
    if (rc != L7_SUCCESS) 
    {
        return L7_FAILURE;
    }

    evnt = &rmonEventEntryArray[array_index];
    
    if(evnt->status != RMON_EVENT_ENTRY_STATUS_VALID)
    {
        return L7_FAILURE;
    }

    if(rmonLogQueueCheck(evnt,queueIndex,&logQueueNumber) == L7_FAILURE)
    {
        return L7_FAILURE;
    }
    log = &(evnt->logQueue[logQueueNumber]);

    if((evnt->index == logEventIndex) && (log->logEventIndex == logEventIndex) && 
       (log->validEntry == L7_TRUE))
    {
        return L7_SUCCESS;
    }

    return L7_FAILURE;
    
}

/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonLogEntryNextGet(L7_uint32 *logEventIndex, L7_uint32 *queueIndex)
{
    rmonEventEntry_t* evnt;
    rmonLogEntry_t*   log;
    L7_RC_t   rc = L7_FAILURE;
    L7_uint32 logQueueNumber;

    L7_uint32 array_index;
    L7_RC_t rc1;
    L7_RC_t rc2;
    L7_uint32 next_snmp_index;
    L7_uint32 highest_snmp_index;


    rc2 = rmonMapHighestGet(eventMap, &highest_snmp_index);
    if (rc2 != L7_SUCCESS) {
       return L7_FAILURE;
    }

    rc1 = rmonMapMatch (eventMap, *logEventIndex, &array_index);
    if (rc1 != L7_SUCCESS)
    {
        rc1 =  rmonMapNextGet (eventMap, *logEventIndex, &next_snmp_index);
        if (rc1 != L7_SUCCESS) {
            return L7_FAILURE;
        }
        *logEventIndex = next_snmp_index;
    }
    while(*logEventIndex <= highest_snmp_index)
    {
        rc1 = rmonMapMatch (eventMap, *logEventIndex, &array_index);
        if (rc1 != L7_SUCCESS)
            return L7_FAILURE;

        evnt = &rmonEventEntryArray[array_index];
        if(evnt->status == RMON_EVENT_ENTRY_STATUS_VALID)
        {
            if((rmonLogQueueCheck(evnt, *queueIndex, &logQueueNumber)) == L7_FAILURE)
            {
                if(evnt->logQueue[1].validEntry == L7_TRUE &&
                   *queueIndex < (evnt->logQueue[1].logIndex))
                {
                    *queueIndex =  evnt->logQueue[1].logIndex;

                }
                else
                {
                    *queueIndex = 1;
                    rc1 =  rmonMapNextGet (eventMap, *logEventIndex, &next_snmp_index);
                    if (rc1 != L7_SUCCESS) {
                        return L7_FAILURE;
                    }
                    *logEventIndex = next_snmp_index;
                }
            }
            else
            {
                log = &(evnt->logQueue[logQueueNumber]);

                if((evnt->index == *logEventIndex) && (log->logEventIndex == *logEventIndex) && 
                    (log->validEntry == L7_TRUE) && 
                    (logQueueNumber <= RMON_LOG_QUEUE_INDEX_MAX_NUM))
                {
                    rc = L7_SUCCESS;
                    break;
                }
            }
        }
        else
        {
            rc1 =  rmonMapNextGet (eventMap, *logEventIndex, &next_snmp_index);
            if (rc1 != L7_SUCCESS) {
                return L7_FAILURE;
            }
            *logEventIndex = next_snmp_index;
            *queueIndex = 1;
        }
    } 
    return rc;
}


/*********************************************************************
*
* @purpose    Gets value of LogTime for Log Entry
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonLogTimeGet(L7_uint32 logEventIndex, L7_uint32 queueIndex, L7_uint32 *val)
{
    rmonEventEntry_t* evnt;
    rmonLogEntry_t* log;

    L7_uint32 logQueueNumber;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, logEventIndex, &array_index);
    if (rc != L7_SUCCESS) 
        return L7_FAILURE;

    evnt = &rmonEventEntryArray[array_index];

    if(evnt->status != RMON_EVENT_ENTRY_STATUS_VALID)
        return L7_FAILURE;

    if((rmonLogQueueCheck(evnt,queueIndex,&logQueueNumber)) == L7_FAILURE)
        return L7_FAILURE;
    
    log = &(evnt->logQueue[logQueueNumber]);
    
    if((evnt->index == logEventIndex) && (log->logEventIndex == logEventIndex) && 
       (log->validEntry == L7_TRUE))
    {
        *val = (log->logTime * 100);
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of LogDescription for Log Entry
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonLogDescriptionGet(L7_uint32 logEventIndex, L7_uint32 queueIndex, L7_char8 *buf)
{
    rmonEventEntry_t* evnt;
    rmonLogEntry_t* log;
    L7_uint32 logQueueNumber;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (eventMap, logEventIndex, &array_index);
    if (rc != L7_SUCCESS) 
        return L7_FAILURE;

    evnt = &rmonEventEntryArray[array_index];

    if(evnt->status != RMON_EVENT_ENTRY_STATUS_VALID)
        return L7_FAILURE;

    if((rmonLogQueueCheck(evnt,queueIndex,&logQueueNumber)) == L7_FAILURE)
        return L7_FAILURE;

    log = &(evnt->logQueue[logQueueNumber]);
    
    if((evnt->index == logEventIndex) && (log->logEventIndex == logEventIndex) && 
       (log->validEntry == L7_TRUE))
    {
        bzero(buf, RMON_LOG_ENTRY_DESCRIPTION_MAX_LENGTH+1);
        memcpy(buf, log->description, strlen(log->description));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/*********************************************************************
*
* @purpose    Generates the Trap for  the specified index
*
* @param      almIndex      index of alarm entry
*             flag          determines the raising or falling event
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonTrapGenerate(L7_uint32 almIndex, L7_uint32 flag)
{
    if(flag == RMON_FLAG_FALLING_EVENT)
    {
        if((usmDbtrapMgrFallingAlarmLogTrap(USMDB_UNIT_CURRENT, almIndex)) == L7_SUCCESS)
            return L7_SUCCESS;
    }
    if(flag == RMON_FLAG_RISING_EVENT)
    {
       if((usmDbtrapMgrRisingAlarmLogTrap(USMDB_UNIT_CURRENT, almIndex)) == L7_SUCCESS)
           return L7_SUCCESS;
    }
    return L7_FAILURE;
}



 /*********************************************************************
*
* @purpose    To check whether a log queue with the given queueIndex exist
              also it gets the corresponding logQueueNumber of the logQueue
*
* @param      evnt                      pointer to eventEntry
*             queueIndex                L7_uint32 (index of the log Queue)
*             logQueueNumber(output)    L7_uint32
*
* @returntype L7_SUCCESS    if success
#             L7_FAILURE    if failure
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonLogQueueCheck(rmonEventEntry_t* evnt ,L7_uint32 queueIndex, L7_uint32 *logQueueNumber)
{ 
    rmonLogEntry_t* log;
    L7_uint32 logCount;
    L7_RC_t   rc = L7_FAILURE;

    for( logCount =1; logCount <= RMON_LOG_QUEUE_INDEX_MAX_NUM; logCount++)
    {
        log = &(evnt->logQueue[logCount]);
        if ( (log->validEntry != L7_TRUE) || (log->logIndex != queueIndex) )
            continue;
        else
            {
                *logQueueNumber = logCount;
                rc = L7_SUCCESS;
                break;
            }
    }
    return rc;
}

