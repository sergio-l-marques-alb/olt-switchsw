
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingGarpInterfaceStats.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  31 October 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingGarpInterfaceStats_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "garp_exports.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interface to be configured for GARP Parameters   
* @notes       
*
* @return
*******************************************************************************/
/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interface to be configured for GARP Parameters   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbValidIntIfNumNext (objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (nextObjInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_GarpStatsGet
*
* @purpose Get 'GarpStatsGet'
 *@description  [GarpStatsGet] <HTML>Get the GARP protocol related statistic   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_GarpStatsGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGarpStatsGetValue;
  xLibU32_t GarpStatsCounterTypeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);
  
  /* retrieve object: GarpStatsCounterType */
  owa.len = sizeof (GarpStatsCounterTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_GarpStatsCounterType,
                          (xLibU8_t *) &GarpStatsCounterTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* get the value from application */
  owa.l7rc = usmDbGarpStatisticGet (keyInterfaceValue, GarpStatsCounterTypeValue, &objGarpStatsGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGarpStatsGetValue, sizeof (objGarpStatsGetValue));

  /* return the object value: GarpStatsGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGarpStatsGetValue,
                           sizeof (objGarpStatsGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_GarpStatsCounterType
*
* @purpose Get 'GarpStatsCounterType'
 *@description  [GarpStatsCounterType] <HTML>GARP Stats Counter Type    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_GarpStatsCounterType (void *wap, void *bufp)
{
  /* Just return success as this is paired with another object*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpInterfaceStats_GarpStatsCounterType
*
* @purpose Set 'GarpStatsCounterType'
 *@description  [GarpStatsCounterType] <HTML>GARP Stats Counter Type    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpInterfaceStats_GarpStatsCounterType (void *wap, void *bufp)
{
  /* Just return success as this is paired with another object*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_GarpErrorStatsGet
*
* @purpose Get 'GarpErrorStatsGet'
 *@description  [GarpErrorStatsGet] <HTML>Get the GARP protocol related statistic   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_GarpErrorStatsGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGarpErrorStatsGetValue;
  xLibU32_t GarpStatsCounterTypeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);
  
  /* retrieve object: GarpStatsCounterType */
  owa.len = sizeof (GarpStatsCounterTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_GarpStatsCounterType,
                          (xLibU8_t *) &GarpStatsCounterTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* get the value from application */
  owa.l7rc = usmDbGarpStatisticGet (keyInterfaceValue, GarpStatsCounterTypeValue, &objGarpErrorStatsGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGarpErrorStatsGetValue, sizeof (objGarpErrorStatsGetValue));

  /* return the object value: GarpErrorStatsGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGarpErrorStatsGetValue,
                           sizeof (objGarpErrorStatsGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_GarpErrorStatsCounterType
*
* @purpose Get 'GarpErrorStatsCounterType'
 *@description  [GarpErrorStatsCounterType] <HTML>Garp Error Stats Counter Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_GarpErrorStatsCounterType (void *wap, void *bufp)
{
   /* Just return success as this is paired with another object*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpInterfaceStats_GarpErrorStatsCounterType
*
* @purpose Set 'GarpErrorStatsCounterType'
 *@description  [GarpErrorStatsCounterType] <HTML>Garp Error Stats Counter Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpInterfaceStats_GarpErrorStatsCounterType (void *wap, void *bufp)
{
  /* Just return success as this is paired with another object*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpInterfaceStats_GarpStatsClear
*
* @purpose Set 'GarpStatsClear'
 *@description  [GarpStatsClear] <HTML>Garp Stats Clear   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpInterfaceStats_GarpStatsClear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGarpStatsClearValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GarpStatsClear */
  owa.len = sizeof (objGarpStatsClearValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGarpStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGarpStatsClearValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbGarpStatisticsClear (keyInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpInterfaceStats_GarpErrorStatsClear
*
* @purpose Set 'GarpErrorStatsClear'
 *@description  [GarpErrorStatsClear] <HTML>Garp Error Stats Clear   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpInterfaceStats_GarpErrorStatsClear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGarpErrorStatsClearValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GarpErrorStatsClear */
  owa.len = sizeof (objGarpErrorStatsClearValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGarpErrorStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGarpErrorStatsClearValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbGarpErrorStatisticsClear (keyInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_GvrpVlanCreationForbid
*
* @purpose Get 'GvrpVlanCreationForbid'
 *@description  [GvrpVlanCreationForbid] <HTML>value of vlan-creation-forbid
* flag for an interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_GvrpVlanCreationForbid (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGvrpVlanCreationForbidValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGvrpVlanCreationForbidGet (keyInterfaceValue, &objGvrpVlanCreationForbidValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGvrpVlanCreationForbidValue,
                     sizeof (objGvrpVlanCreationForbidValue));

  /* return the object value: GvrpVlanCreationForbid */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpVlanCreationForbidValue,
                           sizeof (objGvrpVlanCreationForbidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpInterfaceStats_GvrpVlanCreationForbid
*
* @purpose Set 'GvrpVlanCreationForbid'
 *@description  [GvrpVlanCreationForbid] <HTML>value of vlan-creation-forbid
* flag for an interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpInterfaceStats_GvrpVlanCreationForbid (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGvrpVlanCreationForbidValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GvrpVlanCreationForbid */
  owa.len = sizeof (objGvrpVlanCreationForbidValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpVlanCreationForbidValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGvrpVlanCreationForbidValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbGvrpPortVlanCreationForbidSet (keyInterfaceValue, objGvrpVlanCreationForbidValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingGarpInterfaceStats_GvrpRegistForbid
*
* @purpose Get 'GvrpRegistForbid'
 *@description  [GvrpRegistForbid] <HTML>value of registration-forbid flag for
* an interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingGarpInterfaceStats_GvrpRegistForbid (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGvrpRegistForbidValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGvrpPortRegistrationForbidGet (keyInterfaceValue, &objGvrpRegistForbidValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGvrpRegistForbidValue, sizeof (objGvrpRegistForbidValue));

  /* return the object value: GvrpRegistForbid */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpRegistForbidValue,
                           sizeof (objGvrpRegistForbidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingGarpInterfaceStats_GvrpRegistForbid
*
* @purpose Set 'GvrpRegistForbid'
 *@description  [GvrpRegistForbid] <HTML>value of registration-forbid flag for
* an interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingGarpInterfaceStats_GvrpRegistForbid (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGvrpRegistForbidValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GvrpRegistForbid */
  owa.len = sizeof (objGvrpRegistForbidValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpRegistForbidValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGvrpRegistForbidValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingGarpInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbGvrpPortRegistrationForbidSet (keyInterfaceValue, objGvrpRegistForbidValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
