/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_history.c
*
* @purpose  this is the core file for RMON history group
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

/*************************************************************
                    
*************************************************************/


#include "l7_common.h"
#include "rmon_include.h"
#include "rmon_common.h"
#include "rmon_index.h"
#include "rmon_history.h"
#include "usmdb_mib_rmon_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

/* Array of rmonHistoryControl entries, external here, it is already */ 
/* defined in file snmp_rmon.c */
extern rmonHistoryControlEntry_t *rmonHistoryControlEntryArray;

/* The SNMP to array index mapping handle.
*/
static L7_uint32 historyControlMap;


/*********************************************************************
*
* @purpose    Initialize the RMON History Cintrol Mapping table.
*
* @returntype L7_SUCCESS    History Control  Mapping table is initialized.
*             L7_FAILURE    Initialization failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonHistoryControlTableInitialize(void)
{
  L7_uint32 i;
  rmonHistoryControlEntry_t* hce;
  historyControlMap = rmonMapCreate(RMON_HISTORY_CONTROL_ENTRY_MAX_NUM);

  for (i = 0; i < RMON_HISTORY_CONTROL_ENTRY_MAX_NUM; i++)
  {
    hce = &rmonHistoryControlEntryArray[i];
    memset (hce, 0, sizeof (rmonHistoryControlEntry_t));
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    History Control Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonHistoryControlTableUnconfigure(void)
{
  L7_uint32 index = 0;
  L7_RC_t rc = L7_SUCCESS;

  /* iterate over all entries in the table */
  while (rmonHistoryControlEntryNextGet(&index) == L7_SUCCESS)
  {
    if (rmonHistoryControlTableRemove(index) != L7_SUCCESS)
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


/************************************************************************************
*
* @purpose    Creates a HistoryControl entry with specified index, ifNumber, ifIndex, 
*             controlInterval and Status values
* 
* @param      index               index of HistoryControl Entry to create
* @param      intIfNum            internal interface number of HistoryControl Entry to create
* @param      ifIndex             ifIndex value of the port which has been created
* @param      controlInterval     sampling interval of HistoryControl Entry to create
* @param      Status              Status to be set( here the status is set to VALID)
*
* @returntype L7_SUCCESS    if creation was successful
*             L7_FAILURE    if HistoryControl Entry with that index exists, or if creation
*                            was not successful
*          
* @notes      
*
* @end
*
**************************************************************************************/

L7_RC_t 
rmonHistoryControlEntryCreate(L7_uint32 Index, L7_uint32 intIfNum, L7_uint32 ifIndex, 
                              L7_uint32 controlInterval,L7_ushort16 Status)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapInsert (historyControlMap, Index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Resolve array index from the SNMP index. */
  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  rmonHistoryControlEntryDefaultsSet(hce,Index,intIfNum,ifIndex,controlInterval,Status);

  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose    Sets the members of HistoryControl entry to default values
* 
* @param      hce             HistoryControlEntry for which the default values have to be set
* @param      index           index of HistoryControl Entry to create
* @param      ifIndex         ifIndex value of the port which has been created
* @param      controlInterval sampling interval for a history bucket 
* @param      Status          Status to be set( here the status is set to VALID)
*
* @returntype L7_SUCCESS    if setting of default values was successful
*             L7_FAILURE    if setting of default values was unsuccessful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t 
rmonHistoryControlEntryDefaultsSet(rmonHistoryControlEntry_t* hce, L7_uint32 Index, 
                                   L7_uint32 intIfNum, L7_uint32 ifIndex,
                                   L7_uint32 controlInterval, L7_ushort16 Status)
{
  L7_char8 *historyControlOwner = FD_RMON_HISTORY_CTRL_OWNER;
  L7_uint32 count = 0;
  hce->historyControlIndex = Index;
  hce->dataSource = ifIndex;
  hce->bucketsRequested = RMON_HISTORY_CONTROL_DEFAULT_BUCKETSREQUESTED;

  if (hce->bucketsRequested > RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED)
    hce->bucketsGranted   = RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED;
  else
    hce->bucketsGranted   = hce->bucketsRequested;
  hce->interval         = controlInterval;
  memcpy(hce->owner,historyControlOwner,strlen(historyControlOwner));
  hce->status = Status;

  hce->interface = intIfNum;

  hce->lastSampleIndex = 0;

  hce->lastSampleTime = osapiUpTimeRaw();

  hce->bucketNumber = 0;

  /* Fill the baseline EtherHistory bucket */
  if (rmonHistoryControlBaselineEntryFill(hce)!=L7_SUCCESS)
    return L7_FAILURE;

  /* Initializing bucketQueue */
  for (count = 1; count <= RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED; count++)
  {
    hce->bucketQueue[count].historyIndex = 0;
    hce->bucketQueue[count].historySampleIndex = 0;
    hce->bucketQueue[count].intervalStart = 0;
    hce->bucketQueue[count].dropEvents = 0;
    hce->bucketQueue[count].octets = 0;
    hce->bucketQueue[count].pkts = 0;
    hce->bucketQueue[count].broadcastPkts = 0;
    hce->bucketQueue[count].multicastPkts = 0;
    hce->bucketQueue[count].CRCAlignErrors = 0;
    hce->bucketQueue[count].undersizePkts = 0;
    hce->bucketQueue[count].oversizePkts = 0;
    hce->bucketQueue[count].fragments = 0;
    hce->bucketQueue[count].jabbers = 0;
    hce->bucketQueue[count].collisions = 0;
    hce->bucketQueue[count].utilization = 0;
    hce->bucketQueue[count].validEtherHistoryEntry = L7_FALSE;

  }

  hce->validHistoryControlEntry = L7_TRUE;
  return L7_SUCCESS;                          

}

/*********************************************************************
*
* @purpose    Insert  an HistoryControl entry for a configured lag in the HistoryControl Table, 
*             and also in the Index Mapping table with default values
* 
* @param      lagInterface  internal interface number for the lag
* @param      ifIndex       external  interface number for the lag
* @param      controlInterval       history control interval for this entry
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if insertion was successful
*             L7_FAILURE    if insertion was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t
rmonHistoryControlTableInsert(L7_uint32 lagInterface, L7_uint32 ifIndex, 
                              L7_uint32 controlInterval,L7_uint32 *lagSnmpIndex)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_uint32 Index;
  L7_RC_t rc;

  rc = rmonMapFirstAvailableIndexGet (historyControlMap, &Index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = rmonMapInsert (historyControlMap, Index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Resolve array index from the SNMP index. */
  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  rmonHistoryControlEntryDefaultsSet(hce,Index,lagInterface,ifIndex,
                                     controlInterval,RMON_HISTORY_CONTROL_STATUS_VALID);

  *lagSnmpIndex = Index;
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Remove  an HistoryControl entry for an unconfigured lag from the HistoryControl Table, 
*             and also from the Index Mapping table with default values
* 
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if removal was successful
*             L7_FAILURE    if removal was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t
rmonHistoryControlTableRemove(L7_uint32 lagSnmpIndex)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_RC_t rc;

  /* Resolve array index from the SNMP index. */
  rc = rmonMapMatch (historyControlMap, lagSnmpIndex, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  memset (hce, 0, sizeof (rmonHistoryControlEntry_t));

  rc =  rmonMapRemove (historyControlMap, lagSnmpIndex);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Fills the baseline bucket of HistoryControl Entry
* 
* @param      hce           HistoryControIEntry for which baseline bucket is being filled
*
* @returntype L7_SUCCESS    if baseline bucket filling was successful
*             L7_FAILURE    if baseline bucket filling was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/
L7_RC_t  rmonHistoryControlBaselineEntryFill(rmonHistoryControlEntry_t* hce)
{
  L7_uint32 upTime;
  L7_uint32 dropEventsRmon=0;
  L7_uint32 octetsRmon=0;
  L7_uint32 pktsRmon=0;
  L7_uint32 broadcastPktsRmon=0;
  L7_uint32 multicastPktsRmon=0;
  L7_uint32 CRCAlignErrorsRmon=0;
  L7_uint32 undersizePktsRmon=0;
  L7_uint32 oversizePktsRmon=0;
  L7_uint32 fragmentsRmon=0;
  L7_uint32 jabbersRmon=0;
  L7_uint32 collisionsRmon=0;
  L7_uint32 intIfNum = 0;

  hce->baseline.historyIndex = hce->historyControlIndex;

  hce->baseline.historySampleIndex = 1;

  if (hce == L7_NULLPTR)
    return L7_FAILURE;

  /* get the current uptime */
  upTime = osapiUpTimeRaw();
  hce->lastSampleTime = upTime;
  hce->baseline.intervalStart = upTime;

  /* Get the internal interface number associated with the external ifIndex.*/
  if (rmonHistoryControlInternalInteraceNumGet( hce->historyControlIndex, &intIfNum)!= L7_SUCCESS)
    return L7_FAILURE;

  if (usmDbEtherStatsDropEventsGet(USMDB_UNIT_CURRENT, intIfNum, &dropEventsRmon) == L7_SUCCESS)
  {
    hce->baseline.dropEvents = dropEventsRmon;
  }
  else
  {
    hce->baseline.dropEvents = 0;
  }

  if (usmDbEtherStatsOctetsGet(USMDB_UNIT_CURRENT, intIfNum, &octetsRmon) == L7_SUCCESS)
  {
    hce->baseline.octets = octetsRmon;
  }
  else
  {
    hce->baseline.octets = 0;
  }

  if (usmDbEtherStatsPktsGet(USMDB_UNIT_CURRENT, intIfNum, &pktsRmon) == L7_SUCCESS)
  {
    hce->baseline.pkts = pktsRmon;
  }
  else
  {
    hce->baseline.pkts = 0;
  }

  if (usmDbEtherStatsBroadcastPktsGet(USMDB_UNIT_CURRENT, intIfNum, &broadcastPktsRmon) == L7_SUCCESS)
  {
    hce->baseline.broadcastPkts = broadcastPktsRmon;
  }
  else
  {
    hce->baseline.broadcastPkts = 0;
  }

  if (usmDbEtherStatsMulticastPktsGet(USMDB_UNIT_CURRENT, intIfNum, &multicastPktsRmon) == L7_SUCCESS)
  {
    hce->baseline.multicastPkts = multicastPktsRmon;
  }
  else
  {
    hce->baseline.multicastPkts = 0;
  }

  if (usmDbEtherStatsCRCAlignErrorsGet(USMDB_UNIT_CURRENT, intIfNum, &CRCAlignErrorsRmon) == L7_SUCCESS)
  {
    hce->baseline.CRCAlignErrors = CRCAlignErrorsRmon;
  }
  else
  {
    hce->baseline.CRCAlignErrors = 0;
  }

  if (usmDbEtherStatsUndersizePktsGet(USMDB_UNIT_CURRENT, intIfNum, &undersizePktsRmon) == L7_SUCCESS)
  {
    hce->baseline.undersizePkts = undersizePktsRmon;
  }
  else
  {
    hce->baseline.undersizePkts = 0;
  }

  if (usmDbEtherStatsOversizePktsGet(USMDB_UNIT_CURRENT, intIfNum, &oversizePktsRmon) == L7_SUCCESS)
  {
    hce->baseline.oversizePkts = oversizePktsRmon;
  }
  else
  {
    hce->baseline.oversizePkts = 0;
  }

  if (usmDbEtherStatsFragmentsGet(USMDB_UNIT_CURRENT, intIfNum, &fragmentsRmon) == L7_SUCCESS)
  {
    hce->baseline.fragments = fragmentsRmon;
  }
  else
  {
    hce->baseline.fragments = 0;
  }

  if (usmDbEtherStatsJabbersGet(USMDB_UNIT_CURRENT, intIfNum, &jabbersRmon) == L7_SUCCESS)
  {
    hce->baseline.jabbers = jabbersRmon;
  }
  else
  {
    hce->baseline.jabbers = 0;
  }

  if (usmDbEtherStatsCollisionsGet(USMDB_UNIT_CURRENT, intIfNum, &collisionsRmon) == L7_SUCCESS)
  {
    hce->baseline.collisions = collisionsRmon;
  }
  else
  {
    hce->baseline.collisions = 0;
  }

  hce->baseline.utilization = 0;

  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Fills the EtherHistory entry of HistoryControl Entry
* 
* @param      hce           HistoryControIEntry for which EtherHistory entry is being filled
*             hb            EtherHistory entry being filled
*             bucketCount  this is bucketCount, or, historySampleIndex corresponding to the
*                           EtherHistory entry
*
*
* @returntype L7_SUCCESS    if bucket filling was successful
*             L7_FAILURE    if bucket filling was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t
rmonEtherHistoryEntryFill(rmonHistoryControlEntry_t* hce, L7_uint32 bucketNumber)
{
  L7_uint32 dropEventsRmon=0;
  L7_uint32 octetsRmon=0;
  L7_uint32 pktsRmon=0;
  L7_uint32 broadcastPktsRmon=0;
  L7_uint32 multicastPktsRmon=0;
  L7_uint32 CRCAlignErrorsRmon=0;
  L7_uint32 undersizePktsRmon=0;
  L7_uint32 oversizePktsRmon=0;
  L7_uint32 fragmentsRmon=0;
  L7_uint32 jabbersRmon=0;
  L7_uint32 collisionsRmon=0;
  L7_uint32 intIfNum = 0;
  L7_uint32 intfSpeed;

  rmonEtherHistoryEntry_t* hb;


  hb = &hce->bucketQueue[bucketNumber];

  if (hce == L7_NULLPTR)
    return L7_FAILURE;

  if (hb == L7_NULLPTR)
    return L7_FAILURE;


  hb->historyIndex =  hce->historyControlIndex;  
  hb->historySampleIndex =hce->lastSampleIndex;
  hb->intervalStart = hce->lastSampleTime;


  /* Get the internal interface number associated with the HistoryControl Index */
  if (rmonHistoryControlInternalInteraceNumGet( hce->historyControlIndex, &intIfNum)!= L7_SUCCESS)
    return L7_FAILURE;

  if (usmDbEtherStatsDropEventsGet(USMDB_UNIT_CURRENT, intIfNum, &dropEventsRmon) == L7_SUCCESS)
  {
    hb->dropEvents = dropEventsRmon - (hce->baseline.dropEvents);
  }
  else
  {
    hb->dropEvents = 0;
  }

  if (usmDbEtherStatsOctetsGet(USMDB_UNIT_CURRENT, intIfNum, &octetsRmon) == L7_SUCCESS)
  {
    hb->octets = octetsRmon - (hce->baseline.octets);
  }
  else
  {
    hb->octets = 0;
  }

  if (usmDbEtherStatsPktsGet(USMDB_UNIT_CURRENT, intIfNum, &pktsRmon) == L7_SUCCESS)
  {
    hb->pkts = pktsRmon - (hce->baseline.pkts);
  }
  else
  {
    hb->pkts = 0;
  }

  if (usmDbEtherStatsBroadcastPktsGet(USMDB_UNIT_CURRENT, intIfNum, &broadcastPktsRmon) == L7_SUCCESS)
  {
    hb->broadcastPkts = broadcastPktsRmon -(hce->baseline.broadcastPkts);
  }
  else
  {
    hb->broadcastPkts = 0;
  }

  if (usmDbEtherStatsMulticastPktsGet(USMDB_UNIT_CURRENT, intIfNum, &multicastPktsRmon) == L7_SUCCESS)
  {
    hb->multicastPkts = multicastPktsRmon - (hce->baseline.multicastPkts);
  }
  else
  {
    hb->multicastPkts = 0;
  }

  if (usmDbEtherStatsCRCAlignErrorsGet(USMDB_UNIT_CURRENT, intIfNum, &CRCAlignErrorsRmon) == L7_SUCCESS)
  {
    hb->CRCAlignErrors = CRCAlignErrorsRmon - (hce->baseline.CRCAlignErrors);
  }
  else
  {
    hb->CRCAlignErrors = 0;
  }

  if (usmDbEtherStatsUndersizePktsGet(USMDB_UNIT_CURRENT, intIfNum, &undersizePktsRmon) == L7_SUCCESS)
  {
    hb->undersizePkts = undersizePktsRmon - (hce->baseline.undersizePkts);
  }
  else
  {
    hb->undersizePkts = 0;
  }

  if (usmDbEtherStatsOversizePktsGet(USMDB_UNIT_CURRENT, intIfNum, &oversizePktsRmon) == L7_SUCCESS)
  {
    hb->oversizePkts = oversizePktsRmon - (hce->baseline.oversizePkts);
  }
  else
  {
    hb->oversizePkts = 0;
  }

  if (usmDbEtherStatsFragmentsGet(USMDB_UNIT_CURRENT, intIfNum, &fragmentsRmon) == L7_SUCCESS)
  {
    hb->fragments = fragmentsRmon - (hce->baseline.fragments);
  }
  else
  {
    hb->fragments = 0;
  }

  if (usmDbEtherStatsJabbersGet(USMDB_UNIT_CURRENT, intIfNum, &jabbersRmon) == L7_SUCCESS)
  {
    hb->jabbers = jabbersRmon - (hce->baseline.jabbers);
  }
  else
  {
    hb->jabbers = 0;
  }

  if (usmDbEtherStatsCollisionsGet(USMDB_UNIT_CURRENT, intIfNum, &collisionsRmon) == L7_SUCCESS)
  {
    hb->collisions = collisionsRmon - (hce->baseline.collisions);
  }
  else
  {
    hb->collisions = 0;
  }

  /* According to the formula given in RFC 2819, the network utilization is calculated as follows */
  hb->utilization = ((hb->pkts*(9.6+6.4)) + (hb->octets*0.8))/(hce->interval * 10000);

  nimGetIntfSpeed(intIfNum,&intfSpeed);

  switch (intfSpeed)
  {
  
  case L7_PORTCTRL_PORTSPEED_LAG:
    /* lag utilization can be more than 100 % */
  case L7_PORTCTRL_PORTSPEED_HALF_10T:
  case L7_PORTCTRL_PORTSPEED_FULL_10T:
    break;

  case L7_PORTCTRL_PORTSPEED_HALF_100TX:
  case L7_PORTCTRL_PORTSPEED_FULL_100TX:
  case L7_PORTCTRL_PORTSPEED_FULL_100FX:
    hb->utilization = hb->utilization / 10;
    break;

  case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
    hb->utilization = hb->utilization / 100;
    break;

  /* PTin added (2.5G) */
  case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
      hb->utilization = hb->utilization / 250;
      break;
  /* PTin end */

  case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      hb->utilization = hb->utilization / 1000;
      break;

  case L7_PORTCTRL_PORTSPEED_AAL5_155:
    hb->utilization = hb->utilization / 15;
    break;

  default:break;
  }

  hb->validEtherHistoryEntry = L7_TRUE;

  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose    Deletes a HistoryControl entry with the input ifIndex value
* 
* @param      ifIndex           ifIndex value of the HistoryControl entry to be deleted
* @param      controlInterval   sampling interval for a history bucket
*
* @returntype L7_SUCCESS    if deletion was successful
*             L7_FAILURE    if deletion was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonHistoryControlEntryDelete(L7_uint32 ifIndex, L7_uint32 controlInterval)
{
  rmonHistoryControlEntry_t* hce;
  L7_RC_t rc;
  L7_BOOL interfaceFound = L7_FALSE;
  L7_uint32 next_snmp_index;
  L7_uint32 lowest_snmp_index;
  L7_uint32 highest_snmp_index;
  L7_uint32 index;

  L7_uint32 array_index;


  rc = rmonMapLowestGet(historyControlMap, &lowest_snmp_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  index = lowest_snmp_index;

  rc = rmonMapHighestGet(historyControlMap, &highest_snmp_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  while (index<= highest_snmp_index)
  {
    rc = rmonMapMatch (historyControlMap, index, &array_index);
    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    hce = &rmonHistoryControlEntryArray[array_index];
    if (hce->dataSource == ifIndex && hce->interval == controlInterval)
    {
      interfaceFound = L7_TRUE;
      break;
    }

    rc =  rmonMapNextGet (historyControlMap, index, &next_snmp_index);
    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    index = next_snmp_index;

  }
  if (interfaceFound == L7_TRUE)
  {
    rc =  rmonMapRemove (historyControlMap, index);
    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Gets value of intIfNum for HistoryControl entry
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value of internal interface number
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlInternalInteraceNumGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  *val = hce->interface;
  return L7_SUCCESS;


}


/*********************************************************************
*
* @purpose    Gets value of Index of the History Control table corresponding to the external interface number
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      this method is redundant now, if not used, should be removed later
*
* @end
*
*********************************************************************/
/************
L7_RC_t
rmonHistoryControlInterfaceIndexGet(L7_uint32 ifIndex, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 Index;
  for( Index =1;Index < RMON_HISTORY_CONTROL_ENTRY_MAX_NUM; Index++)
  {
      hce = &rmonHistoryControlEntryArray[Index];
      if(hce->dataSource != ifIndex)
          continue;
      else{
          *val = Index;
          break;
      }
  
  }

  return L7_SUCCESS;
}
***********/

/*******************************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
*             Index            Index of HistoryControlEntry to check
* 
* @returntype L7_SUCCESS    if valid entry
*             L7_FAILURE    if not a valid entry
*          
* @notes      none
*
* @end
*
**********************************************************************************/
L7_RC_t
rmonHistoryControlEntryGet(L7_uint32 Index)
{
  rmonHistoryControlEntry_t* hce;


  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  if ((hce->historyControlIndex==Index) &&
      ((hce->status == RMON_HISTORY_CONTROL_STATUS_VALID) ||
       (hce->status == RMON_HISTORY_CONTROL_STATUS_UNDERCREATION)))
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Checks index for SNMP GETNEXT requests
*
* @param      Index         Index of HistoryControlEntry to check
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
rmonHistoryControlEntryNextGet(L7_uint32 *Index)
{
  L7_RC_t rc;
  L7_uint32 next_snmp_index;
  rc =  rmonMapNextGet (historyControlMap, *Index, &next_snmp_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  *Index = next_snmp_index;


  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Gets value of HistoryControlDataSource for HistoryControl entry

*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlDataSourceGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  *val = hce->dataSource;
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Gets value of HistoryControlBucketsRequested for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBucketsRequestedGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  *val = hce->bucketsRequested;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Gets value of HistoryControlBucketsGranted for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBucketsGrantedGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  *val = hce->bucketsGranted;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Gets value of HistoryControlInterval for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlIntervalGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  *val = hce->interval;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Gets value of HistoryControlOwner for HistoryControl entry
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlOwnerGet(L7_uint32 Index, L7_char8 *buf)
{
  rmonHistoryControlEntry_t* hce;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  strcpy(buf, hce->owner);
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Gets value of HistoryControlStatus for HistoryControl entry
*
* @param      Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlStatusGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  *val = hce->status;
  return L7_SUCCESS;

}

/* SNMP SET Requests */
/*********************************************************************
*
* @purpose    Sets value of HistoryControlDataSource for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlDataSourceSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_uint32 intIfNum;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];
  /* value of datasource already matches to the value to be set, so no change */
  if ( hce->dataSource == val)
    return L7_SUCCESS;

  /* else change the value to val */
  if ((hce->status == RMON_HISTORY_CONTROL_STATUS_UNDERCREATION)  &&
      usmDbVisibleExtIfNumberCheck(UnitIndex,val) == L7_SUCCESS)
  {
    hce->dataSource = val;
    /* Also set the corresponding internal interface number */
    if (usmDbIntIfNumFromExtIfNum(val, &intIfNum) == L7_SUCCESS)
    {
      hce->interface = intIfNum;

      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of HistoryControlBucketsRequested for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBucketsRequestedSet( L7_uint32 Index, L7_uint32 val)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_RC_t rc;
/*  L7_uint32 bucketIndex = 0;*/

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  /* value of BucketsRequested already matches to the value to be set, so no change */
  if ( hce->bucketsRequested == val)
    return L7_SUCCESS;


  /* else change the value to val */
  if (val > 0)
  {
    hce->bucketsRequested = val;

    if (hce->bucketsRequested >  RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED)
      hce->bucketsGranted =  RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED;
    else
      hce->bucketsGranted = hce->bucketsRequested;
#if 0
    /* check to see if we have more buckets filled than granted now */
    if (hce->bucketNumber > hce->bucketsGranted)
    {
      for (bucketIndex = 1; bucketIndex < hce->bucketsGranted; bucketIndex++)
      {
        hce->bucketQueue[bucketIndex].historySampleIndex =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].historySampleIndex;
        hce->bucketQueue[bucketIndex].intervalStart =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].intervalStart;
        hce->bucketQueue[bucketIndex].dropEvents =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].dropEvents;
        hce->bucketQueue[bucketIndex].octets =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].octets;
        hce->bucketQueue[bucketIndex].pkts =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].pkts;
        hce->bucketQueue[bucketIndex].broadcastPkts =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].broadcastPkts;
        hce->bucketQueue[bucketIndex].multicastPkts =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].multicastPkts;
        hce->bucketQueue[bucketIndex].CRCAlignErrors =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].CRCAlignErrors;
        hce->bucketQueue[bucketIndex].undersizePkts =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].undersizePkts;
        hce->bucketQueue[bucketIndex].oversizePkts =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].oversizePkts;
        hce->bucketQueue[bucketIndex].fragments =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].fragments;
        hce->bucketQueue[bucketIndex].jabbers =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].jabbers;
        hce->bucketQueue[bucketIndex].collisions =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].collisions;
        hce->bucketQueue[bucketIndex].utilization =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].utilization;
        hce->bucketQueue[bucketIndex].validEtherHistoryEntry =
        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].validEtherHistoryEntry;

        hce->bucketQueue[bucketIndex + (hce->bucketNumber - hce->bucketsGranted)].validEtherHistoryEntry = L7_FALSE;
      }

      /* set the last used bucket to the last bucket granted */
      hce->bucketNumber = hce->bucketsGranted;
    }
