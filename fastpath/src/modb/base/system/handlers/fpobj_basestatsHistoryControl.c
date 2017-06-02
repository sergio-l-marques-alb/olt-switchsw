/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basestatsHistoryControl.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basestatsHistoryControl_obj.h"
#include "usmdb_mib_rmon_api.h"
#include "usmdb_util_api.h"
#include "rmon_exports.h"

/*******************************************************************************
* @function fpObjGet_basestatsHistoryControl_historyControlFreeEntry
*
* @purpose Get 'historyControlFreeEntry'
*
* @description  An index that uniquely identifies an entry in the historyControl 
*              table. Each such entry defines a set of samples at a particular 
*              interval for an interface on the device. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlFreeEntry (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val, i;
  xLibU32_t nextObjhistoryControlFreeEntryValue;
  FPOBJ_TRACE_ENTER (bufp);


  for ( i=1 ; i <= L7_RMON_HISTORY_CONTROL_ENTRY_MAX_NUM ; i++ )
  {
    if (usmDbHistoryControlStatusGet(L7_UNIT_CURRENT, i, &val ) != L7_SUCCESS )
        break;
    if (( val != RMON_HISTORY_CONTROL_STATUS_VALID) &&
        (val != RMON_HISTORY_CONTROL_STATUS_UNDERCREATION))
        break;
  }
  if ( i <= L7_RMON_HISTORY_CONTROL_ENTRY_MAX_NUM )
    nextObjhistoryControlFreeEntryValue = i;
  else
    nextObjhistoryControlFreeEntryValue = 1;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjhistoryControlFreeEntryValue, owa.len);

  /* return the object value: historyControlFreeEntry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjhistoryControlFreeEntryValue,
                           sizeof (nextObjhistoryControlFreeEntryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_basestatsHistoryControl_historyControlIndex
*
* @purpose Get 'historyControlIndex'
*
* @description  An index that uniquely identifies an entry in the historyControl 
*              table. Each such entry defines a set of samples at a particular 
*              interval for an interface on the device. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlIndex (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlIndexValue;
  xLibU32_t nextObjhistoryControlIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & objhistoryControlIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjhistoryControlIndexValue = 0;
    owa.l7rc = usmDbHistoryControlEntryNext(L7_UNIT_CURRENT,
                                            &nextObjhistoryControlIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objhistoryControlIndexValue, owa.len);
    nextObjhistoryControlIndexValue = objhistoryControlIndexValue;
    owa.l7rc = usmDbHistoryControlEntryNext (L7_UNIT_CURRENT,
                                             &nextObjhistoryControlIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjhistoryControlIndexValue, owa.len);

  /* return the object value: historyControlIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjhistoryControlIndexValue,
                           sizeof (objhistoryControlIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basestatsHistoryControl_historyControlDataSource
*
* @purpose Get 'historyControlDataSource'
*
* @description  This object identifies the source of the data for which historical 
*              data was collected and placed in a media-specific table 
*              on behalf of this historyControlEntry. This source can be any 
*              interface on this device. In order 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlDataSource (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlDataSourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbHistoryControlDataSourceGet (L7_UNIT_CURRENT,
                                               keyhistoryControlIndexValue,
                                               &objhistoryControlDataSourceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(usmDbIntIfNumFromExtIfNum(objhistoryControlDataSourceValue, &objhistoryControlDataSourceValue)!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: historyControlDataSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objhistoryControlDataSourceValue,
                           sizeof (objhistoryControlDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_basestatsHistoryControl_historyControlDataSource 
*
* @purpose Get 'historyControlDataSource'
*
* @description  This object identifies the source of the data for which historical
*              data was collected and placed in a media-specific table
*              on behalf of this historyControlEntry. This source can be any
*              interface on this device. In order
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_basestatsHistoryControl_historyControlDataSource (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlDataSourceValue;
  xLibU32_t nextObjHistoryControlDataSourceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlDataSource,
                          (xLibU8_t *) &objhistoryControlDataSourceValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjHistoryControlDataSourceValue = 0;
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjHistoryControlDataSourceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objhistoryControlDataSourceValue, owa.len);
         owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                               0, objhistoryControlDataSourceValue, &nextObjHistoryControlDataSourceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjHistoryControlDataSourceValue, owa.len);

  /* return the object value: PortMirrorSessionID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjHistoryControlDataSourceValue,
                    sizeof (objhistoryControlDataSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_basestatsHistoryControl_historyControlDataSource
*
* @purpose Set 'historyControlDataSource'
*
* @description  This object identifies the source of the data for which historical 
*              data was collected and placed in a media-specific table 
*              on behalf of this historyControlEntry. This source can be any 
*              interface on this device. In order 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basestatsHistoryControl_historyControlDataSource (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objhistoryControlDataSourceValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: historyControlDataSource */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objhistoryControlDataSourceValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objhistoryControlDataSourceValue, owa.len);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);
  if(usmDbExtIfNumFromIntIfNum(objhistoryControlDataSourceValue, &objhistoryControlDataSourceValue)!=L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbHistoryControlDataSourceSet (L7_UNIT_CURRENT,
                                      keyhistoryControlIndexValue,
                                      objhistoryControlDataSourceValue);
                                      
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
* @function fpObjGet_basestatsHistoryControl_historyControlBucketsRequested
*
* @purpose Get 'historyControlBucketsRequested'
*
* @description  The requested number of discrete time intervals over which data 
*              is to be saved in the part of the media-specific table associated 
*              with this historyControlEntry. When this object is created 
*              or modified, the probe s 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlBucketsRequested (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlBucketsRequestedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbHistoryControlBucketsRequestedGet (L7_UNIT_CURRENT,
                                            keyhistoryControlIndexValue,
                                            &objhistoryControlBucketsRequestedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: historyControlBucketsRequested */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objhistoryControlBucketsRequestedValue,
                    sizeof (objhistoryControlBucketsRequestedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basestatsHistoryControl_historyControlBucketsRequested
*
* @purpose Set 'historyControlBucketsRequested'
*
* @description  The requested number of discrete time intervals over which data 
*              is to be saved in the part of the media-specific table associated 
*              with this historyControlEntry. When this object is created 
*              or modified, the probe s 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basestatsHistoryControl_historyControlBucketsRequested (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlBucketsRequestedValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: historyControlBucketsRequested */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objhistoryControlBucketsRequestedValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objhistoryControlBucketsRequestedValue, owa.len);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbHistoryControlBucketsRequestedSet (L7_UNIT_CURRENT,
                                            keyhistoryControlIndexValue,
                                            objhistoryControlBucketsRequestedValue);
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
* @function fpObjGet_basestatsHistoryControl_historyControlBucketsGranted
*
* @purpose Get 'historyControlBucketsGranted'
*
* @description  The number of discrete sampling intervals over which data shall 
*              be saved in the part of the media-specific table associated 
*              with this historyControlEntry. When the associated historyControlBucketsRequested 
*              object 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlBucketsGranted (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlBucketsGrantedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbHistoryControlBucketsGrantedGet (L7_UNIT_CURRENT,
                                          keyhistoryControlIndexValue,
                                          &objhistoryControlBucketsGrantedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: historyControlBucketsGranted */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objhistoryControlBucketsGrantedValue,
                    sizeof (objhistoryControlBucketsGrantedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basestatsHistoryControl_historyControlInterval
*
* @purpose Get 'historyControlInterval'
*
* @description  The interval in seconds over which the data is sampled for each 
*              bucket in the part of the media-specific table associated 
*              with this historyControlEntry. This interval can be set to any 
*              number of seconds between 1 and 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlInterval (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbHistoryControlIntervalGet (L7_UNIT_CURRENT,
                                    keyhistoryControlIndexValue,
                                    &objhistoryControlIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: historyControlInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhistoryControlIntervalValue,
                           sizeof (objhistoryControlIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basestatsHistoryControl_historyControlInterval
*
* @purpose Set 'historyControlInterval'
*
* @description  The interval in seconds over which the data is sampled for each 
*              bucket in the part of the media-specific table associated 
*              with this historyControlEntry. This interval can be set to any 
*              number of seconds between 1 and 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basestatsHistoryControl_historyControlInterval (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: historyControlInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objhistoryControlIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objhistoryControlIntervalValue, owa.len);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbHistoryControlIntervalSet (L7_UNIT_CURRENT,
                                    keyhistoryControlIndexValue,
                                    objhistoryControlIntervalValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbHistoryControlStatusSet (L7_UNIT_CURRENT, keyhistoryControlIndexValue,
                                             RMON_HISTORY_CONTROL_STATUS_VALID);
  }
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
* @function fpObjGet_basestatsHistoryControl_historyControlOwner
*
* @purpose Get 'historyControlOwner'
*
* @description  The entity that configured this entry and is therefore using 
*              the resources assigned to it. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlOwner (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objhistoryControlOwnerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbHistoryControlOwnerGet (L7_UNIT_CURRENT, keyhistoryControlIndexValue,
                                 objhistoryControlOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: historyControlOwner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objhistoryControlOwnerValue,
                           strlen (objhistoryControlOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basestatsHistoryControl_historyControlOwner
*
* @purpose Set 'historyControlOwner'
*
* @description  The entity that configured this entry and is therefore using 
*              the resources assigned to it. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basestatsHistoryControl_historyControlOwner (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objhistoryControlOwnerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: historyControlOwner */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objhistoryControlOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objhistoryControlOwnerValue, owa.len);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbHistoryControlOwnerSet (L7_UNIT_CURRENT, keyhistoryControlIndexValue,
                                 objhistoryControlOwnerValue);
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
* @function fpObjGet_basestatsHistoryControl_historyControlStatus
*
* @purpose Get 'historyControlStatus'
*
* @description  The status of this historyControl entry. Each instance of the 
*              media-specific table associated with this historyControlEntry 
*              will be deleted by the agent if this historyControlEntry is 
*              not equal to valid(1). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basestatsHistoryControl_historyControlStatus (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  xLibU32_t objhistoryControlStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbHistoryControlStatusGet (L7_UNIT_CURRENT, keyhistoryControlIndexValue,
                                  &objhistoryControlStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: historyControlStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhistoryControlStatusValue,
                           sizeof (objhistoryControlStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basestatsHistoryControl_historyControlStatus
*
* @purpose Set 'historyControlStatus'
*
* @description  The status of this historyControl entry. Each instance of the 
*              media-specific table associated with this historyControlEntry 
*              will be deleted by the agent if this historyControlEntry is 
*              not equal to valid(1). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basestatsHistoryControl_historyControlStatus (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objhistoryControlStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyhistoryControlIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: historyControlStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objhistoryControlStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objhistoryControlStatusValue, owa.len);

  /* retrieve key: historyControlIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basestatsHistoryControl_historyControlIndex,
                          (xLibU8_t *) & keyhistoryControlIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhistoryControlIndexValue, kwa.len);

  if (objhistoryControlStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* set the value in application */
    owa.l7rc = usmDbHistoryControlStatusSet (L7_UNIT_CURRENT, keyhistoryControlIndexValue,
                                             RMON_HISTORY_CONTROL_STATUS_CREATEREQUEST);
  }
  else if (objhistoryControlStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbHistoryControlStatusSet(L7_UNIT_CURRENT, keyhistoryControlIndexValue,
                       RMON_HISTORY_CONTROL_STATUS_INVALID);

  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
