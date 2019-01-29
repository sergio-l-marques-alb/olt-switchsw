 /*******************************************************************
 * <pre>
 * LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2005
 * LL   VV  VV LL   7   77   All Rights Reserved.
 * LL   VV  VV LL      77
 * LL    VVVV  LL     77
 * LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
 * </pre>
 **********************************************************************
 *
 * @filename k_mib_fastpath_timezone.c
 *
 * @purpose  Provide interface to sim Timezone  and summerTime configuration
 *
 * @component unitmgr
 *
 * @create 02/03/2007
 *
 * @author Himabindu
 * @end
 *
 **********************************************************************/
/********************************************************************
 *
 *******************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_timezone_api.h"
#include "usmdb_timezone_api.h"
#include "timezone_exports.h"
#include "usmdb_common.h"

agentSystemTimeGroup_t *
k_agentSystemTimeGroup_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static agentSystemTimeGroup_t agentSystemTimeData;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 zone[L7_TIMEZONE_ACRONYM_LEN+1];

  ZERO_VALID(agentSystemTimeData.valid);
  bzero(snmp_buffer, SNMP_BUFFER_LEN); 
  /*snmpSummerTimeConvert();*/

    switch(nominator)
    {
      case -1:
        break;

      case I_agentSystemTime:
          
       if((snmpSystemTimeGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS) &&
          (SafeMakeOctetStringFromTextExact(&agentSystemTimeData.agentSystemTime, snmp_buffer)==L7_TRUE) )
              SET_VALID(I_agentSystemTime, agentSystemTimeData.valid);
       break;
      case I_agentSystemDate:
        if((snmpSystemDateGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS) &&
           (SafeMakeOctetStringFromTextExact(&agentSystemTimeData.agentSystemDate, snmp_buffer) == L7_TRUE))
             SET_VALID(I_agentSystemDate, agentSystemTimeData.valid);
        break;
      case I_agentSystemTimeZoneAcronym:
        if((snmpSystemTimeZoneAcronymGet(USMDB_UNIT_CURRENT, zone) == L7_SUCCESS) &&
           (SafeMakeOctetStringFromTextExact(&agentSystemTimeData.agentSystemTimeZoneAcronym, zone) == L7_TRUE))
              SET_VALID(I_agentSystemTimeZoneAcronym, agentSystemTimeData.valid);
        break;
      case I_agentSystemTimeSource:
        if(snmpSystemTimeSourceGet(USMDB_UNIT_CURRENT, &agentSystemTimeData.agentSystemTimeSource) == L7_SUCCESS)
          SET_VALID(I_agentSystemTimeSource, agentSystemTimeData.valid);
        break;
      case I_agentSystemSummerTimeState:
         if(snmpSystemSummerTimeStateGet(USMDB_UNIT_CURRENT, &agentSystemTimeData.agentSystemSummerTimeState) == L7_SUCCESS)
          SET_VALID(I_agentSystemSummerTimeState, agentSystemTimeData.valid);
        break;

     default:
        /* unknown nominator */
        return(NULL);
        break;
    }
  
                     
   if (nominator >= 0 && !VALID(nominator, agentSystemTimeData.valid))
    return(NULL);

  return(&agentSystemTimeData);
}

agentTimeZoneGroup_t *
k_agentTimeZoneGroup_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static agentTimeZoneGroup_t agentTimeZoneData;
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  ZERO_VALID(agentTimeZoneData.valid);
  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  
    switch(nominator)
    {
      case -1:
        break;

      case I_agentTimeZoneHoursOffset:
        if(snmpTimeZoneHoursOffsetGet(USMDB_UNIT_CURRENT,&agentTimeZoneData.agentTimeZoneHoursOffset) == L7_SUCCESS)
           SET_VALID(I_agentTimeZoneHoursOffset, agentTimeZoneData.valid);
        break;
      case I_agentTimeZoneMinutesOffset:
        if(snmpTimeZoneMinutesOffsetGet(USMDB_UNIT_CURRENT,&agentTimeZoneData.agentTimeZoneMinutesOffset) == L7_SUCCESS)
          SET_VALID(I_agentTimeZoneMinutesOffset, agentTimeZoneData.valid);
        break;
      case I_agentTimeZoneAcronym:
         if((snmpTimeZoneAcronymGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS) &&
            (SafeMakeOctetStringFromTextExact(&agentTimeZoneData.agentTimeZoneAcronym, snmp_buffer) == L7_TRUE))
              SET_VALID(I_agentTimeZoneAcronym, agentTimeZoneData.valid);
        break;
 
     default:
        /* unknown nominator */
        return(NULL);
        break;
    }
  

   if (nominator >= 0 && !VALID(nominator, agentTimeZoneData.valid))
    return(NULL);

  return(&agentTimeZoneData);
}


