/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1qTpGroup.c
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
#include "_xe_Switchingdot1qTpGroup_obj.h"
#include "usmdb_mib_vlan_api.h"

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpGroup_VlanIndex_Address
*
* @purpose Get 'VlanIndex + Address +'
*
* @description [VlanIndex]: The VLAN-ID or other identifier refering to this
*              VLAN. 
*              [Address]: The destination Group MAC address in a frame to
*              which this entry's filtering information applies. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpGroup_VlanIndex_Address (void *wap,
                                                           void *bufp[],
                                                           xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaVlanIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue, nextObjVlanIndexValue;
  fpObjWa_t owaAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAddressValue, nextObjAddressValue;
  void *outVlanIndex = (void *) bufp[--keyCount];
  void *outAddress = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outVlanIndex);
  FPOBJ_TRACE_ENTER (outAddress);

  /* retrieve key: VlanIndex */
  owaVlanIndex.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_VlanIndex,
                                   (xLibU8_t *) & objVlanIndexValue,
                                   &owaVlanIndex.len);
  if (owaVlanIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Address */
    owaAddress.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_Address,
                                   (xLibU8_t *) objAddressValue,
                                   &owaAddress.len);
  }
  else
  {
    objVlanIndexValue = 0;
    nextObjVlanIndexValue = 0;
    memset(objAddressValue,0x0,sizeof(objAddressValue));
    memset(nextObjAddressValue,0x0,sizeof(nextObjAddressValue));
  }
  FPOBJ_TRACE_CURRENT_KEY (outVlanIndex, &objVlanIndexValue, owaVlanIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outAddress, &objAddressValue, owaAddress.len);
  owa.rc =
    usmDbDot1qTpGroupEntryNextGet(L7_UNIT_CURRENT, &objVlanIndexValue, objAddressValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outVlanIndex, owaVlanIndex);
    FPOBJ_TRACE_EXIT (outAddress, owaAddress);
    return owa.rc;
  }

  memcpy(nextObjAddressValue,objAddressValue,sizeof(objAddressValue));
  nextObjVlanIndexValue = objVlanIndexValue;

  FPOBJ_TRACE_CURRENT_KEY (outVlanIndex, &nextObjVlanIndexValue,
                           owaVlanIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outAddress, &nextObjAddressValue, owaAddress.len);

  /* return the object value: VlanIndex */
  xLibBufDataSet (outVlanIndex,
                  (xLibU8_t *) & nextObjVlanIndexValue,
                  sizeof (nextObjVlanIndexValue));

  /* return the object value: Address */
  xLibBufDataSet (outAddress,
                  (xLibU8_t *) nextObjAddressValue,
                  strlen (nextObjAddressValue));
  FPOBJ_TRACE_EXIT (outVlanIndex, owaVlanIndex);
  FPOBJ_TRACE_EXIT (outAddress, owaAddress);
  return XLIBRC_SUCCESS;
}
#endif
/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpGroup_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex] The VLAN-ID or other identifier refering to this VLAN.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpGroup_VlanIndex (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = 0;
    nextObjVlanIndexValue =0;
    memset(objAddressValue, 0, sizeof(objAddressValue));
    memset(nextObjAddressValue, 0, sizeof(nextObjAddressValue));
    owa.l7rc = usmDbDot1qTpGroupEntryNextGet(L7_UNIT_CURRENT,
                                             &nextObjVlanIndexValue, 
                                             nextObjAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    memset(objAddressValue, 0, sizeof(objAddressValue));
    do
    {
      owa.l7rc = usmDbDot1qTpGroupEntryNextGet (L7_UNIT_CURRENT,
                                                &objVlanIndexValue, nextObjAddressValue);
      nextObjVlanIndexValue=objVlanIndexValue;
    }
    while ((objVlanIndexValue == nextObjVlanIndexValue) && (owa.l7rc == L7_SUCCESS));
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
* @function fpObjGet_Switchingdot1qTpGroup_Address
*
* @purpose Get 'Address'
*
* @description [Address] The destination Group MAC address in a frame to which this entry's filtering information applies.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpGroup_Address (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_Address,
                          (xLibU8_t *) objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objAddressValue ,0, sizeof(objAddressValue));
    nextObjVlanIndexValue = objVlanIndexValue;
    owa.l7rc = usmDbDot1qTpGroupEntryNextGet (L7_UNIT_CURRENT,
                                              &nextObjVlanIndexValue,
                                              nextObjAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objAddressValue, owa.len);

    nextObjVlanIndexValue = objVlanIndexValue;
    owa.l7rc = usmDbDot1qTpGroupEntryNextGet(L7_UNIT_CURRENT,
                                             &nextObjVlanIndexValue,
                                             nextObjAddressValue);
  }

  if ((objVlanIndexValue != nextObjVlanIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjAddressValue, owa.len);

  /* return the object value: Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjAddressValue, strlen (objAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpGroup_EgressPorts
*
* @purpose Get 'EgressPorts'
*
* @description [EgressPorts]: The complete set of ports, in this VLAN, to
*              which frames destined for this Group MAC address are currently
*              being explicitly forwarded. This does not include ports
*              for which this address is only implicitly forwarded, in the
*              dot1qForwardAllPorts list. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpGroup_EgressPorts (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t buff_len;
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objEgressPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpGroupEgressPortsGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                     keyAddressValue, objEgressPortsValue, &buff_len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EgressPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objEgressPortsValue,
                           buff_len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpGroup_Learnt
*
* @purpose Get 'Learnt'
*
* @description [Learnt]: The subset of ports in dot1qTpGroupEgressPorts which
*              were learnt by GMRP or some other dynamic mechanism, in
*              this Filtering database. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpGroup_Learnt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLearntValue;
  xLibU32_t buff_len;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpGroup_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qTpGroupLearntGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                         keyAddressValue, objLearntValue, &buff_len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Learnt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLearntValue,
                           strlen (objLearntValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
