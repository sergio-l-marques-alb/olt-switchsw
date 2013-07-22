/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmptrap.c
*
* Purpose: System Specific code for Etherlike MIB
*
* Created by: Colin Verne ?/?/2000
*
* Component: SNMP
*
*********************************************************************/ 

/*********************************************************************/
                         
#include "k_private_base.h"
#include "usmdb_1643_stats_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

dot3StatsEntry_t *
k_dot3StatsEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 dot3StatsIndex)
{
  static dot3StatsEntry_t dot3StatsEntryData;
  L7_uint32 intIfIndex;
  L7_uint32 tempEtherChipSet[1]; /* array to hold one element OID */

  ZERO_VALID(dot3StatsEntryData.valid);
  dot3StatsEntryData.dot3StatsIndex = dot3StatsIndex;
  SET_VALID(I_dot3StatsIndex, dot3StatsEntryData.valid);

  if ( ( searchType == EXACT ? ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, dot3StatsEntryData.dot3StatsIndex) != L7_SUCCESS ) :
         ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, dot3StatsEntryData.dot3StatsIndex) != L7_SUCCESS &&
           usmDbGetNextVisibleExtIfNumber(dot3StatsEntryData.dot3StatsIndex, &dot3StatsEntryData.dot3StatsIndex) != L7_SUCCESS ) ) ||
       usmDbIntIfNumFromExtIfNum(dot3StatsEntryData.dot3StatsIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(dot3StatsEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot3StatsIndex:
    break;

  case I_dot3StatsAlignmentErrors:
    if ( usmDbDot3StatsAlignmentErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                          &dot3StatsEntryData.dot3StatsAlignmentErrors) == L7_SUCCESS )
      SET_VALID(I_dot3StatsAlignmentErrors, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsFCSErrors:
    if ( usmDbDot3StatsFCSErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                    &dot3StatsEntryData.dot3StatsFCSErrors) == L7_SUCCESS )
      SET_VALID(I_dot3StatsFCSErrors, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsSingleCollisionFrames:
    if ( usmDbDot3StatsSingleCollisionFramesGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                &dot3StatsEntryData.dot3StatsSingleCollisionFrames) == L7_SUCCESS )
      SET_VALID(I_dot3StatsSingleCollisionFrames, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsMultipleCollisionFrames:
    if ( usmDbDot3StatsMultipleCollisionFramesGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                  &dot3StatsEntryData.dot3StatsMultipleCollisionFrames) == L7_SUCCESS )
      SET_VALID(I_dot3StatsMultipleCollisionFrames, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsSQETestErrors:
    if ( usmDbDot3StatsSQETestErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                        &dot3StatsEntryData.dot3StatsSQETestErrors) == L7_SUCCESS )
      SET_VALID(I_dot3StatsSQETestErrors, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsDeferredTransmissions:
    if ( usmDbDot3StatsDeferredTransmissionsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                &dot3StatsEntryData.dot3StatsDeferredTransmissions) == L7_SUCCESS )
      SET_VALID(I_dot3StatsDeferredTransmissions, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsLateCollisions:
    if ( usmDbDot3StatsLateCollisionsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                         &dot3StatsEntryData.dot3StatsLateCollisions) == L7_SUCCESS )
      SET_VALID(I_dot3StatsLateCollisions, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsExcessiveCollisions:
    if ( usmDbDot3StatsExcessiveCollisionsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                              &dot3StatsEntryData.dot3StatsExcessiveCollisions) == L7_SUCCESS )
      SET_VALID(I_dot3StatsExcessiveCollisions, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsInternalMacTransmitErrors:
    if ( usmDbDot3StatsInternalMacTransmitErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                    &dot3StatsEntryData.dot3StatsInternalMacTransmitErrors) == L7_SUCCESS )
      SET_VALID(I_dot3StatsInternalMacTransmitErrors, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsCarrierSenseErrors:
    if ( usmDbDot3StatsCarrierSenseErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                             &dot3StatsEntryData.dot3StatsCarrierSenseErrors) == L7_SUCCESS )
      SET_VALID(I_dot3StatsCarrierSenseErrors, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsFrameTooLongs:
    if ( usmDbDot3StatsFrameTooLongsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                        &dot3StatsEntryData.dot3StatsFrameTooLongs) == L7_SUCCESS )
      SET_VALID(I_dot3StatsFrameTooLongs, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsInternalMacReceiveErrors:
    if ( usmDbDot3StatsInternalMacReceiveErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                   &dot3StatsEntryData.dot3StatsInternalMacReceiveErrors) == L7_SUCCESS )
      SET_VALID(I_dot3StatsInternalMacReceiveErrors, dot3StatsEntryData.valid);
    break;

  case I_dot3StatsEtherChipSet:
    if ( usmDbDot3StatsEtherChipSetGet(USMDB_UNIT_CURRENT, intIfIndex, &tempEtherChipSet[0]) != L7_SUCCESS )
    {
      FreeOID(dot3StatsEntryData.dot3StatsEtherChipSet);
      if ( (dot3StatsEntryData.dot3StatsEtherChipSet = MakeOID(tempEtherChipSet,1)) == NULL )
        SET_VALID(I_dot3StatsEtherChipSet, dot3StatsEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, dot3StatsEntryData.valid) )
    return(NULL);

  return(&dot3StatsEntryData);
}

