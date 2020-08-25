
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename k_mib_rmon.c
*
* @purpose  system specific code for RFC 1757
*
*
* @component SNMP-RMON
*
*
* @create  08/06/2001
*
* @author  Kumar Manish
* @end
*
**********************************************************************/

/*********************************************************************/

#include "k_private_base.h"

#include "oid_lib.h"
#include "frmt_lib.h"
#include "snmp_rmonapi.h"
#include "base_sitedefs.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "rmon_exports.h"

etherStatsEntry_t *
   k_etherStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 etherStatsIndex)
{
  static etherStatsEntry_t etherStatsEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];
/* lvl7_@p0454 start */
  L7_uint32 snmp_uint;
  L7_uint32 intIfIndex;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    etherStatsEntryData.etherStatsDataSource = MakeOIDFromDot("0.0");
    etherStatsEntryData.etherStatsOwner = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(etherStatsEntryData.valid);
  etherStatsEntryData.etherStatsIndex = etherStatsIndex;
  SET_VALID(I_etherStatsIndex, etherStatsEntryData.valid);

  if ( (searchType == EXACT ?
        (snmpRmonEtherStatsEntryGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex) != L7_SUCCESS) :
        ( snmpRmonEtherStatsEntryGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex) != L7_SUCCESS &&
          snmpRmonEtherStatsEntryNextGet(USMDB_UNIT_CURRENT, &etherStatsEntryData.etherStatsIndex) != L7_SUCCESS ) )||
       snmpRmonIntIfNumGet(etherStatsEntryData.etherStatsIndex, &intIfIndex)  != L7_SUCCESS )
  {
    ZERO_VALID(etherStatsEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_etherStatsIndex:
    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsDataSource:
    CLR_VALID(I_etherStatsDataSource, etherStatsEntryData.valid);
    FreeOID(etherStatsEntryData.etherStatsDataSource);
    if (snmpRmonEtherStatsDataSourceGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex,
                                        &snmp_uint) == L7_SUCCESS)
    {
      sprintf(snmp_buffer, "ifIndex.%d", snmp_uint);

      if ((etherStatsEntryData.etherStatsDataSource = MakeOIDFromDot(snmp_buffer)) != NULL )
      {
        SET_VALID(I_etherStatsDataSource, etherStatsEntryData.valid);
      }
      else
      {
        etherStatsEntryData.etherStatsDataSource = MakeOIDFromDot("0.0");
      }
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsDropEvents:
    if ( snmpRmonEtherStatsDropEventsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                         &etherStatsEntryData.etherStatsDropEvents) == L7_SUCCESS )
      SET_VALID(I_etherStatsDropEvents, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsOctets:
    if ( snmpRmonEtherStatsOctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                     &etherStatsEntryData.etherStatsOctets) == L7_SUCCESS )
      SET_VALID(I_etherStatsOctets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts:
    if ( snmpRmonEtherStatsPktsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                   &etherStatsEntryData.etherStatsPkts) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsBroadcastPkts:
    if ( snmpRmonEtherStatsBroadcastPktsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                            &etherStatsEntryData.etherStatsBroadcastPkts) == L7_SUCCESS )
      SET_VALID(I_etherStatsBroadcastPkts, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsMulticastPkts:
    if ( snmpRmonEtherStatsMulticastPktsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                            &etherStatsEntryData.etherStatsMulticastPkts) == L7_SUCCESS )
      SET_VALID(I_etherStatsMulticastPkts, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsCRCAlignErrors:
    if ( snmpRmonEtherStatsCRCAlignErrorsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                             &etherStatsEntryData.etherStatsCRCAlignErrors) == L7_SUCCESS )
      SET_VALID(I_etherStatsCRCAlignErrors, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsUndersizePkts:
    if ( snmpRmonEtherStatsUndersizePktsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                            &etherStatsEntryData.etherStatsUndersizePkts) == L7_SUCCESS )
      SET_VALID(I_etherStatsUndersizePkts, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsOversizePkts:
    if ( snmpRmonEtherStatsOversizePktsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                           &etherStatsEntryData.etherStatsOversizePkts) == L7_SUCCESS )
      SET_VALID(I_etherStatsOversizePkts, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsFragments:
    if ( snmpRmonEtherStatsFragmentsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                        &etherStatsEntryData.etherStatsFragments) == L7_SUCCESS )
      SET_VALID(I_etherStatsFragments, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsJabbers:
    if ( snmpRmonEtherStatsJabbersGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                      &etherStatsEntryData.etherStatsJabbers) == L7_SUCCESS )
      SET_VALID(I_etherStatsJabbers, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsCollisions:
    if ( snmpRmonEtherStatsCollisionsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                         &etherStatsEntryData.etherStatsCollisions) == L7_SUCCESS )
      SET_VALID(I_etherStatsCollisions, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts64Octets:
    if ( snmpRmonEtherStatsPkts64OctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                           &etherStatsEntryData.etherStatsPkts64Octets) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts64Octets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts65to127Octets:
    if ( snmpRmonEtherStatsPkts65to127OctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                                &etherStatsEntryData.etherStatsPkts65to127Octets) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts65to127Octets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts128to255Octets:
    if ( snmpRmonEtherStatsPkts128to255OctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                                 &etherStatsEntryData.etherStatsPkts128to255Octets) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts128to255Octets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts256to511Octets:
    if ( snmpRmonEtherStatsPkts256to511OctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                                 &etherStatsEntryData.etherStatsPkts256to511Octets) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts256to511Octets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts512to1023Octets:
    if ( snmpRmonEtherStatsPkts512to1023OctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                                  &etherStatsEntryData.etherStatsPkts512to1023Octets) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts512to1023Octets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsPkts1024to1518Octets:
    if ( snmpRmonEtherStatsPkts1024to1518OctetsGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, intIfIndex,
                                                   &etherStatsEntryData.etherStatsPkts1024to1518Octets) == L7_SUCCESS )
      SET_VALID(I_etherStatsPkts1024to1518Octets, etherStatsEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsOwner:
    if ( snmpRmonEtherStatsOwnerGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex, snmp_buffer) == L7_SUCCESS )
    {
      if (SafeMakeOctetStringFromText(&etherStatsEntryData.etherStatsOwner, snmp_buffer) == L7_TRUE)
        SET_VALID(I_etherStatsOwner, etherStatsEntryData.valid);
    }
    else
    {
      SafeMakeOctetStringFromText(&etherStatsEntryData.etherStatsOwner, "");
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_etherStatsStatus:
    if ( snmpRmonEtherStatsStatusGet(USMDB_UNIT_CURRENT, etherStatsEntryData.etherStatsIndex,
                                     &etherStatsEntryData.etherStatsStatus) == L7_SUCCESS )
      SET_VALID(I_etherStatsStatus, etherStatsEntryData.valid);
    break;
/* lvl7_@p0454 end */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, etherStatsEntryData.valid) )
    return(NULL);

  return(&etherStatsEntryData);
}

