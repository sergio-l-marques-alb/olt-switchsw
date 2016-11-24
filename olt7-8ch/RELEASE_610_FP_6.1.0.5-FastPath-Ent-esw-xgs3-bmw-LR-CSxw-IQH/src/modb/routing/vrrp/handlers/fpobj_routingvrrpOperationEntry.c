/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpOperationEntry.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to vrrpOper-object.xml
*
* @create  19 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingvrrpOperationEntry_obj.h"
#include "usmdb_mib_vrrp_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] A unique value, greater than zero, for each interface.
* It is recommended that values are assigned contiguously starting
* from 1. The value for each interface sub-layer must remain
* constant at least from one re-initialization of the entity's network
* management system to the next re-initialization.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_ifIndex (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t objVrIdValue;
  xLibU8_t nextObjVrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objifIndexValue, 0, sizeof (objifIndexValue));
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = 0; 
    owa.l7rc =
      usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                           &nextObjVrIdValue, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
    memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
    nextObjifIndexValue = 0;
    do
    {
     memcpy(&objVrIdValue,&nextObjVrIdValue,sizeof(nextObjVrIdValue));
      owa.l7rc =
        usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                             &nextObjVrIdValue, &nextObjifIndexValue);

    }
    while ((objifIndexValue == nextObjifIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_VrId
*
* @purpose Get 'VrId'
 *@description  [VrId] This object contains the Virtual Router Identifier (VRID).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_VrId (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU8_t objVrIdValue;
  xLibU8_t nextObjVrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t vrId = 0;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, kwa.len);

  /* retrieve key: VrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)& vrId, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objVrIdValue, 0, sizeof (objVrIdValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &vrId, owa.len);
    objVrIdValue = (xLibU8_t)vrId;
  }
  memset (&nextObjVrIdValue, 0, sizeof (nextObjVrIdValue));
  nextObjifIndexValue = 0;
  owa.l7rc =
    usmDbVrrpOperEntryNextGet(L7_UNIT_CURRENT, objVrIdValue, objifIndexValue,
                         &nextObjVrIdValue, &nextObjifIndexValue);
 
  if ((objifIndexValue != nextObjifIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVrIdValue, owa.len);

  vrId = nextObjVrIdValue;
  /* return the object value: VrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &vrId, sizeof (vrId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_VirtualMacAddr
*
* @purpose Get 'VirtualMacAddr'
*
* @description [VirtualMacAddr]: The virtual MAC address of the virtual router.
*              Although this object can be derived from the 'vrrpOperVrId'
*              object, it is defined so that it is easily obtainable
*              by a management application and can be included in VRRP-related
*              SNMP traps 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_VirtualMacAddr (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  xLibStr6_t objVirtualMacAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperVirtualMacAddrGet (L7_UNIT_CURRENT, objVrIdValue,
                                             keyifIndexValue,
                                             objVirtualMacAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VirtualMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVirtualMacAddrValue,
                           sizeof (objVirtualMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_State
*
* @purpose Get 'State'
*
* @description [State]: The current state of the virtual router. This object
*              has three defined values `initialize', which indicates that
*              all the virtual router is waiting for a startup event.
*              `backup', which indicates the virtual router is monitoring
*              the availability of the master router.`master', which indicates
*              that the virtual router is forwarding packets for IP addresses
*              that are associated with this router. Setting the
*              `vrrpOperAdminState' object (below) initiates transitions in
*              the value of this object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_State (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperStateGet (L7_UNIT_CURRENT, objVrIdValue,
                                    keyifIndexValue, &objStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: State */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStateValue,
                           sizeof (objStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_AdminState
*
* @purpose Get 'AdminState'
*
* @description [AdminState]: This object will enable/disable the virtual router
*              function. Setting the value to `up', will transition
*              the state of the virtual router from `initialize' to `backup'
*              or `master', depending on the value of `vrrpOperPriority'.
*              Setting the value to `down', will transition the router
*              from `master' or `backup' to `initialize'. State transitions
*              may not be immediate; they sometimes depend on other factors,
*              such as the interface (IF) state. The `vrrpOperAdminState'
*              object must be set to `down' prior to modifying the other
*              read-create objects in the conceptual row. The value of
*              the `vrrpOperRowStatus' object (below) must be `active', signifying
*              that the conceptual row is valid (i.e., the objects
*              are correctly set), in order for this object to be set to
*              `up'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_AdminState (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperVrrpModeGet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, &objAdminStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdminState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminStateValue,
                           sizeof (objAdminStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_AdminState
*
* @purpose Set 'AdminState'
*
* @description [AdminState]: This object will enable/disable the virtual router
*              function. Setting the value to `up', will transition
*              the state of the virtual router from `initialize' to `backup'
*              or `master', depending on the value of `vrrpOperPriority'.
*              Setting the value to `down', will transition the router
*              from `master' or `backup' to `initialize'. State transitions
*              may not be immediate; they sometimes depend on other factors,
*              such as the interface (IF) state. The `vrrpOperAdminState'
*              object must be set to `down' prior to modifying the other
*              read-create objects in the conceptual row. The value of
*              the `vrrpOperRowStatus' object (below) must be `active', signifying
*              that the conceptual row is valid (i.e., the objects
*              are correctly set), in order for this object to be set to
*              `up'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_AdminState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStateValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminState */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminStateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStateValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);


  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpOperVrrpModeSet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, objAdminStateValue);
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
* @function fpObjGet_routingvrrpOperationEntry_OperPriority
*
* @purpose Get 'Priority'
*
* @description [Priority]: This object specifies the priority to be used for
*              the virtual router master election process. Higher values
*              imply higher priority. A priority of '0', although not settable,
*              is sent by the master router to indicate that this
*              router has ceased to participate in VRRP and a backup virtual
*              router should transition to become a new master. A priority
*              of 255 is used for the router that owns the associated
*              IP address(es). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_OperPriority (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPriorityValue;
  L7_uchar8 oldPriority;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperPriorityGet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, &oldPriority);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objPriorityValue = oldPriority;
  /* return the object value: Priority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPriorityValue,
                           sizeof (objPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_ConfigPriority
*
* @purpose Get 'ConfigPriority'
*
* @description [ConfigPriority]: This object specifies the priority to be used for
*              the virtual router master election process. Higher values
*              imply higher priority. A priority of '0', although not settable,
*              is sent by the master router to indicate that this
*              router has ceased to participate in VRRP and a backup virtual
*              router should transition to become a new master. A priority
*              of 255 is used for the router that owns the associated
*              IP address(es). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_ConfigPriority (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  xLibU32_t objPriorityValue;
  L7_uchar8 oldPriority;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_uchar8));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc  != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpConfigPriorityGet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, &oldPriority);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objPriorityValue = oldPriority;
  /* return the object value: Priority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPriorityValue,
                           sizeof (objPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_ConfigPriority
*
* @purpose Set 'ConfigPriority'
*
* @description [ConfigPriority]: This object specifies the priority to be used for
*              the virtual router master election process. Higher values
*              imply higher priority. A priority of '0', although not settable,
*              is sent by the master router to indicate that this
*              router has ceased to participate in VRRP and a backup virtual
*              router should transition to become a new master. A priority
*              of 255 is used for the router that owns the associated
*              IP address(es). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_ConfigPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigPriorityValue;
  xLibU8_t priority;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Priority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objConfigPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfigPriorityValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if ( (objConfigPriorityValue < L7_VRRP_INTF_PRIORITY_MIN ) || ( objConfigPriorityValue > L7_VRRP_INTF_PRIORITY_MAX ) )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  priority = (xLibU8_t)objConfigPriorityValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpOperPrioritySet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, priority);
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
* @function fpObjGet_routingvrrpOperationEntry_IpAddrCount
*
* @purpose Get 'IpAddrCount'
*
* @description [IpAddrCount]: The number of IP addresses that are associated
*              with this virtual router. This number is equal to the number
*              of rows in the vrrpAssoIpAddrTable that correspond to
*              a given IF index/VRID pair. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_IpAddrCount (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t  objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  /*xLibU32_t objIpAddrCountValue; */
  L7_uchar8 IpAddrCount;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperIpAddrCountGet (L7_UNIT_CURRENT, objVrIdValue,
                                          keyifIndexValue,
                                          &IpAddrCount);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpAddrCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & IpAddrCount,
                           sizeof (IpAddrCount));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_MasterIpAddr
*
* @purpose Get 'MasterIpAddr'
*
* @description [MasterIpAddr]: The master router's real (primary) IP address.
*              This is the IP address listed as the source in VRRP advertisement
*              last received by this virtual router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_MasterIpAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMasterIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)&keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperMasterIpAddressGet (L7_UNIT_CURRENT, objVrIdValue,
                                              keyifIndexValue,
                                              &objMasterIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MasterIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMasterIpAddrValue,
                           sizeof (objMasterIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_PrimaryIpAddr
*
* @purpose Get 'PrimaryIpAddr'
*
* @description [PrimaryIpAddr]: In the case where there is more than one IP
*              address for a given `ifIndex', this object is used to specify
*              the IP address that will become the`vrrpOperMasterIpAddr',
*              should the virtual router transition from backup to master.
*              If this object is set to 0.0.0.0, the IP address which
*              is numerically lowest will be selected. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_PrimaryIpAddr (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPrimaryIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperIpAddressGet (L7_UNIT_CURRENT, objVrIdValue,
                                        keyifIndexValue,
                                        &objPrimaryIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PrimaryIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPrimaryIpAddrValue,
                           sizeof (objPrimaryIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_PrimaryIpAddr
*
* @purpose Set 'PrimaryIpAddr'
*
* @description [PrimaryIpAddr]: In the case where there is more than one IP
*              address for a given `ifIndex', this object is used to specify
*              the IP address that will become the`vrrpOperMasterIpAddr',
*              should the virtual router transition from backup to master.
*              If this object is set to 0.0.0.0, the IP address which
*              is numerically lowest will be selected. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_PrimaryIpAddr (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPrimaryIpAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PrimaryIpAddr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPrimaryIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPrimaryIpAddrValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpAssocIpAddress (L7_UNIT_CURRENT, objVrIdValue,
                                      keyifIndexValue, objPrimaryIpAddrValue);
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
* @function fpObjGet_routingvrrpOperationEntry_AuthType
*
* @purpose Get 'AuthType'
*
* @description [AuthType]: Authentication type used for VRRP protocol exchanges
*              between virtual routers. This value of this object is
*              the same for a given ifIndex. New enumerations to this list
*              can only be added via a new RFC on the standards track. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_AuthType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperAuthTypeGet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, &objAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthTypeValue,
                           sizeof (objAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_AuthType
*
* @purpose Set 'AuthType'
*
* @description [AuthType]: Authentication type used for VRRP protocol exchanges
*              between virtual routers. This value of this object is
*              the same for a given ifIndex. New enumerations to this list
*              can only be added via a new RFC on the standards track. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_AuthType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthTypeValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpOperAuthTypeSet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, objAuthTypeValue);
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
* @function fpObjGet_routingvrrpOperationEntry_AuthKey
*
* @purpose Get 'AuthKey'
*
* @description [AuthKey]: The Authentication Key. This object is set according
*              to the value of the 'vrrpOperAuthType' object ('simpleTextPassword'
*              or 'ipAuthenticationHeader'). If the length of
*              the value is less than 16 octets, the agent will left adjust
*              and zero fill to 16 octets. The value of this object is
*              the same for a given ifIndex. When read, vrrpOperAuthKey always
*              returns an Octet String of length zero. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_AuthKey (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperAuthKeyGet (L7_UNIT_CURRENT, objVrIdValue,
                                      keyifIndexValue, objAuthKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthKeyValue,
                           strlen (objAuthKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_AuthKey
*
* @purpose Set 'AuthKey'
*
* @description [AuthKey]: The Authentication Key. This object is set according
*              to the value of the 'vrrpOperAuthType' object ('simpleTextPassword'
*              or 'ipAuthenticationHeader'). If the length of
*              the value is less than 16 octets, the agent will left adjust
*              and zero fill to 16 octets. The value of this object is
*              the same for a given ifIndex. When read, vrrpOperAuthKey always
*              returns an Octet String of length zero. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_AuthKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAuthKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthKeyValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpOperAuthKeySet (L7_UNIT_CURRENT, objVrIdValue,
                                      keyifIndexValue, objAuthKeyValue);
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
* @function fpObjGet_routingvrrpOperationEntry_AdvertisementInterval
*
* @purpose Get 'AdvertisementInterval'
*
* @description [AdvertisementInterval]: The time interval, in seconds, between
*              sending advertisement messages. Only the master router
*              sends VRRP advertisements. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_AdvertisementInterval (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdvertisementIntervalValue;
  L7_uchar8 AdvertInterval;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc =
    usmDbVrrpOperAdvertisementIntervalGet (L7_UNIT_CURRENT, objVrIdValue,
                                           keyifIndexValue,
                                           &AdvertInterval);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objAdvertisementIntervalValue = AdvertInterval;
  /* return the object value: AdvertisementInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdvertisementIntervalValue,
                           sizeof (objAdvertisementIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_AdvertisementInterval
*
* @purpose Set 'AdvertisementInterval'
*
* @description [AdvertisementInterval]: The time interval, in seconds, between
*              sending advertisement messages. Only the master router
*              sends VRRP advertisements. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_AdvertisementInterval (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdvertisementIntervalValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdvertisementInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAdvertisementIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdvertisementIntervalValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if ( (objAdvertisementIntervalValue < L7_VRRP_INTF_PRIORITY_MIN ) || (objAdvertisementIntervalValue > L7_VRRP_INTF_PRIORITY_MAX ) )
  {
    /* SET_ERR_MSG
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext,pStrErr_routing_CfgVrrpAdvintError);*/
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc =
    usmDbVrrpOperAdvertisementIntervalSet (L7_UNIT_CURRENT, objVrIdValue,
                                           keyifIndexValue,
                                           objAdvertisementIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_PreemptMode
*
* @purpose Get 'PreemptMode'
*
* @description [PreemptMode]: Controls whether a higher priority virtual router
*              will preempt a lower priority master. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_PreemptMode (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPreemptModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperPreemptModeGet (L7_UNIT_CURRENT, objVrIdValue,
                                          keyifIndexValue,
                                          &objPreemptModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PreemptMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPreemptModeValue,
                           sizeof (objPreemptModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_PreemptMode
*
* @purpose Set 'PreemptMode'
*
* @description [PreemptMode]: Controls whether a higher priority virtual router
*              will preempt a lower priority master. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_PreemptMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPreemptModeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PreemptMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPreemptModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPreemptModeValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpOperPreemptModeSet (L7_UNIT_CURRENT, objVrIdValue,
                                          keyifIndexValue, objPreemptModeValue);
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
* @function fpObjGet_routingvrrpOperationEntry_VirtualRouterUpTime
*
* @purpose Get 'VirtualRouterUpTime'
*
* @description [VirtualRouterUpTime]: This is the value of the `sysUpTime'
*              object when this virtual router (i.e., the `vrrpOperState')
*              transitioned out of `initialized'. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_VirtualRouterUpTime (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVirtualRouterUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperUpTimeGet (L7_UNIT_CURRENT, objVrIdValue,
                                     keyifIndexValue,
                                     &objVirtualRouterUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VirtualRouterUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVirtualRouterUpTimeValue,
                           sizeof (objVirtualRouterUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_Protocol
*
* @purpose Get 'Protocol'
*
* @description [Protocol]: The particular protocol being controlled by this
*              Virtual Router. New enumerations to this list can only be
*              added via a new RFC on the standards track. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_Protocol (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperProtocolGet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, &objProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Protocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objProtocolValue,
                           sizeof (objProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_Protocol
*
* @purpose Set 'Protocol'
*
* @description [Protocol]: The particular protocol being controlled by this
*              Virtual Router. New enumerations to this list can only be
*              added via a new RFC on the standards track. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_Protocol (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Protocol */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objProtocolValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpOperProtocolSet (L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue, objProtocolValue);
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
* @function fpObjGet_routingvrrpOperationEntry_ZeroPriorityFlg
*
* @purpose Get 'ZeroPriorityFlg'
*
* @description [ZeroPriorityFlg]: To set/get the 0 priority advert rcvd flag
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_ZeroPriorityFlg (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objZeroPriorityFlgValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpZeroPriorityFlgGet (L7_UNIT_CURRENT, objVrIdValue,
                                          keyifIndexValue,
                                          &objZeroPriorityFlgValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ZeroPriorityFlg */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objZeroPriorityFlgValue,
                           sizeof (objZeroPriorityFlgValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_ZeroPriorityFlg
*
* @purpose Set 'ZeroPriorityFlg'
*
* @description [ZeroPriorityFlg]: To set/get the 0 priority advert rcvd flag
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_ZeroPriorityFlg (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objZeroPriorityFlgValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ZeroPriorityFlg */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objZeroPriorityFlgValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objZeroPriorityFlgValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* set the value in application */
  owa.l7rc = usmDbVrrpZeroPriorityFlgSet (L7_UNIT_CURRENT, objVrIdValue,
                                          keyifIndexValue,
                                          objZeroPriorityFlgValue);
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
* @function fpObjGet_routingvrrpOperationEntry_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: The row status variable, used in accordance to
*              installation and removal conventions for conceptual rows.
*              The rowstatus of a currently active row in the vrrpOperTable
*              is constrained by the operational state of the corresponding
*              virtual router. When `vrrpOperRowStatus' is set to active(1),
*              no other objects in the conceptual row, with the exception
*              of 'vrrpOperAdminState', can be modified. Prior to setting
*              the `vrrpOperRowStatus' object from `active' to a different
*              value, the `vrrpOperAdminState' object must be set
*              to `down' and the `vrrpOperState' object be transitioned to
*              `initialize'. To create a row in this table, a manager sets
*              this object to either createAndGo(4) or createAndWait(5).
*              Until instances of all corresponding columns are appropriately
*              configured, the value of the corresponding instance of
*              the `vrrpOperRowStatus' column will be read as notReady(3).
*              In particular, a newly created row cannot be made active(1)
*              until (minimally) the corresponding instance of 'vrrpOperVrId'
*              has been set and there is at least one active row in
*              the `vrrpAssoIpAddrTable' defining an associated IP address
*              for the virtual router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *) &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
  owa.l7rc = usmDbVrrpOperRowStatusGet (L7_UNIT_CURRENT, objVrIdValue,
                                        keyifIndexValue, &objRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingvrrpOperationEntry_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: The row status variable, used in accordance to
*              installation and removal conventions for conceptual rows.
*              The rowstatus of a currently active row in the vrrpOperTable
*              is constrained by the operational state of the corresponding
*              virtual router. When `vrrpOperRowStatus' is set to active(1),
*              no other objects in the conceptual row, with the exception
*              of 'vrrpOperAdminState', can be modified. Prior to setting
*              the `vrrpOperRowStatus' object from `active' to a different
*              value, the `vrrpOperAdminState' object must be set
*              to `down' and the `vrrpOperState' object be transitioned to
*              `initialize'. To create a row in this table, a manager sets
*              this object to either createAndGo(4) or createAndWait(5).
*              Until instances of all corresponding columns are appropriately
*              configured, the value of the corresponding instance of
*              the `vrrpOperRowStatus' column will be read as notReady(3).
*              In particular, a newly created row cannot be made active(1)
*              until (minimally) the corresponding instance of 'vrrpOperVrId'
*              has been set and there is at least one active row in
*              the `vrrpAssoIpAddrTable' defining an associated IP address
*              for the virtual router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingvrrpOperationEntry_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  if( usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyifIndexValue) == L7_FAILURE )
  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* call the usmdb only for add and delete */
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc =
      usmDbVrrpRouterIdCreate(L7_UNIT_CURRENT, objVrIdValue, keyifIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc =
      usmDbVrrpRouterIdDelete (L7_UNIT_CURRENT, objVrIdValue, keyifIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


xLibRC_t fpObjList_routingvrrpOperationEntry_RoutingPortList (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t	rtr_intf_mode;
	xLibBool_t entryFound = XLIB_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_RoutingPortList,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objifIndexValue = 0;
    nextObjifIndexValue = 0; 
    owa.l7rc =
      usmDbValidIntIfNumFirstGet( &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
	nextObjifIndexValue = 0;
    owa.l7rc =
        usmDbValidIntIfNumNext(objifIndexValue, &nextObjifIndexValue);
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  while (owa.l7rc == L7_SUCCESS)
  {
    /* Is it a Routing interface ? */
    if (usmDbIpRtrIntfModeGet(L7_UNIT_CURRENT, nextObjifIndexValue, &rtr_intf_mode)
                             != L7_SUCCESS)
    {
      rtr_intf_mode = L7_DISABLE;
    }

    if (rtr_intf_mode == L7_ENABLE) 
    {
      entryFound= XLIB_TRUE;
      break;
    }
    owa.l7rc = usmDbValidIntIfNumNext(nextObjifIndexValue, &nextObjifIndexValue);
 }


  if (entryFound != XLIB_TRUE)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_routingvrrpOperationEntry_RoutingPortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
	  xLibU32_t	rtr_intf_mode;
	xLibBool_t entryFound = XLIB_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
    owa.l7rc =
      usmDbValidIntIfNumFirstGet( &objIfIndexValue);
	
   if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   while (owa.l7rc == L7_SUCCESS)
  {
    /* Is it a Routing interface ? */
    if (usmDbIpRtrIntfModeGet(L7_UNIT_CURRENT, objIfIndexValue, &rtr_intf_mode)
                             != L7_SUCCESS)
    {
      rtr_intf_mode = L7_DISABLE;
    }

    if (rtr_intf_mode == L7_ENABLE) 
    {
      entryFound= XLIB_TRUE;
      break;
    }
    owa.l7rc = usmDbValidIntIfNumNext(objIfIndexValue, &objIfIndexValue);
 }

 if(entryFound != XLIB_TRUE)
{
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfIndexValue,
                           sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjSet_routingvrrpOperationEntry_RoutingPortList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifIndexValue, owa.len);

  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_RouterIntfAddress
*
* @purpose Get 'RouterIntfAddress'
*
* @description [RouterIntfAddress]: The Ip address assigned to the routing interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_RouterIntfAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRouterIntfAddressValue;
  L7_IP_MASK_t  mask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT,
                                          keyifIndexValue,
                                          (L7_IP_ADDR_t *)&objRouterIntfAddressValue,
                                          &mask);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RouterIntfAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRouterIntfAddressValue,
                           sizeof (objRouterIntfAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingvrrpOperationEntry_isOwner
*
* @purpose Get 'isOwner'
*
* @description [isOwner]: The virtual router is owner if interface address and 
*                                the virtual router address are same
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpOperationEntry_isOwner (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVrIdValue;
  xLibU8_t objVrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  xLibU32_t isOwner;
  xLibU32_t intf_ip_address,virtual_ip_address,mask;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_VrId,
                           (xLibU8_t *)  &keyVrIdValue, &kwa1.len);
  if (kwa1.rc  != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVrIdValue, kwa1.len);

  /* retrieve key: ifIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingvrrpOperationEntry_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa2.len);

  objVrIdValue = (xLibU8_t)keyVrIdValue;
  /* get the value from application */
   owa.l7rc = usmDbVrrpOperIpAddressGet(L7_UNIT_CURRENT, objVrIdValue,
                                       keyifIndexValue,&virtual_ip_address);
   if (owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}

    owa.l7rc = usmDbIpRtrIntfIpAddressGet(L7_UNIT_CURRENT,keyifIndexValue, &intf_ip_address,&mask);
	if (owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}

    if( virtual_ip_address == intf_ip_address)
    {
      isOwner =L7_TRUE ;
    }
    else
    {
      isOwner =L7_FALSE;
    }

  /* return the object value: Priority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &isOwner,
                           sizeof (isOwner));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

