
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDHCPSnooping.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  05 May 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingDHCPSnooping_obj.h"
#include "usmdb_dhcp_snooping.h"
#include "dot1q_exports.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description [AdminMode] The DHCP Snooping Admin mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_AdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDsAdminModeGet ( &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue, sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description [AdminMode] The DHCP Snooping Admin mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_AdminMode (void *wap, void *bufp)
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

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDsAdminModeSet ( objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_RemoteFileName
*
* @purpose Get 'RemoteFileName'
*
* @description [RemoteFileName] Remote file name to store and restore the DHCP SNOOPING database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_RemoteFileName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRemoteFileNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objRemoteFileNameValue, 0x00, sizeof(objRemoteFileNameValue));

  /* get the value from application */
  owa.l7rc = usmDbDsDbFileNameGet ( objRemoteFileNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objRemoteFileNameValue, strlen (objRemoteFileNameValue));

  /* return the object value: RemoteFileName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRemoteFileNameValue,
                           strlen (objRemoteFileNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_RemoteFileName
*
* @purpose Set 'RemoteFileName'
*
* @description [RemoteFileName] Remote file name to store and restore the DHCP SNOOPING database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_RemoteFileName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRemoteFileNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RemoteFileName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objRemoteFileNameValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRemoteFileNameValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDsDbFileNameSet ( objRemoteFileNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_RemoteIPAddress
*
* @purpose Get 'RemoteIPAddress'
*
* @description [RemoteIPAddress] Remote Ip Addr to store and restore the DHCP SNOOPING database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_RemoteIPAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemoteIPAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDsDbIpAddrGet ( &objRemoteIPAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRemoteIPAddressValue, sizeof (objRemoteIPAddressValue));

  /* return the object value: RemoteIPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRemoteIPAddressValue,
                           sizeof (objRemoteIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_RemoteIPAddress
*
* @purpose Set 'RemoteIPAddress'
*
* @description [RemoteIPAddress] Remote Ip Addr to store and restore the DHCP SNOOPING database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_RemoteIPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemoteIPAddressValue;
  fpObjWa_t owaFileName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t remoteFileNameValue;

  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RemoteIPAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRemoteIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRemoteIPAddressValue, owa.len);

  owaFileName.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_RemoteFileName,
                                    (xLibU8_t *)remoteFileNameValue , &owaFileName.len);
  if(owaFileName.rc == XLIBRC_SUCCESS)
  {
    if(usmDbDsDbFileNameSet ( remoteFileNameValue) != L7_SUCCESS)
    {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       return owa.rc;
    }
  }

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDsDbIpAddrSet ( objRemoteIPAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_WriteDelay
*
* @purpose Get 'WriteDelay'
*
* @description [WriteDelay] write delay interval to store the DHCP SNOOPING database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_WriteDelay (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objWriteDelayValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDsDbStoreIntervalGet ( &objWriteDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objWriteDelayValue, sizeof (objWriteDelayValue));

  /* return the object value: WriteDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objWriteDelayValue, sizeof (objWriteDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_WriteDelay
*
* @purpose Set 'WriteDelay'
*
* @description [WriteDelay] write delay interval to store the DHCP SNOOPING database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_WriteDelay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objWriteDelayValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: WriteDelay */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objWriteDelayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objWriteDelayValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDsDbStoreIntervalSet ( objWriteDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_StaticBindingsCount
*
* @purpose Get 'StaticBindingsCount'
*
* @description [StaticBindingsCount] The number of DHCP snooping static bindings
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_StaticBindingsCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStaticBindingsCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objStaticBindingsCountValue = usmDbDsStaticBindingsCount ();
  owa.l7rc = L7_SUCCESS;
 
  FPOBJ_TRACE_VALUE (bufp, &objStaticBindingsCountValue, sizeof (objStaticBindingsCountValue));

  /* return the object value: StaticBindingsCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStaticBindingsCountValue,
                           sizeof (objStaticBindingsCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_VerifyMac
*
* @purpose Get 'VerifyMac'
*
* @description [VerifyMac] whether DHCP snooping verifies source MAC addresses
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_VerifyMac (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVerifyMacValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDsVerifyMacGet ( &objVerifyMacValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objVerifyMacValue, sizeof (objVerifyMacValue));

  /* return the object value: VerifyMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVerifyMacValue, sizeof (objVerifyMacValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_VerifyMac
*
* @purpose Set 'VerifyMac'
*
* @description [VerifyMac] whether DHCP snooping verifies source MAC addresses
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_VerifyMac (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVerifyMacValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VerifyMac */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVerifyMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVerifyMacValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDsVerifyMacSet ( objVerifyMacValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface] Get the DHCP Snooping Interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_Interface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjInterfaceValue = 0;
    objInterfaceValue = 0;
  }


  owa.l7rc = L7_FAILURE;
  while (usmDbGetNextVisibleIntIfNumber(objInterfaceValue, &nextObjInterfaceValue) == L7_SUCCESS)
  {
      if (usmDbDsIntfIsValid(nextObjInterfaceValue) == L7_TRUE)
      {
        owa.l7rc = L7_SUCCESS;
        break;
      }
      else
      {
        objInterfaceValue = nextObjInterfaceValue;
      }
  }
  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_LogInvalidPackets
