/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingVlanStaticConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to VLan-object.xml
*
* @create  18 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingVlanStaticConfig_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "dot1q_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_RangeVLAN_ID
*
* @purpose Get 'ID'
 *@description  [ID] VLAN ID
* @notes
*
* @return
*******************************************************************************/
int getVlanFromMask(L7_VLAN_MASK_t *vlanMask)
{
  int i = 0;
  for(i = 1; i<= L7_VLAN_MAX_MASK_BIT;i++)
  {
    if(L7_VLAN_ISMASKBITSET(*vlanMask,i))
      return i;
  }
  return -1;
}

int getNextVlan(L7_VLAN_MASK_t *vlanMask,int vid)
{
  int i = 0;
  for(i = vid+1;i <= L7_VLAN_MAX_MASK_BIT;i++ )
  {
    if(L7_VLAN_ISMASKBITSET(*vlanMask,i))
      return i;
  }
  return -1;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_VlanIndex
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
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_VlanIndex (void *wap, void *bufp)
{


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t objIDValue;
  xLibU32_t nextObjIDValue;
  L7_VLAN_MASK_t filterMask;
  L7_VLAN_MASK_t returnMask;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&filterMask,0x0,sizeof(filterMask));
  memset(&returnMask,0x0,sizeof(returnMask));
  /* retrieve key: ID */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex, (xLibU8_t *) &filterMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = L7_SUCCESS; nextObjIDValue = 0;
    objIDValue = 0;
  }
  else
  {
    objIDValue = getVlanFromMask(&filterMask);
    if(objIDValue == -1)
    {
      owa.rc = L7_FAILURE;
      return XLIBRC_FAILURE;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIDValue, owa.len);
  }
  owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, objIDValue, &nextObjIDValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  L7_VLAN_SETMASKBIT(returnMask,nextObjIDValue);
  FPOBJ_TRACE_NEW_KEY (bufp, &returnMask, sizeof(returnMask));
  /* return the object value: ID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & returnMask, sizeof (returnMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_VlanName
*
* @purpose Get 'VlanName'
 *@description  [VlanName] The Vlan Names
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_VlanName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objVlanNameValue;
  L7_VLAN_MASK_t filterMask;
  xLibS32_t keyVlanIndexValue;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (L7_VLAN_MASK_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) &filterMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if (keyVlanIndexValue == -1)
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qVlanStaticNameGet (L7_UNIT_CURRENT, keyVlanIndexValue, objVlanNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objVlanNameValue, strlen (objVlanNameValue));

  /* return the object value: VlanName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVlanNameValue, strlen (objVlanNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_VlanName
*
* @purpose Set 'VlanName'
 *@description  [VlanName] The Vlan Names
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_VlanName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objVlanNameValue;
  xLibS32_t keyVlanIndexValue;
  L7_VLAN_MASK_t filterMask;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&filterMask,0x0,sizeof(filterMask));

  /* retrieve object: VlanName */
  owa.len = sizeof (objVlanNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objVlanNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objVlanNameValue, owa.len);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (L7_VLAN_MASK_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) &filterMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVlanIndexValue, owa.len);

  while(1)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, owa.len);

    /* set the value in application */
    owa.l7rc = usmDbDot1qVlanStaticNameSet (L7_UNIT_CURRENT, keyVlanIndexValue, objVlanNameValue);

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    keyVlanIndexValue = getNextVlan(&filterMask,keyVlanIndexValue);
    if( keyVlanIndexValue == -1)
    {
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_Name
*
* @purpose Get 'Name'
*
* @description [Name]: An administratively assigned string, which may be used
*              to identify the VLAN. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_Name (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNameValue;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                         (xLibU8_t *) &filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1)
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qVlanStaticNameGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                          objNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Name */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objNameValue,
                           strlen (objNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_Name
*
* @purpose Set 'Name'
*
* @description [Name]: An administratively assigned string, which may be used
*              to identify the VLAN. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_Name (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNameValue;
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  L7_VLAN_MASK_t filterMask;

  FPOBJ_TRACE_ENTER (bufp);
  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: Name */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objNameValue, owa.len);


  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if(keyVlanIndexValue == -1)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


   while(1)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
    owa.l7rc = usmDbDot1qVlanStaticNameSet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                          objNameValue);

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    keyVlanIndexValue = getNextVlan(&filterMask,keyVlanIndexValue);
    if( keyVlanIndexValue == -1)
    {
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_EgressPorts
*
* @purpose Get 'EgressPorts'
*
* @description [EgressPorts]: The set of ports which are permanently assigned
*              to the egress list for this VLAN by management. Changes
*              to a bit in this object affect the per-port per-VLAN Registrar
*              control for Registration fixed for the relevant GVRP state
*              machine on each port. A por 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_EgressPorts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objEgressPortsValue;
  xLibU32_t length;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qVlanStaticEgressPortsGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                        objEgressPortsValue, &length);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EgressPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objEgressPortsValue,
                           strlen (objEgressPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_EgressPorts
*
* @purpose Set 'EgressPorts'
*
* @description [EgressPorts]: The set of ports which are permanently assigned
*              to the egress list for this VLAN by management. Changes
*              to a bit in this object affect the per-port per-VLAN Registrar
*              control for Registration fixed for the relevant GVRP state
*              machine on each port. A por 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_EgressPorts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objEgressPortsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EgressPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objEgressPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objEgressPortsValue, owa.len);


  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qVlanStaticEgressPortsSet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                        objEgressPortsValue);
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
* @function fpObjGet_SwitchingVlanStaticConfig_ForbiddenEgressPorts
*
* @purpose Get 'ForbiddenEgressPorts'
*
* @description [ForbiddenEgressPorts]: The set of ports which are prohibited
*              by management from being included in the egress list for
*              this VLAN. Changes to this object that cause a port to be
*              included or excluded affect the per-port per-VLAN Registrar
*              control for Registration Forbidden for the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_ForbiddenEgressPorts (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objForbiddenEgressPortsValue;
  xLibU32_t length;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qVlanForbiddenEgressPortsGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                           objForbiddenEgressPortsValue,
                                           &length);
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
* @function fpObjSet_SwitchingVlanStaticConfig_ForbiddenEgressPorts
*
* @purpose Set 'ForbiddenEgressPorts'
*
* @description [ForbiddenEgressPorts]: The set of ports which are prohibited
*              by management from being included in the egress list for
*              this VLAN. Changes to this object that cause a port to be
*              included or excluded affect the per-port per-VLAN Registrar
*              control for Registration Forbidden for the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_ForbiddenEgressPorts (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objForbiddenEgressPortsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
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
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qVlanForbiddenEgressPortsSet (L7_UNIT_CURRENT, keyVlanIndexValue,
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
* @function fpObjGet_SwitchingVlanStaticConfig_UntaggedPorts
*
* @purpose Get 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_UntaggedPorts (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUntaggedPortsValue;
  xLibU32_t length;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qVlanStaticUntaggedPortsGet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                          objUntaggedPortsValue, &length);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUntaggedPortsValue,
                           strlen (objUntaggedPortsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_UntaggedPorts
*
* @purpose Set 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_UntaggedPorts (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUntaggedPortsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UntaggedPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUntaggedPortsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUntaggedPortsValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qVlanStaticUntaggedPortsSet (L7_UNIT_CURRENT, keyVlanIndexValue,
                                          objUntaggedPortsValue);
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
* @function fpObjGet_SwitchingVlanStaticConfig_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: This object is used for creation or deletion of
*              VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;
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

/*  
 *  Tools limits us to send 1k MAX text buffer. We cannot show complete error message when
 *  there are too many errors to display and the error message length is exceeding 1K. 
 *  Fix is truncate the buffer before it reaches the 1K boundary and append a message 
 *  informing the user that there are too many messages and error message is truncated /
 *  not complete. We are now limiting the error message to 1K including the appended 
 *  message info for user. Below message will be appended in truncated case.
 *
 *  -- Too many errors, all errors cannot be displayed. --
 *
 */


#define LAST_MSG_BUF_LEN strlen(TRUNCATE_ERROR_MSG_LAST)
#define TRUNCATE_ERROR_MSG_LAST "\n -- Too many errors, all errors cannot be displayed. --\n"
#define STRNCAT(a,b,l) (strncat(a,b,(L7_MAX_LEN_1024-strlen(a)-(l))))


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: This object is used for creation or deletion of
*              VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  xLibS32_t keyVlanIndexValue;
  L7_VLAN_MASK_t filterMask,defVlanMask;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibU8_t* tempBuf = NULL;
  L7_uchar8 errorBuf[L7_MAX_LEN_1024];
  xLibS32_t errFlag = L7_FALSE;
  xLibS32_t idx = 0;
  int range = 0;
  L7_char8 existsBuf[L7_MAX_LEN_1024];
  L7_char8 createErr[L7_MAX_LEN_1024];
  L7_char8 deleteErr[L7_MAX_LEN_1024];
  L7_uint32 rc=0;
  L7_uint32 defVlan=0;
  memset(&filterMask,0x0,sizeof(filterMask));
  memset(&defVlanMask,0x0,sizeof(defVlanMask));
  memset(errorBuf,0x0,sizeof( errorBuf));
  memset(existsBuf,0x0,sizeof(existsBuf));
  memset(createErr,0x0,sizeof(createErr));
  memset(deleteErr,0x0,sizeof(deleteErr));

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);


  /* retrieve key: ID */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex, (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1)
  {
    if( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) || (objRowStatusValue == L7_ROW_STATUS_DESTROY ))
    {
      memset(&defVlanMask,0x0,sizeof(defVlanMask));
      defVlan = 0;
      owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, defVlan, &defVlan); 

      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      
      L7_VLAN_SETMASKBIT(defVlanMask,defVlan);  
      xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_VlanIndex,0,
                           (xLibU8_t *) &defVlanMask,
                          sizeof( defVlanMask));
      owa.rc = XLIBRC_VLAN_CREATE_FAILED;
      osapiSleep(1);
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if( keyVlanIndexValue == 1)
  {
    owa.rc = XLIBRC_INVALID_VLANID_RANGE_1;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;    
  } 
  range = 0;
  while(1)
  {
    range++;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);
    owa.l7rc = L7_SUCCESS;
    if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
    {
      rc = usmDbVlanIDGet(L7_UNIT_CURRENT, keyVlanIndexValue );
      if (rc != L7_FAILURE)
      {
        if (rc == L7_SUCCESS )  /* VLAN already exists */
        {
          memset(errorBuf, 0x00, sizeof(errorBuf));
          if(strlen(existsBuf ))
          {
            sprintf(errorBuf, ", %d", keyVlanIndexValue);
          }
          else
          {
            sprintf(errorBuf, " %d", keyVlanIndexValue);
          }  

          OSAPI_STRNCAT(existsBuf, errorBuf);

          if(errFlag != L7_TRUE)
          {
           errFlag = L7_TRUE;

           tempBuf = osapiMalloc(L7_CLI_WEB_COMPONENT_ID,L7_MAX_LEN_1024+1);
   
           if(tempBuf == NULL)
           {
             kwa.rc = XLIBRC_MEMORY_ALLOC_FAILED;
             FPOBJ_TRACE_EXIT (bufp, kwa);
             return kwa.rc;
           }
  
           memset(tempBuf,0,L7_MAX_LEN_1024+1);
           if(xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,tempBuf) != XLIBRC_SUCCESS)
           { 
             osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
             kwa.rc = XLIBRC_CONTEXT_SET_FAILED;
             FPOBJ_TRACE_EXIT (bufp, kwa);
             return kwa.rc;
           }

           if(xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorCheck,0,
                       (xLibU8_t *) &errFlag,
                        sizeof(errFlag)) != XLIBRC_SUCCESS)
           {
             osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
             tempBuf = NULL;
             xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,tempBuf);
             owa.rc = XLIBRC_FAILURE;
             FPOBJ_TRACE_EXIT (bufp, owa);
             return owa.rc;
           }
         }
       }  
       else
       {
         rc = usmDbVlanCreate( L7_UNIT_CURRENT, keyVlanIndexValue);
         if( rc!=L7_SUCCESS )
         { 
           memset(errorBuf,0x00,sizeof(errorBuf));
           if(strlen(createErr))
           {
             sprintf(errorBuf, ", %d", keyVlanIndexValue);
           }
           else
           {
             sprintf(errorBuf, " %d", keyVlanIndexValue);
           }  

           OSAPI_STRNCAT(createErr, errorBuf);

           if(errFlag != L7_TRUE)
           {
             errFlag = L7_TRUE;

             tempBuf = osapiMalloc(L7_CLI_WEB_COMPONENT_ID,L7_MAX_LEN_1024+1);
             if(tempBuf == NULL)
             {
               kwa.rc = XLIBRC_MEMORY_ALLOC_FAILED;
               FPOBJ_TRACE_EXIT (bufp, kwa);
               return kwa.rc;
             } 
             memset(tempBuf,0,L7_MAX_LEN_1024+1);
             if(xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,tempBuf) != XLIBRC_SUCCESS)
             {
               osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
               kwa.rc = XLIBRC_CONTEXT_SET_FAILED;
               FPOBJ_TRACE_EXIT (bufp, kwa);
               return kwa.rc;
             } 
             if(xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorCheck,0,
                         (xLibU8_t *) &errFlag,
                     sizeof(errFlag)) != XLIBRC_SUCCESS)
             {
               osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
               tempBuf = NULL;
               xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,tempBuf);
               owa.rc = XLIBRC_FAILURE;
               FPOBJ_TRACE_EXIT (bufp, owa);
               return owa.rc;
             }
           }
         }
       }
     }
    }
    else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
    {
      /* Delete the existing row */
      rc = usmDbVlanDelete( L7_UNIT_CURRENT , keyVlanIndexValue );
      if(rc!=L7_SUCCESS)
      {
        memset(errorBuf,0x00,sizeof(errorBuf));
        if(strlen(deleteErr))
        {
          sprintf(errorBuf, ", %d", keyVlanIndexValue);
        }
        else
        {
          sprintf(errorBuf, " %d", keyVlanIndexValue);
        }  
        OSAPI_STRNCAT(deleteErr, errorBuf);
        if(errFlag != L7_TRUE)
        {
          errFlag = L7_TRUE;

          tempBuf = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, L7_MAX_LEN_1024 + 1);
          if(tempBuf == NULL)
          {
            kwa.rc = XLIBRC_MEMORY_ALLOC_FAILED;
            return kwa.rc;
          }
          memset(tempBuf,0, L7_MAX_LEN_1024+1 );
          if(xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,tempBuf) != XLIBRC_SUCCESS)
          {
            osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
            kwa.rc = XLIBRC_CONTEXT_SET_FAILED;
            return kwa.rc;
          }
          if(xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorCheck,0,
                             (xLibU8_t *) &errFlag,
                             sizeof(errFlag)) != XLIBRC_SUCCESS)
          {
            osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
            tempBuf = NULL;
            xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,tempBuf);
            owa.rc = XLIBRC_FAILURE;
            FPOBJ_TRACE_EXIT (bufp, owa);
            return owa.rc;
          }
       }
       memset(&defVlanMask,0x0,sizeof(defVlanMask));
       defVlan = 0;
       owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, defVlan, &defVlan); 

      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      
       L7_VLAN_SETMASKBIT(defVlanMask,defVlan);
       xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_VlanIndex,0,
                           (xLibU8_t *) &defVlanMask,
                          sizeof( defVlanMask));
      }
    }
    keyVlanIndexValue = getNextVlan(&filterMask,keyVlanIndexValue);
    if( keyVlanIndexValue == -1)
    {
      break;
    }
  }

  if(range > 1)
  {
    memset(&defVlanMask,0x0,sizeof(defVlanMask));
    defVlan = 0;
    owa.l7rc = usmDbNextVlanGet (L7_UNIT_CURRENT, defVlan, &defVlan); 

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
      
    L7_VLAN_SETMASKBIT(defVlanMask,defVlan);  
    xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_VlanIndex,0,
                           (xLibU8_t *) &defVlanMask,
                          sizeof( defVlanMask));
  }
  if (errFlag == L7_TRUE)
  {
    if (strlen(existsBuf) != 0)
    {
      idx++;
      memset(errorBuf,0x00,sizeof(errorBuf));
     (void)osapiSnprintf(errorBuf, sizeof(errorBuf) - 100, "%d. %s %s %s", idx, "VLAN IDs already exist: ", existsBuf, "\n");

      STRNCAT(tempBuf, errorBuf, LAST_MSG_BUF_LEN);
      STRNCAT(tempBuf, "\n", LAST_MSG_BUF_LEN);
   }
   if (strlen(createErr) != 0)
   {
     idx++;
     memset(errorBuf,0x00,sizeof(errorBuf));
     (void)osapiSnprintf(errorBuf,sizeof(errorBuf) - 100,"%d. %s %s %s", idx, "Failed to create VLANs: ", createErr, "\n");
      STRNCAT(tempBuf, errorBuf, LAST_MSG_BUF_LEN);
      STRNCAT(tempBuf, "\n", LAST_MSG_BUF_LEN);
   }
   if (strlen(deleteErr) != 0)
   {
     idx++;
     memset(errorBuf,0x00,sizeof(errorBuf));
     (void)osapiSnprintf(errorBuf,sizeof(errorBuf) - 100, "%d. %s %s %s", idx, "Failed to delete VLANs: ", deleteErr, "\n");
     STRNCAT(tempBuf, errorBuf, LAST_MSG_BUF_LEN);
     STRNCAT(tempBuf, "\n", LAST_MSG_BUF_LEN);
   }
   /* Check for trucated buffer .. if yes, add the truncated message */
   if ((L7_MAX_LEN_1024-strlen(tempBuf)) == LAST_MSG_BUF_LEN)
   {
      STRNCAT(tempBuf, TRUNCATE_ERROR_MSG_LAST, 0);
   }

   owa.rc = XLIBRC_VLAN_CONFIG_ERROR;
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
  }
  else
  {
    xLibFilterSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorCheck,0,
                           (xLibU8_t *) &errFlag,
                          sizeof(errFlag) );
  }  

  FPOBJ_TRACE_EXIT (bufp, owa);
      osapiSleep(1);
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_VlanType
*
* @purpose Get 'VlanType'
 *@description  [VlanType] Determines the VLAN type
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_VlanType (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));

  xLibS32_t keyVlanIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanTypeValue;

  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                                   (xLibU8_t *) & filterMask, &kwaVlanIndex.len);
  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
  {
    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
    return kwaVlanIndex.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if(keyVlanIndexValue == -1)
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwaVlanIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbVlanTypeGet(L7_UNIT_CURRENT, keyVlanIndexValue, &objVlanTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanTypeValue, sizeof (objVlanTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_VlanType
*
* @purpose Set 'VlanType'
*
* @description [VlanType]: Determines the VLAN type
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_VlanType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  L7_VLAN_MASK_t filterMask;

  FPOBJ_TRACE_ENTER (bufp);
  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if(keyVlanIndexValue == -1)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (keyVlanIndexValue == L7_DOT1Q_DEFAULT_VLAN)
  {
    owa.rc = XLIBRC_STATIC_VLAN_SET_FAILED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  while(1)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);
     
       /* set the value in application */
       owa.l7rc = usmDbVlanMakeStaticSet (L7_UNIT_CURRENT, keyVlanIndexValue);
                                  

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    keyVlanIndexValue = getNextVlan(&filterMask,keyVlanIndexValue);
    if( keyVlanIndexValue == -1)
    {
      break;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_Interface
*
* @purpose Get 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_Interface (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterface;


  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & objInterface, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &objInterface);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterface, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  USM_PHYSICAL_INTF | USM_LAG_INTF,
                                  0, objInterface, &objInterface);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objInterface, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInterface,
                           sizeof (objInterface));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_Status
*
* @purpose Get 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_InterfaceStatus (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objInterfaceStatus;

  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1)
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve key: Interface */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa1.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOperVlanMemberSetGet(L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue, &objInterfaceStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objInterfaceStatus,
                           sizeof (objInterfaceStatus));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_Participation
*
* @purpose Get 'Participation'
*
* @description [Participation]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_Participation (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa =FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objParticipation;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if (keyVlanIndexValue == -1)
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve key: Interface */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa1.len);

  /* get the value from application */
  owa.l7rc = usmDbVlanMemberSetGet(L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue, &objParticipation);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objParticipation,
                           sizeof (objParticipation));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_Participation
*
* @purpose Set 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_Participation (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objParticipationValue;
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t objPortVlanMode;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UntaggedPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objParticipationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objParticipationValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa1.len);

  /* set the value in application */
  if (usmDbDot1qSwPortModeGet( L7_UNIT_CURRENT, keyInterfaceValue, &objPortVlanMode) != L7_SUCCESS)
  {
    objPortVlanMode = DOT1Q_SWPORT_MODE_GENERAL;
  }

  do
  {
    if (objParticipationValue == L7_DOT1Q_FORBIDDEN) 
    {
      owa.l7rc = usmDbVlanMemberSet(L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue, objParticipationValue, DOT1Q_SWPORT_MODE_NONE);
    }
    else
    {
      if ((objPortVlanMode == DOT1Q_SWPORT_MODE_ACCESS) && (objParticipationValue == L7_DOT1Q_FIXED))
      {
        owa.l7rc = usmDbDot1qSwPortAccessVlanSet(L7_UNIT_CURRENT, keyInterfaceValue, keyVlanIndexValue);
      }
      else 
      {
        owa.l7rc = usmDbVlanMemberSet(L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue, objParticipationValue, objPortVlanMode);
      }
    }

    if (owa.l7rc != L7_SUCCESS)
    {
      /* roll back config */
      if (objPortVlanMode == DOT1Q_SWPORT_MODE_ACCESS)
        usmDbDot1qSwPortAccessVlanSet(L7_UNIT_CURRENT, keyInterfaceValue, FD_DOT1Q_DEFAULT_VLAN);
      else
        usmDbVlanMemberSet(L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue, L7_DOT1Q_NORMAL_REGISTRATION, objPortVlanMode);
  }
    keyVlanIndexValue = getNextVlan(&filterMask, keyVlanIndexValue);
  }while(keyVlanIndexValue != -1);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_Participationall
*
* @purpose Get 'Participation'
*
* @description [Participation]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_Participationall (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa =FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objParticipation;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if (keyVlanIndexValue == -1)
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve key: Interface */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa1.len);

  /* get the value from application */
  owa.l7rc = usmDbVlanMemberSetGet(L7_UNIT_CURRENT, keyVlanIndexValue, L7_ALL_INTERFACES, &objParticipation);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objParticipation,
                           sizeof (objParticipation));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_Participationall
