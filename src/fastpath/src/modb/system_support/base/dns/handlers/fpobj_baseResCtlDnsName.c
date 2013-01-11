/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseResCtlDnsName.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dns-object.xml
*
* @create  4 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseResCtlDnsName_obj.h"
#include "usmdb_dns_client_api.h"

/*******************************************************************************
* @function fpObjGet_baseResCtlDnsName_ServerIP
*
* @purpose Get 'ServerIP'
*
* @description [ServerIP]: The IP address of the dns server identified by
*              this row of the table. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseResCtlDnsName_ServerIP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objServerIPValue;
  L7_inet_addr_t nextObjServerIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ServerIP */
  owa.rc = xLibFilterGet (wap, XOBJ_baseResCtlDnsName_ServerIP,
                          (xLibU8_t *) & objServerIPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&objServerIPValue, L7_NULL, sizeof(L7_inet_addr_t));
  }
  

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objServerIPValue, owa.len);
  owa.l7rc = usmDbDNSClientNameServerEntryNextGet (&objServerIPValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memcpy(&nextObjServerIPValue, &objServerIPValue, sizeof(L7_inet_addr_t));
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjServerIPValue, owa.len);

  /* return the object value: ServerIP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjServerIPValue,
                           sizeof (objServerIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseResCtlDnsName_Preference
*
* @purpose Get 'Preference'
*
* @description [Preference] The preference for a name server.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseResCtlDnsName_Preference (void *wap, void *bufp)
{

  fpObjWa_t kwaServerIP = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyServerIPValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPreferenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ServerIP */
  kwaServerIP.rc = xLibFilterGet (wap, XOBJ_baseResCtlDnsName_ServerIP,
                                  (xLibU8_t *) &keyServerIPValue, &kwaServerIP.len);
  if (kwaServerIP.rc != XLIBRC_SUCCESS)
  {
    kwaServerIP.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaServerIP);
    return kwaServerIP.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyServerIPValue, kwaServerIP.len);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientNameServerEntryPreferenceGet ( &keyServerIPValue, &objPreferenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Preference */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPreferenceValue, sizeof (objPreferenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseResCtlDnsName_Status
*
* @purpose Get 'Status'
*
* @description [Status]: Status of the DNS Server Address Table Supported
*              values: active(1) - valid entry createAndGo(4) - used to create
*              a new entry destroy(6) - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseResCtlDnsName_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyServerIPValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ServerIP */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseResCtlDnsName_ServerIP,
                          (xLibU8_t *) & keyServerIPValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyServerIPValue, kwa.len);

  /* get the value from application */
  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseResCtlDnsName_Status
*
* @purpose Set 'Status'
*
* @description [Status]: Status of the DNS Server Address Table Supported
*              values: active(1) - valid entry createAndGo(4) - used to create
*              a new entry destroy(6) - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseResCtlDnsName_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyServerIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: ServerIP */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseResCtlDnsName_ServerIP,
                          (xLibU8_t *) & keyServerIPValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyServerIPValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbDNSClientNameServerEntryAdd(&keyServerIPValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbDNSClientNameServerEntryRemove ( &keyServerIPValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}