*
* @purpose Get 'LogInvalidPackets'
*
* @description [LogInvalidPackets] whether DHCP snooping logs invalid packets
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_LogInvalidPackets (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLogInvalidPacketsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfLogInvalidGet ( keyInterfaceValue, &objLogInvalidPacketsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogInvalidPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogInvalidPacketsValue,
                           sizeof (objLogInvalidPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_LogInvalidPackets
*
* @purpose Set 'LogInvalidPackets'
*
* @description [LogInvalidPackets] whether DHCP snooping logs invalid packets
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_LogInvalidPackets (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLogInvalidPacketsValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LogInvalidPackets */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLogInvalidPacketsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLogInvalidPacketsValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDsIntfLogInvalidSet ( keyInterfaceValue, objLogInvalidPacketsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_RateLimit
*
* @purpose Get 'RateLimit'
*
* @description [RateLimit] The rate limit value of incoming DHCP packets on untrusted interfaces
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_RateLimit (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objRateLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfRateLimitGet ( keyInterfaceValue, &objRateLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RateLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRateLimitValue, sizeof (objRateLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_RateLimit
*
* @purpose Set 'RateLimit'
*
* @description [RateLimit] The rate limit value of incoming DHCP packets on untrusted interfaces
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_RateLimit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objRateLimitValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RateLimit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRateLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRateLimitValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDsIntfRateLimitSet ( keyInterfaceValue, objRateLimitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/****************************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_NoLimit
*
* @purpose Get 'NoLimit'
*
* @description [NoLimit] The rate limit value of incoming DHCP packets on untrusted interfaces
*
* @notes
*
* @return
*****************************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_NoLimit (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objRateLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfRateLimitGet ( keyInterfaceValue, &objRateLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RateLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRateLimitValue, sizeof (objRateLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_NoLimit
*
* @purpose Set 'NoLimit'
*
* @description [NoLimit] The rate limit value of incoming DHCP packets on untrusted interfaces
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_NoLimit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS32_t objRateLimitValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RateLimit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRateLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRateLimitValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDsIntfRateLimitSet ( keyInterfaceValue, objRateLimitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_MACVerifyFailures
*
* @purpose Get 'MACVerifyFailures'
*
* @description [MACVerifyFailures] Number of DHCP client packets whose source MAC address didn't match the client hardware address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_MACVerifyFailures (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMACVerifyFailuresValue;
  dhcpSnoopIntfStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfStatsGet (keyInterfaceValue,
                                  &stats );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objMACVerifyFailuresValue = stats.macVerify;

  /* return the object value: MACVerifyFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMACVerifyFailuresValue,
                           sizeof (objMACVerifyFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_intfMismatch
*
* @purpose Get 'intfMismatch'
*
* @description [intfMismatch] Number of DHCP client packets received on an interface different from the interface where the DHCP snooping binding says the client should be Only DHCPRELEASE or DHCPDECLINE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_intfMismatch (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintfMismatchValue;
  dhcpSnoopIntfStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfStatsGet (keyInterfaceValue,
                                  &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: intfMismatch */
  objintfMismatchValue = stats.intfMismatch;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objintfMismatchValue,
                           sizeof (objintfMismatchValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_untrustedSvrMsg
*
* @purpose Get 'untrustedSvrMsg'
*
* @description [untrustedSvrMsg] Number of DHCP server messages received on an untrusted port.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_untrustedSvrMsg (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objuntrustedSvrMsgValue;
  dhcpSnoopIntfStats_t stats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfStatsGet (keyInterfaceValue,
                                  &stats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: untrustedSvrMsg */
  objuntrustedSvrMsgValue = stats.untrustedSvrMsg;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objuntrustedSvrMsgValue,
                           sizeof (objuntrustedSvrMsgValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_TrustedState
*
* @purpose Get 'TrustedState'
*
* @description [TrustedState] Gets/Sets whether DHCP snooping considers a port trusted
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_TrustedState (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrustedStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfTrustGet ( keyInterfaceValue, &objTrustedStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TrustedState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrustedStateValue,
                           sizeof (objTrustedStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_TrustedState
*
* @purpose Set 'TrustedState'
*
* @description [TrustedState] Gets/Sets whether DHCP snooping considers a port trusted
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_TrustedState (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTrustedStateValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrustedState */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrustedStateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrustedStateValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDsIntfTrustSet ( keyInterfaceValue, objTrustedStateValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_BurstInterval
*
* @purpose Get 'BurstInterval'
*
* @description [BurstInterval] The burst interval value of incoming DHCP packets on untrusted interfaces
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_BurstInterval (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBurstIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDsIntfBurstIntervalGet ( keyInterfaceValue, &objBurstIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BurstInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBurstIntervalValue,
                           sizeof (objBurstIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_BurstInterval
*
* @purpose Set 'BurstInterval'
*
* @description [BurstInterval] The burst interval value of incoming DHCP packets on untrusted interfaces
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_BurstInterval (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBurstIntervalValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BurstInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBurstIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBurstIntervalValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDsIntfBurstIntervalSet ( keyInterfaceValue, objBurstIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_ClearBindings
*
* @purpose Set 'ClearBindings'
*
* @description [ClearBindings] Clear DHCP snooping bindings
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_ClearBindings (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClearBindingsValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClearBindings */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClearBindingsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClearBindingsValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }
  keyInterfaceValue = 0;
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDsBindingClear ( keyInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_MACAddress
*
* @purpose Get 'MACAddress'
*
* @description [MACAddress] MAC Address for DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_MACAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  /*fpObjWa_t kwaBindingType = FPOBJ_INIT_WA (sizeof (xLibU32_t));*/
  xLibStr6_t objMACAddressValue;
  xLibStr6_t nextObjMACAddressValue;
  dhcpSnoopBinding_t binding;
  /*dsBindingType_t  objBindingType;*/
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                          (xLibU8_t *) objMACAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objMACAddressValue,0,L7_MAC_ADDR_LEN);
  }

  memcpy(binding.macAddr,objMACAddressValue,L7_MAC_ADDR_LEN);

  while(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDsBindingGetNext(&binding);
    if(owa.l7rc == L7_SUCCESS && binding.bindingType == DS_BINDING_TENTATIVE)
    {
       continue;
    }
    else if(owa.l7rc == L7_SUCCESS && binding.bindingType == DS_BINDING_STATIC)
    {
       break;
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memcpy(nextObjMACAddressValue, binding.macAddr,L7_MAC_ADDR_LEN);

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjMACAddressValue, owa.len);

  /* return the object value: MACAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjMACAddressValue, sizeof (objMACAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_DynMACAddress
*
* @purpose Get 'MACAddress'
*
* @description [MACAddress] MAC Address for Dynamic DHCP Binding
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_DynMACAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  /*fpObjWa_t kwaBindingType = FPOBJ_INIT_WA (sizeof (xLibU32_t));*/
  xLibStr6_t objMACAddressValue;
  xLibStr6_t nextObjMACAddressValue;
  dhcpSnoopBinding_t binding;
  /*dsBindingType_t  objBindingType;*/
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_DynMACAddress,
                          (xLibU8_t *) objMACAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objMACAddressValue,0,L7_MAC_ADDR_LEN);
  }

  memcpy(binding.macAddr,objMACAddressValue,L7_MAC_ADDR_LEN);

  while(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDsBindingGetNext(&binding);
    if(owa.l7rc == L7_SUCCESS && binding.bindingType == DS_BINDING_TENTATIVE)
    {
       continue;
    }
    else if(owa.l7rc == L7_SUCCESS && binding.bindingType == DS_BINDING_DYNAMIC)
    {
       break;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memcpy(nextObjMACAddressValue, binding.macAddr,L7_MAC_ADDR_LEN);

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjMACAddressValue, owa.len);

  /* return the object value: MACAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjMACAddressValue, sizeof (objMACAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_Vlan
*
* @purpose Get 'Vlan'
*
* @description [Vlan] client VLANId for DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_Vlan (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVlanValue = binding.vlanId;

  /* return the object value: Vlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanValue, sizeof (objVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************

* @function fpObjList_SwitchingDHCPSnooping_Vlan
*
* @purpose List 'Vlan'
 *@description  The Vlan to which static DHCP binding is to be configured.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingDHCPSnooping_Vlan (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVlanValue;
  xLibU32_t nextObjVlanValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objVlanValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Vlan,
                          (xLibU8_t *) & objVlanValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjVlanValue = L7_DOT1Q_MIN_VLAN_ID;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanValue, owa.len);
    if((objVlanValue >= L7_DOT1Q_MIN_VLAN_ID) && (objVlanValue <= (L7_DOT1Q_MAX_VLAN_ID-1)))
    {
       nextObjVlanValue = objVlanValue +1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanValue, owa.len);
  /* return the object value: Vlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanValue,
                           sizeof (nextObjVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_DynVlan
*
* @purpose Get 'Vlan'
*
* @description [Vlan] client VLANId for Dynamic DHCP Binding
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_DynVlan (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_DynMACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objVlanValue = binding.vlanId;

  /* return the object value: Vlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanValue, sizeof (objVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_Vlan
*
* @purpose Set 'Vlan'
*
* @description [Vlan] client VLANId for DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_Vlan (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_intIfNumber
*
* @purpose Get 'intIfNumber'
*
* @description [intIfNumber] interface  for DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_intIfNumber (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintIfNumberValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objintIfNumberValue = binding.intIfNum;

  /* return the object value: intIfNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objintIfNumberValue, sizeof (objintIfNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************

* @function fpObjList_TestuspRange_intIfNumber
*
* @purpose Set 'intIfNumber'
*
* @description The port  for configuring DHCP static binding.
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingDHCPSnooping_intIfNumber (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_intIfNumber ,
                          (xLibU8_t *) &objInterfaceValue,
                          &owa.len);

 if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjInterfaceValue = 0;
    objInterfaceValue = 0;
  }


  owa.l7rc = L7_FAILURE;
  while (usmDbGetNextVisibleIntIfNumber(objInterfaceValue, &nextObjInterfaceValue) == L7_SUCCESS)
  {
      if (usmDbDsIntfIsValid(nextObjInterfaceValue) == L7_TRUE)
      {
        owa.l7rc = L7_SUCCESS;
        break;
      }
      else
      {
        objInterfaceValue = nextObjInterfaceValue;
      }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: intIfNumber */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                    sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_DynintIfNumber
*
* @purpose Get 'intIfNumber'
*
* @description [intIfNumber] interface  for DHCP Binding
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_DynintIfNumber (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintIfNumberValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_DynMACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objintIfNumberValue = binding.intIfNum;
  /* return the object value: intIfNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objintIfNumberValue, sizeof (objintIfNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_intIfNumber
*
* @purpose Set 'intIfNumber'
*
* @description [intIfNumber] interface  for DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_intIfNumber (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_BindingType
*
* @purpose Get 'BindingType'
*
* @description [BindingType]  Binding type of  DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_BindingType (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBindingTypeValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objBindingTypeValue = binding.bindingType;

  /* return the object value: BindingType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBindingTypeValue, sizeof (objBindingTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_BindingType
*
* @purpose Set 'BindingType'
*
* @description [BindingType]  Binding type of  DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_BindingType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_IPAddress
*
* @purpose Get 'IPAddress'
*
* @description [IPAddress] IP Address for the DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_IPAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objIPAddressValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objIPAddressValue = binding.ipAddr;
  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPAddressValue, sizeof (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_DynIPAddress
*
* @purpose Get 'IPAddress'
*
* @description [IPAddress] IP Address for the DHCP Binding
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_DynIPAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objIPAddressValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_DynMACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);
  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objIPAddressValue = binding.ipAddr;
  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPAddressValue, sizeof (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_IPAddress
*
* @purpose Set 'IPAddress'
*
* @description [IPAddress] IP Address for the DHCP Binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_IPAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_LeaseTime
*
* @purpose Get 'LeaseTime'
*
* @description [LeaseTime] The Lease time of DHCP dynamic binding
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_LeaseTime (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaseTimeValue;
  dhcpSnoopBinding_t binding;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyMACAddressValue, 0x0, L7_MAC_ADDR_LEN);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_DynMACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  { 
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  memcpy(binding.macAddr,keyMACAddressValue,L7_MAC_ADDR_LEN);

  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);
  if (owa.l7rc != L7_SUCCESS)
  { 
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objLeaseTimeValue = binding.remLease;
  
  /* return the object value: LeaseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLeaseTimeValue, sizeof (objLeaseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_DynClearBindings
*
* @purpose Set 'DynClearBindings'
*
* @description [DynClearBindings] Clear Dynamic DHCP snooping bindings
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_DynClearBindings (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;
 
  dhcpSnoopBinding_t binding;

  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_DynMACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  { 
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);
 
  memcpy(binding.macAddr, keyMACAddressValue, L7_MAC_ADDR_LEN);

  /* get the value from application */
  owa.l7rc = usmDbDsBindingGet(&binding);

  if (owa.l7rc != L7_SUCCESS)
  { 
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDsBindingClear (binding.intIfNum);

  if (owa.l7rc != L7_SUCCESS)
  { 
    owa.rc = XLIBRC_FAILURE; 
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_RowStatus
*
* @purpose Get 'RowStatus'
*
* @description [RowStatus] Add or Remove a static entry to the DHCP snooping bindings database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_RowStatus
*
* @purpose Set 'RowStatus'
*
* @description [RowStatus] Add or Remove a static entry to the DHCP snooping bindings database
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  fpObjWa_t kwaMACAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyMACAddressValue;

  fpObjWa_t kwaIPAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  fpObjWa_t kwaVlan = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  fpObjWa_t kwaintIfNumber = FPOBJ_INIT_WA (sizeof (xLibStr6_t));

  xLibU32_t keyIPAddressValue;
  xLibU32_t keyVlan;
  xLibU32_t keyintIfNumber;

  FPOBJ_TRACE_ENTER (bufp);

  L7_enetMacAddr_t macAddr;

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objRowStatusValue, &owa.len); 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: MACAddress */
  kwaMACAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_MACAddress,
                                    (xLibU8_t *) keyMACAddressValue, &kwaMACAddress.len);
  if (kwaMACAddress.rc != XLIBRC_SUCCESS)
  {
    kwaMACAddress.rc = XLIBRC_MAC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaMACAddress);
    return kwaMACAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMACAddressValue, kwaMACAddress.len);



  owa.l7rc = L7_SUCCESS;
  memcpy(macAddr.addr,keyMACAddressValue,L7_ENET_MAC_ADDR_LEN);

  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {

      /*Retrieve ip addr */
	  kwaIPAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_IPAddress,
	                                    (xLibU8_t *) &keyIPAddressValue, &kwaIPAddress.len);
	  if (kwaIPAddress.rc != XLIBRC_SUCCESS)
	  {
	    kwaIPAddress.rc = XLIBRC_BINDING_ADD_FAILURE;
	    FPOBJ_TRACE_EXIT (bufp, kwaIPAddress);
	    return kwaIPAddress.rc;
	  }

	  /* Retrieve vlanID */
	  kwaVlan.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_Vlan,
	                                    (xLibU8_t *) &keyVlan, &kwaVlan.len);
	  if (kwaVlan.rc != XLIBRC_SUCCESS)
	  {
	    kwaVlan.rc = XLIBRC_BINDING_ADD_FAILURE;;
	    FPOBJ_TRACE_EXIT (bufp, kwaVlan);
	    return kwaVlan.rc;
	  }

	    /* Retrieve intfNum */
	  kwaintIfNumber.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_intIfNumber,
	                                    (xLibU8_t *) &keyintIfNumber, &kwaintIfNumber.len);
	  if (kwaintIfNumber.rc != XLIBRC_SUCCESS)
	  {
	    kwaintIfNumber.rc = XLIBRC_BINDING_ADD_FAILURE;;
	    FPOBJ_TRACE_EXIT (bufp, kwaintIfNumber);
	    return kwaintIfNumber.rc;
	  }
         owa.l7rc = usmDbDsStaticBindingAdd(&macAddr, keyIPAddressValue, keyVlan, 0, keyintIfNumber);
    if (owa.l7rc != L7_SUCCESS)
    {
       owa.rc = XLIBRC_BINDING_ADD_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;   
    }



  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbDsStaticBindingRemove(&macAddr);

     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_BINDING_DEL_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;    
     }
 
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_VlanID
*
* @purpose Get 'VlanID'
*
* @description [VlanID] VlanId
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_VlanID (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIDValue;
  xLibU32_t nextObjVlanIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanID */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_VlanID,
                          (xLibU8_t *) & objVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIDValue = 0;
  }

   if ( ( (objVlanIDValue+1) < L7_DOT1Q_MIN_VLAN_ID) ||
         ( (objVlanIDValue+1) > L7_DOT1Q_MAX_VLAN_ID)  )
   {
       owa.l7rc = L7_FAILURE;
   }

  nextObjVlanIDValue= objVlanIDValue +1;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIDValue, owa.len);

  /* return the object value: VlanID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIDValue, sizeof (objVlanIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDHCPSnooping_VlanDHCPsnoopingMode
*
* @purpose Get 'VlanDHCPsnoopingMode'
*
* @description [VlanDHCPsnoopingMode] Enable or disable DHCP snooping on a VLAN
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDHCPSnooping_VlanDHCPsnoopingMode (void *wap, void *bufp)
{

  fpObjWa_t kwaVlanID = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIDValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanDHCPsnoopingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanID */
  kwaVlanID.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_VlanID,
                                (xLibU8_t *) & keyVlanIDValue, &kwaVlanID.len);
  if (kwaVlanID.rc != XLIBRC_SUCCESS)
  {
    kwaVlanID.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanID);
    return kwaVlanID.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, kwaVlanID.len);

  /* get the value from application */
  owa.l7rc = usmDbDsVlanConfigGet ( keyVlanIDValue, &objVlanDHCPsnoopingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanDHCPsnoopingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanDHCPsnoopingModeValue,
                           sizeof (objVlanDHCPsnoopingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDHCPSnooping_VlanDHCPsnoopingMode
*
* @purpose Set 'VlanDHCPsnoopingMode'
*
* @description [VlanDHCPsnoopingMode] Enable or disable DHCP snooping on a VLAN
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDHCPSnooping_VlanDHCPsnoopingMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanDHCPsnoopingModeValue;

  fpObjWa_t kwaVlanID = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VlanDHCPsnoopingMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVlanDHCPsnoopingModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVlanDHCPsnoopingModeValue, owa.len);

  /* retrieve key: VlanID */
  kwaVlanID.rc = xLibFilterGet (wap, XOBJ_SwitchingDHCPSnooping_VlanID,
                                (xLibU8_t *) & keyVlanIDValue, &kwaVlanID.len);
  if (kwaVlanID.rc != XLIBRC_SUCCESS)
  {
    kwaVlanID.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaVlanID);
    return kwaVlanID.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIDValue, kwaVlanID.len);

  /* set the value in application */
  owa.l7rc = usmDbDsVlanConfigSet ( keyVlanIDValue, keyVlanIDValue, objVlanDHCPsnoopingModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
