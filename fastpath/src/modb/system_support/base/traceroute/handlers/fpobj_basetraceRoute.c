
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basetraceRoute.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  06 October 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basetraceRoute_obj.h"
#include "util_pstring.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_traceroute_api.h"
#include "usmdb_util_api.h"
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include "strlib_common_common.h"
#include "web_buffer.h"
#include "web.h"
#include "traceroute_exports.h"
L7_uchar8 webStaticContentBuffer_g[WEB_STATIC_CONTENT_BUFFER_SIZE];
/*********************************************************************
*
* @purpose  Callback for the traceroute
*
* @param L7_uint32 probePerHop
* @param L7_ushort16 handle
* @param L7_uint32 ttl
* @param L7_uint32 ipda
* @param L7_uint16 hopCount
* @param L7_uint16 probeCount
* @param L7_uint32 rtt
* @param L7_char8  errSym
*
* @returns L7_SUCCESS
*
* @notes
*       Prints response for one probe
*
* @end
*
*********************************************************************/
L7_RC_t traceRoute_Callback( void * param, L7_ushort16 handle, L7_uint32 ttl,
                            L7_uint32 ipDa, L7_ushort16 hopCount, L7_ushort16 probeCount,
                            L7_uint32 rtt, L7_char8 errSym )
{
  static L7_uint32 lastIpAddr = 0;
  L7_uchar8 buff[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 ipBuff[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 tmpBuff[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 probePerHop = (L7_uint32)param;

  buff[0] = '\0';
  if ( probeCount == 1 )
  {
    lastIpAddr = ipDa;
    osapiInetNtoa( ipDa, tmpBuff );
    osapiSnprintf(buff,sizeof(buff), "%d %s   ", ttl, tmpBuff );
  }

  if ( ipDa != lastIpAddr )
  {
    osapiInetNtoa( ipDa, tmpBuff );
    osapiSnprintf(ipBuff, sizeof(ipBuff), "(%s) ", tmpBuff );
    osapiStrncat(buff,ipBuff,strlen(ipBuff));
  }

  sprintfAddBlanks (0, 0, 0, 3, L7_NULLPTR,  tmpBuff, pStrInfo_base_Msec, rtt/TRACEROUTE_RTT_MULTIPLIER, errSym );
  osapiStrncat(buff,tmpBuff,strlen(tmpBuff));
  if ( probePerHop == 0 )
  {
    probePerHop = L7_TRACEROUTE_NUM_PROBE_PACKETS;
  }
  if ( probeCount == probePerHop )
  {
    osapiStrncat(buff, pStrInfo_common_CrLf, strlen(pStrInfo_common_CrLf));
  }

  if(strlen(webStaticContentBuffer_g)+strlen(buff)< sizeof(webStaticContentBuffer_g) )
  {
    osapiStrncat(webStaticContentBuffer_g, buff, strlen(buff));
  }
  return L7_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_basetraceRoute_IPAddress
*
* @purpose Get 'IPAddress'
 *@description  [IPAddress] The IP address or domain name used for traceroute.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_IPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objIPAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objIPAddressValue, 0x0, sizeof(objIPAddressValue));

  owa.len = sizeof (objIPAddressValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_IPAddress,
      (xLibU8_t *) &objIPAddressValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
     memset(objIPAddressValue, 0x0, sizeof(objIPAddressValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objIPAddressValue, strlen (objIPAddressValue));

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIPAddressValue, strlen (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_IPAddress
*
* @purpose Set 'IPAddress'
 *@description  [IPAddress] The IP address or domain name used for traceroute.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_IPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objIPAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IPAddress */
  owa.len = sizeof (objIPAddressValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    memset(objIPAddressValue, 0x0, sizeof(objIPAddressValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objIPAddressValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_maxFail
*
* @purpose Get 'maxFail'
 *@description  [maxFail] Specify the max fail to terminate the traceroute after
* failing to receive a response for this number of consecutive
* probes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_maxFail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmaxFailValue;

  FPOBJ_TRACE_ENTER (bufp);


  memset(&objmaxFailValue, 0x0, sizeof(objmaxFailValue));

  owa.len = sizeof (objmaxFailValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_maxFail,
      (xLibU8_t *) &objmaxFailValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
     objmaxFailValue = TRACEROUTE_DEFAULT_MAX_FAIL;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmaxFailValue, sizeof (objmaxFailValue));

  /* return the object value: maxFail */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmaxFailValue, sizeof (objmaxFailValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_maxFail
*
* @purpose Set 'maxFail'
 *@description  [maxFail] Specify the max fail to terminate the traceroute after
* failing to receive a response for this number of consecutive
* probes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_maxFail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmaxFailValue;

  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: maxFail */
  owa.len = sizeof (objmaxFailValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmaxFailValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmaxFailValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_probes
*
* @purpose Get 'probes'
 *@description  [probes] Specify the number of probes to send for each TTL value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_probes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objprobesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(&objprobesValue, 0x0, sizeof(objprobesValue));

  owa.len = sizeof (objprobesValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_probes,
      (xLibU8_t *) &objprobesValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
     objprobesValue = TRACEROUTE_DEFAULT_PROBE_PER_HOP;
  }

  FPOBJ_TRACE_VALUE (bufp, &objprobesValue, sizeof (objprobesValue));

  /* return the object value: probes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objprobesValue, sizeof (objprobesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_probes
*
* @purpose Set 'probes'
 *@description  [probes] Specify the number of probes to send for each TTL value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_probes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objprobesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: probes */
  owa.len = sizeof (objprobesValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objprobesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objprobesValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_maxTTL
*
* @purpose Get 'maxTTL'
 *@description  [maxTTL] specify the maximum TTL.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_maxTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmaxTTLValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(&objmaxTTLValue, 0x0, sizeof(objmaxTTLValue));

  owa.len = sizeof (objmaxTTLValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_maxTTL,
      (xLibU8_t *) &objmaxTTLValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
     objmaxTTLValue = TRACEROUTE_DEFAULT_MAX_TTL;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmaxTTLValue, sizeof (objmaxTTLValue));

  /* return the object value: maxTTL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmaxTTLValue, sizeof (objmaxTTLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_maxTTL
*
* @purpose Set 'maxTTL'
 *@description  [maxTTL] specify the maximum TTL.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_maxTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmaxTTLValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: maxTTL */
  owa.len = sizeof (objmaxTTLValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmaxTTLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmaxTTLValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_initTTL
*
* @purpose Get 'initTTL'
 *@description  [initTTL] Specify the initial time-to-live, the maximum number
* of router hops between local and remote system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_initTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objinitTTLValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(&objinitTTLValue, 0x0, sizeof(objinitTTLValue));

  owa.len = sizeof (objinitTTLValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_initTTL,
      (xLibU8_t *) &objinitTTLValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objinitTTLValue = TRACEROUTE_DEFAULT_INIT_TTL;
  }

  FPOBJ_TRACE_VALUE (bufp, &objinitTTLValue, sizeof (objinitTTLValue));

  /* return the object value: initTTL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinitTTLValue, sizeof (objinitTTLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_initTTL
*
* @purpose Set 'initTTL'
 *@description  [initTTL] Specify the initial time-to-live, the maximum number
* of router hops between local and remote system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_initTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objinitTTLValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: initTTL */
  owa.len = sizeof (objinitTTLValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objinitTTLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objinitTTLValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_interval
*
* @purpose Get 'interval'
 *@description  [interval] Interval specify the time between probes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_interval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objintervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(&objintervalValue, 0x0, sizeof(objintervalValue));

  owa.len = sizeof (objintervalValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_interval,
      (xLibU8_t *) &objintervalValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objintervalValue = TRACEROUTE_DEFAULT_PROBE_INTERVAL;
  }

  FPOBJ_TRACE_VALUE (bufp, &objintervalValue, sizeof (objintervalValue));

  /* return the object value: interval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objintervalValue, sizeof (objintervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_interval
*
* @purpose Set 'interval'
 *@description  [interval] Interval specify the time between probes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_interval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objintervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: interval */
  owa.len = sizeof (objintervalValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objintervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objintervalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_port
*
* @purpose Get 'port'
 *@description  [port] Destination port of the probe.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_port (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objportValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(&objportValue, 0x0, sizeof(objportValue));

  owa.len = sizeof (objportValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_port,
      (xLibU8_t *) &objportValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objportValue = TRACEROUTE_DEFAULT_PORT;
  }

  FPOBJ_TRACE_VALUE (bufp, &objportValue, sizeof (objportValue));

  /* return the object value: port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objportValue, sizeof (objportValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_port
*
* @purpose Set 'port'
 *@description  [port] Destination port of the probe.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_port (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objportValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: port */
  owa.len = sizeof (objportValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objportValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objportValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_size
*
* @purpose Get 'size'
 *@description  [size] Size of the payload of the Echo Requests sent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_size (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(&objsizeValue, 0x0, sizeof(objsizeValue));

  owa.len = sizeof (objsizeValue);

  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_size,
      (xLibU8_t *) &objsizeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objsizeValue = TRACEROUTE_DEFAULT_PROBE_SIZE;
  }


  FPOBJ_TRACE_VALUE (bufp, &objsizeValue, sizeof (objsizeValue));

  /* return the object value: size */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsizeValue, sizeof (objsizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_size
*
* @purpose Set 'size'
 *@description  [size] Size of the payload of the Echo Requests sent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_size (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: size */
  owa.len = sizeof (objsizeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsizeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRoute_traceoutput
*
* @purpose Get 'traceoutput'
 *@description  [traceoutput] traceroute Reply Status.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRoute_traceoutput (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_uchar8 objtraceoutputValue[USMWEB_BUFFER_SIZE_1024];

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  memset(objtraceoutputValue, 0x0, sizeof(objtraceoutputValue));

  /*Actully we should copy the size of webStaticContentBuffer_g. But xlib tool support 1024 bytes only */
  osapiStrncpy(objtraceoutputValue, webStaticContentBuffer_g, (sizeof(objtraceoutputValue)-1));
  memset(webStaticContentBuffer_g, 0x0, sizeof(webStaticContentBuffer_g));
  FPOBJ_TRACE_VALUE (bufp, objtraceoutputValue, strlen (objtraceoutputValue));

  /* return the object value: traceoutput */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objtraceoutputValue, strlen (objtraceoutputValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basetraceRoute_traceAction
*
* @purpose Set 'traceAction'
 *@description  [traceAction] traceroute query function.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRoute_traceAction (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  fpObjWa_t kwa = FPOBJ_INIT_WA2 ();
  xLibU32_t objtraceActionValue;
  xLibU32_t objprobesValue;
  xLibU32_t objmaxTTLValue;
  xLibU32_t objinitTTLValue;
  xLibU32_t objintervalValue;
  xLibU32_t objportValue;
  xLibU32_t objsizeValue;
  xLibU32_t objmaxFailValue;
  xLibStr256_t objIPAddressValue;
  L7_char8    hostFQDN[USMWEB_BUFFER_SIZE_256];
  L7_char8    buf[USMWEB_BUFFER_SIZE_256];
  L7_uint32 ipaddr;
  L7_inet_addr_t  inetAddr;
  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 ttl;
  L7_ushort16 currHopCount, currProbeCount, testAttempts, testSuccess;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_RC_t rc = L7_FAILURE;
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_uint32 vrfId = L7_VALID_VRID;
 
  memset(objIPAddressValue, 0x0, sizeof(objIPAddressValue));
  memset(&objprobesValue, 0x0, sizeof(objprobesValue));
  memset(&objmaxTTLValue, 0x0, sizeof(objmaxTTLValue));
  memset(&objinitTTLValue, 0x0, sizeof(objinitTTLValue));
  memset(&objintervalValue, 0x0, sizeof(objintervalValue));
  memset(&objportValue, 0x0, sizeof(objportValue));
  memset(&objsizeValue, 0x0, sizeof(objsizeValue ));
  memset(&objmaxFailValue, 0x0, sizeof(objmaxFailValue));
  memset(hostFQDN, 0x0, sizeof(hostFQDN));
  memset(buf, 0x0, sizeof(buf));
  memset(webStaticContentBuffer_g, 0, sizeof(webStaticContentBuffer_g));

  inetAddressReset(&inetAddr);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: traceAction */
  owa.len = sizeof (objtraceActionValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objtraceActionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objtraceActionValue, owa.len);


  kwa.len = sizeof (objmaxFailValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_maxFail,
      (xLibU8_t *) &objmaxFailValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc =  XLIBRC_INVALID_MAX_FAIL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  kwa.len = sizeof (objprobesValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_probes,
      (xLibU8_t *) &objprobesValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc =  XLIBRC_INVALID_PROBES_PER_HOP;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  kwa.len = sizeof (objmaxTTLValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_maxTTL,
      (xLibU8_t *) &objmaxTTLValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc =  XLIBRC_INVALID_MAX_TTL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  kwa.len = sizeof (objinitTTLValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_initTTL,
      (xLibU8_t *) &objinitTTLValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc =  XLIBRC_INVALID_INIT_TTL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  kwa.len = sizeof (objintervalValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_interval,
      (xLibU8_t *) &objintervalValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc =  XLIBRC_INVALID_INTERVAL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  kwa.len = sizeof (objportValue);
  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_port,
      (xLibU8_t *) &objportValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_PORT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  kwa.len = sizeof (objsizeValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_size,
      (xLibU8_t *) &objsizeValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_DATAGRAM_SIZE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  kwa.len = sizeof (objIPAddressValue);

  kwa.rc = xLibFilterGet (wap, XOBJ_basetraceRoute_IPAddress,
      (xLibU8_t *) &objIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_HOST_ADDRESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  rc = usmDbIPHostAddressValidate(objIPAddressValue, &ipaddr, &addrType);
  if (rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_INVALID_HOST_NAME_OR_IPV4_ADDR;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  else
  {
    if ((rc == L7_SUCCESS) && (addrType == L7_IP_ADDRESS_TYPE_DNS))
    {
        rc = usmDbDNSClientInetNameLookup(L7_AF_INET, objIPAddressValue, &status, hostFQDN, &inetAddr);
        inetAddressGet(L7_AF_INET, &inetAddr, &ipaddr);
    }
 
    if ((rc == L7_SUCCESS) && (ipaddr != 0x00))
    {
          if(usmDbTraceRoute( "", "", L7_TRUE, vrfId, ipaddr, objsizeValue, (L7_ushort16)objprobesValue,
                             objintervalValue, L7_FALSE, objportValue, objmaxTTLValue, objinitTTLValue, objmaxFailValue, traceRoute_Callback,
                             (void *)objprobesValue, &handle  ) != L7_SUCCESS )
          {
            sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, webStaticContentBuffer_g, pStrInfo_base_TracerouteFailed );
          }
          else
          {
            if ( usmDbTraceRouteQuery( handle, &operStatus, &ttl,&currHopCount, &currProbeCount,
                                      &testAttempts, &testSuccess ) != L7_SUCCESS )
            {
              sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, webStaticContentBuffer_g, pStrInfo_base_TracerouteFailed );
            }
            else
            {
              memset(buf, 0x0, sizeof(buf)); 
              sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, buf,pStrInfo_base_HopCountLastTtlTestAttemptTestSuccess,
                                currHopCount, ttl, testAttempts, testSuccess );
              osapiStrncat(webStaticContentBuffer_g,buf,strlen(buf));
            }
            usmDbTraceRouteFree( handle );
          }

    }
    else
    {
       owa.rc = XLIBRC_DNS_LOOKUP_FAILED;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
  }
  
  owa.l7rc = L7_SUCCESS;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



