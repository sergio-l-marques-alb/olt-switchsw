
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalInstanceClientAssocTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to captiveportal-object.xml
*
* @create  06 July 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_captiveportalcpCaptivePortalInstanceClientAssocTable_obj.h"

#include "usmdb_cpdm_connstatus_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocInstanceId
*
* @purpose Get 'cpCaptivePortalInstanceClientAssocInstanceId'
 *@description  [cpCaptivePortalInstanceClientAssocInstanceId] The instance ID
* associated with this client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocInstanceId
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceClientAssocInstanceIdValue;
  xLibU32_t nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue=0;
  xLibU16_t cpInstanceId;

  L7_enetMacAddr_t objcpCaptivePortalInstanceClientAssocMacAddressValue;
  L7_enetMacAddr_t nextObjcpCaptivePortalInstanceClientAssocMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceClientAssocInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceClientAssocInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocInstanceId,
                   (xLibU8_t *) & objcpCaptivePortalInstanceClientAssocInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objcpCaptivePortalInstanceClientAssocInstanceIdValue);
    FPOBJ_CLR_MAC (objcpCaptivePortalInstanceClientAssocMacAddressValue.addr);
    FPOBJ_CLR_U32 (nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue);
    FPOBJ_CLR_MAC (nextObjcpCaptivePortalInstanceClientAssocMacAddressValue.addr);
    owa.l7rc = usmDbCpdmCPConnStatusNextGet (objcpCaptivePortalInstanceClientAssocInstanceIdValue,
                    &objcpCaptivePortalInstanceClientAssocMacAddressValue,
                    &cpInstanceId,
                    &nextObjcpCaptivePortalInstanceClientAssocMacAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalInstanceClientAssocInstanceIdValue, owa.len);
    FPOBJ_CLR_MAC (objcpCaptivePortalInstanceClientAssocMacAddressValue.addr);
    FPOBJ_CLR_MAC (nextObjcpCaptivePortalInstanceClientAssocMacAddressValue.addr);
    do
    {
    FPOBJ_CPY_MAC (&objcpCaptivePortalInstanceClientAssocMacAddressValue, &nextObjcpCaptivePortalInstanceClientAssocMacAddressValue);
    owa.l7rc = usmDbCpdmCPConnStatusNextGet (objcpCaptivePortalInstanceClientAssocInstanceIdValue,
                    &objcpCaptivePortalInstanceClientAssocMacAddressValue,
                    &cpInstanceId,
                    &nextObjcpCaptivePortalInstanceClientAssocMacAddressValue);
    }
    while ((objcpCaptivePortalInstanceClientAssocInstanceIdValue == nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue = (xLibU32_t)cpInstanceId;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue, owa.len);

  /* return the object value: cpCaptivePortalInstanceClientAssocInstanceId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue,
                    sizeof (nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocMacAddress
*
* @purpose Get 'cpCaptivePortalInstanceClientAssocMacAddress'
 *@description  [cpCaptivePortalInstanceClientAssocMacAddress] The MAC address
* of the workstation from which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocMacAddress
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_enetMacAddr_t objcpCaptivePortalInstanceClientAssocMacAddressValue;
  L7_enetMacAddr_t nextObjcpCaptivePortalInstanceClientAssocMacAddressValue;

  xLibU32_t objcpCaptivePortalInstanceClientAssocInstanceIdValue;
  xLibU16_t nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceClientAssocInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceClientAssocInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocInstanceId,
                   (xLibU8_t *) & objcpCaptivePortalInstanceClientAssocInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: cpCaptivePortalInstanceClientAssocMacAddress */
  owa.len = sizeof (objcpCaptivePortalInstanceClientAssocMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocMacAddress,
                   (xLibU8_t *) objcpCaptivePortalInstanceClientAssocMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_MAC (objcpCaptivePortalInstanceClientAssocMacAddressValue.addr);

    owa.l7rc = usmDbCpdmCPConnStatusNextGet (objcpCaptivePortalInstanceClientAssocInstanceIdValue,
                    &objcpCaptivePortalInstanceClientAssocMacAddressValue,
                    &nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue,
                    &nextObjcpCaptivePortalInstanceClientAssocMacAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objcpCaptivePortalInstanceClientAssocMacAddressValue.addr, owa.len);
    do
    {
       owa.l7rc = usmDbCpdmCPConnStatusNextGet (objcpCaptivePortalInstanceClientAssocInstanceIdValue,
                       &objcpCaptivePortalInstanceClientAssocMacAddressValue,
                       &nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue,
                       &nextObjcpCaptivePortalInstanceClientAssocMacAddressValue);
    }
    while ((objcpCaptivePortalInstanceClientAssocInstanceIdValue == nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue) && (!(FPOBJ_CMP_MAC (objcpCaptivePortalInstanceClientAssocMacAddressValue.addr, nextObjcpCaptivePortalInstanceClientAssocMacAddressValue.addr))) && (owa.l7rc == L7_SUCCESS));
  }

  if (!(objcpCaptivePortalInstanceClientAssocInstanceIdValue == nextObjcpCaptivePortalInstanceClientAssocInstanceIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjcpCaptivePortalInstanceClientAssocMacAddressValue.addr, owa.len);

  /* return the object value: cpCaptivePortalInstanceClientAssocMacAddress */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) nextObjcpCaptivePortalInstanceClientAssocMacAddressValue.addr,
                    sizeof (nextObjcpCaptivePortalInstanceClientAssocMacAddressValue.addr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocRowStatus
*
* @purpose Get 'cpCaptivePortalInstanceClientAssocRowStatus'
 *@description  [cpCaptivePortalInstanceClientAssocRowStatus] This specifies the
* row-status of this entry. For a valid entry,the row-status will
* return active(1). This is the only supported value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocRowStatus
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceClientAssocRowStatusValue;

  xLibU32_t keycpCaptivePortalInstanceClientAssocInstanceIdValue;
  L7_enetMacAddr_t keycpCaptivePortalInstanceClientAssocMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceClientAssocInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceClientAssocInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceClientAssocInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceClientAssocInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceClientAssocMacAddress */
  owa.len = sizeof (keycpCaptivePortalInstanceClientAssocMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceClientAssocTable_cpCaptivePortalInstanceClientAssocMacAddress,
                   (xLibU8_t *) keycpCaptivePortalInstanceClientAssocMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalInstanceClientAssocMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConnStatusGet (keycpCaptivePortalInstanceClientAssocInstanceIdValue,
                              &keycpCaptivePortalInstanceClientAssocMacAddressValue);

  if (owa.l7rc == L7_SUCCESS)
    objcpCaptivePortalInstanceClientAssocRowStatusValue = L7_ROW_STATUS_ACTIVE;
  else
    objcpCaptivePortalInstanceClientAssocRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceClientAssocRowStatusValue,
                     sizeof (objcpCaptivePortalInstanceClientAssocRowStatusValue));

  /* return the object value: cpCaptivePortalInstanceClientAssocRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceClientAssocRowStatusValue,
                           sizeof (objcpCaptivePortalInstanceClientAssocRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