*
* @purpose Set 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_Participationall (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objParticipationValue;
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  /*fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));*/
  /*xLibU32_t keyInterfaceValue;*/
  xLibU32_t objPortVlanMode;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UntaggedPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objParticipationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objParticipationValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);
  if (usmDbDot1qSwPortModeGet( L7_UNIT_CURRENT, L7_ALL_INTERFACES, &objPortVlanMode) != L7_SUCCESS)
  {
    objPortVlanMode = DOT1Q_SWPORT_MODE_GENERAL;
  }
  do
  {
    if (objParticipationValue == L7_DOT1Q_FORBIDDEN) 
    {
      owa.l7rc = usmDbVlanMemberSet(L7_UNIT_CURRENT, keyVlanIndexValue, L7_ALL_INTERFACES, objParticipationValue, DOT1Q_SWPORT_MODE_NONE);
    }
    else
    {
      if (objParticipationValue == L7_DOT1Q_FIXED)
      owa.l7rc = usmDbVlanMemberSet(L7_UNIT_CURRENT, keyVlanIndexValue, L7_ALL_INTERFACES, objParticipationValue, objPortVlanMode);
    }
    
    if (owa.l7rc != L7_SUCCESS)
    {
      usmDbVlanMemberSet(L7_UNIT_CURRENT, keyVlanIndexValue, L7_ALL_INTERFACES, L7_DOT1Q_NORMAL_REGISTRATION, objPortVlanMode);
    }
    keyVlanIndexValue = getNextVlan(&filterMask, keyVlanIndexValue);
  }while(keyVlanIndexValue != -1);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_UntaggedPorts
