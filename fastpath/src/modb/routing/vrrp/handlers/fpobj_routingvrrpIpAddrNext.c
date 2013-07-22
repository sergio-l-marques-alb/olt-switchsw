
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingvrrpIpAddrNext.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  20 June 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingvrrpIpAddrNext_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpIpAddrNext_VrId
*
* @purpose Get 'VrId'
 *@description  [VrId] This object contains the Virtual Router Identifier (VRID).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpIpAddrNext_VrId (void *wap, void *bufp)
{

  xLibU8_t  objVrIdValue, inputobjVrIdValue;
  xLibU8_t  nextObjVrIdValue;
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  memset (&objVrIdValue, 0, sizeof (objVrIdValue));
  memset (&objifIndexValue, 0, sizeof (objifIndexValue));
  memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
  memset (&nextObjifIndexValue, 0, sizeof (nextObjifIndexValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpIpAddrNext_VrId,
                          (xLibU8_t *) & objVrIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbVrrpOperFirstGet(L7_UNIT_CURRENT,
                                     (xLibU8_t*) &nextObjVrIdValue, &nextObjifIndexValue);  
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrIdValue, owa.len);
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
    inputobjVrIdValue = objVrIdValue;
    do
    {
      owa.l7rc = usmDbVrrpOperNextGet(L7_UNIT_CURRENT,
                                      objVrIdValue,
                                      objifIndexValue,(xLibU8_t*) &nextObjVrIdValue, &nextObjifIndexValue);
      objifIndexValue = nextObjifIndexValue;
      objVrIdValue = nextObjVrIdValue;
    }
    while ((inputobjVrIdValue == nextObjVrIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrIdValue, owa.len);

  /* return the object value: VrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVrIdValue, sizeof (nextObjVrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingvrrpIpAddrNext_ifIndex
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
xLibRC_t fpObjGet_routingvrrpIpAddrNext_ifIndex (void *wap, void *bufp)
{

  xLibU8_t objVrIdValue;
  xLibU8_t nextObjVrIdValue;
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  memset (&objVrIdValue, 0, sizeof (objVrIdValue));
  memset (&objifIndexValue, 0, sizeof (objifIndexValue));
  memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
  memset (&nextObjifIndexValue, 0, sizeof (nextObjifIndexValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpIpAddrNext_VrId,
                          (xLibU8_t *) & objVrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVrIdValue, owa.len);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpIpAddrNext_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
    owa.l7rc = usmDbVrrpOperNextGet(L7_UNIT_CURRENT,
                                     objVrIdValue,
                                     objifIndexValue,(xLibU8_t*) &nextObjVrIdValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

    owa.l7rc = usmDbVrrpOperNextGet(L7_UNIT_CURRENT,
                                    objVrIdValue,
                                    objifIndexValue,(xLibU8_t*) &nextObjVrIdValue, &nextObjifIndexValue);

  }

  if ((objVrIdValue != nextObjVrIdValue) || (owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingvrrpIpAddrNext_IpAddr
*
* @purpose Get 'IpAddr'
 *@description  [IpAddr] The assigned IP addresses that a virtual router is
* responsible for backing up
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpIpAddrNext_IpAddr (void *wap, void *bufp)
{

  fpObjWa_t kwaVrId = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyVrIdValue;
  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objIpAddrValue;
  xLibIpV4_t nextObjIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwaVrId.rc = xLibFilterGet (wap, XOBJ_routingvrrpIpAddrNext_VrId,
                              (xLibU8_t *) & keyVrIdValue, &kwaVrId.len);
  if (kwaVrId.rc != XLIBRC_SUCCESS)
  {
    kwaVrId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVrId);
    return kwaVrId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwaVrId.len);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_routingvrrpIpAddrNext_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: IpAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpIpAddrNext_IpAddr,
                          (xLibU8_t *) & objIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, keyVrIdValue,
                                     keyifIndexValue, 0, &nextObjIpAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddrValue, owa.len);
    owa.l7rc = usmDbVrrpIpAddressNextGet(L7_UNIT_CURRENT, keyVrIdValue,
                                    keyifIndexValue, objIpAddrValue, &nextObjIpAddrValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpAddrValue, owa.len);

  /* return the object value: IpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpAddrValue, sizeof (nextObjIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

