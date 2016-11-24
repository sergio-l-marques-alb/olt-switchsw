/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServServiceStats.c
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
#include "_xe_qosDiffServServiceStats_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_util_api.h"

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_IfIndex_IfDirection
*
* @purpose Get 'IfIndex + IfDirection +'
*
* @description [IfIndex]: External interface number of the interface to which
*              policy is assigned. 
*              [IfDirection]: Interface direction to which the policy is assigned.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_IfIndex_IfDirection (void *wap,
                                                               void *bufp[],
                                                               xLibU16_t
                                                               keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue, nextObjIfIndexValue;
  fpObjWa_t owaIfDirection = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfDirectionValue, nextObjIfDirectionValue;

  xLibU32_t tempIntIfIndexValue;
  xLibU32_t tempNextIntIfIndexValue;

  void *outIfIndex = (void *) bufp[--keyCount];
  void *outIfDirection = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outIfIndex);
  FPOBJ_TRACE_ENTER (outIfDirection);

  /* retrieve key: IfIndex */
  owaIfIndex.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                                 (xLibU8_t *) & objIfIndexValue,
                                 &owaIfIndex.len);
  if (owaIfIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: IfDirection */
    owaIfDirection.rc =
      xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                     (xLibU8_t *) & objIfDirectionValue, &owaIfDirection.len);
  }
  else
  {
    objIfIndexValue = 0;
    objIfDirectionValue = 0;
    tempIntIfIndexValue = 0;
    tempNextIntIfIndexValue = 0;
  }
	
  FPOBJ_TRACE_CURRENT_KEY (outIfIndex, &objIfIndexValue, owaIfIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outIfDirection, &objIfDirectionValue,
                           owaIfDirection.len);
  if( usmDbIntIfNumFromExtIfNum(objIfIndexValue,&tempIntIfIndexValue) != L7_SUCCESS )
  {
       owa.rc = XLIBRC_FAILURE;
	FPOBJ_TRACE_EXIT (outIfIndex, owaIfIndex);
	FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
	return owa.rc;
  }
  owa.rc =
    usmDbDiffServServiceGetNext (L7_UNIT_CURRENT, tempIntIfIndexValue, objIfDirectionValue,
                         &tempNextIntIfIndexValue, &nextObjIfDirectionValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outIfIndex, owaIfIndex);
    FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
    return owa.rc;
  }

  if( usmDbExtIfNumFromIntIfNum(tempNextIntIfIndexValue,&nextObjIfIndexValue) != L7_SUCCESS )
  {
       owa.rc = XLIBRC_FAILURE;
	FPOBJ_TRACE_EXIT (outIfIndex, owaIfIndex);
	FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
	return owa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (outIfIndex, &nextObjIfIndexValue, owaIfIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outIfDirection, &nextObjIfDirectionValue,
                           owaIfDirection.len);

  /* return the object value: IfIndex */
  xLibBufDataSet (outIfIndex,
                  (xLibU8_t *) & nextObjIfIndexValue,
                  sizeof (nextObjIfIndexValue));

  /* return the object value: IfDirection */
  xLibBufDataSet (outIfDirection,
                  (xLibU8_t *) & nextObjIfDirectionValue,
                  sizeof (nextObjIfDirectionValue));
  FPOBJ_TRACE_EXIT (outIfIndex, owaIfIndex);
  FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
  return XLIBRC_SUCCESS;
}

