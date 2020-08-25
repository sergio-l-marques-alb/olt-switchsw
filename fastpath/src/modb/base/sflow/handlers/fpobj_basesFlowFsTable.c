
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basesFlowFsTable.c
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
#include "_xe_basesFlowFsTable_obj.h"
#include "usmdb_sflow.h"

/*******************************************************************************
* @function fpObjGet_basesFlowFsTable_FsDataSource
*
* @purpose List 'FsDataSource'
 *@description  [FsDataSource] sFlowDataSource list for this flow sampler.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_basesFlowFsTable_FsDataSource (void *wap, void *bufp)
{

  xLibStr256_t objFsDataSourceValue;
  xLibStr256_t nextObjFsDataSourceValue;
  xLibU32_t nextObjFsInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FsDataSource */
  memset (objFsDataSourceValue, 0, sizeof (objFsDataSourceValue));
  memset (&nextObjFsInstanceValue, 0, sizeof (nextObjFsInstanceValue));
  /* retrieve key: FsDataSource */
  owa.len = sizeof(objFsDataSourceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                          (xLibU8_t *) objFsDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (nextObjFsDataSourceValue, 0, sizeof (nextObjFsDataSourceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objFsDataSourceValue, owa.len);

    memcpy(nextObjFsDataSourceValue, objFsDataSourceValue, sizeof(objFsDataSourceValue));
    nextObjFsInstanceValue = FD_SFLOW_INSTANCE;
  }

  owa.l7rc = usmdbsFlowFsEntryGetNext (L7_UNIT_CURRENT,
                                       nextObjFsDataSourceValue,
                                       &nextObjFsInstanceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjFsDataSourceValue, owa.len);

  /* return the object value: FsDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjFsDataSourceValue,
                          sizeof(nextObjFsDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowFsTable_FsDataSource
*
* @purpose Get 'FsDataSource'
 *@description  [FsDataSource] sFlowDataSource for this flow sampler.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowFsTable_FsDataSource (void *wap, void *bufp)
{
  xLibStr256_t nextObjFsDataSourceValue;
  xLibU32_t nextObjFsInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t intIfIndex;
  xLibU32_t receiverValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FsDataSource */
  memset (nextObjFsDataSourceValue, 0, sizeof (nextObjFsDataSourceValue));
  memset (&nextObjFsInstanceValue, 0, sizeof (nextObjFsInstanceValue));

  /* retrieve key: FsDataSource */
  owa.len = sizeof(nextObjFsDataSourceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                          (xLibU8_t *) nextObjFsDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (nextObjFsDataSourceValue, 0, sizeof (nextObjFsDataSourceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, nextObjFsDataSourceValue, owa.len);
  }

  do
  {
    owa.l7rc = usmdbsFlowFsEntryGetNext (L7_UNIT_CURRENT,
                                         nextObjFsDataSourceValue,
                                         &nextObjFsInstanceValue);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    } 
    if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, nextObjFsDataSourceValue, &intIfIndex)) == L7_SUCCESS)
    {
      owa.l7rc = usmdbsFlowFsReceiverGet( L7_UNIT_CURRENT, intIfIndex, nextObjFsInstanceValue, &receiverValue);
    }   
  }while(owa.l7rc != L7_SUCCESS);

  
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjFsDataSourceValue, owa.len);

  /* return the object value: FsDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjFsDataSourceValue,
                          sizeof(nextObjFsDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
xLibRC_t fpObjGet_basesFlowFsTable_FsInstance (void *wap, void *bufp)
{

  xLibStr256_t objFsDataSourceValue;
  xLibStr256_t nextObjFsDataSourceValue;
  xLibU32_t objFsInstanceValue = 0;
  xLibU32_t nextObjFsInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FsDataSource */
  memset (objFsDataSourceValue, 0, sizeof (objFsDataSourceValue)); 
  memset (nextObjFsDataSourceValue, 0, sizeof (nextObjFsDataSourceValue));
  owa.len = sizeof(objFsDataSourceValue); 
  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                          (xLibU8_t *) objFsDataSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objFsDataSourceValue, owa.len);

  /* retrieve key: FsInstance */
  owa.len = sizeof(objFsInstanceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsInstance,
                          (xLibU8_t *) & objFsInstanceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memcpy (nextObjFsDataSourceValue, objFsDataSourceValue, sizeof (nextObjFsDataSourceValue));
    memset (&nextObjFsInstanceValue, 0, sizeof (nextObjFsInstanceValue));
    
    nextObjFsInstanceValue = FD_SFLOW_INSTANCE;
    /* right now we are assuming Instance vales as always 1 */

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objFsInstanceValue, owa.len);

    memcpy (nextObjFsDataSourceValue, objFsDataSourceValue, sizeof (nextObjFsDataSourceValue));
    nextObjFsInstanceValue = objFsInstanceValue;
    
    owa.l7rc = usmdbsFlowFsEntryGetNext (L7_UNIT_CURRENT,
                                                             nextObjFsDataSourceValue,
                                                             &nextObjFsInstanceValue);
  }

  if ((nextObjFsInstanceValue == objFsInstanceValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjFsInstanceValue, owa.len);

  /*return the object value: FsInstance */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjFsInstanceValue,
                           sizeof (nextObjFsInstanceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowFsTable_FsReceiver
*
* @purpose Get 'FsReceiver'
 *@description  [FsReceiver] The SFlowReceiver for this flow sampler.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowFsTable_FsReceiver (void *wap, void *bufp)
{

  fpObjWa_t kwaFsDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyFsDataSourceValue;
  xLibU32_t keyFsInstanceValue;
  xLibU32_t intIfIndex;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFsReceiverValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FsDataSource */
  kwaFsDataSource.len = sizeof (xLibStr256_t);
  kwaFsDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                                      (xLibU8_t *) keyFsDataSourceValue, &kwaFsDataSource.len);
  if (kwaFsDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaFsDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaFsDataSource);
    return kwaFsDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFsDataSourceValue, kwaFsDataSource.len);

  keyFsInstanceValue = FD_SFLOW_INSTANCE;
  /* get the value from application */
  if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyFsDataSourceValue, &intIfIndex)) == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowFsReceiverGet( L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, &objFsReceiverValue);
    if ( owa.l7rc == L7_FAILURE)
    {
      objFsReceiverValue = L7_NULL;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FsReceiver */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFsReceiverValue, sizeof (objFsReceiverValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowFsTable_FsReceiver
*
* @purpose Set 'FsReceiver'
 *@description  [FsReceiver] The SFlowReceiver for this flow sampler.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowFsTable_FsReceiver (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFsReceiverValue;
  fpObjWa_t kwaFsDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyFsDataSourceValue;
  xLibU32_t keyFsInstanceValue;
  xLibU32_t intIfIndex;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FsReceiver */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFsReceiverValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFsReceiverValue, owa.len);

  /* retrieve key: FsDataSource */
  kwaFsDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                                      (xLibU8_t *) keyFsDataSourceValue, &kwaFsDataSource.len);
  if (kwaFsDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaFsDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaFsDataSource);
    return kwaFsDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFsDataSourceValue, kwaFsDataSource.len);

   keyFsInstanceValue = FD_SFLOW_INSTANCE;
  
  owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyFsDataSourceValue, &intIfIndex);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowFsReceiverSet( L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, objFsReceiverValue);
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
* @function fpObjGet_basesFlowFsTable_FsPacketSamplingRate
*
* @purpose Get 'FsPacketSamplingRate'
 *@description  [FsPacketSamplingRate] The statistical sampling rate for packet