#ifdef SETS
int
   k_etherStatsEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
   k_etherStatsEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *doHead, doList_t *dp)
{
  etherStatsEntry_t *data = (etherStatsEntry_t *) (dp->data);

  /* check the length, since the Textual Convention OwnerString is defined in the
  IF-MIB as 0..255, but it should be 0..127 in this MIB */
  if (VALID(I_etherStatsOwner, data->valid) &&
      data->etherStatsOwner->length > 127)
    return(WRONG_LENGTH_ERROR);

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
   k_etherStatsEntry_set_defaults(doList_t *dp)
{
   etherStatsEntry_t *data = (etherStatsEntry_t *) (dp->data);

   if ( (data->etherStatsDataSource = MakeOIDFromDot("0.0")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->etherStatsDropEvents = (SR_UINT32) 0;
   data->etherStatsOctets = (SR_UINT32) 0;
   data->etherStatsPkts = (SR_UINT32) 0;
   data->etherStatsBroadcastPkts = (SR_UINT32) 0;
   data->etherStatsMulticastPkts = (SR_UINT32) 0;
   data->etherStatsCRCAlignErrors = (SR_UINT32) 0;
   data->etherStatsUndersizePkts = (SR_UINT32) 0;
   data->etherStatsOversizePkts = (SR_UINT32) 0;
   data->etherStatsFragments = (SR_UINT32) 0;
   data->etherStatsJabbers = (SR_UINT32) 0;
   data->etherStatsCollisions = (SR_UINT32) 0;
   data->etherStatsPkts64Octets = (SR_UINT32) 0;
   data->etherStatsPkts65to127Octets = (SR_UINT32) 0;
   data->etherStatsPkts128to255Octets = (SR_UINT32) 0;
   data->etherStatsPkts256to511Octets = (SR_UINT32) 0;
   data->etherStatsPkts512to1023Octets = (SR_UINT32) 0;
   data->etherStatsPkts1024to1518Octets = (SR_UINT32) 0;
   if ( (data->etherStatsOwner = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }

   return(NO_ERROR);
}

int
   k_etherStatsEntry_set(etherStatsEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  char buffer[SNMP_BUFFER_LEN];
  char subBuffer[SNMP_BUFFER_LEN];
  char ownerBuffer[SNMP_BUFFER_LEN];
  L7_BOOL isCreateNewRow = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  bzero(buffer,SNMP_BUFFER_LEN);
  bzero(subBuffer,SNMP_BUFFER_LEN);
  bzero(ownerBuffer,SNMP_BUFFER_LEN);

  if ( VALID(I_etherStatsDataSource, data->valid))
  {
    if (MakeDotFromOID(data->etherStatsDataSource,buffer)!=-1)
    {
      strncpy(subBuffer,buffer,strlen("ifIndex."));

	  if ((strcmp(subBuffer,"ifIndex.")!=0) || (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,
	  data->etherStatsDataSource->oid_ptr[data->etherStatsDataSource->length-1]) != L7_SUCCESS))
	  {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
    }
  }

  if ( VALID(I_etherStatsOwner, data->valid))
  {
    bzero(ownerBuffer, SNMP_BUFFER_LEN);
    if (SPrintAscii(data->etherStatsOwner, ownerBuffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }
  }

  if (snmpRmonEtherStatsEntryGet(USMDB_UNIT_CURRENT,data->etherStatsIndex) != L7_SUCCESS)
  {
    /* check for row creation */
    if (VALID(I_etherStatsStatus, data->valid) &&
   	    data->etherStatsStatus == D_etherStatsStatus_createRequest &&
	    snmpRmonEtherStatsStatusSet(USMDB_UNIT_CURRENT, data->etherStatsIndex,
								  data->etherStatsStatus) == L7_SUCCESS)
    {
	  isCreateNewRow = L7_TRUE;
	  SET_VALID(I_etherStatsStatus, tempValid);
    }
    else
    {
	  memcpy(data->valid, tempValid, sizeof(data->valid));
	  return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_etherStatsDataSource, data->valid))
  {
    if (snmpRmonEtherStatsDataSourceSet(USMDB_UNIT_CURRENT, data->etherStatsIndex,
      data->etherStatsDataSource->oid_ptr[data->etherStatsDataSource->length-1])!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
  	  SET_VALID(I_etherStatsDataSource, tempValid);
    }
  }

  if ( VALID(I_etherStatsOwner, data->valid))
  {
    if (snmpRmonEtherStatsOwnerSet(USMDB_UNIT_CURRENT,
    data->etherStatsIndex,ownerBuffer)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_etherStatsOwner, tempValid);
    }
  }

  if ( VALID(I_etherStatsStatus, data->valid) && (isCreateNewRow == L7_FALSE) &&
    	 (snmpRmonEtherStatsStatusSet(USMDB_UNIT_CURRENT, data->etherStatsIndex,
								 data->etherStatsStatus) != L7_SUCCESS ) )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_etherStatsEntry_UNDO
/* add #define SR_etherStatsEntry_UNDO in sitedefs.h to
 * include the undo routine for the etherStatsEntry family.
 */
int
   etherStatsEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
  etherStatsEntry_t *data = (etherStatsEntry_t *) doCur->data;
  etherStatsEntry_t *undodata = (etherStatsEntry_t *) doCur->undodata;
  etherStatsEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;
  L7_BOOL isUndoStatValid = L7_FALSE;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->etherStatsStatus = D_etherStatsStatus_invalid;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a deletion, create with original data */
    if((undodata->etherStatsStatus == D_etherStatsStatus_underCreation)
       && (data->etherStatsStatus == D_etherStatsStatus_invalid))
    {
        undodata->etherStatsStatus = D_etherStatsStatus_createRequest;
    }
    else
    {
       if((undodata->etherStatsStatus == D_etherStatsStatus_valid)
          && (data->etherStatsStatus == D_etherStatsStatus_invalid))
       {
         undodata->etherStatsStatus = D_etherStatsStatus_createRequest;
         /* if status changed from valid to invalid, then undo is done
          * with status as valid.
          */
         isUndoStatValid = L7_TRUE;
       }
    }
    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for undo */
  if( k_etherStatsEntry_set(setdata, contextInfo, function) != NO_ERROR )
    return UNDO_FAILED_ERROR;

  /* If undoing a delete then only update the created row with undo data */
  if ( (undodata != NULL) && (data->etherStatsStatus == D_etherStatsStatus_invalid) )
  {
    memset(setdata->valid, 0xff, sizeof(data->valid));
    if( isUndoStatValid == L7_TRUE )
    {
      setdata->etherStatsStatus = D_etherStatsStatus_valid;
    }
    else /* We are here with etherStatsStatus as UnderCreation and we do't want to change it */
    {
      CLR_VALID(I_etherStatsStatus, setdata->valid);
    }
    if( k_etherStatsEntry_set(setdata, contextInfo, function) != NO_ERROR )
      return UNDO_FAILED_ERROR;
  }

  return(NO_ERROR);
}
#endif /* SR_etherStatsEntry_UNDO */

#endif /* SETS */

historyControlEntry_t *
   k_historyControlEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 historyControlIndex)
{
  static historyControlEntry_t historyControlEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_uint;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    historyControlEntryData.historyControlDataSource = MakeOIDFromDot("0.0");
    historyControlEntryData.historyControlOwner = MakeOctetString(NULL, 0);
  }

   /* lvl7_@p0454 start */
  ZERO_VALID(historyControlEntryData.valid);
  historyControlEntryData.historyControlIndex = historyControlIndex;
  SET_VALID(I_historyControlIndex, historyControlEntryData.valid);

  if ( (searchType == EXACT ?
        (snmpRmonHistoryControlEntryGet(USMDB_UNIT_CURRENT, historyControlEntryData.historyControlIndex) != L7_SUCCESS) :
        ( snmpRmonHistoryControlEntryGet(USMDB_UNIT_CURRENT, historyControlEntryData.historyControlIndex) != L7_SUCCESS &&
          snmpRmonHistoryControlEntryNextGet(USMDB_UNIT_CURRENT, &historyControlEntryData.historyControlIndex) != L7_SUCCESS )))
  {
    ZERO_VALID(historyControlEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_historyControlIndex:

    if (nominator != -1) break;
    /* else pass through */

  case I_historyControlDataSource:
    FreeOID(historyControlEntryData.historyControlDataSource);
    if (snmpRmonHistoryControlDataSourceGet(USMDB_UNIT_CURRENT,
                                            historyControlEntryData.historyControlIndex,
                                            &snmp_uint) == L7_SUCCESS)
    {
      sprintf(snmp_buffer, "ifIndex.%d", snmp_uint);

      if ((historyControlEntryData.historyControlDataSource = MakeOIDFromDot(snmp_buffer)) != NULL )
      {
        SET_VALID(I_historyControlDataSource, historyControlEntryData.valid);
      }
      else
      {
        historyControlEntryData.historyControlDataSource = MakeOIDFromDot("0.0");
      }
    }
    else
    {
      historyControlEntryData.historyControlDataSource = MakeOIDFromDot("0.0");
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_historyControlBucketsRequested:
    if ( snmpRmonHistoryControlBucketsRequestedGet(USMDB_UNIT_CURRENT,
                                                   historyControlEntryData.historyControlIndex,
                                                   &historyControlEntryData.historyControlBucketsRequested) == L7_SUCCESS)
      SET_VALID(I_historyControlBucketsRequested, historyControlEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_historyControlBucketsGranted:
    if ( snmpRmonHistoryControlBucketsGrantedGet(USMDB_UNIT_CURRENT, historyControlEntryData.historyControlIndex,
                                                 &historyControlEntryData.historyControlBucketsGranted) == L7_SUCCESS )
      SET_VALID(I_historyControlBucketsGranted, historyControlEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_historyControlInterval:
    if ( snmpRmonHistoryControlIntervalGet(USMDB_UNIT_CURRENT, historyControlEntryData.historyControlIndex,
                                           &historyControlEntryData.historyControlInterval) == L7_SUCCESS )
      SET_VALID(I_historyControlInterval, historyControlEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_historyControlOwner:
    if ( snmpRmonHistoryControlOwnerGet(USMDB_UNIT_CURRENT, historyControlEntryData.historyControlIndex, snmp_buffer) == L7_SUCCESS )
    {
      if (SafeMakeOctetStringFromText(&historyControlEntryData.historyControlOwner, snmp_buffer) == L7_TRUE)
        SET_VALID(I_historyControlOwner, historyControlEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_historyControlStatus:
    if ( snmpRmonHistoryControlStatusGet(USMDB_UNIT_CURRENT, historyControlEntryData.historyControlIndex,
                                         &historyControlEntryData.historyControlStatus) == L7_SUCCESS )
      SET_VALID(I_historyControlStatus, historyControlEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, historyControlEntryData.valid) )
    return(NULL);

  return(&historyControlEntryData);
}

#ifdef SETS
int
   k_historyControlEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
   k_historyControlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{
  historyControlEntry_t *data = (historyControlEntry_t *) (dp->data);

  /* check the length, since the Textual Convention OwnerString is defined in the
  IF-MIB as 0..255, but it should be 0..127 in this MIB */
  if (VALID(I_historyControlOwner, data->valid) &&
      data->historyControlOwner->length > 127)
    return(WRONG_LENGTH_ERROR);

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
   k_historyControlEntry_set_defaults(doList_t *dp)
{
   historyControlEntry_t *data = (historyControlEntry_t *) (dp->data);

   if ( (data->historyControlDataSource = MakeOIDFromDot("0.0")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->historyControlBucketsRequested = 50;
   data->historyControlInterval = 1800;
   if ( (data->historyControlOwner = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }

   return(NO_ERROR);
}

int
   k_historyControlEntry_set(historyControlEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  char buffer[SNMP_BUFFER_LEN];
  char subBuffer[SNMP_BUFFER_LEN];
  char ownerBuffer[SNMP_BUFFER_LEN];
  L7_BOOL isCreateNewRow = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  bzero(buffer,SNMP_BUFFER_LEN);
  bzero(subBuffer,SNMP_BUFFER_LEN);
  bzero(ownerBuffer,SNMP_BUFFER_LEN);


  if ( snmpRmonHistoryControlEntryGet(USMDB_UNIT_CURRENT,data->historyControlIndex) != L7_SUCCESS )
  {
    /* check for row creation */
    if ( VALID(I_historyControlStatus, data->valid) &&
         data->historyControlStatus == D_historyControlStatus_createRequest &&
         snmpRmonHistoryControlStatusSet(USMDB_UNIT_CURRENT, data->historyControlIndex,
                                         data->historyControlStatus) == L7_SUCCESS )
    {
      SET_VALID(I_historyControlStatus, tempValid);
      isCreateNewRow = L7_TRUE;
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_historyControlDataSource, data->valid))
  {
    if (MakeDotFromOID(data->historyControlDataSource, buffer) != -1)
    {
      strncpy(subBuffer,buffer,strlen("ifIndex."));

      if (strcmp(subBuffer,"ifIndex.")!=0)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else if (snmpRmonHistoryControlDataSourceSet(USMDB_UNIT_CURRENT, data->historyControlIndex,
                                                   data->historyControlDataSource->oid_ptr[data->historyControlDataSource->length-1])!= L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_historyControlDataSource, tempValid);
      }
    }
  }

  if ( VALID(I_historyControlBucketsRequested, data->valid))
  {
    if (snmpRmonHistoryControlBucketsRequestedSet(USMDB_UNIT_CURRENT, data->historyControlIndex,
                                                  data->historyControlBucketsRequested) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_historyControlBucketsRequested, tempValid);
    }
  }

  if ( VALID(I_historyControlInterval, data->valid))
  {
    if (snmpRmonHistoryControlIntervalSet(USMDB_UNIT_CURRENT, data->historyControlIndex,
                                          data->historyControlInterval) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_historyControlInterval, tempValid);
    }
  }

  if ( VALID(I_historyControlOwner, data->valid))
  {
    bzero(ownerBuffer, SNMP_BUFFER_LEN);
    if (SPrintAscii(data->historyControlOwner, ownerBuffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }

    if ( snmpRmonHistoryControlOwnerSet(USMDB_UNIT_CURRENT, data->historyControlIndex,
                                        ownerBuffer)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_historyControlOwner, tempValid);
    }
  }


  if ( VALID(I_historyControlStatus, data->valid) && (isCreateNewRow == L7_FALSE) &&
       (snmpRmonHistoryControlStatusSet(USMDB_UNIT_CURRENT, data->historyControlIndex,
                                       data->historyControlStatus) != L7_SUCCESS ) )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_historyControlEntry_UNDO
/* add #define SR_historyControlEntry_UNDO in sitedefs.h to
 * include the undo routine for the historyControlEntry family.
 */
int
   historyControlEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  historyControlEntry_t *data = (historyControlEntry_t *) doCur->data;
  historyControlEntry_t *undodata = (historyControlEntry_t *) doCur->undodata;
  historyControlEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;
  L7_BOOL isUndoStatValid = L7_FALSE;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->historyControlStatus = D_historyControlStatus_invalid;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a deletion, create with original data */
    if((undodata->historyControlStatus == D_historyControlStatus_underCreation)
       && (data->historyControlStatus == D_historyControlStatus_invalid))
    {
        undodata->historyControlStatus = D_historyControlStatus_createRequest;
    }
    else
    {
       if((undodata->historyControlStatus == D_historyControlStatus_valid)
          && (data->historyControlStatus == D_historyControlStatus_invalid))
       {
         undodata->historyControlStatus = D_historyControlStatus_createRequest;
         /* if status changed from valid to invalid, then undo is done
          * with status as valid.
          */
         isUndoStatValid = L7_TRUE;
       }
    }
    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for undo */
  if( k_historyControlEntry_set(setdata, contextInfo, function) != NO_ERROR )
    return UNDO_FAILED_ERROR;

  /* If undoing a delete then only update the created row with undo data */
  if ( (undodata != NULL) && (data->historyControlStatus == D_historyControlStatus_invalid) )
  {
    memset(setdata->valid, 0xff, sizeof(data->valid));
    if( isUndoStatValid == L7_TRUE )
    {
      setdata->historyControlStatus = D_historyControlStatus_valid;
    }
    else /* We are here with historyControlStatus as UnderCreation and we do't want to change it */
    {
      CLR_VALID(I_historyControlStatus, setdata->valid);
    }
    if( k_historyControlEntry_set(setdata, contextInfo, function) != NO_ERROR )
      return UNDO_FAILED_ERROR;
  }

  return(NO_ERROR);
}
#endif /* SR_historyControlEntry_UNDO */

#endif /* SETS */

etherHistoryEntry_t *
   k_etherHistoryEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 etherHistoryIndex,
                           SR_INT32 etherHistorySampleIndex)
{
  static etherHistoryEntry_t etherHistoryEntryData;

  ZERO_VALID(etherHistoryEntryData.valid);
  etherHistoryEntryData.etherHistoryIndex = etherHistoryIndex;
  SET_VALID(I_etherHistoryIndex, etherHistoryEntryData.valid);
  etherHistoryEntryData.etherHistorySampleIndex = etherHistorySampleIndex;
  SET_VALID(I_etherHistorySampleIndex, etherHistoryEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpRmonEtherHistoryEntryGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex, etherHistorySampleIndex) != L7_SUCCESS) :
       ( (snmpRmonEtherHistoryEntryGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex, etherHistorySampleIndex) != L7_SUCCESS) &&
         (snmpRmonEtherHistoryEntryNextGet(USMDB_UNIT_CURRENT, &etherHistoryEntryData.etherHistoryIndex, &etherHistoryEntryData.etherHistorySampleIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(etherHistoryEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_etherHistoryIndex:
  case I_etherHistorySampleIndex:
    break;

  case I_etherHistoryIntervalStart:
    if ( snmpRmonEtherHistoryIntervalStartGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                              etherHistoryEntryData.etherHistorySampleIndex,
                                              &etherHistoryEntryData.etherHistoryIntervalStart) == L7_SUCCESS )
      SET_VALID(I_etherHistoryIntervalStart, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryDropEvents:
    if ( snmpRmonEtherHistoryDropEventsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                           etherHistoryEntryData.etherHistorySampleIndex,
                                           &etherHistoryEntryData.etherHistoryDropEvents) == L7_SUCCESS )
      SET_VALID(I_etherHistoryDropEvents, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryOctets:
    if ( snmpRmonEtherHistoryOctetsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                       etherHistoryEntryData.etherHistorySampleIndex,
                                       &etherHistoryEntryData.etherHistoryOctets) == L7_SUCCESS )
      SET_VALID(I_etherHistoryOctets, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryPkts:
    if ( snmpRmonEtherHistoryPktsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                     etherHistoryEntryData.etherHistorySampleIndex,
                                     &etherHistoryEntryData.etherHistoryPkts) == L7_SUCCESS )
      SET_VALID(I_etherHistoryPkts, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryBroadcastPkts:
    if ( snmpRmonEtherHistoryBroadcastPktsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                              etherHistoryEntryData.etherHistorySampleIndex,
                                              &etherHistoryEntryData.etherHistoryBroadcastPkts) == L7_SUCCESS )
      SET_VALID(I_etherHistoryBroadcastPkts, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryMulticastPkts:
    if ( snmpRmonEtherHistoryMulticastPktsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                              etherHistoryEntryData.etherHistorySampleIndex,
                                              &etherHistoryEntryData.etherHistoryMulticastPkts) == L7_SUCCESS )
      SET_VALID(I_etherHistoryMulticastPkts, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryCRCAlignErrors:
    if ( snmpRmonEtherHistoryCRCAlignErrorsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                               etherHistoryEntryData.etherHistorySampleIndex,
                                               &etherHistoryEntryData.etherHistoryCRCAlignErrors) == L7_SUCCESS )
      SET_VALID(I_etherHistoryCRCAlignErrors, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryUndersizePkts:
    if ( snmpRmonEtherHistoryUndersizePktsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                              etherHistoryEntryData.etherHistorySampleIndex,
                                              &etherHistoryEntryData.etherHistoryUndersizePkts) == L7_SUCCESS )
      SET_VALID(I_etherHistoryUndersizePkts, etherHistoryEntryData.valid);
     break;

  case I_etherHistoryOversizePkts:
    if ( snmpRmonEtherHistoryOversizePktsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                             etherHistoryEntryData.etherHistorySampleIndex,
                                             &etherHistoryEntryData.etherHistoryOversizePkts) == L7_SUCCESS )
      SET_VALID(I_etherHistoryOversizePkts, etherHistoryEntryData.valid);
     break;

  case I_etherHistoryFragments:
    if ( snmpRmonEtherHistoryFragmentsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                          etherHistoryEntryData.etherHistorySampleIndex,
                                          &etherHistoryEntryData.etherHistoryFragments) == L7_SUCCESS )
      SET_VALID(I_etherHistoryFragments, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryJabbers:
    if ( snmpRmonEtherHistoryJabbersGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                        etherHistoryEntryData.etherHistorySampleIndex,
                                        &etherHistoryEntryData.etherHistoryJabbers) == L7_SUCCESS )
      SET_VALID(I_etherHistoryJabbers, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryCollisions:
    if ( snmpRmonEtherHistoryCollisionsGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                           etherHistoryEntryData.etherHistorySampleIndex,
                                           &etherHistoryEntryData.etherHistoryCollisions) == L7_SUCCESS )
      SET_VALID(I_etherHistoryCollisions, etherHistoryEntryData.valid);
    break;

  case I_etherHistoryUtilization:
    if ( snmpRmonEtherHistoryUtilizationGet(USMDB_UNIT_CURRENT, etherHistoryEntryData.etherHistoryIndex,
                                            etherHistoryEntryData.etherHistorySampleIndex,
                                            &etherHistoryEntryData.etherHistoryUtilization) == L7_SUCCESS )
      SET_VALID(I_etherHistoryUtilization, etherHistoryEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, etherHistoryEntryData.valid) )
    return(NULL);

  return(&etherHistoryEntryData);
}

alarmEntry_t *
   k_alarmEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 alarmIndex)
{
  static alarmEntry_t alarmEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];
  char owner_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    alarmEntryData.alarmOwner = MakeOctetString(NULL, 0);
    alarmEntryData.alarmVariable = MakeOIDFromDot("0.0");
  }

  ZERO_VALID(alarmEntryData.valid);
  alarmEntryData.alarmIndex = alarmIndex;
  SET_VALID(I_alarmIndex, alarmEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpRmonAlarmEntryGet(L7_MATCH_RMON_EXACTGET, USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex) != L7_SUCCESS) :
       ( (snmpRmonAlarmEntryGet(L7_MATCH_RMON_GETNEXT, USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex) != L7_SUCCESS) &&
         (snmpRmonAlarmEntryNextGet(USMDB_UNIT_CURRENT, &alarmEntryData.alarmIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(alarmEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_alarmIndex:

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmInterval:
    if ( snmpRmonAlarmIntervalGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                  &alarmEntryData.alarmInterval) == L7_SUCCESS )
      SET_VALID(I_alarmInterval, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmVariable:
    FreeOID(alarmEntryData.alarmVariable);
    if ( snmpRmonAlarmVariableGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                  snmp_buffer) == L7_SUCCESS )
    {
      if (strlen(snmp_buffer) > 0)
      {
        if ( (alarmEntryData.alarmVariable = MakeOIDFromDot(snmp_buffer)) != L7_NULL )
          SET_VALID(I_alarmVariable, alarmEntryData.valid);
      }
      else
      {
        alarmEntryData.alarmVariable = MakeOIDFromDot("0.0");
        SET_VALID(I_alarmVariable, alarmEntryData.valid);
      }
    }
    else
    {
      alarmEntryData.alarmVariable = MakeOIDFromDot("0.0");
      SET_VALID(I_alarmVariable, alarmEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmSampleType:
    if ( snmpRmonAlarmSampleTypeGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                    &alarmEntryData.alarmSampleType) == L7_SUCCESS )
      SET_VALID(I_alarmSampleType, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmValue:
    if ( snmpRmonAlarmValueGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                               &alarmEntryData.alarmValue) == L7_SUCCESS )
      SET_VALID(I_alarmValue, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmStartupAlarm:
    if ( snmpRmonAlarmStartupAlarmGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                      &alarmEntryData.alarmStartupAlarm) == L7_SUCCESS )
      SET_VALID(I_alarmStartupAlarm, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmRisingThreshold:
    if ( snmpRmonAlarmRisingThresholdGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                         &alarmEntryData.alarmRisingThreshold) == L7_SUCCESS )
      SET_VALID(I_alarmRisingThreshold, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmFallingThreshold:
    if ( snmpRmonAlarmFallingThresholdGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                          &alarmEntryData.alarmFallingThreshold) == L7_SUCCESS )
      SET_VALID(I_alarmFallingThreshold, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmRisingEventIndex:
    if ( snmpRmonAlarmRisingEventIndexGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                          &alarmEntryData.alarmRisingEventIndex) == L7_SUCCESS )
      SET_VALID(I_alarmRisingEventIndex, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmFallingEventIndex:
    if ( snmpRmonAlarmFallingEventIndexGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                           &alarmEntryData.alarmFallingEventIndex) == L7_SUCCESS )
      SET_VALID(I_alarmFallingEventIndex, alarmEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmOwner:
    if ( snmpRmonAlarmOwnerGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex, owner_buffer) == L7_SUCCESS )
    {
      if (SafeMakeOctetStringFromText(&alarmEntryData.alarmOwner, owner_buffer) == L7_TRUE)
        SET_VALID(I_alarmOwner, alarmEntryData.valid);
    }
    else
    {
      if (SafeMakeOctetStringFromText(&alarmEntryData.alarmOwner, "") == L7_TRUE)
        SET_VALID(I_alarmOwner, alarmEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_alarmStatus:
    if ( snmpRmonAlarmStatusGet(USMDB_UNIT_CURRENT, alarmEntryData.alarmIndex,
                                &alarmEntryData.alarmStatus) == L7_SUCCESS )
      SET_VALID(I_alarmStatus, alarmEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, alarmEntryData.valid) )
    return(NULL);

  return(&alarmEntryData);
}

#ifdef SETS
int
   k_alarmEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
   k_alarmEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *dp)
{
  alarmEntry_t *data = (alarmEntry_t *) (dp->data);

  /* check the length, since the Textual Convention OwnerString is defined in the
  IF-MIB as 0..255, but it should be 0..127 in this MIB */
  if (VALID(I_alarmOwner, data->valid) &&
      data->alarmOwner->length > 127)
    return(WRONG_LENGTH_ERROR);

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
   k_alarmEntry_set_defaults(doList_t *dp)
{
   alarmEntry_t *data = (alarmEntry_t *) (dp->data);

   if ( (data->alarmVariable = MakeOIDFromDot("0.0")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   if ( (data->alarmOwner = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }

   data->alarmInterval = (SR_UINT32) 0;
   data->alarmSampleType = (SR_UINT32) 0;
   data->alarmValue = (SR_UINT32) 0;
   data->alarmStartupAlarm = (SR_UINT32) 0;
   data->alarmRisingThreshold = (SR_UINT32) 0;
   data->alarmFallingThreshold = (SR_UINT32) 0;
   data->alarmFallingEventIndex = (SR_UINT32) 0;
   data->alarmRisingEventIndex = (SR_UINT32) 0;

   return(NO_ERROR);
}

int
   k_alarmEntry_set(alarmEntry_t *data,
                    ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];
  char owner_buffer[SNMP_BUFFER_LEN];
  L7_BOOL isCreateNewRow = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  if ( snmpRmonAlarmEntryGet(L7_MATCH_RMON_EXACTSET, USMDB_UNIT_CURRENT, data->alarmIndex) != L7_SUCCESS )
  {
    /* check for row creation */
    if ( VALID(I_alarmStatus, data->valid) &&
         data->alarmStatus == D_alarmStatus_createRequest &&
         snmpRmonAlarmStatusSet(USMDB_UNIT_CURRENT, data->alarmIndex, data->alarmStatus) == L7_SUCCESS )
    {
      isCreateNewRow = L7_TRUE;
      SET_VALID(I_alarmStatus, tempValid);
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_alarmInterval, data->valid) )
  {
    if ( snmpRmonAlarmIntervalSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                data->alarmInterval) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmInterval, tempValid);
    }
  }

  if ( VALID(I_alarmVariable, data->valid))
  {
    if (MakeDotFromOID(data->alarmVariable, snmp_buffer) == -1)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }

    if (snmpRmonAlarmVariableSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                 snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmVariable, tempValid);
    }
  }

  if ( VALID(I_alarmSampleType, data->valid) )
  {
    if ( snmpRmonAlarmSampleTypeSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                  data->alarmSampleType) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmSampleType, tempValid);
    }
  }

  if ( VALID(I_alarmStartupAlarm, data->valid) )
  {
    if ( snmpRmonAlarmStartupAlarmSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                    data->alarmStartupAlarm) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmStartupAlarm, tempValid);
    }
  }

  if ( VALID(I_alarmRisingThreshold, data->valid) )
  {
    if ( snmpRmonAlarmRisingThresholdSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                       data->alarmRisingThreshold) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmRisingThreshold, tempValid);
    }
  }

  if ( VALID(I_alarmFallingThreshold, data->valid) )
  {
    if ( snmpRmonAlarmFallingThresholdSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                        data->alarmFallingThreshold) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmFallingThreshold, tempValid);
    }
  }

  if ( VALID(I_alarmRisingEventIndex, data->valid) )
  {
    if ( snmpRmonAlarmRisingEventIndexSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                        data->alarmRisingEventIndex) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmRisingEventIndex, tempValid);
    }
  }

  if ( VALID(I_alarmFallingEventIndex, data->valid) )
  {
    if ( snmpRmonAlarmFallingEventIndexSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                                               data->alarmFallingEventIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmFallingEventIndex, tempValid);
    }
  }

  if ( VALID(I_alarmOwner, data->valid))
  {
    bzero(owner_buffer, SNMP_BUFFER_LEN);
    if (SPrintAscii(data->alarmOwner, owner_buffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }

    if ( snmpRmonAlarmOwnerSet(USMDB_UNIT_CURRENT, data->alarmIndex, owner_buffer)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_alarmOwner, tempValid);
    }
  }

  if ( VALID(I_alarmStatus, data->valid) && (isCreateNewRow == L7_FALSE) &&
       snmpRmonAlarmStatusSet(USMDB_UNIT_CURRENT, data->alarmIndex,
                              data->alarmStatus) != L7_SUCCESS )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_alarmEntry_UNDO
