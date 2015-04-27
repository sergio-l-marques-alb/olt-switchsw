/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1qOperVlan.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1qConfig-object.xml
*
* @create  9 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1qOperVlan_obj.h"
#include "usmdb_mib_vlan_api.h"

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_SwitchingDot1qOperVlan_VlanIndex_Port
*
* @purpose Get 'VlanIndex + Port +'
*
* @description [VlanIndex]: vlan ID 
*              [Port]: physical or logical interface to be configured 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qOperVlan_VlanIndex_Port (void *wap,
                                                         void *bufp[],
                                                         xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue, nextObjVlanIndexValue;
  fpObjWa_t owaPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortValue, nextObjPortValue;
  void *outVlanIndex = (void *) bufp[--keyCount];
  void *outPort = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outVlanIndex);
  FPOBJ_TRACE_ENTER (outPort);

  /* retrieve key: VlanIndex */
  owaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_VlanIndex,
                                   (xLibU8_t *) & objVlanIndexValue,
                                   &owaVlanIndex.len);
  if (owaVlanIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Port */
    owaPort.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_Port,
                                (xLibU8_t *) & objPortValue, &owaPort.len);
  }
  else
  {
     objVlanIndexValue = 0;
     nextObjVlanIndexValue = 0;
     objPortValue = 0;
     nextObjPortValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (outVlanIndex, &objVlanIndexValue, owaVlanIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outPort, &objPortValue, owaPort.len);

  /*We need to fix this function and parameters. */
  /*owa.rc =
    usmDbGetNextUnknown (L7_UNIT_CURRENT, objVlanIndexValue, objPortValue,
                         &nextObjVlanIndexValue, &nextObjPortValue); */

  nextObjVlanIndexValue = objVlanIndexValue;
  nextObjPortValue = objPortValue;

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outVlanIndex, owaVlanIndex);
    FPOBJ_TRACE_EXIT (outPort, owaPort);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outVlanIndex, &nextObjVlanIndexValue,
                           owaVlanIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outPort, &nextObjPortValue, owaPort.len);

  /* return the object value: VlanIndex */
  xLibBufDataSet (outVlanIndex,
                  (xLibU8_t *) & nextObjVlanIndexValue,
                  sizeof (nextObjVlanIndexValue));

  /* return the object value: Port */
  xLibBufDataSet (outPort,
                  (xLibU8_t *) & nextObjPortValue, sizeof (nextObjPortValue));
  FPOBJ_TRACE_EXIT (outVlanIndex, owaVlanIndex);
  FPOBJ_TRACE_EXIT (outPort, owaPort);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qOperVlan_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex] vlan ID
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qOperVlan_VlanIndex (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  xLibU32_t objPortValue;
  xLibU32_t nextObjPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = objPortValue = 0;
    /*owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objVlanIndexValue,
                                     objPortValue, &nextobjVlanIndexValue, &nextObjPortValue);*/
    owa.l7rc = L7_FAILURE;
    nextObjVlanIndexValue = 0;
    nextObjPortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    objPortValue = 0;
    /*do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objVlanIndexValue,
                                      objPortValue, &nextobjVlanIndexValue, &nextObjPortValue);
    }
    while ((objVlanIndexValue == nextObjVlanIndexValue) && (owa.l7rc == L7_SUCCESS));*/
    owa.l7rc = L7_FAILURE;
    nextObjVlanIndexValue = 0;
    nextObjPortValue = 0;
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue, sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qOperVlan_Port
*
* @purpose Get 'Port'
*
* @description [Port] physical or logical interface to be configured
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qOperVlan_Port (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  xLibU32_t objPortValue;
  xLibU32_t nextObjPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  /* retrieve key: Port */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_Port,
                          (xLibU8_t *) & objPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPortValue = 0;
    /*owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objVlanIndexValue,
                                     objPortValue, &nextobjVlanIndexValue, &nextObjPortValue);*/
    owa.l7rc = L7_FAILURE;
    nextObjVlanIndexValue = 0;
    nextObjPortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortValue, owa.len);

    /*owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                    objVlanIndexValue,
                                    objPortValue, &nextobjVlanIndexValue, &nextObjPortValue);*/
     owa.l7rc = L7_FAILURE;
    nextObjVlanIndexValue = 0;
    nextObjPortValue = 0;
  }

  if ((objVlanIndexValue != nextObjVlanIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortValue, owa.len);

  /* return the object value: Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortValue, sizeof (objPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qOperVlan_TaggingMode
*
* @purpose Get 'TaggingMode'
*
* @description [TaggingMode]: Get the tagging configuration for a member port
*              of a dynamic vlan 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qOperVlan_TaggingMode (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTaggingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Port */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qOperVlan_Port,
                           (xLibU8_t *) & keyPortValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qOperVlanTaggedMemberGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                       keyPortValue, &objTaggingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TaggingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTaggingModeValue,
                           sizeof (objTaggingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
