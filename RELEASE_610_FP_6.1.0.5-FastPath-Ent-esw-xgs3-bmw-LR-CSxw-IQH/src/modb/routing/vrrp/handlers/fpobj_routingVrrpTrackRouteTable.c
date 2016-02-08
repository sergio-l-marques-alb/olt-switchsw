
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingVrrpTrackRouteTable.c
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
#include "_xe_routingVrrpTrackRouteTable_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_vrrp_api.h"

#define VRRP_TRACK_ROUTE_PFX_LEN_MIN 1
#define VRRP_TRACK_ROUTE_PFX_LEN_MAX 32

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackRouteTable_ifIndex
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
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_ifIndex (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t nextObjVrrpOperVrIdValue;
  xLibIpV4_t nextObjVrrpTrackRtPfxValue;
  xLibU32_t nextObjVrrpTrackRtPfxLenValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
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
    nextObjVrrpOperVrIdValue = 0;
    nextObjVrrpTrackRtPfxValue = 0;
    nextObjVrrpTrackRtPfxLenValue = 0;
    
    do
    {
      owa.l7rc = usmDbVrrpOperTrackRouteNextGet (L7_UNIT_CURRENT,
                                                                            &nextObjVrrpOperVrIdValue, 
                                                                            &nextObjifIndexValue,
                                                                            &nextObjVrrpTrackRtPfxValue,
                                                                            &nextObjVrrpTrackRtPfxLenValue);

    }
    while ((objifIndexValue == nextObjifIndexValue) && (owa.l7rc == L7_SUCCESS));

  if ((owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingVrrpTrackRouteTable_VrrpOperVrId
*
* @purpose Get 'VrrpOperVrId'
 *@description  [VrrpOperVrId] This object contains the Virtual Router
* Identifier (VrrpOperVrId).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_VrrpOperVrId (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t objVrrpOperVrIdValue;
  xLibU8_t nextObjVrrpOperVrIdValue;
  xLibIpV4_t objVrrpTrackRtPfxValue;
  xLibIpV4_t nextObjVrrpTrackRtPfxValue;
  xLibU32_t objVrrpTrackRtPfxLenValue;
  xLibU32_t nextObjVrrpTrackRtPfxLenValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrrpOperVrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                          (xLibU8_t *) & objVrrpOperVrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjifIndexValue = objifIndexValue;    
    nextObjVrrpTrackRtPfxValue = objVrrpTrackRtPfxValue = 0;
    nextObjVrrpTrackRtPfxLenValue = objVrrpTrackRtPfxLenValue = 0;
    memset (&objVrrpOperVrIdValue, 0, sizeof (objVrrpOperVrIdValue));
    memset (&nextObjVrrpOperVrIdValue, 0, sizeof (nextObjVrrpOperVrIdValue));
  }
	
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpOperVrIdValue, owa.len);
    nextObjifIndexValue = objifIndexValue;
    nextObjVrrpOperVrIdValue = (xLibU8_t)objVrrpOperVrIdValue;
    nextObjVrrpTrackRtPfxValue = objVrrpTrackRtPfxValue = 0;
    nextObjVrrpTrackRtPfxLenValue = objVrrpTrackRtPfxLenValue = 0;

    do
    {
      owa.l7rc = usmDbVrrpOperTrackRouteNextGet (L7_UNIT_CURRENT,
                                                                           &nextObjVrrpOperVrIdValue, 
                                                                           &nextObjifIndexValue,
                                                                           &nextObjVrrpTrackRtPfxValue,
                                                                           &nextObjVrrpTrackRtPfxLenValue);
    }
    while ((objifIndexValue == nextObjifIndexValue)
           && ((xLibU8_t)objVrrpOperVrIdValue == nextObjVrrpOperVrIdValue) && (owa.l7rc == L7_SUCCESS));

  if ((objifIndexValue != nextObjifIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrrpOperVrIdValue, owa.len);

  objVrrpOperVrIdValue = nextObjVrrpOperVrIdValue;
  /* return the object value: VrrpOperVrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrrpOperVrIdValue,
                           sizeof (objVrrpOperVrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtPfx
*
* @purpose Get 'VrrpTrackRtPfx'
 *@description  [VrrpTrackRtPfx] This object specifies the prefix of the route
* to be tracked.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtPfx (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t objVrrpOperVrIdValue;
  xLibU8_t  nextObjVrrpOperVrIdValue;
  xLibIpV4_t objVrrpTrackRtPfxValue;
  xLibIpV4_t nextObjVrrpTrackRtPfxValue;
  xLibU32_t objVrrpTrackRtPfxLenValue;
  xLibU32_t nextObjVrrpTrackRtPfxLenValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrrpOperVrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                          (xLibU8_t *) & objVrrpOperVrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpOperVrIdValue, owa.len);

  nextObjVrrpOperVrIdValue = (xLibU8_t)objVrrpOperVrIdValue;
  nextObjifIndexValue = objifIndexValue;

  /* retrieve key: VrrpTrackRtPfx */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                          (xLibU8_t *) & objVrrpTrackRtPfxValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjVrrpTrackRtPfxValue = objVrrpTrackRtPfxValue = 0;
    nextObjVrrpTrackRtPfxLenValue = objVrrpTrackRtPfxLenValue = 0;

    owa.l7rc = usmDbVrrpOperTrackRouteNextGet (L7_UNIT_CURRENT,
                                               &nextObjVrrpOperVrIdValue, 
                                               &nextObjifIndexValue,
                                               &nextObjVrrpTrackRtPfxValue,
                                               &nextObjVrrpTrackRtPfxLenValue);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpTrackRtPfxValue, owa.len);

    nextObjVrrpTrackRtPfxValue = objVrrpTrackRtPfxValue;
    memset (&objVrrpTrackRtPfxLenValue, 0, sizeof (objVrrpTrackRtPfxLenValue));
    do
    {
      owa.l7rc = usmDbVrrpOperTrackRouteNextGet (L7_UNIT_CURRENT,
                                                 &nextObjVrrpOperVrIdValue, 
                                                 &nextObjifIndexValue,
                                                 &nextObjVrrpTrackRtPfxValue,
                                                 &nextObjVrrpTrackRtPfxLenValue);
    }
    while ((objifIndexValue == nextObjifIndexValue)
           && ((xLibU8_t)objVrrpOperVrIdValue == nextObjVrrpOperVrIdValue)
           && (objVrrpTrackRtPfxValue == nextObjVrrpTrackRtPfxValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objifIndexValue != nextObjifIndexValue) || (objVrrpOperVrIdValue != nextObjVrrpOperVrIdValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrrpTrackRtPfxValue, owa.len);

  /* return the object value: VrrpTrackRtPfx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVrrpTrackRtPfxValue,
                           sizeof (nextObjVrrpTrackRtPfxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen
*
* @purpose Get 'VrrpTrackRtPfxLen'
 *@description  [VrrpTrackRtPfxLen] This object specifies prefix length of the
* route to be tracked.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen (void *wap, void *bufp)
{
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t objVrrpOperVrIdValue;
  xLibU8_t nextObjVrrpOperVrIdValue;
  xLibIpV4_t objVrrpTrackRtPfxValue;
  xLibIpV4_t nextObjVrrpTrackRtPfxValue;
  xLibU32_t objVrrpTrackRtPfxLenValue;
  xLibU32_t nextObjVrrpTrackRtPfxLenValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: VrrpOperVrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                          (xLibU8_t *) & objVrrpOperVrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpOperVrIdValue, owa.len);

  /* retrieve key: VrrpTrackRtPfx */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                          (xLibU8_t *) & objVrrpTrackRtPfxValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpTrackRtPfxValue, owa.len);

  nextObjVrrpOperVrIdValue = (xLibU8_t)objVrrpOperVrIdValue;
  nextObjifIndexValue = objifIndexValue;
  nextObjVrrpTrackRtPfxValue = objVrrpTrackRtPfxValue;

  /* retrieve key: VrrpTrackRtPfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen,
                          (xLibU8_t *) & objVrrpTrackRtPfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjVrrpTrackRtPfxLenValue = objVrrpTrackRtPfxLenValue = 0;
    owa.l7rc = usmDbVrrpOperTrackRouteNextGet (L7_UNIT_CURRENT,
                                               &nextObjVrrpOperVrIdValue, 
                                               &nextObjifIndexValue,
                                               &nextObjVrrpTrackRtPfxValue,
                                               &nextObjVrrpTrackRtPfxLenValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrrpTrackRtPfxLenValue, owa.len);

    nextObjVrrpTrackRtPfxLenValue = objVrrpTrackRtPfxLenValue;

    owa.l7rc = usmDbVrrpOperTrackRouteNextGet (L7_UNIT_CURRENT,
                                               &nextObjVrrpOperVrIdValue, 
                                               &nextObjifIndexValue,
                                               &nextObjVrrpTrackRtPfxValue,
                                               &nextObjVrrpTrackRtPfxLenValue);
  }

  if ((objifIndexValue != nextObjifIndexValue) || ((xLibU8_t)objVrrpOperVrIdValue != nextObjVrrpOperVrIdValue)
      || (objVrrpTrackRtPfxValue != nextObjVrrpTrackRtPfxValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrrpTrackRtPfxLenValue, owa.len);

  /* return the object value: VrrpTrackRtPfxLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVrrpTrackRtPfxLenValue,
                           sizeof (nextObjVrrpTrackRtPfxLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtPrioDec
*
* @purpose Get 'VrrpTrackRtPrioDec'
 *@description  [VrrpTrackRtPrioDec] This object specifies the decrement
* priority of the VRRP router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtPrioDec (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwaVrrpTrackRtPfx = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyVrrpTrackRtPfxValue;
  fpObjWa_t kwaVrrpTrackRtPfxLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackRtPfxLenValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackRtPrioDecValue;

  xLibU8_t Priority;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve key: VrrpTrackRtPfx */
  kwaVrrpTrackRtPfx.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                                        (xLibU8_t *) & keyVrrpTrackRtPfxValue,
                                        &kwaVrrpTrackRtPfx.len);
  if (kwaVrrpTrackRtPfx.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfx.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfx);
    return kwaVrrpTrackRtPfx.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxValue, kwaVrrpTrackRtPfx.len);

  /* retrieve key: VrrpTrackRtPfxLen */
  kwaVrrpTrackRtPfxLen.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen,
                                           (xLibU8_t *) & keyVrrpTrackRtPfxLenValue,
                                           &kwaVrrpTrackRtPfxLen.len);
  if (kwaVrrpTrackRtPfxLen.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfxLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxLenValue, kwaVrrpTrackRtPfxLen.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperTrackRoutePrioGet(L7_UNIT_CURRENT, objVrIdValue, 
                                                                    keyifIndexValue, keyVrrpTrackRtPfxValue, keyVrrpTrackRtPfxLenValue, &Priority);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objVrrpTrackRtPrioDecValue = (L7_uint32)Priority;
  }

  /* return the object value: VrrpTrackRtPrioDec */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrrpTrackRtPrioDecValue,
                           sizeof (objVrrpTrackRtPrioDecValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingVrrpTrackRouteTable_VrrpTrackRtPrioDec
*
* @purpose Set 'VrrpTrackRtPrioDec'
 *@description  [VrrpTrackRtPrioDec] This object specifies the decrement
* priority of the VRRP router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingVrrpTrackRouteTable_VrrpTrackRtPrioDec (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackRtPrioDecValue;

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwaVrrpTrackRtPfx = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyVrrpTrackRtPfxValue;
  fpObjWa_t kwaVrrpTrackRtPfxLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackRtPfxLenValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VrrpTrackRtPrioDec */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVrrpTrackRtPrioDecValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVrrpTrackRtPrioDecValue, owa.len);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve key: VrrpTrackRtPfx */
  kwaVrrpTrackRtPfx.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                                        (xLibU8_t *) & keyVrrpTrackRtPfxValue,
                                        &kwaVrrpTrackRtPfx.len);
  if (kwaVrrpTrackRtPfx.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfx.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfx);
    return kwaVrrpTrackRtPfx.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxValue, kwaVrrpTrackRtPfx.len);

  /* retrieve key: VrrpTrackRtPfxLen */
  kwaVrrpTrackRtPfxLen.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen,
                                           (xLibU8_t *) & keyVrrpTrackRtPfxLenValue,
                                           &kwaVrrpTrackRtPfxLen.len);
  if (kwaVrrpTrackRtPfxLen.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfxLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxLenValue, kwaVrrpTrackRtPfxLen.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  if (usmDbVrrpVrIdIntfTrackRouteGet(L7_UNIT_CURRENT, objVrIdValue, 
                                                        keyifIndexValue, keyVrrpTrackRtPfxValue, keyVrrpTrackRtPfxLenValue) == L7_SUCCESS)
  {
    if (usmDbVrrpOperTrackRouteAdd(L7_UNIT_CURRENT, objVrIdValue,  
                                                      keyifIndexValue, keyVrrpTrackRtPfxValue, keyVrrpTrackRtPfxLenValue,
                                                      (L7_uchar8) objVrrpTrackRtPrioDecValue) != L7_SUCCESS)
    {
      return L7_FAILURE;
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
* @function fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtReachable
*
* @purpose Get 'VrrpTrackRtReachable'
 *@description  [VrrpTrackRtReachable] This object specifies the reachability of
* the tracked route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtReachable (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwaVrrpTrackRtPfx = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyVrrpTrackRtPfxValue;
  fpObjWa_t kwaVrrpTrackRtPfxLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackRtPfxLenValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackRtReachableValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve key: VrrpTrackRtPfx */
  kwaVrrpTrackRtPfx.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                                        (xLibU8_t *) & keyVrrpTrackRtPfxValue,
                                        &kwaVrrpTrackRtPfx.len);
  if (kwaVrrpTrackRtPfx.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfx.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfx);
    return kwaVrrpTrackRtPfx.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxValue, kwaVrrpTrackRtPfx.len);

  /* retrieve key: VrrpTrackRtPfxLen */
  kwaVrrpTrackRtPfxLen.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen,
                                           (xLibU8_t *) & keyVrrpTrackRtPfxLenValue,
                                           &kwaVrrpTrackRtPfxLen.len);
  if (kwaVrrpTrackRtPfxLen.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfxLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxLenValue, kwaVrrpTrackRtPfxLen.len);

  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperTrackRouteReachabilityGet(L7_UNIT_CURRENT, objVrIdValue, 
                                                                                keyifIndexValue, keyVrrpTrackRtPfxValue, 
                                                                                keyVrrpTrackRtPfxLenValue, &objVrrpTrackRtReachableValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VrrpTrackRtReachable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrrpTrackRtReachableValue,
                           sizeof (objVrrpTrackRtReachableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtStatus
*
* @purpose Get 'VrrpTrackRtStatus'
 *@description  [VrrpTrackRtStatus] The row status variable, used according to
* installation and removal conventions for conceptual rows.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVrrpTrackRouteTable_VrrpTrackRtStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwaVrrpTrackRtPfx = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyVrrpTrackRtPfxValue;
  fpObjWa_t kwaVrrpTrackRtPfxLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackRtPfxLenValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackRtStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve key: VrrpTrackRtPfx */
  kwaVrrpTrackRtPfx.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                                        (xLibU8_t *) & keyVrrpTrackRtPfxValue,
                                        &kwaVrrpTrackRtPfx.len);
  if (kwaVrrpTrackRtPfx.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfx.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfx);
    return kwaVrrpTrackRtPfx.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxValue, kwaVrrpTrackRtPfx.len);

  /* retrieve key: VrrpTrackRtPfxLen */
  kwaVrrpTrackRtPfxLen.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen,
                                           (xLibU8_t *) & keyVrrpTrackRtPfxLenValue,
                                           &kwaVrrpTrackRtPfxLen.len);
  if (kwaVrrpTrackRtPfxLen.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfxLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxLenValue, kwaVrrpTrackRtPfxLen.len);

 objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
 /* get the value from application */
 if(usmDbVrrpVrIdIntfTrackRouteGet(L7_UNIT_CURRENT, objVrIdValue, 
                                                      keyifIndexValue, keyVrrpTrackRtPfxValue, keyVrrpTrackRtPfxLenValue) == L7_SUCCESS)

  {
    objVrrpTrackRtStatusValue = L7_ROW_STATUS_ACTIVE;
    owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VrrpTrackRtStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrrpTrackRtStatusValue,
                           sizeof (objVrrpTrackRtStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingVrrpTrackRouteTable_VrrpTrackRtStatus
*
* @purpose Set 'VrrpTrackRtStatus'
 *@description  [VrrpTrackRtStatus] The row status variable, used according to
* installation and removal conventions for conceptual rows.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingVrrpTrackRouteTable_VrrpTrackRtStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrrpTrackRtStatusValue;

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaVrrpOperVrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpOperVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwaVrrpTrackRtPfx = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyVrrpTrackRtPfxValue;
  fpObjWa_t kwaVrrpTrackRtPfxLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrrpTrackRtPfxLenValue;
  L7_uint32 tmpMask = 0x80000000, prefixLen = 0;

  xLibU32_t objVrrpTrackRtPrioDecValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VrrpTrackRtStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVrrpTrackRtStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVrrpTrackRtStatusValue, owa.len);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: VrrpOperVrId */
  kwaVrrpOperVrId.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpOperVrId,
                                      (xLibU8_t *) & keyVrrpOperVrIdValue, &kwaVrrpOperVrId.len);
  if (kwaVrrpOperVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpOperVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpOperVrId);
    return kwaVrrpOperVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpOperVrIdValue, kwaVrrpOperVrId.len);

  /* retrieve key: VrrpTrackRtPfx */
  kwaVrrpTrackRtPfx.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfx,
                                        (xLibU8_t *) & keyVrrpTrackRtPfxValue,
                                        &kwaVrrpTrackRtPfx.len);
  if (kwaVrrpTrackRtPfx.rc != XLIBRC_SUCCESS)
  {
    kwaVrrpTrackRtPfx.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfx);
    return kwaVrrpTrackRtPfx.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxValue, kwaVrrpTrackRtPfx.len);

  /* retrieve key: VrrpTrackRtPfxLen */
  kwaVrrpTrackRtPfxLen.rc = xLibFilterGet (wap, XOBJ_routingVrrpTrackRouteTable_VrrpTrackRtPfxLen,
                                           (xLibU8_t *) & keyVrrpTrackRtPfxLenValue,
                                           &kwaVrrpTrackRtPfxLen.len);
  if (kwaVrrpTrackRtPfxLen.rc != XLIBRC_SUCCESS)
  {
#if 0
    /* Even though the decode function fails for the key variables, the Set function is called
     * for the key with no filter.
     * Tools defect: 89555 is raised for the same.
     * To work around that problem for the time being we throw the same error for 
     * invalid prefix length incase of prefix length missing too.
     * Retaining the commented code here to justify the logic below */
    kwaVrrpTrackRtPfxLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
#endif

    kwaVrrpTrackRtPfxLen.rc = XLIBRC_VRRP_TRK_RT_PFXLEN_OUT_OF_RANGE;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrrpTrackRtPfxLenValue, kwaVrrpTrackRtPfxLen.len);

  while(keyVrrpTrackRtPfxLenValue & tmpMask)
  {
    prefixLen++;
    tmpMask = tmpMask >> 1;
  }

  if(prefixLen < VRRP_TRACK_ROUTE_PFX_LEN_MIN)
  {
    kwaVrrpTrackRtPfxLen.rc = XLIBRC_VRRP_TRK_RT_PFXLEN_OUT_OF_RANGE;
    FPOBJ_TRACE_EXIT (bufp, kwaVrrpTrackRtPfxLen);
    return kwaVrrpTrackRtPfxLen.rc;
  }

  /* retrieve object: VrrpTrackRtPrioDec */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVrrpTrackRtPrioDecValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVrrpTrackRtPrioDecValue, owa.len);


  owa.l7rc = L7_SUCCESS;
  objVrIdValue = (xLibU8_t)keyVrrpOperVrIdValue;
  if ((objVrrpTrackRtStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (objVrrpTrackRtStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT))
  {

    if (usmDbVrrpOperTrackRouteAdd(L7_UNIT_CURRENT, objVrIdValue, 
                                                      keyifIndexValue, keyVrrpTrackRtPfxValue, keyVrrpTrackRtPfxLenValue, 
                                                      (L7_uchar8)objVrrpTrackRtPrioDecValue) != L7_SUCCESS)
    {
      owa.l7rc = L7_FAILURE;
    }
  }
  else if (objVrrpTrackRtStatusValue == L7_ROW_STATUS_DESTROY)
  {
    usmDbVrrpOperTrackRouteDelete(L7_UNIT_CURRENT, objVrIdValue, 
                                                      keyifIndexValue, keyVrrpTrackRtPfxValue, keyVrrpTrackRtPfxLenValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
