
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDhcpL2RelayStats.c
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
#include "_xe_SwitchingDhcpL2RelayStats_obj.h"
#include "dhcp_snooping_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dhcp_snooping.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayStats_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] A row instance contains the DHCP L2Relay statistics
* per interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayStats_Interface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayStats_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextVisibleExtIfNumber(0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbGetNextVisibleExtIfNumber(objInterfaceValue, &nextObjInterfaceValue);
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
* @function fpObjGet_SwitchingDhcpL2RelayStats_UntrustedSrvrMsgsWithOptn82
*
* @purpose Get 'UntrustedSrvrMsgsWithOptn82'
 *@description  [UntrustedSrvrMsgsWithOptn82] This object indicates the number
* of DHCP server messages received with Option-82 field on the
* untrusted interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayStats_UntrustedSrvrMsgsWithOptn82 (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUntrustedSrvrMsgsWithOptn82Value;
  dsL2RelayIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayIntfStatsGet(keyInterfaceValue,
                              &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUntrustedSrvrMsgsWithOptn82Value = intfStats.untrustedSrvMsgsWithOpt82;
  /* return the object value: UntrustedSrvrMsgsWithOptn82 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUntrustedSrvrMsgsWithOptn82Value,
                           sizeof (objUntrustedSrvrMsgsWithOptn82Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayStats_UntrustedClntMsgsWithOptn82
*
* @purpose Get 'UntrustedClntMsgsWithOptn82'
 *@description  [UntrustedClntMsgsWithOptn82] This object indicates the number
* of DHCP client messages received with Option-82 field on the
* untrusted interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayStats_UntrustedClntMsgsWithOptn82 (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUntrustedClntMsgsWithOptn82Value;
  dsL2RelayIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayIntfStatsGet(keyInterfaceValue,
                              &intfStats);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objUntrustedClntMsgsWithOptn82Value = intfStats.untrustedCliMsgsWithOpt82;

  /* return the object value: UntrustedClntMsgsWithOptn82 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUntrustedClntMsgsWithOptn82Value,
                           sizeof (objUntrustedClntMsgsWithOptn82Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayStats_TrustedSrvrMsgsWithoutOptn82
*
* @purpose Get 'TrustedSrvrMsgsWithoutOptn82'
 *@description  [TrustedSrvrMsgsWithoutOptn82] This object indicates the number
* of DHCP server messages received without Option-82 field on the
* trusted interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayStats_TrustedSrvrMsgsWithoutOptn82 (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrustedSrvrMsgsWithoutOptn82Value;
  dsL2RelayIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayIntfStatsGet(keyInterfaceValue,
                              &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objTrustedSrvrMsgsWithoutOptn82Value = intfStats.trustedSrvMsgsWithoutOpt82;

  /* return the object value: TrustedSrvrMsgsWithoutOptn82 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrustedSrvrMsgsWithoutOptn82Value,
                           sizeof (objTrustedSrvrMsgsWithoutOptn82Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayStats_TrustedClntMsgsWithoutOptn82
*
* @purpose Get 'TrustedClntMsgsWithoutOptn82'
 *@description  [TrustedClntMsgsWithoutOptn82] This object indicates the number
* of DHCP client messages received without Option-82 field on the
* trusted interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayStats_TrustedClntMsgsWithoutOptn82 (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrustedClntMsgsWithoutOptn82Value;
  dsL2RelayIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayIntfStatsGet(keyInterfaceValue,
                              &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objTrustedClntMsgsWithoutOptn82Value = intfStats.trustedCliMsgsWithoutOpt82;

  /* return the object value: TrustedClntMsgsWithoutOptn82 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrustedClntMsgsWithoutOptn82Value,
                           sizeof (objTrustedClntMsgsWithoutOptn82Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayStats_IntfStatsClear
*
* @purpose Set 'IntfStatsClear'
 *@description  [IntfStatsClear] Clear the DHCP L2Relay statistics on ports.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayStats_IntfStatsClear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIntfStatsClearValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfStatsClear */
  owa.len = sizeof (objIntfStatsClearValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIntfStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfStatsClearValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDhcpL2RelayStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayIntfStatsClear(keyInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayStats_IntfStatsClearAll
*
* @purpose Set 'IntfStatsClearAll'
 *@description  [IntfStatsClearAll] Clear the DHCP L2Relay statistics on ALL
* ports.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayStats_IntfStatsClearAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIntfStatsClearAllValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfStatsClearAll */
  owa.len = sizeof (objIntfStatsClearAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIntfStatsClearAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfStatsClearAllValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  objInterfaceValue = 0;

  while(usmDbGetNextVisibleExtIfNumber(objInterfaceValue, &nextObjInterfaceValue) == L7_SUCCESS)
  {
    owa.l7rc = usmDbDsL2RelayIntfStatsClear(nextObjInterfaceValue);
    objInterfaceValue = nextObjInterfaceValue;
  } 

  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

