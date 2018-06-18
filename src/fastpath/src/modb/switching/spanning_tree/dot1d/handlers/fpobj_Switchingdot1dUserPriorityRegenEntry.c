
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_Switchingdot1dUserPriorityRegenEntry.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  27 May 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_Switchingdot1dUserPriorityRegenEntry_obj.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1dUserPriorityRegenEntry_Dot1dBasePort
*
* @purpose Get 'Dot1dBasePort'
 *@description  [Dot1dBasePort] list of information for each port of bridge.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dUserPriorityRegenEntry_Dot1dBasePort (void *wap, void *bufp)
{

  xLibU32_t objDot1dBasePortValue;
  xLibU32_t nextObjDot1dBasePortValue;
  xLibU32_t objdot1dUserPriorityValue;
  xLibU32_t nextObjdot1dUserPriorityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  memset (&objDot1dBasePortValue, 0, sizeof (objDot1dBasePortValue));
  memset (&nextObjDot1dBasePortValue, 0, sizeof (nextObjDot1dBasePortValue));
  memset (&objdot1dUserPriorityValue, 0, sizeof (objdot1dUserPriorityValue));
  memset (&nextObjdot1dUserPriorityValue, 0, sizeof (nextObjdot1dUserPriorityValue));

  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_Dot1dBasePort,
                          (xLibU8_t *) & objDot1dBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1dUserPriorityRegenEntryNextGet(L7_UNIT_CURRENT,
                                     &nextObjDot1dBasePortValue,
                                     &nextObjdot1dUserPriorityValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1dBasePortValue, owa.len);
    nextObjDot1dBasePortValue = objDot1dBasePortValue;

    do
    {
      owa.l7rc = usmDbDot1dUserPriorityRegenEntryNextGet(L7_UNIT_CURRENT,
                                      &nextObjDot1dBasePortValue,
                                      &nextObjdot1dUserPriorityValue);
    }
    while ((objDot1dBasePortValue == nextObjDot1dBasePortValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDot1dBasePortValue, owa.len);

  /* return the object value: Dot1dBasePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDot1dBasePortValue,
                           sizeof (objDot1dBasePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dUserPriorityRegenEntry_dot1dUserPriority
*
* @purpose Get 'dot1dUserPriority'
 *@description  [dot1dUserPriority] user priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dUserPriorityRegenEntry_dot1dUserPriority (void *wap, void *bufp)
{

  xLibU32_t objDot1dBasePortValue;
  xLibU32_t nextObjDot1dBasePortValue;
  xLibU32_t objdot1dUserPriorityValue;
  xLibU32_t nextObjdot1dUserPriorityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */

  memset (&objDot1dBasePortValue, 0, sizeof (objDot1dBasePortValue));
  memset (&nextObjDot1dBasePortValue, 0, sizeof (nextObjDot1dBasePortValue));
  memset (&objdot1dUserPriorityValue, 0, sizeof (objdot1dUserPriorityValue));
  memset (&nextObjdot1dUserPriorityValue, 0, sizeof (nextObjdot1dUserPriorityValue));

  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_Dot1dBasePort,
                          (xLibU8_t *) & objDot1dBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1dBasePortValue, owa.len);

  /* retrieve key: dot1dUserPriority */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_dot1dUserPriority,
                          (xLibU8_t *) & objdot1dUserPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  
    nextObjDot1dBasePortValue = objDot1dBasePortValue;

    owa.l7rc = usmDbDot1dUserPriorityRegenEntryNextGet(L7_UNIT_CURRENT,
                                            &nextObjDot1dBasePortValue,
                                        &nextObjdot1dUserPriorityValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dUserPriorityValue, owa.len);

    nextObjDot1dBasePortValue = objDot1dBasePortValue;
    nextObjdot1dUserPriorityValue = objdot1dUserPriorityValue;

    owa.l7rc = usmDbDot1dUserPriorityRegenEntryNextGet(L7_UNIT_CURRENT,
                                            &nextObjDot1dBasePortValue,
                                        &nextObjdot1dUserPriorityValue);

  }

  if ((objDot1dBasePortValue != nextObjDot1dBasePortValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1dUserPriorityValue, owa.len);

  /* return the object value: dot1dUserPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1dUserPriorityValue,
                           sizeof (objdot1dUserPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dUserPriorityRegenEntry_Dot1dRegenUserPriority
*
* @purpose Get 'Dot1dRegenUserPriority'
 *@description  [Dot1dRegenUserPriority] regen user priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dUserPriorityRegenEntry_Dot1dRegenUserPriority (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t kwaDot1dBasePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t kwadot1dUserPriority = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dUserPriorityValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dRegenUserPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwaDot1dBasePort.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_Dot1dBasePort,
                                       (xLibU8_t *) & keyDot1dBasePortValue, &kwaDot1dBasePort.len);
  if (kwaDot1dBasePort.rc != XLIBRC_SUCCESS)
  {
    kwaDot1dBasePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1dBasePort);
    return kwaDot1dBasePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwaDot1dBasePort.len);

  /* retrieve key: dot1dUserPriority */
  kwadot1dUserPriority.rc =
    xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_dot1dUserPriority,
                   (xLibU8_t *) & keydot1dUserPriorityValue, &kwadot1dUserPriority.len);
  if (kwadot1dUserPriority.rc != XLIBRC_SUCCESS)
  {
    kwadot1dUserPriority.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dUserPriority);
    return kwadot1dUserPriority.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dUserPriorityValue, kwadot1dUserPriority.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dRegenUserPriorityGet(L7_UNIT_CURRENT, keyDot1dBasePortValue,
                              keydot1dUserPriorityValue, &objDot1dRegenUserPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dRegenUserPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dRegenUserPriorityValue,
                           sizeof (objDot1dRegenUserPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_Switchingdot1dUserPriorityRegenEntry_Dot1dRegenUserPriority
*
* @purpose Set 'Dot1dRegenUserPriority'
 *@description  [Dot1dRegenUserPriority] regen user priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dUserPriorityRegenEntry_Dot1dRegenUserPriority (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dRegenUserPriorityValue;

  fpObjWa_t kwaDot1dBasePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t kwadot1dUserPriority = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dUserPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dRegenUserPriority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1dRegenUserPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dRegenUserPriorityValue, owa.len);

  /* retrieve key: Dot1dBasePort */
  kwaDot1dBasePort.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_Dot1dBasePort,
                                       (xLibU8_t *) & keyDot1dBasePortValue, &kwaDot1dBasePort.len);
  if (kwaDot1dBasePort.rc != XLIBRC_SUCCESS)
  {
    kwaDot1dBasePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1dBasePort);
    return kwaDot1dBasePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwaDot1dBasePort.len);

  /* retrieve key: dot1dUserPriority */
  kwadot1dUserPriority.rc =
    xLibFilterGet (wap, XOBJ_Switchingdot1dUserPriorityRegenEntry_dot1dUserPriority,
                   (xLibU8_t *) & keydot1dUserPriorityValue, &kwadot1dUserPriority.len);
  if (kwadot1dUserPriority.rc != XLIBRC_SUCCESS)
  {
    kwadot1dUserPriority.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dUserPriority);
    return kwadot1dUserPriority.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dUserPriorityValue, kwadot1dUserPriority.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1dRegenUserPrioritySet(L7_UNIT_CURRENT, keyDot1dBasePortValue,
                              keydot1dUserPriorityValue, objDot1dRegenUserPriorityValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