#ifdef SETS
int
k_agentTimeZoneGroup_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentTimeZoneGroup_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentTimeZoneGroup_set(agentTimeZoneGroup_t *data,
    ContextInfo *contextInfo, int function)
{
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
   bzero(snmp_buffer, SNMP_BUFFER_LEN); 


  if (VALID(I_agentTimeZoneHoursOffset, data->valid)&&
        snmpTimeZoneHoursOffsetSet(USMDB_UNIT_CURRENT,data->agentTimeZoneHoursOffset) != L7_SUCCESS)
  {
    CLR_VALID(I_agentTimeZoneHoursOffset, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentTimeZoneMinutesOffset, data->valid) &&
      snmpTimeZoneMinutesOffsetSet(USMDB_UNIT_CURRENT,data->agentTimeZoneMinutesOffset) != L7_SUCCESS)
  {
    CLR_VALID(I_agentTimeZoneMinutesOffset, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentTimeZoneAcronym, data->valid) && data->agentTimeZoneAcronym->octet_ptr!=NULL)
  { 
     memcpy(snmp_buffer,data->agentTimeZoneAcronym->octet_ptr,data->agentTimeZoneAcronym->length);
     if(snmpTimeZoneAcronymSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
     {
      CLR_VALID(I_agentTimeZoneAcronym, data->valid);
      return COMMIT_FAILED_ERROR;
     }
  }

  return NO_ERROR;
}

int
k_agentTimeZoneGroup_set_defaults(doList_t *dp)
{
  agentTimeZoneGroup_t *data = (agentTimeZoneGroup_t *) (dp->data);

  data->agentTimeZoneHoursOffset = 0;
  data->agentTimeZoneMinutesOffset=0;
  if ((data->agentTimeZoneAcronym = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

#ifdef SR_agentTimeZoneGroup_UNDO
/* add #define SR_agentTimeZoneGroup_UNDO in sitedefs.h to
 * include the undo routine for the pim family.
 */
int
agentTimeZoneGroup_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* agentTimeZoneGroup  */

#endif /* SETS */

agentSummerTimeGroup_t *
k_agentSummerTimeGroup_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static agentSummerTimeGroup_t agentSummerTimeData;

   switch(nominator)
    {
      case -1:
        break;

      case I_agentSummerTimeMode:
           if(snmpStModeGet(USMDB_UNIT_CURRENT,&agentSummerTimeData.agentSummerTimeMode)==L7_SUCCESS)
              SET_VALID(I_agentSummerTimeMode,agentSummerTimeData.valid);
              break;
      default:
           return NULL;
   }  
 

   if (nominator >= 0 && !VALID(nominator, agentSummerTimeData.valid))
    return(NULL);

  return(&agentSummerTimeData);
}


#ifdef SETS
int
k_agentSummerTimeGroup_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSummerTimeGroup_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSummerTimeGroup_set(agentSummerTimeGroup_t *data,
    ContextInfo *contextInfo, int function)
{

 if (VALID(I_agentSummerTimeMode, data->valid) &&
       snmpStModeSet(USMDB_UNIT_CURRENT,data->agentSummerTimeMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSummerTimeMode , data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

int
k_agentSummerTimeGroup_set_defaults(doList_t *dp)
{
  agentSummerTimeGroup_t *data = (agentSummerTimeGroup_t *) (dp->data);

  data->agentSummerTimeMode = 0;
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

#ifdef SR_agentSummerTimeGroup_UNDO
/* add #define SR_agentSummerTimeGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentStRecurringGroup family.
 */
int
agentSummerTimeGroup_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}

#endif /* agentSummerTimeGroup  */

#endif /* SETS */



agentSummerTimeRecurringGroup_t *
k_agentSummerTimeRecurringGroup_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static agentSummerTimeRecurringGroup_t agentSummerTimeRecurringData;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

   bzero(snmp_buffer, SNMP_BUFFER_LEN);
   ZERO_VALID(agentSummerTimeRecurringData.valid);


    switch(nominator)
    {
      case -1:
        break;

      case I_agentStRecurringStartingWeek:
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_START_WEEK,&agentSummerTimeRecurringData.agentStRecurringStartingWeek) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringStartingWeek , agentSummerTimeRecurringData.valid);
        break;
      case I_agentStRecurringStartingDay:
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_START_DAY,&agentSummerTimeRecurringData.agentStRecurringStartingDay) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringStartingDay, agentSummerTimeRecurringData.valid);
        break;
      case I_agentStRecurringStartingMonth:
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_START_MONTH, &agentSummerTimeRecurringData.agentStRecurringStartingMonth) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringStartingMonth, agentSummerTimeRecurringData.valid);
        break;

       case I_agentStRecurringStartingTime:
        if((snmpStRecurringStartingTimeGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS)&&
          (SafeMakeOctetStringFromTextExact(&agentSummerTimeRecurringData.agentStRecurringStartingTime, snmp_buffer) == L7_TRUE))
          SET_VALID(I_agentStRecurringStartingTime, agentSummerTimeRecurringData.valid);
        break;

       case I_agentStRecurringEndingWeek:
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_END_WEEK, &agentSummerTimeRecurringData.agentStRecurringEndingWeek) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringEndingWeek, agentSummerTimeRecurringData.valid);
        break;


       case I_agentStRecurringEndingDay :
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_END_DAY, &agentSummerTimeRecurringData.agentStRecurringEndingDay) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringEndingDay, agentSummerTimeRecurringData.valid);
        break;

        case I_agentStRecurringEndingMonth:
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_END_MONTH, &agentSummerTimeRecurringData.agentStRecurringEndingMonth) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringEndingMonth, agentSummerTimeRecurringData.valid);
        break;

        case I_agentStRecurringEndingTime:
        if((snmpStRecurringEndingTimeGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS)&&
           (SafeMakeOctetStringFromTextExact(&agentSummerTimeRecurringData.agentStRecurringEndingTime,snmp_buffer) == L7_TRUE))
             SET_VALID(I_agentStRecurringEndingTime, agentSummerTimeRecurringData.valid);
        break;
   

         case I_agentStRecurringZoneAcronym:
        if((snmpStRecurringZoneAcronymGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS)&& 
          (SafeMakeOctetStringFromTextExact(&agentSummerTimeRecurringData.agentStRecurringZoneAcronym,snmp_buffer) == L7_TRUE))
             SET_VALID(I_agentStRecurringZoneAcronym, agentSummerTimeRecurringData.valid);
        break;

         case I_agentStRecurringZoneOffset :
        if(usmDbStRecurringGet(USMDB_UNIT_CURRENT,L7_OFFSET_MINUTES, &agentSummerTimeRecurringData.agentStRecurringZoneOffset) == L7_SUCCESS)
          SET_VALID(I_agentStRecurringZoneOffset, agentSummerTimeRecurringData.valid);
        break;

        default:
        /* unknown nominator */
        return(NULL);
        break;
    }
  

   if (nominator >= 0 && !VALID(nominator, agentSummerTimeRecurringData.valid))
    return(NULL);

  return(&agentSummerTimeRecurringData);
}