*
* @purpose Get 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_Tagging (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objTagging;
  xLibU32_t vlanType;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  
  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve key: Interface */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa1.len);

  /* get the value from application */
  owa.l7rc = usmDbVlanTypeGet(L7_UNIT_CURRENT, keyVlanIndexValue, &vlanType);
  if ( owa.l7rc == L7_SUCCESS )
  {
      if(vlanType == L7_DOT1Q_DYNAMIC || vlanType == L7_DOT1Q_WS_DYNAMIC)
      {
        owa.l7rc= usmDbDot1qOperVlanTaggedMemberGet(L7_UNIT_CURRENT,keyVlanIndexValue,keyInterfaceValue,&objTagging);
      }
      else
      {
        owa.l7rc = usmDbVlanTaggedSetGet(L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue, &objTagging);
      }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTagging,
                           sizeof (objTagging));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_UntaggedPorts
*
* @purpose Set 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_Tagging (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTagggingValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t objPortVlanMode;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UntaggedPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objTagggingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTagggingValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve key: VlanIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa1.len);


  if ((usmDbDot1qSwPortModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &objPortVlanMode) == L7_SUCCESS) && (objPortVlanMode == DOT1Q_SWPORT_MODE_GENERAL))
  {
    /* set the value in application */
  do
  {
    owa.l7rc =
      usmDbVlanTaggedSet (L7_UNIT_CURRENT, keyVlanIndexValue, keyInterfaceValue,
                                            objTagggingValue);
  keyVlanIndexValue = getNextVlan(&filterMask, keyVlanIndexValue);
  }while(keyVlanIndexValue != -1);
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

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_Taggingall
*
* @purpose Get 'Taggingall'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_Taggingall (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa =  FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objTagging;
  xLibU32_t vlanType;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  
  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbVlanTypeGet(L7_UNIT_CURRENT, keyVlanIndexValue, &vlanType);
  if ( owa.l7rc == L7_SUCCESS )
  {
    owa.l7rc = usmDbVlanTaggedSetGet(L7_UNIT_CURRENT, keyVlanIndexValue, L7_ALL_INTERFACES, &objTagging);
      
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UntaggedPorts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTagging,
                           sizeof (objTagging));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_Taggingall
*
* @purpose Set 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_Taggingall (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTagggingValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));
  xLibS32_t keyVlanIndexValue;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UntaggedPorts */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objTagggingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTagggingValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) & filterMask, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if( keyVlanIndexValue == -1 )
  {
    owa.rc = L7_FAILURE;
    return XLIBRC_FAILURE;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  do
  {
    owa.l7rc =
      usmDbVlanTaggedSet (L7_UNIT_CURRENT, keyVlanIndexValue, L7_ALL_INTERFACES,
                                            objTagggingValue);
  keyVlanIndexValue = getNextVlan(&filterMask, keyVlanIndexValue);
  }while(keyVlanIndexValue != -1);
  
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
* @function fpObjGet_SwitchingVlanStaticConfig_UntaggedPorts
*
* @purpose Get 'UntaggedPorts'
*
* @description [UntaggedPorts]: The set of ports which should transmit egress
*              packets for this VLAN as untagged. The default value of
*              this object for the default VLAN (dot1qVlanIndex = 1) is a
*              string of appropriate length including all ports. There is
*              no specified default for other VL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_ErrorConsole (void *wap,
                                                           void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_uchar8 objVlanErrorBuf[L7_MAX_LEN_1024];
  char* tempOutput = NULL;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objVlanErrorBuf, 0x0, sizeof(objVlanErrorBuf));
 
  owa.len = sizeof (objVlanErrorBuf);

  if(xLibFilterContextGet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,(void *)&tempOutput) == XLIBRC_SUCCESS)
  {
    /* we did not set any context value so this is possible*/
    if(tempOutput != NULL)
    {
      memcpy(objVlanErrorBuf,tempOutput,strlen(tempOutput));
      osapiFree(L7_CLI_WEB_COMPONENT_ID,tempOutput);
      tempOutput = NULL;
      xLibFilterContextSet(wap,XOBJ_SwitchingVlanStaticConfig_ErrorBuf,(void *)tempOutput);
    }
  }
  FPOBJ_TRACE_VALUE (bufp, objVlanErrorBuf, strlen (objVlanErrorBuf));

  /* return the object value: Pingoutput */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objVlanErrorBuf, strlen (objVlanErrorBuf));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVlanStaticConfig_EgressPorts
