/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSysTransfer.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  6 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseSysTransfer_obj.h"
#include "usmdb_sim_api.h"
#include "usmdb_dim_api.h"
#include "usmdb_trapmgr_api.h"
#ifdef L7_TR069_PACKAGE
#include "usmdb_tr069_api.h"
#endif
#include "usmdb_util_api.h"
#include "config_script_api.h"
#include "xlib_private.h"
#include "strlib_base_common.h"
#include "cli_web_mgr_api.h"

#include "unitmgr_api.h"


L7_BOOL fileTransferState_g = L7_FALSE;

#define USMWEB_BUFFER_SIZE_256_Temp 256

L7_char8 *pStrErr_common_FileTransferFailed_Temp = "File transfer failed!";

L7_char8 previousStatus[USMWEB_BUFFER_SIZE_256_Temp] = "Transfer In Progress, Please wait.......";

/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferMode
*
* @purpose Get 'sysTransferMode'
*
* @description Current transfer mode of the system
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTransferModeGet (L7_UNIT_CURRENT, &objsysTransferModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferModeValue,
                     sizeof (objsysTransferModeValue));

  /* return the object value: sysTransferMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferModeValue,
                           sizeof (objsysTransferModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferMode
*
* @purpose Set 'sysTransferMode'
*
* @description Current transfer mode of the system
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTransferModeSet (L7_UNIT_CURRENT, objsysTransferModeValue);
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
* @function fpObjGet_baseSysTransfer_sysTransferServerAddressType
*
* @purpose Get 'sysTransferServerAddressType'
*
* @description Current address type of destination server
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferServerAddressType (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferServerAddressTypeGet (L7_UNIT_CURRENT,
                                       &objsysTransferServerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferServerAddressTypeValue,
                     sizeof (objsysTransferServerAddressTypeValue));

  /* return the object value: sysTransferServerAddressType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferServerAddressTypeValue,
                    sizeof (objsysTransferServerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferDirection
*
* @purpose Get 'sysTransferDirection'
*
* @description Current transfer direction
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferDirection (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferDirectionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferDirectionGet (&objsysTransferDirectionValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferDirectionValue,
                     sizeof (objsysTransferDirectionValue));

  /* return the object value: sysTransferServerAddressType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferDirectionValue,
                    sizeof (objsysTransferDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferServerAddressType
*
* @purpose Set 'sysTransferServerAddressType'
*
* @description Current address type of destination server
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferServerAddressType (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferServerAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferServerAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferServerAddressTypeValue, owa.len);

  /* set the value in application */
  if (objsysTransferServerAddressTypeValue == L7_IP_ADDRESS_TYPE_DNS)
  {
    /* In case of DNS type the set handler of Server Address will take care of setting to
       proper type after DNS look up success */
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
  owa.l7rc =
    usmDbTransferServerAddressTypeSet (L7_UNIT_CURRENT,
                                       objsysTransferServerAddressTypeValue);
  }
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
* @function fpObjGet_baseSysTransfer_sysTransferServerAddress
*
* @purpose Get 'sysTransferServerAddress'
*
* @description Current IP address of destination server
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferServerAddress (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof(L7_inet_addr_t));
  fpObjWa_t owaType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objsysTransferServerAddressValue;
  L7_inet_addr_t serverAddress;
  xLibU32_t objsysTransferServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objsysTransferServerAddressValue, 0, sizeof(objsysTransferServerAddressValue));
  memset(&serverAddress, 0, sizeof(serverAddress));
  /* get the value from application */
  owa.l7rc =
    usmDbTransferServerAddressGet (L7_UNIT_CURRENT,
                                   (L7_uchar8 *)&(serverAddress.addr));
   /* get the value from application */
  owaType.l7rc =
    usmDbTransferServerAddressTypeGet (L7_UNIT_CURRENT,
                                       &objsysTransferServerAddressTypeValue);

  serverAddress.family = (L7_uchar8)objsysTransferServerAddressTypeValue;
  owa.l7rc = inetAddrHtop (&serverAddress, objsysTransferServerAddressValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferServerAddressValue,
                     sizeof (objsysTransferServerAddressValue));

  /* return the object value: sysTransferServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferServerAddressValue,
                           sizeof(objsysTransferServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferServerAddress
*
* @purpose Set 'sysTransferServerAddress'
*
* @description Current IP address of destination server
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferServerAddress (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof(xLibStr256_t));
  xLibStr256_t objsysTransferServerAddressValue;
  L7_inet_addr_t serverAddress;
  xLibU32_t objsysTransferServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = L7_FAILURE;

  /* retrieve object: sysTransferServerAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *)objsysTransferServerAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferServerAddressValue, owa.len);
    owa.len = sizeof(xLibU32_t) ;

  owa.rc = xLibFilterGet (wap, XOBJ_baseSysTransfer_sysTransferServerAddressType,
                                       (xLibU8_t *) & objsysTransferServerAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  if ((owa.l7rc = usmDbParseInetAddrFromIPAddrHostNameStr (objsysTransferServerAddressValue, &serverAddress))
        == L7_SUCCESS)
  {
      if ((objsysTransferServerAddressTypeValue != L7_IP_ADDRESS_TYPE_DNS) &&
          (objsysTransferServerAddressTypeValue != serverAddress.family))
     {
        owa.l7rc = L7_FAILURE;
     }
     else if (objsysTransferServerAddressTypeValue == L7_IP_ADDRESS_TYPE_DNS)
     {
        /* set the value in application */
        if (usmDbTransferServerAddressTypeSet (L7_UNIT_CURRENT,
                                       serverAddress.family) != L7_SUCCESS)
        {
          owa.l7rc = L7_FAILURE;
        }
     }
  }
  if (owa.l7rc == L7_SUCCESS)
  {
  /* set the value in application */
  owa.l7rc =
    usmDbTransferServerAddressSet (L7_UNIT_CURRENT,
                                       (L7_uchar8 *)&(serverAddress.addr));
  }
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
* @function fpObjGet_baseSysTransfer_sysTransferFilePathLocal
*
* @purpose Get 'sysTransferFilePathLocal'
*
* @description The local transfer file path
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferFilePathLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFilePathLocalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferFilePathLocalGet (L7_UNIT_CURRENT,
                                   objsysTransferFilePathLocalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFilePathLocalValue,
                     strlen (objsysTransferFilePathLocalValue));

  /* return the object value: sysTransferFilePathLocal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferFilePathLocalValue,
                           strlen (objsysTransferFilePathLocalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferFilePathLocal
*
* @purpose Set 'sysTransferFilePathLocal'
*
* @description The local transfer file path
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferFilePathLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFilePathLocalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferFilePathLocal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objsysTransferFilePathLocalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFilePathLocalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferFilePathLocalSet (L7_UNIT_CURRENT,
                                   objsysTransferFilePathLocalValue);
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
* @function fpObjGet_baseSysTransfer_sysTransferFileNameLocal
*
* @purpose Get 'sysTransferFileNameLocal'
*
* @description The local transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferFileNameLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFileNameLocalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferFileNameLocalGet (L7_UNIT_CURRENT,
                                   objsysTransferFileNameLocalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFileNameLocalValue,
                     strlen (objsysTransferFileNameLocalValue));

  /* return the object value: sysTransferFileNameLocal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferFileNameLocalValue,
                           strlen (objsysTransferFileNameLocalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferLogFileNameLocal
*
* @purpose Set 'sysTransferLogFileNameLocal'
*
* @description The local transfer log file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferLogFileNameLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferLogFileNameLocal;
  xLibU32_t objsysTransferUploadFileTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

    memset(objsysTransferLogFileNameLocal,0x0,sizeof(objsysTransferLogFileNameLocal));

  /* get the value from application */
  owa.l7rc =
    usmDbTransferUploadFileTypeGet (L7_UNIT_CURRENT,
                                    &objsysTransferUploadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferUploadFileTypeValue,
                     sizeof (objsysTransferUploadFileTypeValue));

 if ( objsysTransferUploadFileTypeValue == L7_FILE_TYPE_STARTUP_LOG)
 {
    osapiSnprintf(objsysTransferLogFileNameLocal, sizeof(objsysTransferLogFileNameLocal), L7_LOG_PERSISTENT_STARTUP_FILE_MASK, 0);
 }
 else if ( objsysTransferUploadFileTypeValue == L7_FILE_TYPE_OPERATIONAL_LOG)
 {
    osapiSnprintf(objsysTransferLogFileNameLocal, sizeof(objsysTransferLogFileNameLocal), L7_LOG_PERSISTENT_OPERATION_FILE_MASK, 0);
 }
 else
 {
    osapiSnprintf(objsysTransferLogFileNameLocal, sizeof(objsysTransferLogFileNameLocal), L7_LOG_PERSISTENT_STARTUP_FILE_MASK, 0);
 }
  /* set the value */
  owa.l7rc =
    usmDbTransferFileNameLocalSet (L7_UNIT_CURRENT,
                                   objsysTransferLogFileNameLocal);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferLogFileNameLocal,
                     strlen (objsysTransferLogFileNameLocal));

  /* return the object value: sysTransferLogFileNameLocal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferLogFileNameLocal,
                           strlen (objsysTransferLogFileNameLocal));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferFileNameLocal
*
* @purpose Set 'sysTransferFileNameLocal'
*
* @description The local transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferFileNameLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFileNameLocalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferFileNameLocal */
  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)objsysTransferFileNameLocalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFileNameLocalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferFileNameLocalSet (L7_UNIT_CURRENT,
                                   objsysTransferFileNameLocalValue);
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
* @function fpObjSet_baseSysTransfer_sysTransferActiveImageFileNameLocal
*
* @purpose Set 'sysTransferActiveImageFileNameLocal'
*
* @description The local transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferActiveImageFileNameLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferActiveImageFileNameLocal;
  L7_uint32 unit ;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objsysTransferActiveImageFileNameLocal,0x0,sizeof(objsysTransferActiveImageFileNameLocal));

  usmDbUnitMgrNumberGet(&unit);
  usmDbTransferUnitNumberSet(unit);

  owa.l7rc =
     usmDbActiveImageNameGet(unit, objsysTransferActiveImageFileNameLocal);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbTransferFileNameLocalSet (unit,
                                   objsysTransferActiveImageFileNameLocal);
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
* @function fpObjGet_baseSysTransfer_sysTransferBackupImageFileNameLocal
*
* @purpose Set 'sysTransferBackupImageFileNameLocal'
*
* @description backup image name
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferBackupImageFileNameLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferBackupImageFileNameLocal;
  L7_uint32 unit ;

  FPOBJ_TRACE_ENTER (bufp);

  /*get Manager*/
  usmDbUnitMgrMgrNumberGet(&unit);

  memset(objsysTransferBackupImageFileNameLocal,0x0,sizeof(objsysTransferBackupImageFileNameLocal));
  owa.l7rc =
     usmDbBackupImageNameGet(unit, objsysTransferBackupImageFileNameLocal);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsysTransferBackupImageFileNameLocal,
                     strlen (objsysTransferBackupImageFileNameLocal));

  /* return the object value: objsysTransferBackupImageFileNameLocal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferBackupImageFileNameLocal,
                           strlen (objsysTransferBackupImageFileNameLocal));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferHTTPFileNameLocal
*
* @purpose Set 'sysTransferHTTPFileNameLocal'
*
* @description The local transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferHTTPFileNameLocal (void *wap,
                                                            void *bufp)
{
#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferHTTPFileNameLocalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferHTTPFileNameLocal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objsysTransferHTTPFileNameLocalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameLocalValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferHTTPFileNameLocalSet (L7_UNIT_CURRENT,
                                   objsysTransferHTTPFileNameLocalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferFilePathRemote
*
* @purpose Get 'sysTransferFilePathRemote'
*
* @description The remote transfer file path
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferFilePathRemote (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFilePathRemoteValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferFilePathRemoteGet (L7_UNIT_CURRENT,
                                    objsysTransferFilePathRemoteValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFilePathRemoteValue,
                     strlen (objsysTransferFilePathRemoteValue));

  /* return the object value: sysTransferFilePathRemote */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferFilePathRemoteValue,
                           strlen (objsysTransferFilePathRemoteValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferFilePathRemote
*
* @purpose Set 'sysTransferFilePathRemote'
*
* @description The remote transfer file path
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferFilePathRemote (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFilePathRemoteValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferFilePathRemote */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objsysTransferFilePathRemoteValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFilePathRemoteValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferFilePathRemoteSet (L7_UNIT_CURRENT,
                                    objsysTransferFilePathRemoteValue);
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
* @function fpObjGet_baseSysTransfer_sysTransferFileNameRemote
*
* @purpose Get 'sysTransferFileNameRemote'
*
* @description The remote transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferFileNameRemote (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFileNameRemoteValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferFileNameRemoteGet (L7_UNIT_CURRENT,
                                    objsysTransferFileNameRemoteValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFileNameRemoteValue,
                     strlen (objsysTransferFileNameRemoteValue));

  /* return the object value: sysTransferFileNameRemote */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferFileNameRemoteValue,
                           strlen (objsysTransferFileNameRemoteValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferFileNameRemote
*
* @purpose Set 'sysTransferFileNameRemote'
*
* @description The remote transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferFileNameRemote (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferFileNameRemoteValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferFileNameRemote */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objsysTransferFileNameRemoteValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferFileNameRemoteValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferFileNameRemoteSet (L7_UNIT_CURRENT,
                                    objsysTransferFileNameRemoteValue);
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
* @function fpObjSet_baseSysTransfer_sysTransferHTTPFileNameRemote
*
* @purpose Set 'sysTransferHTTPFileNameRemote'
*
* @description The remote transfer file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferHTTPFileNameRemote (void *wap,
                                                             void *bufp)
{
#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferHTTPFileNameRemoteValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferHTTPFileNameRemote */
  owa.rc = xLibFilterPtrGet (wap, XOBJ_baseSysTransfer_sysTransferHTTPFileNameRemote,
                                       (xLibU8_t **) & objsysTransferHTTPFileNameRemoteValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_NO_INFO;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameRemoteValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferFileNameRemoteSet (L7_UNIT_CURRENT,
                                    objsysTransferHTTPFileNameRemoteValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferUploadFileType
*
* @purpose Get 'sysTransferUploadFileType'
*
* @description The transfer upload file type
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferUploadFileType (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferUploadFileTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferUploadFileTypeGet (L7_UNIT_CURRENT,
                                    &objsysTransferUploadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferUploadFileTypeValue,
                     sizeof (objsysTransferUploadFileTypeValue));

  /* return the object value: sysTransferUploadFileType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferUploadFileTypeValue,
                    sizeof (objsysTransferUploadFileTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferUploadFileType
*
* @purpose Set 'sysTransferUploadFileType'
*
* @description The transfer upload file type
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferUploadFileType (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferUploadFileTypeValue;
  xLibU32_t fileType;
  xLibU32_t fileSize;
  xLibStr256_t filePromptName;
  xLibStr256_t fileName;
  xLibStr256_t objsysTransferHTTPFileNameLocalValue;
  xLibU32_t objsysTransferModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(filePromptName,0x00,sizeof(filePromptName));
  memset(fileName,0x0,sizeof(fileName));
  /* retrieve object: sysTransferUploadFileType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferUploadFileTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferUploadFileTypeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferUploadFileTypeSet (L7_UNIT_CURRENT,
                                    objsysTransferUploadFileTypeValue);

  owa.len = sizeof(objsysTransferModeValue);
  owa.rc =  xLibFilterGet (wap, XOBJ_baseSysTransfer_sysTransferMode,
                                       (xLibU8_t *) & objsysTransferModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferModeValue, owa.len);
  if(objsysTransferModeValue ==  L7_TRANSFER_HTTP)
  {

     owa.len = sizeof(objsysTransferHTTPFileNameLocalValue);
    /* retrieve object: sysTransferHTTPFileNameLocal */
      owa.rc = xLibFilterGet(wap,XOBJ_baseSysTransfer_sysTransferHTTPFileNameLocal,
                             (xLibU8_t *) objsysTransferHTTPFileNameLocalValue,
                             &owa.len);
    if ( (owa.rc == XLIBRC_SUCCESS) && (strlen(objsysTransferHTTPFileNameLocalValue) > 0 ))
    {
       FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameLocalValue, owa.len);
       osapiStrncpy(filePromptName,objsysTransferHTTPFileNameLocalValue,sizeof(filePromptName));
#if L7_FEAT_DIM_USE_FILENAME
       if ( osapiStrncmp(filePromptName,"image1",strlen(filePromptName)) == 0 )
       {
         /* osapiFsCopyFile(filePromptName,"image.stk"); */
         osapiStrncpy(fileName,"image1.stk\0",10);
       }
       else if ( osapiStrncmp(filePromptName,"image2",strlen(filePromptName)) == 0 )
       {
         /* osapiFsCopyFile(filePromptName,"image.stk"); */
         osapiStrncpy(fileName,"image2.stk\0",10);
       }
#else
       if (osapiStrncmp(filePromptName, "active", strlen(filePromptName)) == 0)
       {
         osapiStrncpy(fileName, "active.stk\0", 10);
       }
       else if (osapiStrncmp(filePromptName, "backup", strlen(filePromptName)) == 0)
       {
         osapiStrncpy(fileName, "backup.stk\0", 10);
       }
#endif
       else
       {
         osapiStrncpy(fileName,filePromptName,strlen(filePromptName));
       }
    }
    else
    {
    fileType = objsysTransferUploadFileTypeValue;
   if (fileType == L7_FILE_TYPE_CODE)
        {
          osapiStrncpy(filePromptName,"code.bin",9);
        }
        else if (fileType == L7_FILE_TYPE_CONFIG)
        {
          osapiStrncpy(filePromptName,"config.bin",11);
        }
        else if (fileType == L7_FILE_TYPE_ACL_XML)
        {
          osapiStrncpy(filePromptName,"easyacl",8);
        }
        else if (fileType == L7_FILE_TYPE_STARTUP_LOG)
        {
          osapiSnprintf(filePromptName, sizeof(filePromptName), L7_LOG_PERSISTENT_STARTUP_FILE_MASK, 0);
        }
        else if (fileType == L7_FILE_TYPE_OPERATIONAL_LOG)
        {
          osapiSnprintf(filePromptName, sizeof(filePromptName), L7_LOG_PERSISTENT_OPERATION_FILE_MASK, 0);
        }
        else
        {
          osapiStrncpy(filePromptName,"unknown.bin",12);
        }
       osapiStrncpy(fileName,filePromptName,strlen(filePromptName));
     }

     if (objsysTransferUploadFileTypeValue == L7_FILE_TYPE_CONFIG_SCRIPT)
     {
         memset(fileName,0x0,sizeof(fileName));
         if( osapiStrncmp(objsysTransferHTTPFileNameLocalValue, "running-config", sizeof("running-config")) == 0 )
         {
           if( usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_TEMP) == L7_SUCCESS)
           {
              /*remove running-config in case it still exists*/
              osapiFsDeleteFile("running-config");

              if( configScriptCopy(L7_SCRIPTS_TEMP_CONFIG_FILENAME) == L7_SUCCESS )
              {
                osapiFsDeleteFile(L7_SCRIPTS_TEMP_CONFIG_FILENAME);
                if (osapiFsRenameFile(TEMP_CONFIG_SCRIPT_FILE_NAME, "running-config") != L7_SUCCESS)
                {
                  owa.rc = XLIBRC_FAILURE;
                  FPOBJ_TRACE_EXIT (bufp, owa);
                  return owa.rc;
                }
                osapiStrncpy(filePromptName,"running-config",15);
                osapiStrncpy(fileName,"running-config\0",15);
              }
              else
              {
                owa.rc = XLIBRC_FILE_EMPTY_NOT_FOUND;    /* TODO: Change if required */
                FPOBJ_TRACE_EXIT (bufp, owa);
                return owa.rc;
              }
           }

         }
         else if( osapiStrncmp(objsysTransferHTTPFileNameLocalValue, L7_SCRIPTS_STARTUP_CONFIG_FILENAME, sizeof(L7_SCRIPTS_STARTUP_CONFIG_FILENAME)) == 0 )
         {
              if( configScriptCopy(L7_SCRIPTS_STARTUP_CONFIG_FILENAME) == L7_SUCCESS )
              {
                osapiStrncpy(filePromptName,L7_SCRIPTS_STARTUP_CONFIG_FILENAME,15);
                osapiStrncpy(fileName,"startup-config\0",14);
              }
              else
              {
                owa.rc = XLIBRC_FILE_EMPTY_NOT_FOUND;    /* TODO: Change if required */
                FPOBJ_TRACE_EXIT (bufp, owa);
                return owa.rc;
              }
         }
         else if( osapiStrncmp(objsysTransferHTTPFileNameLocalValue, L7_SCRIPTS_BACKUP_CONFIG_FILENAME, sizeof(L7_SCRIPTS_BACKUP_CONFIG_FILENAME)) == 0 )
         {
              if( configScriptCopy(L7_SCRIPTS_BACKUP_CONFIG_FILENAME) == L7_SUCCESS )
              {
                osapiStrncpy(filePromptName,L7_SCRIPTS_BACKUP_CONFIG_FILENAME,14);
                osapiStrncpy(fileName,"backup-config\0",13);
              }
              else
              {
                owa.rc = XLIBRC_FILE_EMPTY_NOT_FOUND;    /* TODO: Change if required */
                FPOBJ_TRACE_EXIT (bufp, owa);
                return owa.rc;
              }
         }
     }

     owa.l7rc = osapiFsFileSizeGet (filePromptName, &fileSize);
     if ( ( owa.l7rc == L7_SUCCESS) && ( fileSize > 0 ) )
     {
        xLibDownLoadUrlSet(wap, fileName);
     }
     else
     {
       owa.rc = XLIBRC_FILE_EMPTY_NOT_FOUND;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }
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
* @function fpObjGet_baseSysTransfer_sysTransferDownloadFileType
*
* @purpose Get 'sysTransferDownloadFileType'
*
* @description The transfer download file type
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferDownloadFileType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferDownloadFileTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferDownloadFileTypeGet (L7_UNIT_CURRENT,
                                      &objsysTransferDownloadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferDownloadFileTypeValue,
                     sizeof (objsysTransferDownloadFileTypeValue));

  /* return the object value: sysTransferDownloadFileType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferDownloadFileTypeValue,
                    sizeof (objsysTransferDownloadFileTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferDownloadFileType
*
* @purpose Set 'sysTransferDownloadFileType'
*
* @description The transfer download file type
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferDownloadFileType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferDownloadFileTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferDownloadFileType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferDownloadFileTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferDownloadFileTypeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferDownloadFileTypeSet (L7_UNIT_CURRENT,
                                      objsysTransferDownloadFileTypeValue);
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
* @function fpObjSet_baseSysTransfer_sysTransferDownStart
*
* @purpose Set 'sysTransferDownStart'
*
* @description Start download
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferDownStart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferDownStartValue;
  L7_char8 auditMsg[L7_CLI_MAX_STRING_LENGTH];

  FPOBJ_TRACE_ENTER (bufp);

  memset(auditMsg, L7_NULL, sizeof(auditMsg));

  /* retrieve object: sysTransferDownStart */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferDownStartValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferDownStartValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferDownStartSet (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    objsysTransferDownStartValue = L7_FALSE;
    fileTransferState_g = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  fileTransferState_g = L7_TRUE;
  objsysTransferDownStartValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferHTTPDownStart
*
* @purpose Set 'sysTransferDownStart'
*
* @description Start download
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferHTTPDownStart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysTransferHTTPFileNameRemoteValue;
  xLibStr256_t objsysTransferHTTPFileNameLocalValue;
  xLibU32_t objsysTransferDownloadFileTypeValue;
  xLibStr256_t image1NameStr;
  xLibStr256_t image2NameStr;

#ifdef L7_TR069_PACKAGE
  L7_BOOL   upgradesManaged = L7_FALSE;
#endif


  FPOBJ_TRACE_ENTER (bufp);

  memset(objsysTransferHTTPFileNameRemoteValue,0x0,sizeof(objsysTransferHTTPFileNameRemoteValue));
  memset(objsysTransferHTTPFileNameLocalValue,0x0,sizeof(objsysTransferHTTPFileNameLocalValue));

  /* get the value from application */
  owa.l7rc =
    usmDbTransferDownloadFileTypeGet (L7_UNIT_CURRENT,
                                      &objsysTransferDownloadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

#ifdef _L7_OS_LINUX_
  /* retrieve object: sysTransferHTTPFileNameRemote */
  owa.len = sizeof(objsysTransferHTTPFileNameRemoteValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSysTransfer_sysTransferHTTPFileNameRemote,
                         (xLibU8_t *)  objsysTransferHTTPFileNameRemoteValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_NO_INFO;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#elif (_L7_OS_VXWORKS_ || _L7_OS_ECOS_)
 if (usmDbTransferFileNameLocalGet(L7_UNIT_CURRENT,objsysTransferHTTPFileNameRemoteValue)!= L7_SUCCESS)
 {
   owa.rc = XLIBRC_NO_INFO;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
 }

 #ifdef _L7_OS_ECOS_
 (void)usmDbHttpTransferDownStartSet(L7_UNIT_CURRENT);
 #endif
#endif

  if((objsysTransferDownloadFileTypeValue == L7_FILE_TYPE_CONFIG_SCRIPT) || (objsysTransferDownloadFileTypeValue == L7_FILE_TYPE_CODE))
  {
    owa.len = sizeof(objsysTransferHTTPFileNameLocalValue);
    /* retrieve object: sysTransferHTTPFileNameLocal */
    owa.rc = xLibFilterGet(wap,XOBJ_baseSysTransfer_sysTransferHTTPFileNameLocal,
                           (xLibU8_t *) objsysTransferHTTPFileNameLocalValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameLocalValue, owa.len);
  }
  if (objsysTransferDownloadFileTypeValue == L7_FILE_TYPE_CODE)
  {
#ifdef L7_TR069_PACKAGE
    /* Do not allow UI to upgrade code when this upgradesManaged is true*/
    if (usmdbTr069ACSUpgradesManagedGet(&upgradesManaged) == L7_SUCCESS &&
                                        upgradesManaged == L7_FALSE)
    {
#endif
      /* get the name from application */
#if L7_FEAT_DIM_USE_FILENAME
      owa.l7rc = usmDbImageFileNameGet(0, image1NameStr);
#else
      owa.l7rc = usmDbActiveNameGet(image1NameStr);
#endif
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      /* get the name from application */
#if L7_FEAT_DIM_USE_FILENAME
      owa.l7rc = usmDbImageFileNameGet(1, image2NameStr);
#else
      owa.l7rc = usmDbBackupNameGet(image2NameStr);
#endif
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      if (!FPOBJ_CMP_STR256(objsysTransferHTTPFileNameLocalValue, image1NameStr))
      {
         if ((usmDbTransferFileNameLocalSet(L7_UNIT_CURRENT, image1NameStr) != L7_SUCCESS) ||
             (usmDbTransferFileNameRemoteSet(L7_UNIT_CURRENT, objsysTransferHTTPFileNameRemoteValue) != L7_SUCCESS))
         {
           owa.rc = XLIBRC_TRANSFER_FAILED;    /* TODO: Change if required */
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
         }
      }
      else  if (!FPOBJ_CMP_STR256(objsysTransferHTTPFileNameLocalValue, image2NameStr))
      {
         if ((usmDbTransferFileNameLocalSet(L7_UNIT_CURRENT, image2NameStr) != L7_SUCCESS) ||
             (usmDbTransferFileNameRemoteSet(L7_UNIT_CURRENT, objsysTransferHTTPFileNameRemoteValue) != L7_SUCCESS))
         {
           owa.rc = XLIBRC_TRANSFER_FAILED;    /* TODO: Change if required */
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
         }
      }
#ifdef L7_TR069_PACKAGE
    }
    else
    {
      owa.rc = XLIBRC_FILE_TRANSFER_NOT_ALLOWED;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
#endif
  }
  else if (objsysTransferDownloadFileTypeValue == L7_FILE_TYPE_CONFIG_SCRIPT)
  {
    if (!FPOBJ_CMP_STR256(objsysTransferHTTPFileNameLocalValue, L7_SCRIPTS_STARTUP_CONFIG_FILENAME))
    {
      if (usmDbHttpProcessConfigFile(L7_SCRIPTS_STARTUP_CONFIG_FILENAME) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_TRANSFER_FAILED;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    else  if (!FPOBJ_CMP_STR256(objsysTransferHTTPFileNameLocalValue, L7_SCRIPTS_BACKUP_CONFIG_FILENAME))
    {
      if (usmDbHttpProcessConfigFile(L7_SCRIPTS_BACKUP_CONFIG_FILENAME) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_TRANSFER_FAILED;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    else
    {
      owa.rc = XLIBRC_TRANSFER_FAILED;    /* Script Transfer failed */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
   /*
    for file types other than code,rename them to "",
    bacause this function will rename them to appropriate name
   */
   owa.l7rc = usmDbHttpTransferFileRename(L7_UNIT_CURRENT, objsysTransferDownloadFileTypeValue,
                                                                      objsysTransferHTTPFileNameRemoteValue,"");
  }

  /* Validate downloaded file */ 
  if (objsysTransferDownloadFileTypeValue == L7_FILE_TYPE_CODE)
  {
    owa.l7rc = usmDbTransferDownStartSet (L7_UNIT_CURRENT);
  }
  else
  {
    usmDbTransferInProgressSet(L7_UNIT_CURRENT, L7_TRUE);
    owa.l7rc = usmDbHttpProcessTransferFile (L7_UNIT_CURRENT, objsysTransferDownloadFileTypeValue); 
    usmDbTransferInProgressSet(L7_UNIT_CURRENT, L7_FALSE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_TRANSFER_FAILED;    /* Transfer failed */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    owa.rc = XLIBRC_SUCCESS;
    FPOBJ_TRACE_EXIT (bufp, owa);
    printf("\nHTTP Transfer completed. Management interfaces are released. \n");
    return owa.rc;
  }


}
/*******************************************************************************
* @function  fpObjSet_baseSysTransfer_sysTransferHTTPUploadStart
*
* @purpose Set 'sysTransferHTTPUploadStart'
*
* @description Start Upload
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferHTTPUploadStart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysTransferHTTPFileNameLocalValue;
  xLibU32_t objsysTransferUploadFileTypeValue;
  FPOBJ_TRACE_ENTER (bufp);
  memset(objsysTransferHTTPFileNameLocalValue,0x0,sizeof(objsysTransferHTTPFileNameLocalValue));
  /* get the value from application */
  owa.l7rc =
    usmDbTransferUploadFileTypeGet (L7_UNIT_CURRENT,
                                      &objsysTransferUploadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
if (objsysTransferUploadFileTypeValue == L7_FILE_TYPE_CODE)
{
   owa.len = sizeof(objsysTransferHTTPFileNameLocalValue);
    /* retrieve object: sysTransferHTTPFileNameLocal */
    owa.rc = xLibFilterGet(wap,XOBJ_baseSysTransfer_sysTransferHTTPFileNameLocal,
                             (xLibU8_t *) objsysTransferHTTPFileNameLocalValue,
                             &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameLocalValue, owa.len);
  /* set the value in application */
  owa.l7rc =  usmDbTransferFileNameLocalSet(L7_UNIT_CURRENT, objsysTransferHTTPFileNameLocalValue);
}
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbHttpTransferUpStartSet(L7_UNIT_CURRENT);
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 /*   if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbHttpProcessTransferFile (L7_UNIT_CURRENT,objsysTransferUploadFileTypeValue);

  }
  else
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
*/
 if(owa.l7rc != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
 }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function  fpObjSet_baseSysTransfer_sysTransferActiveHTTPUploadStart
*
* @purpose Set 'sysTransferActiveHTTPUploadStart'
*
* @description Start Upload
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferActiveHTTPUploadStart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysTransferHTTPFileNameLocalValue;
  xLibU32_t objsysTransferUploadFileTypeValue;
  L7_uint32 unit ;
  FPOBJ_TRACE_ENTER (bufp);
  memset(objsysTransferHTTPFileNameLocalValue,0x0,sizeof(objsysTransferHTTPFileNameLocalValue));
  /* get the value from application */

  usmDbUnitMgrNumberGet(&unit);
  usmDbTransferUnitNumberSet(unit);

  owa.l7rc =
    usmDbTransferUploadFileTypeGet (unit,
                                      &objsysTransferUploadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
if (objsysTransferUploadFileTypeValue != L7_FILE_TYPE_CODE)
{
   owa.len = sizeof(objsysTransferHTTPFileNameLocalValue);
    /* retrieve object: sysTransferActiveHTTPUploadStart */
    owa.rc = xLibFilterGet(wap,XOBJ_baseSysTransfer_sysTransferHTTPFileNameLocal,
                             (xLibU8_t *) objsysTransferHTTPFileNameLocalValue,
                             &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameLocalValue, owa.len);
  /* set the value in application */
  owa.l7rc =  usmDbTransferFileNameLocalSet(unit, objsysTransferHTTPFileNameLocalValue);
}
else
{
    /* retrieve object: sysTransferActiveHTTPUploadStart */
    owa.l7rc =
       usmDbActiveImageNameGet(unit, objsysTransferHTTPFileNameLocalValue);

    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, objsysTransferHTTPFileNameLocalValue, owa.len);
  /* set the value in application */
  owa.l7rc =  usmDbTransferFileNameLocalSet(unit, objsysTransferHTTPFileNameLocalValue);
}
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbHttpTransferUpStartSet(unit);
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbHttpProcessTransferFile (unit,objsysTransferUploadFileTypeValue);

  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 if(owa.l7rc != L7_SUCCESS)
 {
   owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
 }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferUploadStart
*
* @purpose Set 'sysTransferUploadStart'
*
* @description Start download
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferUploadStart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferUploadStartValue;
  L7_char8 auditMsg[L7_CLI_MAX_STRING_LENGTH];
  FPOBJ_TRACE_ENTER (bufp);
  memset(auditMsg, L7_NULL, sizeof(auditMsg));

  /* retrieve object: sysTransferUploadStart */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferUploadStartValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferUploadStartValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbTransferUpStartSet (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    fileTransferState_g = L7_FALSE;
    objsysTransferUploadStartValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  fileTransferState_g = L7_TRUE;
  objsysTransferUploadStartValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferResult
*
* @purpose Get 'sysTransferResult'
*
* @description Transfer result
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferResult (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t tempValue;
  xLibStr256_t objsysTransferResultValue;
  L7_char8 buf[USMWEB_BUFFER_SIZE_256_Temp];
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(buf,0x0,sizeof(buf));
  memset(objsysTransferResultValue,0x0,sizeof(objsysTransferResultValue));

  /*osapiStrncpySafe(buf, pStrInfo_base_FileTransferOperationStarted, strlen(pStrInfo_base_FileTransferOperationStarted));*/
  owa.l7rc =
    usmDbTransferResultGet (L7_UNIT_CURRENT, &tempValue, buf);

  if (owa.l7rc != L7_SUCCESS)
  {
    osapiStrncpySafe(objsysTransferResultValue, pStrErr_common_FileTransferFailed_Temp, strlen(pStrErr_common_FileTransferFailed_Temp));
  }
  if(owa.l7rc == L7_SUCCESS)
 { switch (tempValue)  /* Transfer Status */
  {
    case L7_NO_STATUS_CHANGE:
         if(fileTransferState_g == L7_TRUE)
         osapiStrncpySafe(objsysTransferResultValue, buf, strlen(buf));
         break;

    case L7_STATUS_CHANGED:
         if(fileTransferState_g == L7_TRUE)
         osapiStrncpySafe(objsysTransferResultValue, buf, strlen(buf));
         break;

    case L7_TRANSFER_SUCCESS:
    case L7_TRANSFER_FAILED:
    case L7_OTHER_FILE_TYPE_FAILED:
    case L7_TRANSFER_BAD_CRC:
    case L7_FLASH_FAILED:
    case L7_OPR_FILE_TYPE_FAILED:
         osapiStrncpySafe(objsysTransferResultValue, buf, strlen(buf));
         break;

    default:
         L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID,
                 "Unknown return code from tftp result" );
         break;
  }
 }
  FPOBJ_TRACE_VALUE (bufp, objsysTransferResultValue,
                     strlen (objsysTransferResultValue));

  /* return the object value: sysTransferResult */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferCompleteStatus
*
* @purpose Get 'sysTransferCompleteStatus'
*
* @description Transfer Complete Status
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferCompleteStatus(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferResultValue;
  xLibStr256_t msgBuffer;
  L7_uint32 direction = 0;
  L7_uint32 data = 0;
  static L7_uint32 bytesTransfered = 0xFFFFFFFF; 
  xLibU32_t objsysTransferModeValue;
  L7_uint32 resultCode = L7_TRANSFER_CODE_FAILURE;
  L7_uint32 unit ;
  xLibU32_t tempValue;

  L7_char8 *progressMessage = "Transfer In Progress, Please wait.......";
  /*L7_char8 *failMessage = "Transfer Failed.";*/
  L7_char8 *completedMessage = "Transfer Completed.";
  L7_char8 *downLoadFailed = "Download Failed.";
  L7_char8 *upLoadFailed = "Upload Failed.";

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(msgBuffer,0x0,sizeof(msgBuffer));
  memset(objsysTransferResultValue,0x0,sizeof(objsysTransferResultValue));

  data = usmDbTransferCompleteStatus();
  owa.l7rc = usmDbTransferDirectionGet (&direction);
  usmDbUnitMgrNumberGet(&unit);

  usmDbTransferModeGet(unit, &objsysTransferModeValue);
  if ( (objsysTransferModeValue == L7_TRANSFER_SFTP)  ||
       (objsysTransferModeValue == L7_TRANSFER_SCP )  ||
       (objsysTransferModeValue == L7_TRANSFER_HTTP ))
  {
     /*owa.l7rc = usmDbTransferDownloadResultCodeGet (unit,&resultCode);*/
     owa.l7rc = usmDbTransferResultGet(unit, &tempValue, msgBuffer);

     if( osapiStrnlen(msgBuffer, sizeof(msgBuffer)) != 0 )
     {
       memset(previousStatus,0x0,sizeof(previousStatus));
       osapiStrncpySafe(previousStatus, msgBuffer, sizeof(msgBuffer));
     }
     /* TODO: Transfer module returns NULL string if there is no status
        change (L7_NO_STATUS_CHANGE), so we're displaying previous status.
        this handler must be rewritten when transfer module returns proper status
     */
     OSAPI_STRNCAT(objsysTransferResultValue,previousStatus);
     owa.l7rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;

#if 0
     if(usmDbTransferInProgressGet (unit) == L7_TRUE)
     {
        osapiStrncpySafe(msgBuffer, progressMessage, sizeof(msgBuffer));
        if (resultCode == L7_TRANSFER_CODE_FAILURE)
        {
          osapiStrncpySafe(msgBuffer, failMessage, sizeof(msgBuffer));
        }
        OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                            strlen (objsysTransferResultValue));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
     else
     {
        if (resultCode == L7_TRANSFER_CODE_FAILURE)
        {
           osapiStrncpySafe(msgBuffer, failMessage, sizeof(msgBuffer));
        }
        else
        {
          osapiStrncpySafe(msgBuffer, completedMessage, sizeof(msgBuffer));
        }
        OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                            strlen (objsysTransferResultValue));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
#endif

  }

if (usmDbTransferInProgressGet (unit) == L7_TRUE)
  {
    if (direction == L7_TRANSFER_DIRECTION_DOWN)
    {
      if((data == bytesTransfered) && (bytesTransfered != 0))
      {
  osapiSnprintf(msgBuffer, sizeof(msgBuffer), "Download Completed. %d bytes downloaded. Image is being distributed to all units. Please wait.....", data);
  OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      }
      else
      {
  osapiSnprintf(msgBuffer, sizeof(msgBuffer), "%d bytes downloaded...", data);
  OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      }
      bytesTransfered = data;
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
    if (direction == L7_TRANSFER_DIRECTION_UP)
    {
      osapiSnprintf(msgBuffer, sizeof(msgBuffer), "%d bytes uploaded...", data);
      OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
      osapiStrncpySafe(msgBuffer, progressMessage, sizeof(msgBuffer));
      OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
  }

  if (direction == L7_TRANSFER_DIRECTION_DOWN)
  {
    usmDbTransferDownloadResultCodeGet (unit,&resultCode);
    if (resultCode == L7_TRANSFER_CODE_FAILURE)
    {
      osapiStrncpySafe(msgBuffer, downLoadFailed , sizeof(msgBuffer ));
      OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
    }
    else
    {
      osapiSnprintf(msgBuffer, sizeof(msgBuffer), "Download Completed. %d bytes downloaded.", data);
      OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
    }
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  else if (direction == L7_TRANSFER_DIRECTION_UP)
  {
    usmDbTransferUploadResultCodeGet (unit, &resultCode);
    if (resultCode == L7_TRANSFER_CODE_SUCCESS)
    {
      osapiSnprintf(msgBuffer, sizeof(msgBuffer), "Upload Completed. %d bytes uploaded.", data);
      OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
    }
    else if (resultCode == L7_TRANSFER_CODE_FAILURE)
    {
      osapiStrncpySafe(msgBuffer, upLoadFailed, sizeof(msgBuffer));
      OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                           strlen (objsysTransferResultValue));
    }
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;

  }

   FPOBJ_TRACE_VALUE (bufp, objsysTransferResultValue,
                     strlen (objsysTransferResultValue));

  osapiStrncpySafe(msgBuffer, completedMessage, sizeof(msgBuffer));
  OSAPI_STRNCAT(objsysTransferResultValue,msgBuffer);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysTransferResultValue,
                          strlen (objsysTransferResultValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferUploadResultCode
*
* @purpose Get 'sysTransferUploadResultCode'
*
* @description the status of a transfer Upload
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferUploadResultCode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferUploadResultCodeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferUploadResultCodeGet (L7_UNIT_CURRENT,
                                      &objsysTransferUploadResultCodeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferUploadResultCodeValue,
                     sizeof (objsysTransferUploadResultCodeValue));

  /* return the object value: sysTransferUploadResultCode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferUploadResultCodeValue,
                    sizeof (objsysTransferUploadResultCodeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferDownloadResultCode
*
* @purpose Get 'sysTransferDownloadResultCode'
*
* @description the status of a transfer Upload
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferDownloadResultCode (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferDownloadResultCodeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTransferDownloadResultCodeGet (L7_UNIT_CURRENT,
                                        &objsysTransferDownloadResultCodeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferDownloadResultCodeValue,
                     sizeof (objsysTransferDownloadResultCodeValue));

  /* return the object value: sysTransferDownloadResultCode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferDownloadResultCodeValue,
                    sizeof (objsysTransferDownloadResultCodeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSysTransfer_sysTransferInProgress
*
* @purpose Get 'sysTransferInProgress'
*
* @description Transfer progress
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_sysTransferInProgress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferInProgressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objsysTransferInProgressValue =  usmDbTransferInProgressGet (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferInProgressValue,
                     sizeof (objsysTransferInProgressValue));

  /* return the object value: sysTransferInProgress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysTransferInProgressValue,
                           sizeof (objsysTransferInProgressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferInProgress
*
* @purpose Set 'sysTransferInProgress'
*
* @description Transfer progress
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferInProgress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysTransferInProgressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysTransferInProgress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferInProgressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferInProgressValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbTransferInProgressSet (L7_UNIT_CURRENT,
                                         objsysTransferInProgressValue);
  if (owa.l7rc == L7_IMAGE_IN_USE)
  {
     owa.rc = XLIBRC_IMAGE_IN_USE;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSysTransfer_pageRefresh
*
* @purpose Get 'pageRefresh'
 *@description  [pageRefresh] page refresh
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysTransfer_pageRefresh (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objpageRefreshValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objpageRefreshValue = usmDbTransferInProgressGet (L7_UNIT_CURRENT);

  fileTransferState_g = objpageRefreshValue;

  FPOBJ_TRACE_VALUE (bufp, &objpageRefreshValue, sizeof (objpageRefreshValue));

  /* return the object value: pageRefresh */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpageRefreshValue, sizeof (objpageRefreshValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSysTransfer_sysTransferScriptFileNameLocal
*
* @purpose Set 'sysTransferScriptFileNameLocal'
*
* @description The local transfer log file name
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysTransfer_sysTransferScriptFileNameLocal (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysTransferScriptFileNameLocal;
  xLibU32_t objsysTransferUploadFileTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysTransferScriptFileNameLocal, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferScriptFileNameLocal,
                     sizeof (objsysTransferScriptFileNameLocal));

    /* get the value from application */
  owa.l7rc =
    usmDbTransferUploadFileTypeGet (L7_UNIT_CURRENT,
                                    &objsysTransferUploadFileTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysTransferUploadFileTypeValue,
                     sizeof (objsysTransferUploadFileTypeValue));

  if (owa.rc == XLIBRC_SUCCESS)
  {
    if (objsysTransferUploadFileTypeValue == L7_FILE_TYPE_CONFIG_SCRIPT)
    {
      if( osapiStrncmp(objsysTransferScriptFileNameLocal, L7_SCRIPTS_RUNNING_CONFIG_FILENAME, sizeof(L7_SCRIPTS_RUNNING_CONFIG_FILENAME)) == 0 )
      {
        usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_RUNNING);
        osapiStrncpy(objsysTransferScriptFileNameLocal, L7_SCRIPTS_RUNNING_CONFIG_FILENAME, sizeof(objsysTransferScriptFileNameLocal));
      }
      else if( osapiStrncmp(objsysTransferScriptFileNameLocal, L7_SCRIPTS_TEMP_CONFIG_FILENAME, sizeof(L7_SCRIPTS_TEMP_CONFIG_FILENAME)) == 0 )
      {
        usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_TEMP);
        osapiStrncpy(objsysTransferScriptFileNameLocal,L7_SCRIPTS_TEMP_CONFIG_FILENAME,16);
      }
      else if( osapiStrncmp(objsysTransferScriptFileNameLocal, L7_SCRIPTS_STARTUP_CONFIG_FILENAME, sizeof(L7_SCRIPTS_STARTUP_CONFIG_FILENAME)) == 0 )
      {
        osapiStrncpy(objsysTransferScriptFileNameLocal,L7_SCRIPTS_STARTUP_CONFIG_FILENAME,15);
      }
      else if( osapiStrncmp(objsysTransferScriptFileNameLocal, L7_SCRIPTS_BACKUP_CONFIG_FILENAME, sizeof(L7_SCRIPTS_BACKUP_CONFIG_FILENAME)) == 0 )
      {
        osapiStrncpy(objsysTransferScriptFileNameLocal,L7_SCRIPTS_BACKUP_CONFIG_FILENAME,14);
      }
    }
  }

  /* set the value */
  owa.l7rc =
     setConfigScriptSourceFileName (objsysTransferScriptFileNameLocal);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