#ifdef SETS
int
k_agentSummerTimeRecurringGroup_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSummerTimeRecurringGroup_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSummerTimeRecurringGroup_set(agentSummerTimeRecurringGroup_t *data,
    ContextInfo *contextInfo, int function)
{
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
   bzero(snmp_buffer, SNMP_BUFFER_LEN);


  if (VALID(I_agentStRecurringStartingWeek, data->valid) &&
      usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_START_WEEK, &data->agentStRecurringStartingWeek) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStRecurringStartingWeek, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentStRecurringStartingDay, data->valid) &&
      usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_START_DAY,&data->agentStRecurringStartingDay) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStRecurringStartingDay, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentStRecurringStartingMonth, data->valid) &&
       usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_START_MONTH, &data->agentStRecurringStartingMonth) != L7_SUCCESS)
   {
    CLR_VALID(I_agentStRecurringStartingMonth, data->valid);
    return COMMIT_FAILED_ERROR;
   }
   if (VALID(I_agentStRecurringStartingTime, data->valid) && data->agentStRecurringStartingTime->octet_ptr!=NULL)
   {
     memcpy(snmp_buffer,data->agentStRecurringStartingTime->octet_ptr,data->agentStRecurringStartingTime->length);
     if(snmpStRecurringStartingTimeSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
     {
       CLR_VALID(I_agentStRecurringStartingTime, data->valid);
       return COMMIT_FAILED_ERROR;
     }

   }
   if (VALID(I_agentStRecurringEndingWeek, data->valid) &&
       usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_END_WEEK, &data->agentStRecurringEndingWeek) != L7_SUCCESS)
   {
    CLR_VALID(I_agentStRecurringEndingWeek, data->valid);
    return COMMIT_FAILED_ERROR;
   }
   if (VALID(I_agentStRecurringEndingDay, data->valid) &&
       usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_END_DAY,&data->agentStRecurringEndingDay) != L7_SUCCESS)
   {
    CLR_VALID(I_agentStRecurringEndingDay, data->valid);
    return COMMIT_FAILED_ERROR;
   }
   if (VALID(I_agentStRecurringEndingMonth, data->valid) &&
       usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_END_MONTH,&data->agentStRecurringEndingMonth) != L7_SUCCESS)
   {
    CLR_VALID(I_agentStRecurringEndingMonth, data->valid);
    return COMMIT_FAILED_ERROR;
   }
   if (VALID(I_agentStRecurringEndingTime, data->valid) && data->agentStRecurringEndingTime->octet_ptr!=NULL)
   {
      memcpy(snmp_buffer,data->agentStRecurringEndingTime->octet_ptr,data->agentStRecurringEndingTime->length);
      if( snmpStRecurringEndingTimeSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
      {
        CLR_VALID(I_agentStRecurringEndingTime, data->valid);
        return COMMIT_FAILED_ERROR;
      }
   }
   if (VALID(I_agentStRecurringZoneAcronym, data->valid) && data->agentStRecurringZoneAcronym->octet_ptr!=NULL)
   {
       memcpy(snmp_buffer,data->agentStRecurringZoneAcronym->octet_ptr,data->agentStRecurringZoneAcronym->length);
       if(snmpStRecurringZoneAcronymSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
       {
         CLR_VALID(I_agentStRecurringZoneAcronym, data->valid);
         return COMMIT_FAILED_ERROR;
       }
   }
   if (VALID(I_agentStRecurringZoneOffset, data->valid) &&
       usmDbStRecurringSet(USMDB_UNIT_CURRENT,L7_OFFSET_MINUTES,&data->agentStRecurringZoneOffset) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStRecurringZoneOffset , data->valid);
    return COMMIT_FAILED_ERROR;
  }
   return NO_ERROR;
}


