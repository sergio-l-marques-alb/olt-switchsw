/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfNetworkArea.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfNetworkArea-object.xml
*
* @create  28 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfNetworkArea_obj.h"
#include "usmdb_ospf_api.h"


/*******************************************************************************
* @function fpObjGet_routingospfNetworkArea_ipAddr
*
* @purpose Get 'ipAddr'
 *@description  [ipAddr] ip address for create/delete a network area entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNetworkArea_ipAddr (void *wap, void *bufp)
{

  xLibIpV4_t objipAddrValue;
  xLibIpV4_t nextObjipAddrValue;
  xLibIpV4_t objwildcardMaskValue;
  xLibIpV4_t nextObjwildcardMaskValue;
  xLibU32_t objareaIdValue;
  xLibU32_t nextObjareaIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddr */
  memset (&nextObjipAddrValue, 0, sizeof (nextObjipAddrValue));
  memset (&objwildcardMaskValue, 0, sizeof (objwildcardMaskValue));
  memset (&objareaIdValue, 0, sizeof (objareaIdValue));
  memset (&nextObjipAddrValue, 0, sizeof (nextObjipAddrValue));
  memset (&nextObjwildcardMaskValue, 0, sizeof (nextObjwildcardMaskValue));
  memset (&nextObjareaIdValue, 0, sizeof (nextObjareaIdValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_ipAddr,
                          (xLibU8_t *) & objipAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &nextObjipAddrValue, &nextObjwildcardMaskValue,
                         &nextObjareaIdValue);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipAddrValue, owa.len);
    nextObjipAddrValue = objipAddrValue;
    do
    {
      owa.l7rc = usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &nextObjipAddrValue, &nextObjwildcardMaskValue,
                         &nextObjareaIdValue);
    }
    while ((objipAddrValue == nextObjipAddrValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipAddrValue, owa.len);

  /* return the object value: ipAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipAddrValue, sizeof (objipAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfNetworkArea_wildcardMask
*
* @purpose Get 'wildcardMask'
 *@description  [wildcardMask] wildcardMask for Create/delete a network area
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNetworkArea_wildcardMask (void *wap, void *bufp)
{

  xLibIpV4_t objipAddrValue;
  xLibIpV4_t nextObjipAddrValue;
  xLibIpV4_t objwildcardMaskValue;
  xLibIpV4_t nextObjwildcardMaskValue;
  xLibU32_t objareaIdValue;
  xLibU32_t nextObjareaIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddr */
  memset (&nextObjipAddrValue, 0, sizeof (nextObjipAddrValue));
  memset (&objwildcardMaskValue, 0, sizeof (objwildcardMaskValue));
  memset (&objareaIdValue, 0, sizeof (objareaIdValue));
  memset (&nextObjipAddrValue, 0, sizeof (nextObjipAddrValue));
  memset (&nextObjwildcardMaskValue, 0, sizeof (nextObjwildcardMaskValue));
  memset (&nextObjareaIdValue, 0, sizeof (nextObjareaIdValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_ipAddr,
                          (xLibU8_t *) & objipAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipAddrValue, owa.len);

  /* retrieve key: wildcardMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_wildcardMask,
                          (xLibU8_t *) & objwildcardMaskValue, &owa.len);

  nextObjipAddrValue = objipAddrValue;
  nextObjwildcardMaskValue = objwildcardMaskValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &nextObjipAddrValue, &nextObjwildcardMaskValue,
                         &nextObjareaIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objwildcardMaskValue, owa.len);
    do
    {
      owa.l7rc = usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &nextObjipAddrValue, &nextObjwildcardMaskValue,
                         &nextObjareaIdValue);
    }
    while ((objipAddrValue == nextObjipAddrValue)
           && (objwildcardMaskValue == nextObjwildcardMaskValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objipAddrValue != nextObjipAddrValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjwildcardMaskValue, owa.len);

  /* return the object value: wildcardMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjwildcardMaskValue,
                           sizeof (objwildcardMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfNetworkArea_areaId
*
* @purpose Get 'areaId'
 *@description  [areaId] area associated with the address   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNetworkArea_areaId (void *wap, void *bufp)
{

  xLibIpV4_t objipAddrValue;
  xLibIpV4_t nextObjipAddrValue;
  xLibIpV4_t objwildcardMaskValue;
  xLibIpV4_t nextObjwildcardMaskValue;
  xLibU32_t objareaIdValue;
  xLibU32_t nextObjareaIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddr */

  memset (&nextObjipAddrValue, 0, sizeof (nextObjipAddrValue));
  memset (&objwildcardMaskValue, 0, sizeof (objwildcardMaskValue));
  memset (&objareaIdValue, 0, sizeof (objareaIdValue));
  memset (&nextObjipAddrValue, 0, sizeof (nextObjipAddrValue));
  memset (&nextObjwildcardMaskValue, 0, sizeof (nextObjwildcardMaskValue));
  memset (&nextObjareaIdValue, 0, sizeof (nextObjareaIdValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_ipAddr,
                          (xLibU8_t *) & objipAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipAddrValue, owa.len);

  /* retrieve key: wildcardMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_wildcardMask,
                          (xLibU8_t *) & objwildcardMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objwildcardMaskValue, owa.len);

  /* retrieve key: areaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_areaId,
                          (xLibU8_t *) & objareaIdValue, &owa.len);

  nextObjipAddrValue = objipAddrValue;
  nextObjwildcardMaskValue = objwildcardMaskValue;
  nextObjareaIdValue = objareaIdValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &nextObjipAddrValue, &nextObjwildcardMaskValue,
                         &nextObjareaIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objareaIdValue, owa.len);
    owa.l7rc = usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &nextObjipAddrValue, &nextObjwildcardMaskValue,
                         &nextObjareaIdValue);

  }

  if ((objipAddrValue != nextObjipAddrValue) || (objwildcardMaskValue != nextObjwildcardMaskValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjareaIdValue, owa.len);

  /* return the object value: areaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjareaIdValue, sizeof (objareaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#if 0
/*******************************************************************************
* @function fpObjGet_routingospfNetworkArea_ipAddr_wildcardMask_areaId
*
* @purpose Get 'ipAddr + wildcardMask + areaId +'
*
* @description [ipAddr]: ip address for create/delete a network area entry.
*              
*              [wildcardMask]: wildcardMask for Create/delete a network area
*              entry. 
*              [areaId]: area associated with the address 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNetworkArea_ipAddr_wildcardMask_areaId (void *wap,
                                                                     void
                                                                     *bufp[],
                                                                     xLibU16_t
                                                                     keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaipAddr = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAddrValue, nextObjipAddrValue;
  fpObjWa_t owawildcardMask = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objwildcardMaskValue, nextObjwildcardMaskValue;
  fpObjWa_t owaareaId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objareaIdValue, nextObjareaIdValue;
  void *outipAddr = (void *) bufp[--keyCount];
  void *outwildcardMask = (void *) bufp[--keyCount];
  void *outareaId = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outipAddr);
  FPOBJ_TRACE_ENTER (outwildcardMask);
  FPOBJ_TRACE_ENTER (outareaId);

  /* retrieve key: ipAddr */
  owaipAddr.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_ipAddr,
                                (xLibU8_t *) & objipAddrValue, &owaipAddr.len);
  if (owaipAddr.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: wildcardMask */
    owawildcardMask.rc =
      xLibFilterGet (wap, XOBJ_routingospfNetworkArea_wildcardMask,
                     (xLibU8_t *) & objwildcardMaskValue, &owawildcardMask.len);
    if (owawildcardMask.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: areaId */
      owaareaId.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_areaId,
                                    (xLibU8_t *) & objareaIdValue,
                                    &owaareaId.len);
    }
  }
  else
  {
    objipAddrValue = 0 ;
    objwildcardMaskValue = 0 ;
    objareaIdValue = 0 ;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (outipAddr, &objipAddrValue, owaipAddr.len);
  FPOBJ_TRACE_CURRENT_KEY (outwildcardMask, &objwildcardMaskValue,
                           owawildcardMask.len);
  FPOBJ_TRACE_CURRENT_KEY (outareaId, &objareaIdValue, owaareaId.len);

  owa.rc =  usmDbOspfNetworkAreaEntryNext(L7_UNIT_CURRENT, &objipAddrValue, &objwildcardMaskValue,
                         &objareaIdValue);

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outipAddr, owaipAddr);
    FPOBJ_TRACE_EXIT (outwildcardMask, owawildcardMask);
    FPOBJ_TRACE_EXIT (outareaId, owaareaId);
    return owa.rc;
  }

  nextObjipAddrValue = objipAddrValue;
  nextObjwildcardMaskValue = objwildcardMaskValue;
  nextObjareaIdValue = objareaIdValue;
  
  FPOBJ_TRACE_CURRENT_KEY (outipAddr, &nextObjipAddrValue, owaipAddr.len);
  FPOBJ_TRACE_CURRENT_KEY (outwildcardMask, &nextObjwildcardMaskValue,
                           owawildcardMask.len);
  FPOBJ_TRACE_CURRENT_KEY (outareaId, &nextObjareaIdValue, owaareaId.len);

  /* return the object value: ipAddr */
  xLibBufDataSet (outipAddr,
                  (xLibU8_t *) & nextObjipAddrValue,
                  sizeof (nextObjipAddrValue));

  /* return the object value: wildcardMask */
  xLibBufDataSet (outwildcardMask,
                  (xLibU8_t *) & nextObjwildcardMaskValue,
                  sizeof (nextObjwildcardMaskValue));

  /* return the object value: areaId */
  xLibBufDataSet (outareaId,
                  (xLibU8_t *) & nextObjareaIdValue,
                  sizeof (nextObjareaIdValue));
  FPOBJ_TRACE_EXIT (outipAddr, owaipAddr);
  FPOBJ_TRACE_EXIT (outwildcardMask, owawildcardMask);
  FPOBJ_TRACE_EXIT (outareaId, owaareaId);
  return XLIBRC_SUCCESS;
}

