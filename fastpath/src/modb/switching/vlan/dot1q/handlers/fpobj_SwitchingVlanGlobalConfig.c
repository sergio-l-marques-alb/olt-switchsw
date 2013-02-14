
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingVlanGlobalConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  19 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingVlanGlobalConfig_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjSet_SwitchingVlanGlobalConfig_Pvid
*
* @purpose Set 'Pvid'
 *@description  [Pvid] The PVID, the VLAN ID assigned to untagged frames or
* Priority-Tagged frames received on this port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanGlobalConfig_Pvid (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPvidValue;
  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Pvid */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPvidValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPvidValue, owa.len);
  owa.l7rc = usmDbQportsPVIDSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                 objPvidValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingVlanGlobalConfig_AcceptableFrameTypes
*
* @purpose Set 'AcceptableFrameTypes'
 *@description  [AcceptableFrameTypes] When this is admitOnlyVlanTagged(2) the
* device will discard untagged frames or Priority-Tagged frames
* received on this port. When admitAll(1), untagged frames or
* Priority-Tagged frames received on this port will be accepted and assigned    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanGlobalConfig_AcceptableFrameTypes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAcceptableFrameTypesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AcceptableFrameTypes */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAcceptableFrameTypesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAcceptableFrameTypesValue, owa.len);
  owa.l7rc = usmDbQportsAcceptFrameTypeSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                            objAcceptableFrameTypesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingVlanGlobalConfig_IngressFiltering
*
* @purpose Set 'IngressFiltering'
 *@description  [IngressFiltering]  When this is true(1) the device will discard
* incoming frames for VLANs which do not include this Port in its
* Member set. When false(2), the port will accept all incoming
* frames. This control does not affect VLAN independent BPDU   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanGlobalConfig_IngressFiltering (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIngressFilteringValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IngressFiltering */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIngressFilteringValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIngressFilteringValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbQportsEnableIngressFilteringGet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                          &objIngressFilteringValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingVlanGlobalConfig_PortPriority
*
* @purpose Set 'PortPriority'
 *@description  [PortPriority] the ingress User Priority   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanGlobalConfig_PortPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortPriority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortPriorityValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDot1dPortDefaultUserPrioritySet(L7_UNIT_CURRENT,L7_ALL_INTERFACES,objPortPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