int
k_agentSummerTimeRecurringGroup_set_defaults(doList_t *dp)
{
  agentSummerTimeRecurringGroup_t *data = (agentSummerTimeRecurringGroup_t *) (dp->data);

  data->agentStRecurringStartingWeek=0;
  data->agentStRecurringStartingDay=0;
  data->agentStRecurringStartingMonth=0;
  if ((data->agentStRecurringStartingTime = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  data->agentStRecurringEndingWeek=0;
  data->agentStRecurringEndingDay=0;
  data->agentStRecurringEndingMonth=0;
  if ((data->agentStRecurringEndingTime = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
   if ((data->agentStRecurringZoneAcronym = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->agentStRecurringZoneOffset=0;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

#ifdef SR_agentStRecurringGroup_UNDO
/* add #define SR_agentStRecurringGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentStRecurringGroup family.
 */
int
agentSummerTimeRecurringGroup_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}

#endif /* agentStRecurringGroup  */

#endif /* SETS */

agentSummerTimeNonRecurringGroup_t *
k_agentSummerTimeNonRecurringGroup_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static agentSummerTimeNonRecurringGroup_t agentSummerTimeNonRecurringData;
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];


    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    ZERO_VALID(agentSummerTimeNonRecurringData.valid);

    switch(nominator)
    {
      case -1:
        break;
      
      case I_agentStNonRecurringStartingDay:
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_START_DAY,&agentSummerTimeNonRecurringData.agentStNonRecurringStartingDay) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringStartingDay, agentSummerTimeNonRecurringData.valid);
        break;
      case I_agentStNonRecurringStartingMonth:
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_START_MONTH, &agentSummerTimeNonRecurringData.agentStNonRecurringStartingMonth) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringStartingMonth, agentSummerTimeNonRecurringData.valid);
        break;

       case I_agentStNonRecurringStartingYear:
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_START_YEAR, &agentSummerTimeNonRecurringData.agentStNonRecurringStartingYear) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringStartingYear, agentSummerTimeNonRecurringData.valid);
        break;

       case I_agentStNonRecurringStartingTime:
        if((snmpStNonRecurringStartingTimeGet(USMDB_UNIT_CURRENT, snmp_buffer)==L7_SUCCESS) &&
           (SafeMakeOctetStringFromTextExact(&agentSummerTimeNonRecurringData.agentStNonRecurringStartingTime,snmp_buffer) == L7_TRUE))
             SET_VALID(I_agentStNonRecurringStartingTime, agentSummerTimeNonRecurringData.valid);
        break;

       case I_agentStNonRecurringEndingDay :
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_END_DAY, &agentSummerTimeNonRecurringData.agentStNonRecurringEndingDay) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringEndingDay, agentSummerTimeNonRecurringData.valid);
        break;

        case I_agentStNonRecurringEndingMonth:
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_END_MONTH, &agentSummerTimeNonRecurringData.agentStNonRecurringEndingMonth) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringEndingMonth, agentSummerTimeNonRecurringData.valid);
        break;

         case I_agentStNonRecurringEndingYear:
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_END_YEAR, &agentSummerTimeNonRecurringData.agentStNonRecurringEndingYear) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringEndingYear, agentSummerTimeNonRecurringData.valid);
        break;

        case I_agentStNonRecurringEndingTime:
        if((snmpStNonRecurringEndingTimeGet(USMDB_UNIT_CURRENT, snmp_buffer)==L7_SUCCESS) &&
          (SafeMakeOctetStringFromTextExact(&agentSummerTimeNonRecurringData.agentStNonRecurringEndingTime,snmp_buffer) == L7_TRUE))
          SET_VALID(I_agentStNonRecurringEndingTime, agentSummerTimeNonRecurringData.valid);
        break;


         case I_agentStNonRecurringZoneAcronym:
        if((snmpStNonRecurringZoneAcronymGet(USMDB_UNIT_CURRENT,snmp_buffer)==L7_SUCCESS) &&
           (SafeMakeOctetStringFromTextExact(&agentSummerTimeNonRecurringData.agentStNonRecurringZoneAcronym,snmp_buffer) == L7_TRUE))
          SET_VALID(I_agentStNonRecurringZoneAcronym, agentSummerTimeNonRecurringData.valid);
        break;

          case I_agentStNonRecurringZoneOffset :
        if(usmDbStNonRecurringGet(USMDB_UNIT_CURRENT,L7_OFFSET_MINUTES, &agentSummerTimeNonRecurringData.agentStNonRecurringZoneOffset) == L7_SUCCESS)
          SET_VALID(I_agentStNonRecurringZoneOffset, agentSummerTimeNonRecurringData.valid);
        break;

       default:
        /* unknown nominator */
        return(NULL);
        break;
    }
  

   if (nominator >= 0 && !VALID(nominator, agentSummerTimeNonRecurringData.valid))
    return(NULL);

  return(&agentSummerTimeNonRecurringData);
}


