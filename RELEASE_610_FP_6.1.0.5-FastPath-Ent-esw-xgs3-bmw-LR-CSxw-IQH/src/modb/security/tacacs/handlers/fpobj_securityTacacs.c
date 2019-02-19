/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securityTacacs.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to tacacs-object.xml
*
* @create  23 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "usmdb_util_api.h"
#include "_xe_securityTacacs_obj.h"
#include "_xe_securityTacacsWeb_obj.h"
#include "usmdb_tacacs_api.h"
#include "osapi_support.h"

/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit unsigned integer
*          the value is >= 0 and <= 4294967295.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-numeric character
*              is included in buf.
*
* @notes This f(x) checks each letter inside the buf to make certain
*         it is an integer.  It initially verifies the number of digits
*         does not exceed the number of digits in Max 32 bit unsigned int.
*         Start with the last digit, convert each ascii character into its
*         integer equivalent.  Multiply each consecutive digit by the next
*         power of 10.  Verify adding the new digit to the old sum will not
*         exceed MAXINT.  If so, this indicates the value is too large to be
*         represented by a 32 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t fpObjUtil_securityTacacs_ConvertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{

  L7_int32 f;
  L7_int32 j;
  L7_uint32 tempval;
#define MAXUINT 4294967295U
#define MAXMSB 4

  if ( buf[0] == '-' )
    return L7_FAILURE;

  j = strlen(buf);

  if ( j > 10 )
    return L7_FAILURE;
  
  if( (strlen(buf)== 10))
  {
    tempval = (L7_uint32)(buf[0] -'0');
    if (tempval > MAXMSB)
    {
      return L7_FAILURE;
    }
  }

  for ( *pVal = 0, f=1, j=j-1; j >= 0; j--, f=f*10)
  {
    tempval = (L7_uint32)(buf[j] -'0');

    if ( ( tempval > 9 ) || ( tempval < 0 ) )
      return L7_FAILURE;

    tempval = tempval * f;

    if ( (MAXUINT - tempval) < *pVal )
      return L7_FAILURE;
    else
      *pVal = *pVal + tempval;
  }

  return L7_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsServerIpAddrType
*
* @purpose Get 'TacacsServerIpAddrType'
 *@description  [TacacsServerIpAddrType] The type of address of the TACACS
* server. IPv4 and DNS types are supported currently   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsServerIpAddrType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsServerIpAddrTypeValue;
  xLibU32_t nextObjTacacsServerIpAddrTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddrType */
  owa.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                          (xLibU8_t *) & objTacacsServerIpAddrTypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjTacacsServerIpAddrTypeValue = L7_IP_ADDRESS_TYPE_IPV4;
  	owa.l7rc = L7_SUCCESS;
  }
  else
  {
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objTacacsServerIpAddrTypeValue, owa.len);
	if(objTacacsServerIpAddrTypeValue == L7_IP_ADDRESS_TYPE_IPV4)
	{
	  nextObjTacacsServerIpAddrTypeValue = L7_IP_ADDRESS_TYPE_DNS;
	  owa.l7rc = L7_SUCCESS;
	}
	else
	{
	  owa.l7rc = L7_FAILURE;
	}
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTacacsServerIpAddrTypeValue, owa.len);

  /* return the object value: TacacsServerIpAddrType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTacacsServerIpAddrTypeValue,
                           sizeof (nextObjTacacsServerIpAddrTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsServerIpAddress
*
* @purpose Get 'TacacsServerIpAddress'
 *@description  [TacacsServerIpAddress] The IP address or HostName of the TACACS
* server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsServerIpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaTacacsServerIpAddrType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t tempTacacsServerIpAddrTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsServerIpAddressValue;
  xLibStr256_t nextObjTacacsServerIpAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddrType */
  kwaTacacsServerIpAddrType.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                                                (xLibU8_t *) &keyTacacsServerIpAddrTypeValue,
                                                &kwaTacacsServerIpAddrType.len);
  if (kwaTacacsServerIpAddrType.rc != XLIBRC_SUCCESS)
  {
    kwaTacacsServerIpAddrType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTacacsServerIpAddrType);
    return kwaTacacsServerIpAddrType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwaTacacsServerIpAddrType.len);
  
  memset(objTacacsServerIpAddressValue,0x00,sizeof(objTacacsServerIpAddressValue));
  memset(nextObjTacacsServerIpAddressValue,0x00,sizeof(nextObjTacacsServerIpAddressValue));
  /* retrieve key: TacacsServerIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                          (xLibU8_t *) objTacacsServerIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(nextObjTacacsServerIpAddressValue, 0x00,
		                  sizeof(nextObjTacacsServerIpAddressValue)) ;

  	owa.l7rc = usmDbTacacsHostNameServerFirstGet(&tempTacacsServerIpAddrTypeValue,
                                                   nextObjTacacsServerIpAddressValue);
  	if (owa.l7rc == L7_SUCCESS)
    {
      if(tempTacacsServerIpAddrTypeValue != keyTacacsServerIpAddrTypeValue)
      {
        do
       	{
       	  strcpy(objTacacsServerIpAddressValue,nextObjTacacsServerIpAddressValue);
	  		  memset(nextObjTacacsServerIpAddressValue, 0x00,
					                sizeof(nextObjTacacsServerIpAddressValue));
       	  owa.l7rc = usmDbTacacsHostNameServerNextGet(objTacacsServerIpAddressValue,
                                                &tempTacacsServerIpAddrTypeValue, 
                                                nextObjTacacsServerIpAddressValue);
		  	  if ((owa.l7rc != L7_SUCCESS) ||
    					(tempTacacsServerIpAddrTypeValue == keyTacacsServerIpAddrTypeValue))
  			  {
	  		    break;
		  	  }
       	} while(L7_TRUE);
      }
	  }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objTacacsServerIpAddressValue, owa.len);
	  tempTacacsServerIpAddrTypeValue = keyTacacsServerIpAddrTypeValue;
  	do
	  {
	    owa.l7rc = usmDbTacacsHostNameServerNextGet(objTacacsServerIpAddressValue,
                                                  &tempTacacsServerIpAddrTypeValue, 
                                                  nextObjTacacsServerIpAddressValue);
  	  if ((owa.l7rc != L7_SUCCESS) || 
    			(keyTacacsServerIpAddrTypeValue == tempTacacsServerIpAddrTypeValue))
  	  {
	      break;
  	  }
	    strcpy(objTacacsServerIpAddressValue,nextObjTacacsServerIpAddressValue);
  	  memset(nextObjTacacsServerIpAddressValue, 0x00,
					                sizeof(nextObjTacacsServerIpAddressValue));
	  }while(L7_TRUE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjTacacsServerIpAddressValue, owa.len);

  /* return the object value: TacacsServerIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjTacacsServerIpAddressValue,
                           strlen (nextObjTacacsServerIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsPortNumber
*
* @purpose Get 'TacacsPortNumber'
*
* @description [TacacsPortNumber]: The port number of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsPortNumber (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsPortNumberValue=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNamePortNumberGet (
                                      keyTacacsServerIpAddrTypeValue,
                                      keyTacacsServerIpAddressValue,
                                      (xLibU16_t *)&objTacacsPortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsPortNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsPortNumberValue,
                           sizeof (objTacacsPortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsPortNumber
*
* @purpose Set 'TacacsPortNumber'
*
* @description [TacacsPortNumber]: The port number of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsPortNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsPortNumberValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsPortNumber */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsPortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsPortNumberValue, owa.len);

  /* retrieve key: TacacsServerIpAddress */
  
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNamePortNumberSet (
                                      keyTacacsServerIpAddrTypeValue,
                                      keyTacacsServerIpAddressValue,
                                      (xLibU16_t) objTacacsPortNumberValue);
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
* @function fpObjGet_securityTacacs_TacacsTimeOut
*
* @purpose Get 'TacacsTimeOut'
*
* @description [TacacsTimeOut]: The time out value for the TACACS server.
*              If not set the value of TacacsGlobalTimeout will be used 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsTimeOut (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU8_t objTacacsTimeOutValue=0;
  xLibStr6_t tempValueStr;
  L7_BOOL useGlobal;
  memset(tempValueStr,0x00,sizeof(tempValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
  
  if(usmDbTacacsHostNameServerUseGlobalTimeoutGet(keyTacacsServerIpAddrTypeValue,
  	                              keyTacacsServerIpAddressValue, &useGlobal) == L7_SUCCESS)
  {
    if(useGlobal)
    {
      owa.l7rc = usmDbTacacsGblTimeOutGet(&objTacacsTimeOutValue);
    }
    else
    {
      owa.l7rc = usmDbTacacsHostNameTimeOutGet(keyTacacsServerIpAddrTypeValue,
	  	keyTacacsServerIpAddressValue, &objTacacsTimeOutValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: TacacsTimeOut */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsTimeOutValue,
                           sizeof (objTacacsTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsTimeOut
*
* @purpose Set 'TacacsTimeOut'
*
* @description [TacacsTimeOut]: The time out value for the TACACS server.
*              If not set the value of TacacsGlobalTimeout will be used 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsTimeOut (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU8_t objTacacsTimeOutValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsTimeOut */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsTimeOutValue, owa.len);


  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* set the value in application */
  
  owa.l7rc =
      usmDbTacacsHostNameTimeOutSet (
                                   keyTacacsServerIpAddrTypeValue,
                                   keyTacacsServerIpAddressValue,
                                   objTacacsTimeOutValue);
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
* @function fpObjSet_securityTacacs_TacacsKey
*
* @purpose Set 'TacacsKey'
*
* @description [TacacsKey]: The authentication key for the TACACS server.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objTacacsKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTacacsKeyValue, owa.len);



  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNameKeySet ( 
                               keyTacacsServerIpAddrTypeValue,
                               keyTacacsServerIpAddressValue,
                               objTacacsKeyValue);
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
* @function fpObjGet_securityTacacs_TacacsPriority
*
* @purpose Get 'TacacsPriority'
*
* @description [TacacsPriority]: The priority of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsPriority (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsPriorityValue=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNamePriorityGet (
                                    keyTacacsServerIpAddrTypeValue,
                                    keyTacacsServerIpAddressValue,
                                    (xLibU16_t *)&objTacacsPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsPriorityValue,
                           sizeof (objTacacsPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsPriority
*
* @purpose Set 'TacacsPriority'
*
* @description [TacacsPriority]: The priority of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsPriorityValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsPriorityValue, owa.len);


  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNamePrioritySet (
                                    keyTacacsServerIpAddrTypeValue,
					 keyTacacsServerIpAddressValue,
  					 (xLibU16_t)objTacacsPriorityValue);
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
* @function fpObjGet_securityTacacs_TacacsSourceIP
*
* @purpose Get 'TacacsSourceIP'
*
* @description [TacacsSourceIP]: IP Address to be used as Source IP for Tacacs
*              requests 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsSourceIP (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsSourceIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsSourceIpAddrGet ( keyTacacsServerIpAddrTypeValue,
                                keyTacacsServerIpAddressValue,
                                &objTacacsSourceIPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsSourceIP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsSourceIPValue,
                           sizeof (objTacacsSourceIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsSourceIP
*
* @purpose Set 'TacacsSourceIP'
*
* @description [TacacsSourceIP]: IP Address to be used as Source IP for Tacacs
*              requests 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsSourceIP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsSourceIPValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: TacacsSourceIP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsSourceIPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsSourceIPValue, owa.len);


  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* set the value in application */
  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsUseGlobalKey
*
* @purpose Get 'TacacsUseGlobalKey'
*
* @description [TacacsUseGlobalKey]: Use the Global Key for this Tacacs Server
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsUseGlobalKey (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsUseGlobalKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNameServerUseGlobalKeyGet (
                                              keyTacacsServerIpAddrTypeValue,
                                              keyTacacsServerIpAddressValue,
                                              &objTacacsUseGlobalKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsUseGlobalKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsUseGlobalKeyValue,
                           sizeof (objTacacsUseGlobalKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsUseGlobalTimeout
*
* @purpose Get 'TacacsUseGlobalTimeout'
*
* @description [TacacsUseGlobalTimeout]: Use the Global Timeout for this Tacacs
*              Server 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsUseGlobalTimeout (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsUseGlobalTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNameServerUseGlobalTimeoutGet (
                                                  keyTacacsServerIpAddrTypeValue,
                                                  keyTacacsServerIpAddressValue,
                                                  &objTacacsUseGlobalTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsUseGlobalTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsUseGlobalTimeoutValue,
                           sizeof (objTacacsUseGlobalTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsUseGlobalSourceIP
*
* @purpose Get 'TacacsUseGlobalSourceIP'
*
* @description [TacacsUseGlobalSourceIP]: Use the Global SourceIP for this
*              Tacacs Server 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsUseGlobalSourceIP (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  owa.rc = XLIBRC_NOT_SUPPORTED;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;  
}


/*******************************************************************************
* @function fpObjGet_securityTacacs_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: The row status of this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
 kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: The row status of this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddrTemp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_TACACS_IP_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_TACACS_IP_TYPE_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {

    if(keyTacacsServerIpAddrTypeValue == L7_IP_ADDRESS_TYPE_IPV4)
    {
       /*check if entered dotted format is valid */
	ipAddrTemp = osapiInet_addr(keyTacacsServerIpAddressValue);
	if(ipAddrTemp <= 0)
	{
      		owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;  /* TODO: Change if required */
      		FPOBJ_TRACE_EXIT (bufp, owa);
      		return owa.rc;
       }
    }
    else if(keyTacacsServerIpAddrTypeValue != L7_IP_ADDRESS_TYPE_DNS)
    {
      owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbTacacsHostNameServerAdd (keyTacacsServerIpAddrTypeValue, 
		               keyTacacsServerIpAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TACACS_SERVER_ADD_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbTacacsHostNameServerRemove (keyTacacsServerIpAddrTypeValue,
                                keyTacacsServerIpAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TACACS_SERVER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if(objRowStatusValue != L7_ROW_STATUS_ACTIVE)
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  
  return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsGlobalKey
*
* @purpose Get 'TacacsGlobalKey'
*
* @description [TacacsGlobalKey]:The Encryption and Authentication key used
*              in communication with TACACS servers .
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsGlobalKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsGlobalKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsGblKeyGet (objTacacsGlobalKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalKeyValue,
                     strlen (objTacacsGlobalKeyValue));

  /* return the object value: TacacsGlobalTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsGlobalKeyValue,
                           strlen (objTacacsGlobalKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsGlobalKey
*
* @purpose Set 'TacacsGlobalKey'
*
* @description [TacacsGlobalKey]: The Encryption and Authentication key used
*              in communication with TACACS servers . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsGlobalKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsGlobalKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsGlobalKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objTacacsGlobalKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTacacsGlobalKeyValue, owa.len);

  if(strlen(objTacacsGlobalKeyValue) > 128)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbTacacsGblKeySet ( objTacacsGlobalKeyValue);
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
* @function fpObjGet_securityTacacs_TacacsGlobalTimeout
*
* @purpose Get 'TacacsGlobalTimeout'
*
* @description [TacacsGlobalTimeout]: The time-out in seconds for communication
*              with TACACS servers . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsGlobalTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t objTacacsGlobalTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsGblTimeOutGet (&objTacacsGlobalTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalTimeoutValue,
                     sizeof (objTacacsGlobalTimeoutValue));

  /* return the object value: TacacsGlobalTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsGlobalTimeoutValue,
                           sizeof (objTacacsGlobalTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsGlobalTimeout
*
* @purpose Set 'TacacsGlobalTimeout'
*
* @description [TacacsGlobalTimeout]: The time-out in seconds for communication
*              with TACACS servers . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsGlobalTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsGlobalTimeoutValue;
  L7_uchar8 timeOutVal;
  timeOutVal = 0x00;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsGlobalTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsGlobalTimeoutValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalTimeoutValue, owa.len);

  /* set the value in application */

  timeOutVal = (L7_char8)objTacacsGlobalTimeoutValue;
  owa.l7rc =
    usmDbTacacsGblTimeOutSet ( objTacacsGlobalTimeoutValue);
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
* @function fpObjGet_securityTacacs_TacacsGlobalSourceIP
*
* @purpose Get 'TacacsGlobalSourceIP'
*
* @description [TacacsGlobalSourceIP]: IP Address to be used as Source IP
*              for Tacacs requests 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacs_TacacsGlobalSourceIP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsGlobalSourceIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsGblSrcIpAddrGet ( &objTacacsGlobalSourceIPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalSourceIPValue,
                     sizeof (objTacacsGlobalSourceIPValue));

  /* return the object value: TacacsGlobalSourceIP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsGlobalSourceIPValue,
                           sizeof (objTacacsGlobalSourceIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsGlobalSourceIP
*
* @purpose Set 'TacacsGlobalSourceIP'
*
* @description [TacacsGlobalSourceIP]: IP Address to be used as Source IP
*              for Tacacs requests 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacs_TacacsGlobalSourceIP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsGlobalSourceIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsGlobalSourceIP */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsGlobalSourceIPValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalSourceIPValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTacacsGblSrcIpAddrSet ( objTacacsGlobalSourceIPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/********************* Handlers for the Web objects********************************************/


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsGlobalKey
*
* @purpose Set 'TacacsGlobalKey'
*
* @description [TacacsGlobalKey]: The Encryption and Authentication key used
*              in communication with TACACS servers . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsGlobalKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsGlobalKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsGlobalKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objTacacsGlobalKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTacacsGlobalKeyValue, owa.len);

  if(strlen(objTacacsGlobalKeyValue) > 128)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbTacacsGblKeySet ( objTacacsGlobalKeyValue);
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
* @function fpObjGet_securityTacacs_TacacsGlobalTimeout
*
* @purpose Get 'TacacsGlobalTimeout'
*
* @description [TacacsGlobalTimeout]: The time-out in seconds for communication
*              with TACACS servers . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsGlobalTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t objTacacsGlobalTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsGblTimeOutGet (&objTacacsGlobalTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalTimeoutValue,
                     sizeof (objTacacsGlobalTimeoutValue));

  /* return the object value: TacacsGlobalTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsGlobalTimeoutValue,
                           sizeof (objTacacsGlobalTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsGlobalTimeout
*
* @purpose Set 'TacacsGlobalTimeout'
*
* @description [TacacsGlobalTimeout]: The time-out in seconds for communication
*              with TACACS servers . 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsGlobalTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsGlobalTimeoutValue;
  L7_uchar8 timeOutVal;
  timeOutVal = 0x00;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsGlobalTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsGlobalTimeoutValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsGlobalTimeoutValue, owa.len);

  /* set the value in application */

  timeOutVal = (L7_char8)objTacacsGlobalTimeoutValue;
  owa.l7rc =
    usmDbTacacsGblTimeOutSet ( objTacacsGlobalTimeoutValue);
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
* @function fpObjGet_securityTacacs_TacacsServerIpAddress
*
* @purpose Get 'TacacsServerIpAddress'
 *@description  [TacacsServerIpAddress] The IP address or HostName of the TACACS
* server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsServerIpAddress (void *wap, void *bufp)
{

  xLibU32_t ipAddr = L7_NULL;
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsServerIpAddressValue;
  xLibStr256_t nextObjTacacsServerIpAddressValue;
  FPOBJ_TRACE_ENTER (bufp);
#if 0
  /* retrieve key: TacacsServerIpAddrType */
  kwaTacacsServerIpAddrType.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddrType,
                                                (xLibU8_t *) &keyTacacsServerIpAddrTypeValue,
                                                &kwaTacacsServerIpAddrType.len);
  if (kwaTacacsServerIpAddrType.rc != XLIBRC_SUCCESS)
  {
    kwaTacacsServerIpAddrType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTacacsServerIpAddrType);
    return kwaTacacsServerIpAddrType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwaTacacsServerIpAddrType.len);
#endif
  
  memset(objTacacsServerIpAddressValue,0x00,sizeof(objTacacsServerIpAddressValue));
  memset(nextObjTacacsServerIpAddressValue,0x00,sizeof(nextObjTacacsServerIpAddressValue));
  /* retrieve key: TacacsServerIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                          (xLibU8_t *) objTacacsServerIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(nextObjTacacsServerIpAddressValue, 0x00,
		                  sizeof(nextObjTacacsServerIpAddressValue)) ;

    type = L7_IP_ADDRESS_TYPE_IPV4;

  	owa.l7rc = usmDbTacacsHostNameServerFirstGet(&type,
                                                   nextObjTacacsServerIpAddressValue);
  	if (owa.l7rc != L7_SUCCESS)
    {
          type = L7_IP_ADDRESS_TYPE_DNS;
	  		  memset(nextObjTacacsServerIpAddressValue, 0x00,
					                sizeof(nextObjTacacsServerIpAddressValue));

  	      owa.l7rc = usmDbTacacsHostNameServerFirstGet(&type,
                                                   nextObjTacacsServerIpAddressValue);
	  }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objTacacsServerIpAddressValue, owa.len);

   if (usmDbIPHostAddressValidate(objTacacsServerIpAddressValue, &ipAddr,
                                       &type) != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

  	do
	  {
	    owa.l7rc = usmDbTacacsHostNameServerNextGet(objTacacsServerIpAddressValue,
                                                &type,
                                                  nextObjTacacsServerIpAddressValue);

      if( (owa.l7rc!=L7_SUCCESS) && (type == L7_IP_ADDRESS_TYPE_IPV4))
      {
	        owa.l7rc = usmDbTacacsHostNameServerNextGet(objTacacsServerIpAddressValue,
                                                &type,
                                                  nextObjTacacsServerIpAddressValue);
          type = L7_IP_ADDRESS_TYPE_DNS;

      }
	    strcpy(objTacacsServerIpAddressValue,nextObjTacacsServerIpAddressValue);
   } while(0);

  }

  if ( (owa.l7rc != L7_SUCCESS) && (type == L7_IP_ADDRESS_TYPE_DNS ))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjTacacsServerIpAddressValue, owa.len);

  /* return the object value: TacacsServerIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjTacacsServerIpAddressValue,
                           strlen (nextObjTacacsServerIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsPortNumber
*
* @purpose Get 'TacacsPortNumber'
*
* @description [TacacsPortNumber]: The port number of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsPortNumber (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU16_t objTacacsPortNumberValue=0;

  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;
  xLibU32_t ipAddr = L7_NULL;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &type) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

  
  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNamePortNumberGet (
                                       type,
                                      keyTacacsServerIpAddressValue,
                                      &objTacacsPortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsPortNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsPortNumberValue,
                           sizeof (objTacacsPortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsPortNumber
*
* @purpose Set 'TacacsPortNumber'
*
* @description [TacacsPortNumber]: The port number of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsPortNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsPortNumberValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsPortNumber */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsPortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsPortNumberValue, owa.len);

  /* retrieve key: TacacsServerIpAddress */
  
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

#endif
 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNamePortNumberSet (
                                      keyTacacsServerIpAddrTypeValue,
                                      keyTacacsServerIpAddressValue,
                                      (xLibU16_t) objTacacsPortNumberValue);
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
* @function fpObjGet_securityTacacs_TacacsTimeOut
*
* @purpose Get 'TacacsTimeOut'
*
* @description [TacacsTimeOut]: The time out value for the TACACS server.
*              If not set the value of TacacsGlobalTimeout will be used 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsTimeOut (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU8_t objTacacsTimeOutValue=0;
  xLibStr6_t tempValueStr;
  L7_BOOL useGlobal;
  xLibU32_t ipAddr = L7_NULL;
  memset(tempValueStr,0x00,sizeof(tempValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);
#if 0
  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif
 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

  
  if(usmDbTacacsHostNameServerUseGlobalTimeoutGet(keyTacacsServerIpAddrTypeValue,
  	                              keyTacacsServerIpAddressValue, &useGlobal) == L7_SUCCESS)
  {
    if(useGlobal)
    {
      owa.l7rc = usmDbTacacsGblTimeOutGet(&objTacacsTimeOutValue);
    }
    else
    {
      owa.l7rc = usmDbTacacsHostNameTimeOutGet(keyTacacsServerIpAddrTypeValue,
	  	keyTacacsServerIpAddressValue, &objTacacsTimeOutValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: TacacsTimeOut */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsTimeOutValue,
                           sizeof (objTacacsTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsTimeOut
*
* @purpose Set 'TacacsTimeOut'
*
* @description [TacacsTimeOut]: The time out value for the TACACS server.
*              If not set the value of TacacsGlobalTimeout will be used 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsTimeOut (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsTimeOutValue;
  xLibU8_t  objTacacsTimeOutValue1;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsTimeOut */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsTimeOutValue, owa.len);

  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


  /* set the value in application */
  objTacacsTimeOutValue1 = ( xLibU8_t) objTacacsTimeOutValue;
  owa.l7rc =
      usmDbTacacsHostNameTimeOutSet (
                                   keyTacacsServerIpAddrTypeValue,
                                   keyTacacsServerIpAddressValue,
                                   objTacacsTimeOutValue1);
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
* @function fpObjSet_securityTacacs_TacacsTimeOut
*
* @purpose Set 'TacacsTimeOut'
*
* @description [TacacsTimeOut]: The time out value for the TACACS server.
*              If not set the value of TacacsGlobalTimeout will be used 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsTimeOutDefault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU8_t objTacacsTimeOutValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsTimeOut */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsTimeOutValue, owa.len);


  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


  /* set the value in application */
  
  owa.l7rc = usmDbTacacsGblTimeOutGet(&objTacacsTimeOutValue);
 
  owa.l7rc =
      usmDbTacacsHostNameTimeOutSet (
                                   keyTacacsServerIpAddrTypeValue,
                                   keyTacacsServerIpAddressValue,
                                   objTacacsTimeOutValue);
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
* @function fpObjGet_securityTacacsWeb_TacacsKey
*
* @purpose Get 'TacacsKey'
*
* @description [TacacsKey]:The authentication key for the TACACS server.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsKey (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsKeyValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }
   /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNameKeyGet (
                                    keyTacacsServerIpAddrTypeValue,
                                    keyTacacsServerIpAddressValue,
                                    objTacacsKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsKeyValue,
                           sizeof (objTacacsKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsKey
*
* @purpose Set 'TacacsKey'
*
* @description [TacacsKey]: The authentication key for the TACACS server.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objTacacsKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTacacsKeyValue, owa.len);



  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0
  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif


 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNameKeySet ( 
                               keyTacacsServerIpAddrTypeValue,
                               keyTacacsServerIpAddressValue,
                               objTacacsKeyValue);
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
* @function fpObjSet_securityTacacs_TacacsKey
*
* @purpose Set 'TacacsKey'
*
* @description [TacacsKey]: The authentication key for the TACACS server.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsKeyDefault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTacacsKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objTacacsKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTacacsKeyValue, owa.len);



  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0
  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif


 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

 owa.l7rc= usmDbTacacsGblKeySet(objTacacsKeyValue);

  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNameKeySet ( 
                               keyTacacsServerIpAddrTypeValue,
                               keyTacacsServerIpAddressValue,
                               objTacacsKeyValue);
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
* @function fpObjGet_securityTacacs_TacacsPriority
*
* @purpose Get 'TacacsPriority'
*
* @description [TacacsPriority]: The priority of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsPriority (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU16_t objTacacsPriorityValue=0;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNamePriorityGet (
                                    keyTacacsServerIpAddrTypeValue,
                                    keyTacacsServerIpAddressValue,
                                    &objTacacsPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsPriorityValue,
                           sizeof (objTacacsPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_TacacsPriority
*
* @purpose Set 'TacacsPriority'
*
* @description [TacacsPriority]: The priority of the TACACS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_TacacsPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsPriorityValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TacacsPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTacacsPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTacacsPriorityValue, owa.len);


  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


  /* set the value in application */
  owa.l7rc =
    usmDbTacacsHostNamePrioritySet (
                                    keyTacacsServerIpAddrTypeValue,
					 keyTacacsServerIpAddressValue,
  					 (xLibU16_t)objTacacsPriorityValue);
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
* @function fpObjGet_securityTacacs_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus]: The row status of this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
 memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
 kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);
#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityTacacs_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus]: The row status of this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityTacacsWeb_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  xLibU32_t ipAddrTemp;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: TacacsServerIpAddress */
  memset(keyTacacsServerIpAddressValue,0,sizeof( xLibStr256_t) );
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc =XLIBRC_TACACS_IP_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

#if 0

  /* retrieve key: TacacsServerIpAddrType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_securityTacacs_TacacsServerIpAddrType,
                           (xLibU8_t *) & keyTacacsServerIpAddrTypeValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTacacsServerIpAddrTypeValue, kwa2.len);

#endif

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

  /* call the usmdb only for add and delete */
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {

    if(keyTacacsServerIpAddrTypeValue == L7_IP_ADDRESS_TYPE_IPV4)
    {
       /*check if entered dotted format is valid */
	ipAddrTemp = osapiInet_addr(keyTacacsServerIpAddressValue);
	if(ipAddrTemp <= 0)
	{
      		owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;  /* TODO: Change if required */
      		FPOBJ_TRACE_EXIT (bufp, owa);
      		return owa.rc;
       }
    }
    else if(keyTacacsServerIpAddrTypeValue != L7_IP_ADDRESS_TYPE_DNS)
    {
      owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbTacacsHostNameServerAdd (keyTacacsServerIpAddrTypeValue, 
		               keyTacacsServerIpAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TACACS_SERVER_ADD_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbTacacsHostNameServerRemove (keyTacacsServerIpAddrTypeValue,
                                keyTacacsServerIpAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TACACS_SERVER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if(objRowStatusValue != L7_ROW_STATUS_ACTIVE)
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsUseGlobalKey
*
* @purpose Get 'TacacsUseGlobalKey'
*
* @description [TacacsUseGlobalKey]: Use the Global Key for this Tacacs Server
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsUseGlobalKey (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsUseGlobalKeyValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

   if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNameServerUseGlobalKeyGet (
                                              keyTacacsServerIpAddrTypeValue,
                                              keyTacacsServerIpAddressValue,
                                              &objTacacsUseGlobalKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsUseGlobalKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsUseGlobalKeyValue,
                           sizeof (objTacacsUseGlobalKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_securityTacacs_TacacsUseGlobalTimeout
*
* @purpose Get 'TacacsUseGlobalTimeout'
*
* @description [TacacsUseGlobalTimeout]: Use the Global Timeout for this Tacacs
*              Server 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityTacacsWeb_TacacsUseGlobalTimeout (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyTacacsServerIpAddressValue;
  xLibU32_t keyTacacsServerIpAddrTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTacacsUseGlobalTimeoutValue;
  xLibU32_t ipAddr = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TacacsServerIpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_securityTacacsWeb_TacacsServerIpAddress,
                           (xLibU8_t *) keyTacacsServerIpAddressValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyTacacsServerIpAddressValue, kwa1.len);

 if (usmDbIPHostAddressValidate(keyTacacsServerIpAddressValue, &ipAddr,
                                     &keyTacacsServerIpAddrTypeValue) != L7_SUCCESS)
 {
   owa.rc = XLIBRC_TACACS_INVALID_IP_ADDRESS;    /* TODO: Change if required */
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }

  /* get the value from application */
  owa.l7rc =
    usmDbTacacsHostNameServerUseGlobalTimeoutGet (
                                                  keyTacacsServerIpAddrTypeValue,
                                                  keyTacacsServerIpAddressValue,
                                                  &objTacacsUseGlobalTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TacacsUseGlobalTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTacacsUseGlobalTimeoutValue,
                           sizeof (objTacacsUseGlobalTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_securityTacacsWeb_TacacsUseGlobalTimeout (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
xLibRC_t fpObjSet_securityTacacsWeb_TacacsUseGlobalKey (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
