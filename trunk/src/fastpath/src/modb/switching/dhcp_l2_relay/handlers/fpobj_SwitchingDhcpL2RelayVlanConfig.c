
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDhcpL2RelayVlanConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  29 June 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDhcpL2RelayVlanConfig_obj.h"
#include "usmdb_dhcp_snooping.h"
#include "dot1q_exports.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayVlanConfig_VlanID
*
* @purpose Get 'VlanID'
 *@description  [VlanID] This object indicates the VLAN number on which DHCP L2
* Relay feature is configured.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayVlanConfig_VlanID (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVlanIDValue;
  xLibU32_t nextObjVlanIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanID */
  owa.len = sizeof (objVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & objVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanIDValue = 1;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIDValue, owa.len);

    if ( ((objVlanIDValue+1) < L7_DOT1Q_MIN_VLAN_ID) ||
        ((objVlanIDValue+1) > L7_DOT1Q_MAX_VLAN_ID) )
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjVlanIDValue = objVlanIDValue + 1;
      owa.l7rc = L7_SUCCESS;
    }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIDValue, owa.len);

  /* return the object value: VlanID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIDValue, sizeof (nextObjVlanIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayVlanConfig_L2RelayMode
*
* @purpose Get 'L2RelayMode'
 *@description  [L2RelayMode] This object indicates whether DHCP L2 Relay is
* enabled in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayVlanConfig_L2RelayMode (void *wap, void *bufp)
{

  xLibU32_t keyVlanIDValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanID */
  owa.len = sizeof (keyVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & keyVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayVlanModeGet(keyVlanIDValue, &objL2RelayModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: L2RelayMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objL2RelayModeValue, sizeof (objL2RelayModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayVlanConfig_L2RelayMode
*
* @purpose Set 'L2RelayMode'
 *@description  [L2RelayMode] This object indicates whether DHCP L2 Relay is
* enabled in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayVlanConfig_L2RelayMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayModeValue;

  xLibU32_t keyVlanIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: L2RelayMode */
  owa.len = sizeof (objL2RelayModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objL2RelayModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objL2RelayModeValue, owa.len);

  /* retrieve key: VlanID */
  owa.len = sizeof (keyVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & keyVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayVlanModeSet(keyVlanIDValue, keyVlanIDValue, objL2RelayModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayVlanConfig_CircuitIdMode
*
* @purpose Get 'CircuitIdMode'
 *@description  [CircuitIdMode] This object indicates whether DHCP CircuitId is
* enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayVlanConfig_CircuitIdMode (void *wap, void *bufp)
{

  xLibU32_t keyVlanIDValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCircuitIdModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanID */
  owa.len = sizeof (keyVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & keyVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayCircuitIdGet(keyVlanIDValue, &objCircuitIdModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CircuitIdMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCircuitIdModeValue,
                           sizeof (objCircuitIdModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayVlanConfig_CircuitIdMode
*
* @purpose Set 'CircuitIdMode'
 *@description  [CircuitIdMode] This object indicates whether DHCP CircuitId is
* enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayVlanConfig_CircuitIdMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCircuitIdModeValue;

  xLibU32_t keyVlanIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CircuitIdMode */
  owa.len = sizeof (objCircuitIdModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCircuitIdModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCircuitIdModeValue, owa.len);

  /* retrieve key: VlanID */
  owa.len = sizeof (keyVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & keyVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayCircuitIdSet(keyVlanIDValue, keyVlanIDValue, objCircuitIdModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayVlanConfig_L2RelayRemoteId
*
* @purpose Get 'L2RelayRemoteId'
 *@description  [L2RelayRemoteId] When this object is set with a non-empty
* string, DHCP RemoteId is enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayVlanConfig_L2RelayRemoteId (void *wap, void *bufp)
{

  xLibU32_t keyVlanIDValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objL2RelayRemoteIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanID */
  owa.len = sizeof (keyVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & keyVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayRemoteIdGet(keyVlanIDValue, objL2RelayRemoteIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: L2RelayRemoteId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objL2RelayRemoteIdValue,
                           strlen (objL2RelayRemoteIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayVlanConfig_L2RelayRemoteId
*
* @purpose Set 'L2RelayRemoteId'
 *@description  [L2RelayRemoteId] When this object is set with a non-empty
* string, DHCP RemoteId is enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayVlanConfig_L2RelayRemoteId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objL2RelayRemoteIdValue;

  xLibU32_t keyVlanIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: L2RelayRemoteId */
  owa.len = sizeof (objL2RelayRemoteIdValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objL2RelayRemoteIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objL2RelayRemoteIdValue, owa.len);

  /* retrieve key: VlanID */
  owa.len = sizeof (keyVlanIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayVlanConfig_VlanID,
                          (xLibU8_t *) & keyVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayRemoteIdSet(keyVlanIDValue, keyVlanIDValue, objL2RelayRemoteIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