* sampling from this source.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowFsTable_FsPacketSamplingRate (void *wap, void *bufp)
{

  fpObjWa_t kwaFsDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyFsDataSourceValue;
  xLibU32_t keyFsInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFsPacketSamplingRateValue;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FsDataSource */
  kwaFsDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                                      (xLibU8_t *) keyFsDataSourceValue, &kwaFsDataSource.len);
  if (kwaFsDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaFsDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaFsDataSource);
    return kwaFsDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFsDataSourceValue, kwaFsDataSource.len);

  keyFsInstanceValue = FD_SFLOW_INSTANCE;
  /* get the value from application */
  if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyFsDataSourceValue, &intIfIndex)) == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowFsPacketSamplingRateGet( L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, &objFsPacketSamplingRateValue);
    if ( owa.l7rc == L7_FAILURE)
    {
      objFsPacketSamplingRateValue = FD_SFLOW_SAMPLING_RATE;
      owa.l7rc = L7_SUCCESS;
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FsPacketSamplingRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFsPacketSamplingRateValue,
                           sizeof (objFsPacketSamplingRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowFsTable_FsPacketSamplingRate
*
* @purpose Set 'FsPacketSamplingRate'
 *@description  [FsPacketSamplingRate] The statistical sampling rate for packet
* sampling from this source.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowFsTable_FsPacketSamplingRate (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFsPacketSamplingRateValue;
  fpObjWa_t kwaFsDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyFsDataSourceValue;
  xLibU32_t keyFsInstanceValue;
  xLibU32_t rcvr=L7_NULL;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FsPacketSamplingRate */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFsPacketSamplingRateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFsPacketSamplingRateValue, owa.len);
  /* retrieve key: FsDataSource */
  kwaFsDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                                      (xLibU8_t *) keyFsDataSourceValue, &kwaFsDataSource.len);
  if (kwaFsDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaFsDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaFsDataSource);
    return kwaFsDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFsDataSourceValue, kwaFsDataSource.len);

  keyFsInstanceValue = FD_SFLOW_INSTANCE;

  /* set the value in application */
  owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyFsDataSourceValue, &intIfIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
    if(usmdbsFlowFsReceiverGet(L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, &rcvr) == L7_SUCCESS)
    {
      if(rcvr != L7_NULL)
      {
        owa.l7rc = usmdbsFlowFsPacketSamplingRateSet( L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, 
                                                    objFsPacketSamplingRateValue);
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

/*******************************************************************************
* @function fpObjGet_basesFlowFsTable_FsMaximumHeaderSize
*
* @purpose Get 'FsMaximumHeaderSize'
 *@description  [FsMaximumHeaderSize] The maximum number of bytes that should be
* copied from a sampled packet.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowFsTable_FsMaximumHeaderSize (void *wap, void *bufp)
{

  fpObjWa_t kwaFsDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyFsDataSourceValue;
  xLibU32_t keyFsInstanceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFsMaximumHeaderSizeValue;
  xLibU32_t intIfIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FsDataSource */
  kwaFsDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                                      (xLibU8_t *) keyFsDataSourceValue, &kwaFsDataSource.len);
  if (kwaFsDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaFsDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaFsDataSource);
    return kwaFsDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFsDataSourceValue, kwaFsDataSource.len);
 
  keyFsInstanceValue = FD_SFLOW_INSTANCE;
  /* get the value from application */
  if((owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyFsDataSourceValue, &intIfIndex)) == L7_SUCCESS)
  {
    owa.l7rc = usmdbsFlowFsMaximumHeaderSizeGet( L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, &objFsMaximumHeaderSizeValue);
    if ( owa.l7rc == L7_FAILURE)
    {
      objFsMaximumHeaderSizeValue = FD_SFLOW_DEFAULT_HEADER_SIZE;
      owa.l7rc = L7_SUCCESS;
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FsMaximumHeaderSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFsMaximumHeaderSizeValue,
                           sizeof (objFsMaximumHeaderSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowFsTable_FsMaximumHeaderSize
*
* @purpose Set 'FsMaximumHeaderSize'
 *@description  [FsMaximumHeaderSize] The maximum number of bytes that should be
* copied from a sampled packet.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowFsTable_FsMaximumHeaderSize (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFsMaximumHeaderSizeValue;
  fpObjWa_t kwaFsDataSource = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyFsDataSourceValue;
  xLibU32_t keyFsInstanceValue;
  xLibU32_t rcvr = L7_NULL;
  xLibU32_t intIfIndex;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FsMaximumHeaderSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFsMaximumHeaderSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFsMaximumHeaderSizeValue, owa.len);

  /* retrieve key: FsDataSource */
  kwaFsDataSource.rc = xLibFilterGet (wap, XOBJ_basesFlowFsTable_FsDataSource,
                                      (xLibU8_t *) keyFsDataSourceValue, &kwaFsDataSource.len);
  if (kwaFsDataSource.rc != XLIBRC_SUCCESS)
  {
    kwaFsDataSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaFsDataSource);
    return kwaFsDataSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFsDataSourceValue, kwaFsDataSource.len);

  keyFsInstanceValue = FD_SFLOW_INSTANCE;
 
  owa.l7rc = usmdbsFlowintIfIndexGet(L7_UNIT_CURRENT, keyFsDataSourceValue, &intIfIndex);

  if (owa.l7rc == L7_SUCCESS)
  {
    if(usmdbsFlowFsReceiverGet(L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, &rcvr) == L7_SUCCESS)
    {
      if(rcvr != L7_NULL)
      {
        owa.l7rc = usmdbsFlowFsMaximumHeaderSizeSet( L7_UNIT_CURRENT, intIfIndex, keyFsInstanceValue, 
                                                     objFsMaximumHeaderSizeValue);
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
