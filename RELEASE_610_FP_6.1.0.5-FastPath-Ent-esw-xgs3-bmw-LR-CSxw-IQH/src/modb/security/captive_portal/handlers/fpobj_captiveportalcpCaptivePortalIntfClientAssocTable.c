
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalIntfClientAssocTable.c
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
#include "_xe_captiveportalcpCaptivePortalIntfClientAssocTable_obj.h"

#include "usmdb_cpdm_connstatus_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientIfIndex
*
* @purpose Get 'cpCaptivePortalIntfClientIfIndex'
 *@description  [cpCaptivePortalIntfClientIfIndex] The interface on which the
* client was authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientIfIndex (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfClientIfIndexValue;
  xLibU32_t nextObjcpCaptivePortalIntfClientIfIndexValue;

  L7_enetMacAddr_t  objcpCaptivePortalIntfClientAssocMacAddressValue;
  L7_enetMacAddr_t  nextObjcpCaptivePortalIntfClientAssocMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalIntfClientIfIndex */
  owa.len = sizeof (objcpCaptivePortalIntfClientIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientIfIndex,
                   (xLibU8_t *) & objcpCaptivePortalIntfClientIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objcpCaptivePortalIntfClientIfIndexValue);
    FPOBJ_CLR_U32 (nextObjcpCaptivePortalIntfClientIfIndexValue);
    FPOBJ_CLR_MAC (objcpCaptivePortalIntfClientAssocMacAddressValue.addr);
    FPOBJ_CLR_MAC (nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr);
    owa.l7rc = usmDbCpdmInterfaceConnStatusNextGet (objcpCaptivePortalIntfClientIfIndexValue, 
                    &objcpCaptivePortalIntfClientAssocMacAddressValue, 
                    &nextObjcpCaptivePortalIntfClientIfIndexValue, 
                    &nextObjcpCaptivePortalIntfClientAssocMacAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalIntfClientIfIndexValue, owa.len);
    FPOBJ_CLR_MAC (objcpCaptivePortalIntfClientAssocMacAddressValue.addr);
    FPOBJ_CLR_MAC (nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr);
    do
    {
        FPOBJ_CPY_MAC (objcpCaptivePortalIntfClientAssocMacAddressValue.addr, nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr);
        owa.l7rc = usmDbCpdmInterfaceConnStatusNextGet (objcpCaptivePortalIntfClientIfIndexValue, 
                    &objcpCaptivePortalIntfClientAssocMacAddressValue, 
                    &nextObjcpCaptivePortalIntfClientIfIndexValue, 
                    &nextObjcpCaptivePortalIntfClientAssocMacAddressValue);
    }
    while((objcpCaptivePortalIntfClientIfIndexValue == nextObjcpCaptivePortalIntfClientIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalIntfClientIfIndexValue, owa.len);

  /* return the object value: cpCaptivePortalIntfClientIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalIntfClientIfIndexValue,
                           sizeof (nextObjcpCaptivePortalIntfClientIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientAssocMacAddress
*
* @purpose Get 'cpCaptivePortalIntfClientAssocMacAddress'
 *@description  [cpCaptivePortalIntfClientAssocMacAddress] The MAC address of
* the workstation from which the client is authenticated.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientAssocMacAddress
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_enetMacAddr_t  objcpCaptivePortalIntfClientAssocMacAddressValue;
  L7_enetMacAddr_t  nextObjcpCaptivePortalIntfClientAssocMacAddressValue;

  xLibU32_t objcpCaptivePortalIntfClientIfIndexValue;
  xLibU32_t nextObjcpCaptivePortalIntfClientIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalIntfClientIfIndex */
  owa.len = sizeof (objcpCaptivePortalIntfClientIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientIfIndex,
                   (xLibU8_t *) & objcpCaptivePortalIntfClientIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: cpCaptivePortalIntfClientAssocMacAddress */
  owa.len = sizeof (objcpCaptivePortalIntfClientAssocMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientAssocMacAddress,
                   (xLibU8_t *) objcpCaptivePortalIntfClientAssocMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_MAC (objcpCaptivePortalIntfClientAssocMacAddressValue.addr);

    owa.l7rc = usmDbCpdmInterfaceConnStatusNextGet (objcpCaptivePortalIntfClientIfIndexValue, 
                    &objcpCaptivePortalIntfClientAssocMacAddressValue, 
                    &nextObjcpCaptivePortalIntfClientIfIndexValue, 
                    &nextObjcpCaptivePortalIntfClientAssocMacAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objcpCaptivePortalIntfClientAssocMacAddressValue.addr, owa.len);
    do
    {
        owa.l7rc = usmDbCpdmInterfaceConnStatusNextGet (objcpCaptivePortalIntfClientIfIndexValue, 
                        &objcpCaptivePortalIntfClientAssocMacAddressValue, 
                        &nextObjcpCaptivePortalIntfClientIfIndexValue, 
                        &nextObjcpCaptivePortalIntfClientAssocMacAddressValue);
    }
    while ((objcpCaptivePortalIntfClientIfIndexValue == nextObjcpCaptivePortalIntfClientIfIndexValue) && (!FPOBJ_CMP_MAC(objcpCaptivePortalIntfClientAssocMacAddressValue.addr,nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr)) && (owa.l7rc == L7_SUCCESS));
  }

  if (!(objcpCaptivePortalIntfClientIfIndexValue == nextObjcpCaptivePortalIntfClientIfIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr, owa.len);

  /* return the object value: cpCaptivePortalIntfClientAssocMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr,
                           sizeof (nextObjcpCaptivePortalIntfClientAssocMacAddressValue.addr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientAssocRowStatus
*
* @purpose Get 'cpCaptivePortalIntfClientAssocRowStatus'
 *@description  [cpCaptivePortalIntfClientAssocRowStatus] This specifies the
* row-status of this entry. For a valid entry,the row-status will
* return active(1). This is the only supported value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientAssocRowStatus
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfClientAssocRowStatusValue;

  xLibU32_t keycpCaptivePortalIntfClientIfIndexValue;
  L7_enetMacAddr_t  keycpCaptivePortalIntfClientAssocMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalIntfClientIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfClientIfIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfClientIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfClientIfIndexValue, owa.len);

  /* retrieve key: cpCaptivePortalIntfClientAssocMacAddress */
  owa.len = sizeof (keycpCaptivePortalIntfClientAssocMacAddressValue.addr);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalIntfClientAssocTable_cpCaptivePortalIntfClientAssocMacAddress,
                   (xLibU8_t *) keycpCaptivePortalIntfClientAssocMacAddressValue.addr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keycpCaptivePortalIntfClientAssocMacAddressValue.addr, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmInterfaceConnStatusGet (keycpCaptivePortalIntfClientIfIndexValue,
                              &keycpCaptivePortalIntfClientAssocMacAddressValue);

  if (owa.l7rc == L7_SUCCESS)
    objcpCaptivePortalIntfClientAssocRowStatusValue = L7_ROW_STATUS_ACTIVE;
  else
    objcpCaptivePortalIntfClientAssocRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIntfClientAssocRowStatusValue,
                     sizeof (objcpCaptivePortalIntfClientAssocRowStatusValue));

  /* return the object value: cpCaptivePortalIntfClientAssocRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIntfClientAssocRowStatusValue,
                           sizeof (objcpCaptivePortalIntfClientAssocRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
