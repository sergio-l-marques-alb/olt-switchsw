
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDhcpClientVendorGlobalConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  02 July 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDhcpClientVendorGlobalConfig_obj.h"
#include "usmdb_dhcp_client.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdMode
*
* @purpose Get 'VendorClassIdMode'
 *@description  [VendorClassIdMode] This object indicates whether DHCP Vendor
* Classifier Id Option (60) is enabled for DHCP Network Client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVendorClassIdModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDhcpVendorClassOptionAdminModeGet(&objVendorClassIdModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objVendorClassIdModeValue, sizeof (objVendorClassIdModeValue));

  /* return the object value: VendorClassIdMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVendorClassIdModeValue,
                           sizeof (objVendorClassIdModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdMode
*
* @purpose Set 'VendorClassIdMode'
 *@description  [VendorClassIdMode] This object indicates whether DHCP Vendor
* Classifier Id Option (60) is enabled for DHCP Network Client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVendorClassIdModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VendorClassIdMode */
  owa.len = sizeof (objVendorClassIdModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVendorClassIdModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVendorClassIdModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDhcpVendorClassOptionAdminModeSet(objVendorClassIdModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdString
*
* @purpose Get 'VendorClassIdString'
 *@description  [VendorClassIdString] When this object is set with a non-empty
* string, the DHCP Vendor Classifier Id Option (60) string is
* enabled for DHCP Network Client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdString (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objVendorClassIdStringValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(objVendorClassIdStringValue,0x00,sizeof(objVendorClassIdStringValue));
  owa.l7rc = usmdbDhcpVendorClassOptionStringGet(objVendorClassIdStringValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objVendorClassIdStringValue, strlen (objVendorClassIdStringValue));

  /* return the object value: VendorClassIdString */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVendorClassIdStringValue,
                           strlen (objVendorClassIdStringValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdString
*
* @purpose Set 'VendorClassIdString'
 *@description  [VendorClassIdString] When this object is set with a non-empty
* string, the DHCP Vendor Classifier Id Option (60) string is
* enabled for DHCP Network Client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDhcpClientVendorGlobalConfig_VendorClassIdString (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objVendorClassIdStringValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VendorClassIdString */
  owa.len = sizeof (objVendorClassIdStringValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objVendorClassIdStringValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objVendorClassIdStringValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDhcpVendorClassOptionStringSet(objVendorClassIdStringValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
