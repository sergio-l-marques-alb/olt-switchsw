/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingPortSecurityGroup.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to PortMACLocking-object.xml

*

* @create  12 February 2008

*

* @author Radha K 

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingPortSecurityGroup_obj.h"

#include "usmdb_pml_api.h"

#include "usmdb_util_api.h"

#include "usmdb_mib_vlan_api.h"



extern L7_RC_t cliWebXuiConvertTo32BitUnsignedInteger (const L7_char8 * buf, L7_uint32 * pVal);

extern L7_BOOL cliConvertMac(L7_uchar8 * buf, L7_uchar8 * mac);



/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_Interface

*

* @purpose Get 'Interface'

*

* @description [Interface]: Interface to be configured for PortSecurity 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_Interface (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objInterfaceValue;

  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & objInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objInterfaceValue = 0;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

  owa.l7rc = usmDbPmlNextValidIntfGet(objInterfaceValue,&nextObjInterfaceValue); 



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);



  /* return the object value: Interface */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,

                           sizeof (objInterfaceValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_Mode

*

* @purpose Get 'Mode'

*

* @description [Mode]: Mode showing whether at port level security is enabled

*              or not 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_Mode (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfModeGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                  &objModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: Mode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objModeValue,

                           sizeof (objModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_Mode

*

* @purpose Set 'Mode'

*

* @description [Mode]: Mode showing whether at port level security is enabled

*              or not 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_Mode (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objModeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: Mode */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objModeValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfModeSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                  objModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}




/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_TrapFrequency

*

* @purpose Get 'TrapFrequency'

*

* @description [DTrapFrequency]: This variable signifies the Trap Frequency that

*              need to be configured for a trap. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_TrapFrequency (void *wap,

                                                           void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t TrapFrequencyValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfTrapFrequencyGet(keyInterfaceValue,&TrapFrequencyValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: DynamicLimit */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & TrapFrequencyValue,

                           sizeof (TrapFrequencyValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_TrapFrequency

*

* @purpose Set 'TrapFrequency'

*

* @description [TrapFrequency]: This variable signifies the Trap Frequent that 

*              need to be configured for a port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_TrapFrequency (void *wap,

                                                           void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t TrapFrequencyValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: TrapFrequency */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & TrapFrequencyValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &TrapFrequencyValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfTrapFrequencySet(keyInterfaceValue,TrapFrequencyValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_DynamicLimit

*

* @purpose Get 'DynamicLimit'

*

* @description [DynamicLimit]: This variable signifies the limit of dynamically

*              locked MAC addresses allowed on a specific port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_DynamicLimit (void *wap,

                                                           void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDynamicLimitValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfDynamicLimitGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                          &objDynamicLimitValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: DynamicLimit */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicLimitValue,

                           sizeof (objDynamicLimitValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_DynamicLimit

*

* @purpose Set 'DynamicLimit'

*

* @description [DynamicLimit]: This variable signifies the limit of dynamically

*              locked MAC addresses allowed on a specific port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_DynamicLimit (void *wap,

                                                           void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDynamicLimitValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: DynamicLimit */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDynamicLimitValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objDynamicLimitValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfDynamicLimitSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                          objDynamicLimitValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_StaticLimit

*

* @purpose Get 'StaticLimit'

*

* @description [StaticLimit]: This variable signifies the limit of statically

*              locked MAC addresses allowed on a specific port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_StaticLimit (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStaticLimitValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfStaticLimitGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                         &objStaticLimitValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StaticLimit */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticLimitValue,

                           sizeof (objStaticLimitValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_StaticLimit

*

* @purpose Set 'StaticLimit'

*

* @description [StaticLimit]: This variable signifies the limit of statically

*              locked MAC addresses allowed on a specific port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_StaticLimit (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStaticLimitValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StaticLimit */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStaticLimitValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStaticLimitValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfStaticLimitSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                         objStaticLimitValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_ViolationTrapMode

*

* @purpose Get 'ViolationTrapMode'

*

* @description [ViolationTrapMode]: This variable is used to enable or disable

*              the sending of new violation traps designating when a

*              packet with a disallowed MAC address is received on a locked

*              port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_ViolationTrapMode (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objViolationTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbPmlIntfViolationTrapModeGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                      &objViolationTrapModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: ViolationTrapMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objViolationTrapModeValue,

                           sizeof (objViolationTrapModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_ViolationTrapMode

*

* @purpose Set 'ViolationTrapMode'

*

* @description [ViolationTrapMode]: This variable is used to enable or disable

*              the sending of new violation traps designating when a

*              packet with a disallowed MAC address is received on a locked

*              port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_ViolationTrapMode (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objViolationTrapModeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: ViolationTrapMode */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objViolationTrapModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objViolationTrapModeValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc =

    usmDbPmlIntfViolationTrapModeSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                      objViolationTrapModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



#if 0  

WE CAN REMOVE THIS CODE.SINCE WE HAVE PLACED SEPERATE CODE TO READ STATIC MACS

/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_StaticMACs

*

* @purpose Get 'StaticMACs'

*

* @description [StaticMACs]: This variable displays the statically locked

*              MAC addresses for port. he list displayed in a particular fashion

*              :2 a0:b1:c2:d1:e3:a1,11 a0:b1:c2:d3:e4:f5 (i.e., VLAN

*              MAC pairs separated by commas). 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_StaticMACs (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStaticMACsValue;

  L7_enetMacAddr_t  macAddr;

  xLibU16_t vlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));

  memcpy(&macAddr.addr, objStaticMACsValue, L7_ENET_MAC_ADDR_LEN);

  /* get the value from application */

  owa.l7rc = usmDbPmlIntfStaticEntryGetFirst(L7_UNIT_CURRENT,keyInterfaceValue,

                                             &macAddr, &vlanIdValue);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  memset(objStaticMACsValue, 0x00, L7_ENET_MAC_ADDR_LEN);

  memcpy(objStaticMACsValue, macAddr.addr, sizeof(L7_enetMacAddr_t));

  /* return the object value: StaticMACs */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStaticMACsValue,

                           strlen (objStaticMACsValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

#endif 



/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_LastDiscardedMAC

*

* @purpose Get 'LastDiscardedMAC'

*

* @description [LastDiscardedMAC]: This variable displays the vlan-id and

*              source MAC address of the last packet that was discarded on

*              a locked port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_LastDiscardedMAC (void *wap,

                                                               void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));

  xLibStr6_t objLastDiscardedMACValue;

  L7_enetMacAddr_t  macAddr;

  xLibU16_t vlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));

  

  /* get the value from application */

  owa.l7rc =

    usmDbPmlIntfLastViolationAddrGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                      &macAddr, &vlanIdValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  memset(objLastDiscardedMACValue, 0x00, sizeof(objLastDiscardedMACValue));

  memcpy(objLastDiscardedMACValue, macAddr.addr, sizeof(L7_enetMacAddr_t));

  /* return the object value: LastDiscardedMAC */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLastDiscardedMACValue,

                           sizeof (objLastDiscardedMACValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_LastDiscardedVLAN

*

* @purpose Get 'LastDiscardedVLAN'

*

* @description [LastDiscardedVLAN]: This variable displays the vlan-id and

*              source MAC address of the last packet that was discarded on

*              a locked port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_LastDiscardedVLAN(void *wap,

                                                               void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLastDiscardedVLANValue;

  L7_enetMacAddr_t  macAddr;

  xLibU16_t vlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));

  

  /* get the value from application */

  owa.l7rc =

    usmDbPmlIntfLastViolationAddrGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                      &macAddr, &vlanIdValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  objLastDiscardedVLANValue = vlanIdValue; 

  

  /* return the object value: LastDiscardedMAC */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objLastDiscardedVLANValue,

                           sizeof (objLastDiscardedVLANValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_MACAddressMove

*

* @purpose Set 'MACAddressMove'

*

* @description [MACAddressMove] This MIB variable accepts a VLAN id and MAC address 

* to be removed from the list of statically locked MAC addresses on a port.. The VLAN

* id and MAC address combination would be entered in a particular fashion like :- 

* 2 a0:b0:c0:d1:e2:a1(the vlan-id and MAC

*

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_MACAddressMove (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

/*  xLibStr256_t objMACAddressMoveValue; */



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: MACAddressMove 

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMACAddressMoveValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objMACAddressMoveValue, owa.len);*/



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfDynamicToStaticMove(L7_UNIT_CURRENT, keyInterfaceValue);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



#if 0

/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_DynamicVLANId

*

* @purpose Get 'DynamicVLANId'

*

* @description [DynamicVLANId]: Source VLAN id of the packet that is received

*              on the dynamically locked port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_DynamicVLANId (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDynamicVLANIdValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibStr256_t DynamicMACAddressValue;

  L7_enetMacAddr_t  macAddr;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));

  memcpy(&macAddr.addr, DynamicMACAddressValue, L7_ENET_MAC_ADDR_LEN);

  /* get the value from application */

  owa.l7rc =

    usmDbPmlIntfDynamicEntryGetFirst (L7_UNIT_CURRENT, 

                                      keyInterfaceValue,

                                      &macAddr,

                                      (xLibU16_t *)&objDynamicVLANIdValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objDynamicVLANIdValue, sizeof (objDynamicVLANIdValue));



  /* return the object value: DynamicVLANId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicVLANIdValue,

                           sizeof (objDynamicVLANIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_DynamicMACAddress

*

* @purpose Get 'DynamicMACAddress'

*

* @description [DynamicMACAddress]: Source MAC address of the packet that

*              is received on the dynamically locked port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_DynamicMACAddress (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objDynamicMACAddressValue;

  L7_enetMacAddr_t  macAddr;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t DynamicVLANIdValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));

  memcpy(&macAddr.addr, objDynamicMACAddressValue, L7_ENET_MAC_ADDR_LEN);

  /* get the value from application */

  owa.l7rc =

    usmDbPmlIntfDynamicEntryGetFirst (L7_UNIT_CURRENT,

                                     keyInterfaceValue,

                                     &macAddr,

                                     (xLibU16_t *) &DynamicVLANIdValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objDynamicMACAddressValue, strlen (objDynamicMACAddressValue));



  memset(objDynamicMACAddressValue, 0x00, L7_ENET_MAC_ADDR_LEN);

  memcpy(objDynamicMACAddressValue, macAddr.addr, sizeof(L7_enetMacAddr_t));



  /* return the object value: DynamicMACAddress */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDynamicMACAddressValue,

                           strlen (objDynamicMACAddressValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

#endif



/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_GlobalPortSecurityMode

*

* @purpose Get 'GlobalPortSecurityMode'

*

* @description [GlobalPortSecurityMode]: Mode showing whether at the global

*              level, port security is enabled or not 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_GlobalPortSecurityMode (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objGlobalPortSecurityModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbPmlAdminModeGet (L7_UNIT_CURRENT, &objGlobalPortSecurityModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalPortSecurityModeValue,

                     sizeof (objGlobalPortSecurityModeValue));



  /* return the object value: GlobalPortSecurityMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalPortSecurityModeValue,

                           sizeof (objGlobalPortSecurityModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_GlobalPortSecurityMode

*

* @purpose Set 'GlobalPortSecurityMode'

*

* @description [GlobalPortSecurityMode]: Mode showing whether at the global

*              level, port security is enabled or not 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_GlobalPortSecurityMode (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objGlobalPortSecurityModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: GlobalPortSecurityMode */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objGlobalPortSecurityModeValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalPortSecurityModeValue, owa.len);



  /* set the value in application */

  owa.l7rc =

    usmDbPmlAdminModeSet (L7_UNIT_CURRENT, objGlobalPortSecurityModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_Switchingpml_DynamicMaxLimit

*

* @purpose Get 'DynamicMaxLimit'

*

* @description [DynamicMaxLimit]: The Port MAC Locking maximum dynamic limit

*              for the specified interface 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_DynamicMaxLimit (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDynamicMaxLimitValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfDynamicMaxLimitGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                             &objDynamicMaxLimitValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: DynamicMaxLimit */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicMaxLimitValue,

                           sizeof (objDynamicMaxLimitValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_Switchingpml_StaticCount

*

* @purpose Get 'StaticCount'

*

* @description [StaticCount]: The Port MAC Locking current static entries

*              used count for the specified interface 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_StaticCount (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStaticCountValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfStaticCountGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                         &objStaticCountValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StaticCount */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticCountValue,

                           sizeof (objStaticCountValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************

* @function fpObjGet_Switchingpml_DynamicCount

*

* @purpose Get 'DynamicCount'

*

* @description [DynamicCount]: The Port MAC Locking current Dynamic entries

*              used count for the specified interface

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_DynamicCount (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDynamicCountValue = 0;

  L7_enetMacAddr_t macAddr;

  xLibU16_t vlanId;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */



  owa.l7rc = usmDbPmlIntfDynamicEntryGetFirst(0, keyInterfaceValue, &macAddr, &vlanId);



  while (owa.l7rc == L7_SUCCESS)

  {

    objDynamicCountValue++;

    owa.l7rc = usmDbPmlIntfDynamicEntryGetNext(0, keyInterfaceValue, &macAddr, &vlanId);

  }



  /* return the object value: StaticCount */

   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicCountValue,

                           sizeof (objDynamicCountValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_Switchingpml_StaticMaxLimit

*

* @purpose Get 'StaticMaxLimit'

*

* @description [StaticMaxLimit]: The Port MAC Locking maximum static limit

*              for the specified interface 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_StaticMaxLimit (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStaticMaxLimitValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbPmlIntfStaticMaxLimitGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                            &objStaticMaxLimitValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StaticMaxLimit */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticMaxLimitValue,

                           sizeof (objStaticMaxLimitValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************

* @function fpObjGet_SwitchingPortSecurityGroup_CheckStaticLimit

*

* @purpose Get 'StaticMaxLimit'

*

* @description [CheckStaticLimit]: The Port MAC Locking check static limit

*              for the specified interface 

*

* @return
*******************************************************************************/

xLibRC_t fpObjGet_SwitchingPortSecurityGroup_CheckStaticLimit (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCheckStaticLimitValue;
  L7_enetMacAddr_t    macAddr;
  L7_ushort16         vlanId;
  L7_uint32           count = 0;
  L7_uint32           CheckStaticLimit;
  L7_RC_t             rc = L7_FAILURE;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  rc = usmDbPmlIntfStaticLimitGet(L7_UNIT_CURRENT, keyInterfaceValue, &CheckStaticLimit);
  if (CheckStaticLimit == 0)
    objCheckStaticLimitValue =XLIB_TRUE;

  rc = usmDbPmlIntfStaticEntryGetFirst(L7_UNIT_CURRENT,keyInterfaceValue,&macAddr, &vlanId);

  while (rc == L7_SUCCESS)
  {
    rc = usmDbPmlIntfStaticEntryGetNext(L7_UNIT_CURRENT, keyInterfaceValue, &macAddr, &vlanId);
    count++;
  }
  if ( (CheckStaticLimit !=0) && (CheckStaticLimit == count) )
    objCheckStaticLimitValue =XLIB_TRUE;
  else
    objCheckStaticLimitValue =XLIB_FALSE;
  
 /* return the object value: StaticMaxLimit */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objCheckStaticLimitValue,
                           sizeof (objCheckStaticLimitValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


#if 0

/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_MACAddressAdd

*

* @purpose Set 'MACAddressAdd'

*

* @description [MACAddressAdd] This MIB variable accepts a VLAN id and MAC address 

* to be added to the list of statically locked MAC addresses on a port. The VLAN 

* id and MAC address combination would be entered in a particular fashion like 

* :- 2 a0:b0:c0:d1:e2:a1(the vlan-id and MAC addr

*

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_MACAddressAdd (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objMACAddressAddValue;



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue, vlanID = 0, len;



  xLibS8_t strMacAddr[L7_ENET_MAC_ADDR_LEN];

  L7_enetMacAddr_t macAddr;

  xLibU8_t *vidBuf;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: MACAddressAdd */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMACAddressAddValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objMACAddressAddValue, owa.len);



  vidBuf = strtok(objMACAddressAddValue, "-");



  if (cliWebXuiConvertTo32BitUnsignedInteger(vidBuf, &vlanID) != L7_SUCCESS)

  {

      return XLIBRC_FAILURE;

  }



  len = strlen(vidBuf);



  memset (strMacAddr, 0x0, sizeof(strMacAddr) );



  strMacAddr[0] = objMACAddressAddValue[len+1];

  strMacAddr[1] = objMACAddressAddValue[len+2];

  strMacAddr[2] = objMACAddressAddValue[len+3];

  strMacAddr[3] = objMACAddressAddValue[len+4];

  strMacAddr[4] = objMACAddressAddValue[len+5];

  strMacAddr[5] = objMACAddressAddValue[len+6];



  memcpy(&macAddr.addr, strMacAddr, L7_ENET_MAC_ADDR_LEN);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfStaticEntryAdd(L7_UNIT_CURRENT, keyInterfaceValue, (L7_enetMacAddr_t)macAddr, (L7_ushort16)vlanID);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_StaticEntryDelete

*

* @purpose Set 'StaticEntryDelete'

*

* @description [StaticEntryDelete] Deletes a statically locked entry from an interface.

*

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_StaticEntryDelete (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStaticEntryDeleteValue;



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue, vlanID, len;



  xLibU8_t *vidBuf;

  xLibS8_t strMacAddr[L7_ENET_MAC_ADDR_LEN];

  L7_enetMacAddr_t macAddr;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StaticEntryDelete */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objStaticEntryDeleteValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objStaticEntryDeleteValue, owa.len);



  vidBuf = strtok(objStaticEntryDeleteValue, "-");



  if (cliWebXuiConvertTo32BitUnsignedInteger(vidBuf, &vlanID) != L7_SUCCESS)

  {

      return XLIBRC_FAILURE;

  }



  len = strlen(vidBuf);



  memset (strMacAddr, 0x0, sizeof(strMacAddr) );



  strMacAddr[0] = objStaticEntryDeleteValue[len+1];

  strMacAddr[1] = objStaticEntryDeleteValue[len+2];

  strMacAddr[2] = objStaticEntryDeleteValue[len+3];

  strMacAddr[3] = objStaticEntryDeleteValue[len+4];

  strMacAddr[4] = objStaticEntryDeleteValue[len+5];

  strMacAddr[5] = objStaticEntryDeleteValue[len+6];



  memcpy(&macAddr.addr, &strMacAddr, L7_ENET_MAC_ADDR_LEN);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfStaticEntryDelete (L7_UNIT_CURRENT, keyInterfaceValue, (L7_enetMacAddr_t)macAddr, (L7_ushort16) vlanID);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

#endif

/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_MACAddressAdd

*

* @purpose Set 'MACAddressAdd'

*

* @description [MACAddressAdd] This MIB variable accepts a VLAN id and MAC address 

* to be added to the list of statically locked MAC addresses on a port. The VLAN 

* id and MAC address combination would be entered in a particular fashion like 

* :- 2 a0:b0:c0:d1:e2:a1(the vlan-id and MAC addr

*

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_MACAddressAdd (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr6_t objMACAddressAddValue;

  L7_enetMacAddr_t macAddr;



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t keyVlanIdValue;

  xLibU16_t vlanID;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: MACAddressAdd */

  owa.len = sizeof (objMACAddressAddValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMACAddressAddValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objMACAddressAddValue, owa.len);



  memset(&macAddr, 0x0, sizeof(macAddr));

  memcpy(&macAddr.addr, objMACAddressAddValue, L7_ENET_MAC_ADDR_LEN);



  if(!(macAddr.addr[0]|macAddr.addr[1]|macAddr.addr[2]|

       macAddr.addr[3]|macAddr.addr[4]|macAddr.addr[5]))

  {

    owa.rc = XLIBRC_INVALID_NON_ZERO_MAC;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  

  /* retrieve key: Interface */

  kwaInterface.len = sizeof (keyInterfaceValue);

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* retrieve key: VlanId */

  owa.len = sizeof (keyVlanIdValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_VlanId,

                                   (xLibU8_t *) & keyVlanIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, owa.len);



  vlanID = keyVlanIdValue;



  /* set the value in application */

  owa.l7rc = usmDbPmlIntfStaticEntryAdd(L7_UNIT_CURRENT, keyInterfaceValue, macAddr, vlanID);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingPortSecurityGroup_VlanId

*

* @purpose Set 'VlanId'

*

* @description [VlanId] 

*

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingPortSecurityGroup_VlanId (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t objVlanIdValue;



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: VlanId */

  owa.len = sizeof (objVlanIdValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)&objVlanIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objVlanIdValue, owa.len);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



 }
/*******************************************************************************
* @function fpObjList_SwitchingPortSecurityGroup_VlanId
*
* @purpose List 'VlanId'
 *@description  [VlanId] This MIB variable accepts a VLAN id and MAC address to
* be added to the list of statically locked MAC addresses on a
* port. The VLAN id and MAC address combination would be entered in a
* particular fashion like :- 2 a0:b0:c0:d1:e2:a1(the vlan-id and MAC
* addr   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingPortSecurityGroup_VlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVlanIdValue;
  xLibU32_t nextObjVlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objVlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPortSecurityGroup_VlanId,
                          (xLibU8_t *) & objVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIdValue, owa.len);
  owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, objVlanIdValue, &nextObjVlanIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIdValue, owa.len);

  /* return the object value: VlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIdValue, sizeof (nextObjVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