#endif

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_IfIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex] External interface number of the interface to which policy is assigned.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_IfIndex (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  xLibU32_t objIfDirectionValue;
  xLibU32_t nextObjIfDirectionValue;
  xLibU32_t policyIndex;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIfIndexValue = objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServServiceGetNext(L7_UNIT_CURRENT,
                                     objIfIndexValue,
                                     objIfDirectionValue, &nextObjIfIndexValue,
                                     &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    objIfDirectionValue = 0;
	 nextObjIfDirectionValue = objIfDirectionValue;
    do
    {
      objIfDirectionValue = nextObjIfDirectionValue;
      owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
                                      objIfIndexValue,
                                      objIfDirectionValue, &nextObjIfIndexValue,
                                      &nextObjIfDirectionValue);
	  if(owa.l7rc == L7_SUCCESS)
	  {
	       if ( (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
				 	       nextObjIfIndexValue, nextObjIfDirectionValue, &policyIndex) != L7_SUCCESS) ||
				 	 (usmDbDiffServPolicyGet(L7_UNIT_CURRENT, policyIndex) != L7_SUCCESS) )
	       {
				 	 owa.l7rc = L7_FAILURE;
	       }
	  }
    }
    while ((objIfIndexValue == nextObjIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);
 

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue, sizeof (nextObjIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_IfPhyIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex] External interface number of the interface to which policy is assigned.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_IfPhyIndex (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  xLibU32_t objIfDirectionValue;
  xLibU32_t nextObjIfDirectionValue;
  xLibU32_t policyIndex;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfPhyIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIfIndexValue = objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServServiceGetNext(L7_UNIT_CURRENT,
                                     objIfIndexValue,
                                     objIfDirectionValue, &nextObjIfIndexValue,
                                     &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    objIfDirectionValue = 0;
   nextObjIfDirectionValue = objIfDirectionValue;
    do
    {
      objIfDirectionValue = nextObjIfDirectionValue;
      owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
                                      objIfIndexValue,
                                      objIfDirectionValue, &nextObjIfIndexValue,
                                      &nextObjIfDirectionValue);
    if(owa.l7rc == L7_SUCCESS)
    {
         if ( (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
                 nextObjIfIndexValue, nextObjIfDirectionValue, &policyIndex) != L7_SUCCESS) ||
           (usmDbDiffServPolicyGet(L7_UNIT_CURRENT, policyIndex) != L7_SUCCESS) )
         {
           owa.l7rc = L7_FAILURE;
         }
    }
    }
    while ((objIfIndexValue == nextObjIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);
  if (usmDbIntIfNumTypeCheckValid(L7_UNIT_CURRENT, USM_PHYSICAL_INTF , 0, nextObjIfIndexValue) != L7_SUCCESS ){
   owa.rc = XLIBRC_FAILURE; 
   FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
 }

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue, sizeof (nextObjIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_IfLagIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex] External interface number of the interface to which policy is assigned.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_IfLagIndex (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  xLibU32_t objIfDirectionValue;
  xLibU32_t nextObjIfDirectionValue;
  xLibU32_t policyIndex;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfLagIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    /*objIfIndexValue = 1026;*/
    if (usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_LAG_INTF, 0, &objIfIndexValue) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    
    objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServServiceGetNext(L7_UNIT_CURRENT,
                                     objIfIndexValue,
                                     objIfDirectionValue, &nextObjIfIndexValue,
                                     &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    objIfDirectionValue = 0;
	 nextObjIfDirectionValue = objIfDirectionValue;
    do
    {
      objIfDirectionValue = nextObjIfDirectionValue;
      owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
                                      objIfIndexValue,
                                      objIfDirectionValue, &nextObjIfIndexValue,
                                      &nextObjIfDirectionValue);
	  if(owa.l7rc == L7_SUCCESS)
	  {
	       if ( (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
				 	       nextObjIfIndexValue, nextObjIfDirectionValue, &policyIndex) != L7_SUCCESS) ||
				 	 (usmDbDiffServPolicyGet(L7_UNIT_CURRENT, policyIndex) != L7_SUCCESS) )
	       {
				 	 owa.l7rc = L7_FAILURE;
	       }
	  }
    }
    while ((objIfIndexValue == nextObjIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);
  if (usmDbIntIfNumTypeCheckValid(L7_UNIT_CURRENT, USM_LAG_INTF , 0, nextObjIfIndexValue) != L7_SUCCESS ){
   owa.rc = XLIBRC_FAILURE; 
   FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
 }

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue, sizeof (nextObjIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_IfDirection
*
* @purpose Get 'IfDirection'
*
* @description [IfDirection] Interface direction to which the policy is assigned.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_IfDirection (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue = 0;
  xLibU32_t objIfDirectionValue = 0;
  xLibU32_t nextObjIfDirectionValue = 0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);

  /* retrieve key: IfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                          (xLibU8_t *) & objIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
                                     objIfIndexValue,
                                     objIfDirectionValue, &nextObjIfIndexValue,
                                     &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfDirectionValue, owa.len);

    owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
                                    objIfIndexValue,
                                    objIfDirectionValue, &nextObjIfIndexValue,
                                    &nextObjIfDirectionValue);

  }

  if ((objIfIndexValue != nextObjIfIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfDirectionValue, owa.len);

  /* return the object value: IfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfDirectionValue,
                           sizeof (objIfDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfOfferedOctets
*
* @purpose Get 'PerfOfferedOctets'
*
* @description [PerfOfferedOctets]: Offered octets count for the specified
*              service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfOfferedOctets (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfOfferedOctetsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                              keyIfDirectionValue,
                                              &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64, sizeof(L7_ulong64));


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfOfferedPackets
*
* @purpose Get 'PerfOfferedPackets'
*
* @description [PerfOfferedPackets]: Offered packets count for specified service
*              interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfOfferedPackets (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

	FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfOfferedPacketsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                               keyIfDirectionValue, &data64.high,&data64.low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof(L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfDiscardedOctets
*
* @purpose Get 'PerfDiscardedOctets'
*
* @description [PerfDiscardedOctets]: Discarded octets count for the specified
*              service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfDiscardedOctets (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfDiscardedOctetsGet (L7_UNIT_CURRENT,
                                                keyIfIndexValue,
                                                keyIfDirectionValue,
                                                 &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfDiscardedPackets
*
* @purpose Get 'PerfDiscardedPackets'
*
* @description [PerfDiscardedPackets]: Discarded packets count for the specified
*              service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfDiscardedPackets (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfDiscardedPacketsGet (L7_UNIT_CURRENT,
                                                 keyIfIndexValue,
                                                 keyIfDirectionValue,
                                                 &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: PerfDiscardedPackets */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_qosDiffServServiceStats_MemberClassList (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMemberClassList  ;
  xLibStr256_t nextMemberClassList  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t PolicyIndexValue;
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue  ;
  xLibU32_t classIndexValueNext;
  L7_uint32  policyInstId, len = 0;

  xLibBool_t isFirstTime = XLIB_FALSE;
  xLibBool_t entryFound = XLIB_FALSE;
  L7_BOOL lookForNextAdd = L7_TRUE;


  /* retrieve key: PolicyIndex */
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);


  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);
  memset(objMemberClassList, 0x00, sizeof(
        objMemberClassList ));

  owa.rc = xLibFilterGet (wap,
      XOBJ_qosDiffServServiceStats_MemberClassList ,
      (xLibU8_t *) objMemberClassList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objMemberClassList, 0x00, sizeof(
          objMemberClassList));
    isFirstTime = XLIB_TRUE;
  }
  if(usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue,&PolicyIndexValue)!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   /*Get the next valid class name */
  /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to
    class name. push to fileter
   */

  if(isFirstTime == XLIB_TRUE )
  {
    classIndexValueNext = 0;
  }
  else
  {
    /*get the index of the class retrieved by the filterget */
    owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT,
        objMemberClassList,
        &classIndexValueNext);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }

  while (lookForNextAdd == L7_TRUE)
  {
    if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
          classIndexValueNext) == L7_SUCCESS)
    {
      policyInstId = 0;
      if( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT,
            PolicyIndexValue, classIndexValueNext, &policyInstId) == L7_SUCCESS )
      {
        len = sizeof(nextMemberClassList);
        memset(nextMemberClassList, 0, sizeof(
              nextMemberClassList));
        usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,
            nextMemberClassList, &len);
        entryFound = XLIB_TRUE;
        break;
      }
    }
    else
    {
      lookForNextAdd = L7_FALSE;
    }
  }

  if(entryFound != XLIB_TRUE )
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextMemberClassList,
      len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED
