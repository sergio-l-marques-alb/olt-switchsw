/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basepingCtrlConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  19 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basepingCtrlConfig_obj.h"
#include "usmdb_ping_api.h"
#include "_xe_basepingCtrlConfigweb_obj.h"
#include "dns_client_api.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_util_api.h"
#include "ping_exports.h"
#ifndef L7_XCLI_PACKAGE
#ifdef L7_CLI_PACKAGE
#include "util_pstring.h"
#endif
#endif



/*********************************************************************
*  Get the Row Status field
*
* @param    handle   @b{(input)} handle for the session
* @param    pingRowStatus   @b{(output)} Row Status
*
* @returns  L7_SUCCESS  
*
* @comments 
*            
*
* @end
*********************************************************************/
L7_RC_t 
fpObjUtil_basepingCtrlConfig_RowStatusGet(L7_ushort16 handle,int *pingRowStatus)
{
  L7_uint32 ipDa;
    
  if(usmDbPingQueryTargetAddrs(handle, &ipDa)== L7_SUCCESS && ipDa != 0)
  {
    *pingRowStatus =  L7_ENABLE;
  } 
  else
  {
    *pingRowStatus =  L7_ENABLE;
  }
  
  return L7_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_pingCtlOwnerIndex
*
* @purpose Get 'pingCtlOwnerIndex'
*
* @description [pingCtlOwnerIndex] To facilitate the provisioning of access control by a security administrator using the View-Based Access Control Model (RFC 2575, VACM) for tables in which multiple users may need to independently create or modify entries, the initial index is used as an 'owner index'. Such an initial index has a syntax of SnmpAdminString, and can thus be trivially mapped to a securityName or groupName as defined in VACM, in accordance with a security policy. When used in conjunction with such a security policy all entries in the table belonging to a particular user (or group) will have the same value for this initial index. For a given user's entries in a particular table, the object identifiers for the information in these entries will have the same subidentifiers (except for the 'column' subidentifier) up to the end of the encoded owner index. To configure VACM to permit access to this portion of the table, one would create vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree including the owner index portion, and vacmViewTreeFamilyMask 'wildcarding' the column subidentifier. More elaborate configurations are possible.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_pingCtlOwnerIndex (void *wap, void *bufp)
{

  xLibStr256_t objpingCtlOwnerIndexValue;
  xLibStr256_t nextObjpingCtlOwnerIndexValue;
  xLibStr256_t objpingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  xLibU16_t handle;
  L7_BOOL operStatus;
  xLibU32_t vrfId;
  xLibU32_t ipDa;
  xLibU16_t count;
  xLibU16_t size;
  xLibU16_t interval;

  handle = 0;
  operStatus = XLIB_FALSE;
  vrfId = 0;
  ipDa = 0;
  count = 0;
  size = 0;
  interval = 0;
  
  /* retrieve key: pingCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                          (xLibU8_t *) objpingCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objpingCtlOwnerIndexValue, 0x00, sizeof(objpingCtlOwnerIndexValue)); 
    memset(objpingCtlTestNameValue, 0x00, sizeof(objpingCtlTestNameValue)); 
    owa.l7rc = usmDbPingSessionGetFirst( objpingCtlOwnerIndexValue, 
  	 objpingCtlTestNameValue, &handle,
        &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpingCtlOwnerIndexValue, owa.len);
    strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);    
    do
    {
      memset(objpingCtlTestNameValue, 0x00, sizeof(objpingCtlTestNameValue)); 
      owa.l7rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	    objpingCtlTestNameValue, &handle,
          &operStatus, &vrfId, &ipDa, &count, &size, &interval );
    }
    while ((strcmp(objpingCtlOwnerIndexValue, nextObjpingCtlOwnerIndexValue)== 0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpingCtlOwnerIndexValue, owa.len);

  /* return the object value: pingCtlOwnerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpingCtlOwnerIndexValue,
                           strlen (objpingCtlOwnerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_pingCtlTestName
*
* @purpose Get 'pingCtlTestName'
*
* @description [pingCtlTestName] The name of the ping test. This is locally unique, within the scope of an pingCtlOwnerIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_pingCtlTestName (void *wap, void *bufp)
{

  xLibStr256_t objpingCtlOwnerIndexValue;
  xLibStr256_t nextObjpingCtlOwnerIndexValue;
  xLibStr256_t objpingCtlTestNameValue;
  xLibStr256_t nextObjpingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  xLibU16_t handle;
  L7_BOOL operStatus;
  xLibU32_t vrfId;
  xLibU32_t ipDa;
  xLibU16_t count;
  xLibU16_t size;
  xLibU16_t interval;

  handle = 0;
  operStatus = XLIB_FALSE;
  vrfId = 0;
  ipDa = 0;
  count = 0;
  size = 0;
  interval = 0;
  
  /* retrieve key: pingCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                          (xLibU8_t *) objpingCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objpingCtlOwnerIndexValue, owa.len);

  /* retrieve key: pingCtlTestName */
  owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                          (xLibU8_t *) objpingCtlTestNameValue, &owa.len);

  strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objpingCtlTestNameValue, 0x00, sizeof(objpingCtlTestNameValue));
    owa.l7rc = usmDbPingSessionGetFirst( objpingCtlOwnerIndexValue, 
  	          objpingCtlTestNameValue, &handle,
              &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpingCtlTestNameValue, owa.len);
    owa.l7rc = usmDbPingSessionGetFirst( objpingCtlOwnerIndexValue, 
  	          objpingCtlTestNameValue, &handle,
              &operStatus, &vrfId, &ipDa, &count, &size, &interval );

  }

  if ((strcmp(objpingCtlOwnerIndexValue, nextObjpingCtlOwnerIndexValue) != 0) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy(nextObjpingCtlTestNameValue,objpingCtlTestNameValue);

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpingCtlTestNameValue, owa.len);

  /* return the object value: pingCtlTestName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpingCtlTestNameValue,
                           strlen (objpingCtlTestNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_pingCtlOwnerIndex_pingCtlTestName
*
* @purpose Get 'pingCtlOwnerIndex + pingCtlTestName +'
*
* @description [pingCtlOwnerIndex]: To facilitate the provisioning of access
*              control by a security administrator using the View-Based
*              Access Control Model (RFC 2575, VACM) for tables in which
*              multiple users may need to independently create or modify entries,
*              the initial index is used as an 'owner index'. Such
*              an initial index has a syntax of SnmpAdminString, and can
*              thus be trivially mapped to a securityName or groupName as
*              defined in VACM, in accordance with a security policy. When
*              used in conjunction with such a security policy all entries
*              in the table belonging to a particular user (or group) will
*              have the same value for this initial index. For a given
*              user's entries in a particular table, the object identifiers
*              for the information in these entries will have the same subidentifiers
*              (except for the 'column' subidentifier) up to
*              the end of the encoded owner index. To configure VACM to permit
*              access to this portion of the table, one would create
*              vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree
*              including the owner index portion, and vacmViewTreeFamilyMask
*              'wildcarding' the column subidentifier.
*              More elaborate configurations are possible. 
*              [pingCtlTestName]: The name of the ping test. This is locally
*              unique, within the scope of an pingCtlOwnerIndex. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_pingCtlOwnerIndex_pingCtlTestName (void
                                                                        *wap,
                                                                        void
                                                                        *bufp[],
                                                                        xLibU16_t
                                                                        keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owapingCtlOwnerIndex = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpingCtlOwnerIndexValue, nextObjpingCtlOwnerIndexValue;
  fpObjWa_t owapingCtlTestName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpingCtlTestNameValue, nextObjpingCtlTestNameValue;
  void *outpingCtlOwnerIndex = (void *) bufp[--keyCount];
  void *outpingCtlTestName = (void *) bufp[--keyCount];


  xLibU16_t handle;
  L7_BOOL operStatus;
  xLibU32_t vrfId;
  xLibU32_t ipDa;
  xLibU16_t count;
  xLibU16_t size;
  xLibU16_t interval;

  handle = 0;
  operStatus = XLIB_FALSE;
  vrfId = 0;
  ipDa = 0;
  count = 0;
  size = 0;
  interval = 0;
  
  FPOBJ_TRACE_ENTER (outpingCtlOwnerIndex);
  FPOBJ_TRACE_ENTER (outpingCtlTestName);

  /* retrieve key: pingCtlOwnerIndex */
  owapingCtlOwnerIndex.rc =
    xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                   (xLibU8_t *) objpingCtlOwnerIndexValue,
                   &owapingCtlOwnerIndex.len);
  if (owapingCtlOwnerIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: pingCtlTestName */
    owapingCtlTestName.rc =
      xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                     (xLibU8_t *) objpingCtlTestNameValue,
                     &owapingCtlTestName.len);
  }
  else
  {
     FPOBJ_TRACE_CURRENT_KEY (outpingCtlOwnerIndex, &objpingCtlOwnerIndexValue,
                           owapingCtlOwnerIndex.len);
     FPOBJ_TRACE_CURRENT_KEY (outpingCtlTestName, &objpingCtlTestNameValue,
                           owapingCtlTestName.len);

     owa.rc = usmDbPingSessionGetFirst( objpingCtlOwnerIndexValue, 
  	  objpingCtlTestNameValue, &handle,
         &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  }

  if (owapingCtlOwnerIndex.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (outpingCtlOwnerIndex, &objpingCtlOwnerIndexValue,
                           owapingCtlOwnerIndex.len);
     FPOBJ_TRACE_CURRENT_KEY (outpingCtlTestName, &objpingCtlTestNameValue,
                           owapingCtlTestName.len);

     owa.rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	  objpingCtlTestNameValue, &handle,
         &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  }
  
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outpingCtlOwnerIndex, owapingCtlOwnerIndex);
    FPOBJ_TRACE_EXIT (outpingCtlTestName, owapingCtlTestName);
    return owa.rc;
  }
  strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
  strcpy(nextObjpingCtlTestNameValue,objpingCtlTestNameValue);
  
  FPOBJ_TRACE_CURRENT_KEY (outpingCtlOwnerIndex, &nextObjpingCtlOwnerIndexValue,
                           owapingCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outpingCtlTestName, &nextObjpingCtlTestNameValue,
                           owapingCtlTestName.len);

  /* return the object value: pingCtlOwnerIndex */
  xLibBufDataSet (outpingCtlOwnerIndex,
                  (xLibU8_t *) nextObjpingCtlOwnerIndexValue,
                  strlen (nextObjpingCtlOwnerIndexValue));

  /* return the object value: pingCtlTestName */
  xLibBufDataSet (outpingCtlTestName,
                  (xLibU8_t *) nextObjpingCtlTestNameValue,
                  strlen (nextObjpingCtlTestNameValue));
  FPOBJ_TRACE_EXIT (outpingCtlOwnerIndex, owapingCtlOwnerIndex);
  FPOBJ_TRACE_EXIT (outpingCtlTestName, owapingCtlTestName);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_pingCtlTargetAddressType
