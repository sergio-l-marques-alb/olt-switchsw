/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingVlanCurrentConfig.c
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
#include "_xe_SwitchingVlanCurrentConfig_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_common.h"


/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanTimeMark
*
* @purpose Get 'VlanTimeMark'
*
* @description [VlanTimeMark] A timeFilter for this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanTimeMark (void *wap, void *bufp)
{

  xLibU32_t objVlanTimeMarkValue;
  xLibU32_t nextObjVlanTimeMarkValue;
  xLibU32_t objVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanTimeMark */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                          (xLibU8_t *) & objVlanTimeMarkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanTimeMarkValue = objVlanIndexValue = 0;
    nextObjVlanTimeMarkValue = 1;
    owa.l7rc = L7_SUCCESS; /* TimeMark is not supported and hence setting it to 1 */
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanTimeMarkValue, owa.len);
    objVlanIndexValue = 0;
    owa.l7rc = L7_FAILURE; /* TimeMark is not supported and hence returning error for next key get */
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanTimeMarkValue, owa.len);

  /* return the object value: VlanTimeMark */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanTimeMarkValue,
                           sizeof (objVlanTimeMarkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex] The VLAN-ID or other identifier refering to this VLAN.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanIndex (void *wap, void *bufp)
{

  xLibU32_t objVlanTimeMarkValue;
  xLibU32_t nextObjVlanTimeMarkValue;
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanTimeMark */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                          (xLibU8_t *) & objVlanTimeMarkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanTimeMarkValue, owa.len);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = 0;
    nextObjVlanTimeMarkValue = objVlanTimeMarkValue; /*copied since timemark is not supported */
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);

  owa.l7rc = usmDbDot1qVlanCurrentEntryNextGet (L7_UNIT_CURRENT, &nextObjVlanTimeMarkValue,
                                                 &objVlanIndexValue);

  if ((objVlanTimeMarkValue != nextObjVlanTimeMarkValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjVlanIndexValue = objVlanIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue, sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}




#if 0
/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanTimeMark
*
* @purpose Get 'VlanTimeMark'
*
* @description [VlanTimeMark]: A timeFilter for this entry. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanTimeMark (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanTimeMarkValue;
  xLibU32_t vlanId;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanTimeMark */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                          (xLibU8_t *) & objVlanTimeMarkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1qVlanCurrentEntryGet (L7_UNIT_CURRENT,
                                              objVlanTimeMarkValue,
                                              vlanId);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanTimeMarkValue, owa.len);
    owa.l7rc = usmDbDot1qVlanCurrentEntryNextGet (L7_UNIT_CURRENT, &objVlanTimeMarkValue,
                                                 &vlanId);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objVlanTimeMarkValue, owa.len);

  /* return the object value: VlanTimeMark */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanTimeMarkValue,
                           sizeof (objVlanTimeMarkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex]: The VLAN-ID or other identifier refering to this
*              VLAN. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue;
  xLibU32_t TimeMarkValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1qVlanCurrentEntryGet (L7_UNIT_CURRENT, TimeMarkValue,
                                              objVlanIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    owa.l7rc = usmDbDot1qVlanCurrentEntryNextGet (L7_UNIT_CURRENT, &TimeMarkValue,
                                                  &objVlanIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanIndexValue,
                           sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanFdbId
*
* @purpose Get 'VlanFdbId'
*
* @description [VlanFdbId]: The Filtering Database used by this VLAN. This
*              value is allocated automatically by the device whenever the
*              VLAN is created: either dynamically by GVRP, or by management.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanFdbId (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanFdbIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanTimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                           (xLibU8_t *) & keyVlanTimeMarkValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanTimeMarkValue, kwa1.len);

  /* retrieve key: VlanIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qVlanFdbIdGet (L7_UNIT_CURRENT, keyVlanTimeMarkValue,
                                     keyVlanIndexValue, &objVlanFdbIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanFdbId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanFdbIdValue,
                           sizeof (objVlanFdbIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanCurrentEgressPorts
*
* @purpose Get 'VlanCurrentEgressPorts'
*
* @description [VlanCurrentEgressPorts]: The set of ports which are transmitting
*              traffic for this VLAN as either tagged or untagged frames
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanCurrentEgressPorts (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVlanCurrentEgressPortsValue;
  xLibU32_t size;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objVlanCurrentEgressPortsValue, 0x00, sizeof(objVlanCurrentEgressPortsValue));

  /* retrieve key: VlanTimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                           (xLibU8_t *) & keyVlanTimeMarkValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanTimeMarkValue, kwa1.len);

  /* retrieve key: VlanIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qVlanCurrentEgressPortsGet (L7_UNIT_CURRENT, keyVlanTimeMarkValue,
                                         keyVlanIndexValue,
                                         objVlanCurrentEgressPortsValue,
                                         &size);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanCurrentEgressPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVlanCurrentEgressPortsValue,
                           strlen (objVlanCurrentEgressPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanCurrentUntaggedPorts
*
* @purpose Get 'VlanCurrentUntaggedPorts'
*
* @description [VlanCurrentUntaggedPorts]: The set of ports which are transmitting
*              traffic for this VLAN as untagged frames 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanCurrentUntaggedPorts (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVlanCurrentUntaggedPortsValue;
  xLibU32_t size;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objVlanCurrentUntaggedPortsValue,0x00,sizeof(objVlanCurrentUntaggedPortsValue));

  /* retrieve key: VlanTimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                           (xLibU8_t *) & keyVlanTimeMarkValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanTimeMarkValue, kwa1.len);

  /* retrieve key: VlanIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qVlanCurrentUntaggedPortsGet (L7_UNIT_CURRENT, keyVlanTimeMarkValue,
                                           keyVlanIndexValue,
                                           objVlanCurrentUntaggedPortsValue,
                                           &size);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanCurrentUntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVlanCurrentUntaggedPortsValue,
                           strlen (objVlanCurrentUntaggedPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanStatus
*
* @purpose Get 'VlanStatus'
*
* @description [VlanStatus]: This object indicates the status of this entry.
*              other(1) - this entry is currently in use but the conditions
*              under which it will remain so differ from the following
*              values. permanent(2) - Created by Admin dynamicGvrp(3) -
*              this entr 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanTimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                           (xLibU8_t *) & keyVlanTimeMarkValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanTimeMarkValue, kwa1.len);

  /* retrieve key: VlanIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qVlanStatus (L7_UNIT_CURRENT, keyVlanTimeMarkValue,
                                   keyVlanIndexValue, &objVlanStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanStatusValue,
                           sizeof (objVlanStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanCurrentConfig_VlanCreationTime
*
* @purpose Get 'VlanCreationTime'
*
* @description [VlanCreationTime]: The value of sysUpTime when this VLAN was
*              created 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanCurrentConfig_VlanCreationTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVlanCreationTimeValue;
  usmDbTimeSpec_t timeVal;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanTimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanTimeMark,
                           (xLibU8_t *) & keyVlanTimeMarkValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanTimeMarkValue, kwa1.len);

  /* retrieve key: VlanIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanCurrentConfig_VlanIndex,
                           (xLibU8_t *) & keyVlanIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qVlanCreationTime (L7_UNIT_CURRENT, keyVlanTimeMarkValue,
                                         keyVlanIndexValue,
                                         &timeVal);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* objVlanCreationTimeValue = timeVal; */

  /* return the object value: VlanCreationTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVlanCreationTimeValue,
                           strlen (objVlanCreationTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
