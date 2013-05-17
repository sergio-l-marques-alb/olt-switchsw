
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingIpVlanConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  21 July 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingIpVlanConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_sim_api.h"


#include "_xe_routingswitchIPInterfaceConfig_obj.h"
#include "_xe_baseinterfaceListing_obj.h"
#include "_xe_routingLocalArpConfig_obj.h"
#include "_xe_baseInterfaceInfo_obj.h"
#include "_xe_baseport_obj.h"

#ifdef L7_IPV6_PACKAGE
#include "_xe_IPv6Ipv6InterfaceConfigTable_obj.h"
#endif


/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_vlanId
*
* @purpose Get 'vlanId'
 *@description  [vlanId] The Vlan's Id.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_vlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanIdValue;
  xLibU32_t nextObjvlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (objvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & objvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjvlanIdValue = 0;
    owa.l7rc = usmDbIpVlanRtrVlanIdGetNext(L7_UNIT_CURRENT, &nextObjvlanIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objvlanIdValue, owa.len);
    nextObjvlanIdValue = objvlanIdValue;
    owa.l7rc = usmDbIpVlanRtrVlanIdGetNext(L7_UNIT_CURRENT, &nextObjvlanIdValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjvlanIdValue, owa.len);

  /* return the object value: vlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjvlanIdValue, sizeof (nextObjvlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_vlanIfIndex
*
* @purpose Get 'vlanIfIndex'
 *@description  [vlanIfIndex] The IfIndex associated with this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_vlanIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanIfIndexValue;

  xLibU32_t keyvlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &objvlanIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvlanIfIndexValue, sizeof (objvlanIfIndexValue));

  /* return the object value: vlanIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvlanIfIndexValue, sizeof (objvlanIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_ipAddress
*
* @purpose Get 'ipAddress'
 *@description  [ipAddress] The IpAddress assigned to this interface. When
* setting this value, the value of agentSwitchIpInterfaceNetMask must be
* set at the same time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_ipAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objipAddressValue;
  xLibIpV4_t objMaskValue;

  xLibU32_t keyvlanIdValue,objvlanIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application 
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyvlanIdValue, &objipAddressValue);
*/
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &objvlanIfIndexValue);

  owa.l7rc = usmDbIpRtrIntfIpAddressGet(L7_UNIT_CURRENT, objvlanIfIndexValue, &objipAddressValue, &objMaskValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    memset(&objipAddressValue,0, sizeof(objipAddressValue));   
  }

  FPOBJ_TRACE_VALUE (bufp, &objipAddressValue, sizeof (objipAddressValue));

  /* return the object value: ipAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipAddressValue, sizeof (objipAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_netMask
*
* @purpose Get 'netMask'
 *@description  [netMask] The NetMask assigned to this interface. When setting
* this value, the value of agentSwitchIpInterfaceIpAddress must be
* set at the same time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_netMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objnetMaskValue;
  xLibIpV4_t objipAddressValue;
  xLibU32_t keyvlanIdValue,objvlanIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application 
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyvlanIdValue, &objnetMaskValue);
*/
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &objvlanIfIndexValue); 
  owa.l7rc = usmDbIpRtrIntfIpAddressGet(L7_UNIT_CURRENT, objvlanIfIndexValue, &objipAddressValue, &objnetMaskValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    memset(&objnetMaskValue,0, sizeof(objnetMaskValue));   
  }
 

  FPOBJ_TRACE_VALUE (bufp, &objnetMaskValue, sizeof (objnetMaskValue));

  /* return the object value: netMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objnetMaskValue, sizeof (objnetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_ifPhysAddress
*
* @purpose Get 'ifPhysAddress'
 *@description  [ifPhysAddress]  The interface's address at its protocol
* sub-layer. For example, for an 802.x interface, this object normally
* contains a MAC address. The interface's media-specific MIB must
* define the bit and byte ordering and the fo   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_ifPhysAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr6_t objifPhysAddressValue;

  xLibU32_t keyvlanIdValue,objvlanIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &objvlanIfIndexValue);

  owa.l7rc = usmDbIfIntfL3MacAddressGet(L7_UNIT_CURRENT, objvlanIfIndexValue, objifPhysAddressValue);

  if (owa.l7rc != L7_SUCCESS)
  {
   memset(objifPhysAddressValue, 0, sizeof(objifPhysAddressValue));   
  }
 

  FPOBJ_TRACE_VALUE (bufp, objifPhysAddressValue, sizeof (objifPhysAddressValue));

  /* return the object value: ifPhysAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objifPhysAddressValue,
                           sizeof (objifPhysAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_USPFormat
*
* @purpose Get 'USPFormat'
 *@description  [USPFormat] This object returns the vlan number in
* Unit/Slot/Port Format.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_USPFormat (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUSPFormatValue;
  xLibU32_t keyvlanIdValue, u, s, p,objvlanIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &objvlanIfIndexValue);
  owa.l7rc = usmDbUnitSlotPortGet(objvlanIfIndexValue, &u, &s, &p);
  if (owa.l7rc == L7_SUCCESS)
  {
      if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID))
      {
        osapiSnprintf(objUSPFormatValue, sizeof(objUSPFormatValue),  "%d/%d/%d", u, s, p);
      }
      else
      {
        osapiSnprintf(objUSPFormatValue, sizeof(objUSPFormatValue),  "%d/%d", s, p);
      }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objUSPFormatValue, strlen (objUSPFormatValue));

  /* return the object value: USPFormat */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUSPFormatValue, strlen (objUSPFormatValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_vlanRoutingStatus
*
* @purpose Get 'vlanRoutingStatus'
 *@description  [vlanRoutingStatus] Status of Routing mode per Vlan. active(1) -
* Vlan is enabled for routing; createAndGo(4) - Adds Vlan entry
* for routing; destroy(6) - Removes Vlan from routing   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_vlanRoutingStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanRoutingStatusValue;

  xLibU32_t keyvlanIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application 
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyvlanIdValue, &objvlanRoutingStatusValue);
*/
  owa.l7rc = L7_SUCCESS;
  objvlanRoutingStatusValue = L7_ROW_STATUS_ACTIVE; 

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvlanRoutingStatusValue, sizeof (objvlanRoutingStatusValue));

  /* return the object value: vlanRoutingStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvlanRoutingStatusValue,
                           sizeof (objvlanRoutingStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingIpVlanConfig_vlanRoutingStatus
*
* @purpose Set 'vlanRoutingStatus'
 *@description  [vlanRoutingStatus] Status of Routing mode per Vlan. active(1) -
* Vlan is enabled for routing; createAndGo(4) - Adds Vlan entry
* for routing; destroy(6) - Removes Vlan from routing   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpVlanConfig_vlanRoutingStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanRoutingStatusValue;

  xLibU32_t keyvlanIdValue;
  xLibU32_t mgmtVlanId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: vlanRoutingStatus */
  owa.len = sizeof (objvlanRoutingStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvlanRoutingStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvlanRoutingStatusValue, owa.len);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objvlanRoutingStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
     if (usmDbMgmtVlanIdGet(L7_UNIT_CURRENT, &mgmtVlanId) == L7_SUCCESS)
      {
        if (mgmtVlanId == keyvlanIdValue)
        {
          owa.rc = XLIBRC_VLAN_ROUTING_MODE_CANT_ENABLEON_MGMTVLAN;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
        else
        {
           owa.l7rc = usmDbIpVlanRoutingIntfCreate(L7_UNIT_CURRENT, keyvlanIdValue, 0);
           if (owa.l7rc != L7_SUCCESS)
           {
              owa.rc = XLIBRC_MAX_VLAN_ROUTING_INTF_REACHD;
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;

           }
        }
      }
  }
  else if (objvlanRoutingStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbIpVlanRoutingIntfDelete(L7_UNIT_CURRENT, keyvlanIdValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_vlanRoutingCreate
*
* @purpose Get 'vlanRoutingCreate'
 *@description  [vlanRoutingCreate] Status of Routing mode per Vlan. 
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_vlanRoutingCreate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanRoutingCreateValue = L7_DISABLE;

  xLibU32_t keyvlanIdValue;
  xLibU32_t intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &intIfNum);
    if (owa.l7rc == L7_SUCCESS)
  {
    objvlanRoutingCreateValue = L7_ENABLE;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvlanRoutingCreateValue, sizeof (objvlanRoutingCreateValue));

  /* return the object value: vlanRoutingStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvlanRoutingCreateValue,
                           sizeof (objvlanRoutingCreateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_routingIpVlanConfig_vlanRoutingCreate
*
* @purpose Set 'vlanRoutingCreate'
 *@description  [vlanRoutingCreate] Status of Routing mode per Vlan.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpVlanConfig_vlanRoutingCreate (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanRoutingCreateValue;

  xLibU32_t keyvlanIdValue;
  xLibU32_t mgmtVlanId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: vlanRoutingStatus */
  owa.len = sizeof (objvlanRoutingCreateValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvlanRoutingCreateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvlanRoutingCreateValue, owa.len);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
     FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objvlanRoutingCreateValue == L7_ENABLE)
  {
     if (usmDbMgmtVlanIdGet(L7_UNIT_CURRENT, &mgmtVlanId) == L7_SUCCESS)
      {
        if (mgmtVlanId == keyvlanIdValue)
        {
          owa.rc = XLIBRC_VLAN_ROUTING_MODE_CANT_ENABLEON_MGMTVLAN;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
        else
        {
           owa.l7rc = usmDbIpVlanRoutingIntfCreate(L7_UNIT_CURRENT, keyvlanIdValue, 0);
           if (owa.l7rc != L7_SUCCESS)
           {
              owa.rc = XLIBRC_MAX_VLAN_ROUTING_INTF_REACHD;
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;

           }
        }
      }
  }
  else if (objvlanRoutingCreateValue == L7_DISABLE)
  {
    owa.l7rc = usmDbIpVlanRoutingIntfDelete(L7_UNIT_CURRENT, keyvlanIdValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpVlanConfig_vlanRoutingCreateAndFillIfIndex
*
* @purpose Get 'vlanRoutingCreateAndFillIfIndex'
 *@description  [vlanRoutingCreateAndFillIfIndex] Status of Routing mode per Vlan.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpVlanConfig_vlanRoutingCreateAndFillIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanRoutingCreateValue = L7_ROW_STATUS_CREATE_AND_GO;

  xLibU32_t keyvlanIdValue;
  xLibU32_t intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
                          (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &intIfNum);
    if (owa.l7rc == L7_SUCCESS)
  {
    objvlanRoutingCreateValue = L7_ROW_STATUS_DESTROY;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objvlanRoutingCreateValue, sizeof (objvlanRoutingCreateValue));

  /* return the object value: vlanRoutingStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvlanRoutingCreateValue,
                           sizeof (objvlanRoutingCreateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingIpVlanConfig_vlanRoutingCreateAndFillIfIndex
*
* @purpose Set 'vlanRoutingCreateAndFillIfIndex'
 *@description  [vlanRoutingCreateAndFillIfIndex] Status of Routing mode per Vlan.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpVlanConfig_vlanRoutingCreateAndFillIfIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanRoutingCreateValue;

  xLibU32_t keyvlanIdValue;
  xLibU32_t mgmtVlanId;
  xLibU32_t intfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: vlanRoutingCreateAndFillIfIndex */
  owa.len = sizeof (objvlanRoutingCreateValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvlanRoutingCreateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvlanRoutingCreateValue, owa.len);

  /* retrieve key: vlanId */
  owa.len = sizeof (keyvlanIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpVlanConfig_vlanId,
      (xLibU8_t *) & keyvlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyvlanIdValue, owa.len);
  if (usmDbMgmtVlanIdGet(L7_UNIT_CURRENT, &mgmtVlanId) == L7_SUCCESS)
  {
     if (mgmtVlanId == keyvlanIdValue)
     {
        owa.rc = XLIBRC_VLAN_ROUTING_MODE_CANT_ENABLEON_MGMTVLAN;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
  }
  owa.l7rc = L7_SUCCESS;
  if (objvlanRoutingCreateValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
      owa.l7rc = usmDbIpVlanRoutingIntfCreate(L7_UNIT_CURRENT, keyvlanIdValue, 0);
      if (owa.l7rc != L7_SUCCESS)
      {
	      owa.rc = XLIBRC_MAX_VLAN_ROUTING_INTF_REACHD;
	      FPOBJ_TRACE_EXIT (bufp, owa);
	      return owa.rc;

      }
      /* Creation of index successful Push the index into the filter*/
        (void)usmDbIpVlanRtrVlanIdToIntIfNum(L7_UNIT_CURRENT, keyvlanIdValue, &intfNum);
        (void)xLibFilterSet (wap, XOBJ_routingswitchIPInterfaceConfig_IfIndex, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));
#ifdef L7_IPV6_PACKAGE
        (void)xLibFilterSet (wap, XOBJ_IPv6Ipv6InterfaceConfigTable_Ipv6InterfaceIfIndex, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));
#endif
        (void)xLibFilterSet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));
        (void)xLibFilterSet (wap, XOBJ_baseInterfaceInfo_ifIndex, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));
        (void)xLibFilterSet (wap, XOBJ_routingLocalArpConfig_interface, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));
        (void)xLibFilterSet (wap, XOBJ_baseport_Interface, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));
  }
  else if (objvlanRoutingCreateValue == L7_ROW_STATUS_DESTROY)
  {
     owa.l7rc = usmDbIpVlanRoutingIntfDelete(L7_UNIT_CURRENT, keyvlanIdValue);
     if (owa.l7rc == L7_SUCCESS)
     {
       intfNum = keyvlanIdValue+8000; 
       (void)xLibFilterSet (wap, XOBJ_baseinterfaceListing_AllRtrIntfVlansLoopbacksTunnels, 0,  (xLibU8_t *)&intfNum,sizeof(intfNum));

     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_VLAN_ROUTING_MODE_CANT_SET;    /* TODO: Change if required */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