/* add #define SR_alarmEntry_UNDO in sitedefs.h to
 * include the undo routine for the alarmEntry family.
 */
int
   alarmEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
  alarmEntry_t *data = (alarmEntry_t *) doCur->data;
  alarmEntry_t *undodata = (alarmEntry_t *) doCur->undodata;
  alarmEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;
  L7_BOOL isUndoStatValid = L7_FALSE;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->alarmStatus = D_alarmStatus_invalid;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a deletion, create with original data */
    if((undodata->alarmStatus == D_alarmStatus_underCreation)
       && (data->alarmStatus == D_alarmStatus_invalid))
    {
        undodata->alarmStatus = D_alarmStatus_createRequest;
    }
    else
    {
       if((undodata->alarmStatus == D_alarmStatus_valid)
          && (data->alarmStatus == D_alarmStatus_invalid))
       {
         undodata->alarmStatus = D_alarmStatus_createRequest;
         /* if status changed from valid to invalid, then undo is done
          * with status as valid.
          */
         isUndoStatValid = L7_TRUE;
       }
    }
    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for undo */
  if( k_alarmEntry_set(setdata, contextInfo, function) != NO_ERROR )
    return UNDO_FAILED_ERROR;

  /* If undoing a delete then only update the created row with undo data */
  if ( (undodata != NULL) && (data->alarmStatus == D_alarmStatus_invalid) )
  {
    memset(setdata->valid, 0xff, sizeof(data->valid));
    if( isUndoStatValid == L7_TRUE )
    {
      setdata->alarmStatus = D_alarmStatus_valid;
    }
    else /* We are here with alarmStatus as UnderCreation and we do't want to change it */
    {
      CLR_VALID(I_alarmStatus, setdata->valid);
    }
    if( k_alarmEntry_set(setdata, contextInfo, function) != NO_ERROR )
      return UNDO_FAILED_ERROR;
  }

  return(NO_ERROR);
}
#endif /* SR_alarmEntry_UNDO */