#endif
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    Sets value of HistoryControlInterval for HistoryControl entry
*
* @param      UnitIndex     index of current unit
*             Index         index of HistoryControl entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlIntervalSet( L7_uint32 Index, L7_uint32 val)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  /* value of BucketsRequested already matches to the value to be set, so no change */
  if ( hce->interval == val)
    return L7_SUCCESS;


  /* else change the value to val */
  if ((hce->status == RMON_HISTORY_CONTROL_STATUS_UNDERCREATION) && 
      ((val >= RMON_HISTORY_CONTROL_INTERVAL_MIN) && (val <= RMON_HISTORY_CONTROL_INTERVAL_MAX)))
  {
    /* Refresh the timer of bucket filling for this History entry to zero */
    hce->timerHistory =0;
    hce->interval = val;
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}

/*********************************************************************
*
* @purpose    Sets value of HistoryControlOwner for HistoryControl entry
*
* @param      Index         index of HistoryControl to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlOwnerSet(L7_uint32 Index, L7_char8 *buf)
{
  rmonHistoryControlEntry_t* hce;
  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (strlen(buf) > RMON_HISTORY_CONTROL_OWNER_MAX_LENGTH)
    return L7_FAILURE;

  bzero( hce->owner, RMON_HISTORY_CONTROL_OWNER_MAX_LENGTH+1);

  memcpy(hce->owner,buf,strlen(buf));

  return L7_SUCCESS;

}

/*******************  From RFC 2819 **********************************
                          Create   Under
         To:       Valid  Request  Creation  Invalid
 From:
 valid             OK       NO        OK       OK
 createRequest     N/A      N/A       N/A      N/A
 underCreation     OK       NO        OK       OK
 invalid           NO       NO        NO       OK
 nonExistent       NO       OK        NO       OK
**********************************************************************/

