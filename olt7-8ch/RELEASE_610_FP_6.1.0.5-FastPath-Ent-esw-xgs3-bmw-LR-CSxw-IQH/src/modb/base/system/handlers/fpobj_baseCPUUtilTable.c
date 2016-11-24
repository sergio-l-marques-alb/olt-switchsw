
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseCPUUtilTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  01 October 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseCPUUtilTable_obj.h"
#include "usmdb_switch_cpu_api.h"



/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_TaskId
*
* @purpose Get 'TaskId'
 *@description  [TaskId] This is the ID of a task running on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_TaskId(void *wap, void *bufp)
{

  fpObjWa_t kwataskIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTaskIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objtaskId;

   FPOBJ_TRACE_ENTER (bufp);
  
   /* retrieve key: Task Index */
  kwataskIndex.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                                   (xLibU8_t *) & keyTaskIndexValue, &kwataskIndex.len);
  if (kwataskIndex.rc != XLIBRC_SUCCESS)
  {
    kwataskIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwataskIndex);
    return kwataskIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTaskIndexValue, kwataskIndex.len);

  /* get the task ID */
  owa.l7rc = usmDbTaskIdGet(L7_UNIT_CURRENT,keyTaskIndexValue,objtaskId, sizeof(xLibStr256_t));
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: taskId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtaskId,
                           sizeof(objtaskId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_TaskIndex
*
* @purpose Get 'Task Index'
 *@description  Gets presently available Task Indeces.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_TaskIndex(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTaskIndexValue;
  usmDbProcessEntry_t taskEntry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Task Index */
  owa.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                          (xLibU8_t *) & objTaskIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	objTaskIndexValue =0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTaskIndexValue, owa.len);
	objTaskIndexValue++;
  }

  owa.l7rc = usmDbSwitchCpuProcessEntryGet(L7_UNIT_CURRENT,objTaskIndexValue,&taskEntry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objTaskIndexValue, owa.len);

  /* return the object value: Task Index */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTaskIndexValue,
                           sizeof (objTaskIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_TaskName
*
* @purpose Get 'TaskName'
 *@description  [TaskName] This is the name of a task running on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_TaskName(void *wap, void *bufp)
{

  fpObjWa_t kwataskIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTaskIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtaskName;
  usmDbProcessEntry_t taskEntry;

   FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Task Index */
  kwataskIndex.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                                   (xLibU8_t *) & keyTaskIndexValue, &kwataskIndex.len);
  if (kwataskIndex.rc != XLIBRC_SUCCESS)
  {
    kwataskIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwataskIndex);
    return kwataskIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTaskIndexValue, kwataskIndex.len);
  
  memset((char *)&taskEntry,0,sizeof(taskEntry));
  /* get the task entry */
  owa.l7rc = usmDbSwitchCpuProcessEntryGet(L7_UNIT_CURRENT,keyTaskIndexValue,&taskEntry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiStrncpy(objtaskName, taskEntry.taskName, sizeof(xLibStr256_t));
  /* return the object value: taskName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtaskName,
                           strlen (objtaskName));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_Utilization
*
* @purpose Get 'Utilization'
 *@description  [Utilization] This is the CPU time taken by the given task in
* terms of percentage   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_Utilization(void *wap, void *bufp)
{

  fpObjWa_t kwataskIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTaskIndexValue;

  fpObjWa_t kwaUtilStr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtaskUtilization;
  usmDbProcessEntry_t taskEntry;

   FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Task Index */
  kwataskIndex.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                                   (xLibU8_t *) & keyTaskIndexValue, &kwataskIndex.len);
  if (kwataskIndex.rc != XLIBRC_SUCCESS)
  {
    kwataskIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwataskIndex);
    return kwataskIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTaskIndexValue, kwataskIndex.len);

 /* Now check if the value is present in the filter */
  /* retrieve key: Utilization */
    memset((char *)&taskEntry,0,sizeof(taskEntry));
  kwaUtilStr.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_Utilization,
                          (xLibU8_t *) objtaskUtilization, &kwaUtilStr.len);
  if (kwaUtilStr.rc != XLIBRC_SUCCESS)
  {
	  memset((char *)&taskEntry,0,sizeof(taskEntry));
	  /* get the Task Entry */
	  kwaUtilStr.l7rc = usmDbSwitchCpuProcessEntryGet(L7_UNIT_CURRENT,keyTaskIndexValue,&taskEntry);
	  if (kwaUtilStr.l7rc != L7_SUCCESS)
	  {
	    kwaUtilStr.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	    FPOBJ_TRACE_EXIT (bufp, kwaUtilStr);
	    return kwaUtilStr.rc;
	  }
  }
  else
  {
    kwaUtilStr.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, kwaUtilStr);
    return kwaUtilStr.rc;
  }

  osapiStrncpy(objtaskUtilization, taskEntry.taskPercentString, sizeof(xLibStr256_t));
  /* return the object value: Utilization */
  kwaUtilStr.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtaskUtilization,
                           strlen (objtaskUtilization));
  FPOBJ_TRACE_EXIT (bufp, kwaUtilStr);
  return kwaUtilStr.rc;

}


/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_UtilizationFor5seconds
*
* @purpose Get 'Utilization'
 *@description  [Utilization] This is the CPU time taken by the given task in