#endif /* SETS */


eventEntry_t *
   k_eventEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 eventIndex)
{
  static eventEntry_t eventEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];
  char owner_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    eventEntryData.eventCommunity = MakeOctetString(NULL, 0);
    eventEntryData.eventDescription = MakeOctetString(NULL, 0);
    eventEntryData.eventOwner = MakeOctetString(NULL, 0);
  }

  eventEntryData.eventIndex = eventIndex;
  SET_VALID(I_eventIndex, eventEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpRmonEventEntryGet(L7_MATCH_RMON_EXACTGET, USMDB_UNIT_CURRENT, eventEntryData.eventIndex) != L7_SUCCESS) :
       ( (snmpRmonEventEntryGet(L7_MATCH_RMON_GETNEXT, USMDB_UNIT_CURRENT, eventEntryData.eventIndex) != L7_SUCCESS) &&
         (snmpRmonEventEntryNextGet(USMDB_UNIT_CURRENT, &eventEntryData.eventIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(eventEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
/* lvl7_@p2111 start */
  case I_eventIndex:
    if (nominator != -1) break;
    /* else pass through */
/* lvl7_@p2111 end */

  case I_eventDescription:
    if ( snmpRmonEventDescriptionGet(USMDB_UNIT_CURRENT, eventEntryData.eventIndex, snmp_buffer) == L7_SUCCESS )
    {
      if (SafeMakeOctetStringFromText(&eventEntryData.eventDescription, snmp_buffer) == L7_TRUE)
        SET_VALID(I_eventDescription, eventEntryData.valid);
    }
    else
    {
      if (SafeMakeOctetStringFromText(&eventEntryData.eventDescription, "") == L7_TRUE)
       SET_VALID(I_eventDescription, eventEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_eventType:
    if ( snmpRmonEventTypeGet(USMDB_UNIT_CURRENT, eventEntryData.eventIndex,
                              &eventEntryData.eventType) == L7_SUCCESS )
      SET_VALID(I_eventType, eventEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_eventCommunity:
    if ( snmpRmonEventCommunityGet(USMDB_UNIT_CURRENT, eventEntryData.eventIndex, snmp_buffer) == L7_SUCCESS &&
         SafeMakeOctetStringFromText(&eventEntryData.eventCommunity, snmp_buffer) == L7_TRUE)
      SET_VALID(I_eventCommunity, eventEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_eventLastTimeSent:
    if ( snmpRmonEventLastTimeSentGet(USMDB_UNIT_CURRENT, eventEntryData.eventIndex,
                                      &eventEntryData.eventLastTimeSent) == L7_SUCCESS )
      SET_VALID(I_eventLastTimeSent, eventEntryData.valid);

    if (nominator != -1) break;
    /* else pass through */

  case I_eventOwner:
    if ( snmpRmonEventOwnerGet(USMDB_UNIT_CURRENT, eventEntryData.eventIndex, owner_buffer) == L7_SUCCESS )
    {
      if (SafeMakeOctetStringFromText(&eventEntryData.eventOwner, owner_buffer) == L7_TRUE)
        SET_VALID(I_eventOwner, eventEntryData.valid);
    }
    else
    {
      if (SafeMakeOctetStringFromText(&eventEntryData.eventOwner, ""))
        SET_VALID(I_eventOwner, eventEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_eventStatus:
    if ( snmpRmonEventStatusGet(USMDB_UNIT_CURRENT, eventEntryData.eventIndex,
                                &eventEntryData.eventStatus) == L7_SUCCESS )
      SET_VALID(I_eventStatus, eventEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, eventEntryData.valid) )
    return(NULL);

  return(&eventEntryData);
}

#ifdef SETS
int
   k_eventEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

   return(NO_ERROR);
}

int
   k_eventEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *doHead, doList_t *dp)
{
  eventEntry_t *data = (eventEntry_t *) (dp->data);

  /* check the length, since the Textual Convention OwnerString is defined in the
  IF-MIB as 0..255, but it should be 0..127 in this MIB */
  if (VALID(I_eventOwner, data->valid) &&
      data->eventOwner->length > 127)
    return(WRONG_LENGTH_ERROR);

   dp->state = SR_ADD_MODIFY;
   return(NO_ERROR);
}

int
   k_eventEntry_set_defaults(doList_t *dp)
{
   eventEntry_t *data = (eventEntry_t *) (dp->data);

   if ( (data->eventDescription = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   if ( (data->eventCommunity = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   if ( (data->eventOwner = MakeOctetStringFromText("")) == 0 )
   {
      return(RESOURCE_UNAVAILABLE_ERROR);
   }
   data->eventLastTimeSent = (SR_UINT32) 0;
   data->eventType = (SR_UINT32) 0;

   return(NO_ERROR);
}

int
   k_eventEntry_set(eventEntry_t *data,
                    ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];
  char owner_buffer[SNMP_BUFFER_LEN];
  L7_BOOL isCreateNewRow = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if ( snmpRmonEventEntryGet(L7_MATCH_RMON_EXACTSET, USMDB_UNIT_CURRENT, data->eventIndex) != L7_SUCCESS )
  {
    /* check for row creation */
    if ( VALID(I_eventStatus, data->valid) &&
         data->eventStatus == D_eventStatus_createRequest &&
         snmpRmonEventStatusSet(USMDB_UNIT_CURRENT, data->eventIndex, data->eventStatus) == L7_SUCCESS )
    {
      SET_VALID(I_eventStatus, tempValid);
      isCreateNewRow = L7_TRUE;
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if ( VALID(I_eventDescription, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (SPrintAscii(data->eventDescription, snmp_buffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }

    if (snmpRmonEventDescriptionSet(USMDB_UNIT_CURRENT, data->eventIndex, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_eventDescription, tempValid);
    }
  }

  if ( VALID(I_eventType, data->valid) )
  {
    if ( snmpRmonEventTypeSet(USMDB_UNIT_CURRENT, data->eventIndex, data->eventType) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_eventType, tempValid);
    }
  }

  if ( VALID(I_eventCommunity, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (SPrintAscii(data->eventCommunity, snmp_buffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }

    if (snmpRmonEventCommunitySet(USMDB_UNIT_CURRENT, data->eventIndex, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_eventCommunity, tempValid);
    }
  }

  if ( VALID(I_eventOwner, data->valid))
  {
    bzero(owner_buffer, SNMP_BUFFER_LEN);
    if (SPrintAscii(data->eventOwner, owner_buffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }

    if (snmpRmonEventOwnerSet(USMDB_UNIT_CURRENT, data->eventIndex, owner_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_eventOwner, tempValid);
    }
  }

  if ( VALID(I_eventStatus, data->valid) && (isCreateNewRow == L7_FALSE) &&
       (snmpRmonEventStatusSet(USMDB_UNIT_CURRENT, data->eventIndex, data->eventStatus) != L7_SUCCESS) )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }

  return(NO_ERROR);
}

#ifdef SR_eventEntry_UNDO
/* add #define SR_eventEntry_UNDO in sitedefs.h to
 * include the undo routine for the eventEntry family.
 */
int
   eventEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
  eventEntry_t *data = (eventEntry_t *) doCur->data;
  eventEntry_t *undodata = (eventEntry_t *) doCur->undodata;
  eventEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;
  L7_BOOL isUndoStatValid = L7_FALSE;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL)
    memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->eventStatus = D_eventStatus_invalid;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a deletion, create with original data */
    if((undodata->eventStatus == D_eventStatus_underCreation)
       && (data->eventStatus == D_eventStatus_invalid))
    {
        undodata->eventStatus = D_eventStatus_createRequest;
    }
    else
    {
       if((undodata->eventStatus == D_eventStatus_valid)
          && (data->eventStatus == D_eventStatus_invalid))
       {
         undodata->eventStatus = D_eventStatus_createRequest;
         /* if status changed from valid to invalid, then undo is done
          * with status as valid.
          */
         isUndoStatValid = L7_TRUE;
       }
    }
    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for undo */
  if( k_eventEntry_set(setdata, contextInfo, function) != NO_ERROR )
    return UNDO_FAILED_ERROR;

  /* If undoing a delete then only update the created row with undo data */
  if ( (undodata != NULL) && (data->eventStatus == D_eventStatus_invalid) )
  {
    memset(setdata->valid, 0xff, sizeof(data->valid));
    if( isUndoStatValid == L7_TRUE )
    {
      setdata->eventStatus = D_eventStatus_valid;
    }
    else /* We are here with eventStatus as UnderCreation and we do't want to change it */
    {
      CLR_VALID(I_eventStatus, setdata->valid);
    }
    if( k_eventEntry_set(setdata, contextInfo, function) != NO_ERROR )
      return UNDO_FAILED_ERROR;
  }

  return(NO_ERROR);
}
#endif /* SR_eventEntry_UNDO */

#endif /* SETS */

logEntry_t *
   k_logEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 logEventIndex,
                  SR_INT32 logIndex)
{
  static logEntry_t logEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];

  logEntryData.logEventIndex = logEventIndex;
  logEntryData.logIndex = logIndex;
  SET_VALID(I_logEventIndex, logEntryData.valid);
  SET_VALID(I_logIndex, logEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpRmonLogEntryGet(L7_MATCH_RMON_EXACTGET, USMDB_UNIT_CURRENT, logEntryData.logEventIndex, logIndex) != L7_SUCCESS) :
       ( (snmpRmonLogEntryGet(L7_MATCH_RMON_GETNEXT, USMDB_UNIT_CURRENT, logEntryData.logEventIndex, logIndex) != L7_SUCCESS) &&
         (snmpRmonLogEntryNextGet(USMDB_UNIT_CURRENT, &logEntryData.logEventIndex, &logEntryData.logIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(logEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
/* lvl7_@p2111 start */
  case I_logEventIndex:
  case I_logIndex:
    break;
/* lvl7_@p2111 end */

  case I_logTime:
    if ( snmpRmonLogTimeGet(USMDB_UNIT_CURRENT, logEntryData.logEventIndex,
                            logEntryData.logIndex,
                            &logEntryData.logTime) == L7_SUCCESS )
      SET_VALID(I_logTime, logEntryData.valid);
    break;

  case I_logDescription:
    if ( snmpRmonLogDescriptionGet(USMDB_UNIT_CURRENT, logEntryData.logEventIndex,
                                   logEntryData.logIndex,
                                   snmp_buffer) == L7_SUCCESS )
    {
      if (SafeMakeOctetStringFromText(&logEntryData.logDescription, snmp_buffer) == L7_TRUE)
        SET_VALID(I_logDescription, logEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, logEntryData.valid) )
    return(NULL);

  return(&logEntryData);
}
