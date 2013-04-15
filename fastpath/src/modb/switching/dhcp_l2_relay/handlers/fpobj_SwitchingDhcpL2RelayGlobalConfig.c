
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDhcpL2RelayGlobalConfig.c
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
#include "_xe_SwitchingDhcpL2RelayGlobalConfig_obj.h"
#include "usmdb_dhcp_snooping.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpL2RelayGlobalConfig_AdminMode
*
* @purpose Get 'AdminMode'
 *@description  [AdminMode] This object indicates whether DHCP L2 Relay is
* enabled globally or not.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpL2RelayGlobalConfig_AdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDsL2RelayAdminModeGet(&objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue, sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpL2RelayGlobalConfig_AdminMode
*
* @purpose Set 'AdminMode'
 *@description  [AdminMode] This object indicates whether DHCP L2 Relay is
* enabled globally or not.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpL2RelayGlobalConfig_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.len = sizeof (objAdminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDsL2RelayAdminModeSet(objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
