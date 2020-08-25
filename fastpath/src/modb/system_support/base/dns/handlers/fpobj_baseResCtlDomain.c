/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseResCtlDomain.c
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
#include "_xe_baseResCtlDomain_obj.h"
#include "usmdb_dns_client_api.h"

/*******************************************************************************
* @function fpObjGet_baseResCtlDomain_ListName
*
* @purpose Get 'ListName'
*
* @description [ListName]: The dns domain list entry identified by this row
*              of the table. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseResCtlDomain_ListName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objListNameValue;
  xLibStr256_t nextObjListNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ListName */
  owa.rc = xLibFilterGet (wap, XOBJ_baseResCtlDomain_ListName,
                          (xLibU8_t *) objListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objListNameValue,0x00,sizeof(objListNameValue));
    memset(nextObjListNameValue,0x00,sizeof(nextObjListNameValue));
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objListNameValue, owa.len);
  owa.l7rc = usmDbDNSClientDomainNameListGetNext (objListNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(nextObjListNameValue,objListNameValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjListNameValue, owa.len);

  /* return the object value: ListName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjListNameValue,
                           strlen (objListNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseResCtlDomain_Status
*
* @purpose Get 'Status'
*
* @description [Status]: Status of the DNS domain list Entry Table Supported
*              values: active(1) - valid entry createAndGo(4) - used to
*              create a new entry destroy(6) - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseResCtlDomain_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyListNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ListName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseResCtlDomain_ListName,
                          (xLibU8_t *) keyListNameValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyListNameValue, kwa.len);

  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseResCtlDomain_Status
*
* @purpose Set 'Status'
*
* @description [Status]: Status of the DNS domain list Entry Table Supported
*              values: active(1) - valid entry createAndGo(4) - used to
*              create a new entry destroy(6) - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseResCtlDomain_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyListNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: ListName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseResCtlDomain_ListName,
                          (xLibU8_t *) keyListNameValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyListNameValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbDNSClientDomainNameListSet (keyListNameValue);
    if (owa.l7rc == L7_NOT_SUPPORTED)
    {
      owa.rc = XLIBRC_INVALID_DOMAIN_NAME;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else if (owa.l7rc != L7_SUCCESS)
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
    owa.l7rc = usmDbDNSClientDomainNameListRemove (keyListNameValue);
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
