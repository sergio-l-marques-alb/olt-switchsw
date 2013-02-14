/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1qStaticUnicast.c
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
#include "_xe_Switchingdot1qStaticUnicast_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1qStaticUnicast_FdbId
*
* @purpose Get 'FdbId'
*
* @description [FdbId] The identity of this Filtering Database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticUnicast_FdbId (void *wap, void *bufp)
{

  xLibU32_t objFdbIdValue;
  xLibU32_t nextObjFdbIdValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  xLibU32_t objReceivePortValue;
  xLibU32_t nextObjReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbId */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                          (xLibU8_t *) & objFdbIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objFdbIdValue = objReceivePortValue = 0;
    memset(objAddressValue, 0x00, sizeof(objAddressValue));
    memset(nextObjAddressValue, 0x00, sizeof(nextObjAddressValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objFdbIdValue, owa.len);
    memset(objAddressValue, 0x00, sizeof(objAddressValue));
    objReceivePortValue = 0;
    do
    {
      owa.l7rc = usmDbDot1qStaticUnicastEntryNextGet(L7_UNIT_CURRENT,
                                                     &objFdbIdValue,
                                                     objAddressValue,
                                                     &objReceivePortValue); 
     nextObjFdbIdValue = objFdbIdValue;
     strcpy(nextObjAddressValue, objAddressValue);
     nextObjReceivePortValue = objReceivePortValue;
    }
    while ((objFdbIdValue == nextObjFdbIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjFdbIdValue, owa.len);

  /* return the object value: FdbId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjFdbIdValue, sizeof (objFdbIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qStaticUnicast_Address
*
* @purpose Get 'Address'
*
* @description [Address] The destination MAC address in a frame to which this entry's filtering information applies. This object must take the value of a Multicast or Broadcast address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticUnicast_Address (void *wap, void *bufp)
{

  xLibU32_t objFdbIdValue;
  xLibU32_t nextObjFdbIdValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  xLibU32_t objReceivePortValue;
  xLibU32_t nextObjReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbId */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                          (xLibU8_t *) & objFdbIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objFdbIdValue, owa.len);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_Address,
                          (xLibU8_t *) objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objAddressValue, 0x00, sizeof(objAddressValue));
    objReceivePortValue = 0;

    nextObjFdbIdValue = 0;
    memset(nextObjAddressValue, 0, sizeof(nextObjAddressValue));
    nextObjReceivePortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objAddressValue, owa.len);
    objReceivePortValue = 0;
    do
    {
      owa.l7rc = usmDbDot1qStaticUnicastEntryNextGet(L7_UNIT_CURRENT,
                                                     &objFdbIdValue,
                                                     objAddressValue,
                                                     &objReceivePortValue);
      nextObjFdbIdValue = objFdbIdValue;
      strcpy(nextObjAddressValue, objAddressValue);
      nextObjReceivePortValue = objReceivePortValue;
    }
    while ((objFdbIdValue == nextObjFdbIdValue) && (objAddressValue == nextObjAddressValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objFdbIdValue != nextObjFdbIdValue) || (owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_Switchingdot1qStaticUnicast_ReceivePort
*
* @purpose Get 'ReceivePort'
*
* @description [ReceivePort] Either the value '0', or the port number of the port from which a frame must be received in order for this entry's filtering information to apply. A value of zero indicates that this entry applies on all ports of the device for which there is no other applicable entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticUnicast_ReceivePort (void *wap, void *bufp)
{

  xLibU32_t objFdbIdValue;
  xLibU32_t nextObjFdbIdValue=0;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  xLibU32_t objReceivePortValue;
  xLibU32_t nextObjReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbId */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                          (xLibU8_t *) & objFdbIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objFdbIdValue, owa.len);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_Address,
                          (xLibU8_t *) objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objAddressValue, owa.len);

  /* retrieve key: ReceivePort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_ReceivePort,
                          (xLibU8_t *) & objReceivePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objReceivePortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objReceivePortValue, owa.len);

    owa.l7rc = usmDbDot1qStaticUnicastEntryNextGet(L7_UNIT_CURRENT,
                                                   &objFdbIdValue,
                                                   objAddressValue,
                                                   &objReceivePortValue);
   nextObjFdbIdValue = objFdbIdValue;
   strcpy(nextObjAddressValue, objAddressValue);
   nextObjReceivePortValue = objReceivePortValue;
  }

  if ((objFdbIdValue != nextObjFdbIdValue) || (objAddressValue != nextObjAddressValue)
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
* @function fpObjGet_Switchingdot1qStaticUnicast_AllowedToGoTo
*
* @purpose Get 'AllowedToGoTo'
*
* @description [AllowedToGoTo]: The set of ports for which a frame with a
*              specific unicast address will be flooded in the event that
*              it has not been learned. It also specifies the set of ports
*              a specific unicast address may be dynamically learnt on. The
*              dot1qTpFdbTable will have an equivalent entry with a dot1qTpFdbPort
*              value of '0' until this address has been learnt,
*              when it will be updated with the port the address has been
*              seen on. This only applies to ports that are members of the
*              VLAN, defined by dot1qVlanCurrentEgressPorts. The default
*              value of this object is a string of ones of propriate length.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticUnicast_AllowedToGoTo (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyFdbIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAllowedToGoToValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                           (xLibU8_t *) & keyFdbIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbIdValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qStaticUnicastAllowedToGoToGet (L7_UNIT_CURRENT, keyFdbIdValue,
                                             keyAddressValue,
                                             keyReceivePortValue,
                                             objAllowedToGoToValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AllowedToGoTo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAllowedToGoToValue,
                           strlen (objAllowedToGoToValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1qStaticUnicast_AllowedToGoTo
*
* @purpose Set 'AllowedToGoTo'
*
* @description [AllowedToGoTo]: The set of ports for which a frame with a
*              specific unicast address will be flooded in the event that
*              it has not been learned. It also specifies the set of ports
*              a specific unicast address may be dynamically learnt on. The
*              dot1qTpFdbTable will have an equivalent entry with a dot1qTpFdbPort
*              value of '0' until this address has been learnt,
*              when it will be updated with the port the address has been
*              seen on. This only applies to ports that are members of the
*              VLAN, defined by dot1qVlanCurrentEgressPorts. The default
*              value of this object is a string of ones of propriate length.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1qStaticUnicast_AllowedToGoTo (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAllowedToGoToValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyFdbIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AllowedToGoTo */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAllowedToGoToValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAllowedToGoToValue, owa.len);

  /* retrieve key: FdbId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                           (xLibU8_t *) & keyFdbIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbIdValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_ReceivePort,
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
    usmDbDot1qStaticUnicastAllowedToGoToSet (L7_UNIT_CURRENT, keyFdbIdValue,
                                             keyAddressValue,
                                             keyReceivePortValue,
                                             objAllowedToGoToValue);
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
* @function fpObjGet_Switchingdot1qStaticUnicast_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This object indicates the status of this entry. other(1)
*              - this entry is currently in use but the conditions
*              under which it will remain so differ from the following values.
*              nvalid(2) - writing this value to the object removes
*              the corresponding entry. permanent(3) - this entry is currently
*              in use and will remain so after the next reset of the
*              bridge. deleteOnReset(4) - this entry is currently in use and
*              will remain so until the next reset of the bridge. deleteOnTimeout(5)
*              - this entry is currently in use and will remain
*              so until it is aged out. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qStaticUnicast_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyFdbIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FdbId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                           (xLibU8_t *) & keyFdbIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbIdValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qStaticUnicastStatusGet (L7_UNIT_CURRENT, keyFdbIdValue,
                                               keyAddressValue,
                                               keyReceivePortValue,
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
* @function fpObjSet_Switchingdot1qStaticUnicast_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This object indicates the status of this entry. other(1)
*              - this entry is currently in use but the conditions
*              under which it will remain so differ from the following values.
*              nvalid(2) - writing this value to the object removes
*              the corresponding entry. permanent(3) - this entry is currently
*              in use and will remain so after the next reset of the
*              bridge. deleteOnReset(4) - this entry is currently in use and
*              will remain so until the next reset of the bridge. deleteOnTimeout(5)
*              - this entry is currently in use and will remain
*              so until it is aged out. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1qStaticUnicast_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyFdbIdValue;
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

  /* retrieve key: FdbId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_FdbId,
                           (xLibU8_t *) & keyFdbIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFdbIdValue, kwa1.len);

  /* retrieve key: Address */
  kwa2.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_Address,
                           (xLibU8_t *) keyAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwa2.len);

  /* retrieve key: ReceivePort */
  kwa3.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qStaticUnicast_ReceivePort,
                           (xLibU8_t *) & keyReceivePortValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyReceivePortValue, kwa3.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1qStaticUnicastStatusSet (L7_UNIT_CURRENT, keyFdbIdValue,
                                               keyAddressValue,
                                               keyReceivePortValue,
                                               objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