dot3CollEntry_t *
k_dot3CollEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 ifIndex,
                    SR_INT32 dot3CollCount)
{
  static dot3CollEntry_t dot3CollEntryData;

  ZERO_VALID(dot3CollEntryData.valid);
  dot3CollEntryData.ifIndex = ifIndex;
  SET_VALID(I_dot3CollEntryIndex_ifIndex, dot3CollEntryData.valid);
  dot3CollEntryData.dot3CollCount = dot3CollCount;
  SET_VALID(I_dot3CollCount, dot3CollEntryData.valid);

  if ( ((searchType == EXACT) ?
        usmDbDot3CollEntryGet(USMDB_UNIT_CURRENT, 
                              dot3CollEntryData.ifIndex,
                              dot3CollEntryData.dot3CollCount) :
        usmDbDot3CollEntryNext(USMDB_UNIT_CURRENT, 
                               dot3CollEntryData.ifIndex,
                               &dot3CollEntryData.dot3CollCount)) != L7_SUCCESS )
  {
    ZERO_VALID(dot3CollEntryData.valid);
    return(NULL);
  }


  switch (nominator)
  {
  case -1:
  case I_dot3CollEntryIndex_ifIndex:
  case I_dot3CollCount:
    break;

  case I_dot3CollFrequencies:
    if ( usmDbDot3CollFrequenciesGet(USMDB_UNIT_CURRENT, dot3CollEntryData.ifIndex,
                                     dot3CollEntryData.dot3CollCount,
                                     &dot3CollEntryData.dot3CollFrequencies) != L7_SUCCESS )
      CLR_VALID(I_dot3CollFrequencies, dot3CollEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, dot3CollEntryData.valid) )
    return(NULL);

  return(&dot3CollEntryData);
}

dot3ControlEntry_t *
k_dot3ControlEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 dot3StatsIndex)
{
#ifdef NOT_YET
   static dot3ControlEntry_t dot3ControlEntryData;

   /*
    * put your code to retrieve the information here
    */

   dot3ControlEntryData.dot3ControlFunctionsSupported = ;
   dot3ControlEntryData.dot3ControlInUnknownOpcodes = ;
   dot3ControlEntryData.dot3HCControlInUnknownOpcodes = ;
   dot3ControlEntryData.dot3StatsIndex = ;
   SET_ALL_VALID(dot3ControlEntryData.valid);
   return(&dot3ControlEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

dot3PauseEntry_t *
k_dot3PauseEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_INT32 dot3StatsIndex)
{
#ifdef NOT_YET
   static dot3PauseEntry_t dot3PauseEntryData;

   /*
    * put your code to retrieve the information here
    */

   dot3PauseEntryData.dot3PauseAdminMode = ;
   dot3PauseEntryData.dot3PauseOperMode = ;
   dot3PauseEntryData.dot3InPauseFrames = ;
   dot3PauseEntryData.dot3OutPauseFrames = ;
   dot3PauseEntryData.dot3HCInPauseFrames = ;
   dot3PauseEntryData.dot3HCOutPauseFrames = ;
   dot3PauseEntryData.dot3StatsIndex = ;
   SET_ALL_VALID(dot3PauseEntryData.valid);
   return(&dot3PauseEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_dot3PauseEntry_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dot3PauseEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dot3PauseEntry_set_defaults(doList_t *dp)
{
    dot3PauseEntry_t *data = (dot3PauseEntry_t *) (dp->data);

    data->dot3InPauseFrames = (SR_UINT32) 0;
    data->dot3OutPauseFrames = (SR_UINT32) 0;
    data->dot3HCInPauseFrames = (SR_UINT32) 0;
    data->dot3HCOutPauseFrames = (SR_UINT32) 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dot3PauseEntry_set(dot3PauseEntry_t *data,
                     ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_dot3PauseEntry_UNDO
/* add #define SR_dot3PauseEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot3PauseEntry family.
 */
int
dot3PauseEntry_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dot3PauseEntry_UNDO */

#endif /* SETS */

dot3HCStatsEntry_t *
k_dot3HCStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 dot3StatsIndex)
{
#ifdef NOT_YET
   static dot3HCStatsEntry_t dot3HCStatsEntryData;

   /*
    * put your code to retrieve the information here
    */

   dot3HCStatsEntryData.dot3HCStatsAlignmentErrors = ;
   dot3HCStatsEntryData.dot3HCStatsFCSErrors = ;
   dot3HCStatsEntryData.dot3HCStatsInternalMacTransmitErrors = ;
   dot3HCStatsEntryData.dot3HCStatsFrameTooLongs = ;
   dot3HCStatsEntryData.dot3HCStatsInternalMacReceiveErrors = ;
   dot3HCStatsEntryData.dot3HCStatsSymbolErrors = ;
   dot3HCStatsEntryData.dot3StatsIndex = ;
   SET_ALL_VALID(dot3HCStatsEntryData.valid);
   return(&dot3HCStatsEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

