/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1qStaticMulticast.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1q-object.xml
*
* @create  9 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot1qStaticMulticast_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1qStaticMulticast_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex] A value used to index per-VLAN tables: values of 0 and 4095 are not permitted; if the value is between 1 and 4094 inclusive, it represents an IEEE 802.1Q VLAN-ID with global scope within a given bridged domain (see VlanId textual convention). If the value is greater than 4095 then it represents a VLAN with scope local to the particular agent, i.e. one without a global VLAN-ID assigned to it. Such VLANs are outside the scope of IEEE 802.1Q but it is convenient to be able to manage them
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticMulticast_VlanIndex (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  xLibU32_t objReceivePortValue;
  xLibU32_t nextObjReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = objReceivePortValue = 0;
    memset(objAddressValue, 0x00, sizeof(objAddressValue));
    memset(nextObjAddressValue, 0x00, sizeof(nextObjAddressValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    memset(objAddressValue, 0x00, sizeof(objAddressValue));
    objReceivePortValue = 0;
    do
    {
      owa.l7rc = usmDbDot1qStaticMulticastEntryNextGet(L7_UNIT_CURRENT,
                                                       &objVlanIndexValue,
                                                       objAddressValue,
                                                       &objReceivePortValue);
     nextObjVlanIndexValue = objVlanIndexValue;
     strcpy(nextObjAddressValue,objAddressValue);
     nextObjReceivePortValue = objReceivePortValue;
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
* @function fpObjGet_Switchingdot1qStaticMulticast_Address
*
* @purpose Get 'Address'
*
* @description [Address] The destination MAC address in a frame to which this entry's filtering information applies. This object must take the value of a Multicast or Broadcast address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticMulticast_Address (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  xLibU32_t objReceivePortValue;
  xLibU32_t nextObjReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                          (xLibU8_t *) objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objReceivePortValue = 0;
    memset(objAddressValue, 0x00, sizeof(objAddressValue));

    nextObjVlanIndexValue = 0;
    memset(nextObjAddressValue, 0x00, sizeof(nextObjAddressValue));
    nextObjReceivePortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objAddressValue, owa.len);
    objReceivePortValue = 0;
    do
    {
      owa.l7rc = usmDbDot1qStaticMulticastEntryNextGet(L7_UNIT_CURRENT,
                                                       &objVlanIndexValue,
                                                       objAddressValue,
                                                       &objReceivePortValue);
     nextObjVlanIndexValue = objVlanIndexValue;
     strcpy(nextObjAddressValue, objAddressValue);
     nextObjReceivePortValue = objReceivePortValue;
    }
    while ((objVlanIndexValue == nextObjVlanIndexValue) && (objAddressValue == nextObjAddressValue)
           && (owa.l7rc == L7_SUCCESS));
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
* @function fpObjGet_Switchingdot1qStaticMulticast_ReceivePort
*
* @purpose Get 'ReceivePort'
*
* @description [ReceivePort] Either the value '0', or the port number of the port from which a frame must be received in order for this entry's filtering information to apply. A value of zero indicates that this entry applies on all ports of the device for which there is no other applicable entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticMulticast_ReceivePort (void *wap, void *bufp)
{

  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue = 0;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  xLibU32_t objReceivePortValue;
  xLibU32_t nextObjReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                          (xLibU8_t *) objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objAddressValue, owa.len);

  /* retrieve key: ReceivePort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                          (xLibU8_t *) & objReceivePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objReceivePortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objReceivePortValue, owa.len);

    owa.l7rc = usmDbDot1qStaticMulticastEntryNextGet(L7_UNIT_CURRENT,
                                    &objVlanIndexValue,
                                    objAddressValue,
                                    &objReceivePortValue);
    nextObjVlanIndexValue = objVlanIndexValue;
    strcpy(nextObjAddressValue, objAddressValue);
    nextObjReceivePortValue = objReceivePortValue;
  }

  if ((objVlanIndexValue != nextObjVlanIndexValue) || (objAddressValue != nextObjAddressValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjReceivePortValue, owa.len);

  /* return the object value: ReceivePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjReceivePortValue,
                           sizeof (objReceivePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qStaticMulticast_ForbiddenEgressPorts
*
* @purpose Get 'ForbiddenEgressPorts'
*
* @description [ForbiddenEgressPorts]: The set of ports to which frames received
*              from a specific port and destined for a specific Multicast
*              or Broadcast MAC address must not be forwarded, regardless
*              of any dynamic information e.g. from GMRP. A port may
*              not be added in this set if it is already a member of the
*              set of ports in dot1qStaticMulticastStaticEgressPorts. The
*              default value of this object is a string of zeros of appropriate
*              length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticMulticast_ForbiddenEgressPorts (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objForbiddenEgressPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qStaticMulticastForbiddenEgressPortsGet (L7_UNIT_CURRENT,
                                                      keyVlanIndexValue,
                                                      keyAddressValue,
                                                      keyReceivePortValue,
                                                      objForbiddenEgressPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ForbiddenEgressPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objForbiddenEgressPortsValue,
                           strlen (objForbiddenEgressPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1qStaticMulticast_ForbiddenEgressPorts
*
* @purpose Set 'ForbiddenEgressPorts'
*
* @description [ForbiddenEgressPorts]: The set of ports to which frames received
*              from a specific port and destined for a specific Multicast
*              or Broadcast MAC address must not be forwarded, regardless
*              of any dynamic information e.g. from GMRP. A port may
*              not be added in this set if it is already a member of the
*              set of ports in dot1qStaticMulticastStaticEgressPorts. The
*              default value of this object is a string of zeros of appropriate
*              length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1qStaticMulticast_ForbiddenEgressPorts (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objForbiddenEgressPortsValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ForbiddenEgressPorts */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objForbiddenEgressPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objForbiddenEgressPortsValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qStaticMulticastForbiddenEgressPortsSet (L7_UNIT_CURRENT,
                                                      keyVlanIndexValue,
                                                      keyAddressValue,
                                                      keyReceivePortValue,
                                                      objForbiddenEgressPortsValue);
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
* @function fpObjGet_Switchingdot1qStaticMulticast_StaticEgressPorts
*
* @purpose Get 'StaticEgressPorts'
*
* @description [StaticEgressPorts]: The set of ports to which frames received
*              from a specific port and destined for a specific Multicast
*              or Broadcast MAC address must be forwarded, regardless
*              of any dynamic information e.g. from GMRP. A port may not be
*              added in this set if it is already a member of the set of
*              ports in dot1qStaticMulticastForbiddenEgressPorts. The default
*              value of this object is a string of ones of appropriate
*              length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticMulticast_StaticEgressPorts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStaticEgressPortsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qStaticMulticastStaticEgressPortsGet (L7_UNIT_CURRENT,
                                                   keyVlanIndexValue,
                                                   keyAddressValue,
                                                   keyReceivePortValue,
                                                   objStaticEgressPortsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StaticEgressPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStaticEgressPortsValue,
                           strlen (objStaticEgressPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1qStaticMulticast_StaticEgressPorts
*
* @purpose Set 'StaticEgressPorts'
*
* @description [StaticEgressPorts]: The set of ports to which frames received
*              from a specific port and destined for a specific Multicast
*              or Broadcast MAC address must be forwarded, regardless
*              of any dynamic information e.g. from GMRP. A port may not be
*              added in this set if it is already a member of the set of
*              ports in dot1qStaticMulticastForbiddenEgressPorts. The default
*              value of this object is a string of ones of appropriate
*              length. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1qStaticMulticast_StaticEgressPorts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStaticEgressPortsValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StaticEgressPorts */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objStaticEgressPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objStaticEgressPortsValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qStaticMulticastStaticEgressPortsSet (L7_UNIT_CURRENT,
                                                   keyVlanIndexValue,
                                                   keyAddressValue,
                                                   keyReceivePortValue,
                                                   objStaticEgressPortsValue);
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
* @function fpObjGet_Switchingdot1qStaticMulticast_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This object indicates the status of this entry. other(1)
*              - this entry is currently in use but the conditions
*              under which it will remain so differ from the following values.
*              invalid(2) - writing this value to the object removes
*              the corresponding entry. permanent(3) - this entry is currently
*              in use and will remain so after the next reset of the
*              bridge. deleteOnReset(4) - this entry is currently in use
*              and will remain so until the next reset of the bridge. deleteOnTimeout(5)
*              - this entry is currently in use and will remain
*              so until it is aged out. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticMulticast_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qStaticMulticastStatusGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                        keyAddressValue, keyReceivePortValue,
                                        &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1qStaticMulticast_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This object indicates the status of this entry. other(1)
*              - this entry is currently in use but the conditions
*              under which it will remain so differ from the following values.
*              invalid(2) - writing this value to the object removes
*              the corresponding entry. permanent(3) - this entry is currently
*              in use and will remain so after the next reset of the
*              bridge. deleteOnReset(4) - this entry is currently in use
*              and will remain so until the next reset of the bridge. deleteOnTimeout(5)
*              - this entry is currently in use and will remain
*              so until it is aged out. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1qStaticMulticast_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticMulticast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qStaticMulticastStatusSet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                        keyAddressValue, keyReceivePortValue,
                                        &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