* terms of percentage   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_UtilizationFor5seconds(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUtilizationFor5seconds;

  fpObjWa_t kwataskIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTaskIndexValue;

  fpObjWa_t kwaUtilStr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtaskUtilization;

   FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Task Index */
  kwataskIndex.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                                   (xLibU8_t *) & keyTaskIndexValue, &kwataskIndex.len);
  if (kwataskIndex.rc != XLIBRC_SUCCESS)
  {
    kwataskIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwataskIndex);
    return kwataskIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTaskIndexValue, kwataskIndex.len);

  /* retrieve key: Utilization */
  memset(objtaskUtilization, 0x00, sizeof(objtaskUtilization));
  kwaUtilStr.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_Utilization,
                                   (xLibU8_t *)  objtaskUtilization, &kwaUtilStr.len);
  if (kwaUtilStr.rc != XLIBRC_SUCCESS)
  {
    kwaUtilStr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUtilStr);
    return kwaUtilStr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objtaskUtilization, kwaUtilStr.len);

  memset(objUtilizationFor5seconds, 0x00, sizeof(objUtilizationFor5seconds));
  /* Parse the objtaskUtilization and retrieve the value */
  osapiStrncpySafe(objUtilizationFor5seconds, objtaskUtilization+14,6);
  if(objUtilizationFor5seconds[0]== ' ' )
  {
    objUtilizationFor5seconds[0]='0';
  }
  /*end of Parsing */

   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Utilization */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUtilizationFor5seconds,
                           strlen (objUtilizationFor5seconds));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_UtilizationFor1Minute
*
* @purpose Get 'Utilization'
 *@description  [Utilization] This is the CPU time taken by the given task in
* terms of percentage   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_UtilizationFor1Minute(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUtilizationFor1Minute;

  fpObjWa_t kwataskIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTaskIndexValue;

  fpObjWa_t kwaUtilStr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtaskUtilization;

   FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Task Index */
  kwataskIndex.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                                   (xLibU8_t *) & keyTaskIndexValue, &kwataskIndex.len);
  if (kwataskIndex.rc != XLIBRC_SUCCESS)
  {
    kwataskIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwataskIndex);
    return kwataskIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTaskIndexValue, kwataskIndex.len);

  /* retrieve key: Utilization */
  memset(objtaskUtilization, 0x00, sizeof(objtaskUtilization));
  kwaUtilStr.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_Utilization,
                                   (xLibU8_t *)  objtaskUtilization, &kwaUtilStr.len);
  if (kwaUtilStr.rc != XLIBRC_SUCCESS)
  {
    kwaUtilStr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUtilStr);
    return kwaUtilStr.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, objtaskUtilization, kwaUtilStr.len);

  memset(objUtilizationFor1Minute, 0x00, sizeof(objUtilizationFor1Minute));
  /* Parse the objtaskUtilization and retrieve the value */
  osapiStrncpySafe(objUtilizationFor1Minute, objtaskUtilization+36, 6);
  if(objUtilizationFor1Minute[0]== ' ' )
  {
    objUtilizationFor1Minute[0]='0';
  }

  /*end of Parsing */

   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Utilization */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUtilizationFor1Minute,
                           strlen (objUtilizationFor1Minute));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_UtilizationFor5Minute
*
* @purpose Get 'Utilization'
 *@description  [Utilization] This is the CPU time taken by the given task in
* terms of percentage   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_UtilizationFor5Minutes(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUtilizationFor5Minutes;

  fpObjWa_t kwataskIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTaskIndexValue;

  fpObjWa_t kwaUtilStr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtaskUtilization;

   FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Task Index */
  kwataskIndex.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_TaskIndex,
                                   (xLibU8_t *) & keyTaskIndexValue, &kwataskIndex.len);
  if (kwataskIndex.rc != XLIBRC_SUCCESS)
  {
    kwataskIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwataskIndex);
    return kwataskIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTaskIndexValue, kwataskIndex.len);

  /* retrieve key: Utilization */
  memset(objtaskUtilization, 0x00, sizeof(objtaskUtilization));
  kwaUtilStr.rc = xLibFilterGet (wap, XOBJ_baseCPUUtilTable_Utilization,
                                   (xLibU8_t *)  objtaskUtilization, &kwaUtilStr.len);
  if (kwaUtilStr.rc != XLIBRC_SUCCESS)
  {
    kwaUtilStr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUtilStr);
    return kwaUtilStr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objtaskUtilization, kwaUtilStr.len);

  memset(objUtilizationFor5Minutes, 0x00, sizeof(objUtilizationFor5Minutes));
  /* Parse the objtaskUtilization and retrieve the value */
  osapiStrncpySafe(objUtilizationFor5Minutes, objtaskUtilization+59, 6);
  if(objUtilizationFor5Minutes[0]== ' ' )
  {
    objUtilizationFor5Minutes[0]='0';
  }

  /*end of Parsing */

   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Utilization */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUtilizationFor5Minutes,
                           strlen (objUtilizationFor5Minutes));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCPUUtilTable_TotalUtilization
*
* @purpose Get 'Total CPU Utilization'
 *@description  [Utilization] This is the Total CPU Utilization
* terms of percentage   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCPUUtilTable_TotalUtilization(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTotalCpuUtilization;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the Total CPU Utilization */
  owa.l7rc = usmDbTotalCpuUtilGet(L7_UNIT_CURRENT,objTotalCpuUtilization,sizeof(xLibStr256_t));
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Utilization */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTotalCpuUtilization,
                           strlen (objTotalCpuUtilization));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
