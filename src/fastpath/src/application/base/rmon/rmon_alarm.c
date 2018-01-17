/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_alarm.c
*
* @purpose  this is the core file for RMON Alarm group
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
#include <rmon_alarm.h>
#include <rmon_event.h>
#include <rmon_index.h>


/* This has been declared in snmp_rmon.c */
extern rmonAlarmEntry_t *rmonAlarmEntryArray;

/* The SNMP to array inde mapping handle.
*/
static L7_uint32 alarmMap;


/*********************************************************************
*
* @purpose    Initialize the RMON alarm group.
*
* @returntype L7_SUCCESS    Alarm group is initialized.
*             L7_FAILURE    Initialization failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonAlarmTableInitialize(void)
{
    L7_uint32 i;
    rmonAlarmEntry_t* alm;
    alarmMap = rmonMapCreate(RMON_ALARM_ENTRY_MAX_NUM);
    
    for(i = 0; i < RMON_ALARM_ENTRY_MAX_NUM; i++)
    {
        alm = &rmonAlarmEntryArray[i];
        memset (alm, 0, sizeof (rmonAlarmEntry_t));
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    Alarm Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonAlarmTableUnconfigure(void)
{
    L7_uint32 index = 0;
    L7_RC_t rc = L7_SUCCESS;

    /* iterate over all entries in the table */
    while (rmonAlarmEntryNextGet(&index) == L7_SUCCESS)
    {
      if (rmonAlarmEntryDelete(index) != L7_SUCCESS)
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
* @purpose    Deletes the AlarmEntry for  the specified index
*
* @param      Index         Index of AlarmEntry to be deleted
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
rmonAlarmEntryDelete(L7_uint32 Index)
{
    L7_RC_t rc;
    rmonAlarmEntry_t* alm;
    L7_uint32 array_index;
    
    /* Resolve array index from the SNMP index.
    */
    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    alm = &rmonAlarmEntryArray[array_index];
    
    memset (alm, 0, sizeof (rmonAlarmEntry_t));

    rc =  rmonMapRemove (alarmMap, Index);

    if (rc != L7_SUCCESS)
        return L7_FAILURE;
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose    Creates the default AlarmEntry for  the specified index
*
* @param      Index         Index of AlarmEntry
*             alm           struct of AlarmEntry
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
rmonAlarmEntryDefaultsSet(rmonAlarmEntry_t* alm, L7_uint32 Index, L7_uint32 status)   
{
    char* tempVariable = FD_RMON_ALARM_VARIABLE_NAME;
    char* tempOwner    = FD_RMON_ALARM_OWNER_NAME;
    bzero(alm->variable, RMON_ALARM_ENTRY_VARIABLE_MAX_LENGTH+1);
    bzero(alm->owner, RMON_ALARM_ENTRY_OWNER_MAX_LENGTH+1);
    if(alm == L7_NULLPTR)
       return L7_FAILURE;
    alm->index = Index;
	alm->interval = 1;
    memcpy(alm->variable, tempVariable, strlen(tempVariable));
	alm->sampleType = RMON_ALARM_ENTRY_SAMPLETYPE_ABSOLUTE;
	alm->value = 0;
	alm->prevValue = 0;
	alm->prevDeltaValue = 0;
	alm->startupAlarm = RMON_ALARM_ENTRY_STARTUPALARM_RISINGFALLING;
	alm->risingThreshold = 1;
	alm->fallingThreshold = 1;
	alm->risingEventIndex = 1;
	alm->fallingEventIndex = 2;
    memcpy(alm->owner, tempOwner, strlen(tempOwner));
	alm->status = status;
    alm->isRisingEvent = L7_FALSE;
	alm->isFallingEvent = L7_FALSE;
    alm->justStarted = L7_FALSE;
    alm->validEntry = L7_TRUE;
    alm->alarmTimerCount = 0;
    return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose    Checks index for the valid Alarm Entry
*
* @param      Index         Index of AlarmEntry to check
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
rmonAlarmEntryGet(L7_uint32 searchType, L7_uint32 Index)
{
   rmonAlarmEntry_t* alm;
   L7_uint32 array_index;
   L7_RC_t rc;

   rc = rmonMapMatch (alarmMap, Index, &array_index);

   if (rc == L7_SUCCESS)
   {
     alm = &rmonAlarmEntryArray[array_index];

     if (alm->index != Index)
       rc = L7_FAILURE;
   }

   return rc;
}
#ifdef OLD_CODE
L7_RC_t 
rmonAlarmEntryGet(L7_uint32 searchType, L7_int32 integerValue, L7_uint32 Index)
{
   rmonAlarmEntry_t* alm;
   L7_uint32 array_index;
   L7_RC_t rc;

   if(searchType == L7_MATCH_RMON_EXACTSET && integerValue == RMON_ALARM_ENTRY_STATUS_CREATEREQUEST)
   {

       return L7_SUCCESS;
   }
 
   rc = rmonMapMatch (alarmMap, Index, &array_index);
   if (rc != L7_SUCCESS) {
       return L7_FAILURE;
   }

   alm = &rmonAlarmEntryArray[array_index];

   /* integerValue is having the value of nominator if it is GET or GETNEXT operation */

   if((searchType == L7_MATCH_RMON_EXACTGET) || (searchType == L7_MATCH_RMON_GETNEXT))
   {
       if ((integerValue >= 0) && (alm->index == Index) &&
           ((alm->status == RMON_ALARM_ENTRY_STATUS_VALID) ||
            (alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)))
        return L7_SUCCESS;

       if ((integerValue < 0) && (alm->index == Index) &&
           ((alm->status == RMON_ALARM_ENTRY_STATUS_VALID) ||
            (alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION) ||
            (alm->status == RMON_ALARM_ENTRY_STATUS_INVALID)))
       {
           alm->validEntry = L7_TRUE;
           return L7_SUCCESS;
       }

   }
   /* integerValue is having the value of STATUS of the input Alarm entry used for set */
   /* if it is SET operation */

   if(searchType == L7_MATCH_RMON_EXACTSET)
   {
       
       if ((integerValue != RMON_ALARM_ENTRY_STATUS_CREATEREQUEST) && 
           (alm->index==Index) &&
           ((alm->status == RMON_ALARM_ENTRY_STATUS_VALID) ||
            (alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)))
           return L7_SUCCESS;
   }
   return L7_FAILURE;
}
#endif

/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      Index         Index of AlarmEntry to check
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
rmonAlarmEntryNextGet(L7_uint32 *Index)
{
    L7_RC_t rc;
    L7_uint32 next_snmp_index;

    rc =  rmonMapNextGet (alarmMap, *Index, &next_snmp_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    *Index = next_snmp_index;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Gets value of Interval for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             val(output)    pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmIntervalGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    alm = &rmonAlarmEntryArray[array_index];
    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->interval;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of Variable for Alarm Entry
*
* @param      Index         index of AlarmEntry
*             buf           pointer to OID
*             buf_len       length of buf
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmVariableGet(L7_uint32 Index, L7_char8 *buf)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }


    alm = &rmonAlarmEntryArray[array_index];
    if((buf != L7_NULLPTR) && (alm->validEntry != L7_FALSE))
    {
        bzero(buf, RMON_ALARM_ENTRY_VARIABLE_MAX_LENGTH+1);
        memcpy(buf, alm->variable, strlen(alm->variable));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of SampleType for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmSampleTypeGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }


    alm = &rmonAlarmEntryArray[array_index];
    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->sampleType;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of AlarmValue for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmValueGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;


    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    alm = &rmonAlarmEntryArray[array_index];
    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->value;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of StartUpAlarm for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmStartUpAlarmGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;


    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

        alm = &rmonAlarmEntryArray[array_index];

    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->startupAlarm;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of RisingThreshold for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmRisingThresholdGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;


    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

         alm = &rmonAlarmEntryArray[array_index];

    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->risingThreshold;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of FallingThreshold for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmFallingThresholdGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

     alm = &rmonAlarmEntryArray[array_index];

    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->fallingThreshold;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of RisingEventIndex for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmRisingEventIndexGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;


    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

        alm = &rmonAlarmEntryArray[array_index];

    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->risingEventIndex;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of FallingEventIndex for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmFallingEventIndexGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

         alm = &rmonAlarmEntryArray[array_index];


    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->fallingEventIndex;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of Owner for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmOwnerGet(L7_uint32 Index, L7_char8 *buf)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

     alm = &rmonAlarmEntryArray[array_index];

    if((buf != L7_NULLPTR) && (alm->validEntry != L7_FALSE))
    {
        bzero(buf, RMON_ALARM_ENTRY_OWNER_MAX_LENGTH+1);
        memcpy(buf, alm->owner, strlen(alm->owner)); 
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Gets value of Status for Alarm Entry
*
* @param      Index         index of AlarmEntry
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
rmonAlarmStatusGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

       alm = &rmonAlarmEntryArray[array_index];

    if(alm->validEntry != L7_FALSE)
    {
        *val = alm->status;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/* SET Requests for Alarm group*/


/*********************************************************************
*
* @purpose    Sets value of Interval for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmIntervalSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

       alm = &rmonAlarmEntryArray[array_index];


    if(alm->interval == val)
        return L7_SUCCESS;
   
    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
        if ((val >= RMON_UINT_MIN_NUM) && (val <= RMON_UINT_MAX_NUM)) 
        {
            alm->interval = val;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of Variable for Alarm entry
*
* @param      Index         index of the AlarmEntry
*             buf           pointer to OID
*             buf_len       length of buf
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonAlarmVariableSet(L7_uint32 Index, L7_char8 *buf)
{
    rmonAlarmEntry_t* alm;
    L7_uint32 tempValue;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

       alm = &rmonAlarmEntryArray[array_index];



    if(strcmp(alm->variable,buf) == 0)
        return L7_SUCCESS;
    if(buf != L7_NULLPTR)
    {
        if(strlen(buf) > RMON_ALARM_ENTRY_VARIABLE_MAX_LENGTH)
            return L7_FAILURE;
        if((rmonAlarmSampledValueGet(buf, &tempValue)) != L7_SUCCESS)
            return L7_FAILURE;
        if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
        {
            bzero(alm->variable, RMON_ALARM_ENTRY_VARIABLE_MAX_LENGTH+1);
            memcpy(alm->variable, buf, strlen(buf));
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of SampleType for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmSampleTypeSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

      alm = &rmonAlarmEntryArray[array_index];

    
    if(alm->sampleType == val)
        return L7_SUCCESS;

    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
        if ((val == RMON_ALARM_ENTRY_SAMPLETYPE_ABSOLUTE) || (val == RMON_ALARM_ENTRY_SAMPLETYPE_DELTA)) 
        {
            alm->sampleType = val;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of StartUpAlarm for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmStartUpAlarmSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

      alm = &rmonAlarmEntryArray[array_index];


    if(alm->startupAlarm == val)
        return L7_SUCCESS;

    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
        if ((val == RMON_ALARM_ENTRY_STARTUPALARM_RISING) || 
            (val == RMON_ALARM_ENTRY_STARTUPALARM_FALLING) ||
            (val == RMON_ALARM_ENTRY_STARTUPALARM_RISINGFALLING)) 
        {
            alm->startupAlarm = val;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of Rising Threshold for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmRisingThresholdSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

       alm = &rmonAlarmEntryArray[array_index];


    if(alm->risingThreshold == val)
        return L7_SUCCESS;
   
    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
        if ((val >= RMON_UINT_MIN_NUM) && (val <= RMON_UINT_MAX_NUM))
        {
            alm->risingThreshold = val;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of Falling Threshold for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmFallingThresholdSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;

    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

      alm = &rmonAlarmEntryArray[array_index];


    if(alm->fallingThreshold == val)
        return L7_SUCCESS;
    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
        if((val >= RMON_UINT_MIN_NUM) && (val <= RMON_UINT_MAX_NUM))
        {
            alm->fallingThreshold = val;
            return L7_SUCCESS;
        }
    }
    return L7_FAILURE;
}
   
/*********************************************************************
*
* @purpose    Sets value of Rising Event Index for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmRisingEventIndexSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;
    L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (alarmMap, Index, &array_index);
  if (rc != L7_SUCCESS) {
      return L7_FAILURE;
  }


    alm = &rmonAlarmEntryArray[array_index];

    if(alm->risingEventIndex == val)
        return L7_SUCCESS;
   
    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
            alm->risingEventIndex = val;
            return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of Falling Event Index for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmFallingEventIndexSet(L7_uint32 Index, L7_uint32 val)
{
    
    rmonAlarmEntry_t* alm;


    L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (alarmMap, Index, &array_index);
  if (rc != L7_SUCCESS) {
      return L7_FAILURE;
  }

    alm = &rmonAlarmEntryArray[array_index];

    if(alm->fallingEventIndex == val)
        return L7_SUCCESS;
   
    if(alm->status == RMON_ALARM_ENTRY_STATUS_UNDERCREATION)
    {
            alm->fallingEventIndex = val;
            return L7_SUCCESS;
    }
    return L7_FAILURE;
    
}  


/*********************************************************************
*
* @purpose    Sets value of Owner for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmOwnerSet(L7_uint32 Index, L7_char8 *buf)
{
    rmonAlarmEntry_t* alm;
    L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (alarmMap, Index, &array_index);
  if (rc != L7_SUCCESS) {
      return L7_FAILURE;
  }

    alm = &rmonAlarmEntryArray[array_index];

    if(strcmp(alm->owner,buf) == 0)
        return L7_SUCCESS;

    if(buf != L7_NULLPTR)
    {
        if(strlen(buf) > RMON_ALARM_ENTRY_OWNER_MAX_LENGTH)
            return L7_FAILURE;
        bzero(alm->owner, RMON_ALARM_ENTRY_OWNER_MAX_LENGTH+1);
        memcpy(alm->owner, buf, strlen(buf));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}


/*********************************************************************
*
* @purpose    Sets value of Status for Alarm entry
*
* @param      Index         index of the AlarmEntry
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
rmonAlarmStatusSet(L7_uint32 Index, L7_uint32 val)
{
    rmonAlarmEntry_t* alm;
    L7_RC_t rc = L7_FAILURE;

    L7_uint32 array_index;
    L7_RC_t rc1;

  if ((val == RMON_ALARM_ENTRY_STATUS_VALID) ||
      (val == RMON_ALARM_ENTRY_STATUS_UNDERCREATION) ||
      (val == RMON_ALARM_ENTRY_STATUS_INVALID)) {

    rc1 = rmonMapMatch (alarmMap, Index, &array_index);
    if (rc1 != L7_SUCCESS) {
        return L7_FAILURE;
    }

    alm = &rmonAlarmEntryArray[array_index];


    if (alm->status == val)
            return L7_SUCCESS;
  } else {
      if (val == RMON_ALARM_ENTRY_STATUS_CREATEREQUEST) {
          rc1 = rmonMapMatch (alarmMap, Index, &array_index);
          if (rc1 == L7_SUCCESS) {
              return L7_FAILURE;
          }

          rc1 = rmonMapInsert (alarmMap, Index);
          if (rc1 != L7_SUCCESS) {
              return L7_FAILURE;
          }

          rc1 = rmonMapMatch (alarmMap, Index, &array_index);
          if (rc1 != L7_SUCCESS) {
              return L7_FAILURE;
          }
          alm = &rmonAlarmEntryArray[array_index];

          rmonAlarmEntryDefaultsSet(alm, Index, RMON_ALARM_ENTRY_STATUS_UNDERCREATION);
          return L7_SUCCESS;

      } else {
          return L7_FAILURE;
      }
  }


  switch (alm->status)
     {
      case RMON_ALARM_ENTRY_STATUS_VALID:
          switch (val) {
          case RMON_ALARM_ENTRY_STATUS_UNDERCREATION:
              alm->status = val;
              alm->validEntry = L7_TRUE;
              rc = L7_SUCCESS;
            break;
          case RMON_ALARM_ENTRY_STATUS_INVALID:
              memset (alm, 0, sizeof (rmonAlarmEntry_t));
              rc1 =  rmonMapRemove (alarmMap, Index);
              rc = L7_SUCCESS;
            break;
          default:
             rc = L7_FAILURE;
          }
      break;

       case RMON_ALARM_ENTRY_STATUS_UNDERCREATION:
           switch (val) 
           {
            case RMON_ALARM_ENTRY_STATUS_VALID:
                        alm->status = val;
                        alm->validEntry = L7_TRUE;
                        alm->alarmTimerCount = 0;
                        alm->prevDeltaValue = 0;
                        alm->prevValue = 0;
                        rc = L7_SUCCESS;
                     break;
           case RMON_ALARM_ENTRY_STATUS_INVALID:
                        memset (alm, 0, sizeof (rmonAlarmEntry_t));
                        rc1 =  rmonMapRemove (alarmMap, Index);
                        rc = L7_SUCCESS;
                     break;
            default:
                        rc = L7_FAILURE;
                    }
                    break;
            }
    return rc;
}

/*********************************************************************
*
* @purpose    Starts polling for Alarm
*
* @param      alm           struct of alarmEntry
*
* @returntype void          void
*
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t
rmonAlarmStart(rmonAlarmEntry_t* alm)
{
    L7_uint32 flag;

    if(alm == L7_NULLPTR )
        return L7_FAILURE;
    
    if((alm->isFallingEvent == L7_FALSE) && (alm->isRisingEvent == L7_FALSE))
    {
        switch(alm->startupAlarm)
        {
            case RMON_ALARM_ENTRY_STARTUPALARM_RISINGFALLING:
                if(alm->value < alm->fallingThreshold)
                {
                    flag = RMON_FLAG_FALLING_EVENT;
                    if((rmonEventGenerate(alm->index, alm->fallingEventIndex, flag)) != L7_SUCCESS)
                        return L7_FAILURE;
                    alm->isFallingEvent = L7_TRUE;
                    alm->isRisingEvent = L7_FALSE;
                }

                if(alm->value > alm->risingThreshold)
                {
                    flag = RMON_FLAG_RISING_EVENT;
                    if((rmonEventGenerate(alm->index, alm->risingEventIndex, flag)) != L7_SUCCESS)
                        return L7_FAILURE;
                    alm->isRisingEvent = L7_TRUE;
                    alm->isFallingEvent = L7_FALSE;
                }
                break;

            case RMON_ALARM_ENTRY_STARTUPALARM_RISING:
                if(alm->value > alm->risingThreshold)
                {
                    flag = RMON_FLAG_RISING_EVENT;
                    if((rmonEventGenerate(alm->index, alm->risingEventIndex, flag)) != L7_SUCCESS)
                        return L7_FAILURE;
                    alm->isRisingEvent = L7_TRUE;
                    alm->isFallingEvent = L7_FALSE;
                }
                break;
            case RMON_ALARM_ENTRY_STARTUPALARM_FALLING:
                if(alm->value < alm->fallingThreshold)
                {
                    flag = RMON_FLAG_FALLING_EVENT;
                    if((rmonEventGenerate(alm->index, alm->fallingEventIndex, flag)) != L7_SUCCESS)
                        return L7_FAILURE;
                    alm->isFallingEvent = L7_TRUE;
                    alm->isRisingEvent = L7_FALSE;
                }
                break;
            default:
                break;
        }
        return L7_SUCCESS;
    }

    if((alm->value < alm->fallingThreshold) && (alm->isFallingEvent == L7_FALSE))
    {
        flag = RMON_FLAG_FALLING_EVENT;
        if((rmonEventGenerate(alm->index, alm->fallingEventIndex, flag)) != L7_SUCCESS)
            return L7_FAILURE;
        alm->isFallingEvent = L7_TRUE;
        alm->isRisingEvent = L7_FALSE;
        return L7_SUCCESS;
    }
    if((alm->value > alm->risingThreshold) && (alm->isRisingEvent == L7_FALSE))
    {
        flag = RMON_FLAG_RISING_EVENT;
        if((rmonEventGenerate(alm->index, alm->risingEventIndex, flag)) != L7_SUCCESS)
            return L7_FAILURE;
        alm->isRisingEvent = L7_TRUE;
        alm->isFallingEvent = L7_FALSE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Gets the sampled value for the alarm variable
*
* @param      buf           pointer to L7_char8 (alarmVariable)
*             value(output)   pointer to L7_uint32 (alarmValue)
*
* @returntype L7_SUCCESS  if sampling is successful
*             L7_FAILURE  if sampling is not successful
*          
* @notes      none
*
* @end
*********************************************************************/

L7_RC_t 
rmonAlarmSampledValueGet(L7_char8 *buf, L7_uint32 *value)
{
    if((SnmpGetVarUInt(buf, value)) != L7_SUCCESS)
        return L7_FAILURE;
    return L7_SUCCESS;
}