*
* @purpose Set 'EgressPorts'
*
* @description [EgressPorts]: The set of ports which are permanently assigned
*              to the egress list for this VLAN by management. Changes
*              to a bit in this object affect the per-port per-VLAN Registrar
*              control for Registration fixed for the relevant GVRP state
*              machine on each port. A por 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanStaticConfig_ErrorBuf(void *wap, void *bufp)
{ 
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjGet_SwitchingVlanStaticConfig_ErrorCheck(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  int errorHappened = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

 
  owa.len = sizeof (errorHappened);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_ErrorCheck,
      (xLibU8_t *) &errorHappened, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    errorHappened = L7_FALSE;
  }

  FPOBJ_TRACE_VALUE (bufp, errorHappened, sizeof( errorHappened));

  /* return the object value: Pingoutput */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &errorHappened, sizeof ( errorHappened));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingVlanStaticConfig_ErrorCheck(void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
 
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_ErrorBuf(void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_SwitchingVlanStaticConfig_IsAccessVlanForAnyPort
*
* @purpose Get 'IsAccessVlanForAnyPort'
 *@description  [IsAccessVlanForAnyPort] To check whether a VLAN is Access VLAN
* for any port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVlanStaticConfig_IsAccessVlanForAnyPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIsAccessVlanForAnyPortValue;
  xLibS32_t keyVlanIndexValue;
  L7_VLAN_MASK_t filterMask;

  memset(&filterMask,0x0,sizeof(filterMask));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.len = sizeof (L7_VLAN_MASK_t);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVlanStaticConfig_VlanIndex,
                          (xLibU8_t *) &filterMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  keyVlanIndexValue = getVlanFromMask(&filterMask);
  if (keyVlanIndexValue == -1)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyVlanIndexValue, owa.len);

  /* get the value from application */
  objIsAccessVlanForAnyPortValue = usmDbDot1qIsAccessVlanForAnyPort(L7_UNIT_CURRENT, keyVlanIndexValue);
  
  FPOBJ_TRACE_VALUE (bufp, &objIsAccessVlanForAnyPortValue,
                     sizeof (objIsAccessVlanForAnyPortValue));

  /* return the object value: IsAccessVlanForAnyPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsAccessVlanForAnyPortValue,
                           sizeof (objIsAccessVlanForAnyPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

