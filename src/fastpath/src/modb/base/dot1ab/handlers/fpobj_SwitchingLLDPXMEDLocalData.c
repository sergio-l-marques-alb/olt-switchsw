/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingLLDPXMEDLocalData.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to LLDP-object.xml

*

* @create  11 February 2008

*

* @author  Radha K

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingLLDPXMEDLocalData_obj.h"

#include "usmdb_lldp_api.h"

#include "usmdb_util_api.h"







/*********************************************************************

* @purpose To find out if and interface is enabled for LLDP transmits

* @param   {{input}}intIfNum  internal interface number of the interface

*                             in question

* @returns L7_TRUE  Valid Local LLDP interface - Transmits enabled

*          L7_FALSE

* @end

*********************************************************************/

L7_BOOL fpObjGet_lldpMedIsValidLldpLocalIntf(L7_uint32 intIfNum)

{

  L7_uint32 txMode;



  if ((usmDbLldpIntfTxModeGet(intIfNum, &txMode) == L7_SUCCESS)

      && (txMode == L7_ENABLE))

  {

    return L7_TRUE;

  }

  return L7_FALSE;

}





/*********************************************************************

*

* @purpose Get Next application type

*

* @param    intIfNum Interface number

*           Media Policy Application Type

*

* @returns  L7_SUCCESS or L7_FAILURE

*

* @end

*********************************************************************/

L7_uint32 fpObjGet_LldpNetApplicationTypeGetNext(L7_uint32 intIfNum, L7_uint32 appType, L7_uint32 *appTypeNext)

{

   lldpXMedCapabilities_t  capGet;

   L7_uint32               tempVal = 0;



   if(usmDbLldpXMedPortConfigTLVsEnabledGet(intIfNum, &capGet) == L7_SUCCESS)

   {

      if(capGet.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)

      {

         if(appType < videoSignaling_policyapptype)

         {

            *appTypeNext = appType+1;

            if(usmDbLldpXMedLocMediaPolicyVlanIDGet(intIfNum, *appTypeNext, &tempVal) == L7_SUCCESS)

            {

               return L7_SUCCESS;

            }

         }

      }

   }

   else

   {

      return L7_FAILURE;

   }

   return L7_FAILURE;

}









/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_LocalPortNum

*

* @purpose Get 'LocalPortNum'

*

