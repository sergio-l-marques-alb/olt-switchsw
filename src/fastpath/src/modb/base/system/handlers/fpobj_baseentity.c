/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseentity.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  15 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseentity_obj.h"
#include "usmdb_edb_api.h"

/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalIndex
*
* @purpose Get 'entPhysicalIndex'
*
* @description The index for this entry. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalIndexValue;
  xLibU32_t nextObjentPhysicalIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & objentPhysicalIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjentPhysicalIndexValue = 0;
  	objentPhysicalIndexValue = 0;
  }
  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objentPhysicalIndexValue, owa.len);
    owa.l7rc =
      usmDbEdbPhysicalEntryNextGet (&objentPhysicalIndexValue);
    nextObjentPhysicalIndexValue = objentPhysicalIndexValue;
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjentPhysicalIndexValue, owa.len);

  /* return the object value: entPhysicalIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjentPhysicalIndexValue,
                           sizeof (objentPhysicalIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalDescr
*
* @purpose Get 'entPhysicalDescr'
*
* @description A textual description of physical entity. This object should contain 
*              a string which identifies the manufacturer's name for 
*              the physical entity, and should be set to a distinct value for 
*              each version or model of the physical entity. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalDescrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalDescrGet (keyentPhysicalIndexValue,
                              objentPhysicalDescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalDescrValue,
                           strlen (objentPhysicalDescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalVendorType
*
* @purpose Get 'entPhysicalVendorType'
*
* @description An indication of the vendor-specific hardware type of the physical 
*              entity. Note that this is different from the definition 
*              of MIB-II's sysObjectID. An agent should set this object to a 
*              enterprise-specific registration identifier value indicating 
*              the specific equipment type in detail. The associated instance 
*              of entPhysicalClass is used to indicate the general type of 
*              hardware device. If no vendor-specific registration identifier 
*              exists for this physical entity, or the value is unknown 
*              by this agent, then the value { 0 0 } is returned. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalVendorType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalVendorTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalVendorTypeGet (keyentPhysicalIndexValue,
                                   objentPhysicalVendorTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalVendorType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalVendorTypeValue,
                           strlen (objentPhysicalVendorTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalContainedIn
*
* @purpose Get 'entPhysicalContainedIn'
*
* @description The value of entPhysicalIndex for the physical entity which 'contains' 
*              this physical entity. A value of zero indicates this 
*              physical entity is not contained in any other physical entity. 
*              Note that the set of 'containment' relationships define a 
*              strict hierarchy; that is, recursion is not allowed. In the event 
*              a physical entity is contained by more than one physical 
*              entity (e.g., double-wide modules), this object should identify 
*              the containing entity with the lowest value of entPhysicalIndex. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalContainedIn (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalContainedInValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalContainedInGet (keyentPhysicalIndexValue,
                                    &objentPhysicalContainedInValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalContainedIn */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objentPhysicalContainedInValue,
                           sizeof (objentPhysicalContainedInValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalClass
*
* @purpose Get 'entPhysicalClass'
*
* @description An indication of the general hardware type of the physical entity. 
*              An agent should set this object to the standard enumeration 
*              value which most accurately indicates the general class of 
*              the physical entity, or the primary class if there is more 
*              than one. If no appropriate standard registration identifier 
*              exists for this physical entity, then the value 'other(1)' is 
*              returned. If the value is unknown by this agent, then the value 
*              'unknown(2)' is returned. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalClass (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalClassValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalClassGet (keyentPhysicalIndexValue,
                              &objentPhysicalClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objentPhysicalClassValue,
                           sizeof (objentPhysicalClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalParentRelPos
*
* @purpose Get 'entPhysicalParentRelPos'
*
* @description An indication of the relative position of this 'child' component 
*              among all its 'sibling' components. Sibling components are 
*              defined as entPhysicalEntries which share the same instance 
*              values of each of the entPhysicalContainedIn and entPhysicalClass 
*              objects. An NMS can use this object to identify the relative 
*              ordering for all sibling components of a particular parent 
*              (identified by the entPhysicalContainedIn instance in each 
*              sibling entry). This value should match any external labeling 
*              of the physical component if possible. For example, for a container 
*              (e.g., card slot) labeled as 'slot #3', entPhysicalParentRelPos 
*              should have the value '3'. Note that the entPhysicalEntry 
*              for the module plugged in slot 3 should have an entPhysicalParentRelPos 
*              value of '1'. If the physical position of 
*              this component does not match any external numbering or clearly 
*              visible ordering, then user documentation or other external 
*              reference material should be used to determine the parent-relative 
*              position. If this is not possible, then the the agent 
*              should assign a consistent (but possibly arbitrary) ordering 
*              to a given set of 'sibling' components, perhaps based on internal 
*              representation of the components. If the agent cannot 
*              determine the parent-relative position for some reason, or if 
*              the associated value of entPhysicalContainedIn is '0', then 
*              the value '-1' is returned. Otherwise a non-negative integer 
*              is returned, indicating the parent-relative position of this 
*              physical entity. Parent-relative ordering normally starts from 
*              '1' and continues to 'N', where 'N' represents the highest 
*              positioned child entity. However, if the physical entities (e.g., 
*              slots) are labeled from a starting position of zero, then 
*              the first sibling should be associated with a entPhysicalParentRelPos 
*              value of '0'. Note that this ordering may be sparse 
*              or dense, depending on agent implementation. The actual values 
*              returned are not globally meaningful, as each 'parent' component 
*              may use different numbering algorithms. The ordering 
*              is only meaningful among siblings of the same parent component. 
*              The agent should retain parent-relative position values across 
*              reboots, either through algorithmic assignment or use of 
*              non-volatile storage. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalParentRelPos (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalParentRelPosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalParentRelPosGet ( keyentPhysicalIndexValue,
                                     &objentPhysicalParentRelPosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalParentRelPos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objentPhysicalParentRelPosValue,
                           sizeof (objentPhysicalParentRelPosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalName
*
* @purpose Get 'entPhysicalName'
*
* @description The textual name of the physical entity. The value of this object 
*              should be the name of the component as assigned by the local 
*              device and should be suitable for use in commands entered 
*              at the device's `console'. This might be a text name, such as 
*              `console' or a simple component number (e.g., port or module 
*              number), such as `1', depending on the physical component naming 
*              syntax of the device. If there is no local name, or this 
*              object is otherwise not applicable, then this object contains 
*              a zero-length string. Note that the value of entPhysicalName 
*              for two physical entities will be the same in the event that 
*              the console interface does not distinguish between them, e.g., 
*              slot-1 and the card in slot-1. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbEdbPhysicalNameGet (keyentPhysicalIndexValue,
                                      objentPhysicalNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalNameValue,
                           strlen (objentPhysicalNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalHardwareRev
*
* @purpose Get 'entPhysicalHardwareRev'
*
* @description The vendor-specific hardware revision string for the physical 
*              entity. The preferred value is the hardware revision identifier 
*              actually printed on the component itself (if present). Note 
*              that if revision information is stored internally in a non-printable 
*              (e.g., binary) format, then the agent must convert 
*              such information to a printable format, in an implementation-specific 
*              manner. If no specific hardware revision string is associated 
*              with the physical component, or this information is 
*              unknown to the agent, then this object will contain a zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalHardwareRev (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalHardwareRevValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalHardwareRevGet (keyentPhysicalIndexValue,
                                    objentPhysicalHardwareRevValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalHardwareRev */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalHardwareRevValue,
                           strlen (objentPhysicalHardwareRevValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalFirmwareRev
*
* @purpose Get 'entPhysicalFirmwareRev'
*
* @description The vendor-specific firmware revision string for the physical 
*              entity. Note that if revision information is stored internally 
*              in a non-printable (e.g., binary) format, then the agent must 
*              convert such information to a printable format, in an implementation-specific 
*              manner. If no specific firmware programs 
*              are associated with the physical component, or this information 
*              is unknown to the agent, then this object will contain a zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalFirmwareRev (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalFirmwareRevValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalFirmwareRevGet (keyentPhysicalIndexValue,
                                    objentPhysicalFirmwareRevValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalFirmwareRev */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalFirmwareRevValue,
                           strlen (objentPhysicalFirmwareRevValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalSoftwareRev
*
* @purpose Get 'entPhysicalSoftwareRev'
*
* @description The vendor-specific software revision string for the physical 
*              entity. Note that if revision information is stored internally 
*              in a non-printable (e.g., binary) format, then the agent must 
*              convert such information to a printable format, in an implementation-specific 
*              manner. If no specific software programs 
*              are associated with the physical component, or this information 
*              is unknown to the agent, then this object will contain a zero-length 
*              string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalSoftwareRev (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalSoftwareRevValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalSoftwareRevGet (keyentPhysicalIndexValue,
                                    objentPhysicalSoftwareRevValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalSoftwareRev */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalSoftwareRevValue,
                           strlen (objentPhysicalSoftwareRevValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalSerialNum
*
* @purpose Get 'entPhysicalSerialNum'
*
* @description The vendor-specific serial number string for the physical entity. 
*              The preferred value is the serial number string actually 
*              printed on the component itself (if present). On the first instantiation 
*              of an physical entity, the value of entPhysicalSerialNum 
*              associated with that entity is set to the correct vendor-assigned 
*              serial number, if this information is available 
*              to the agent. If a serial number is unknown or non-existent, 
*              the entPhysicalSerialNum will be set to a zero-length string 
*              instead. Note that implementations which can correctly identify 
*              the serial numbers of all installed physical entities do not 
*              need to provide write access to the entPhysicalSerialNum object. 
*              Agents which cannot provide non-volatile storage for the 
*              entPhysicalSerialNum strings are not required to implement 
*              write access for this object. Not every physical component will 
*              have a serial number, or even need one. Physical entities 
*              for which the associated value of the entPhysicalIsFRU object 
*              is equal to 'false(2)' (e.g., the repeater ports within a repeater 
*              module), do not need their own unique serial number. 
*              An agent does not have to provide write access for such entities, 
*              and may return a zero-length string. If write access is 
*              implemented for an instance of entPhysicalSerialNum, and a value 
*              is written into the instance, the agent must retain the supplied 
*              value in the entPhysicalSerialNum instance associated 
*              with the same physical entity for as long as that entity remains 
*              instantiated. This includes instantiations across all re- 
*              initializations/reboots of the network management system, including 
*              those which result in a change of the physical entity's 
*              entPhysicalIndex value. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalSerialNum (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalSerialNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalSerialNumGet (keyentPhysicalIndexValue,
                                  objentPhysicalSerialNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalSerialNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalSerialNumValue,
                           strlen (objentPhysicalSerialNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseentity_entPhysicalSerialNum
*
* @purpose Set 'entPhysicalSerialNum'
*
* @description The vendor-specific serial number string for the physical entity. 
*              The preferred value is the serial number string actually 
*              printed on the component itself (if present). On the first instantiation 
*              of an physical entity, the value of entPhysicalSerialNum 
*              associated with that entity is set to the correct vendor-assigned 
*              serial number, if this information is available 
*              to the agent. If a serial number is unknown or non-existent, 
*              the entPhysicalSerialNum will be set to a zero-length string 
*              instead. Note that implementations which can correctly identify 
*              the serial numbers of all installed physical entities do not 
*              need to provide write access to the entPhysicalSerialNum object. 
*              Agents which cannot provide non-volatile storage for the 
*              entPhysicalSerialNum strings are not required to implement 
*              write access for this object. Not every physical component will 
*              have a serial number, or even need one. Physical entities 
*              for which the associated value of the entPhysicalIsFRU object 
*              is equal to 'false(2)' (e.g., the repeater ports within a repeater 
*              module), do not need their own unique serial number. 
*              An agent does not have to provide write access for such entities, 
*              and may return a zero-length string. If write access is 
*              implemented for an instance of entPhysicalSerialNum, and a value 
*              is written into the instance, the agent must retain the supplied 
*              value in the entPhysicalSerialNum instance associated 
*              with the same physical entity for as long as that entity remains 
*              instantiated. This includes instantiations across all re- 
*              initializations/reboots of the network management system, including 
*              those which result in a change of the physical entity's 
*              entPhysicalIndex value. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseentity_entPhysicalSerialNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalSerialNumValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: entPhysicalSerialNum */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objentPhysicalSerialNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objentPhysicalSerialNumValue, owa.len);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEdbPhysicalSerialNumSet (keyentPhysicalIndexValue,
                              objentPhysicalSerialNumValue);
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
* @function fpObjGet_baseentity_entPhysicalMfgName
*
* @purpose Get 'entPhysicalMfgName'
*
* @description The name of the manufacturer of this physical component. The preferred 
*              value is the manufacturer name string actually printed 
*              on the component itself (if present). Note that comparisons 
*              between instances of the entPhysicalModelName, entPhysicalFirmwareRev, 
*              entPhysicalSoftwareRev, and the entPhysicalSerialNum 
*              objects, are only meaningful amongst entPhysicalEntries with 
*              the same value of entPhysicalMfgName. If the manufacturer 
*              name string associated with the physical component is unknown 
*              to the agent, then this object will contain a zero-length string. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalMfgName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalMfgNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalMfgNameGet (keyentPhysicalIndexValue,
                                objentPhysicalMfgNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalMfgName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalMfgNameValue,
                           strlen (objentPhysicalMfgNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalModelName
*
* @purpose Get 'entPhysicalModelName'
*
* @description The vendor-specific model name identifier string associated with 
*              this physical component. The preferred value is the customer-visible 
*              part number, which may be printed on the component 
*              itself. If the model name string associated with the physical 
*              component is unknown to the agent, then this object will contain 
*              a zero-length string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalModelName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalModelNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalModelNameGet ( keyentPhysicalIndexValue,
                                  objentPhysicalModelNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalModelName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalModelNameValue,
                           strlen (objentPhysicalModelNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentity_entPhysicalAlias
*
* @purpose Get 'entPhysicalAlias'
*
* @description This object is an 'alias' name for the physical entity as specified 
*              by a network manager, and provides a non-volatile 'handle' 
*              for the physical entity. On the first instantiation of an 
*              physical entity, the value of entPhysicalAlias associated with 
*              that entity is set to the zero-length string. However, agent 
*              may set the value to a locally unique default value, instead 
*              of a zero-length string. If write access is implemented for 
*              an instance of entPhysicalAlias, and a value is written into 
*              the instance, the agent must retain the supplied value in the 
*              entPhysicalAlias instance associated with the same physical 
*              entity for as long as that entity remains instantiated. This 
*              includes instantiations across all re- initializations/reboots 
*              of the network management system, including those which result 
*              in a change of the physical entity's entPhysicalIndex value. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalAlias (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalAliasValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalAliasGet ( keyentPhysicalIndexValue,
                              objentPhysicalAliasValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalAlias */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalAliasValue,
                           strlen (objentPhysicalAliasValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseentity_entPhysicalAlias
*
* @purpose Set 'entPhysicalAlias'
*
* @description This object is an 'alias' name for the physical entity as specified 
*              by a network manager, and provides a non-volatile 'handle' 
*              for the physical entity. On the first instantiation of an 
*              physical entity, the value of entPhysicalAlias associated with 
*              that entity is set to the zero-length string. However, agent 
*              may set the value to a locally unique default value, instead 
*              of a zero-length string. If write access is implemented for 
*              an instance of entPhysicalAlias, and a value is written into 
*              the instance, the agent must retain the supplied value in the 
*              entPhysicalAlias instance associated with the same physical 
*              entity for as long as that entity remains instantiated. This 
*              includes instantiations across all re- initializations/reboots 
*              of the network management system, including those which result 
*              in a change of the physical entity's entPhysicalIndex value. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseentity_entPhysicalAlias (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalAliasValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: entPhysicalAlias */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objentPhysicalAliasValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objentPhysicalAliasValue, owa.len);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEdbPhysicalAliasSet (keyentPhysicalIndexValue,
                              objentPhysicalAliasValue);
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
* @function fpObjGet_baseentity_entPhysicalAssetID
*
* @purpose Get 'entPhysicalAssetID'
*
* @description This object is a user-assigned asset tracking identifier for the 
*              physical entity as specified by a network manager, and provides 
*              non-volatile storage of this information. On the first 
*              instantiation of an physical entity, the value of entPhysicalAssetID 
*              associated with that entity is set to the zero-length 
*              string. Not every physical component will have a asset tracking 
*              identifier, or even need one. Physical entities for which 
*              the associated value of the entPhysicalIsFRU object is equal 
*              to 'false(2)' (e.g., the repeater ports within a repeater module), 
*              do not need their own unique asset tracking identifier. 
*              An agent does not have to provide write access for such entities, 
*              and may instead return a zero-length string. If write 
*              access is implemented for an instance of entPhysicalAssetID, 
*              and a value is written into the instance, the agent must retain 
*              the supplied value in the entPhysicalAssetID instance associated 
*              with the same physical entity for as long as that entity 
*              remains instantiated. This includes instantiations across 
*              all re- initializations/reboots of the network management system, 
*              including those which result in a change of the physical 
*              entity's entPhysicalIndex value. If no asset tracking information 
*              is associated with the physical component, then this object 
*              will contain a zero- length string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalAssetID (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalAssetIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalAssetIDGet (keyentPhysicalIndexValue,
                                objentPhysicalAssetIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalAssetID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objentPhysicalAssetIDValue,
                           strlen (objentPhysicalAssetIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseentity_entPhysicalAssetID
*
* @purpose Set 'entPhysicalAssetID'
*
* @description This object is a user-assigned asset tracking identifier for the 
*              physical entity as specified by a network manager, and provides 
*              non-volatile storage of this information. On the first 
*              instantiation of an physical entity, the value of entPhysicalAssetID 
*              associated with that entity is set to the zero-length 
*              string. Not every physical component will have a asset tracking 
*              identifier, or even need one. Physical entities for which 
*              the associated value of the entPhysicalIsFRU object is equal 
*              to 'false(2)' (e.g., the repeater ports within a repeater module), 
*              do not need their own unique asset tracking identifier. 
*              An agent does not have to provide write access for such entities, 
*              and may instead return a zero-length string. If write 
*              access is implemented for an instance of entPhysicalAssetID, 
*              and a value is written into the instance, the agent must retain 
*              the supplied value in the entPhysicalAssetID instance associated 
*              with the same physical entity for as long as that entity 
*              remains instantiated. This includes instantiations across 
*              all re- initializations/reboots of the network management system, 
*              including those which result in a change of the physical 
*              entity's entPhysicalIndex value. If no asset tracking information 
*              is associated with the physical component, then this object 
*              will contain a zero- length string. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseentity_entPhysicalAssetID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objentPhysicalAssetIDValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: entPhysicalAssetID */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objentPhysicalAssetIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objentPhysicalAssetIDValue, owa.len);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbEdbPhysicalAssetIDSet (keyentPhysicalIndexValue,
                              objentPhysicalAssetIDValue);
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
* @function fpObjGet_baseentity_entPhysicalIsFRU
*
* @purpose Get 'entPhysicalIsFRU'
*
* @description This object indicates whether or not this physical entity is considered 
*              a 'field replaceable unit' by the vendor. If this object 
*              contains the value 'true(1)' then this entPhysicalEntry 
*              identifies a field replaceable unit. For all entPhysicalEntries 
*              which represent components that are permanently contained 
*              within a field replaceable unit, the value 'false(2)' should 
*              be returned for this object. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentity_entPhysicalIsFRU (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyentPhysicalIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalIsFRUValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseentity_entPhysicalIndex,
                          (xLibU8_t *) & keyentPhysicalIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyentPhysicalIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbPhysicalIsFRUGet ( keyentPhysicalIndexValue,
                              &objentPhysicalIsFRUValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: entPhysicalIsFRU */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objentPhysicalIsFRUValue,
                           sizeof (objentPhysicalIsFRUValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
