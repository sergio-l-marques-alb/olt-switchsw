/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServPolicyPerfInStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to diffserv-object.xml
*
* @create  1 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_qosDiffServPolicyPerfInStats_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_PolicyIndex_PolicyInstIndex_ifIndex
*
* @purpose Get 'PolicyIndex + PolicyInstIndex + ifIndex +'
*
* @description [PolicyIndex]: The identifier for DiffServ Policy table entry.
*              
*              [PolicyInstIndex]: The identifier for policy-class Instance
*              table entry within a policy. 
*              [ifIndex]: A unique value for each interface. Its value ranges
*              between 1 and the value of ifNumber. The value for each
*              interface must remain constant at least from one re-initialization
*              of the entity's network management 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_qosDiffServPolicyPerfInStats_PolicyIndex_PolicyInstIndex_ifIndex (void
                                                                           *wap,
                                                                           void
                                                                           *bufp
                                                                           [],
                                                                           xLibU16_t
                                                                           keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaPolicyIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyIndexValue, nextObjPolicyIndexValue;
  fpObjWa_t owaPolicyInstIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstIndexValue, nextObjPolicyInstIndexValue;
  fpObjWa_t owaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue, nextObjifIndexValue;
  
  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempNextIntIfIndexValue;
  L7_RC_t rc = L7_FAILURE;
  
  void *outPolicyIndex = (void *) bufp[--keyCount];
  void *outPolicyInstIndex = (void *) bufp[--keyCount];
  void *outifIndex = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outPolicyIndex);
  FPOBJ_TRACE_ENTER (outPolicyInstIndex);
  FPOBJ_TRACE_ENTER (outifIndex);

  /* retrieve key: PolicyIndex */
  owaPolicyIndex.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                   (xLibU8_t *) & objPolicyIndexValue, &owaPolicyIndex.len);
  if (owaPolicyIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: PolicyInstIndex */
    owaPolicyInstIndex.rc =
      xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                     (xLibU8_t *) & objPolicyInstIndexValue,
                     &owaPolicyInstIndex.len);
    if (owaPolicyInstIndex.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: ifIndex */
      owaifIndex.rc =
        xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                       (xLibU8_t *) & objifIndexValue, &owaifIndex.len);
    }
  }
  else
  {
    objPolicyIndexValue = 0;
    objPolicyInstIndexValue = 0;
    tempIntIfIndexValue = 0;
    tempNextIntIfIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (outPolicyIndex, &objPolicyIndexValue,
                           owaPolicyIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outPolicyInstIndex, &objPolicyInstIndexValue,
                           owaPolicyInstIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outifIndex, &objifIndexValue, owaifIndex.len);

  if( usmDbIntIfNumFromExtIfNum(objifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outPolicyIndex, owaPolicyIndex);
    FPOBJ_TRACE_EXIT (outPolicyInstIndex, owaPolicyInstIndex);
    FPOBJ_TRACE_EXIT (outifIndex, owaifIndex);
    return owa.rc;
  }
  
  while( (usmDbDiffServPolicyPerfOutGetNext (L7_UNIT_CURRENT, objPolicyIndexValue,
                         objPolicyInstIndexValue, tempIntIfIndexValue,
                         &nextObjPolicyIndexValue, &nextObjPolicyInstIndexValue,
                         &tempNextIntIfIndexValue) == L7_SUCCESS) &&
                ( usmDbExtIfNumFromIntIfNum(tempNextIntIfIndexValue, &nextObjifIndexValue) == L7_SUCCESS) )
  {
      if(usmDbDiffServPolicyPerfOutGet(USMDB_UNIT_CURRENT, 
                                    nextObjPolicyIndexValue, 
                                    nextObjPolicyInstIndexValue, 
                                    tempNextIntIfIndexValue) == L7_SUCCESS)
  	{
  	   rc = L7_SUCCESS;
          break;
      	}
  }

  if(rc != L7_SUCCESS )
  {
     owa.rc = L7_FAILURE;
  }
  else
  {
     owa.rc = L7_SUCCESS;
  }

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outPolicyIndex, owaPolicyIndex);
    FPOBJ_TRACE_EXIT (outPolicyInstIndex, owaPolicyInstIndex);
    FPOBJ_TRACE_EXIT (outifIndex, owaifIndex);
    return owa.rc;
  }

  if( usmDbExtIfNumFromIntIfNum(tempNextIntIfIndexValue,&nextObjifIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outPolicyIndex, owaPolicyIndex);
    FPOBJ_TRACE_EXIT (outPolicyInstIndex, owaPolicyInstIndex);
    FPOBJ_TRACE_EXIT (outifIndex, owaifIndex);
    return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (outPolicyIndex, &nextObjPolicyIndexValue,
                           owaPolicyIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outPolicyInstIndex, &nextObjPolicyInstIndexValue,
                           owaPolicyInstIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outifIndex, &nextObjifIndexValue, owaifIndex.len);

  /* return the object value: PolicyIndex */
  xLibBufDataSet (outPolicyIndex,
                  (xLibU8_t *) & nextObjPolicyIndexValue,
                  sizeof (nextObjPolicyIndexValue));

  /* return the object value: PolicyInstIndex */
  xLibBufDataSet (outPolicyInstIndex,
                  (xLibU8_t *) & nextObjPolicyInstIndexValue,
                  sizeof (nextObjPolicyInstIndexValue));

  /* return the object value: ifIndex */
  xLibBufDataSet (outifIndex,
                  (xLibU8_t *) & nextObjifIndexValue,
                  sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (outPolicyIndex, owaPolicyIndex);
  FPOBJ_TRACE_EXIT (outPolicyInstIndex, owaPolicyInstIndex);
  FPOBJ_TRACE_EXIT (outifIndex, owaifIndex);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_PolicyIndex
*
* @purpose Get 'PolicyIndex'
*
* @description [PolicyIndex] The identifier for DiffServ Policy table entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_PolicyIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyIndexValue = objPolicyInstIndexValue = objifIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyPerfOutGetNext(L7_UNIT_CURRENT,
                                     objPolicyIndexValue,
                                     objPolicyInstIndexValue,
                                     objifIndexValue, &nextObjPolicyIndexValue,
                                     &nextObjPolicyInstIndexValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);
    objPolicyInstIndexValue = objifIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDiffServPolicyPerfOutGetNext (L7_UNIT_CURRENT,
                                      objPolicyIndexValue,
                                      objPolicyInstIndexValue,
                                      objifIndexValue, &nextObjPolicyIndexValue,
                                      &nextObjPolicyInstIndexValue, &nextObjifIndexValue);
    }
    while ((objPolicyIndexValue == nextObjPolicyIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyIndexValue, owa.len);

  /* return the object value: PolicyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyIndexValue,
                           sizeof (objPolicyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_PolicyInstIndex
*
* @purpose Get 'PolicyInstIndex'
*
* @description [PolicyInstIndex] The identifier for policy-class Instance table entry within a policy.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_PolicyInstIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);

  /* retrieve key: PolicyInstIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                          (xLibU8_t *) & objPolicyInstIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyInstIndexValue = objifIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyPerfOutGetNext (L7_UNIT_CURRENT,
                                     objPolicyIndexValue,
                                     objPolicyInstIndexValue,
                                     objifIndexValue, &nextObjPolicyIndexValue,
                                     &nextObjPolicyInstIndexValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyInstIndexValue, owa.len);
    objifIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDiffServPolicyPerfOutGetNext (L7_UNIT_CURRENT,
                                      objPolicyIndexValue,
                                      objPolicyInstIndexValue,
                                      objifIndexValue, &nextObjPolicyIndexValue,
                                      &nextObjPolicyInstIndexValue, &nextObjifIndexValue);
    }
    while ((objPolicyIndexValue == nextObjPolicyIndexValue)
           && (objPolicyInstIndexValue == nextObjPolicyInstIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objPolicyIndexValue != nextObjPolicyIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyInstIndexValue, owa.len);

  /* return the object value: PolicyInstIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyInstIndexValue,
                           sizeof (objPolicyInstIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_ifIndex
*
* @purpose Get 'ifIndex'
*
* @description [ifIndex] A unique value for each interface. Its value ranges between 1 and the value of ifNumber. The value for each interface must remain constant at least from one re-initialization of the entity's network management
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_ifIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);

  /* retrieve key: PolicyInstIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                          (xLibU8_t *) & objPolicyInstIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyInstIndexValue, owa.len);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objifIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyPerfOutGetNext (L7_UNIT_CURRENT,
                                     objPolicyIndexValue,
                                     objPolicyInstIndexValue,
                                     objifIndexValue, &nextObjPolicyIndexValue,
                                     &nextObjPolicyInstIndexValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

    owa.l7rc = usmDbDiffServPolicyPerfOutGetNext (L7_UNIT_CURRENT,
                                    objPolicyIndexValue,
                                    objPolicyInstIndexValue,
                                    objifIndexValue, &nextObjPolicyIndexValue,
                                    &nextObjPolicyInstIndexValue, &nextObjifIndexValue);

  }

  if ((objPolicyIndexValue != nextObjPolicyIndexValue)
      || (objPolicyInstIndexValue != nextObjPolicyInstIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_OfferedOctets
*
* @purpose Get 'OfferedOctets'
*
* @description [OfferedOctets]: Offered octets count for the inbound policy-class
*              instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_OfferedOctets (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOfferedOctetsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInOfferedOctetsGet (L7_UNIT_CURRENT,
                                               keyPolicyIndexValue,
                                               keyPolicyInstIndexValue,
                                               tempIntIfIndexValue,
                                             &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: OfferedOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOfferedOctetsValue,
                           sizeof (objOfferedOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_OfferedPackets
*
* @purpose Get 'OfferedPackets'
*
* @description [OfferedPackets]: Offered packets count for the inbound policy-class
*              instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_OfferedPackets (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOfferedPacketsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInOfferedPacketsGet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                tempIntIfIndexValue,
                                              &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: OfferedPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOfferedPacketsValue,
                           sizeof (objOfferedPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_DiscardedOctets
*
* @purpose Get 'DiscardedOctets'
*
* @description [DiscardedOctets]: Discarded octets count for the inbound policy-class
*              instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_DiscardedOctets (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiscardedOctetsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInDiscardedOctetsGet (L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue,
                                                 keyPolicyInstIndexValue,
                                                 tempIntIfIndexValue,
                                               &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DiscardedOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDiscardedOctetsValue,
                           sizeof (objDiscardedOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_DiscardedPackets
*
* @purpose Get 'DiscardedPackets'
*
* @description [DiscardedPackets]: Discarded packets count for the inbound
*              policy-class instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_DiscardedPackets (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiscardedPacketsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInDiscardedPacketsGet (L7_UNIT_CURRENT,
                                                  keyPolicyIndexValue,
                                                  keyPolicyInstIndexValue,
                                                  tempIntIfIndexValue,
                                                &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DiscardedPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDiscardedPacketsValue,
                           sizeof (objDiscardedPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_HCOfferedOctets
*
* @purpose Get 'HCOfferedOctets'
*
* @description [HCOfferedOctets]: Offered octets high capacity count for the
*              inbound policy-class instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_HCOfferedOctets (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHCOfferedOctetsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInOfferedOctetsGet (L7_UNIT_CURRENT,
                                               keyPolicyIndexValue,
                                               keyPolicyInstIndexValue,
                                               tempIntIfIndexValue,
                                             &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HCOfferedOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHCOfferedOctetsValue,
                           sizeof (objHCOfferedOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_HCOfferedPackets
*
* @purpose Get 'HCOfferedPackets'
*
* @description [HCOfferedPackets]: Offered packets high capacity count for
*              the inbound policy-class instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_HCOfferedPackets (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHCOfferedPacketsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInOfferedPacketsGet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                tempIntIfIndexValue,
                                              &tempLowValue,&tempHighValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HCOfferedPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHCOfferedPacketsValue,
                           sizeof (objHCOfferedPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_HCDiscardedOctets
*
* @purpose Get 'HCDiscardedOctets'
*
* @description [HCDiscardedOctets]: Discarded octets high capacity count for
*              the inbound policy-class instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_HCDiscardedOctets (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHCDiscardedOctetsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInDiscardedOctetsGet (L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue,
                                                 keyPolicyInstIndexValue,
                                                 tempIntIfIndexValue,
                                               &tempLowValue,&tempHighValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HCDiscardedOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHCDiscardedOctetsValue,
                           sizeof (objHCDiscardedOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_HCDiscardedPackets
*
* @purpose Get 'HCDiscardedPackets'
*
* @description [HCDiscardedPackets]: Discarded packets high capacity count
*              for the inbound policy-class instance performance entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_HCDiscardedPackets (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHCDiscardedPacketsValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInDiscardedPacketsGet (L7_UNIT_CURRENT,
                                                  keyPolicyIndexValue,
                                                  keyPolicyInstIndexValue,
                                                  tempIntIfIndexValue,
                                                &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HCDiscardedPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHCDiscardedPacketsValue,
                           sizeof (objHCDiscardedPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_StorageType
*
* @purpose Get 'StorageType'
*
* @description [StorageType]: Storage-type for this conceptual row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_StorageType (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageTypeValue;

  xLibU32_t tempIntIfIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInStorageTypeGet (L7_UNIT_CURRENT,
                                             keyPolicyIndexValue,
                                             keyPolicyInstIndexValue,
                                             tempIntIfIndexValue,
                                             &objStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStorageTypeValue,
                           sizeof (objStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyPerfInStats_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: The status of this conceptual row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyPerfInStats_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  xLibU32_t tempIntIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: ifIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyPerfInStats_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa3.len);

  if( usmDbIntIfNumFromExtIfNum(keyifIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyPerfInRowStatusGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           tempIntIfIndexValue, &objRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