* @description [LocalPortNum]: Local Port Number 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_LocalPortNum (void *wap,

                                                           void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLocalPortNumValue;

  xLibU32_t nextObjLocalPortNumValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                          (xLibU8_t *) & objLocalPortNumValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbLldpValidIntfFirstGet ( &nextObjLocalPortNumValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocalPortNumValue, owa.len);

    owa.l7rc = usmDbLldpValidIntfNextGet ( objLocalPortNumValue,

                                           &nextObjLocalPortNumValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  do

  {

    if (fpObjGet_lldpMedIsValidLldpLocalIntf(nextObjLocalPortNumValue) == L7_TRUE)

    {

      break;

    }

    objLocalPortNumValue = nextObjLocalPortNumValue; 

    owa.l7rc = usmDbLldpValidIntfNextGet ( objLocalPortNumValue,

                                           &nextObjLocalPortNumValue);

  }while(owa.l7rc == L7_SUCCESS);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLocalPortNumValue, owa.len);



  /* return the object value: LocalPortNum */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLocalPortNumValue,

                           sizeof (objLocalPortNumValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyAppType

*

* @purpose Get 'MediaPolicyAppType'

*

* @description [MediaPolicyAppType]: The media type that defines the primary

*              function of the application for the policy advertised by

*              an endpoint. 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyAppType (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t objMediaPolicyAppTypeValue;

  xLibU32_t nextObjMediaPolicyAppTypeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);



  /* retrieve key: MediaPolicyAppType */

  owa.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_MediaPolicyAppType,

                   (xLibU8_t *) &objMediaPolicyAppTypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objMediaPolicyAppTypeValue = 0;

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, objMediaPolicyAppTypeValue, owa.len);

  }



  owa.l7rc = fpObjGet_LldpNetApplicationTypeGetNext(keyLocalPortNumValue,objMediaPolicyAppTypeValue,

                                                                &nextObjMediaPolicyAppTypeValue);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMediaPolicyAppTypeValue, owa.len);



  /* return the object value: MediaPolicyAppType */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjMediaPolicyAppTypeValue,

                           sizeof (nextObjMediaPolicyAppTypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyVlanID

*

* @purpose Get 'MediaPolicyVlanID'

*

* @description [MediaPolicyVlanID]: An extension of the VLAN Identifier for

*              the port, as defined in IEEE 802.1P-1998. A value of 1 through

*              4094 is used to define a valid PVID. A value of 0 shall

*              be used if the device is using priority tagged frames, meaning

*              that only the 802.1p priority lev 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyVlanID (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t keyMediaPolicyAppTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMediaPolicyVlanIDValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);



  /* retrieve key: MediaPolicyAppType */

  kwa2.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_MediaPolicyAppType,

                   (xLibU8_t *) &keyMediaPolicyAppTypeValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  

  owa.l7rc =

    usmDbLldpXMedLocMediaPolicyVlanIDGet (keyLocalPortNumValue,

                                          keyMediaPolicyAppTypeValue,

                                          &objMediaPolicyVlanIDValue);

                                          

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: MediaPolicyVlanID */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMediaPolicyVlanIDValue,

                           sizeof (objMediaPolicyVlanIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyPriority

*

* @purpose Get 'MediaPolicyPriority'

*

* @description [MediaPolicyPriority]: This object contains the value of the

*              802.1p priority which is associated with the given port on

*              the local system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyPriority (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t keyMediaPolicyAppTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMediaPolicyPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);



  /* retrieve key: MediaPolicyAppType */

  kwa2.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_MediaPolicyAppType,

                   (xLibU8_t *) &keyMediaPolicyAppTypeValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocMediaPolicyPriorityGet (keyLocalPortNumValue,

                                            keyMediaPolicyAppTypeValue,

                                            &objMediaPolicyPriorityValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: MediaPolicyPriority */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMediaPolicyPriorityValue,

                           sizeof (objMediaPolicyPriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyDscp

*

* @purpose Get 'MediaPolicyDscp'

*

* @description [MediaPolicyDscp]: This object contains the value of the Differentiated

*              Service Code Point (DSCP) as defined in IETF RFC

*              2474 and RFC 2475 which is associated with the given port

*              on the local system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyDscp (void *wap,

                                                              void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t keyMediaPolicyAppTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMediaPolicyDscpValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);



  /* retrieve key: MediaPolicyAppType */

  kwa2.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_MediaPolicyAppType,

                   (xLibU8_t *) &keyMediaPolicyAppTypeValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocMediaPolicyDscpGet (keyLocalPortNumValue,

                                        keyMediaPolicyAppTypeValue,

                                        &objMediaPolicyDscpValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: MediaPolicyDscp */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMediaPolicyDscpValue,

                           sizeof (objMediaPolicyDscpValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyUnknown

*

* @purpose Get 'MediaPolicyUnknown'

*

* @description [MediaPolicyUnknown]: A value of 'true' indicates that the

*              network policy for the specified application type is currently

*              unknown. In this case, the VLAN ID, the layer 2 priority

*              and the DSCP value fields are ignored. A value of 'false'

*              indicates that this network policy is 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyUnknown (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t keyMediaPolicyAppTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMediaPolicyUnknownValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);



  /* retrieve key: MediaPolicyAppType */

  kwa2.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_MediaPolicyAppType,

                   (xLibU8_t *) &keyMediaPolicyAppTypeValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocMediaPolicyUnknownGet (keyLocalPortNumValue,

                                           keyMediaPolicyAppTypeValue,

                                           &objMediaPolicyUnknownValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: MediaPolicyUnknown */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMediaPolicyUnknownValue,

                           sizeof (objMediaPolicyUnknownValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyTagged

*

* @purpose Get 'MediaPolicyTagged'

*

* @description [MediaPolicyTagged]: A value of 'true' indicates that the application

*              is using a tagged VLAN. A value of 'false' indicates

*              that for the specific application the device either is

*              using an untagged VLAN or does not support port based VLAN

*              operation. In this case, both the VLAN 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MediaPolicyTagged (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t keyMediaPolicyAppTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMediaPolicyTaggedValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);



  /* retrieve key: MediaPolicyAppType */

  kwa2.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_MediaPolicyAppType,

                   (xLibU8_t *) &keyMediaPolicyAppTypeValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocMediaPolicyTaggedGet(keyLocalPortNumValue,

                                          keyMediaPolicyAppTypeValue,

                                          &objMediaPolicyTaggedValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: MediaPolicyTagged */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMediaPolicyTaggedValue,

                           sizeof (objMediaPolicyTaggedValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_HardwareRev

*

* @purpose Get 'HardwareRev'

*

* @description [HardwareRev]: The vendor-specific hardware revision string

*              as advertised by the endpoint. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_HardwareRev (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objHardwareRevValue;

  FPOBJ_TRACE_ENTER (bufp);

  

  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocHardwareRevGet (objHardwareRevValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objHardwareRevValue, strlen (objHardwareRevValue));



  /* return the object value: HardwareRev */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objHardwareRevValue,

                           strlen (objHardwareRevValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_LldpXMedLocAssetID

*

* @purpose Get 'LldpXMedLocAssetID'

*

* @description [LldpXMedLocAssetID]: The Asset ID 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_LldpXMedLocAssetID(void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibStr256_t objLldpXMedLocAssetIDValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocAssetIDGet ((xLibU8_t *)objLldpXMedLocAssetIDValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objLldpXMedLocAssetIDValue, strlen(objLldpXMedLocAssetIDValue));



  /* return the object value: PoEPDPowerReq */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLldpXMedLocAssetIDValue,

                           strlen (objLldpXMedLocAssetIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_LldpXMedInterface

*

* @purpose Get 'LldpXMedInterface'

*

* @description [LldpXMedInterface]: An interface on the Local System. 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_LldpXMedInterface(void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLldpXMedInterfaceValue;

  xLibU32_t nextObjLldpXMedInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedInterface,

                          (xLibU8_t *) & objLldpXMedInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjLldpXMedInterfaceValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLldpXMedInterfaceValue, owa.len);

    owa.l7rc = usmDbGetNextVisibleExtIfNumber ( objLldpXMedInterfaceValue,

                                                &nextObjLldpXMedInterfaceValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLldpXMedInterfaceValue, owa.len);



  /* return the object value: LocalPortNum */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLldpXMedInterfaceValue,

                           sizeof (objLldpXMedInterfaceValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_LldpXMedLocLocationSubType

*

* @purpose Get 'LldpXMedLocLocationSubType'

*

* @description [LldpXMedLocLocationSubType]: The location subtype advertised

*              by the local device. 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_LldpXMedLocLocationSubType(void *wap, void *bufp) 

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLldpXMedLocLocationSubTypeValue;

  xLibU32_t nextObjLldpXMedLocLocationSubTypeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LldpXMedLocLocationSubType */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedLocLocationSubType,

                          (xLibU8_t *) & objLldpXMedLocLocationSubTypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjLldpXMedLocLocationSubTypeValue = L7_COORDINATEBASED_LOCSUBTYPE;

    owa.l7rc = L7_SUCCESS;

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLldpXMedLocLocationSubTypeValue, owa.len);



    if (objLldpXMedLocLocationSubTypeValue == L7_ELIN_LOCSUBTYPE)

    {

      owa.l7rc = L7_FAILURE;

    }

    else

    {

      nextObjLldpXMedLocLocationSubTypeValue = objLldpXMedLocLocationSubTypeValue + 1;

      owa.l7rc = L7_SUCCESS;    

    }

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLldpXMedLocLocationSubTypeValue, owa.len);



  /* return the object value: LldpXMedLocLocationSubType */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLldpXMedLocLocationSubTypeValue,

                           sizeof (objLldpXMedLocLocationSubTypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_lldpXMedLocLocationInfo

*

* @purpose Get 'lldpXMedLocLocationInfo'

*

* @description [lldpXMedLocLocationInfo]: The location information.. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_lldpXMedLocLocationInfo(void *wap, void *bufp) 

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLldpXMedInterfaceValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t keyLldpXMedLocLocationSubTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objlldpXMedLocLocationInfoValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LldpXMedInterface */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedInterface,

                           (xLibU8_t *) & keyLldpXMedInterfaceValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwa1.len);



  /* retrieve key: XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedLocLocationSubType */

  kwa2.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedLocLocationSubType,

                   (xLibU8_t *) &keyLldpXMedLocLocationSubTypeValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedLocLocationSubTypeValue, kwa2.len);



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocLocationInfoGet (keyLldpXMedInterfaceValue,

                                            (xLibU32_t)keyLldpXMedLocLocationSubTypeValue,

                                            objlldpXMedLocLocationInfoValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: objlldpXMedLocLocationInfoValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objlldpXMedLocLocationInfoValue,

                           sizeof (objlldpXMedLocLocationInfoValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingLLDPXMEDLocalData_lldpXMedLocLocationInfo

*

* @purpose Set 'lldpXMedLocLocationInfo'

*

* @description [lldpXMedLocLocationInfo]: The location information.. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLLDPXMEDLocalData_lldpXMedLocLocationInfo(void *wap, void *bufp) 

{

  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibStr256_t));

  xLibStr256_t objlldpXMedLocLocationInfoValue;

  fpObjWa_t kwaLldpXMedInterface = FPOBJ_INIT_WA(sizeof(xLibU32_t));

  xLibU32_t keyLldpXMedInterfaceValue;

  fpObjWa_t kwaLldpXMedLocLocationSubType = FPOBJ_INIT_WA(sizeof(xLibU32_t));

  xLibU32_t keyLldpXMedLocLocationSubTypeValue;



  FPOBJ_TRACE_ENTER(bufp);



  /* retrieve object: lldpXMedLocLocationInfo */

  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&objlldpXMedLocLocationInfoValue, &owa.len);



  if(owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT(bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_VALUE (bufp, &objlldpXMedLocLocationInfoValue, owa.len);



  /* retrieve key: LldpXMedInterface */

  kwaLldpXMedInterface.rc = xLibFilterGet(wap, XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedInterface, 

                                                                (xLibU8_t *) &keyLldpXMedInterfaceValue, &kwaLldpXMedInterface.len);



  if(kwaLldpXMedInterface.rc != XLIBRC_SUCCESS)

  {

    kwaLldpXMedInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT(bufp, kwaLldpXMedInterface);

    return kwaLldpXMedInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwaLldpXMedInterface.len);



  /* retrieve key: LldpXMedLocLocationSubType */

  kwaLldpXMedLocLocationSubType.rc = xLibFilterGet(wap, XOBJ_SwitchingLLDPXMEDLocalData_LldpXMedLocLocationSubType, 

                                                                                (xLibU8_t *) &keyLldpXMedLocLocationSubTypeValue, 

                                                                                &kwaLldpXMedLocLocationSubType.len);



  if(kwaLldpXMedLocLocationSubType.rc != XLIBRC_SUCCESS)

  {

    kwaLldpXMedLocLocationSubType.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT(bufp, kwaLldpXMedLocLocationSubType);

    return kwaLldpXMedLocLocationSubType.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedLocLocationSubTypeValue, kwaLldpXMedLocLocationSubType.len);



  /* set the value in application */

  owa.l7rc = usmDbLldpXMedLocLocationInfoSet (keyLldpXMedInterfaceValue,

                                                                              keyLldpXMedLocLocationSubTypeValue,

                                                                              objlldpXMedLocLocationInfoValue);

  if(owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */

    FPOBJ_TRACE_EXIT(bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_EXIT(bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_MfgName

*

* @purpose Get 'MfgName'

*

* @description [MfgName]: The vendor-specific manufacturer name as advertised

*              by the endpoint. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_MfgName (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objMfgNameValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbLldpXMedLocMfgNameGet (objMfgNameValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objMfgNameValue, strlen (objMfgNameValue));



  /* return the object value: MfgName */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMfgNameValue,

                           strlen (objMfgNameValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_PoEPDPowerReq

*

* @purpose Get 'PoEPDPowerReq'

*

* @description [PoEPDPowerReq]: This object contains the value of the power

*              required by a PD expressed in units of 0.1 watts 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_PoEPDPowerReq (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPoEPDPowerReqValue;

  fpObjWa_t kwaLldpXMedInterface = FPOBJ_INIT_WA(sizeof(xLibU32_t));

  xLibU32_t keyLldpXMedInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LldpXMedInterface */

  kwaLldpXMedInterface.rc = xLibFilterGet(wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum, 

                                                                (xLibU8_t *) &keyLldpXMedInterfaceValue, &kwaLldpXMedInterface.len);



  if(kwaLldpXMedInterface.rc != XLIBRC_SUCCESS)

  {

    kwaLldpXMedInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT(bufp, kwaLldpXMedInterface);

    return kwaLldpXMedInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwaLldpXMedInterface.len);

  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocXPoePDPowerReqGet ( keyLldpXMedInterfaceValue, &objPoEPDPowerReqValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objPoEPDPowerReqValue,

                     sizeof (objPoEPDPowerReqValue));



  /* return the object value: PoEPDPowerReq */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPDPowerReqValue,

                           sizeof (objPoEPDPowerReqValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_SerialNum

*

* @purpose Get 'SerialNum'

*

* @description [SerialNum]: The vendor-specific serial number as advertised

*              by the endpoint 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_SerialNum (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objSerialNumValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbLldpXMedLocSerialNumGet (objSerialNumValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objSerialNumValue, strlen (objSerialNumValue));



  /* return the object value: SerialNum */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSerialNumValue,

                           strlen (objSerialNumValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_ModelName

*

* @purpose Get 'ModelName'

*

* @description [ModelName]: The vendor-specific model name as advertised by

*              the endpoint. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_ModelName (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objModelNameValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbLldpXMedLocModelNameGet (objModelNameValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objModelNameValue, strlen (objModelNameValue));



  /* return the object value: ModelName */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objModelNameValue,

                           strlen (objModelNameValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_SoftwareRev

*

* @purpose Get 'SoftwareRev'

*

* @description [SoftwareRev]: The vendor-specific software revision string

*              as advertised by the endpoint. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_SoftwareRev (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objSoftwareRevValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocSoftwareRevGet (objSoftwareRevValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objSoftwareRevValue, strlen (objSoftwareRevValue));



  /* return the object value: SoftwareRev */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSoftwareRevValue,

                           strlen (objSoftwareRevValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_PoEPSEPowerSource

*

* @purpose Get 'PoEPSEPowerSource'

*

* @description [PoEPSEPowerSource]: Defines the type of PSE Power Source advertised

*              by the local device. A value primary(2) indicates

*              that the device advertises its power source as primary. A

*              value backup(3) indicates that the device advertises its power

*              Source as backup. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_PoEPSEPowerSource (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPoEPSEPowerSourceValue={0};

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbLldpXMedLocXPoePSEPowerSourceGet (&objPoEPSEPowerSourceValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  

  /* return the object value: PoEPSEPowerSource */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objPoEPSEPowerSourceValue,

                           sizeof(objPoEPSEPowerSourceValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_FirmwareRev

*

* @purpose Get 'FirmwareRev'

*

* @description [FirmwareRev]: The vendor-specific firmware revision string

*              as advertised by the endpoint 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_FirmwareRev (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objFirmwareRevValue;

  FPOBJ_TRACE_ENTER (bufp);

  

  /* get the value from application */

  owa.l7rc = usmDbLldpXMedLocFirmwareRevGet(objFirmwareRevValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objFirmwareRevValue, strlen (objFirmwareRevValue));



  /* return the object value: FirmwareRev */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFirmwareRevValue,

                           strlen (objFirmwareRevValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_PoEPDPowerPriority

*

* @purpose Get 'PoEPDPowerPriority'

*

* @description [PoEPDPowerPriority]: Defines the priority advertised as being

*              required by this PD. A value critical(2) indicates that

*              the device advertises its power Priority as critical, as per

*              RFC 3621. A value high(3) indicates that the device advertises

*              its power Priority as high, as p 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_PoEPDPowerPriority (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPoEPDPowerPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

 

  /* retrieve key: LocalPortNum */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocXPoePSEPortPDPriorityGet (keyLocalPortNumValue,

                                        &objPoEPDPowerPriorityValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objPoEPDPowerPriorityValue,

                     sizeof (objPoEPDPowerPriorityValue));



  /* return the object value: PoEPDPowerPriority */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPDPowerPriorityValue,

                           sizeof (objPoEPDPowerPriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_PoEDeviceType

*

* @purpose Get 'PoEDeviceType'

*

* @description [PoEDeviceType]: Defines the type of Power-via-MDI (Power over

*              Ethernet) advertised by the local device. A value pseDevice(2)

*              indicates that the device is advertised as a Power

*              Sourcing Entity (PSE). A value pdDevice(3) indicates that the

*              device is advertised as a Powered 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_PoEDeviceType (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPoEDeviceTypeValue={0};

  fpObjWa_t kwaLldpXMedInterface = FPOBJ_INIT_WA(sizeof(xLibU32_t));

  xLibU32_t keyLldpXMedInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



   /* retrieve key: LldpXMedInterface */

  kwaLldpXMedInterface.rc = xLibFilterGet(wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                                                                (xLibU8_t *) &keyLldpXMedInterfaceValue, &kwaLldpXMedInterface.len);



  if(kwaLldpXMedInterface.rc != XLIBRC_SUCCESS)

  {

    kwaLldpXMedInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT(bufp, kwaLldpXMedInterface);

    return kwaLldpXMedInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwaLldpXMedInterface.len);

  /* get the value from application */

  owa.l7rc = usmDbLldpXMedLocXPoeDeviceTypeGet (keyLldpXMedInterfaceValue, &objPoEDeviceTypeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: PoEDeviceType */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objPoEDeviceTypeValue,

                           sizeof(objPoEDeviceTypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_PoEPSEPortPowerAv

*

* @purpose Get 'PoEPSEPortPowerAv'

*

* @description [PoEPSEPortPowerAv]: This object contains the value of the

*              power available from the PSE via this port expressed in units

*              of 0.1 watts. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_PoEPSEPortPowerAv (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPoEPSEPortPowerAvValue;

  xLibU32_t tempPoEPSEPortPowerAvValue;

  xLibU32_t tempPoEPSEPortPowerAvValueFraction;

  FPOBJ_TRACE_ENTER (bufp);



   memset(&objPoEPSEPortPowerAvValue,0,sizeof(objPoEPSEPortPowerAvValue));

  /* retrieve key: LocalPortNum */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocXPoePSEPortPowerAvGet (keyLocalPortNumValue,

                                           &tempPoEPSEPortPowerAvValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  tempPoEPSEPortPowerAvValueFraction = (tempPoEPSEPortPowerAvValue-(tempPoEPSEPortPowerAvValue/10)*10);

  tempPoEPSEPortPowerAvValue = tempPoEPSEPortPowerAvValue/10;



  osapiSnprintf(objPoEPSEPortPowerAvValue, sizeof(objPoEPSEPortPowerAvValue),

                  "%d.%02d Watts",tempPoEPSEPortPowerAvValue,tempPoEPSEPortPowerAvValueFraction);

 

  /* return the object value: PoEPSEPortPowerAv */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPSEPortPowerAvValue,

                           strlen (objPoEPSEPortPowerAvValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPXMEDLocalData_PoEPSEPortPDPriority

*

* @purpose Get 'PoEPSEPortPDPriority'

*

* @description [PoEPSEPortPDPriority]: Reflects the PD power priority that

*              is being advertised on this PSE port. If both locally configure

*              priority and ldpXMedRemXPoEPDPowerPriority are available

*              on this port, it is a matter of local policy which one

*              takes precedence. This object reflects the 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPXMEDLocalData_PoEPSEPortPDPriority (void *wap,

                                                                   void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objPoEPSEPortPDPriorityValue={0};

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDLocalData_LocalPortNum,

                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbLldpXMedLocXPoePSEPortPDPriorityGet (keyLocalPortNumValue,

                                              &objPoEPSEPortPDPriorityValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* return the object value: PoEPSEPortPDPriority */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objPoEPSEPortPDPriorityValue,

                           sizeof (objPoEPSEPortPDPriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

