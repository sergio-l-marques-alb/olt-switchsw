
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingVrrpTrackIntfTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  02 June 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingVrrpTrackIntfTable_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackIntfTable_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] A unique value, greater than zero, for each interface.
* It is recommended that values are assigned contiguously starting
* from 1. The value for each interface sub-layer must remain
* constant at least from one re-initialization of the entity's network
* management system to the next re-initialization.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackIntfTable_ifIndex (void *wap, void *bufp)
{
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t nextObjVrrpTrackIntfValue;
  xLibU8_t  nextObjVrrpOperVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjifIndexValue = 0;
	objifIndexValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
	
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

    nextObjifIndexValue = objifIndexValue;

    nextObjVrrpTrackIntfValue = 0;
    memset (&nextObjVrrpOperVrIdValue, 0, sizeof (nextObjVrrpOperVrIdValue));
    do
    {
      owa.l7rc = usmDbVrrpOperTrackIntfNextGet (L7_UNIT_CURRENT,
                                                                       (xLibU8_t*) &nextObjVrrpOperVrIdValue,
                                                                        &nextObjifIndexValue,
                                                                        &nextObjVrrpTrackIntfValue);
    }
    while ((objifIndexValue == nextObjifIndexValue) && (owa.l7rc == L7_SUCCESS));

  if ((owa.l7rc != L7_SUCCESS)   )
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackIntfTable_VrrpOperVrId
*
* @purpose Get 'VrrpOperVrId'
 *@description  [VrrpOperVrId] This object contains the Virtual Router
* Identifier (VrrpOperVrId).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackIntfTable_VrrpOperVrId (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;

  xLibU32_t nextObjVrrpTrackIntfValue;
  xLibU32_t vrId = 0;
  xLibU8_t  objVrrpOperVrIdValue;
  xLibU8_t  nextObjVrrpOperVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (objifIndexValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, kwa.len);

  /* retrieve key: VrrpOperVrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                          (xLibU8_t *) & vrId, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjifIndexValue = objifIndexValue;
    nextObjVrrpTrackIntfValue = 0;
    
    memset (&objVrrpOperVrIdValue, 0, sizeof (objVrrpOperVrIdValue));
    memset (&nextObjVrrpOperVrIdValue, 0, sizeof (nextObjVrrpOperVrIdValue));

    owa.l7rc = usmDbVrrpOperTrackIntfNextGet (L7_UNIT_CURRENT,
                                                                    (xLibU8_t*)  &nextObjVrrpOperVrIdValue,
                                                                      &nextObjifIndexValue,
                                                                      &nextObjVrrpTrackIntfValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpOperVrIdValue, owa.len);

    memset (&nextObjVrrpTrackIntfValue, 0, sizeof (nextObjVrrpTrackIntfValue));

    nextObjifIndexValue = objifIndexValue;

    objVrrpOperVrIdValue = (xLibU8_t)vrId;
    memcpy(&nextObjVrrpOperVrIdValue,&objVrrpOperVrIdValue,sizeof(nextObjVrrpOperVrIdValue));
    do
    {
      owa.l7rc = usmDbVrrpOperTrackIntfNextGet (L7_UNIT_CURRENT,
                                                                      (xLibU8_t*)  &nextObjVrrpOperVrIdValue,
                                                                        &nextObjifIndexValue,
                                                                        &nextObjVrrpTrackIntfValue);
                                                                     
    }
    while ((objifIndexValue == nextObjifIndexValue)
           && (objVrrpOperVrIdValue == nextObjVrrpOperVrIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objifIndexValue != nextObjifIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrrpOperVrIdValue, owa.len);

  vrId = nextObjVrrpOperVrIdValue;
  /* return the object value: VrrpOperVrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & vrId,
                           sizeof (vrId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIntf
*
* @purpose Get 'VrrpTrackIntf'
 *@description  [VrrpTrackIntf] This object specifies the interface to be
* tracked.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIntf (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;

  xLibU32_t nextObjifIndexValue;
  xLibU32_t nextObjVrrpTrackIntfValue;
  xLibU32_t objVrrpTrackIntfValue;
  xLibU8_t  nextObjVrrpOperVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);
  
  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                       (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  nextObjifIndexValue = keyifIndexValue;
  nextObjVrrpOperVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;

  /* retrieve key: VrrpTrackIntf */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntf,
                          (xLibU8_t *) & objVrrpTrackIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objVrrpTrackIntfValue, 0, sizeof (objVrrpTrackIntfValue));
    nextObjVrrpTrackIntfValue = objVrrpTrackIntfValue;
    owa.l7rc = usmDbVrrpOperTrackIntfNextGet (L7_UNIT_CURRENT,
                                              (xLibU8_t*)  &nextObjVrrpOperVrIdValue,
                                              &nextObjifIndexValue,
                                              &nextObjVrrpTrackIntfValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpTrackIntfValue, owa.len);

    nextObjVrrpTrackIntfValue = objVrrpTrackIntfValue;
    
    owa.l7rc = usmDbVrrpOperTrackIntfNextGet (L7_UNIT_CURRENT,
                                              (xLibU8_t*)  &nextObjVrrpOperVrIdValue,
                                              &nextObjifIndexValue,
                                              &nextObjVrrpTrackIntfValue);
  }

  if ((keyifIndexValue != nextObjifIndexValue)
      || ((xLibU8_t)keyVrrpOperVrIdValue != nextObjVrrpOperVrIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrrpTrackIntfValue, owa.len);

  /* return the object value: VrrpTrackIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVrrpTrackIntfValue,
                           sizeof (nextObjVrrpTrackIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIfPrioDec
*
* @purpose Get 'VrrpTrackIfPrioDec'
 *@description  [VrrpTrackIfPrioDec] This object specifies the decrement
* priority of the VRRP router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIfPrioDec (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpTrackIntf = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackIntfValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU8_t objVrrpTrackIfPrioDecValue;
  xLibU32_t objPrioDecValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpTrackIntf */
  kwaVrrpTrackIntf.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntf,
                                       (xLibU8_t *) & keyVrrpTrackIntfValue, &kwaVrrpTrackIntf.len);
  if (kwaVrrpTrackIntf.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackIntf.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackIntf);
    return kwaVrrpTrackIntf.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackIntfValue, kwaVrrpTrackIntf.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperTrackIntfPrioGet (L7_UNIT_CURRENT, objVrIdValue,
                                                                  keyifIndexValue,keyVrrpTrackIntfValue,
                                                                 (xLibU8_t*) &objVrrpTrackIfPrioDecValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objPrioDecValue = objVrrpTrackIfPrioDecValue;
  /* return the object value: VrrpTrackIfPrioDec */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPrioDecValue,
                           sizeof (objPrioDecValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingVrrpTrackIntfTable_VrrpTrackIfPrioDec
*
* @purpose Set 'VrrpTrackIfPrioDec'
 *@description  [VrrpTrackIfPrioDec] This object specifies the decrement
* priority of the VRRP router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingVrrpTrackIntfTable_VrrpTrackIfPrioDec (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackIfPrioDecValue;

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpTrackIntf = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackIntfValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VrrpTrackIfPrioDec */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVrrpTrackIfPrioDecValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVrrpTrackIfPrioDecValue, owa.len);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpTrackIntf */
  kwaVrrpTrackIntf.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntf,
                                       (xLibU8_t *) & keyVrrpTrackIntfValue, &kwaVrrpTrackIntf.len);
  if (kwaVrrpTrackIntf.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackIntf.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackIntf);
    return kwaVrrpTrackIntf.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackIntfValue, kwaVrrpTrackIntf.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  /* set the value in application */
  if (usmDbVrrpVrIdIntfTrackIntfGet(L7_UNIT_CURRENT, objVrIdValue, 
                                                    keyifIndexValue, keyVrrpTrackIntfValue ) == L7_SUCCESS)
  {
    if (usmDbVrrpOperTrackIntfAdd(L7_UNIT_CURRENT, objVrIdValue, keyifIndexValue, 
          keyVrrpTrackIntfValue,(L7_uchar8) objVrrpTrackIfPrioDecValue) != L7_SUCCESS)
    {
      owa.l7rc = XLIBRC_FAILURE;    /* TODO: Change if required */
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
* @function fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIfState
*
* @purpose Get 'VrrpTrackIfState'
 *@description  [VrrpTrackIfState] This object specifies the IP state of the
* tracked interface for the virtual router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIfState (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpTrackIntf = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackIntfValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackIfStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpTrackIntf */
  kwaVrrpTrackIntf.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntf,
                                       (xLibU8_t *) & keyVrrpTrackIntfValue, &kwaVrrpTrackIntf.len);
  if (kwaVrrpTrackIntf.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackIntf.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackIntf);
    return kwaVrrpTrackIntf.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackIntfValue, kwaVrrpTrackIntf.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperTrackIntfStateGet (L7_UNIT_CURRENT, objVrIdValue,
                                                                    keyifIndexValue, keyVrrpTrackIntfValue,
                                                                    &objVrrpTrackIfStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VrrpTrackIfState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrrpTrackIfStateValue,
                           sizeof (objVrrpTrackIfStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIfStatus
*
* @purpose Get 'VrrpTrackIfStatus'
 *@description  [VrrpTrackIfStatus] The row status variable, used according to
* installation and removal conventions for conceptual rows. Setting
* this object to active(1) or createAndGo(4) results in the
* addition of an associated address for a virtual router. Destroying the
* entry or setting it to notInService(2) removes the associated
* address from the virtual router. The use of other values is
* implementation-dependent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIfStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpTrackIntf = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackIntfValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackIfStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpTrackIntf */
  kwaVrrpTrackIntf.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntf,
                                       (xLibU8_t *) & keyVrrpTrackIntfValue, &kwaVrrpTrackIntf.len);
  if (kwaVrrpTrackIntf.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackIntf.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackIntf);
    return kwaVrrpTrackIntf.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackIntfValue, kwaVrrpTrackIntf.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  /* get the value from application */
  if (usmDbVrrpVrIdIntfTrackIntfGet(L7_UNIT_CURRENT, objVrIdValue, 
                                                    keyifIndexValue, keyVrrpTrackIntfValue) == L7_SUCCESS)
  {
    objVrrpTrackIfStatusValue = L7_ROW_STATUS_ACTIVE;
    owa.l7rc = L7_SUCCESS;
  }  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VrrpTrackIfStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrrpTrackIfStatusValue,
                           sizeof (objVrrpTrackIfStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingVrrpTrackIntfTable_VrrpTrackIfStatus
*
* @purpose Set 'VrrpTrackIfStatus'
 *@description  [VrrpTrackIfStatus] The row status variable, used according to
* installation and removal conventions for conceptual rows. Setting
* this object to active(1) or createAndGo(4) results in the
* addition of an associated address for a virtual router. Destroying the
* entry or setting it to notInService(2) removes the associated
* address from the virtual router. The use of other values is
* implementation-dependent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingVrrpTrackIntfTable_VrrpTrackIfStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackIfStatusValue;

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpTrackIntf = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackIntfValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwaVrrpTrackIfPrioDec = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackIfPrioDecValue;
  xLibU32_t objVrrpTrackIfPrioDecValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VrrpTrackIfStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVrrpTrackIfStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVrrpTrackIfStatusValue, owa.len);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpTrackIntf */
  kwaVrrpTrackIntf.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntf,
                                       (xLibU8_t *) & keyVrrpTrackIntfValue, &kwaVrrpTrackIntf.len);
  if (kwaVrrpTrackIntf.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackIntf.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackIntf);
    return kwaVrrpTrackIntf.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackIntfValue, kwaVrrpTrackIntf.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve object: VrrpTrackIfPrioDec */
  kwaVrrpTrackIfPrioDec.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIfPrioDec,
		                            (xLibU8_t *) & keyVrrpTrackIfPrioDecValue, &kwaVrrpTrackIfPrioDec.len);
  if (kwaVrrpTrackIfPrioDec.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackIfPrioDec.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackIfPrioDec);
    return kwaVrrpTrackIfPrioDec.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackIfPrioDecValue,kwaVrrpTrackIfPrioDec.len);

  owa.l7rc = L7_SUCCESS;
  
  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  objVrrpTrackIfPrioDecValue = (xLibU8_t)keyVrrpTrackIfPrioDecValue;
  if ((objVrrpTrackIfStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (objVrrpTrackIfStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT))
  {

    if (usmDbVrrpOperEntryGet(USMDB_UNIT_CURRENT, objVrIdValue, keyifIndexValue) == L7_SUCCESS)
    {
      if (usmDbVrrpVrIdIntfTrackIntfGet(USMDB_UNIT_CURRENT, objVrIdValue, keyifIndexValue,
                                                        keyVrrpTrackIntfValue) != L7_SUCCESS)
      {
        owa.l7rc = usmDbVrrpOperTrackIntfAdd (L7_UNIT_CURRENT, 
                                                                    objVrIdValue,
                                                                    keyifIndexValue,
                                                                    keyVrrpTrackIntfValue,
                                                                    objVrrpTrackIfPrioDecValue);
      }
    }
  }
  else if (objVrrpTrackIfStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbVrrpOperTrackIntfDelete (L7_UNIT_CURRENT, 
                                                                    objVrIdValue,
                                                                    keyifIndexValue,
                                                                    keyVrrpTrackIntfValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjList_routingVrrpTrackIntfTable_VrrpTrackIntfPortList (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpTrackIntfPortList,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objifIndexValue = 0;
    nextObjifIndexValue = 0; 
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF,
                                             0, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
	nextObjifIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF,
                                        0, objifIndexValue, &nextObjifIndexValue);
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  do
  {
    if(usmDbVrrpVrIdIntfTrackIntfGet(
			           L7_UNIT_CURRENT, 
			           objVrIdValue, 
			           keyifIndexValue, 
			           nextObjifIndexValue) != L7_SUCCESS)
    {
       owa.l7rc = L7_SUCCESS;
       break;
    }
    else
    {
       owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF,
                                           0, nextObjifIndexValue, &nextObjifIndexValue);
    }
  }
  while(owa.l7rc == L7_SUCCESS);


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_routingVrrpTrackIntfTable_VrrpTrackIntfPortList (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackIntfTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* get the value from application */
  owa.l7rc =
      usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF,
                                0, &objIfIndexValue);

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  do
  {
    if(usmDbVrrpVrIdIntfTrackIntfGet(
			           L7_UNIT_CURRENT, 
			           objVrIdValue, 
			           keyifIndexValue, 
			           objIfIndexValue) != L7_SUCCESS)
    {
       owa.l7rc = L7_SUCCESS;
       break;
    }
    else
    {
       owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF,
                                           0, objIfIndexValue, &objIfIndexValue);
    }
  }
  while(owa.l7rc == L7_SUCCESS);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfIndexValue,
                           sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjSet_routingVrrpTrackIntfTable_VrrpTrackIntfPortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifIndex */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifIndexValue, owa.len);

  return XLIBRC_SUCCESS;
}