#ifdef SETS
int
k_agentSummerTimeNonRecurringGroup_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSummerTimeNonRecurringGroup_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSummerTimeNonRecurringGroup_set(agentSummerTimeNonRecurringGroup_t *data,
    ContextInfo *contextInfo, int function)

{

  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  
  if (VALID(I_agentStNonRecurringStartingDay, data->valid) &&
      usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_START_DAY,&data->agentStNonRecurringStartingDay) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStNonRecurringStartingDay, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentStNonRecurringStartingMonth, data->valid) &&
       usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_START_MONTH,&data->agentStNonRecurringStartingMonth) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStRecurringStartingMonth, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentStNonRecurringStartingYear, data->valid) &&
       usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_START_YEAR,&data->agentStNonRecurringStartingYear) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStNonRecurringStartingYear, data->valid);
    return COMMIT_FAILED_ERROR;
  }

   if (VALID(I_agentStNonRecurringStartingTime, data->valid) && data->agentStNonRecurringStartingTime->octet_ptr!=NULL)
   {
      memcpy(snmp_buffer,data->agentStNonRecurringStartingTime->octet_ptr,data->agentStNonRecurringStartingTime->length);
      if(snmpStNonRecurringStartingTimeSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
      {
       CLR_VALID(I_agentStNonRecurringStartingTime, data->valid);
       return COMMIT_FAILED_ERROR;
      }
  }
  if (VALID(I_agentStNonRecurringEndingDay, data->valid) &&
       usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_END_DAY,&data->agentStNonRecurringEndingDay) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStNonRecurringEndingDay, data->valid);
    return COMMIT_FAILED_ERROR;
  }
   if (VALID(I_agentStNonRecurringEndingMonth, data->valid) &&
       usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_END_MONTH,&data->agentStNonRecurringEndingMonth) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStNonRecurringEndingMonth, data->valid);
    return COMMIT_FAILED_ERROR;
  }
   if (VALID(I_agentStNonRecurringEndingYear, data->valid) &&
       usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_END_YEAR,&data->agentStNonRecurringEndingYear) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStNonRecurringEndingYear, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentStNonRecurringEndingTime, data->valid) && data->agentStNonRecurringEndingTime->octet_ptr!=NULL)
  {
     memcpy(snmp_buffer,data->agentStNonRecurringEndingTime->octet_ptr,data->agentStNonRecurringEndingTime->length);
     if(snmpStNonRecurringEndingTimeSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
     {
       CLR_VALID(I_agentStNonRecurringEndingTime, data->valid);
       return COMMIT_FAILED_ERROR;
    }
  }
   if (VALID(I_agentStNonRecurringZoneAcronym, data->valid) && data->agentStNonRecurringZoneAcronym->octet_ptr!=NULL)
  {
     memcpy(snmp_buffer,data->agentStNonRecurringZoneAcronym->octet_ptr,data->agentStNonRecurringZoneAcronym->length);  
     if(snmpStNonRecurringZoneAcronymSet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS)
     {
       CLR_VALID(I_agentStNonRecurringZoneAcronym, data->valid);
       return COMMIT_FAILED_ERROR;
     }
  }
   if (VALID(I_agentStNonRecurringZoneOffset, data->valid) &&
       usmDbStNonRecurringSet(USMDB_UNIT_CURRENT,L7_OFFSET_MINUTES,&data->agentStNonRecurringZoneOffset) != L7_SUCCESS)
  {
    CLR_VALID(I_agentStNonRecurringZoneOffset , data->valid);
    return COMMIT_FAILED_ERROR;
  }
   
  return NO_ERROR;
}

int
k_agentSummerTimeNonRecurringGroup_set_defaults(doList_t *dp)
{
  agentSummerTimeNonRecurringGroup_t *data = (agentSummerTimeNonRecurringGroup_t *) (dp->data);

  data->agentStNonRecurringStartingYear=0;
  data->agentStNonRecurringStartingDay=0;
  data->agentStNonRecurringStartingMonth=0;
  if ((data->agentStNonRecurringStartingTime = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  data->agentStNonRecurringEndingYear=0;
  data->agentStNonRecurringEndingDay=0;
  data->agentStNonRecurringEndingMonth=0;
  if ((data->agentStNonRecurringEndingTime = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
   if ((data->agentStNonRecurringZoneAcronym = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->agentStNonRecurringZoneOffset=0;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

#ifdef SR_agentStNonRecurringGroup_UNDO
/* add #define SR_agentStNonRecurringGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentStNonRecurringGroup family.
 */
int
agentSummerTimeNonRecurringGroup_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}

#endif /* agentStNonRecurringGroup  */

#endif /* SETS */