#endif
/*******************************************************************************
* @function fpObjGet_routingospfNetworkArea_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: This variable displays the status of the en- try.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNetworkArea_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keywildcardMaskValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyareaIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_ipAddr,
                           (xLibU8_t *) & keyipAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddrValue, kwa1.len);

  /* retrieve key: wildcardMask */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_wildcardMask,
                           (xLibU8_t *) & keywildcardMaskValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keywildcardMaskValue, kwa2.len);

  /* retrieve key: areaId */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_areaId,
                           (xLibU8_t *) & keyareaIdValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyareaIdValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNetworkAreaEntryGet(L7_UNIT_CURRENT, keyipAddrValue,
                              keywildcardMaskValue,
                              keyareaIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfNetworkArea_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: This variable displays the status of the en- try.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfNetworkArea_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keywildcardMaskValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyareaIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: ipAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_ipAddr,
                           (xLibU8_t *) & keyipAddrValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAddrValue, kwa1.len);

  /* retrieve key: wildcardMask */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_wildcardMask,
                           (xLibU8_t *) & keywildcardMaskValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keywildcardMaskValue, kwa2.len);

  /* retrieve key: areaId */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfNetworkArea_areaId,
                           (xLibU8_t *) & keyareaIdValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyareaIdValue, kwa3.len);

  /* call the usmdb only for add and delete */
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc =
      usmDbOspfNetworkAreaEntryCreate(L7_UNIT_CURRENT, keyipAddrValue,
                                       keywildcardMaskValue, keyareaIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc =
      
      usmDbOspfNetworkAreaEntryDelete (L7_UNIT_CURRENT, keyipAddrValue,
                                       keywildcardMaskValue, keyareaIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}
