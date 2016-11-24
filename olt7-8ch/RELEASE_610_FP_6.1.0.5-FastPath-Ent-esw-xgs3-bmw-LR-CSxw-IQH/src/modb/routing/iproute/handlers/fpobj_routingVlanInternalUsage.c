
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_routingVlanInternalUsage.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipconfig-object.xml
*
* @create  21 January 2009
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rajakrishna Aketi
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingVlanInternalUsage_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_routingVlanInternalUsage_VlanId
*
* @purpose Get 'VLAN ID'
 *@description  Gets VALN ID associated with routing interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVlanInternalUsage_VlanId(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanId;
  xLibU32_t intIfNum=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key(VLAN ID) */
  owa.rc = xLibFilterGet (wap, XOBJ_routingVlanInternalUsage_VlanId,
                          (xLibU8_t *) &objVlanId, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbInternalVlanFindFirst(&objVlanId,&intIfNum);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanId, owa.len);
    owa.l7rc = usmDbInternalVlanFindNext(objVlanId, &objVlanId, &intIfNum);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objVlanId, owa.len);

  /* return the object value: Internal VLAN Index */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanId,
                           sizeof (objVlanId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingVlanInternalUsage_Interface
*
* @purpose Get 'Interface Number'
 *@description Get the routing Interface associated with VLAN.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVlanInternalUsage_Interface(void *wap, void *bufp)
{

  fpObjWa_t kwaVlanId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterface;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: VLAN ID */
  kwaVlanId.rc = xLibFilterGet (wap, XOBJ_routingVlanInternalUsage_VlanId,
                                   (xLibU8_t *) & keyVlanIdValue, &kwaVlanId.len);
  if (kwaVlanId.rc != XLIBRC_SUCCESS)
  {
    kwaVlanId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanId);
    return kwaVlanId.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIdValue, kwaVlanId.len);
  
  /* get the Internal Interface */
  objInterface = usmDbInternalVlanRoutingInterfaceGet(keyVlanIdValue);
  if (objInterface == 0)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Internal Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objInterface,
                           sizeof(objInterface));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingVlanInternalUsage_baseVlanId
*
* @purpose Get 'Base VLAN ID'
 *@description Gets the base VLAN ID tells about allowed VLAN's
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVlanInternalUsage_baseVlanId(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBaseVlanId;
  owa.rc = XLIBRC_SUCCESS;
   FPOBJ_TRACE_ENTER (bufp);

  /* get the Base VLAN ID */
  objBaseVlanId = usmDbBaseInternalVlanIdGet( );

  /* return the object value: baseVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objBaseVlanId,
                           sizeof (objBaseVlanId));
   
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingVlanInternalUsage_baseVlanId
*
* @purpose Set 'Base VLAN ID'
* @description Sets the base VLAN ID tells about allowed VLAN's
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingVlanInternalUsage_baseVlanId(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBaseVlanId;

  owa.rc = XLIBRC_SUCCESS;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve object: VLAN ID */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objBaseVlanId,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Set the VLAN ID */
  
  owa.l7rc = usmDbBaseInternalVlanIdSet(objBaseVlanId);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingVlanInternalUsage_AllocationPolicy
*
* @purpose Get 'Allocation Policy'
 *@description  Gets the Allocation policy with respect to Base VLAN ID.
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingVlanInternalUsage_AllocationPolicy(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAllocationPolicy;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the Allocation Policy */
  objAllocationPolicy = usmDbInternalVlanPolicyGet( );

  /* return the object value:Allocation Policy */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objAllocationPolicy,
                           sizeof(objAllocationPolicy));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingVlanInternalUsage_AllocationPolicy
*
* @purpose Set 'Allocation Policy'
 *@description  Gets the Allocation policy with respect to Base VLAN ID.
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingVlanInternalUsage_AllocationPolicy(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAllocationPolicy;

  owa.rc = XLIBRC_SUCCESS;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve object: Allocation Policy */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAllocationPolicy,
                           &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Set the Allocation Policy */
  owa.l7rc = usmDbInternalVlanPolicySet(objAllocationPolicy);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
