/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingVlanPortConfig.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to VLan-object.xml

*

* @create  13 February 2008

*

* @author  Radha K

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingVlanPortConfig_obj.h"

#include "usmdb_mib_vlan_api.h"

#include "usmdb_garp.h"

#include "usmdb_util_api.h"

#include "usmdb_dot1q_api.h"



extern L7_RC_t usmDbConvertIntfListToMask(L7_uint32 *list, L7_uint32 listNum, L7_INTF_MASK_t *mask);



/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_Interface

*

* @purpose Get 'Interface'

*

* @description [Interface]: Specifies all configurable interfaces for VLAN

*              parameters. 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_Interface (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objInterfaceValue;

  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & objInterfaceValue, &owa.len);



  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objInterfaceValue = 0;

    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,

                                 0, &nextObjInterfaceValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

   owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,

                                               0, objInterfaceValue, &nextObjInterfaceValue);

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

                           sizeof (objInterfaceValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_Pvid

*

* @purpose Get 'Pvid'

*

* @description [Pvid]: The PVID, the VLAN ID assigned to untagged frames or

*              Priority-Tagged frames received on this port 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_Pvid (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPvidValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbQportsPVIDGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                 &objPvidValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: Pvid */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPvidValue,

                           sizeof (objPvidValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_Pvid

*

* @purpose Set 'Pvid'

*

* @description [Pvid]: The PVID, the VLAN ID assigned to untagged frames or

*              Priority-Tagged frames received on this port 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_Pvid (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPvidValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: Pvid */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPvidValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objPvidValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbQportsPVIDSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                 objPvidValue);

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

* @function fpObjGet_SwitchingVlanPortConfig_AcceptableFrameTypes

*

* @purpose Get 'AcceptableFrameTypes'

*

* @description [AcceptableFrameTypes]: When this is admitOnlyVlanTagged(2)

*              the device will discard untagged frames or Priority-Tagged

*              frames received on this port. When admitAll(1), untagged frames

*              or Priority-Tagged frames received on this port will

*              be accepted and assigned 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_AcceptableFrameTypes (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objAcceptableFrameTypesValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbQportsAcceptFrameTypeGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                            &objAcceptableFrameTypesValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: AcceptableFrameTypes */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAcceptableFrameTypesValue,

                           sizeof (objAcceptableFrameTypesValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_AcceptableFrameTypes

*

* @purpose Set 'AcceptableFrameTypes'

*

* @description [AcceptableFrameTypes]: When this is admitOnlyVlanTagged(2)

*              the device will discard untagged frames or Priority-Tagged

*              frames received on this port. When admitAll(1), untagged frames

*              or Priority-Tagged frames received on this port will

*              be accepted and assigned 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_AcceptableFrameTypes (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objAcceptableFrameTypesValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: AcceptableFrameTypes */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objAcceptableFrameTypesValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objAcceptableFrameTypesValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbQportsAcceptFrameTypeSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                            objAcceptableFrameTypesValue);

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

* @function fpObjGet_SwitchingVlanPortConfig_IngressFiltering

*

* @purpose Get 'IngressFiltering'

*

* @description [IngressFiltering]:  When this is true(1) the device will discard

*              incoming frames for VLANs which do not include this

*              Port in its Member set. When false(2), the port will accept

*              all incoming frames. This control does not affect VLAN independent

*              BPDU 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_IngressFiltering (void *wap,

                                                            void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objIngressFilteringValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbQportsEnableIngressFilteringGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                          &objIngressFilteringValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: IngressFiltering */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIngressFilteringValue,

                           sizeof (objIngressFilteringValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_IngressFiltering

*

* @purpose Set 'IngressFiltering'

*

* @description [IngressFiltering]:  When this is true(1) the device will discard

*              incoming frames for VLANs which do not include this

*              Port in its Member set. When false(2), the port will accept

*              all incoming frames. This control does not affect VLAN independent

*              BPDU 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_IngressFiltering (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objIngressFilteringValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: IngressFiltering */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objIngressFilteringValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objIngressFilteringValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

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

    usmDbQportsEnableIngressFilteringSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                          objIngressFilteringValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





#ifdef L7_FEAT_GMRP

/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_GvrpStatus

*

* @purpose Get 'GvrpStatus'

*

* @description [GvrpStatus]: The state of GVRP operation on this port. The

*              value enabled(1) indicates that GVRP is enabled on this port,

*              as long asGlobal GvrpStatus is also enabled for this device.

*              When disabled(2) but global GVRP Status is still enabled

*              for the device, GVRP 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_GvrpStatus (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objGvrpStatusValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbQportsEnableGVRPGet (L7_UNIT_CURRENT, keyInterfaceValue,

                                       &objGvrpStatusValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: GvrpStatus */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpStatusValue,

                           sizeof (objGvrpStatusValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_GvrpStatus

*

* @purpose Set 'GvrpStatus'

*

* @description [GvrpStatus]: The state of GVRP operation on this port. The

*              value enabled(1) indicates that GVRP is enabled on this port,

*              as long asGlobal GvrpStatus is also enabled for this device.

*              When disabled(2) but global GVRP Status is still enabled

*              for the device, GVRP 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_GvrpStatus (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objGvrpStatusValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: GvrpStatus */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpStatusValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objGvrpStatusValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbQportsEnableGVRPSet (L7_UNIT_CURRENT, keyInterfaceValue,

                                       objGvrpStatusValue);

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

* @function fpObjGet_SwitchingVlanPortConfig_GvrpFailedRegistrations

*

* @purpose Get 'GvrpFailedRegistrations'

*

* @description [GvrpFailedRegistrations]: The total number of failed GVRP

*              registrations, for any reason, on this port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_GvrpFailedRegistrations (void *wap,

                                                                   void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objGvrpFailedRegistrationsValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1qPortGvrpFailedRegistrations (L7_UNIT_CURRENT, keyInterfaceValue,

                                           &objGvrpFailedRegistrationsValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: GvrpFailedRegistrations */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpFailedRegistrationsValue,

                           sizeof (objGvrpFailedRegistrationsValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_GvrpLastPduOrigin

*

* @purpose Get 'GvrpLastPduOrigin'

*

* @description [GvrpLastPduOrigin]: The Source MAC Address of the last GVRP

*              message received on this port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_GvrpLastPduOrigin (void *wap,

                                                             void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objGvrpLastPduOriginValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1qPortGvrpLastPduOrigin (L7_UNIT_CURRENT, keyInterfaceValue,

                                     objGvrpLastPduOriginValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: GvrpLastPduOrigin */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGvrpLastPduOriginValue,

                           strlen (objGvrpLastPduOriginValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_GvrpPortRegistration

*

* @purpose Get 'GvrpPortRegistration'

 *@description  [GvrpPortRegistration] <HTML>Todo

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_GvrpPortRegistration (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objGvrpPortRegistrationValue;



  xLibU32_t keyInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* get the value from application */

  owa.l7rc = usmDbGvrpPortRegistrationForbidGet (keyInterfaceValue, &objGvrpPortRegistrationValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_VALUE (bufp, &objGvrpPortRegistrationValue, sizeof (objGvrpPortRegistrationValue));



  /* return the object value: GvrpPortRegistration */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpPortRegistrationValue,

                           sizeof (objGvrpPortRegistrationValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_GvrpPortRegistration

*

* @purpose Set 'GvrpPortRegistration'

 *@description  [GvrpPortRegistration] <HTML>Todo

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_GvrpPortRegistration (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objGvrpPortRegistrationValue;



  xLibU32_t keyInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: GvrpPortRegistration */

  owa.len = sizeof (objGvrpPortRegistrationValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpPortRegistrationValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objGvrpPortRegistrationValue, owa.len);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbGvrpPortRegistrationForbidSet (keyInterfaceValue, objGvrpPortRegistrationValue);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}





/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_GvrpPortVlanCreation

*

* @purpose Get 'GvrpPortVlanCreation'

 *@description  [GvrpPortVlanCreation] <HTML>Todo

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_GvrpPortVlanCreation (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objGvrpPortVlanCreationValue;



  xLibU32_t keyInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* get the value from application */

  owa.l7rc = usmDbGvrpVlanCreationForbidGet (keyInterfaceValue, &objGvrpPortVlanCreationValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_VALUE (bufp, &objGvrpPortVlanCreationValue, sizeof (objGvrpPortVlanCreationValue));



  /* return the object value: GvrpPortVlanCreation */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGvrpPortVlanCreationValue,

                           sizeof (objGvrpPortVlanCreationValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_GvrpPortVlanCreation

*

* @purpose Set 'GvrpPortVlanCreation'

 *@description  [GvrpPortVlanCreation] <HTML>Todo

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_GvrpPortVlanCreation (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objGvrpPortVlanCreationValue;



  xLibU32_t keyInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: GvrpPortVlanCreation */

  owa.len = sizeof (objGvrpPortVlanCreationValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGvrpPortVlanCreationValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objGvrpPortVlanCreationValue, owa.len);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbGvrpPortVlanCreationForbidSet (keyInterfaceValue, objGvrpPortVlanCreationValue);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}





#endif



/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_CurrentPVID

*

* @purpose Get 'CurrentPVID'

 *@description  [CurrentPVID] The Current VID configuration for a port

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_CurrentPVID (void *wap, void *bufp)

{



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objCurrentPVIDValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* get the value from application */

  owa.l7rc = usmDbQportsCurrentPVIDGet(L7_UNIT_CURRENT, keyInterfaceValue, &objCurrentPVIDValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: CurrentPVID */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCurrentPVIDValue, sizeof (objCurrentPVIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_CurrentIngressFiltering

*

* @purpose Get 'CurrentIngressFiltering'

 *@description  [CurrentIngressFiltering] The Current Ingress Filtering

* configuration for a port

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_CurrentIngressFiltering (void *wap, void *bufp)

{



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objCurrentIngressFilteringValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* get the value from application */

  owa.l7rc = usmDbQportsCurrentIngressFilteringGet(L7_UNIT_CURRENT, keyInterfaceValue, &objCurrentIngressFilteringValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: CurrentIngressFiltering */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCurrentIngressFilteringValue,

                           sizeof (objCurrentIngressFilteringValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingVlanPortConfig_PortVlanMode

*

* @purpose Get 'PortVlanMode'

*

* @description [PortVlanMode]: PortVlanMode

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_PortVlanMode (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPortVlanMode;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  



  /* get the value from application */

  owa.l7rc = usmDbDot1qSwPortModeGet( L7_UNIT_CURRENT, keyInterfaceValue, &objPortVlanMode);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: AcceptableFrameTypes */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortVlanMode,

                           sizeof (objPortVlanMode));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_PortVlanMode

*

* @purpose Set 'PortVlanMode'

*

* @description [PortVlanMode]: PortVlanMode 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_PortVlanMode (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPortVlanMode;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  L7_INTF_MASK_t portMask;

  L7_uint32 intfList[L7_FILTER_MAX_INTF]={0};

  L7_uint32 numIntf;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: AcceptableFrameTypes */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objPortVlanMode,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objPortVlanMode, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  numIntf = 0;

  intfList[numIntf] = keyInterfaceValue;

  numIntf++;

  memset(&portMask, 0, sizeof(portMask));

  if (numIntf != 0)

  { 

    owa.l7rc =  usmDbConvertIntfListToMask(intfList, numIntf, &portMask);

    if (owa.l7rc != L7_SUCCESS)

    {

      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

      FPOBJ_TRACE_EXIT (bufp, owa);

      return owa.rc;

    }

  }





  /* set the value in application */

  owa.l7rc = usmDbDot1qSwPortModeSet (L7_UNIT_CURRENT, (NIM_INTF_MASK_t * )&portMask,

                                            objPortVlanMode);

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

* @function fpObjGet_SwitchingVlanPortConfig_Pvid

*

* @purpose Get 'Pvid'

*

* @description [Pvid]: The PVID, the VLAN ID assigned to untagged frames or

*              Priority-Tagged frames received on this port 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingVlanPortConfig_PortVlanModePvid (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPortVlanMode;

  xLibU32_t objPvidValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  kwa.len = sizeof(xLibU32_t);

  /* retrieve port vlan mode */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_PortVlanMode,

                          (xLibU8_t *) & objPortVlanMode, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }



  /* get the value from application */

  if (objPortVlanMode == DOT1Q_SWPORT_MODE_ACCESS)

  {

    owa.l7rc = usmDbDot1qSwPortAccessVlanGet (L7_UNIT_CURRENT, keyInterfaceValue, &objPvidValue);

  }

  else if (objPortVlanMode == DOT1Q_SWPORT_MODE_GENERAL)

  {

    owa.l7rc = usmDbQportsPVIDGet (L7_UNIT_CURRENT, keyInterfaceValue, &objPvidValue);

  }

  else

  {

    owa.l7rc = L7_FAILURE;

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: Pvid */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPvidValue,

                           sizeof (objPvidValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingVlanPortConfig_Pvid

*

* @purpose Set 'Pvid'

*

* @description [Pvid]: The PVID, the VLAN ID assigned to untagged frames or

*              Priority-Tagged frames received on this port 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingVlanPortConfig_PortVlanModePvid (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPvidValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t objPortVlanMode;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: Pvid */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPvidValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objPvidValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  kwa.len = sizeof(xLibU32_t );

 /* retrieve port vlan mode */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanPortConfig_PortVlanMode,

                          (xLibU8_t *) & objPortVlanMode, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  if (objPortVlanMode == DOT1Q_SWPORT_MODE_ACCESS)
  {

    if (usmDbVlanIDGet(L7_UNIT_CURRENT, objPvidValue) == L7_SUCCESS)

    {

      owa.l7rc = usmDbDot1qSwPortAccessVlanSet(L7_UNIT_CURRENT, keyInterfaceValue, objPvidValue);

    }

    else

    {

      owa.rc = XLIBRC_VLAN_ID_DOES_NOT_EXISTS;    /* TODO: Change if required */

      FPOBJ_TRACE_EXIT (bufp, owa);

      return owa.rc;

    }

  }

  else if (objPortVlanMode == DOT1Q_SWPORT_MODE_GENERAL)

  {

    owa.l7rc = usmDbQportsPVIDSet(L7_UNIT_CURRENT, keyInterfaceValue, objPvidValue);

  }

  else

  {

    owa.l7rc = L7_FAILURE;

  }


  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}





