
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basesFlowCpTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  05 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basesFlowCpTable_obj.h"
#include "usmdb_sflow.h"
/*******************************************************************************
* @function fpObjList_basesFlowCpTable_CpDataSource
*
* @purpose List 'CpDataSource'
 *@description  [CpDataSource] Lists the Data Source
* poller.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_basesFlowCpTable_CpDataSource (void *wap, void *bufp)
{

  xLibStr256_t objCpDataSourceValue;
  xLibStr256_t nextObjCpDataSourceValue;
  xLibU32_t nextObjCpInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (objCpDataSourceValue, 0, sizeof (objCpDataSourceValue));
  memset (&nextObjCpInstanceValue, 0, sizeof (nextObjCpInstanceValue));
  /* retrieve key: CpDataSource */
  owa.len = sizeof (xLibStr256_t);
  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                          (xLibU8_t *) objCpDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (nextObjCpDataSourceValue, 0, sizeof (nextObjCpDataSourceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objCpDataSourceValue, owa.len);
    memcpy(nextObjCpDataSourceValue, objCpDataSourceValue, sizeof(objCpDataSourceValue));
    nextObjCpInstanceValue = FD_SFLOW_INSTANCE;
  }
  owa.l7rc = usmdbsFlowCpEntryGetNext (L7_UNIT_CURRENT,
                                       nextObjCpDataSourceValue,
                                       &nextObjCpInstanceValue);
  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjCpDataSourceValue, owa.len);

  /* return the object value: CpDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjCpDataSourceValue,
                           sizeof(nextObjCpDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowCpTable_CpDataSource
*
* @purpose Get 'CpDataSource'
 *@description  [CpDataSource] Identifies the source of the data for the counter
* poller.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowCpTable_CpDataSource (void *wap, void *bufp)
{

  xLibStr256_t nextObjCpDataSourceValue;
  xLibU32_t nextObjCpInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t intIfIndex;
  xLibU32_t receiverValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CpDataSource */
  memset (nextObjCpDataSourceValue, 0, sizeof (nextObjCpDataSourceValue));
  memset (&nextObjCpInstanceValue, 0, sizeof (nextObjCpInstanceValue));

  /* retrieve key: CpDataSource */
  owa.len = sizeof(nextObjCpDataSourceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                          (xLibU8_t *) nextObjCpDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (nextObjCpDataSourceValue, 0, sizeof (nextObjCpDataSourceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, nextObjCpDataSourceValue, owa.len);
  }


  do
  {
					 
    owa.l7rc = usmdbsFlowCpEntryGetNext (L7_UNIT_CURRENT,
                                         nextObjCpDataSourceValue,
                                         &nextObjCpInstanceValue);

    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    } 

    
    if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, nextObjCpDataSourceValue, &intIfIndex)) == L7_SUCCESS)
    {
      owa.l7rc = usmdbsFlowCpReceiverGet( L7_UNIT_CURRENT, intIfIndex, nextObjCpInstanceValue, &receiverValue);
    }   
  }while(owa.l7rc != L7_SUCCESS);

  
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjCpDataSourceValue, owa.len);

  /* return the object value: CpDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjCpDataSourceValue,
                          sizeof(nextObjCpDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}





/*******************************************************************************
* @function fpObjGet_basesFlowCpTable_NewCpDataSource
*
* @purpose Get 'NewCpDataSource'
 *@description  [CpDataSource] Identifies the source of the data for the counter
* poller.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowCpTable_NewCpDataSource (void *wap, void *bufp)
{

  xLibStr256_t nextObjCpDataSourceValue;
  xLibU32_t nextObjCpInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CpDataSource */
  memset (nextObjCpDataSourceValue, 0, sizeof (nextObjCpDataSourceValue));
  memset (&nextObjCpInstanceValue, 0, sizeof (nextObjCpInstanceValue));

  /* retrieve key: CpDataSource */
  owa.len = sizeof(nextObjCpDataSourceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_NewCpDataSource,
                          (xLibU8_t *) nextObjCpDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (nextObjCpDataSourceValue, 0, sizeof (nextObjCpDataSourceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, nextObjCpDataSourceValue, owa.len);
  }

    owa.l7rc = usmdbsFlowCpEntryGetNext (L7_UNIT_CURRENT,
                                         nextObjCpDataSourceValue,
                                         &nextObjCpInstanceValue);

    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }


  
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjCpDataSourceValue, owa.len);

  /* return the object value: CpDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjCpDataSourceValue,
                          sizeof(nextObjCpDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}











xLibRC_t fpObjGet_basesFlowCpTable_CpInstance (void *wap, void *bufp)
{

  xLibStr256_t objCpDataSourceValue;
  xLibStr256_t nextObjCpDataSourceValue;
  xLibU32_t objCpInstanceValue = 0;
  xLibU32_t nextObjCpInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CpDataSource */
  memset (nextObjCpDataSourceValue, 0, sizeof (nextObjCpDataSourceValue));
  memset (objCpDataSourceValue, 0, sizeof (objCpDataSourceValue));
  owa.len = sizeof (objCpDataSourceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                          (xLibU8_t *) objCpDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objCpDataSourceValue, owa.len);

  /* retrieve key: CpInstance */
  owa.len = sizeof(objCpInstanceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpInstance,
                          (xLibU8_t *) & objCpInstanceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memcpy (nextObjCpDataSourceValue, objCpDataSourceValue, sizeof (nextObjCpDataSourceValue));
    memset (&nextObjCpInstanceValue, 0, sizeof (nextObjCpInstanceValue));
     nextObjCpInstanceValue = FD_SFLOW_INSTANCE;
    /* right now we are assuming Instance vales as always 1 */
    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCpInstanceValue, owa.len);
    memcpy (nextObjCpDataSourceValue, objCpDataSourceValue, sizeof (nextObjCpDataSourceValue));
    nextObjCpInstanceValue = objCpInstanceValue; 
    owa.l7rc = usmdbsFlowCpEntryGetNext (L7_UNIT_CURRENT,
                                                             nextObjCpDataSourceValue,
                                                             &nextObjCpInstanceValue);

  }

  if ((nextObjCpInstanceValue == objCpInstanceValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCpInstanceValue, owa.len);

  /* return the object value: CpInstance */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCpInstanceValue,
                           sizeof (nextObjCpInstanceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowCpTable_CpReceiver
*
* @purpose Get 'CpReceiver'
 *@description  [CpReceiver] The SFlowReciever associated with this counter
* poller.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowCpTable_CpReceiver (void *wap, void *bufp)
{

  fpObjWa_t kwaCpDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyCpDataSourceValue;
  xLibU32_t keyCpInstanceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCpReceiverValue;
  xLibU32_t intIfIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CpDataSource */
  kwaCpDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                                      (xLibU8_t *) keyCpDataSourceValue, &kwaCpDataSource.len);
  if (kwaCpDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaCpDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCpDataSource);
    return kwaCpDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCpDataSourceValue, kwaCpDataSource.len);
  
  keyCpInstanceValue = FD_SFLOW_INSTANCE;
  /* get the value from application */
  if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyCpDataSourceValue, &intIfIndex)) == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowCpReceiverGet( L7_UNIT_CURRENT, intIfIndex, keyCpInstanceValue, &objCpReceiverValue);
    if ( owa.l7rc == L7_FAILURE)
    {
      objCpReceiverValue = L7_NULL;
      owa.l7rc = L7_SUCCESS;
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CpReceiver */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCpReceiverValue, sizeof (objCpReceiverValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowCpTable_CpReceiver
*
* @purpose Set 'CpReceiver'
 *@description  [CpReceiver] The SFlowReciever associated with this counter
* poller.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowCpTable_CpReceiver (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCpReceiverValue;
  fpObjWa_t kwaCpDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyCpDataSourceValue;
  xLibU32_t keyCpInstanceValue;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CpReceiver */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCpReceiverValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCpReceiverValue, owa.len);

  /* retrieve key: CpDataSource */
  kwaCpDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                                      (xLibU8_t *) keyCpDataSourceValue, &kwaCpDataSource.len);
  if (kwaCpDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaCpDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCpDataSource);
    return kwaCpDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCpDataSourceValue, kwaCpDataSource.len);
  /*support only one instance this is done becoz this field(key) will be hidden in web
  also .For this kind of Things we need to setdefault value to the field.*/

  keyCpInstanceValue = FD_SFLOW_INSTANCE;
  /* set the value in application */
  owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyCpDataSourceValue, &intIfIndex);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowCpReceiverSet( L7_UNIT_CURRENT, intIfIndex, keyCpInstanceValue, objCpReceiverValue);
  }
  if(owa.l7rc == L7_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }
  else if (owa.l7rc == L7_ALREADY_CONFIGURED)
  {
    owa.rc = XLIBRC_ALREADY_EXSITS;    /* TODO: Change if required */
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowCpTable_CpInterval
*
* @purpose Get 'CpInterval'
 *@description  [CpInterval] The maximum number of seconds between successive
* samples of the counters associated with this data source.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowCpTable_CpInterval (void *wap, void *bufp)
{

  fpObjWa_t kwaCpDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyCpDataSourceValue;
  xLibU32_t keyCpInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCpIntervalValue;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CpDataSource */
  kwaCpDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                                      (xLibU8_t *) keyCpDataSourceValue, &kwaCpDataSource.len);
  if (kwaCpDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaCpDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCpDataSource);
    return kwaCpDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCpDataSourceValue, kwaCpDataSource.len);
  
  keyCpInstanceValue = FD_SFLOW_INSTANCE;
  /* get the value from application */
  if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyCpDataSourceValue, &intIfIndex)) == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowCpIntervalGet( L7_UNIT_CURRENT, intIfIndex, keyCpInstanceValue, &objCpIntervalValue);
    if ( owa.l7rc == L7_FAILURE)
    {
      objCpIntervalValue = FD_SFLOW_POLL_INTERVAL;
      owa.l7rc = L7_SUCCESS;
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CpInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCpIntervalValue, sizeof (objCpIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowCpTable_CpInterval
*
* @purpose Set 'CpInterval'
 *@description  [CpInterval] The maximum number of seconds between successive
* samples of the counters associated with this data source.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowCpTable_CpInterval (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCpIntervalValue;
  fpObjWa_t kwaCpDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyCpDataSourceValue;
  xLibU32_t keyCpInstanceValue;
  xLibU32_t rcvr=L7_NULL;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CpInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCpIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCpIntervalValue, owa.len);

  /* retrieve key: CpDataSource */
  kwaCpDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowCpTable_CpDataSource,
                                      (xLibU8_t *) keyCpDataSourceValue, &kwaCpDataSource.len);
  if (kwaCpDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaCpDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCpDataSource);
    return kwaCpDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCpDataSourceValue, kwaCpDataSource.len);

  keyCpInstanceValue = FD_SFLOW_INSTANCE;

  /* set the value in application */
  owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyCpDataSourceValue, &intIfIndex);

  if (owa.l7rc == L7_SUCCESS)
  {
    if(usmdbsFlowCpReceiverGet( L7_UNIT_CURRENT, intIfIndex, keyCpInstanceValue, &rcvr) == L7_SUCCESS)
    {
      if(rcvr != L7_NULL)
      {
        owa.l7rc = usmdbsFlowCpIntervalSet( L7_UNIT_CURRENT, intIfIndex, keyCpInstanceValue, objCpIntervalValue);
      }
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
