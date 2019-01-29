/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basednsResCtl.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dns-object.xml
*
* @create  4 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basednsResCtl_obj.h"
#include "usmdb_dns_client_api.h"

/*******************************************************************************
* @function fpObjGet_basednsResCtl_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description the global admin mode for the DNS client 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtl_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientAdminModeGet (&objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue,
                           sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basednsResCtl_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description the global admin mode for the DNS client 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basednsResCtl_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDNSClientAdminModeSet (objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basednsResCtl_DefDomainName
*
* @purpose Get 'DefDomainName'
*
* @description the default domain for the DNS client 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtl_DefDomainName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDefDomainNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientDefaultDomainGet (objDefDomainNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefDomainNameValue,
                     strlen (objDefDomainNameValue));

  /* return the object value: DefDomainName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefDomainNameValue,
                           strlen (objDefDomainNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basednsResCtl_DefDomainName
*
* @purpose Set 'DefDomainName'
*
* @description the default domain for the DNS client 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basednsResCtl_DefDomainName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDefDomainNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefDomainName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefDomainNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefDomainNameValue, owa.len);

  /* set the value in application */
  if ( osapiStrnlen(objDefDomainNameValue, sizeof(objDefDomainNameValue)) == 0 )
  {
    owa.l7rc = usmDbDNSClientDefaultDomainClear();
  }
  else
  {
    owa.l7rc = usmDbDNSClientDefaultDomainSet (objDefDomainNameValue);
  }
  if (owa.l7rc == L7_NOT_SUPPORTED)
  {
    owa.rc = XLIBRC_INVALID_DOMAIN_NAME;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basednsResCtl_RequestTimeout
*
* @purpose Get 'RequestTimeout'
*
* @description DNS query timeout value 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtl_RequestTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRequestTimeoutValue;
  xLibU16_t requestTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientQueryTimeoutGet (&requestTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objRequestTimeoutValue = requestTimeoutValue;

  FPOBJ_TRACE_VALUE (bufp, &objRequestTimeoutValue,
                     sizeof (objRequestTimeoutValue));

  /* return the object value: RequestTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRequestTimeoutValue,
                           sizeof (objRequestTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basednsResCtl_RequestTimeout
*
* @purpose Set 'RequestTimeout'
*
* @description DNS query timeout value 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basednsResCtl_RequestTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRequestTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RequestTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRequestTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRequestTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDNSClientQueryTimeoutSet ((xLibU16_t ) objRequestTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basednsResCtl_RequestRetransmits
*
* @purpose Get 'RequestRetransmits'
*
* @description number of DNS request retransmits 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtl_RequestRetransmits (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRequestRetransmitsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientRetransmitsGet (&objRequestRetransmitsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRequestRetransmitsValue,
                     sizeof (objRequestRetransmitsValue));

  /* return the object value: RequestRetransmits */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRequestRetransmitsValue,
                           sizeof (objRequestRetransmitsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basednsResCtl_RequestRetransmits
*
* @purpose Set 'RequestRetransmits'
*
* @description number of DNS request retransmits 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basednsResCtl_RequestRetransmits (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRequestRetransmitsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RequestRetransmits */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRequestRetransmitsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRequestRetransmitsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDNSClientRetransmitsSet (objRequestRetransmitsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