*
* @purpose Get 'pingCtlTargetAddressType'
*
* @description [pingCtlTargetAddressType]: Specifies the type of host address
*              to be used at a remote host for performing a ping operation.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_pingCtlTargetAddressType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpingCtlTargetAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypingCtlOwnerIndexValue,
                              keypingCtlTestNameValue,
                              &objpingCtlTargetAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }*/

  objpingCtlTargetAddressTypeValue = 1 ; /* Always return ipv4 */
  owa.l7rc  = L7_SUCCESS;

  /* return the object value: pingCtlTargetAddressType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objpingCtlTargetAddressTypeValue,
                    sizeof (objpingCtlTargetAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_pingCtlTargetAddressType
*
* @purpose Set 'pingCtlTargetAddressType'
*
* @description [pingCtlTargetAddressType]: Specifies the type of host address
*              to be used at a remote host for performing a ping operation.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_pingCtlTargetAddressType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpingCtlTargetAddressTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pingCtlTargetAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objpingCtlTargetAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpingCtlTargetAddressTypeValue, owa.len);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* set the value in application */
  /*owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypingCtlOwnerIndexValue,
                              keypingCtlTestNameValue,
                              objpingCtlTargetAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } */

  objpingCtlTargetAddressTypeValue = 1; /*always set to ipv4 */
  owa.rc = XLIBRC_SUCCESS;   
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_TargetAddress
*
* @purpose Get 'TargetAddress'
*
* @description [TargetAddress]: Specifies the host address to be used at a
*              remote host for performing a ping operation. The host address
*              type is determined by the object value of corresponding
*              pingCtlTargetAddressType. A value for this object MUST be
*              set prior to transitioning its corresponding pingCtlEntry to
*              active(1) via pingCtlRowStatus. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_TargetAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTargetAddressValue;

  xLibU16_t handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */
  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbPingQueryTargetAddrs (handle, &objTargetAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: TargetAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTargetAddressValue,
                           sizeof (objTargetAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_TargetAddress
*
* @purpose Set 'TargetAddress'
*
* @description [TargetAddress]: Specifies the host address to be used at a
*              remote host for performing a ping operation. The host address
*              type is determined by the object value of corresponding
*              pingCtlTargetAddressType. A value for this object MUST be
*              set prior to transitioning its corresponding pingCtlEntry to
*              active(1) via pingCtlRowStatus. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_TargetAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTargetAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;

  xLibU16_t handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TargetAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTargetAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTargetAddressValue, owa.len);

    
  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* set the value in application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingDestAddrSet (handle,objTargetAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
	owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }  
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_DataSize
*
* @purpose Get 'DataSize'
*
* @description [DataSize]: Specifies the size of the data portion to be transmitted
*              in a ping operation in octets. A ping request is
*              usually an ICMP message encoded into an IP packet. An IP packet
*              has a maximum size of 65535 octets. Subtracting the size
*              of the ICMP or UDP header (both 8 octets) and the size of
*              the IP header (20 octets) yields a maximum size of 65507
*              octets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_DataSize (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDataSizeValue;

  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingProbeSizeGet (handle, &objDataSizeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: DataSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDataSizeValue,
                           sizeof (objDataSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_DataSize
*
* @purpose Set 'DataSize'
*
* @description [DataSize]: Specifies the size of the data portion to be transmitted
*              in a ping operation in octets. A ping request is
*              usually an ICMP message encoded into an IP packet. An IP packet
*              has a maximum size of 65535 octets. Subtracting the size
*              of the ICMP or UDP header (both 8 octets) and the size of
*              the IP header (20 octets) yields a maximum size of 65507
*              octets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_DataSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDataSizeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;


  xLibU16_t handle;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DataSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDataSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDataSizeValue, owa.len);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* set the value in application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingProbeSizeSet (handle, objDataSizeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_TimeOut
*
* @purpose Get 'TimeOut'
*
* @description [TimeOut]: Specifies the time-out value, in seconds, for a
*              remote ping operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_TimeOut (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeOutValue;
  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
 kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
       usmDbPingProbeIntervalGet (handle, &objTimeOutValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: TimeOut */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeOutValue,
                           sizeof (objTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_TimeOut
*
* @purpose Set 'TimeOut'
*
* @description [TimeOut]: Specifies the time-out value, in seconds, for a
*              remote ping operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_TimeOut (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeOutValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;

  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TimeOut */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeOutValue, owa.len);

  
  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* set the value in application */


  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbPingProbeIntervalSet (handle, objTimeOutValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_ProbeCount
*
* @purpose Get 'ProbeCount'
*
* @description [ProbeCount]: Specifies the number of times to perform a ping
*              operation at a remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_ProbeCount (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProbeCountValue;

  xLibU16_t handle;
	  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingProbeCountGet (handle, &objProbeCountValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: ProbeCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objProbeCountValue,
                           sizeof (objProbeCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_ProbeCount
*
* @purpose Set 'ProbeCount'
*
* @description [ProbeCount]: Specifies the number of times to perform a ping
*              operation at a remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_ProbeCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProbeCountValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;

  xLibU16_t handle;
	    
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ProbeCount */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objProbeCountValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objProbeCountValue, owa.len);

  
  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* set the value in application */
  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingProbeCountSet (handle,objProbeCountValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_AdminStatus
*
* @purpose Get 'AdminStatus'
*
* @description [AdminStatus]: Reflects the desired state that a pingCtlEntry
*              should be in: enabled(1) - Attempt to activate the test
*              as defined by this pingCtlEntry. disabled(2) - Deactivate the
*              test as defined by this pingCtlEntry. Refer to the corresponding
*              pingResultsOperStatus to determine the operational
*              state of the test defined by this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_AdminStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatusValue;
  xLibU16_t handle;
  L7_BOOL    operStatus;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbPingQueryOperStatus (handle, &operStatus);
	
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
	
    if(operStatus == L7_TRUE)
    {
      objAdminStatusValue = L7_ENABLE;
    }
    else
    {
     objAdminStatusValue = L7_DISABLE;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: AdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminStatusValue,
                           sizeof (objAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_AdminStatus
*
* @purpose Set 'AdminStatus'
*
* @description [AdminStatus]: Reflects the desired state that a pingCtlEntry
*              should be in: enabled(1) - Attempt to activate the test
*              as defined by this pingCtlEntry. disabled(2) - Deactivate the
*              test as defined by this pingCtlEntry. Refer to the corresponding
*              pingResultsOperStatus to determine the operational
*              state of the test defined by this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_AdminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;

  L7_uint32 rowStatus;
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 handle;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStatusValue, owa.len);

 
  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* set the value in application */
  /*owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypingCtlOwnerIndexValue,
                              keypingCtlTestNameValue, objAdminStatusValue);*/

  switch (objAdminStatusValue)
  {
    case L7_ENABLE:

      rc= usmDbPingSessionHandleFromIndexTestNameGet( keypingCtlOwnerIndexValue,keypingCtlTestNameValue, &handle);

      if (rc == L7_SUCCESS)
      {
        rc = fpObjUtil_basepingCtrlConfig_RowStatusGet(handle,&rowStatus);

        if ((rc == L7_SUCCESS) && (rowStatus == L7_ENABLE))
        {
          rc = usmDbPingSessionStart(handle); 
        }
        else
        {
          rc = L7_FAILURE;
        }
      }
      break;

    case L7_DISABLE:
      
      rc= usmDbPingSessionHandleFromIndexTestNameGet( keypingCtlOwnerIndexValue,keypingCtlTestNameValue, &handle);

      if (rc == L7_SUCCESS)
      {
        rc = fpObjUtil_basepingCtrlConfig_RowStatusGet(handle,&rowStatus);

        if ((rc == L7_SUCCESS) && (rowStatus == L7_ENABLE))
        {
          rc = usmDbPingSessionEnd(handle);
        }
      }

      break;  

    default :
      rc = L7_FAILURE;
      break; 
  }
  

  
  if (rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_SourceAddress
*
* @purpose Get 'SourceAddress'
*
* @description [SourceAddress]: Use the specified IP address (which must be
*              given in numeric form, not as a hostname) as the source address
*              in outgoing probe packets. On hosts with more than one
*              IP address, this option can be used to force the source
*              address to be something other than the primary IP address of
*              the interface the probe packet is sent on. If the IP address
*              is not one of this machine's interface addresses, an error
*              is returned and nothing is sent. A zero length octet string
*              value for this object disables source address specification.
*              The address type (InetAddressType) that relates to this
*              object is specified by the corresponding value of pingCtlSourceAddressType.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_SourceAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_SourceAddress
*
* @purpose Set 'SourceAddress'
*
* @description [SourceAddress]: Use the specified IP address (which must be
*              given in numeric form, not as a hostname) as the source address
*              in outgoing probe packets. On hosts with more than one
*              IP address, this option can be used to force the source
*              address to be something other than the primary IP address of
*              the interface the probe packet is sent on. If the IP address
*              is not one of this machine's interface addresses, an error
*              is returned and nothing is sent. A zero length octet string
*              value for this object disables source address specification.
*              The address type (InetAddressType) that relates to this
*              object is specified by the corresponding value of pingCtlSourceAddressType.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_SourceAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSourceAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SourceAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objSourceAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objSourceAddressValue, owa.len);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_PingRowStatus
*
* @purpose Get 'PingRowStatus'
*
* @description [PingRowStatus]: Status of the Ping config Entry Table Supported
*              values: active(1) - valid entry createAndGo(4) - used
*              to create a new entry destroy(6) - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfig_PingRowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPingRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);
  L7_ushort16 handle;
  
  L7_uint32 ipDa;

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_PING_OWNER_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_PING_TEST_NAME_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  owa.l7rc= usmDbPingSessionHandleFromIndexTestNameGet( keypingCtlOwnerIndexValue,keypingCtlTestNameValue, &handle);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PING_TEST_NAME_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    
  if(usmDbPingQueryTargetAddrs(handle, &ipDa)== L7_SUCCESS && ipDa != 0)
  {
    objPingRowStatusValue =  L7_ROW_STATUS_ACTIVE;
  } 
  else
  {
    objPingRowStatusValue =  L7_ROW_STATUS_CREATE_AND_GO;
  }


  /* return the object value: PingRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPingRowStatusValue,
                           sizeof (objPingRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_PingRowStatus
*
* @purpose Set 'PingRowStatus'
*
* @description [PingRowStatus]: Status of the Ping config Entry Table Supported
*              values: active(1) - valid entry createAndGo(4) - used
*              to create a new entry destroy(6) - removes the entry 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfig_PingRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPingRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);
  L7_ushort16 handle;
 /* L7_BOOL syncFlag = L7_FALSE; */

  /* retrieve object: PingRowStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPingRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPingRowStatusValue, owa.len);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_PING_OWNER_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfig_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_PING_TEST_NAME_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objPingRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    xLibU16_t handle;

    /* Create a row */
    owa.l7rc = usmDbPingSessionAlloc( keypingCtlOwnerIndexValue, 
                            keypingCtlTestNameValue, L7_TRUE, NULL, NULL, NULL, &handle );
    
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_PING_OWNER_ADD_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objPingRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbPingSessionHandleFromIndexTestNameGet( keypingCtlOwnerIndexValue,
    keypingCtlTestNameValue, &handle);
      
    if(owa.l7rc == L7_SUCCESS)
        owa.l7rc = usmDbPingSessionFree( handle ); 

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_PING_OWNER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


L7_RC_t ping_Callback( void * ctxt, L7_ushort16 seqNo, L7_uint32 rtt, L7_ushort16 type, L7_uchar8 *srcIp )
{

  xLibU8_t str[256];
 
  xLibU8_t *ping_res = (L7_char8 *)ctxt;

  xLibU8_t *pStrInfo_base_ReplyFromForIcmpSeqTimeUsec = "\r\nReply From %s: icmp_seq = %d. time= %d usec.";
  xLibU8_t *pStrInfo_base_ReplyFrom = "\r\nReply From %s: %s";
  xLibU8_t *pStrInfo_base_PingDestUnreachable = "Destination Unreachable.";
  
  if (type == PING_PROBE_RESP_TYPE_UNREACH)
  {
    osapiSnprintf(str, sizeof(str), pStrInfo_base_ReplyFrom, srcIp, pStrInfo_base_PingDestUnreachable);
  }
  else
  {
    osapiSnprintf(str, sizeof(str), pStrInfo_base_ReplyFromForIcmpSeqTimeUsec, srcIp, seqNo, rtt);
  }
  if( strlen( ping_res ) + strlen( str ) <  L7_MAX_LEN_1024)
  {
    osapiStrncat(ping_res, str, strlen(str));
  }

  return L7_SUCCESS;

}




/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_TargetAddress
*
* @purpose Set 'TargetAddress'
*
* @description [TargetAddress]: Specifies the host address to be used at a
*              remote host for performing a ping operation. The host address
*              type is determined by the object value of corresponding
*              pingCtlTargetAddressType. A value for this object MUST be
*              set prior to transitioning its corresponding pingCtlEntry to
*              active(1) via pingCtlRowStatus. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfigweb_TargetAddressweb (void *wap, void *bufp)
{

  xLibStr256_t objTargetAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof ( objTargetAddressValue));

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: TargetAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTargetAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTargetAddressValue, owa.len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_basepingCtrlConfigweb_TargetAddressweb (void *wap, void *bufp)
{

  xLibStr256_t objTargetAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objTargetAddressValue));
  FPOBJ_TRACE_ENTER (bufp);

  memset(& objTargetAddressValue,0x0,sizeof(objTargetAddressValue));


  owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_TargetAddressweb,
      (xLibU8_t *) &objTargetAddressValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    memset(objTargetAddressValue, 0x0, sizeof( objTargetAddressValue));
  }

  FPOBJ_TRACE_VALUE (bufp, objTargetAddressValue, strlen (objTargetAddressValue));

  /* return the object value: IPv6Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTargetAddressValue, strlen (objTargetAddressValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_DataSize
*
* @purpose Get 'DataSize'
*
* @description [DataSize]: Specifies the size of the data portion to be transmitted
*              in a ping operation in octets. A ping request is
*              usually an ICMP message encoded into an IP packet. An IP packet
*              has a maximum size of 65535 octets. Subtracting the size
*              of the ICMP or UDP header (both 8 octets) and the size of
*              the IP header (20 octets) yields a maximum size of 65507
*              octets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfigweb_DataSizeweb (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDataSizeValue;


  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_DataSizeweb,
      (xLibU8_t *) &objDataSizeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objDataSizeValue = PING_DEFAULT_PROBE_SIZE;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDatagramSizeValue, sizeof (objDatagramSizeValue));

  /* return the object value: DatagramSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDataSizeValue,
      sizeof (objDataSizeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_DataSize
*
* @purpose Set 'DataSize'
*
* @description [DataSize]: Specifies the size of the data portion to be transmitted
*              in a ping operation in octets. A ping request is
*              usually an ICMP message encoded into an IP packet. An IP packet
*              has a maximum size of 65535 octets. Subtracting the size
*              of the ICMP or UDP header (both 8 octets) and the size of
*              the IP header (20 octets) yields a maximum size of 65507
*              octets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfigweb_DataSizeweb (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDataSizeValue;

  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DataSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDataSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDataSizeValue, owa.len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_TimeOut
*
* @purpose Get 'TimeOut'
*
* @description [TimeOut]: Specifies the time-out value, in seconds, for a
*              remote ping operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfigweb_TimeOutweb (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeOutValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_TimeOutweb,
      (xLibU8_t *) &objTimeOutValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objTimeOutValue = PING_DEFAULT_PROBE_INTERVAL;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTimeOutValue, sizeof (objTimeOutValue));

  /* return the object value: DatagramSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeOutValue,
      sizeof (objTimeOutValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_TimeOut
*
* @purpose Set 'TimeOut'
*
* @description [TimeOut]: Specifies the time-out value, in seconds, for a
*              remote ping operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfigweb_TimeOutweb (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeOutValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TimeOut */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeOutValue, owa.len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_ProbeCount
*
* @purpose Get 'ProbeCount'
*
* @description [ProbeCount]: Specifies the number of times to perform a ping
*              operation at a remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfigweb_ProbeCountweb (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProbeCountValue;

  FPOBJ_TRACE_ENTER (bufp);


 owa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_ProbeCountweb,
      (xLibU8_t *) &objProbeCountValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    objProbeCountValue = PING_DEFAULT_PROBE_COUNT;
  }

  FPOBJ_TRACE_VALUE (bufp, &objProbeCountValue, sizeof (objProbeCountValue));

  /* return the object value: DatagramSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objProbeCountValue,
      sizeof (objProbeCountValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_ProbeCount
*
* @purpose Set 'ProbeCount'
*
* @description [ProbeCount]: Specifies the number of times to perform a ping
*              operation at a remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfigweb_ProbeCountweb (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProbeCountValue;

	    
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ProbeCount */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objProbeCountValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objProbeCountValue, owa.len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basepingCtrlConfig_ProbeCount
*
* @purpose Get 'ProbeCount'
*
* @description [ProbeCount]: Specifies the number of times to perform a ping
*              operation at a remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingCtrlConfigweb_PingResultsweb (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  /* xLibStr256_t objPingoutputValue; */
  L7_uchar8 objPingoutputValue[L7_MAX_LEN_1024];
  char* tempOutput = NULL;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objPingoutputValue, 0x0, sizeof(objPingoutputValue));
 
  owa.len = sizeof (objPingoutputValue);

  if(xLibFilterContextGet(wap,XOBJ_basepingCtrlConfigweb_PingAction,(void *)&tempOutput) == XLIBRC_SUCCESS)
  {
    /*we did not set any context value so this is possible*/
    if(tempOutput != NULL)
    {
      memcpy(objPingoutputValue,tempOutput,strlen(tempOutput));
      osapiFree(L7_CLI_WEB_COMPONENT_ID,tempOutput);
    }
  }
  FPOBJ_TRACE_VALUE (bufp, objPingoutputValue, strlen (objPingoutputValue));

  /* return the object value: Pingoutput */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPingoutputValue, strlen (objPingoutputValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basepingCtrlConfig_TimeOut
*
* @purpose Set 'TimeOut'
*
* @description [TimeOut]: Specifies the time-out value, in seconds, for a
*              remote ping operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basepingCtrlConfigweb_PingAction (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objTargetAddressValue;

  xLibU32_t objPingActionValue;
  xLibU32_t objProbeCountValue;
  xLibU32_t objTimeOutValue;
  xLibU32_t ipaddr = 0;
  xLibU16_t handle;
  xLibU32_t  vrfId = L7_VALID_VRID ;
  xLibU32_t objDataSizeValue;
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  xLibU8_t    hostFQDN[256];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  xLibU16_t probeSent, probeSucc, probeFail;
  xLibU32_t minRtt, maxRtt, avgRtt;
  xLibU8_t buf[80];
  xLibU8_t* tempBuf = NULL;
  L7_BOOL operStatus;

  xLibU8_t  *pStrInfo_base_TxRxMinMaxAvgRttMsec = "Tx = %d, Rx = %d Min/Max/Avg RTT = %d/%d/%d msec";
  xLibU8_t *pStrInfo_common_CrLf = "\r\n";


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TimeOut */
  kwa.len = sizeof (objPingActionValue);
  kwa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPingActionValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPingActionValue, kwa.len);



  kwa.len = sizeof (objDataSizeValue);
  kwa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_DataSizeweb,
      (xLibU8_t *) &objDataSizeValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_DATAGRAM_SIZE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


  kwa.len = sizeof (objProbeCountValue);
  kwa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_ProbeCountweb,
      (xLibU8_t *) &objProbeCountValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_PROBE_COUNT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  kwa.len = sizeof ( objTimeOutValue);
  kwa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_TimeOutweb,
      (xLibU8_t *) &objTimeOutValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_INTERVAL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }


 
  kwa.len = sizeof(objTargetAddressValue);
  memset(&objTargetAddressValue, 0x0, sizeof(objTargetAddressValue));
  kwa.rc = xLibFilterGet (wap, XOBJ_basepingCtrlConfigweb_TargetAddressweb,
      (xLibU8_t *) &objTargetAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_INVALID_HOST_NAME_OR_IPV4_ADDR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  kwa.l7rc = usmDbIPHostAddressValidate(objTargetAddressValue, &ipaddr, &addrType);
  if (kwa.l7rc != L7_SUCCESS)
  {

    kwa.rc = XLIBRC_INVALID_HOST_NAME_OR_IPV4_ADDR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
   
  }
 
  if (addrType == L7_IP_ADDRESS_TYPE_DNS)
  {
    kwa.l7rc = usmDbDNSClientNameLookup(objTargetAddressValue, &status, hostFQDN, &ipaddr);
  }

  if (kwa.l7rc != L7_SUCCESS)
  {

    kwa.rc = XLIBRC_DNS_LOOKUP_FAILED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
   
  }

 
  if  (ipaddr != 0x00) 
  {
  
    tempBuf = osapiMalloc(L7_CLI_WEB_COMPONENT_ID,L7_MAX_LEN_1024);
   
    if(tempBuf == NULL)
    {
      kwa.rc = XLIBRC_MEMORY_ALLOC_FAILED;    /* TODO: Change if required */
      return kwa.rc;
    }
   
    memset(tempBuf,0,L7_MAX_LEN_1024);
    if(xLibFilterContextSet(wap,XOBJ_basepingCtrlConfigweb_PingAction,tempBuf) != XLIBRC_SUCCESS)
    {
      osapiFree(L7_CLI_WEB_COMPONENT_ID,tempBuf);
      kwa.rc = XLIBRC_CONTEXT_SET_FAILED;    /* TODO: Change if required */
      return kwa.rc;
    }
  
    if(usmDbPingStart( "", "", L7_TRUE, vrfId, ipaddr, objProbeCountValue, objDataSizeValue,objTimeOutValue,
                       0, ping_Callback,L7_NULL,tempBuf, &handle ) != L7_SUCCESS )
    {

      kwa.rc = XLIBRC_PING_FAILED;    /* TODO: Change if required */
      return kwa.rc;
    }
    else
    {

      if ( usmDbPingQuery( handle, &operStatus, &probeSent, &probeSucc, &probeFail,
                                   &minRtt, &maxRtt, &avgRtt ) != L7_SUCCESS )
      {

        kwa.rc = XLIBRC_PING_FAILED;    /* TODO: Change if required */
        return kwa.rc;
      }
      else
      {
  
        osapiSnprintf(buf, sizeof(buf), pStrInfo_base_TxRxMinMaxAvgRttMsec, probeSent,
                       probeSucc, minRtt/PING_RTT_MULTIPLIER, maxRtt/PING_RTT_MULTIPLIER, avgRtt/PING_RTT_MULTIPLIER );

        if( strlen( tempBuf ) + strlen( pStrInfo_common_CrLf ) <  L7_MAX_LEN_1024)
        {
          osapiStrncat(tempBuf, pStrInfo_common_CrLf, strlen( pStrInfo_common_CrLf));
        }

        if( strlen( tempBuf ) + strlen( buf ) <  L7_MAX_LEN_1024)
        { 
          osapiStrncat(tempBuf, buf, strlen(buf));
        }

      }
     
      usmDbPingSessionFree( handle );    

    }
  
  }

  FPOBJ_TRACE_EXIT (bufp, kwa);
  return kwa.rc;

}