/*********************************************************************
*
* @purpose    Sets value of HistoryControlStatus for HCB entry
*
* @param      Index         index of HistoryControlEntry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlStatusSet(L7_uint32 Index, L7_uint32 val)
{

  rmonHistoryControlEntry_t* hce;
  L7_RC_t rc = L7_FAILURE;

  L7_uint32 array_index;
  L7_RC_t rc1;
  L7_uint32 firstIntIfNum=0;
  L7_uint32 firstIfIndex=0;

  /* If the input status is VALID, UNDERCREATION, or INVALID, the entry should be there in the */
  /* mapping table */
  if ((val == RMON_HISTORY_CONTROL_STATUS_VALID) ||
      (val == RMON_HISTORY_CONTROL_STATUS_UNDERCREATION) ||
      (val ==  RMON_HISTORY_CONTROL_STATUS_INVALID))
  {

    rc1 = rmonMapMatch (historyControlMap, Index, &array_index);
    if (rc1 != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    hce = &rmonHistoryControlEntryArray[array_index];


    if (hce->status == val)
      return L7_SUCCESS;
  }
  else
  {

    if (val == RMON_HISTORY_CONTROL_STATUS_CREATEREQUEST)
    {
      rc1 = rmonMapMatch (historyControlMap, Index, &array_index);
      if (rc1 == L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      rc1 = rmonMapInsert (historyControlMap, Index);
      if (rc1 != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      rc1 = rmonMapMatch (historyControlMap, Index, &array_index);
      if (rc1 != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      hce = &rmonHistoryControlEntryArray[array_index];


      if (usmDbValidIntIfNumFirstGet(&firstIntIfNum) == L7_SUCCESS &&
          usmDbExtIfNumFromIntIfNum(firstIntIfNum, &firstIfIndex) == L7_SUCCESS)
      {
        /* for default values, use the first internal and external interface */
        rmonHistoryControlEntryDefaultsSet(hce, Index, firstIntIfNum, firstIfIndex,
                                           RMON_HISTORY_CONTROL_DEFAULT_INTERVAL,
                                           RMON_HISTORY_CONTROL_STATUS_UNDERCREATION);
        return L7_SUCCESS;
      }
      return L7_FAILURE;

    }
    else
    {
      return L7_FAILURE;
    }
  }


  switch (hce->status)
  {
  case RMON_HISTORY_CONTROL_STATUS_VALID:
    switch (val)
    {
    case RMON_HISTORY_CONTROL_STATUS_UNDERCREATION:
      hce->status = val;
      hce->validHistoryControlEntry = L7_TRUE;

      /* if the status is altered from VALID to UNDERCREATION, the timer is stopped to */
      /* halt the bucketfilling */
      rmonHistoryControlBlockTimerStop(hce);
      rc = L7_SUCCESS;
      break;
    case RMON_HISTORY_CONTROL_STATUS_INVALID:
      rc1 =  rmonMapRemove (historyControlMap, Index);
      if (rc1 != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      memset (hce, 0, sizeof (rmonHistoryControlEntry_t));
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
    }
    break;

  case RMON_HISTORY_CONTROL_STATUS_UNDERCREATION:
    switch (val)
    {
    case RMON_HISTORY_CONTROL_STATUS_VALID:

      if ((usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,hce->dataSource)!= L7_SUCCESS))
        return L7_FAILURE;

      hce->status = val;
      hce->validHistoryControlEntry = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case RMON_HISTORY_CONTROL_STATUS_INVALID:
      rc1 =  rmonMapRemove (historyControlMap, Index);
      if (rc1 != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      memset (hce, 0, sizeof (rmonHistoryControlEntry_t));
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
    }
    break;
  }/* end of outer switch */


  return rc;  
}
/*********************************************************************
*
* @purpose    Stop the timer for bucketqueue filling
*
* @param      hce     pointer to HistoryControlEntry object doing the polling
*
* @returntype L7_SUCCESS  always
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlBlockTimerStop(rmonHistoryControlEntry_t* hce)
{
  L7_uint32 count;

  /* Refresh the bucketNumber for this HistoryControl entry to be zero, s.t., buckets for */
  /* this entry can be filled from start */
  hce->bucketNumber = 0;

  /* As sampling of buckets stops for this entry, the lastSampleIndex is made zero */ 
  hce->lastSampleIndex = 0;

  /* For all buckets of this historyControlEntry set the default values */
  for (count = 1;count<=RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED;count++)
  {
    hce->bucketQueue[count].historyIndex = hce->historyControlIndex;
    hce->bucketQueue[count].historySampleIndex = 0;
    hce->bucketQueue[count].intervalStart = 0;
    hce->bucketQueue[count].dropEvents = 0;
    hce->bucketQueue[count].octets = 0;
    hce->bucketQueue[count].pkts = 0;
    hce->bucketQueue[count].broadcastPkts = 0;
    hce->bucketQueue[count].multicastPkts = 0;
    hce->bucketQueue[count].CRCAlignErrors = 0;
    hce->bucketQueue[count].undersizePkts = 0;
    hce->bucketQueue[count].oversizePkts = 0;
    hce->bucketQueue[count].fragments = 0;
    hce->bucketQueue[count].jabbers = 0;
    hce->bucketQueue[count].collisions = 0;
    hce->bucketQueue[count].utilization = 0;
    hce->bucketQueue[count].validEtherHistoryEntry = L7_FALSE;
  }

  return L7_SUCCESS;

}



/*********************************************************************
*
* @purpose    To check whether a bucket with the given SampleIndex exists or not 
*             for the particular HistoryControl entry, also it gets the corresponding 
*             BucketNumber of the BucketQueue
*
* @param      hce            pointer to HistoryControlEntry object doing the polling
*             SampleIndex    historyControlSampleIndex for this sample
*             BucketNumber   bucketNumber in the bucketQueue for this sampleIndex
*
* @returntype L7_SUCCESS  always
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonHistoryControlIsBucketAvailableCheck(rmonHistoryControlEntry_t* hce,L7_uint32 SampleIndex,
                                         L7_uint32 *BucketNumber)
{
/*  rmonEtherHistoryEntry_t* ehe;
  L7_uint32 count;*/
  L7_RC_t   rc = L7_FAILURE;

  if ((SampleIndex <= hce->lastSampleIndex) &&
      ((hce->lastSampleIndex < hce->bucketsGranted) ||
       (SampleIndex > (hce->lastSampleIndex - hce->bucketsGranted))))
  {
    *BucketNumber = SampleIndex % RMON_HISTORY_CONTROL_MAX_BUCKETSGRANTED;
    if (hce->bucketQueue[*BucketNumber].validEtherHistoryEntry == L7_TRUE &&
        hce->bucketQueue[*BucketNumber].historySampleIndex == SampleIndex)
    {
      rc = L7_SUCCESS;
    }
  }

#if 0
  for ( count =1;count <= hce->bucketsGranted; count++)
  {
    ehe = &(hce->bucketQueue[count]);
    if (ehe->historySampleIndex != SampleIndex)
      continue;
    else
    {
      *BucketNumber = count;
      rc = L7_SUCCESS;
      break;
    }

  }
#endif
  return rc;

}




/*********************************************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
* @param      intSearchType    SearchType, whether it is EXACT GET, EXCATSET, or GETNEXT
*             integerValue     it holds the value of nominator or input HistoryControlStatus,
*                              depending upon searchType
*             Index            Index of HistoryControlEntry to check
*             sampleIndex      sample index for the etherHistory entry
* 
* @returntype L7_SUCCESS    if valid entry
*             L7_FAILURE    if not a valid entry
*          
* @notes      none
*
* @end
*
**********************************************************************************************/
L7_RC_t
rmonEtherHistoryEntryGet(L7_uint32 Index,L7_uint32 SampleIndex)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  /* To check whether a bucket with the given SampleIndex exists or not for the particular */
  /* HistoryControl entry, also it gets the corresponding BucketNumber of the BucketQueue  */
  if (rmonHistoryControlIsBucketAvailableCheck(hce,SampleIndex,&BucketNumber) != L7_SUCCESS)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);


  if ((hce->historyControlIndex != Index) || (ehe ->historyIndex != Index) ||
      (ehe->validEtherHistoryEntry != L7_TRUE))
    return L7_FAILURE;
  return L7_SUCCESS;

}

