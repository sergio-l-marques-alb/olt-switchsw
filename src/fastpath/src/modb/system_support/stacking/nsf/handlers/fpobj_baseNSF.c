/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseNSF.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to nsf-object.xml
*
* @create  11 June 2009
*
* @author  Colin Verne
* @end
*
********************************************************************************/
#include "flex.h"
#include "fpobj_util.h"
#include "_xe_baseStacking_obj.h"
#include "_xe_baseNSF_obj.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_ckpt.h"
#include "osapi.h"
#include "strlib_common_common.h"
#ifdef L7_OSPF_PACKAGE
#include "usmdb_ospf_api.h"
#endif
#include "unitmgr_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfUnitSupport
*
* @purpose Get 'NsfUnitSupport'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseStacking_NsfUnitSupport (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyStackMemberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfUnitSupportValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseStacking_StackMember,
                          (xLibU8_t *) & keyStackMemberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyStackMemberValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbUnitMgrNsfSupportGet (keyStackMemberValue, &objNsfUnitSupportValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfUnitSupportValue, sizeof (objNsfUnitSupportValue));

  /* return the object value: NsfUnitSupport */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfUnitSupportValue,
                           sizeof (objNsfUnitSupportValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfAdminStatus
*
* @purpose Get 'NsfAdminStatus'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfAdminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfAdminStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objNsfAdminStatusValue = usmDbUnitMgrNsfAdminGet();

  FPOBJ_TRACE_VALUE (bufp, &objNsfAdminStatusValue, sizeof (objNsfAdminStatusValue));

  /* return the object value: NsfAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfAdminStatusValue,
                           sizeof (objNsfAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_NsfAdminStatus
*
* @purpose Set 'NsfAdminStatus'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_NsfAdminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfAdminStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NsfAdminStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNsfAdminStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfAdminStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbUnitMgrNsfAdminSet (objNsfAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOperStatus
*
* @purpose Get 'NsfOperStatus'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOperStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOperStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objNsfOperStatusValue = usmDbUnitMgrNsfOperStatusGet ();
  FPOBJ_TRACE_VALUE (bufp, &objNsfOperStatusValue, sizeof (objNsfOperStatusValue));

  /* return the object value: NsfOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfOperStatusValue,
                           sizeof (objNsfOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfLastStartupReason
*
* @purpose Get 'NsfLastStartupReason'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfLastStartupReason (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfLastStartupReasonValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objNsfLastStartupReasonValue = usmDbUnitMgrStartupReasonGet ();
  FPOBJ_TRACE_VALUE (bufp, &objNsfLastStartupReasonValue, sizeof (objNsfLastStartupReasonValue));

  /* return the object value: NsfLastStartupReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfLastStartupReasonValue,
                           sizeof (objNsfLastStartupReasonValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfTimeSinceLastRestart
*
* @purpose Get 'NsfTimeSinceLastRestart'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfTimeSinceLastRestart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfTimeSinceLastRestartValue;
  xLibStr256_t objTimeValue;
  L7_timespec ts;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objNsfTimeSinceLastRestartValue = usmDbUnitMgrTimeSinceLastRestart ();
  FPOBJ_TRACE_VALUE (bufp, &objNsfTimeSinceLastRestartValue, sizeof (objNsfTimeSinceLastRestartValue));

  osapiConvertRawUpTime(objNsfTimeSinceLastRestartValue, &ts);
  sprintf(objTimeValue, pStrInfo_common_DaysHrsMinsSecs, ts.days, 
                  ts.hours, ts.minutes, ts.seconds);

  /* return the object value: NsfTimeSinceLastRestart */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeValue,
                           sizeof (objTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfRestartInProgress
*
* @purpose Get 'NsfRestartInProgress'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfRestartInProgress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfRestartInProgressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objNsfRestartInProgressValue = usmDbUnitMgrRestartInProgress ();
  FPOBJ_TRACE_VALUE (bufp, &objNsfRestartInProgressValue, sizeof (objNsfRestartInProgressValue));

  /* return the object value: NsfRestartInProgress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfRestartInProgressValue,
                           sizeof (objNsfRestartInProgressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfWarmRestartReady
*
* @purpose Get 'NsfWarmRestartReady'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfWarmRestartReady (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfWarmRestartReadyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objNsfWarmRestartReadyValue = usmDbCkptWarmRestartReady ();
  FPOBJ_TRACE_VALUE (bufp, &objNsfWarmRestartReadyValue, sizeof (objNsfWarmRestartReadyValue));

  /* return the object value: NsfWarmRestartReady */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfWarmRestartReadyValue,
                           sizeof (objNsfWarmRestartReadyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfBackupConfigurationAge
*
* @purpose Get 'NsfBackupConfigurationAge'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfBackupConfigurationAge (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfBackupConfigurationAgeValue;
  xLibStr256_t objTimeValue;
  L7_timespec ts;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if (usmDbUnitMgrConfigAgeOnBackup(&objNsfBackupConfigurationAgeValue) == L7_SUCCESS)
  {
      FPOBJ_TRACE_VALUE (bufp, &objNsfBackupConfigurationAgeValue, sizeof (objNsfBackupConfigurationAgeValue));
    
      osapiConvertRawUpTime(objNsfBackupConfigurationAgeValue, &ts);
      sprintf(objTimeValue, pStrInfo_common_DaysHrsMinsSecs, ts.days, 
                      ts.hours, ts.minutes, ts.seconds);
  }
  else
  {
      sprintf(objTimeValue, "Not yet copied");
  }

  /* return the object value: NsfBackupConfigurationAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeValue,
                           sizeof (objTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfBackupConfigurationStatus
*
* @purpose Get 'NsfBackupConfigurationStatus'
*
* @description [NsfBackupConfigurationStatus]:Status of copying running congfig 
*                                             to backup unit.
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfBackupConfigurationStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_RUN_CFG_STATUS_t runCfgStatus;
  xLibStr256_t objNsfBackupConfigurationStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  runCfgStatus = usmDbUnitMgrRunCfgStatusGet();
  owa.l7rc = usmDbUnitMgrRunCfgStatusString(runCfgStatus, objNsfBackupConfigurationStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfBackupConfigurationStatus, sizeof (objNsfBackupConfigurationStatus));

  /* return the object value: NsfBackupConfigurationAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfBackupConfigurationStatus,
                           sizeof (objNsfBackupConfigurationStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_NsfInitiateFailover
*
* @purpose Set 'NsfInitiateFailover'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_NsfInitiateFailover (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfInitiateFailoverValue;
  L7_uint32 sfs_status;
  L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NsfInitiateFailover */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNsfInitiateFailoverValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfInitiateFailoverValue, owa.len);
  
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_STACKING_COMPONENT_ID,
                               L7_STACKING_SFS_FEATURE_ID) == L7_TRUE)
  {
    rc = usmDbUnitMgrStackFirmwareSyncStatusGet(&sfs_status);
    if(rc == L7_SUCCESS && sfs_status == L7_SFS_SWITCH_STATUS_IN_PROGRESS)
    {
      owa.rc = XLIBRC_SFS_SWITCH_STATUS_IN_PROGRESS;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  
  /* set the value in application */
  owa.l7rc = usmDbFailoverInitiate ();
  if (owa.l7rc != L7_SUCCESS)
  {
    switch (owa.l7rc)
    {
    case L7_REQUEST_DENIED:
      owa.rc = XLIBRC_NSF_INITIATE_FAILOVER_DENIED;
      break;
    default:
      owa.rc = XLIBRC_NSF_INITIATE_FAILOVER_FAILED;
      break;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_CheckpointClearStatistics
*
* @purpose Set 'CheckpointClearStatistics'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_CheckpointClearStatistics (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCheckpointClearStatisticsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CheckpointClearStatistics */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCheckpointClearStatisticsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCheckpointClearStatisticsValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCkptStatsClear ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_CheckpointMessages
*
* @purpose Get 'CheckpointMessages'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_CheckpointMessages (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  CkptStats_t ckptStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCkptStatsGet(&ckptStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ckptStats.numMsgs, sizeof (ckptStats.numMsgs));

  /* return the object value: CheckpointMessages */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ckptStats.numMsgs,
                           sizeof (ckptStats.numMsgs));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_CheckpointBytes
*
* @purpose Get 'CheckpointBytes'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_CheckpointBytes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  CkptStats_t ckptStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCkptStatsGet(&ckptStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ckptStats.numBytes, sizeof (ckptStats.numBytes));

  /* return the object value: CheckpointBytes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ckptStats.numBytes,
                           sizeof (ckptStats.numBytes));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_CheckpointTimeSinceCountersCleared
*
* @purpose Get 'CheckpointTimeSinceCountersCleared'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_CheckpointTimeSinceCountersCleared (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  CkptStats_t ckptStats;
  xLibU32_t objCheckpointTimeSinceCountersClearedValue;
  xLibStr256_t objTimeValue;
  L7_timespec ts;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCkptStatsGet(&ckptStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCheckpointTimeSinceCountersClearedValue = simSystemUpTimeGet() - ckptStats.timestamp;
  FPOBJ_TRACE_VALUE (bufp, &objCheckpointTimeSinceCountersClearedValue, sizeof (objCheckpointTimeSinceCountersClearedValue));

  osapiConvertRawUpTime(objCheckpointTimeSinceCountersClearedValue, &ts);
  sprintf(objTimeValue, pStrInfo_common_DaysHrsMinsSecs, ts.days, 
                  ts.hours, ts.minutes, ts.seconds);

  /* return the object value: CheckpointTimeSinceCountersCleared */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeValue,
                           sizeof (objTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_CheckpointMessageRate
*
* @purpose Get 'CheckpointMessageRate'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_CheckpointMessageRate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  CkptStats_t ckptStats;
  FPOBJ_TRACE_ENTER (bufp);
  L7_uint32 statsTime;
  xLibStr256_t objCheckpointMessageRate;

  /* get the value from application */
  owa.l7rc = usmDbCkptStatsGet(&ckptStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  statsTime = simSystemUpTimeGet() - ckptStats.timestamp;
  osapiSnprintf(objCheckpointMessageRate, sizeof(objCheckpointMessageRate), "%d.%.3d msg/sec", 
                ckptStats.msgRate / statsTime,
                ((ckptStats.numMsgs % statsTime) * 1000)/statsTime);
  FPOBJ_TRACE_VALUE (bufp, &objCheckpointMessageRate, sizeof (objCheckpointMessageRate));

  /* return the object value: CheckpointMessageRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCheckpointMessageRate,
                           sizeof (objCheckpointMessageRate));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_CheckpointLast10SecMessageRate
*
* @purpose Get 'CheckpointLast10SecMessageRate'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_CheckpointLast10SecMessageRate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  CkptStats_t ckptStats;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objCheckpointLast10SecMessageRate;

  /* get the value from application */
  owa.l7rc = usmDbCkptStatsGet(&ckptStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(objCheckpointLast10SecMessageRate, sizeof(objCheckpointLast10SecMessageRate), "%d.%.1d msg/sec",
                ckptStats.msgRate / CKPT_STATS_COLL_INT,
                ckptStats.msgRate % CKPT_STATS_COLL_INT);
  FPOBJ_TRACE_VALUE (bufp, &objCheckpointLast10SecMessageRate, sizeof (objCheckpointLast10SecMessageRate));

  /* return the object value: CheckpointLast10SecMessageRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCheckpointLast10SecMessageRate,
                           sizeof (objCheckpointLast10SecMessageRate));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_CheckpointHighest10SecMessageRate
*
* @purpose Get 'CheckpointHighest10SecMessageRate'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_CheckpointHighest10SecMessageRate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  CkptStats_t ckptStats;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objCheckpointHighest10SecMessageRate;

  /* get the value from application */
  owa.l7rc = usmDbCkptStatsGet(&ckptStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(objCheckpointHighest10SecMessageRate, sizeof(objCheckpointHighest10SecMessageRate), "%d.%.1d msg/sec",
                ckptStats.msgRateMax / CKPT_STATS_COLL_INT,
                ckptStats.msgRateMax % CKPT_STATS_COLL_INT);
  FPOBJ_TRACE_VALUE (bufp, &objCheckpointHighest10SecMessageRate, sizeof (objCheckpointHighest10SecMessageRate));

  /* return the object value: CheckpointHighest10SecMessageRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCheckpointHighest10SecMessageRate,
                           sizeof (objCheckpointHighest10SecMessageRate));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#ifdef L7_OSPF_PACKAGE
/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfSupportMode
*
* @purpose Get 'NsfOspfSupportMode'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfSupportMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfSupportModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfNsfSupportGet(&objNsfOspfSupportModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfSupportModeValue, sizeof (objNsfOspfSupportModeValue));

  /* return the object value: NsfOspfSupportMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfOspfSupportModeValue,
                           sizeof (objNsfOspfSupportModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_NsfOspfSupportMode
*
* @purpose Set 'NsfOspfSupportMode'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_NsfOspfSupportMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfSupportModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NsfOspfSupportMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNsfOspfSupportModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfSupportModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfNsfSupportSet(objNsfOspfSupportModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfRestartInterval
*
* @purpose Get 'NsfOspfRestartInterval'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfRestartInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfRestartIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfRestartIntervalGet(&objNsfOspfRestartIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfRestartIntervalValue, sizeof (objNsfOspfRestartIntervalValue));

  /* return the object value: NsfOspfRestartInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfOspfRestartIntervalValue,
                           sizeof (objNsfOspfRestartIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_NsfOspfRestartInterval
*
* @purpose Set 'NsfOspfRestartInterval'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_NsfOspfRestartInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfRestartIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NsfOspfRestartInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNsfOspfRestartIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfRestartIntervalValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfRestartIntervalSet (objNsfOspfRestartIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfRestartStatus
*
* @purpose Get 'NsfOspfRestartStatus'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfRestartStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfStatus_t ospfStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfStatusGet (&ospfStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ospfStatus.restartStatus, sizeof (ospfStatus.restartStatus));

  /* return the object value: NsfOspfRestartStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ospfStatus.restartStatus,
                           sizeof (ospfStatus.restartStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfRestartAge
*
* @purpose Get 'NsfOspfRestartAge'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfRestartAge (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfStatus_t ospfStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfStatusGet (&ospfStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ospfStatus.restartAge, sizeof (ospfStatus.restartAge));

  /* return the object value: NsfOspfRestartAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ospfStatus.restartAge,
                           sizeof (ospfStatus.restartAge));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfRestartExitReason
*
* @purpose Get 'NsfOspfRestartExitReason'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfRestartExitReason (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfStatus_t ospfStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfStatusGet (&ospfStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ospfStatus.grExitReason, sizeof (ospfStatus.grExitReason));

  /* return the object value: NsfOspfRestartExitReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ospfStatus.grExitReason,
                           sizeof (ospfStatus.grExitReason));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#ifdef L7_IPV6_PACKAGE
/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfv3SupportMode
*
* @purpose Get 'NsfOspfv3SupportMode'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfv3SupportMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfv3SupportModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfv3NsfSupportGet(&objNsfOspfv3SupportModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfv3SupportModeValue, sizeof (objNsfOspfv3SupportModeValue));

  /* return the object value: NsfOspfv3SupportMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfOspfv3SupportModeValue,
                           sizeof (objNsfOspfv3SupportModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_NsfOspfv3SupportMode
*
* @purpose Set 'NsfOspfv3SupportMode'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_NsfOspfv3SupportMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfv3SupportModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NsfOspfv3SupportMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNsfOspfv3SupportModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfv3SupportModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfv3NsfSupportSet(objNsfOspfv3SupportModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfv3RestartInterval
*
* @purpose Get 'NsfOspfv3RestartInterval'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfv3RestartInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfv3RestartIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfv3RestartIntervalGet(&objNsfOspfv3RestartIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfv3RestartIntervalValue, sizeof (objNsfOspfv3RestartIntervalValue));

  /* return the object value: NsfOspfv3RestartInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNsfOspfv3RestartIntervalValue,
                           sizeof (objNsfOspfv3RestartIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseNSF_NsfOspfv3RestartInterval
*
* @purpose Set 'NsfOspfv3RestartInterval'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseNSF_NsfOspfv3RestartInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNsfOspfv3RestartIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NsfOspfv3RestartInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNsfOspfv3RestartIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNsfOspfv3RestartIntervalValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfv3RestartIntervalSet (objNsfOspfv3RestartIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfv3RestartStatus
*
* @purpose Get 'NsfOspfv3RestartStatus'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfv3RestartStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfStatus_t ospfStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfv3StatusGet (&ospfStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ospfStatus.restartStatus, sizeof (ospfStatus.restartStatus));

  /* return the object value: NsfOspfv3RestartStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ospfStatus.restartStatus,
                           sizeof (ospfStatus.restartStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfv3RestartAge
*
* @purpose Get 'NsfOspfv3RestartAge'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfv3RestartAge (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfStatus_t ospfStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfv3StatusGet (&ospfStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ospfStatus.restartAge, sizeof (ospfStatus.restartAge));

  /* return the object value: NsfOspfv3RestartAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ospfStatus.restartAge,
                           sizeof (ospfStatus.restartAge));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseNSF_NsfOspfv3RestartExitReason
*
* @purpose Get 'NsfOspfv3RestartExitReason'
*
* @description 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseNSF_NsfOspfv3RestartExitReason (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfStatus_t ospfStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfv3StatusGet (&ospfStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &ospfStatus.grExitReason, sizeof (ospfStatus.grExitReason));

  /* return the object value: NsfOspfv3RestartExitReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & ospfStatus.grExitReason,
                           sizeof (ospfStatus.grExitReason));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif /* L7_IPV6_PACKAGE */

#endif /* L7_OSPF_PACKAGE */
