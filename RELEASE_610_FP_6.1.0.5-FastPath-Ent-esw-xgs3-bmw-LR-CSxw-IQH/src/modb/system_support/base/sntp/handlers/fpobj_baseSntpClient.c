/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSntpClient.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Sntp-object.xml
*
* @create  3 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseSntpClient_obj.h"
#include "usmdb_sntp_api.h"
#include "sntp_exports.h"
#include "usmdb_util_api.h"

#include "osapi.h"
#include "osapi_support.h"

extern L7_char8 * usmDbConvertTimeToDateString(L7_uint32 now);
/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerIndex
*
* @purpose Get 'UcastServerIndex'
*
* @description This object uniquely identifies the entry in the table. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerIndexValue;
  xLibU32_t nextObjUcastServerIndexValue;
  xLibU32_t serverStatusValue;
  xLibU32_t serverIndexValue;
  xLibU32_t serverMaxCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & objUcastServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
 objUcastServerIndexValue = 0;
  }
    nextObjUcastServerIndexValue = 0;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objUcastServerIndexValue, owa.len);
    owa.l7rc = L7_FAILURE; 
    if (usmDbSntpServerMaxEntriesGet(L7_UNIT_CURRENT, &serverMaxCountValue) == L7_SUCCESS)
    {
    for (serverIndexValue = objUcastServerIndexValue + 1; serverIndexValue <= serverMaxCountValue; serverIndexValue++)
    {
        owa.l7rc = usmDbSntpServerTableRowStatusGet(L7_UNIT_CURRENT, serverIndexValue, &serverStatusValue);
     if (owa.l7rc == L7_SUCCESS) 
       {
          if(serverStatusValue == L7_SNTP_SERVER_STATUS_INVALID ||
         serverStatusValue == L7_SNTP_SERVER_STATUS_DESTROY)
          {
            continue;
           }
           else
        {
           nextObjUcastServerIndexValue = serverIndexValue;
           break;
        }
     }
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUcastServerIndexValue, owa.len);

  /* return the object value: UcastServerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUcastServerIndexValue,
                           sizeof (objUcastServerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerAddressType
*
* @purpose Get 'UcastServerAddressType'
*
* @description This object specifies how SntpClientUcastServerAddr is encoded. 
*              Support for all possible enumerations defined by InetAddressType 
*              is NOT REQUIRED. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerAddressType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerTableAddressTypeGet (L7_UNIT_CURRENT,
                                        keyUcastServerIndexValue,
                                        &objUcastServerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UcastServerAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerAddressTypeValue,
                           sizeof (objUcastServerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UcastServerAddressType
*
* @purpose Set 'UcastServerAddressType'
*
* @description This object specifies how SntpClientUcastServerAddr is encoded. 
*              Support for all possible enumerations defined by InetAddressType 
*              is NOT REQUIRED. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UcastServerAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerAddressTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UcastServerAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUcastServerAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerAddressTypeValue, owa.len);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpServerTableAddressTypeSet (L7_UNIT_CURRENT,
                                        keyUcastServerIndexValue,
                                        objUcastServerAddressTypeValue);
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
* @function fpObjGet_baseSntpClient_castServerAddress
*
* @purpose Get 'castServerAddress'
*
* @description The encoded internet address of an SNTP server. Unicast SNTP requests 
*              will be sent to this address. If this address is a DNS 
*              hostname, then that hostname SHOULD be resolved into an IP 
*              address each time a SNTP request is sent to it. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_castServerAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objcastServerAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerTableIpAddressGet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                      objcastServerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: castServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcastServerAddressValue,
                           strlen (objcastServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_castServerAddress
*
* @purpose Set 'castServerAddress'
*
* @description The encoded internet address of an SNTP server. Unicast SNTP requests 
*              will be sent to this address. If this address is a DNS 
*              hostname, then that hostname SHOULD be resolved into an IP 
*              address each time a SNTP request is sent to it. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_castServerAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objcastServerAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: castServerAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objcastServerAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcastServerAddressValue, owa.len);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpServerTableIpAddressSet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                      objcastServerAddressValue);
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
* @function fpObjGet_baseSntpClient_UcastServerPort
*
* @purpose Get 'UcastServerPort'
*
* @description The port number on the server to which poll requests are sent. 
*              A set request MUST NOT use a value of 0 for this object. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerPort (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerPortValue;
  xLibU16_t serverPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerTablePortGet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                  &serverPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objUcastServerPortValue = serverPortValue;

  /* return the object value: UcastServerPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerPortValue,
                           sizeof (objUcastServerPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UcastServerPort
*
* @purpose Set 'UcastServerPort'
*
* @description The port number on the server to which poll requests are sent. 
*              A set request MUST NOT use a value of 0 for this object. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UcastServerPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerPortValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UcastServerPort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUcastServerPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerPortValue, owa.len);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpServerTablePortSet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                 objUcastServerPortValue);
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
* @function fpObjGet_baseSntpClient_UcastServerVersion
*
* @purpose Get 'UcastServerVersion'
*
* @description The SNTP version this server supports. This is the value that 
*              will be encoded in NTP polls when operating in unicast(1) administrative 
*              mode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerVersion (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerVersionValue;
  xLibU16_t serverVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerTableVersionGet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                    &serverVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objUcastServerVersionValue = (xLibU32_t)serverVersionValue;

  /* return the object value: UcastServerVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerVersionValue,
                           sizeof (objUcastServerVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UcastServerVersion
*
* @purpose Set 'UcastServerVersion'
*
* @description The SNTP version this server supports. This is the value that 
*              will be encoded in NTP polls when operating in unicast(1) administrative 
*              mode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UcastServerVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerVersionValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UcastServerVersion */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUcastServerVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerVersionValue, owa.len);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpServerTableVersionSet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                    objUcastServerVersionValue);
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
* @function fpObjGet_baseSntpClient_UcastServerPrecedence
*
* @purpose Get 'UcastServerPrecedence'
*
* @description The precedence that this server has in relation to its peers in 
*              the determining the sequence of servers to which SNTP requests 
*              will be sent. The client continues sending requests to different 
*              servers until a successful response is received or all 
*              servers are exhausted. This object indicates the order in which 
*              to query the servers. A server entry with a precedence of 
*              1 will be queried before a server with a precedence of 2, and 
*              so forth. If more than one server has the same precedence 
*              then the request order will follow the lexicographical ordering 
*              of the entries in this table. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerPrecedence (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerPrecedenceValue;
  xLibU16_t serverPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerTablePriorityGet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                     &serverPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objUcastServerPrecedenceValue = (xLibU32_t)serverPrecedenceValue;

  /* return the object value: UcastServerPrecedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerPrecedenceValue,
                           sizeof (objUcastServerPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UcastServerPrecedence
*
* @purpose Set 'UcastServerPrecedence'
*
* @description The precedence that this server has in relation to its peers in 
*              the determining the sequence of servers to which SNTP requests 
*              will be sent. The client continues sending requests to different 
*              servers until a successful response is received or all 
*              servers are exhausted. This object indicates the order in which 
*              to query the servers. A server entry with a precedence of 
*              1 will be queried before a server with a precedence of 2, and 
*              so forth. If more than one server has the same precedence 
*              then the request order will follow the lexicographical ordering 
*              of the entries in this table. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UcastServerPrecedence (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerPrecedenceValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UcastServerPrecedence */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUcastServerPrecedenceValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerPrecedenceValue, owa.len);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpServerTablePrioritySet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                                     objUcastServerPrecedenceValue);
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
* @function fpObjGet_baseSntpClient_UcastServerLastUpdateTime
*
* @purpose Get 'UcastServerLastUpdateTime'
*
* @description The local date and time that the response from this server was 
*              used to update the system time on the device since agent reboot. 
*              If the SNTP client has not updated the time using a response 
*              from this server then this object MUST return '00000000'H. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerLastUpdateTime (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUcastServerLastUpdateTimeValue;
  FPOBJ_TRACE_ENTER (bufp);
#if 1
  xLibU32_t serverLastUpdateTimeValue;
  xLibStr256_t buf;
  xLibStr256_t stat;
#endif

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerStatsLastUpdateTimeGet (L7_UNIT_CURRENT,
                                           keyUcastServerIndexValue,
                                           &serverLastUpdateTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

#if 1
  memset (buf, 0,sizeof(buf));
  memset (objUcastServerLastUpdateTimeValue, 0,sizeof(objUcastServerLastUpdateTimeValue));
  osapiStrncpySafe(buf, usmDbConvertTimeToDateString(serverLastUpdateTimeValue),
                         ((21) < (sizeof(buf)-1) ? (21) : (sizeof(buf)-1)));
  osapiSnprintf(stat,sizeof(stat), buf);
  OSAPI_STRNCAT(objUcastServerLastUpdateTimeValue, stat);
#endif

  /* return the object value: UcastServerLastUpdateTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUcastServerLastUpdateTimeValue,
                           strlen (objUcastServerLastUpdateTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerLastAttemptTime
*
* @purpose Get 'UcastServerLastAttemptTime'
*
* @description The local date and time that this SNTP server was last queried 
*              since agent reboot. Essentially, this value is a timestamp for 
*              the SntpClientUcastServerLastAttemptStatus object. If this 
*              server has not been queried then this object MUST return '00000000'H. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerLastAttemptTime (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUcastServerLastAttemptTimeValue;
  FPOBJ_TRACE_ENTER (bufp);
#if 1
  xLibU32_t serverLastAttemptTimeValue;
  xLibStr256_t buf;
  xLibStr256_t stat;
#endif

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerStatsLastAttemptTimeGet (L7_UNIT_CURRENT,
                                            keyUcastServerIndexValue,
                                            &serverLastAttemptTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

#if 1
  memset (buf, 0,sizeof(buf));
  memset (objUcastServerLastAttemptTimeValue, 0,sizeof(objUcastServerLastAttemptTimeValue));
  osapiStrncpySafe(buf, usmDbConvertTimeToDateString(serverLastAttemptTimeValue),
                         ((21) < (sizeof(buf)-1) ? (21) : (sizeof(buf)-1)));
  osapiSnprintf(stat,sizeof(stat), buf);
  OSAPI_STRNCAT(objUcastServerLastAttemptTimeValue, stat);
#endif


  /* return the object value: UcastServerLastAttemptTime */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objUcastServerLastAttemptTimeValue,
                    strlen (objUcastServerLastAttemptTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerLastAttemptStatus
*
* @purpose Get 'UcastServerLastAttemptStatus'
*
* @description The status of the last SNTP request to this server since agent 
*              reboot. If no requests have been made then this object should 
*              return 'other'. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerLastAttemptStatus (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerLastAttemptStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerStatsLastUpdateStatusGet (L7_UNIT_CURRENT,
                                             keyUcastServerIndexValue,
                                             &objUcastServerLastAttemptStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UcastServerLastAttemptStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerLastAttemptStatusValue,
                    sizeof (objUcastServerLastAttemptStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerNumRequests
*
* @purpose Get 'UcastServerNumRequests'
*
* @description The number of SNTP requests made to this server since the last 
*              agent reboot. This includes retry attempts to the server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerNumRequests (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerNumRequestsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerStatsUnicastServerNumRequestsGet (L7_UNIT_CURRENT,
                                                     keyUcastServerIndexValue,
                                                     &objUcastServerNumRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UcastServerNumRequests */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerNumRequestsValue,
                           sizeof (objUcastServerNumRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerNumFailedRequests
*
* @purpose Get 'UcastServerNumFailedRequests'
*
* @description The number of SNTP requests made to this server that did not result 
*              in a successful response since the last agent reboot. This 
*              includes retry attempts to the server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerNumFailedRequests (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerNumFailedRequestsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerStatsUnicastServerNumFailedRequestsGet (L7_UNIT_CURRENT,
                                                           keyUcastServerIndexValue,
                                                           &objUcastServerNumFailedRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UcastServerNumFailedRequests */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerNumFailedRequestsValue,
                    sizeof (objUcastServerNumFailedRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerRowStatus
*
* @purpose Get 'UcastServerRowStatus'
*
* @description [UcastServerRowStatus]: The row status of this conceptual row
*              in the table. active - The server is available for use in
*              SNTP client operations. Other writable leaves in this table
*              MAY be modified while the row is in the active state. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerRowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSntpServerTableRowStatusGet (L7_UNIT_CURRENT, keyUcastServerIndexValue,
                              &objUcastServerRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UcastServerRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerRowStatusValue,
                           sizeof (objUcastServerRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UcastServerRowStatus
*
* @purpose Set 'UcastServerRowStatus'
*
* @description [UcastServerRowStatus]: The row status of this conceptual row
*              in the table. active - The server is available for use in
*              SNTP client operations. Other writable leaves in this table
*              MAY be modified while the row is in the active state. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UcastServerRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerRowStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUcastServerIndexValue;

  fpObjWa_t owaServerAddressValue = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objcastServerAddressValue;

  fpObjWa_t owaServerAddressTypeValue = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objcastServerAddressTypeValue;

  L7_uint32  ipAddr = L7_NULL;
  L7_uint32 new_ndx;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_SNTP_ADDRESS_TYPE_t  type;
  
  FPOBJ_TRACE_ENTER (bufp);  

  /* retrieve object: UcastServerRowStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUcastServerRowStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerRowStatusValue, owa.len);


  if(objUcastServerRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
       memset(objcastServerAddressValue, 0x00, sizeof(objcastServerAddressValue));
  memset(&objcastServerAddressTypeValue, 0x00, sizeof(objcastServerAddressTypeValue));

  /* Get IP Address */

  /* retrieve object: castServerAddress */
  owaServerAddressValue.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_castServerAddress,
                          (xLibU8_t *) objcastServerAddressValue, &owaServerAddressValue.len);
  if (owaServerAddressValue.rc != XLIBRC_SUCCESS)
  {
    owaServerAddressValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owaServerAddressValue);
    return owaServerAddressValue.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, objcastServerAddressValue, owaServerAddressValue.len);

  /* Get IP Address type*/
  /* retrieve object: castServerAddressType */
  owaServerAddressTypeValue.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerAddressType,
                          (xLibU8_t *) &objcastServerAddressTypeValue, &owaServerAddressTypeValue.len);
  if (owaServerAddressTypeValue.rc != XLIBRC_SUCCESS)
  {
    owaServerAddressTypeValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owaServerAddressTypeValue);
    return owaServerAddressTypeValue.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, objcastServerAddressTypeValue, owaServerAddressTypeValue.len);


    owa.l7rc = usmDbIPHostAddressValidate(objcastServerAddressValue, 
                                   &ipAddr, &addrType);
  if( (owa.l7rc != L7_SUCCESS) || ( addrType != objcastServerAddressTypeValue ) )
  {
     /*Validation of Host address */
     owa.rc = XLIBRC_INVALID_IPV4_DNS_ADDRESS;  /* TODO: add error string */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  
     owa.l7rc = usmDbSntpServerAdd(L7_UNIT_CURRENT, objcastServerAddressValue, objcastServerAddressTypeValue, &new_ndx);
     if (owa.l7rc == L7_ERROR)
     {
         /*invalid address */
        owa.rc = XLIBRC_FAILURE;  /* TODO: add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
     else if (owa.l7rc  == L7_ALREADY_CONFIGURED)
     {
         /*address already configured */
        owa.rc = XLIBRC_ALREADY_EXISTS;  /* TODO: add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
     else if (owa.l7rc  == L7_TABLE_IS_FULL)
     {
  /*table full error */
        owa.rc = XLIBRC_TABLE_IS_FULL;  /* TODO: add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
  }

  /* push key into filter */
  xLibFilterSet(wap,XOBJ_baseSntpClient_UcastServerIndex,0,
                       (xLibU8_t *) &new_ndx,
                       sizeof(new_ndx));
  
  }
  else if(objUcastServerRowStatusValue ==L7_ROW_STATUS_DESTROY)
  {

     /* retrieve key: UcastServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, kwa.len);

  /* Get the ip Address and addr type at given index */
    memset(objcastServerAddressValue, 0x00, sizeof(owaServerAddressValue));
  owa.l7rc = usmDbSntpServerTableIpAddressGet(L7_UNIT_CURRENT, 
                             keyUcastServerIndexValue, objcastServerAddressValue);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
  }

  owa.l7rc = usmDbSntpServerTableAddressTypeGet(L7_UNIT_CURRENT,
                        keyUcastServerIndexValue, &type);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
  }
  owa.l7rc =usmDbSntpServerDelete(L7_UNIT_CURRENT, objcastServerAddressValue, type);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
  }
  
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_ServerStratum
*
* @purpose Get 'ServerStratum'
*
* @description This is an 8-bit integer identifying the stratum of the server 
*              as indicated in the last received packet with values defined 
*              as follows: Stratum Meaning ------------------------------------ 
*              0 unspecified 1 primary reference 2-15 secondary reference 
*              16-255 reserved 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_ServerStratum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objServerStratumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerStratumGet (L7_UNIT_CURRENT, &objServerStratumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objServerStratumValue,
                     sizeof (objServerStratumValue));

  /* return the object value: ServerStratum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServerStratumValue,
                           sizeof (objServerStratumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UnicastPollTimeout
*
* @purpose Get 'UnicastPollTimeout'
*
* @description The number of seconds to wait for a response from a SNTP server 
*              before considering the attempt to have 'timed out'. This timeout 
*              is used for SNTP requests in unicast(1) administrative 
*              mode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UnicastPollTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastPollTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpClientUnicastPollTimeoutGet (L7_UNIT_CURRENT,
                                          &objUnicastPollTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastPollTimeoutValue,
                     sizeof (objUnicastPollTimeoutValue));

  /* return the object value: UnicastPollTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastPollTimeoutValue,
                           sizeof (objUnicastPollTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UnicastPollTimeout
*
* @purpose Set 'UnicastPollTimeout'
*
* @description The number of seconds to wait for a response from a SNTP server 
*              before considering the attempt to have 'timed out'. This timeout 
*              is used for SNTP requests in unicast(1) administrative 
*              mode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UnicastPollTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastPollTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastPollTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnicastPollTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastPollTimeoutValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpClientUnicastPollTimeoutSet (L7_UNIT_CURRENT,
                                          objUnicastPollTimeoutValue);
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
* @function fpObjGet_baseSntpClient_UnicastPollRetry
*
* @purpose Get 'UnicastPollRetry'
*
* @description The number of times to retry a request to the same SNTP server 
*              that has 'timed out.'. This retry count is used for directed 
*              SNTP requests in unicast(1) administrative mode. For example, 
*              assume this object has been SET to a value of 2. When the SNTP 
*              client queries a given server it will send 1 SNTP request 
*              frame. If that original attempt fails, the client will retry 
*              up to a maximum of 2 more times before declaring the unicast 
*              poll unsuccessful and attempting the next server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UnicastPollRetry (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastPollRetryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpClientUnicastPollRetryGet (L7_UNIT_CURRENT,
                                        &objUnicastPollRetryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastPollRetryValue,
                     sizeof (objUnicastPollRetryValue));

  /* return the object value: UnicastPollRetry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastPollRetryValue,
                           sizeof (objUnicastPollRetryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UnicastPollRetry
*
* @purpose Set 'UnicastPollRetry'
*
* @description The number of times to retry a request to the same SNTP server 
*              that has 'timed out.'. This retry count is used for directed 
*              SNTP requests in unicast(1) administrative mode. For example, 
*              assume this object has been SET to a value of 2. When the SNTP 
*              client queries a given server it will send 1 SNTP request 
*              frame. If that original attempt fails, the client will retry 
*              up to a maximum of 2 more times before declaring the unicast 
*              poll unsuccessful and attempting the next server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UnicastPollRetry (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastPollRetryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastPollRetry */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnicastPollRetryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastPollRetryValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpClientUnicastPollRetrySet (L7_UNIT_CURRENT,
                                        objUnicastPollRetryValue);
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
* @function fpObjGet_baseSntpClient_Port
*
* @purpose Get 'Port'
*
* @description The local port number used to listen for broadcasts and responses 
*              from servers. A set request MUST NOT use a value of 0 for 
*              this object. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_Port (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU16_t objPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSntpClientPortGet (L7_UNIT_CURRENT, (xLibU16_t *)&objPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortValue, sizeof (objPortValue));

  /* return the object value: Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortValue,
                           sizeof (objPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_Port
*
* @purpose Set 'Port'
*
* @description The local port number used to listen for broadcasts and responses 
*              from servers. A set request MUST NOT use a value of 0 for 
*              this object. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_Port (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Port */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSntpClientPortSet (L7_UNIT_CURRENT, objPortValue);
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
* @function fpObjGet_baseSntpClient_LastAttemptStatus
*
* @purpose Get 'LastAttemptStatus'
*
* @description The status of the last SNTP request or unsolicited SNTP message 
*              for this SNTP client since agent reboot. The status is updated 
*              for all non-disabled administrative modes of the SNTP client. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_LastAttemptStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLastAttemptStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpLastAttemptStatusGet (L7_UNIT_CURRENT, &objLastAttemptStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLastAttemptStatusValue,
                     sizeof (objLastAttemptStatusValue));

  /* return the object value: LastAttemptStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLastAttemptStatusValue,
                           sizeof (objLastAttemptStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_ServerMode
*
* @purpose Get 'ServerMode'
*
* @description This is a 3-bit integer identifying the mode of the server as 
*              indicated in the last received packet with values defined as 
*              follows: Mode Meaning ------------------------------------ 0 
*              reserved 1 symmetric active 2 symmetric passive 3 client 4 server 
*              5 broadcast 6 reserved for NTP control message 7 reserved 
*              for private use 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_ServerMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objServerModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSntpServerModeGet (L7_UNIT_CURRENT, &objServerModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objServerModeValue, sizeof (objServerModeValue));

  /* return the object value: ServerMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServerModeValue,
                           sizeof (objServerModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_LastAttemptTime
*
* @purpose Get 'LastAttemptTime'
*
* @description The local date and time of the last SNTP request or unsolicited 
*              SNTP message for this SNTP client since agent reboot. This 
*              value is a timestamp for the SntpClientLastAttemptStatus object. 
*              When the SntpClientLastAttemptStatus has a value of success(2), 
*              this object's value should be equal to the value returned 
*              by SntpClientLastUpdateTime. If no SNTP frames have been 
*              processed by the SNTP client then the client MUST return '00000000'H. 
*              This object is updated for all non-disabled administrative 
*              modes of the SNTP client. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_LastAttemptTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLastAttemptTimeValue;
  xLibU32_t clientLastAttemptTimeValue;
  xLibStr256_t buf;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpLastAttemptTimeGet (L7_UNIT_CURRENT, &clientLastAttemptTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &clientLastAttemptTimeValue,
                     sizeof (clientLastAttemptTimeValue));

  memset (buf, 0,sizeof(buf));
  memset (objLastAttemptTimeValue, 0,sizeof(objLastAttemptTimeValue));
  osapiStrncpySafe(buf, usmDbConvertTimeToDateString(clientLastAttemptTimeValue),
                         ((21) < (sizeof(buf)-1) ? (21) : (sizeof(buf)-1)));
  OSAPI_STRNCAT(objLastAttemptTimeValue, buf);

  /* return the object value: LastAttemptTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLastAttemptTimeValue,
                           strlen (objLastAttemptTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UcastServerCurrEntries
*
* @purpose Get 'UcastServerCurrEntries'
*
* @description The current number of server entries in the SntpClientUcastServerTable. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerCurrEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerCurrEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerCurrentEntriesGet (L7_UNIT_CURRENT,
                                      &objUcastServerCurrEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerCurrEntriesValue,
                     sizeof (objUcastServerCurrEntriesValue));

  /* return the object value: UcastServerCurrEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerCurrEntriesValue,
                           sizeof (objUcastServerCurrEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_ServerRefClkId
*
* @purpose Get 'ServerRefClkId'
*
* @description This is the value of the Reference Identifier in the last received 
*              packet defined as follows. Reference Identifier: This is 
*              a 32-bit bitstring identifying the particular reference source. 
*              In the case of NTP Version 3 or Version 4 stratum-0 (unspecified) 
*              or stratum-1 (primary) servers, this is a four-character 
*              ASCII string, left justified and zero padded to 32 bits. 
*              In NTP Version 3 secondary servers, this is the 32-bit IPv4 
*              address of the reference source. In NTP Version 4 secondary servers, 
*              this is the low order 32 bits of the latest transmit 
*              timestamp of the reference source. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_ServerRefClkId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objServerRefClkIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSntpServerRefIdGet (L7_UNIT_CURRENT, objServerRefClkIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objServerRefClkIdValue,
                     strlen (objServerRefClkIdValue));

  /* return the object value: ServerRefClkId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objServerRefClkIdValue,
                           strlen (objServerRefClkIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_ServerAddressType
*
* @purpose Get 'ServerAddressType'
*
* @description The address type of the SNTP server as identified by the last 
*              received packet. Support for all address types is NOT REQUIRED. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_ServerAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerAddressTypeGet (L7_UNIT_CURRENT, &objServerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objServerAddressTypeValue,
                     sizeof (objServerAddressTypeValue));

  /* return the object value: ServerAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServerAddressTypeValue,
                           sizeof (objServerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_LastUpdateTime
*
* @purpose Get 'LastUpdateTime'
*
* @description The local date and time that the SNTP client last updated the 
*              system time on the device since agent reboot. This time is updated 
*              for all non-disabled administrative modes of the SNTP client. 
*              If the SNTP client has not updated the time then the client 
*              MUST return '00000000'H. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_LastUpdateTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLastUpdateTimeValue;
  xLibU32_t clientLastUpdateTimeValue;
  xLibStr256_t buf;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpLastUpdateTimeGet (L7_UNIT_CURRENT, &clientLastUpdateTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &clientLastUpdateTimeValue,
                     sizeof (clientLastUpdateTimeValue));

  memset (buf, 0,sizeof(buf));
  memset (objLastUpdateTimeValue, 0,sizeof(objLastUpdateTimeValue));
  osapiStrncpySafe(buf, usmDbConvertTimeToDateString(clientLastUpdateTimeValue),
                         ((21) < (sizeof(buf)-1) ? (21) : (sizeof(buf)-1)));
  OSAPI_STRNCAT(objLastUpdateTimeValue, buf);

  /* return the object value: LastUpdateTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLastUpdateTimeValue,
                           strlen (objLastUpdateTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_UnicastPollInterval
*
* @purpose Get 'UnicastPollInterval'
*
* @description The minimum number of seconds between successive SNTP polls of 
*              the server in seconds as a power of two. This polling interval 
*              is used for SNTP requests in unicast(1) administrative mode. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UnicastPollInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastPollIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpClientUnicastPollIntervalGet (L7_UNIT_CURRENT,
                                           &objUnicastPollIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastPollIntervalValue,
                     sizeof (objUnicastPollIntervalValue));

  /* return the object value: UnicastPollInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastPollIntervalValue,
                           sizeof (objUnicastPollIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_UnicastPollInterval
*
* @purpose Set 'UnicastPollInterval'
*
* @description The minimum number of seconds between successive SNTP polls of 
*              the server in seconds as a power of two. This polling interval 
*              is used for SNTP requests in unicast(1) administrative mode. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_UnicastPollInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastPollIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastPollInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnicastPollIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastPollIntervalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpClientUnicastPollIntervalSet (L7_UNIT_CURRENT,
                                           objUnicastPollIntervalValue);
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
* @function fpObjGet_baseSntpClient_Version
*
* @purpose Get 'Version'
*
* @description The highest SNTP version this client supports. Per RFC 2030, higher 
*              versions are required to be backwards compatible with all 
*              lower versions with the exception of version 0. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_Version (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSntpClientVersionGet (L7_UNIT_CURRENT, &objVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVersionValue, sizeof (objVersionValue));

  /* return the object value: Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVersionValue,
                           sizeof (objVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_Mode
*
* @purpose Get 'Mode'
*
* @description The current administrative mode of the SNTP client. A SET of this 
*              object will cause the SNTP client to change administrative 
*              modes. A SET request MUST have only 1 bit set since is not 
*              possible to operate in multiple modes simultaneously. SETs of 
*              this object are limited to values supported by the device as 
*              specified by SntpClientSupportedMode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSntpClientModeGet (L7_UNIT_CURRENT, &objModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objModeValue, sizeof (objModeValue));

  /* return the object value: Mode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objModeValue,
                           sizeof (objModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_Mode
*
* @purpose Set 'Mode'
*
* @description The current administrative mode of the SNTP client. A SET of this 
*              object will cause the SNTP client to change administrative 
*              modes. A SET request MUST have only 1 bit set since is not 
*              possible to operate in multiple modes simultaneously. SETs of 
*              this object are limited to values supported by the device as 
*              specified by SntpClientSupportedMode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Mode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSntpClientModeSet (L7_UNIT_CURRENT, objModeValue);
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
* @function fpObjGet_baseSntpClient_ServerAddress
*
* @purpose Get 'ServerAddress'
*
* @description The encoded address of the SNTP server as identified by the last 
*              received packet. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_ServerAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objServerAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerIpAddressGet (L7_UNIT_CURRENT, objServerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objServerAddressValue,
                     strlen (objServerAddressValue));

  /* return the object value: ServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objServerAddressValue,
                           strlen (objServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_BroadcastCount
*
* @purpose Get 'BroadcastCount'
*
* @description The number of unsolicited broadcast SNTP messages that have been 
*              received and processed by the SNTP client. Unsolicited SNTP 
*              broadcast frames will not be counted unless the SNTP agent 
*              is operating in broadcast(3) mode, as specified by SntpClientMode. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_BroadcastCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpBroadcastCountGet (L7_UNIT_CURRENT, &objBroadcastCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastCountValue,
                     sizeof (objBroadcastCountValue));

  /* return the object value: BroadcastCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastCountValue,
                           sizeof (objBroadcastCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_BroadcastInterval
*
* @purpose Get 'BroadcastInterval'
*
* @description The number of seconds the client will wait before processing another 
*              broadcast packet expressed as a power of two. Packets 
*              received during the wait interval are silently discarded. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_BroadcastInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpClientBroadcastPollIntervalGet (L7_UNIT_CURRENT,
                                             &objBroadcastIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastIntervalValue,
                     sizeof (objBroadcastIntervalValue));

  /* return the object value: BroadcastInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastIntervalValue,
                           sizeof (objBroadcastIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSntpClient_BroadcastInterval
*
* @purpose Set 'BroadcastInterval'
*
* @description The number of seconds the client will wait before processing another 
*              broadcast packet expressed as a power of two. Packets 
*              received during the wait interval are silently discarded. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_BroadcastInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objBroadcastIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastIntervalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSntpClientBroadcastPollIntervalSet (L7_UNIT_CURRENT,
                                             objBroadcastIntervalValue);
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
* @function fpObjGet_baseSntpClient_UcastServerMaxEntries
*
* @purpose Get 'UcastServerMaxEntries'
*
* @description The maximum number of server entries that are allowed in the SntpClientUcastServerTable. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_UcastServerMaxEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUcastServerMaxEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpServerMaxEntriesGet (L7_UNIT_CURRENT,
                                  &objUcastServerMaxEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUcastServerMaxEntriesValue,
                     sizeof (objUcastServerMaxEntriesValue));

  /* return the object value: UcastServerMaxEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUcastServerMaxEntriesValue,
                           sizeof (objUcastServerMaxEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_SupportedMode
*
* @purpose Get 'SupportedMode'
*
* @description The SNTP client administrative modes that this device supports. 
*              A client may support more than one administrative mode. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_SupportedMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSupportedModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSntpClientSupportedModeGet (L7_UNIT_CURRENT, &objSupportedModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSupportedModeValue,
                     sizeof (objSupportedModeValue));

/* return the object value: SupportedMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSupportedModeValue,
                           sizeof (objSupportedModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthMode
*
* @purpose Get 'sntpAuthMode'
 *@description  [sntpAuthMode] Get the current authentication mode for this
* client   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSntpClientAuthenticateModeGet(&objsntpAuthModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthModeValue, sizeof (objsntpAuthModeValue));

  /* return the object value: sntpAuthMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsntpAuthModeValue,
                           sizeof (objsntpAuthModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthMode
*
* @purpose Set 'sntpAuthMode'
 *@description  [sntpAuthMode] Get the current authentication mode for this
* client   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sntpAuthMode */
  owa.len = sizeof (objsntpAuthModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsntpAuthModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (  objsntpAuthModeValue == L7_ENABLE )
  {
      owa.l7rc = usmDbSntpClientAuthenticateModeSet(objsntpAuthModeValue);
      if (owa.l7rc != L7_SUCCESS)
     {
         owa.rc = XLIBRC_ERROR_AUTHENTICATE;
     }
  }
  else
  {
      owa.l7rc = usmDbSntpClientAuthenticateModeSet(objsntpAuthModeValue);
      if (owa.l7rc != L7_SUCCESS)
     {
         owa.rc = XLIBRC_ERROR_NO_AUTHENTICATE;
     }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthKeyIndex
*
* @purpose Get 'sntpAuthKeyIndex'
 *@description  [sntpAuthKeyIndex] Authentication Key Index Used for Auth Key
* Generation   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthKeyIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthKeyIndexValue;
  xLibU32_t objsntpAuthKeyStatusValue;
  xLibU32_t nextObjsntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (objsntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & objsntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objsntpAuthKeyIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objsntpAuthKeyIndexValue, owa.len);
  }
  owa.l7rc = usmDbSntpAuthenticateTableRowStatusGetNext(objsntpAuthKeyIndexValue,
                                                 &objsntpAuthKeyStatusValue,&nextObjsntpAuthKeyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjsntpAuthKeyIndexValue, owa.len);

  /* return the object value: sntpAuthKeyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjsntpAuthKeyIndexValue,
                           sizeof (nextObjsntpAuthKeyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthKeyNumber
*
* @purpose Get 'sntpAuthKeyNumber'
 *@description  [sntpAuthKeyNumber] The key number of the authenticate table
* entry specified by index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthKeyNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthKeyNumberValue;

  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSntpAuthenticateTableKeyNumberGet (keysntpAuthKeyIndexValue,
                              &objsntpAuthKeyNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthKeyNumberValue, sizeof (objsntpAuthKeyNumberValue));

  /* return the object value: sntpAuthKeyNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsntpAuthKeyNumberValue,
                           sizeof (objsntpAuthKeyNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthKeyNumber
*
* @purpose Set 'sntpAuthKeyNumber'
 *@description  [sntpAuthKeyNumber] The key number of the authenticate table
* entry specified by index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthKeyNumber (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthKeyValue
*
* @purpose Get 'sntpAuthKeyValue'
 *@description  [sntpAuthKeyValue] The key value of the authenticate table entry
* specified by index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthKeyValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsntpAuthKeyValueValue;

  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSntpAuthenticateTableKeyValueGet (keysntpAuthKeyIndexValue, objsntpAuthKeyValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsntpAuthKeyValueValue, strlen (objsntpAuthKeyValueValue));

  /* return the object value: sntpAuthKeyValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsntpAuthKeyValueValue,
                           strlen (objsntpAuthKeyValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthKeyValue
*
* @purpose Set 'sntpAuthKeyValue'
 *@description  [sntpAuthKeyValue] The key value of the authenticate table entry
* specified by index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthKeyValue (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsntpAuthKeyValueValue;

  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sntpAuthKeyValue */
  owa.len = sizeof (objsntpAuthKeyValueValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objsntpAuthKeyValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsntpAuthKeyValueValue, owa.len);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSntpAuthenticateTableKeyValueSet (keysntpAuthKeyIndexValue, objsntpAuthKeyValueValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthKeyTrustMode
*
* @purpose Get 'sntpAuthKeyTrustMode'
 *@description  [sntpAuthKeyTrustMode] The Trusted Key Mode option for the
* authenticated Key   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthKeyTrustMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthKeyTrustModeValue;

  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSntpAuthenticateTableKeyTrustedGet( keysntpAuthKeyIndexValue,
                              &objsntpAuthKeyTrustModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthKeyTrustModeValue, sizeof (objsntpAuthKeyTrustModeValue));

  /* return the object value: sntpAuthKeyTrustMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsntpAuthKeyTrustModeValue,
                           sizeof (objsntpAuthKeyTrustModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthKeyTrustMode
*
* @purpose Set 'sntpAuthKeyTrustMode'
 *@description  [sntpAuthKeyTrustMode] The Trusted Key Mode option for the
* authenticated Key   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthKeyTrustMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthKeyTrustModeValue;

  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sntpAuthKeyTrustMode */
  owa.len = sizeof (objsntpAuthKeyTrustModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsntpAuthKeyTrustModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthKeyTrustModeValue, owa.len);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSntpAuthenticateTableKeyTrustedSet (keysntpAuthKeyIndexValue,
                              objsntpAuthKeyTrustModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthRowStatus
*
* @purpose Get 'sntpAuthRowStatus'
 *@description  [sntpAuthRowStatus] Row Status object for SNTP Auth Server   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthRowStatusValue;

  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSntpAuthenticateTableRowStatusGet (keysntpAuthKeyIndexValue,
                              &objsntpAuthRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthRowStatusValue, sizeof (objsntpAuthRowStatusValue));

  /* return the object value: sntpAuthRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsntpAuthRowStatusValue,
                           sizeof (objsntpAuthRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthRowStatus
*
* @purpose Set 'sntpAuthRowStatus'
 *@description  [sntpAuthRowStatus] Row Status object for SNTP Auth Server   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  fpObjWa_t owakeyobjsntpAuthKeyValueValue = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthRowStatusValue;

  xLibU32_t objsntpIndexValue;
  xLibU32_t keysntpAuthKeyNumber;
  xLibStr256_t keyobjsntpAuthKeyValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sntpAuthRowStatus */
  owa.len = sizeof (objsntpAuthRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsntpAuthRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthRowStatusValue, owa.len);

  /* retrieve key: sntpAuthKeyValue */
  owakeyobjsntpAuthKeyValueValue.len = sizeof (keyobjsntpAuthKeyValue);
  owakeyobjsntpAuthKeyValueValue.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyValue,
                          (xLibU8_t *) keyobjsntpAuthKeyValue, &owakeyobjsntpAuthKeyValueValue.len);
  if (owakeyobjsntpAuthKeyValueValue.rc != XLIBRC_SUCCESS)
  {
    owakeyobjsntpAuthKeyValueValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owakeyobjsntpAuthKeyValueValue);
    return owakeyobjsntpAuthKeyValueValue.rc;
  }

  if (objsntpAuthRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Get the AuthKeyNumber also which is needed for the add routine */
     
    /* retrieve key: sntpAuthKeyNumber */
    owa.len = sizeof (keysntpAuthKeyNumber);
    owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyNumber,
                          (xLibU8_t *) & keysntpAuthKeyNumber, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
   
     owa.l7rc = usmDbSntpAuthenticateTableKeyAdd (keysntpAuthKeyNumber, keyobjsntpAuthKeyValue,
                                &objsntpIndexValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

    /* Push the new Index Value Content into the key */
     owa.len = sizeof (keysntpAuthKeyNumber);  
     owa.rc = xLibFilterSet(wap,XOBJ_baseSntpClient_sntpAuthKeyIndex,0,
                   (xLibU8_t *) &objsntpIndexValue,owa.len);
  }
  else if (objsntpAuthRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* retrieve key: sntpAuthKeyIndex */
    owa.len = sizeof (keysntpAuthKeyNumber);
    owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & objsntpIndexValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  
    owa.l7rc = usmDbSntpAuthenticateTableKeyNumberGet(objsntpIndexValue,&keysntpAuthKeyNumber);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
   
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyNumber, owa.len);
    owa.l7rc = usmDbSntpAuthenticateTableKeyDelete (keysntpAuthKeyNumber);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);

  /* Set key: keysntpAuthKeyNumber */
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthEncryptionKey
*
* @purpose Get 'sntpAuthEncryptionKey'
 *@description  [sntpAuthEncryptionKey] The authentication key for an SNTP
* server table entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthEncryptionKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthEncryptionKeyValue;

  xLibU32_t keyUcastServerIndexValue;
  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  owa.len = sizeof (keyUcastServerIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, owa.len);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);
  
  /* get the value from application */
  owa.l7rc = usmDbSntpServerTableAuthenticateKeyGet (keyUcastServerIndexValue,&objsntpAuthEncryptionKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthEncryptionKeyValue, sizeof (objsntpAuthEncryptionKeyValue));

  /* return the object value: sntpAuthEncryptionKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsntpAuthEncryptionKeyValue,
                           sizeof (objsntpAuthEncryptionKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthEncryptionKey
*
* @purpose Set 'sntpAuthEncryptionKey'
 *@description  [sntpAuthEncryptionKey] The authentication key for an SNTP
* server table entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthEncryptionKey (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthEncryptionKeyValue;
  xLibU32_t objsntpAuthEncryptionKeyNumberValue;

  xLibU32_t keyUcastServerIndexValue;
  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sntpAuthEncryptionKey */
  owa.len = sizeof (objsntpAuthEncryptionKeyValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsntpAuthEncryptionKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthEncryptionKeyValue, owa.len);

  /* retrieve key: UcastServerIndex */
  owa.len = sizeof (keyUcastServerIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, owa.len);

  /* retrieve key: sntpAuthKeyIndex */
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthEncryptionKeyNumber,
                          (xLibU8_t *) & objsntpAuthEncryptionKeyNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);
   /* Extract the value associated with the key */
  owa.l7rc = usmDbSntpAuthenticateTableIndexGet(objsntpAuthEncryptionKeyNumberValue,
                                          &keysntpAuthKeyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbSntpServerTableAuthenticateKeySet (keyUcastServerIndexValue,keysntpAuthKeyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthType
*
* @purpose Get 'sntpAuthType'
 *@description  [sntpAuthType] The Authentication type of the given entry
* specified by index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsntpAuthTypeValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* hard code the value for now as becuase there is no specific USMDB implemented 
   * for this. This need to be replaced with appropriate USMDB down the line
   */
  memset(objsntpAuthTypeValue,0x00,sizeof(objsntpAuthTypeValue));
  strncpy(objsntpAuthTypeValue,"MD5",strlen("MD5"));
  owa.l7rc = L7_SUCCESS;
  /* get the value from application */

  FPOBJ_TRACE_VALUE (bufp, objsntpAuthTypeValue, strlen (objsntpAuthTypeValue));

  /* return the object value: sntpAuthKeyValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsntpAuthTypeValue,
                           strlen (objsntpAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthEncryptionKeyNumber
*
* @purpose Get 'sntpAuthEncryptionKeyNumber'
 *@description  [sntpAuthEncryptionKeyNumber] The authentication key for an SNTP
* server table entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthEncryptionKeyNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthEncryptionKeyNumber;

  xLibU32_t keyUcastServerIndexValue;
  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  owa.len = sizeof (keyUcastServerIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, owa.len);
/* This is not needed for now */
  /* retrieve key: sntpAuthKeyIndex 
  owa.len = sizeof (keysntpAuthKeyIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthKeyIndex,
                          (xLibU8_t *) & keysntpAuthKeyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);
  */ 
  /* get the value from application */
  owa.l7rc = usmDbSntpServerTableAuthenticateKeyGet (keyUcastServerIndexValue,&keysntpAuthKeyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc =  usmDbSntpAuthenticateTableKeyNumberGet (keysntpAuthKeyIndexValue,
                              &objsntpAuthEncryptionKeyNumber);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsntpAuthEncryptionKeyNumber, sizeof (objsntpAuthEncryptionKeyNumber));

  /* return the object value: sntpAuthEncryptionKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsntpAuthEncryptionKeyNumber,
                           sizeof (objsntpAuthEncryptionKeyNumber));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjList_baseSntpClient_sntpAuthEncryptionKeyNumber
*
* @purpose List 'sntpAuthEncryptionKeyNumber'
 *@description  [sntpAuthEncryptionKeyNumber] The authentication key for an SNTP
* server table entry   
* @notes       
*
* @return
*******************************************************************************/
/* This list function is a bit tricky to be in place. 
 * The way its being implemented is......
 * First time when this is loaded,filter returns error and hence this objects 
 * content is filled by getting the first existing valied key object and its 
 * corresponding value. Sub sequent calls of the key is extracted with a kind of
 * reverse mapping function and then the next key is extracted from it
 */
xLibRC_t fpObjList_baseSntpClient_sntpAuthEncryptionKeyNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsntpAuthEncryptionKeyNumberValue;
  xLibU32_t nextObjsntpAuthEncryptionKeyNumberValue;
  xLibU32_t objsntpAuthKeyStatusValue;
  xLibU32_t objsntpAuthKeyIndexValue;
  xLibU32_t nextobjsntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objsntpAuthEncryptionKeyNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_sntpAuthEncryptionKeyNumber,
                          (xLibU8_t *) & objsntpAuthEncryptionKeyNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    
    objsntpAuthKeyIndexValue = 0;
    nextobjsntpAuthKeyIndexValue = 0;
    /* Get the key first in this case AuthKeyIndex is key */
    owa.l7rc = usmDbSntpAuthenticateTableRowStatusGetNext (objsntpAuthKeyIndexValue,
                                                           &objsntpAuthKeyStatusValue,&nextobjsntpAuthKeyIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objsntpAuthEncryptionKeyNumberValue, owa.len);
    /* With the value present in the filter first try to do a reverse mapping
     * and get the key associated with this object. Later get the next key and then
     * get the value associated with that key 
     */
    owa.l7rc = usmDbSntpAuthenticateTableIndexGet(objsntpAuthEncryptionKeyNumberValue,
                                         &objsntpAuthKeyIndexValue);  
    {
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    owa.l7rc = usmDbSntpAuthenticateTableRowStatusGetNext (objsntpAuthKeyIndexValue,
                                                           &objsntpAuthKeyStatusValue,&nextobjsntpAuthKeyIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* Extract the value associated with the key */
  owa.l7rc = usmDbSntpAuthenticateTableKeyNumberGet(nextobjsntpAuthKeyIndexValue,
                                          &nextObjsntpAuthEncryptionKeyNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjsntpAuthEncryptionKeyNumberValue, owa.len);

  /* return the object value: sntpAuthEncryptionKeyNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjsntpAuthEncryptionKeyNumberValue,
                           sizeof (nextObjsntpAuthEncryptionKeyNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_baseSntpClient_sntpAuthEncryptionKeyNumber
*
* @purpose List 'sntpAuthEncryptionKeyNumber'
 *@description  [sntpAuthEncryptionKeyNumber] The authentication key for an SNTP
* server table entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSntpClient_sntpAuthEncryptionKeyNumber (void *wap, void *bufp)
{
  return  XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseSntpClient_sntpAuthEncryptionKeyNumberBind
*
* @purpose Get 'sntpAuthEncryptionKeyNumberBind'
 *@description  [sntpAuthEncryptionKeyNumberBind] The authentication key for an SNTP
* server table entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSntpClient_sntpAuthEncryptionKeyNumberBind (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t keyUcastServerIndexValue;
  xLibU32_t keysntpAuthKeyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UcastServerIndex */
  owa.len = sizeof (keyUcastServerIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSntpClient_UcastServerIndex,
                          (xLibU8_t *) & keyUcastServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUcastServerIndexValue, owa.len);
  /* get the value from application */
  owa.l7rc = usmDbSntpServerTableAuthenticateKeyGet (keyUcastServerIndexValue,&keysntpAuthKeyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
 FPOBJ_TRACE_NEW_KEY (bufp, &keysntpAuthKeyIndexValue, owa.len);
 /* return the object value: sntpAuthEncryptionKeyNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & keysntpAuthKeyIndexValue,
                           sizeof (keysntpAuthKeyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

