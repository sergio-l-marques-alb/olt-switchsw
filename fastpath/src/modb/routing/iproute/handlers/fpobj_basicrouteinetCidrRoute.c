/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basicrouteinetCidrRoute.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  11 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basicrouteinetCidrRoute_obj.h"
#include "osapi_support.h"
#include "usmdb_mib_ipfwd_api.h"
#include "usmdb_common.h"

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDestType
*
* @purpose Get 'inetCidrRouteDestType'
 *@description  [inetCidrRouteDestType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDestType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibU32_t nextObjinetCidrRouteDestTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjinetCidrRouteDestTypeValue = 1;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinetCidrRouteDestTypeValue, owa.len);

  /* return the object value: inetCidrRouteDestType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinetCidrRouteDestTypeValue,
                           sizeof (objinetCidrRouteDestTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDest
*
* @purpose Get 'inetCidrRouteDest'
 *@description  [inetCidrRouteDest] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDest (void *wap, void *bufp)
{
  xLibStr256_t objinetCidrRouteDestValue;
  xLibStr256_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibStr256_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 netMask = 0xffffffff;
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 routeTosNext = 0;
  L7_uint32 length = 0;

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                         (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objinetCidrRouteDestValue, 0, sizeof (objinetCidrRouteDestValue));
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    /* Converting Prefix Length to Netmask */
    netMask = netMask << (32- objinetCidrRoutePfxLenValue);


  
    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
    memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
    ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
    memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
    ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

    owa.l7rc =
      usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                      &netMask,&routeTosNext,&ipAddrNexthopValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
    memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
    osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    do
    {
      /* Converting Prefix Length to Netmask */
      netMask = netMask << (32- objinetCidrRoutePfxLenValue);
      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
      memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
      ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
      memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
      ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

      owa.l7rc =
        usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                      &netMask,&routeTosNext,&ipAddrNexthopValue);

      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
      memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
      osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );

      while(netMask & bitValue)
      {  
        length ++;
        bitValue = bitValue >> 1;
      }

      objinetCidrRoutePfxLenValue = length; 

      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      ipAddrNexthopValue = (xLibU32_t) osapiNtohl (ipAddrNexthopValue);
      memcpy (tempStrVal, &ipAddrNexthopValue, sizeof (ipAddrNexthopValue));
      osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteNextHopValue, sizeof(nextObjinetCidrRouteDestValue));
     
      strcpy(nextObjinetCidrRouteNextHopValue,objinetCidrRouteNextHopValue);    
      
    }
    while ((strcmp(objinetCidrRouteDestValue,nextObjinetCidrRouteDestValue)==0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjinetCidrRouteDestValue, strlen (objinetCidrRouteDestValue));

  /* return the object value: inetCidrRouteDest */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjinetCidrRouteDestValue,
                           strlen (objinetCidrRouteDestValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePfxLen
*
* @purpose Get 'inetCidrRoutePfxLen'
 *@description  [inetCidrRoutePfxLen] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePfxLen (void *wap, void *bufp)
{

  xLibStr256_t objinetCidrRouteDestValue;
  xLibStr256_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibU32_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibStr256_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 netMask = 0xffffffff;
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 routeTosNext = 0;
  L7_uint32 length = 0;

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);

  /* retrieve key: inetCidrRoutePfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                          (xLibU8_t *) & objinetCidrRoutePfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    /* Converting Prefix Length to Netmask */
    netMask = netMask << (32- objinetCidrRoutePfxLenValue);


  
    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
    memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
    ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
    memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
    ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

    owa.l7rc =
      usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                      &netMask,&routeTosNext,&ipAddrNexthopValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
    memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
    osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );

    
    while(netMask & bitValue)
    {  
      length ++;
      bitValue = bitValue >> 1;
    }
    nextObjinetCidrRoutePfxLenValue = length; 

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRoutePfxLenValue, owa.len);
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    nextObjinetCidrRoutePfxLenValue = objinetCidrRoutePfxLenValue;
    do
    {
      /* Converting Prefix Length to Netmask */
      netMask = netMask << (32- objinetCidrRoutePfxLenValue);
      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
      memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
      ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
      memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
      ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

      owa.l7rc =
        usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                      &netMask,&routeTosNext,&ipAddrNexthopValue);

      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
      memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
      osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );

      while(netMask & bitValue)
      {  
        length ++;
        bitValue = bitValue >> 1;
      }

      objinetCidrRoutePfxLenValue = length; 

      memset(tempStrVal,0x00,sizeof(tempStrVal));  
      ipAddrNexthopValue = (xLibU32_t) osapiNtohl (ipAddrNexthopValue);
      memcpy (tempStrVal, &ipAddrNexthopValue, sizeof (ipAddrNexthopValue));
      osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteNextHopValue, sizeof(nextObjinetCidrRouteDestValue));
     
      strcpy(nextObjinetCidrRouteNextHopValue,objinetCidrRouteNextHopValue);    
      
    }
    while ((strcmp(objinetCidrRouteDestValue, nextObjinetCidrRouteDestValue)==0)
           && (objinetCidrRoutePfxLenValue == nextObjinetCidrRoutePfxLenValue)
           && (owa.l7rc == L7_SUCCESS));

    nextObjinetCidrRoutePfxLenValue = objinetCidrRoutePfxLenValue;
  }

  if ((strcmp(objinetCidrRouteDestValue,nextObjinetCidrRouteDestValue)!=0) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinetCidrRoutePfxLenValue, owa.len);

  /* return the object value: inetCidrRoutePfxLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinetCidrRoutePfxLenValue,
                           sizeof (objinetCidrRoutePfxLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePolicy
*
* @purpose Get 'inetCidrRoutePolicy'
 *@description  [inetCidrRoutePolicy] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePolicy (void *wap, void *bufp)
{

  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibStr256_t nextObjinetCidrRoutePolicyValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: inetCidrRoutePolicy */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                          (xLibU8_t *) objinetCidrRoutePolicyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    strcpy(nextObjinetCidrRoutePolicyValue,"0.0");
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRoutePolicyValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }
  
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjinetCidrRoutePolicyValue, strlen(nextObjinetCidrRoutePolicyValue));

  /* return the object value: inetCidrRoutePolicy */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjinetCidrRoutePolicyValue,
                           strlen (nextObjinetCidrRoutePolicyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHopType
*
* @purpose Get 'inetCidrRouteNextHopType'
 *@description  [inetCidrRouteNextHopType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHopType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibU32_t nextObjinetCidrRouteNextHopTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteNextHopType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                          (xLibU8_t *) & objinetCidrRouteNextHopTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjinetCidrRouteNextHopTypeValue = 1;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteNextHopTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinetCidrRouteNextHopTypeValue, owa.len);

  /* return the object value: inetCidrRouteNextHopType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinetCidrRouteNextHopTypeValue,
                           sizeof (objinetCidrRouteNextHopTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHop
*
* @purpose Get 'inetCidrRouteNextHop'
 *@description  [inetCidrRouteNextHop] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHop (void *wap, void *bufp)
{

  xLibStr256_t objinetCidrRouteDestValue;
  xLibStr256_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibU32_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibStr256_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 routeTosNext = 0;
  L7_uint32 length = 0;

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);

  /* retrieve key: inetCidrRoutePfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                          (xLibU8_t *) & objinetCidrRoutePfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRoutePfxLenValue, owa.len);

  /* retrieve key: inetCidrRoutePolicy */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                          (xLibU8_t *) objinetCidrRoutePolicyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRoutePolicyValue, owa.len);

  /* retrieve key: inetCidrRouteNextHop */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                          (xLibU8_t *) objinetCidrRouteNextHopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));

    /* Converting Prefix Length to Netmask */
    netMask = netMask << (32- objinetCidrRoutePfxLenValue);


  
    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
    memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
    ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
    memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
    ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

    owa.l7rc =
      usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                      &netMask,&routeTosNext,&ipAddrNexthopValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
    memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
    osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );

    
    while(netMask & bitValue)
    {  
      length ++;
      bitValue = bitValue >> 1;
    }
    nextObjinetCidrRoutePfxLenValue = length; 

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    ipAddrNexthopValue = (xLibU32_t) osapiNtohl (ipAddrNexthopValue);
    memcpy (tempStrVal, &ipAddrNexthopValue, sizeof (ipAddrNexthopValue));
    osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteNextHopValue, sizeof(nextObjinetCidrRouteDestValue));
    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteNextHopValue, owa.len);
    /* Converting Prefix Length to Netmask */
    netMask = netMask << (32- objinetCidrRoutePfxLenValue);


  
    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
    memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
    ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
    memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
    ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

    owa.l7rc =
      usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                      &netMask,&routeTosNext,&ipAddrNexthopValue);

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
    memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
    osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );

    
    while(netMask & bitValue)
    {  
      length ++;
      bitValue = bitValue >> 1;
    }
    nextObjinetCidrRoutePfxLenValue = length; 

    memset(tempStrVal,0x00,sizeof(tempStrVal));  
    ipAddrNexthopValue = (xLibU32_t) osapiNtohl (ipAddrNexthopValue);
    memcpy (tempStrVal, &ipAddrNexthopValue, sizeof (ipAddrNexthopValue));
    osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteNextHopValue, sizeof(nextObjinetCidrRouteDestValue));
   
  }

  if ((strcmp(objinetCidrRouteDestValue,nextObjinetCidrRouteDestValue)!=0)
      || (objinetCidrRoutePfxLenValue != nextObjinetCidrRoutePfxLenValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjinetCidrRouteNextHopValue, strlen(nextObjinetCidrRouteNextHopValue));

  /* return the object value: inetCidrRouteNextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjinetCidrRouteNextHopValue,
                           strlen(nextObjinetCidrRouteNextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDestType_inetCidrRouteDest_inetCidrRoutePfxLen_inetCidrRoutePolicy_inetCidrRouteNextHopType_inetCidrRouteNextHop
*
* @purpose Get 'inetCidrRouteDestType + inetCidrRouteDest + inetCidrRoutePfxLen + inetCidrRoutePolicy + inetCidrRouteNextHopType + inetCidrRouteNextHop +'
*
* @description [inetCidrRouteDestType]: ToDO: Add Help 
*              [inetCidrRouteDest]: ToDO: Add Help 
*              [inetCidrRoutePfxLen]: ToDO: Add Help 
*              [inetCidrRoutePolicy]: ToDO: Add Help 
*              [inetCidrRouteNextHopType]: ToDO: Add Help 
*              [inetCidrRouteNextHop]: ToDO: Add Help 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDestType_inetCidrRouteDest_inetCidrRoutePfxLen_inetCidrRoutePolicy_inetCidrRouteNextHopType_inetCidrRouteNextHop
(void *wap, void *bufp[], xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owainetCidrRouteDestType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteDestTypeValue, nextObjinetCidrRouteDestTypeValue;
  fpObjWa_t owainetCidrRouteDest = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objinetCidrRouteDestValue, nextObjinetCidrRouteDestValue;
  fpObjWa_t owainetCidrRoutePfxLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRoutePfxLenValue, nextObjinetCidrRoutePfxLenValue;
  fpObjWa_t owainetCidrRoutePolicy = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objinetCidrRoutePolicyValue, nextObjinetCidrRoutePolicyValue;
  fpObjWa_t owainetCidrRouteNextHopType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteNextHopTypeValue,
    nextObjinetCidrRouteNextHopTypeValue;
  fpObjWa_t owainetCidrRouteNextHop = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objinetCidrRouteNextHopValue, nextObjinetCidrRouteNextHopValue;
  void *outinetCidrRouteDestType = (void *) bufp[--keyCount];
  void *outinetCidrRouteDest = (void *) bufp[--keyCount];
  void *outinetCidrRoutePfxLen = (void *) bufp[--keyCount];
  void *outinetCidrRoutePolicy = (void *) bufp[--keyCount];
  void *outinetCidrRouteNextHopType = (void *) bufp[--keyCount];
  void *outinetCidrRouteNextHop = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outinetCidrRouteDestType);
  FPOBJ_TRACE_ENTER (outinetCidrRouteDest);
  FPOBJ_TRACE_ENTER (outinetCidrRoutePfxLen);
  FPOBJ_TRACE_ENTER (outinetCidrRoutePolicy);
  FPOBJ_TRACE_ENTER (outinetCidrRouteNextHopType);
  FPOBJ_TRACE_ENTER (outinetCidrRouteNextHop);

  L7_uint32 netMask = 0xffffffff;
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 routeTosNext = 0;
  L7_uint32 length = 0;

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  owainetCidrRouteDestType.rc =
    xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                   (xLibU8_t *) & objinetCidrRouteDestTypeValue,
                   &owainetCidrRouteDestType.len);
  if (owainetCidrRouteDestType.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: inetCidrRouteDest */
    owainetCidrRouteDest.rc =
      xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                     (xLibU8_t *) objinetCidrRouteDestValue,
                     &owainetCidrRouteDest.len);
    if (owainetCidrRouteDest.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: inetCidrRoutePfxLen */
      owainetCidrRoutePfxLen.rc =
        xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                       (xLibU8_t *) & objinetCidrRoutePfxLenValue,
                       &owainetCidrRoutePfxLen.len);
      if (owainetCidrRoutePfxLen.rc == XLIBRC_SUCCESS)
      {
        /* retrieve key: inetCidrRoutePolicy */
        owainetCidrRoutePolicy.rc =
          xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                         (xLibU8_t *) objinetCidrRoutePolicyValue,
                         &owainetCidrRoutePolicy.len);
        if (owainetCidrRoutePolicy.rc == XLIBRC_SUCCESS)
        {
          /* retrieve key: inetCidrRouteNextHopType */
          owainetCidrRouteNextHopType.rc =
            xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & objinetCidrRouteNextHopTypeValue,
                           &owainetCidrRouteNextHopType.len);
          if (owainetCidrRouteNextHopType.rc == XLIBRC_SUCCESS)
          {
            /* retrieve key: inetCidrRouteNextHop */
            owainetCidrRouteNextHop.rc =
              xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                             (xLibU8_t *) objinetCidrRouteNextHopValue,
                             &owainetCidrRouteNextHop.len);
          }
        }
      }
    }
  }
  else
  {
  	objinetCidrRouteDestTypeValue = 1;
	memset(objinetCidrRouteDestValue,0x00,sizeof(objinetCidrRouteDestValue));
	objinetCidrRoutePfxLenValue = 0;
       memset(objinetCidrRoutePolicyValue,0x00,sizeof(objinetCidrRoutePolicyValue));
       objinetCidrRouteNextHopTypeValue = 1;
       memset(objinetCidrRouteNextHopValue,0x00,sizeof(objinetCidrRouteNextHopValue));	
  
  }
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteDestType,
                           &objinetCidrRouteDestTypeValue,
                           owainetCidrRouteDestType.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteDest, &objinetCidrRouteDestValue,
                           owainetCidrRouteDest.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRoutePfxLen, &objinetCidrRoutePfxLenValue,
                           owainetCidrRoutePfxLen.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRoutePolicy, &objinetCidrRoutePolicyValue,
                           owainetCidrRoutePolicy.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteNextHopType,
                           &objinetCidrRouteNextHopTypeValue,
                           owainetCidrRouteNextHopType.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteNextHop,
                           &objinetCidrRouteNextHopValue,
                           owainetCidrRouteNextHop.len);


  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- objinetCidrRoutePfxLenValue);


  
  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, objinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, objinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  
  owa.rc =
    usmDbIpCidrRouteEntryGetNext (USMDB_UNIT_CURRENT,&ipAddrRouteDestValue,
                                    &netMask,&routeTosNext,&ipAddrNexthopValue);

  while(netMask & bitValue)
  {
    length ++;
    bitValue = bitValue >> 1;
  }

  nextObjinetCidrRoutePfxLenValue = length; 

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outinetCidrRouteDestType, owainetCidrRouteDestType);
    FPOBJ_TRACE_EXIT (outinetCidrRouteDest, owainetCidrRouteDest);
    FPOBJ_TRACE_EXIT (outinetCidrRoutePfxLen, owainetCidrRoutePfxLen);
    FPOBJ_TRACE_EXIT (outinetCidrRoutePolicy, owainetCidrRoutePolicy);
    FPOBJ_TRACE_EXIT (outinetCidrRouteNextHopType, owainetCidrRouteNextHopType);
    FPOBJ_TRACE_EXIT (outinetCidrRouteNextHop, owainetCidrRouteNextHop);
    return owa.rc;
  }
  
  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  ipAddrRouteDestValue = (xLibU32_t) osapiNtohl (ipAddrRouteDestValue);
  memcpy (tempStrVal, &ipAddrRouteDestValue, sizeof (ipAddrRouteDestValue));
  osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteDestValue, sizeof(nextObjinetCidrRouteDestValue) );

 memset(tempStrVal,0x00,sizeof(tempStrVal));  
  ipAddrNexthopValue = (xLibU32_t) osapiNtohl (ipAddrNexthopValue);
  memcpy (tempStrVal, &ipAddrNexthopValue, sizeof (ipAddrNexthopValue));
  osapiInetNtop (L7_AF_INET, tempStrVal, (void *) nextObjinetCidrRouteNextHopValue, sizeof(nextObjinetCidrRouteDestValue));
	
  
  nextObjinetCidrRouteDestTypeValue = 1;
  memset(nextObjinetCidrRoutePolicyValue,0x00,sizeof(nextObjinetCidrRoutePolicyValue));
  nextObjinetCidrRouteNextHopTypeValue = 1;

  
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteDestType,
                           &nextObjinetCidrRouteDestTypeValue,
                           owainetCidrRouteDestType.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteDest, &nextObjinetCidrRouteDestValue,
                           owainetCidrRouteDest.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRoutePfxLen,
                           &nextObjinetCidrRouteDestValue,
                           owainetCidrRoutePfxLen.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRoutePolicy,
                           &nextObjinetCidrRoutePolicyValue,
                           owainetCidrRoutePolicy.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteNextHopType,
                           &nextObjinetCidrRouteNextHopTypeValue,
                           owainetCidrRouteNextHopType.len);
  FPOBJ_TRACE_CURRENT_KEY (outinetCidrRouteNextHop,
                           &nextObjinetCidrRouteNextHopValue,
                           owainetCidrRouteNextHop.len);

  /* return the object value: inetCidrRouteDestType */
  xLibBufDataSet (outinetCidrRouteDestType,
                  (xLibU8_t *) & nextObjinetCidrRouteDestTypeValue,
                  sizeof (nextObjinetCidrRouteDestTypeValue));

  /* return the object value: inetCidrRouteDest */
  xLibBufDataSet (outinetCidrRouteDest,
                  (xLibU8_t *) nextObjinetCidrRouteDestValue,
                  strlen (nextObjinetCidrRouteDestValue));

  /* return the object value: inetCidrRoutePfxLen */
  xLibBufDataSet (outinetCidrRoutePfxLen,
                  (xLibU8_t *) & nextObjinetCidrRoutePfxLenValue,
                  sizeof (nextObjinetCidrRoutePfxLenValue));

  /* return the object value: inetCidrRoutePolicy */
  xLibBufDataSet (outinetCidrRoutePolicy,
                  (xLibU8_t *) nextObjinetCidrRoutePolicyValue,
                  strlen (nextObjinetCidrRoutePolicyValue));

  /* return the object value: inetCidrRouteNextHopType */
  xLibBufDataSet (outinetCidrRouteNextHopType,
                  (xLibU8_t *) & nextObjinetCidrRouteNextHopTypeValue,
                  sizeof (nextObjinetCidrRouteNextHopTypeValue));

  /* return the object value: inetCidrRouteNextHop */
  xLibBufDataSet (outinetCidrRouteNextHop,
                  (xLibU8_t *) nextObjinetCidrRouteNextHopValue,
                  strlen (nextObjinetCidrRouteNextHopValue));
  FPOBJ_TRACE_EXIT (outinetCidrRouteDestType, owainetCidrRouteDestType);
  FPOBJ_TRACE_EXIT (outinetCidrRouteDest, owainetCidrRouteDest);
  FPOBJ_TRACE_EXIT (outinetCidrRoutePfxLen, owainetCidrRoutePfxLen);
  FPOBJ_TRACE_EXIT (outinetCidrRoutePolicy, owainetCidrRoutePolicy);
  FPOBJ_TRACE_EXIT (outinetCidrRouteNextHopType, owainetCidrRouteNextHopType);
  FPOBJ_TRACE_EXIT (outinetCidrRouteNextHop, owainetCidrRouteNextHop);
  return XLIBRC_SUCCESS;
}
#endif
#if 0
/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDestType
*
* @purpose Get 'inetCidrRouteDestType'
 *@description  [inetCidrRouteDestType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDestType (void *wap, void *bufp)
{

  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibU32_t nextObjinetCidrRouteDestTypeValue;
  xLibStr256_t objinetCidrRouteDestValue;
  xLibU32_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibU32_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibU32_t nextObjinetCidrRoutePolicyValue;
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibU32_t nextObjinetCidrRouteNextHopTypeValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibU32_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objinetCidrRouteDestTypeValue, 0, sizeof (objinetCidrRouteDestTypeValue));
    memset (objinetCidrRouteDestValue, 0, sizeof (objinetCidrRouteDestValue));
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objinetCidrRouteDestTypeValue,
                                     objinetCidrRouteDestValue,
                                     objinetCidrRoutePfxLenValue,
                                     objinetCidrRoutePolicyValue,
                                     objinetCidrRouteNextHopTypeValue,
                                     objinetCidrRouteNextHopValue,
                                     &nextObjinetCidrRouteDestTypeValue,
                                     nextObjinetCidrRouteDestValue,
                                     &nextObjinetCidrRoutePfxLenValue,
                                     nextObjinetCidrRoutePolicyValue,
                                     &nextObjinetCidrRouteNextHopTypeValue,
                                     nextObjinetCidrRouteNextHopValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);
    memset (objinetCidrRouteDestValue, 0, sizeof (objinetCidrRouteDestValue));
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objinetCidrRouteDestTypeValue,
                                      objinetCidrRouteDestValue,
                                      objinetCidrRoutePfxLenValue,
                                      objinetCidrRoutePolicyValue,
                                      objinetCidrRouteNextHopTypeValue,
                                      objinetCidrRouteNextHopValue,
                                      &nextObjinetCidrRouteDestTypeValue,
                                      nextObjinetCidrRouteDestValue,
                                      &nextObjinetCidrRoutePfxLenValue,
                                      nextObjinetCidrRoutePolicyValue,
                                      &nextObjinetCidrRouteNextHopTypeValue,
                                      nextObjinetCidrRouteNextHopValue);
    }
    while ((objinetCidrRouteDestTypeValue == nextObjinetCidrRouteDestTypeValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinetCidrRouteDestTypeValue, owa.len);

  /* return the object value: inetCidrRouteDestType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinetCidrRouteDestTypeValue,
                           sizeof (objinetCidrRouteDestTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDest
*
* @purpose Get 'inetCidrRouteDest'
 *@description  [inetCidrRouteDest] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteDest (void *wap, void *bufp)
{

  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibStr256_t nextObjinetCidrRouteDestTypeValue;
  xLibStr256_t objinetCidrRouteDestValue;
  xLibStr256_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibStr256_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibStr256_t nextObjinetCidrRoutePolicyValue;
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibStr256_t nextObjinetCidrRouteNextHopTypeValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibStr256_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objinetCidrRouteDestValue, 0, sizeof (objinetCidrRouteDestValue));
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objinetCidrRouteDestTypeValue,
                                     objinetCidrRouteDestValue,
                                     objinetCidrRoutePfxLenValue,
                                     objinetCidrRoutePolicyValue,
                                     objinetCidrRouteNextHopTypeValue,
                                     objinetCidrRouteNextHopValue,
                                     &nextObjinetCidrRouteDestTypeValue,
                                     nextObjinetCidrRouteDestValue,
                                     &nextObjinetCidrRoutePfxLenValue,
                                     nextObjinetCidrRoutePolicyValue,
                                     &nextObjinetCidrRouteNextHopTypeValue,
                                     nextObjinetCidrRouteNextHopValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objinetCidrRouteDestTypeValue,
                                      objinetCidrRouteDestValue,
                                      objinetCidrRoutePfxLenValue,
                                      objinetCidrRoutePolicyValue,
                                      objinetCidrRouteNextHopTypeValue,
                                      objinetCidrRouteNextHopValue,
                                      &nextObjinetCidrRouteDestTypeValue,
                                      nextObjinetCidrRouteDestValue,
                                      &nextObjinetCidrRoutePfxLenValue,
                                      nextObjinetCidrRoutePolicyValue,
                                      &nextObjinetCidrRouteNextHopTypeValue,
                                      nextObjinetCidrRouteNextHopValue);
    }
    while ((objinetCidrRouteDestTypeValue == nextObjinetCidrRouteDestTypeValue)
           && (objinetCidrRouteDestValue == nextObjinetCidrRouteDestValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objinetCidrRouteDestTypeValue != nextObjinetCidrRouteDestTypeValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjinetCidrRouteDestValue, owa.len);

  /* return the object value: inetCidrRouteDest */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjinetCidrRouteDestValue,
                           strlen (objinetCidrRouteDestValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePfxLen
*
* @purpose Get 'inetCidrRoutePfxLen'
 *@description  [inetCidrRoutePfxLen] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePfxLen (void *wap, void *bufp)
{

  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibU32_t nextObjinetCidrRouteDestTypeValue;
  xLibStr256_t objinetCidrRouteDestValue;
  xLibU32_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibU32_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibU32_t nextObjinetCidrRoutePolicyValue;
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibU32_t nextObjinetCidrRouteNextHopTypeValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibU32_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);

  /* retrieve key: inetCidrRoutePfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                          (xLibU8_t *) & objinetCidrRoutePfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objinetCidrRoutePfxLenValue, 0, sizeof (objinetCidrRoutePfxLenValue));
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objinetCidrRouteDestTypeValue,
                                     objinetCidrRouteDestValue,
                                     objinetCidrRoutePfxLenValue,
                                     objinetCidrRoutePolicyValue,
                                     objinetCidrRouteNextHopTypeValue,
                                     objinetCidrRouteNextHopValue,
                                     &nextObjinetCidrRouteDestTypeValue,
                                     nextObjinetCidrRouteDestValue,
                                     &nextObjinetCidrRoutePfxLenValue,
                                     nextObjinetCidrRoutePolicyValue,
                                     &nextObjinetCidrRouteNextHopTypeValue,
                                     nextObjinetCidrRouteNextHopValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRoutePfxLenValue, owa.len);
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objinetCidrRouteDestTypeValue,
                                      objinetCidrRouteDestValue,
                                      objinetCidrRoutePfxLenValue,
                                      objinetCidrRoutePolicyValue,
                                      objinetCidrRouteNextHopTypeValue,
                                      objinetCidrRouteNextHopValue,
                                      &nextObjinetCidrRouteDestTypeValue,
                                      nextObjinetCidrRouteDestValue,
                                      &nextObjinetCidrRoutePfxLenValue,
                                      nextObjinetCidrRoutePolicyValue,
                                      &nextObjinetCidrRouteNextHopTypeValue,
                                      nextObjinetCidrRouteNextHopValue);
    }
    while ((objinetCidrRouteDestTypeValue == nextObjinetCidrRouteDestTypeValue)
           && (objinetCidrRouteDestValue == nextObjinetCidrRouteDestValue)
           && (objinetCidrRoutePfxLenValue == nextObjinetCidrRoutePfxLenValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objinetCidrRouteDestTypeValue != nextObjinetCidrRouteDestTypeValue)
      || (objinetCidrRouteDestValue != nextObjinetCidrRouteDestValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinetCidrRoutePfxLenValue, owa.len);

  /* return the object value: inetCidrRoutePfxLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinetCidrRoutePfxLenValue,
                           sizeof (objinetCidrRoutePfxLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePolicy
*
* @purpose Get 'inetCidrRoutePolicy'
 *@description  [inetCidrRoutePolicy] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRoutePolicy (void *wap, void *bufp)
{

  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibStr256_t nextObjinetCidrRouteDestTypeValue;
  xLibStr256_t objinetCidrRouteDestValue;
  xLibStr256_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibStr256_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibStr256_t nextObjinetCidrRoutePolicyValue;
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibStr256_t nextObjinetCidrRouteNextHopTypeValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibStr256_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);

  /* retrieve key: inetCidrRoutePfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                          (xLibU8_t *) & objinetCidrRoutePfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRoutePfxLenValue, owa.len);

  /* retrieve key: inetCidrRoutePolicy */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                          (xLibU8_t *) objinetCidrRoutePolicyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objinetCidrRoutePolicyValue, 0, sizeof (objinetCidrRoutePolicyValue));
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objinetCidrRouteDestTypeValue,
                                     objinetCidrRouteDestValue,
                                     objinetCidrRoutePfxLenValue,
                                     objinetCidrRoutePolicyValue,
                                     objinetCidrRouteNextHopTypeValue,
                                     objinetCidrRouteNextHopValue,
                                     &nextObjinetCidrRouteDestTypeValue,
                                     nextObjinetCidrRouteDestValue,
                                     &nextObjinetCidrRoutePfxLenValue,
                                     nextObjinetCidrRoutePolicyValue,
                                     &nextObjinetCidrRouteNextHopTypeValue,
                                     nextObjinetCidrRouteNextHopValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRoutePolicyValue, owa.len);
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objinetCidrRouteDestTypeValue,
                                      objinetCidrRouteDestValue,
                                      objinetCidrRoutePfxLenValue,
                                      objinetCidrRoutePolicyValue,
                                      objinetCidrRouteNextHopTypeValue,
                                      objinetCidrRouteNextHopValue,
                                      &nextObjinetCidrRouteDestTypeValue,
                                      nextObjinetCidrRouteDestValue,
                                      &nextObjinetCidrRoutePfxLenValue,
                                      nextObjinetCidrRoutePolicyValue,
                                      &nextObjinetCidrRouteNextHopTypeValue,
                                      nextObjinetCidrRouteNextHopValue);
    }
    while ((objinetCidrRouteDestTypeValue == nextObjinetCidrRouteDestTypeValue)
           && (objinetCidrRouteDestValue == nextObjinetCidrRouteDestValue)
           && (objinetCidrRoutePfxLenValue == nextObjinetCidrRoutePfxLenValue)
           && (objinetCidrRoutePolicyValue == nextObjinetCidrRoutePolicyValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objinetCidrRouteDestTypeValue != nextObjinetCidrRouteDestTypeValue)
      || (objinetCidrRouteDestValue != nextObjinetCidrRouteDestValue)
      || (objinetCidrRoutePfxLenValue != nextObjinetCidrRoutePfxLenValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjinetCidrRoutePolicyValue, owa.len);

  /* return the object value: inetCidrRoutePolicy */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjinetCidrRoutePolicyValue,
                           strlen (objinetCidrRoutePolicyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHopType
*
* @purpose Get 'inetCidrRouteNextHopType'
 *@description  [inetCidrRouteNextHopType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHopType (void *wap, void *bufp)
{

  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibU32_t nextObjinetCidrRouteDestTypeValue;
  xLibStr256_t objinetCidrRouteDestValue;
  xLibU32_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibU32_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibU32_t nextObjinetCidrRoutePolicyValue;
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibU32_t nextObjinetCidrRouteNextHopTypeValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibU32_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);

  /* retrieve key: inetCidrRoutePfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                          (xLibU8_t *) & objinetCidrRoutePfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRoutePfxLenValue, owa.len);

  /* retrieve key: inetCidrRoutePolicy */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                          (xLibU8_t *) objinetCidrRoutePolicyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRoutePolicyValue, owa.len);

  /* retrieve key: inetCidrRouteNextHopType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                          (xLibU8_t *) & objinetCidrRouteNextHopTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objinetCidrRouteNextHopTypeValue, 0, sizeof (objinetCidrRouteNextHopTypeValue));
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objinetCidrRouteDestTypeValue,
                                     objinetCidrRouteDestValue,
                                     objinetCidrRoutePfxLenValue,
                                     objinetCidrRoutePolicyValue,
                                     objinetCidrRouteNextHopTypeValue,
                                     objinetCidrRouteNextHopValue,
                                     &nextObjinetCidrRouteDestTypeValue,
                                     nextObjinetCidrRouteDestValue,
                                     &nextObjinetCidrRoutePfxLenValue,
                                     nextObjinetCidrRoutePolicyValue,
                                     &nextObjinetCidrRouteNextHopTypeValue,
                                     nextObjinetCidrRouteNextHopValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteNextHopTypeValue, owa.len);
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objinetCidrRouteDestTypeValue,
                                      objinetCidrRouteDestValue,
                                      objinetCidrRoutePfxLenValue,
                                      objinetCidrRoutePolicyValue,
                                      objinetCidrRouteNextHopTypeValue,
                                      objinetCidrRouteNextHopValue,
                                      &nextObjinetCidrRouteDestTypeValue,
                                      nextObjinetCidrRouteDestValue,
                                      &nextObjinetCidrRoutePfxLenValue,
                                      nextObjinetCidrRoutePolicyValue,
                                      &nextObjinetCidrRouteNextHopTypeValue,
                                      nextObjinetCidrRouteNextHopValue);
    }
    while ((objinetCidrRouteDestTypeValue == nextObjinetCidrRouteDestTypeValue)
           && (objinetCidrRouteDestValue == nextObjinetCidrRouteDestValue)
           && (objinetCidrRoutePfxLenValue == nextObjinetCidrRoutePfxLenValue)
           && (objinetCidrRoutePolicyValue == nextObjinetCidrRoutePolicyValue)
           && (objinetCidrRouteNextHopTypeValue == nextObjinetCidrRouteNextHopTypeValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objinetCidrRouteDestTypeValue != nextObjinetCidrRouteDestTypeValue)
      || (objinetCidrRouteDestValue != nextObjinetCidrRouteDestValue)
      || (objinetCidrRoutePfxLenValue != nextObjinetCidrRoutePfxLenValue)
      || (objinetCidrRoutePolicyValue != nextObjinetCidrRoutePolicyValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinetCidrRouteNextHopTypeValue, owa.len);

  /* return the object value: inetCidrRouteNextHopType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinetCidrRouteNextHopTypeValue,
                           sizeof (objinetCidrRouteNextHopTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHop
*
* @purpose Get 'inetCidrRouteNextHop'
 *@description  [inetCidrRouteNextHop] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHop (void *wap, void *bufp)
{

  xLibU32_t objinetCidrRouteDestTypeValue;
  xLibStr256_t nextObjinetCidrRouteDestTypeValue;
  xLibStr256_t objinetCidrRouteDestValue;
  xLibStr256_t nextObjinetCidrRouteDestValue;
  xLibU32_t objinetCidrRoutePfxLenValue;
  xLibStr256_t nextObjinetCidrRoutePfxLenValue;
  xLibStr256_t objinetCidrRoutePolicyValue;
  xLibStr256_t nextObjinetCidrRoutePolicyValue;
  xLibU32_t objinetCidrRouteNextHopTypeValue;
  xLibStr256_t nextObjinetCidrRouteNextHopTypeValue;
  xLibStr256_t objinetCidrRouteNextHopValue;
  xLibStr256_t nextObjinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                          (xLibU8_t *) & objinetCidrRouteDestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteDestTypeValue, owa.len);

  /* retrieve key: inetCidrRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                          (xLibU8_t *) objinetCidrRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteDestValue, owa.len);

  /* retrieve key: inetCidrRoutePfxLen */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                          (xLibU8_t *) & objinetCidrRoutePfxLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRoutePfxLenValue, owa.len);

  /* retrieve key: inetCidrRoutePolicy */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                          (xLibU8_t *) objinetCidrRoutePolicyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRoutePolicyValue, owa.len);

  /* retrieve key: inetCidrRouteNextHopType */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                          (xLibU8_t *) & objinetCidrRouteNextHopTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objinetCidrRouteNextHopTypeValue, owa.len);

  /* retrieve key: inetCidrRouteNextHop */
  owa.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                          (xLibU8_t *) objinetCidrRouteNextHopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objinetCidrRouteNextHopValue, 0, sizeof (objinetCidrRouteNextHopValue));
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objinetCidrRouteDestTypeValue,
                                     objinetCidrRouteDestValue,
                                     objinetCidrRoutePfxLenValue,
                                     objinetCidrRoutePolicyValue,
                                     objinetCidrRouteNextHopTypeValue,
                                     objinetCidrRouteNextHopValue,
                                     &nextObjinetCidrRouteDestTypeValue,
                                     nextObjinetCidrRouteDestValue,
                                     &nextObjinetCidrRoutePfxLenValue,
                                     nextObjinetCidrRoutePolicyValue,
                                     &nextObjinetCidrRouteNextHopTypeValue,
                                     nextObjinetCidrRouteNextHopValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objinetCidrRouteNextHopValue, owa.len);

    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                    objinetCidrRouteDestTypeValue,
                                    objinetCidrRouteDestValue,
                                    objinetCidrRoutePfxLenValue,
                                    objinetCidrRoutePolicyValue,
                                    objinetCidrRouteNextHopTypeValue,
                                    objinetCidrRouteNextHopValue,
                                    &nextObjinetCidrRouteDestTypeValue,
                                    nextObjinetCidrRouteDestValue, &nextObjinetCidrRoutePfxLenValue,
                                    nextObjinetCidrRoutePolicyValue,
                                    &nextObjinetCidrRouteNextHopTypeValue,
                                    nextObjinetCidrRouteNextHopValue);

  }

  if ((objinetCidrRouteDestTypeValue != nextObjinetCidrRouteDestTypeValue)
      || (objinetCidrRouteDestValue != nextObjinetCidrRouteDestValue)
      || (objinetCidrRoutePfxLenValue != nextObjinetCidrRoutePfxLenValue)
      || (objinetCidrRoutePolicyValue != nextObjinetCidrRoutePolicyValue)
      || (objinetCidrRouteNextHopTypeValue != nextObjinetCidrRouteNextHopTypeValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjinetCidrRouteNextHopValue, owa.len);

  /* return the object value: inetCidrRouteNextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjinetCidrRouteNextHopValue,
                           strlen (objinetCidrRouteNextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif
/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteIfIndex
*
* @purpose Get 'inetCidrRouteIfIndex'
*
* @description [inetCidrRouteIfIndex]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

     xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

   L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */

 
  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);




  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteIfIndexGet(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteIfIndexValue);
  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteIfIndexValue,
                           sizeof (objinetCidrRouteIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteType
*
* @purpose Get 'inetCidrRouteType'
*
* @description [inetCidrRouteType]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

     xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */
  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteTypeGet(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteTypeValue);
                                  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteTypeValue,
                           sizeof (objinetCidrRouteTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteProto
*
* @purpose Get 'inetCidrRouteProto'
*
* @description [inetCidrRouteProto]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteProto (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteProtoValue;
  FPOBJ_TRACE_ENTER (bufp);

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

     xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */
  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteProtoGet(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteProtoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteProto */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteProtoValue,
                           sizeof (objinetCidrRouteProtoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteAge
*
* @purpose Get 'inetCidrRouteAge'
*
* @description [inetCidrRouteAge]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteAge (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteAgeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  

 /* routeTos is passed just to use below function call */
  /* Presently RouteAge is not implemented, so return 0 */

  objinetCidrRouteAgeValue = 0;  

  /* return the object value: inetCidrRouteAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteAgeValue,
                           sizeof (objinetCidrRouteAgeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHopAS
*
* @purpose Get 'inetCidrRouteNextHopAS'
*
* @description [inetCidrRouteNextHopAS]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteNextHopAS (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteNextHopASValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

 /* Presently as Next Hop AS implementation is not in the RTO, so just returning default 
          0 */
  objinetCidrRouteNextHopASValue = 0;
          
  /* return the object value: inetCidrRouteNextHopAS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteNextHopASValue,
                           sizeof (objinetCidrRouteNextHopASValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric1
*
* @purpose Get 'inetCidrRouteMetric1'
*
* @description [inetCidrRouteMetric1]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric1 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteMetric1Value;
  FPOBJ_TRACE_ENTER (bufp);

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */
  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc= usmDbIpCidrRouteMetric1Get(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteMetric1Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteMetric1 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteMetric1Value,
                           sizeof (objinetCidrRouteMetric1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric2
*
* @purpose Get 'inetCidrRouteMetric2'
*
* @description [inetCidrRouteMetric2]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric2 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteMetric2Value;
  FPOBJ_TRACE_ENTER (bufp);

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);;

  /* get the value from application */
   /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteMetric2Get(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteMetric2Value);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteMetric2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteMetric2Value,
                           sizeof (objinetCidrRouteMetric2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric3
*
* @purpose Get 'inetCidrRouteMetric3'
*
* @description [inetCidrRouteMetric3]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric3 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteMetric3Value;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */
     /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteMetric3Get(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteMetric3Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteMetric3 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteMetric3Value,
                           sizeof (objinetCidrRouteMetric3Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric4
*
* @purpose Get 'inetCidrRouteMetric4'
*
* @description [inetCidrRouteMetric4]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric4 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteMetric4Value;
  FPOBJ_TRACE_ENTER (bufp);

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */
  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteMetric4Get(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteMetric4Value);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteMetric4 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteMetric4Value,
                           sizeof (objinetCidrRouteMetric4Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric5
*
* @purpose Get 'inetCidrRouteMetric5'
*
* @description [inetCidrRouteMetric5]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteMetric5 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteMetric5Value;
  FPOBJ_TRACE_ENTER (bufp);

    L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 

  xLibU32_t ipAddrRouteDestValue;
  xLibU32_t ipAddrNexthopValue;
  xLibStr256_t tempStrVal;

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  /* get the value from application */

   /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- keyinetCidrRoutePfxLenValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteDestValue, tempStrVal);
  memcpy (&ipAddrRouteDestValue, tempStrVal, sizeof (ipAddrRouteDestValue));
  ipAddrRouteDestValue = osapiHtonl (ipAddrRouteDestValue);

  memset(tempStrVal,0x00,sizeof(tempStrVal));  
  osapiInetPton (L7_AF_INET, keyinetCidrRouteNextHopValue, tempStrVal);
  memcpy (&ipAddrNexthopValue, tempStrVal, sizeof (ipAddrNexthopValue));
  ipAddrNexthopValue = osapiHtonl (ipAddrNexthopValue);

  /* routeTos is passed just to use below function call */
  owa.l7rc = usmDbIpCidrRouteMetric5Get(USMDB_UNIT_CURRENT,ipAddrRouteDestValue,netMask,
                                  routeTos,ipAddrNexthopValue,&objinetCidrRouteMetric5Value);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteMetric5 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteMetric5Value,
                           sizeof (objinetCidrRouteMetric5Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basicrouteinetCidrRoute_inetCidrRouteStatus
*
* @purpose Get 'inetCidrRouteStatus'
*
* @description [inetCidrRouteStatus]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basicrouteinetCidrRoute_inetCidrRouteStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteDestTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteDestValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRoutePfxLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRoutePolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinetCidrRouteNextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyinetCidrRouteNextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinetCidrRouteStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: inetCidrRouteDestType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDestType,
                           (xLibU8_t *) & keyinetCidrRouteDestTypeValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteDestTypeValue, kwa1.len);

  /* retrieve key: inetCidrRouteDest */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteDest,
                           (xLibU8_t *) keyinetCidrRouteDestValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteDestValue, kwa2.len);

  /* retrieve key: inetCidrRoutePfxLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePfxLen,
                           (xLibU8_t *) & keyinetCidrRoutePfxLenValue,
                           &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRoutePfxLenValue, kwa3.len);

  /* retrieve key: inetCidrRoutePolicy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRoutePolicy,
                           (xLibU8_t *) keyinetCidrRoutePolicyValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRoutePolicyValue, kwa4.len);

  /* retrieve key: inetCidrRouteNextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHopType,
                           (xLibU8_t *) & keyinetCidrRouteNextHopTypeValue,
                           &kwa5.len);
  if (kwa5.l7rc != L7_SUCCESS)
  {
    kwa5.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa5);
    return kwa5.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinetCidrRouteNextHopTypeValue, kwa5.len);

  /* retrieve key: inetCidrRouteNextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_basicrouteinetCidrRoute_inetCidrRouteNextHop,
                           (xLibU8_t *) keyinetCidrRouteNextHopValue,
                           &kwa6.len);
  if (kwa6.l7rc != L7_SUCCESS)
  {
    kwa6.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa6);
    return kwa6.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyinetCidrRouteNextHopValue, kwa6.len);

  objinetCidrRouteStatusValue = L7_ROW_STATUS_ACTIVE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: inetCidrRouteStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinetCidrRouteStatusValue,
                           sizeof (objinetCidrRouteStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
