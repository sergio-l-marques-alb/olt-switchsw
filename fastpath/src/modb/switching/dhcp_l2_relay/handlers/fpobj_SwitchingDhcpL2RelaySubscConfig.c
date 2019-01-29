
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDhcpL2RelaySubscConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  14 August 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDhcpL2RelaySubscConfig_obj.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelaySubscConfig_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] A row instance contains the configuration for DHCP
* L2 Relay at each physical interface capable of this feature.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelaySubscConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextVisibleExtIfNumber(0, &nextObjInterfaceValue);
    if (owa.l7rc == L7_SUCCESS)
    {
      if (usmDbDsIntfIsValid(nextObjInterfaceValue) != L7_TRUE)
      {
        owa.l7rc = L7_FAILURE;
      }
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbGetNextVisibleExtIfNumber(objInterfaceValue, &nextObjInterfaceValue);
    if (owa.l7rc == L7_SUCCESS)
    {
      if (usmDbDsIntfIsValid(nextObjInterfaceValue) != L7_TRUE)
      {
        owa.l7rc = L7_FAILURE;
      }
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (nextObjInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelaySubscConfig_SubscriptionName
*
* @purpose Get 'SubscriptionName'
 *@description  [SubscriptionName] A row instance contains the configuration for
* DHCP L2 Relay at each subscription capable of this feature.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelaySubscConfig_SubscriptionName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t keyInterfaceValue;

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  xLibStr256_t objSubscriptionNameValue;
  xLibStr256_t nextObjSubscriptionNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (objSubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) objSubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_STR256 (objSubscriptionNameValue);
    nextObjInterfaceValue = objInterfaceValue;
    owa.l7rc = usmdbDsL2RelayIntfSubscriptionNextGet (&nextObjInterfaceValue,
                                     nextObjSubscriptionNameValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objSubscriptionNameValue, owa.len);
    nextObjInterfaceValue = objInterfaceValue;
    FPOBJ_CPY_STR256(nextObjSubscriptionNameValue, objSubscriptionNameValue);
    owa.l7rc = usmdbDsL2RelayIntfSubscriptionNextGet (&nextObjInterfaceValue,
                                    nextObjSubscriptionNameValue);

  }

  if ((!FPOBJ_CMP_U32 (objInterfaceValue, nextObjInterfaceValue)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjSubscriptionNameValue, owa.len);

  /* return the object value: SubscriptionName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjSubscriptionNameValue,
                           strlen (nextObjSubscriptionNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelaySubscConfig_L2RelayMode
*
* @purpose Get 'L2RelayMode'
 *@description  [L2RelayMode] This object indicates whether DHCP L2 Relay is
* enabled in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelaySubscConfig_L2RelayMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayModeValue;

  xLibU32_t keyInterfaceValue;
  xLibStr256_t keySubscriptionNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (keySubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) keySubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySubscriptionNameValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbDsL2RelaySubscripionModeGet (keyInterfaceValue,
                              keySubscriptionNameValue, &objL2RelayModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objL2RelayModeValue, sizeof (objL2RelayModeValue));

  /* return the object value: L2RelayMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objL2RelayModeValue, sizeof (objL2RelayModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelaySubscConfig_L2RelayMode
*
* @purpose Set 'L2RelayMode'
 *@description  [L2RelayMode] This object indicates whether DHCP L2 Relay is
* enabled in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelaySubscConfig_L2RelayMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayModeValue;

  xLibU32_t keyInterfaceValue;
  xLibStr256_t keySubscriptionNameValue;

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

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (keySubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) keySubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySubscriptionNameValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbDsL2RelaySubscripionModeSet (keyInterfaceValue,
                              keySubscriptionNameValue, objL2RelayModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelaySubscConfig_CircuitIdMode
*
* @purpose Get 'CircuitIdMode'
 *@description  [CircuitIdMode] This object indicates whether DHCP CircuitId is
* enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelaySubscConfig_CircuitIdMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCircuitIdModeValue;

  xLibU32_t keyInterfaceValue;
  xLibStr256_t keySubscriptionNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (keySubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) keySubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySubscriptionNameValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbDsL2RelaySubscripionCircuitIdModeGet (keyInterfaceValue,
                              keySubscriptionNameValue, &objCircuitIdModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objCircuitIdModeValue, sizeof (objCircuitIdModeValue));

  /* return the object value: CircuitIdMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCircuitIdModeValue,
                           sizeof (objCircuitIdModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelaySubscConfig_CircuitIdMode
*
* @purpose Set 'CircuitIdMode'
 *@description  [CircuitIdMode] This object indicates whether DHCP CircuitId is
* enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelaySubscConfig_CircuitIdMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCircuitIdModeValue;

  xLibU32_t keyInterfaceValue;
  xLibStr256_t keySubscriptionNameValue;

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

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (keySubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) keySubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySubscriptionNameValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbDsL2RelaySubscripionCircuitIdModeSet (keyInterfaceValue,
                              keySubscriptionNameValue, objCircuitIdModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelaySubscConfig_L2RelayRemoteId
*
* @purpose Get 'L2RelayRemoteId'
 *@description  [L2RelayRemoteId] When this object is set with a non-empty
* string, DHCP RemoteId is enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelaySubscConfig_L2RelayRemoteId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objL2RelayRemoteIdValue;

  xLibU32_t keyInterfaceValue;
  xLibStr256_t keySubscriptionNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (keySubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) keySubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySubscriptionNameValue, owa.len);

  /* get the value from application */
  owa.l7rc = dsL2RelaySubscripionRemoteIdModeGet (keyInterfaceValue,
                              keySubscriptionNameValue, objL2RelayRemoteIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objL2RelayRemoteIdValue, strlen (objL2RelayRemoteIdValue));

  /* return the object value: L2RelayRemoteId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objL2RelayRemoteIdValue,
                           strlen (objL2RelayRemoteIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelaySubscConfig_L2RelayRemoteId
*
* @purpose Set 'L2RelayRemoteId'
 *@description  [L2RelayRemoteId] When this object is set with a non-empty
* string, DHCP RemoteId is enabled for L2 relaying in this VLAN.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelaySubscConfig_L2RelayRemoteId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objL2RelayRemoteIdValue;

  xLibU32_t keyInterfaceValue;
  xLibStr256_t keySubscriptionNameValue;

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

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: SubscriptionName */
  owa.len = sizeof (keySubscriptionNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelaySubscConfig_SubscriptionName,
                          (xLibU8_t *) keySubscriptionNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keySubscriptionNameValue, owa.len);

  /* set the value in application */
  owa.l7rc = dsL2RelaySubscripionRemoteIdModeSet (keyInterfaceValue,
                              keySubscriptionNameValue, objL2RelayRemoteIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