/*******************************************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
* @param      Index         Index of HistoryControlEntry to check
*             SampleIndex   sampleIndex of the etherHistory to check
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************************************/
L7_RC_t
rmonEtherHistoryEntryNextGet(L7_uint32 *Index, L7_uint32 *SampleIndex)
{

  rmonHistoryControlEntry_t* hce;
/*  rmonEtherHistoryEntry_t*   ehe;*/
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_uint32 highest_snmp_index;
  L7_uint32 next_snmp_index;
  L7_RC_t   rc = L7_FAILURE;
  L7_RC_t rc1;


  rc1 = rmonMapHighestGet(historyControlMap, &highest_snmp_index);
  if (rc1 != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (rmonMapMatch (historyControlMap, *Index, &array_index) != L7_SUCCESS)
  {
    rc1 =  rmonMapNextGet (historyControlMap, *Index, &next_snmp_index);
    if (rc1 != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    *Index = next_snmp_index;
  }

  while (*Index <= highest_snmp_index)
  {
    rc1 = rmonMapMatch (historyControlMap, *Index, &array_index);
    if (rc1 != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    hce = &rmonHistoryControlEntryArray[array_index];

    if (hce->status ==  RMON_HISTORY_CONTROL_STATUS_VALID)
    {
      if (rmonHistoryControlIsBucketAvailableCheck(hce, *SampleIndex, &BucketNumber) == L7_FAILURE)
      {
        if (*SampleIndex <= (hce->lastSampleIndex - hce->bucketsGranted))
        {
          /* get the first available index for this row */
          *SampleIndex = ((hce->lastSampleIndex - hce->bucketsGranted) + 1);
          if (rmonHistoryControlIsBucketAvailableCheck(hce, *SampleIndex, &BucketNumber) == L7_SUCCESS)
            return L7_SUCCESS;
        }
        else
        {
          *SampleIndex = 1;

          rc1 =  rmonMapNextGet (historyControlMap, *Index, &next_snmp_index);
          if (rc1 != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
          *Index = next_snmp_index;
        }
      }
      else
      {
        return L7_SUCCESS;
      }

#if 0
      if (rmonHistoryControlIsBucketAvailableCheck(hce, *SampleIndex, &BucketNumber) == L7_FAILURE)
      {
        if (*SampleIndex <  hce->bucketQueue[1].historySampleIndex)
        {
          /* SampleIndex jumps to the historySampleIndex of the first bucket */
          *SampleIndex =  hce->bucketQueue[1].historySampleIndex;

          /* Index remains the same */
          /* *Index       =  *Index; */

        }
        else if (*SampleIndex > hce->bucketQueue[hce->bucketNumber].historySampleIndex)
        {
          /* index becomes next snmp index and refresh the SampleIndex to 1 */
          *SampleIndex = 1;
          rc1 =  rmonMapNextGet (historyControlMap, *Index, &next_snmp_index);
          if (rc1 != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
          *Index = next_snmp_index;

        }

      }
      else
      {
        ehe = &(hce->bucketQueue[BucketNumber]);

        if ((hce->historyControlIndex==(*Index)) &&  (ehe ->historyIndex == *Index) &&
            (ehe->validEtherHistoryEntry == L7_TRUE) && 
            (BucketNumber <= hce->bucketsGranted))
        {

          rc = L7_SUCCESS;
          break; /* come out of while loop */
        }

      }
#endif
    }
    else
    {
      /* index becomes next snmp index and refresh the SampleIndex to 1 */
      *SampleIndex = 1;
      rc1 =  rmonMapNextGet (historyControlMap, *Index, &next_snmp_index);
      if (rc1 != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      *Index = next_snmp_index;

    }
  } /* end of while loop */

  return rc;

}

/*********************************************************************
*
* @purpose    Obtains the historySampleIndex member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
/**********************
L7_RC_t
rmonEtherHistorySampleIndexGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;

  hce = &rmonHistoryControlEntryArray[Index];
  ehe = &(hce->bucketQueue[SampleIndex]);
  if(hce != L7_NULLPTR && ehe != L7_NULLPTR)
  {
    *val = ehe->historySampleIndex;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
 
}
********************/


/*********************************************************************
*
* @purpose    Obtains the IntervalStart member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryIntervalStartGet( L7_uint32 Index, L7_uint32 SampleIndex, L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = (ehe->intervalStart)*100; /* Time-ticks are measured as 100th of a second */
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Obtains the no. of dropEvents member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryDropEventsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                               L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->dropEvents;
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Obtains the Octets member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryOctetsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                           L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->octets;
  return L7_SUCCESS;

}
/*********************************************************************
*
* @purpose    Obtains the Pkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryPktsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                         L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);


  *val = ehe->pkts;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the BroadcastPkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryBroadcastPktsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                                  L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];

  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);


  *val = ehe->broadcastPkts;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the MulticastPkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryMulticastPktsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                                  L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->multicastPkts;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the CRCAlignErrors member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryCRCAlignErrorsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                                   L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];



  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->CRCAlignErrors;
  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose    Obtains the UndersizePkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryUndersizePktsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                                  L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->undersizePkts;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the OversizePkts member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryOversizePktsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                                 L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);


  *val = ehe->oversizePkts;
  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose    Obtains the Fragments member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryFragmentsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                              L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->fragments;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the Jabbers member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryJabbersGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                            L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->jabbers;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the Collisions member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             intIfNum      internal interface no. of that interface  
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryCollisionsGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                               L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);

  *val = ehe->collisions;
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose    Obtains the Utilization member associated with the given indicies.
*
* @param      Index         index of HistoryControl to set
*             sampleIndex   sampleIndex of EtherHistory entry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherHistoryUtilizationGet( L7_uint32 Index, L7_uint32 SampleIndex, 
                                L7_uint32 *val)
{
  rmonHistoryControlEntry_t* hce;
  rmonEtherHistoryEntry_t*   ehe;
  L7_uint32 BucketNumber;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (historyControlMap, Index, &array_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  hce = &rmonHistoryControlEntryArray[array_index];


  if (rmonHistoryControlIsBucketAvailableCheck(hce, SampleIndex, &BucketNumber)== L7_FAILURE)
    return L7_FAILURE;

  ehe = &(hce->bucketQueue[BucketNumber]);


  *val = ehe->utilization;
  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose  Callback function for timer to perform polling activities for RMON History group
*           to fill EtherHistory buckets
*
* @param    void
*
* @returns  L7_RC_t
 
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t 
rmonHistoryTimerCallback()
{

  L7_RC_t rc;
  L7_uint32 next_snmp_index;
  L7_uint32 lowest_snmp_index;
  L7_uint32 highest_snmp_index;
  L7_uint32 historyControlIndex;
  L7_uint32 array_index;

/*  L7_uint32 countBucket  = 1;*/
  rmonHistoryControlEntry_t* hce;


  rc = rmonMapLowestGet(historyControlMap, &lowest_snmp_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  historyControlIndex = lowest_snmp_index;

  rc = rmonMapHighestGet(historyControlMap, &highest_snmp_index);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  while (historyControlIndex <= highest_snmp_index)
  {

    rc = rmonMapMatch (historyControlMap, historyControlIndex, &array_index);
    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    hce = &rmonHistoryControlEntryArray[array_index];

    /* increase the timer counter by one */
    hce->timerHistory++;

    /* if the timer-counter is greater or equal to the control interval, fill the buckets */
    if (hce->timerHistory >= hce->interval)
    {
      hce->timerHistory = 0;

      if (hce->status ==  RMON_HISTORY_CONTROL_STATUS_VALID)
      {

        hce->lastSampleIndex++;

        /* find which bucket to place this sample into */
        hce->bucketNumber = (hce->lastSampleIndex % hce->bucketsGranted);


#if 0
        /* if the bucketNumber is more than bucketsGranted, fill the first bucket with */
        /* the entry of second one, fill the second bucket with the entry of third one */
        /* and so on, for the last bucket in the bucketQueue,                          */
        /* call rmonEtherHistoryEntryFill(hce,hce->bucketNumber) to fill it            */ 

        if (hce->bucketNumber > hce->bucketsGranted)
        {
          for (countBucket = 1; countBucket < hce->bucketsGranted;
              countBucket++)
          {
            hce->bucketQueue[countBucket].historySampleIndex =
            hce->bucketQueue[countBucket+1].historySampleIndex;
            hce->bucketQueue[countBucket].intervalStart =
            hce->bucketQueue[countBucket+1].intervalStart;
            hce->bucketQueue[countBucket].dropEvents =
            hce->bucketQueue[countBucket+1].dropEvents;
            hce->bucketQueue[countBucket].octets =
            hce->bucketQueue[countBucket+1].octets;
            hce->bucketQueue[countBucket].pkts =
            hce->bucketQueue[countBucket+1].pkts;
            hce->bucketQueue[countBucket].broadcastPkts =
            hce->bucketQueue[countBucket+1].broadcastPkts;
            hce->bucketQueue[countBucket].multicastPkts =
            hce->bucketQueue[countBucket+1].multicastPkts;
            hce->bucketQueue[countBucket].CRCAlignErrors =
            hce->bucketQueue[countBucket+1].CRCAlignErrors;
            hce->bucketQueue[countBucket].undersizePkts =
            hce->bucketQueue[countBucket+1].undersizePkts;
            hce->bucketQueue[countBucket].oversizePkts =
            hce->bucketQueue[countBucket+1].oversizePkts;
            hce->bucketQueue[countBucket].fragments =
            hce->bucketQueue[countBucket+1].fragments;
            hce->bucketQueue[countBucket].jabbers =
            hce->bucketQueue[countBucket+1].jabbers;
            hce->bucketQueue[countBucket].collisions =
            hce->bucketQueue[countBucket+1].collisions;
            hce->bucketQueue[countBucket].utilization =
            hce->bucketQueue[countBucket+1].utilization;
          }

          hce->bucketNumber = hce->bucketsGranted;
        }
#endif
        /* Fill the etherHistory buckets of bucketQueue */
        if (rmonEtherHistoryEntryFill(hce,hce->bucketNumber) != L7_SUCCESS)
          continue;

        /* Fill the baseline bucket for that HistoryControl Entry */
        if (rmonHistoryControlBaselineEntryFill(hce) != L7_SUCCESS)
          continue;

      }
    }

    rc =  rmonMapNextGet (historyControlMap, historyControlIndex, &next_snmp_index);
    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    historyControlIndex = next_snmp_index;

  }/* end of while loop */

  return L7_SUCCESS;
}
#if 0
/*********************************************************************
*
* @purpose  Gets the max table size after next reset.
*
* @returns  L7_RC_t

* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rmonHistoryResetTablesizeGet(L7_uint32 *val)
{
  if (val == L7_NULL)
  {
    return L7_FAILURE;
  }

  *val = historyResetTableSize;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the current max table size.
*
* @returns  L7_RC_t

* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rmonHistoryCurrentTablesizeGet(L7_uint32 *val)
{
  if (val == L7_NULL)
  {
    return L7_FAILURE;
  }

  *val = historyCurrentTableSize;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the current max table size.
*
* @param    void
*
* @returns  L7_RC_t

* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rmonHistoryTablesizeSet(L7_uint32 val)
{
  if ((val < L7_RMON_MIN_HISTORY_TABLESIZE_VALUE) || (val > L7_RMON_MAX_HISTORY_TABLESIZE_VALUE))
  {
    return L7_FAILURE;
  }

  historyResetTableSize = val;

  return L7_SUCCESS;
}

#endif
