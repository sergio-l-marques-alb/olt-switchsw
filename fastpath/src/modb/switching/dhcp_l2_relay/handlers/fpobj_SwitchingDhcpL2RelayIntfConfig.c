
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDhcpL2RelayIntfConfig.c
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
#include "_xe_SwitchingDhcpL2RelayIntfConfig_obj.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayIntfConfig_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] A row instance contains the configuration for DHCP
* L2 Relay at each physical interface capable of this feature.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayIntfConfig_Interface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue=0;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayIntfConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	owa.l7rc = usmDbIntIfNumTypeFirstGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF | USM_LAG_INTF,0,

                                       &nextObjInterfaceValue);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
   owa.l7rc = usmDbIntIfNumTypeNextGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF | USM_LAG_INTF,0,objInterfaceValue,
                                       &nextObjInterfaceValue);

  }

  if (owa.l7rc != L7_SUCCESS)
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
* @function fpObjGet_SwitchingDhcpL2RelayIntfConfig_L2RelayMode
*
* @purpose Get 'L2RelayMode'
 *@description  [L2RelayMode] This object indicates whether the DHCP L2Relay is
* enabled or not   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayIntfConfig_L2RelayMode (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayIntfModeGet(keyInterfaceValue, &objL2RelayModeValue);
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
* @function fpObjSet_SwitchingDhcpL2RelayIntfConfig_L2RelayMode
*
* @purpose Set 'L2RelayMode'
 *@description  [L2RelayMode] This object indicates whether the DHCP L2Relay is
* enabled or not   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayIntfConfig_L2RelayMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayModeValue;

  xLibU32_t keyInterfaceValue;

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
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayIntfModeSet(keyInterfaceValue, objL2RelayModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayIntfConfig_L2RelayTrustMode
*
* @purpose Get 'L2RelayTrustMode'
 *@description  [L2RelayTrustMode] This object indicates whether the interface
* is trusted for DHCP L2 Relay purpose.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayIntfConfig_L2RelayTrustMode (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayTrustModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayIntfTrustGet(keyInterfaceValue, &objL2RelayTrustModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: L2RelayTrustMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objL2RelayTrustModeValue,
                           sizeof (objL2RelayTrustModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayIntfConfig_L2RelayTrustMode
*
* @purpose Set 'L2RelayTrustMode'
 *@description  [L2RelayTrustMode] This object indicates whether the interface
* is trusted for DHCP L2 Relay purpose.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayIntfConfig_L2RelayTrustMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objL2RelayTrustModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: L2RelayTrustMode */
  owa.len = sizeof (objL2RelayTrustModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objL2RelayTrustModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objL2RelayTrustModeValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayIntfTrustSet(keyInterfaceValue, objL2RelayTrustModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
