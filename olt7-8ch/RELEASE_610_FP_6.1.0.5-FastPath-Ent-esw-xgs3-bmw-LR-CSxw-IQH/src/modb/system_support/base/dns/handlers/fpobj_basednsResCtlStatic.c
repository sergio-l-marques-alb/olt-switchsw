/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basednsResCtlStatic.c
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
#include "_xe_basednsResCtlStatic_obj.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_basednsResCtlStatic_HostName
*
* @purpose Get 'HostName'
*
* @description [HostName] tatic host entries for the DNS client
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtlStatic_HostName (void *wap, void *bufp)
{
  L7_uchar8 objHostNameValue[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_uchar8 nextObjHostNameValue[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  xLibU32_t len;  
  L7_inet_addr_t objIPAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  FPOBJ_TRACE_ENTER (bufp);

  memset(&objIPAddressValue,0x00,sizeof(objIPAddressValue));
  memset(nextObjHostNameValue,0,sizeof(nextObjHostNameValue));
  memset(objHostNameValue,0,sizeof(objHostNameValue));
  
  owa.len = sizeof(objHostNameValue);
  /* retrieve key: HostName */
  owa.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_HostName,
                          (xLibU8_t *) objHostNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbDNSClientStaticHostEntryNextGet (objHostNameValue, &objIPAddressValue);
     if (owa.l7rc == L7_SUCCESS)
     {
        len=osapiStrnlen(objHostNameValue,sizeof(xLibStr256_t));
        if(len == sizeof(objHostNameValue))
       {
          objHostNameValue[len-1] = L7_EOS;
       }
       else
       {
         objHostNameValue[len] = L7_EOS;        
       } 
     }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objHostNameValue, owa.len);
    osapiStrncpySafe(nextObjHostNameValue,objHostNameValue,sizeof(nextObjHostNameValue));

    memset(objHostNameValue,0,sizeof(objHostNameValue));
    owa.l7rc = usmDbDNSClientStaticHostEntryNextGet (objHostNameValue, &objIPAddressValue);

    while((owa.l7rc == L7_SUCCESS) &&
          (strncmp(objHostNameValue, nextObjHostNameValue, sizeof(nextObjHostNameValue)) != 0))
    {
      len=osapiStrnlen(objHostNameValue,sizeof(xLibStr256_t));
      if(len == sizeof(objHostNameValue))
      {
        objHostNameValue[len-1] = L7_EOS;
      }
      else
      {
        objHostNameValue[len] = L7_EOS;        
      } 
      owa.l7rc = usmDbDNSClientStaticHostEntryNextGet (objHostNameValue, &objIPAddressValue);
    }

    if(owa.l7rc == L7_SUCCESS)
    {
      /* Found the matching host name entry in the list, now iterate again till you get the next host name */
      do
      {
        owa.l7rc = usmDbDNSClientStaticHostEntryNextGet (objHostNameValue, &objIPAddressValue);
        if (owa.l7rc == L7_SUCCESS)
        {
          len=osapiStrnlen(objHostNameValue,sizeof(xLibStr256_t));
          if(len == sizeof(objHostNameValue))
         {
            objHostNameValue[len-1] = L7_EOS;
         }
         else
         {
           objHostNameValue[len] = L7_EOS;        
         } 
        }
      }
      while ((strncmp(objHostNameValue, nextObjHostNameValue, sizeof(nextObjHostNameValue)) == 0) && (owa.l7rc == L7_SUCCESS));
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiStrncpySafe(nextObjHostNameValue,objHostNameValue,sizeof(objHostNameValue));
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjHostNameValue, owa.len);

  /* return the object value: HostName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjHostNameValue, strlen (objHostNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basednsResCtlStatic_IPAddress
*
* @purpose Get 'IPAddress'
*
* @description [IPAddress] tatic host entries for the DNS client
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtlStatic_IPAddress (void *wap, void *bufp)
{

  L7_uchar8 objHostNameValue[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_uchar8 nextObjHostNameValue[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_inet_addr_t objIPAddressValue;
  L7_inet_addr_t nextObjIPAddressValue;
  xLibU32_t len;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);
  memset(&objIPAddressValue,0x00,sizeof(objIPAddressValue)); 
  memset(objHostNameValue,0x00,sizeof(objHostNameValue));

  owa.len=sizeof(objHostNameValue);

  /* retrieve key: HostName */
  owa.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_HostName,
                          (xLibU8_t *) objHostNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objHostNameValue, owa.len);

  /* retrieve key: IPAddress */
  owa.len=sizeof(objIPAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_IPAddress,
                           (xLibU8_t *)&objIPAddressValue, &owa.len);

  osapiStrncpySafe(nextObjHostNameValue,objHostNameValue,sizeof(nextObjHostNameValue));
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&objIPAddressValue,0x00,sizeof(objIPAddressValue));
    owa.l7rc = usmDbDNSClientStaticHostEntry (objHostNameValue, &objIPAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIPAddressValue, owa.len);
    owa.l7rc = usmDbDNSClientStaticHostEntryNextGet (objHostNameValue, &objIPAddressValue);
  }
  if (owa.l7rc == L7_SUCCESS)
  {
   len=osapiStrnlen(objHostNameValue,sizeof(xLibStr256_t));      
    if(len == sizeof(objHostNameValue))
   {
      objHostNameValue[len-1] = L7_EOS;
   }
   else
   {
     objHostNameValue[len] = L7_EOS;        
   } 
  }

  if ((owa.l7rc != L7_SUCCESS) ||
      (strncmp(objHostNameValue, nextObjHostNameValue, sizeof(nextObjHostNameValue)) != 0))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  memcpy(&nextObjIPAddressValue, &objIPAddressValue,sizeof(L7_inet_addr_t));
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIPAddressValue, owa.len);

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjIPAddressValue, sizeof (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basednsResCtlStatic_Status
*
* @purpose Get 'Status'
*
* @description [Status]: Name server status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basednsResCtlStatic_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyHostNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyIPAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: HostName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_HostName,
                           (xLibU8_t *) keyHostNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyHostNameValue, kwa1.len);

  /* retrieve key: IPAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_IPAddress,
                           (xLibU8_t *) & keyIPAddressValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa2.len);

  if ( usmDbDNSClientStaticHostEntryGet(keyHostNameValue, &keyIPAddressValue) == L7_SUCCESS)
  {
    objStatusValue = L7_ROW_STATUS_ACTIVE;
  }
  else
  {
    objStatusValue = L7_ROW_STATUS_INVALID;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basednsResCtlStatic_Status
*
* @purpose Set 'Status'
*
* @description [Status]: Name server status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basednsResCtlStatic_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX);
  L7_uchar8 keyHostNameValue[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyIPAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyHostNameValue,0x00,sizeof(keyHostNameValue));
  memset(&keyIPAddressValue,0x00,sizeof(keyIPAddressValue));
  objStatusValue = 0;
  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: HostName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_HostName,
                           (xLibU8_t *) keyHostNameValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyHostNameValue, kwa1.len);

  /* retrieve key: IPAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basednsResCtlStatic_IPAddress,
                           (xLibU8_t *) & keyIPAddressValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    if (usmDbHostNameValidate(keyHostNameValue) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbDNSClientStaticHostEntryAdd (keyHostNameValue, &keyIPAddressValue);
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
    owa.l7rc = usmDbDNSClientStaticHostEntryRemove (keyHostNameValue, keyIPAddressValue.family);
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

