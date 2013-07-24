
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_Switchingdot1dPortOutboundAccessPriorityEntry.c
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
#include "_xe_Switchingdot1dPortOutboundAccessPriorityEntry_obj.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1dPortOutboundAccessPriorityEntry_Dot1dBasePort
*
* @purpose Get 'Dot1dBasePort'
 *@description  [Dot1dBasePort] list of information for each port of bridge.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dPortOutboundAccessPriorityEntry_Dot1dBasePort (void *wap,
                                                                               void *bufp)
{
  xLibU32_t objDot1dBasePortValue;
  xLibU32_t nextObjDot1dBasePortValue;
  xLibU32_t nextObjdot1dOutboundRegenUserPriorityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  memset (&objDot1dBasePortValue, 0, sizeof (objDot1dBasePortValue));

  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dPortOutboundAccessPriorityEntry_Dot1dBasePort,
                          (xLibU8_t *) & objDot1dBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset (&nextObjDot1dBasePortValue, 0, sizeof (nextObjDot1dBasePortValue));
    memset (&nextObjdot1dOutboundRegenUserPriorityValue, 0,
            sizeof (nextObjdot1dOutboundRegenUserPriorityValue));

    owa.l7rc = usmDbDot1dPortOutboundAccessPriorityEntryNextGet(L7_UNIT_CURRENT, &nextObjDot1dBasePortValue,
                            &nextObjdot1dOutboundRegenUserPriorityValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1dBasePortValue, owa.len);

    memset (&nextObjdot1dOutboundRegenUserPriorityValue, 0,
            sizeof (nextObjdot1dOutboundRegenUserPriorityValue));

    nextObjDot1dBasePortValue = objDot1dBasePortValue;

    do
    {
      owa.l7rc = usmDbDot1dPortOutboundAccessPriorityEntryNextGet(L7_UNIT_CURRENT,
                                      &nextObjDot1dBasePortValue,
                                      &nextObjdot1dOutboundRegenUserPriorityValue);
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
* @function fpObjGet_Switchingdot1dPortOutboundAccessPriorityEntry_dot1dOutboundRegenUserPriority
*
* @purpose Get 'dot1dOutboundRegenUserPriority'
 *@description  [dot1dOutboundRegenUserPriority] Outbound user priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dPortOutboundAccessPriorityEntry_dot1dOutboundRegenUserPriority (void
                                                                                                *wap,
                                                                                                void
                                                                                                *bufp)
{

  xLibU32_t objDot1dBasePortValue;
  xLibU32_t nextObjDot1dBasePortValue;
  xLibU32_t objdot1dOutboundRegenUserPriorityValue;
  xLibU32_t nextObjdot1dOutboundRegenUserPriorityValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  memset (&objDot1dBasePortValue, 0, sizeof (objDot1dBasePortValue));
  memset (&objdot1dOutboundRegenUserPriorityValue, 0, sizeof (objdot1dOutboundRegenUserPriorityValue));

  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dPortOutboundAccessPriorityEntry_Dot1dBasePort,
                          (xLibU8_t *) & objDot1dBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDot1dBasePortValue, owa.len);

  /* retrieve key: dot1dOutboundRegenUserPriority */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1dPortOutboundAccessPriorityEntry_dot1dOutboundRegenUserPriority,
                   (xLibU8_t *) & objdot1dOutboundRegenUserPriorityValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjdot1dOutboundRegenUserPriorityValue, 0,
            sizeof (nextObjdot1dOutboundRegenUserPriorityValue));

    nextObjDot1dBasePortValue = objDot1dBasePortValue;
    owa.l7rc = usmDbDot1dPortOutboundAccessPriorityEntryNextGet (L7_UNIT_CURRENT, 
                                                     &nextObjDot1dBasePortValue,
                                    &nextObjdot1dOutboundRegenUserPriorityValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dOutboundRegenUserPriorityValue, owa.len);
    nextObjDot1dBasePortValue = objDot1dBasePortValue;
    nextObjdot1dOutboundRegenUserPriorityValue=objdot1dOutboundRegenUserPriorityValue;

    owa.l7rc = usmDbDot1dPortOutboundAccessPriorityEntryNextGet(L7_UNIT_CURRENT,
                                                     &nextObjDot1dBasePortValue,
                                    &nextObjdot1dOutboundRegenUserPriorityValue);

  }

  if ((objDot1dBasePortValue != nextObjDot1dBasePortValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1dOutboundRegenUserPriorityValue, owa.len);

  /* return the object value: dot1dOutboundRegenUserPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1dOutboundRegenUserPriorityValue,
                           sizeof (objdot1dOutboundRegenUserPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dPortOutboundAccessPriorityEntry_dot1dPortOutboundAccessPriority
*
* @purpose Get 'dot1dPortOutboundAccessPriority'
 *@description  [dot1dPortOutboundAccessPriority] Access priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_Switchingdot1dPortOutboundAccessPriorityEntry_dot1dPortOutboundAccessPriority (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaDot1dBasePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDot1dBasePortValue;
  fpObjWa_t kwadot1dOutboundRegenUserPriority = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dOutboundRegenUserPriorityValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1dPortOutboundAccessPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1dBasePort */
  kwaDot1dBasePort.rc =
    xLibFilterGet (wap, XOBJ_Switchingdot1dPortOutboundAccessPriorityEntry_Dot1dBasePort,
                   (xLibU8_t *) & keyDot1dBasePortValue, &kwaDot1dBasePort.len);
  if (kwaDot1dBasePort.rc != XLIBRC_SUCCESS)
  {
    kwaDot1dBasePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1dBasePort);
    return kwaDot1dBasePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDot1dBasePortValue, kwaDot1dBasePort.len);

  /* retrieve key: dot1dOutboundRegenUserPriority */
  kwadot1dOutboundRegenUserPriority.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1dPortOutboundAccessPriorityEntry_dot1dOutboundRegenUserPriority,
                   (xLibU8_t *) & keydot1dOutboundRegenUserPriorityValue,
                   &kwadot1dOutboundRegenUserPriority.len);
  if (kwadot1dOutboundRegenUserPriority.rc != XLIBRC_SUCCESS)
  {
    kwadot1dOutboundRegenUserPriority.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dOutboundRegenUserPriority);
    return kwadot1dOutboundRegenUserPriority.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dOutboundRegenUserPriorityValue,
                           kwadot1dOutboundRegenUserPriority.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dPortOutboundAccessPriorityGet(L7_UNIT_CURRENT, keyDot1dBasePortValue,
                              keydot1dOutboundRegenUserPriorityValue,
                              &objdot1dPortOutboundAccessPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1dPortOutboundAccessPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1dPortOutboundAccessPriorityValue,
                           sizeof (objdot1dPortOutboundAccessPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