TO SEE IF WE CAN REMOVE THIS OBJECT LATER*/

xLibRC_t fpObjSet_qosDiffServServiceStats_MemberClassList(void *wap, void
*bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMemberClassList;

  xLibU32_t PolicyIndexValue;

  L7_uint32  policyInstId, classIndexValueTemp;
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);


  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  if(usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue,&PolicyIndexValue)!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* retrieve object: AddToMemberClassList */
  memset(objMemberClassList, 0x00, sizeof(objMemberClassList));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMemberClassList, &
owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMemberClassList, owa.len);

  owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT,
                                       objMemberClassList,
                                      &classIndexValueTemp);
  if (owa.l7rc != L7_SUCCESS)
   {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  if (usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT,
      PolicyIndexValue,
      classIndexValueTemp, &policyInstId) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*********************************************************************
*
* @purpose Get the policy instance index associated with the
*          specified policy index and class index
*
* @param unit
*
* @param policyIndex
*
* @param classIndex
*
* @param *policyInstIndex
*
* @returns option
*
* @end
*
*********************************************************************/
L7_RC_t diffServUtilPolicyInstGet(L7_uint32 unit,
                                    L7_uint32 policyIndex,
                                    L7_uint32 classIndex,
                                    L7_uint32 * policyInstIndex)
{
  L7_uint32 nextPolicyIndex;
  L7_uint32 prevPolicyInstIndex;
  L7_uint32 nextPolicyInstIndex;
  L7_uint32 tempClassIndex;
  L7_RC_t rc;

  prevPolicyInstIndex = 0;
  tempClassIndex = 0;
  rc = usmDbDiffServPolicyInstGetNext(unit, policyIndex, prevPolicyInstIndex, 
                                &nextPolicyIndex, &nextPolicyInstIndex);
  while ((rc == L7_SUCCESS) && (tempClassIndex != classIndex) && 
                            (policyIndex == nextPolicyIndex))
  {
    rc = usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex, 
                             nextPolicyInstIndex, &tempClassIndex);
    if (rc == L7_SUCCESS)
    {
      if (tempClassIndex == classIndex)
      {
        *policyInstIndex = nextPolicyInstIndex;
        break;
      }
      else
      {
        prevPolicyInstIndex = nextPolicyInstIndex;
        rc = usmDbDiffServPolicyInstGetNext(unit, policyIndex, 
             prevPolicyInstIndex, &nextPolicyIndex, &nextPolicyInstIndex);
      }
    }
  }

  if (tempClassIndex == classIndex)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}
/* THIS HANDLER IS IMPLEMENTED AS PART OF TEMPORARY METHOD TO DEVELOP 
   DIFFSERV SERVICE STATS DETAIL PAGE*/ 
/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_OfferedOctets
*
* @purpose Get 'PerfOfferedOctets'
*
* @description [OfferedOctets]: Offered octets count for the specified
*              service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_OfferedOctets (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t memberClass;  
  
  xLibU32_t  policyIndex, policyInstIndex, classIndex;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  

  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_MemberClassList,
                           (xLibU8_t *) memberClass, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, memberClass, kwa3.len);


  if ((usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, memberClass , 
                                    &classIndex) != L7_SUCCESS) ||
      (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue, 
               keyIfDirectionValue, &policyIndex) != L7_SUCCESS) ||
      (diffServUtilPolicyInstGet(L7_UNIT_CURRENT, policyIndex, 
                                 classIndex, &policyInstIndex)!= L7_SUCCESS))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 

  if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_IN)
  {
  /* get the value from application */
  owa.l7rc =

    usmDbDiffServPolicyPerfInOfferedOctetsGet  (L7_UNIT_CURRENT, policyIndex, 
                                                policyInstIndex, 
                                                keyIfIndexValue, &data64.high, &data64.low);
  }
  else if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_OUT)
  {
    owa.l7rc =

    usmDbDiffServPolicyPerfOutOfferedOctetsGet  (L7_UNIT_CURRENT, policyIndex,
                                                policyInstIndex,
                                                keyIfIndexValue, &data64.high, &data64.low);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64, sizeof(L7_ulong64));


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/* THIS HANDLER IS IMPLEMENTED AS PART OF TEMPORARY METHOD TO DEVELOP
   DIFFSERV SERVICE STATS DETAIL PAGE*/
/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_OfferedPackets
*
* @purpose Get 'OfferedPackets'
*
* @description [OfferedPackets]: Offered packets count for specified service
*              interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_OfferedPackets (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t memberClass;

  xLibU32_t  policyIndex, policyInstIndex, classIndex;


	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

	FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_MemberClassList,
                           (xLibU8_t *) memberClass, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, memberClass, kwa3.len);
  
  if ((usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, memberClass ,
                                    &classIndex) != L7_SUCCESS) ||
      (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue,
               keyIfDirectionValue, &policyIndex) != L7_SUCCESS) ||
      (diffServUtilPolicyInstGet(L7_UNIT_CURRENT, policyIndex,
                                 classIndex, &policyInstIndex)!= L7_SUCCESS))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_IN)
  {
  /* get the value from application */
  owa.l7rc =

    usmDbDiffServPolicyPerfInOfferedPacketsGet (L7_UNIT_CURRENT, policyIndex,
                                                policyInstIndex,
                                                keyIfIndexValue, &data64.high, &data64.low);
  }
  else if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_OUT)
  {

    owa.l7rc =

    usmDbDiffServPolicyPerfOutOfferedPacketsGet (L7_UNIT_CURRENT, policyIndex,
                                                policyInstIndex,
                                                keyIfIndexValue, &data64.high, &data64.low);


  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof(L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/* THIS HANDLER IS IMPLEMENTED AS PART OF TEMPORARY METHOD TO DEVELOP
   DIFFSERV SERVICE STATS DETAIL PAGE*/

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_DiscardedOctets
*
* @purpose Get 'DiscardedOctets'
*
* @description [DiscardedOctets]: Discarded octets count for the specified
*              service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_DiscardedOctets (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));


  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t memberClass;

  xLibU32_t  policyIndex, policyInstIndex, classIndex;
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

 
  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_MemberClassList,
                           (xLibU8_t *) memberClass, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, memberClass, kwa3.len);


  if ((usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, memberClass ,
                                    &classIndex) != L7_SUCCESS) ||
      (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue,
               keyIfDirectionValue, &policyIndex) != L7_SUCCESS) ||

      (diffServUtilPolicyInstGet(L7_UNIT_CURRENT, policyIndex,
                                 classIndex, &policyInstIndex)!= L7_SUCCESS))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

  if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_IN)
  {
  
  /* get the value from application */
  owa.l7rc =

    usmDbDiffServPolicyPerfInDiscardedOctetsGet (L7_UNIT_CURRENT, policyIndex,
                                                policyInstIndex,
                                                keyIfIndexValue, &data64.high, &data64.low);

  }
  else if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_OUT)
  {
    owa.l7rc =

    usmDbDiffServPolicyPerfOutDiscardedOctetsGet (L7_UNIT_CURRENT, policyIndex,
                                                policyInstIndex,
                                                keyIfIndexValue, &data64.high, &data64.low);

  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/* THIS HANDLER IS IMPLEMENTED AS PART OF TEMPORARY METHOD TO DEVELOP
   DIFFSERV SERVICE STATS DETAIL PAGE*/

/*******************************************************************************
 * @function fpObjGet_qosDiffServServiceStats_PolicyIndex
*
 * @purpose Get 'PolicyIndex'
 *
 * @description [PolicyIndex]: Index of the Policy Table row whose policy definition
 *              is attached to the interface in the specified direction.
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PolicyIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyIndexValue;
 
  FPOBJ_TRACE_ENTER (bufp);

  keyIfIndexValue = keyIfDirectionValue = objPolicyIndexValue = 0;
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePolicyIndexGet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue,
        &objPolicyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyIndexValue,
      sizeof (objPolicyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/* THIS HANDLER IS IMPLEMENTED AS PART OF TEMPORARY METHOD TO DEVELOP
   DIFFSERV SERVICE STATS DETAIL PAGE*/

/*******************************************************************************
 * @function fpObjGet_qosDiffServServiceStats_IfOperStatus
 *
 * @purpose Get 'IfOperStatus'
 *
 * @description [IfOperStatus]: The current operational state of the DiffServ
 *              service interface. If agentDiffServGenStatusAdminMode is
 *              disable(2) then agentDiffServServiceIfOperStatus should be
 *              down(2). If agentDiffServServiceGenStatusAdminMode is ch
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_IfOperStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfOperStatusValue;
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServiceIfOperStatusGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                                keyIfDirectionValue,
        &objIfOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IfOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfOperStatusValue,
      sizeof (objIfOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED
TO SEE IF WE CAN REMOVE THIS LATER*/

xLibRC_t fpObjGet_qosDiffServServiceStats_PolicyName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t selPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  xLibU32_t tempPolicyIndex, len = 0;
  xLibStr256_t objPolicyNameList;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));

  owa.l7rc = usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT,
      keyIfIndexValue, (L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t) keyIfDirectionValue, &tempPolicyIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
    len = sizeof(selPolicyName);
    memset(selPolicyName, 0x00, sizeof(selPolicyName));
    owa.l7rc = usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT,tempPolicyIndex, selPolicyName, &len);
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) selPolicyName, len);
    FPOBJ_TRACE_EXIT (bufp, owa);
    owa.rc = XLIBRC_SUCCESS;
    return owa.rc;
  }
  return XLIBRC_FAILURE;

}
/* THIS HANDLER IS IMPLEMENTED AS PART OF TEMPORARY METHOD TO DEVELOP
   DIFFSERV SERVICE STATS DETAIL PAGE*/

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_DiscardedPackets
*
* @purpose Get 'DiscardedPackets'
*
* @description [DiscardedPackets]: Discarded packets count for the specified
*              service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_DiscardedPackets (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t memberClass;

  xLibU32_t  policyIndex, policyInstIndex, classIndex;

  L7_ulong64 data64;

  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_MemberClassList,
                           (xLibU8_t *) memberClass, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, memberClass, kwa3.len);
  

  if ((usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, memberClass ,
                                    &classIndex) != L7_SUCCESS) ||
      (usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue,
               keyIfDirectionValue, &policyIndex) != L7_SUCCESS) ||
      (diffServUtilPolicyInstGet(L7_UNIT_CURRENT, policyIndex,
                                 classIndex, &policyInstIndex)!= L7_SUCCESS))
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_IN)
  {
  /* get the value from application */
  owa.l7rc =

    usmDbDiffServPolicyPerfInDiscardedPacketsGet(L7_UNIT_CURRENT, policyIndex,
                                                 policyInstIndex,
                                                 keyIfIndexValue, &data64.high, &data64.low);

  }
  else if (keyIfDirectionValue == L7_DIFFSERV_IF_DIRECTION_OUT)
  {
     /* get the value from application */
  owa.l7rc =

    usmDbDiffServPolicyPerfOutDiscardedPacketsGet(L7_UNIT_CURRENT, policyIndex,
                                                 policyInstIndex,
                                                 keyIfIndexValue, &data64.high, &data64.low);

  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
  /* return the object value: PerfDiscardedPackets */
  
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfSentOctets
*
* @purpose Get 'PerfSentOctets'
*
* @description [PerfSentOctets]: Sent octets count for the specified service
*              interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfSentOctets (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfSentOctetsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                           keyIfDirectionValue,
                                            &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PerfSentOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfSentPackets
*
* @purpose Get 'PerfSentPackets'
*
* @description [PerfSentPackets]: Sent packets count for the specified service
*              interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfSentPackets (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfSentPacketsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                            keyIfDirectionValue,
                                            &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PerfSentPackets */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfHCOfferedOctets
*
* @purpose Get 'PerfHCOfferedOctets'
*
* @description [PerfHCOfferedOctets]: Offered octets high capacity count for
*              the specified service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfHCOfferedOctets (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  FPOBJ_TRACE_ENTER (bufp);
  
  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfOfferedOctetsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                              keyIfDirectionValue,
                                             &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfHCOfferedPackets
*
* @purpose Get 'PerfHCOfferedPackets'
*
* @description [PerfHCOfferedPackets]: Offered packets high capacity count
*              for the specified service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfHCOfferedPackets (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfOfferedPacketsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                               keyIfDirectionValue,
                                               &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PerfHCOfferedPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfHCDiscardedOctets
*
* @purpose Get 'PerfHCDiscardedOctets'
*
* @description [PerfHCDiscardedOctets]: Discarded octets high capacity count
*              for the specified service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfHCDiscardedOctets (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfDiscardedOctetsGet (L7_UNIT_CURRENT,
                                                keyIfIndexValue,
                                                keyIfDirectionValue,
                                                &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfHCDiscardedPackets
*
* @purpose Get 'PerfHCDiscardedPackets'
*
* @description [PerfHCDiscardedPackets]: Discarded packets high capacity count
*              for the specified service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfHCDiscardedPackets (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfDiscardedPacketsGet (L7_UNIT_CURRENT,
                                                 keyIfIndexValue,
                                                 keyIfDirectionValue,
                                                 &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PerfHCDiscardedPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfHCSentOctets
*
* @purpose Get 'PerfHCSentOctets'
*
* @description [PerfHCSentOctets]: Sent octets high capacity count for the
*              specified service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfHCSentOctets (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfSentOctetsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                           keyIfDirectionValue,                
                                           &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PerfHCSentOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServServiceStats_PerfHCSentPackets
*
* @purpose Get 'PerfHCSentPackets'
*
* @description [PerfHCSentPackets]: Sent packets high capacity count for the
*              specified service interface and direction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServServiceStats_PerfHCSentPackets (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_ulong64 data64;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServServiceStats_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePerfSentPacketsGet (L7_UNIT_CURRENT, keyIfIndexValue,
                                            keyIfDirectionValue,
                                           &data64.high,&data64.low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PerfHCSentPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &data64,
                           sizeof (L7_ulong64));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
