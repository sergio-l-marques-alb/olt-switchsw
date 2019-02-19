/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securityRadius.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to radius-object.xml
*
* @create  23 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_securityRadius_obj.h"
#include "usmdb_radius_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "radius_exports.h"

L7_RC_t fpObjUtil_securityRadiusServIndexNextValidGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;

  rc = usmDbRadiusServerCountGet(L7_UNIT_CURRENT, &temp_val);
       
  if (rc == L7_SUCCESS) 
  {
      /* set next valid index accordingly */
      *val = temp_val+1;

      if (*val > FD_RADIUS_MAX_AUTH_SERVERS)
      {
        *val = 0;
      }
  }

  return rc;

}

L7_RC_t fpObjUtil_securityRadiusAccServIndexNextValidGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;

  rc = usmDbRadiusConfiguredServersCountGet(L7_UNIT_CURRENT, RADIUS_SERVER_TYPE_ACCT, &temp_val);

  if (rc == L7_SUCCESS)
  {
      /* set next valid index accordingly */
      *val = temp_val+1;

      if (*val > FD_RADIUS_MAX_ACCT_SERVERS)
      {
        *val = 0;
      }
  }

  return rc;

}

L7_RC_t fpObjUtil_securityRadiusServEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 maxIndex = 0;
  xLibStr256_t serverAddress;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN; 
  memset(serverAddress,0x00,sizeof(serverAddress));

  /* look for the next index */
  *serverIndex += 1;

  if (usmDbRadiusServerCountGet(UnitIndex, &maxIndex) != L7_SUCCESS)
    return rc;

  if(*serverIndex > maxIndex)
  	return rc;

  while (rc == L7_FAILURE &&
         *serverIndex <= maxIndex)
  {
    if ((rc = usmDbRadiusServerIPHostNameByIndexGet(UnitIndex, *serverIndex, serverAddress, &addrType)) == L7_SUCCESS)
    {
        break;
    }
    /* increment the index */
    *serverIndex += 1;
  }

  return rc;
}

L7_RC_t fpObjUtil_securityRadiusAccConfigEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex)
{
  L7_RC_t rc = L7_FAILURE;
  xLibStr256_t serverAddress;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN; 
  memset(serverAddress,0x00,sizeof(serverAddress));
  L7_uint32 maxIndex = 0;  /*Currently only 1 accounting server is supported */

  /* look for the next index */
  *serverIndex += 1;

  if (usmDbRadiusConfiguredServersCountGet(L7_UNIT_CURRENT, RADIUS_SERVER_TYPE_ACCT, &maxIndex) != L7_SUCCESS)
    return rc;

  if(*serverIndex > maxIndex)
    return rc;

  while (rc == L7_FAILURE &&
         *serverIndex <= maxIndex)
  {
    if ((rc = usmDbRadiusAccountingServerIPHostNameByIndexGet(UnitIndex, *serverIndex, serverAddress, &addrType)) == L7_SUCCESS)
    {
      break;
    }
    /* increment the index */
    *serverIndex += 1;
  }

  return rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingServerIndex
*
* @purpose Get 'RadiusAccountingServerIndex'
*
* @description [RadiusAccountingServerIndex] Unique index of the configured RADIUS accounting server. The next valid value of this object for creation is specified by RadiusAccountingIndexNextValid.
*
* @notes  
*`
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingServerIndex (void *wap, void *bufp)
{

  xLibStr256_t objRadiusAccountingServerAddressValue;
  xLibStr256_t nextObjRadiusAccountingServerAddressValue;
  xLibU32_t objRadiusAccountingServerIndexValue;
  xLibU32_t nextObjRadiusAccountingServerIndexValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerAddress,
                          (xLibU8_t *) objRadiusAccountingServerAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objRadiusAccountingServerAddressValue, owa.len);

  /* retrieve key: RadiusAccountingServerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & objRadiusAccountingServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjRadiusAccountingServerIndexValue = 0;
    do
    {
    owa.l7rc = fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                     &nextObjRadiusAccountingServerIndexValue);
      if(owa.l7rc == L7_SUCCESS)
      {
        owa.l7rc =
        usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                  nextObjRadiusAccountingServerIndexValue,
                                                  nextObjRadiusAccountingServerAddressValue,
                                                  &addrType);
      }
    } while(( owa.l7rc == L7_SUCCESS) && ( strcmp(nextObjRadiusAccountingServerAddressValue,objRadiusAccountingServerAddressValue) != 0 ) );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRadiusAccountingServerIndexValue, owa.len);
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRadiusAccountingServerIndexValue, owa.len);

  /* return the object value: RadiusAccountingServerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRadiusAccountingServerIndexValue,
                           sizeof (objRadiusAccountingServerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingServerAddress
*
* @purpose Get 'RadiusAccountingServerAddress'
*
* @description [RadiusAccountingServerAddress] IP Address of the configured RADIUS accounting server. This object cannot be changed after creation.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingServerAddress (void *wap, void *bufp)
{

  xLibStr256_t objRadiusAccountingServerAddressValue;
  xLibStr256_t nextObjRadiusAccountingServerAddressValue;
  xLibU32_t nextObjRadiusAccountingServerIndexValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerAddress,
                          (xLibU8_t *) objRadiusAccountingServerAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjRadiusAccountingServerIndexValue = 0;
    owa.l7rc = fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                     &nextObjRadiusAccountingServerIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc =
      usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                nextObjRadiusAccountingServerIndexValue,
                                                objRadiusAccountingServerAddressValue,
                                                &addrType);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objRadiusAccountingServerAddressValue, owa.len);
    nextObjRadiusAccountingServerIndexValue = 0;
    do
    {
      owa.l7rc = fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                     &nextObjRadiusAccountingServerIndexValue);
      if(owa.l7rc == L7_SUCCESS)
      {
        owa.l7rc =
        usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                  nextObjRadiusAccountingServerIndexValue,
                                                  nextObjRadiusAccountingServerAddressValue,
                                                  &addrType);
      }
    }
    while ((strcmp(objRadiusAccountingServerAddressValue,nextObjRadiusAccountingServerAddressValue) != 0 )
           && (owa.l7rc == L7_SUCCESS));
    if ( owa.l7rc == L7_SUCCESS )
    {
      owa.l7rc = fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                     &nextObjRadiusAccountingServerIndexValue);
      if(owa.l7rc == L7_SUCCESS)
      {
        owa.l7rc =
        usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                  nextObjRadiusAccountingServerIndexValue,
                                                  nextObjRadiusAccountingServerAddressValue,
                                                  &addrType);
      }
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjRadiusAccountingServerAddressValue, owa.len);

  /* return the object value: RadiusAccountingServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjRadiusAccountingServerAddressValue,
                           strlen (objRadiusAccountingServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#endif

/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingServerIndex
*
* @purpose Get 'RadiusAccountingServerIndex'
*
* @description [RadiusAccountingServerIndex]: Unique index of the configured
*              RADIUS accounting server. The next valid value of this object
*              for creation is specified by RadiusAccountingIndexNextValid.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingServerIndex (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingServerIndexValue;
  xLibU32_t nextObjRadiusAccountingServerIndexValue;
  objRadiusAccountingServerIndexValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & objRadiusAccountingServerIndexValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjRadiusAccountingServerIndexValue = 0;
    objRadiusAccountingServerIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objRadiusAccountingServerIndexValue,
                             owa.len);
  owa.l7rc =
      fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                                       &objRadiusAccountingServerIndexValue);
  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjRadiusAccountingServerIndexValue = objRadiusAccountingServerIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRadiusAccountingServerIndexValue, owa.len);

  /* return the object value: RadiusAccountingServerIndex */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & nextObjRadiusAccountingServerIndexValue,
                    sizeof (objRadiusAccountingServerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingServerAddress
*
* @purpose Get 'RadiusAccountingServerAddress'
*
* @description [RadiusAccountingServerAddress]: IP Address of the configured
*              RADIUS accounting server. This object cannot be changed
*              after creation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingServerAddress (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusAccountingServerAddressValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyRadiusAccountingServerIndexValue,
                                              objRadiusAccountingServerAddressValue,
                                              &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusAccountingServerAddress */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objRadiusAccountingServerAddressValue,
                    strlen (objRadiusAccountingServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingServerAddress
*
* @purpose Set 'RadiusAccountingServerAddress'
*
* @description [RadiusAccountingServerAddress]: IP Address of the configured
*              RADIUS accounting server. This object cannot be changed
*              after creation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingServerAddress (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusAccountingServerAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve object: RadiusAccountingServerAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objRadiusAccountingServerAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRadiusAccountingServerAddressValue, owa.len);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

    /* retrieve row status object value */
  /*kwaServStatus.rc =
    xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerStatus,
                   (xLibU8_t *) &objRadiusServerStatusValue, &kwaServStatus.len);
  if (kwaServStatus.rc != XLIBRC_SUCCESS)
  {
    kwaServStatus.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaServStatus);
    return kwaServStatus.rc;
  }*/
  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingServerAddressType
*
* @purpose Get 'RadiusAccountingServerAddressType'
*
* @description [RadiusAccountingServerAddressType]: IP Address Type of the
*              configured RADIUS accounting server. This object cannot be
*              changed after creation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingServerAddressType (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingServerAddressTypeValue;
  xLibStr256_t addrValueStr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &objRadiusAccountingServerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusAccountingServerAddressType */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objRadiusAccountingServerAddressTypeValue,
                    sizeof (objRadiusAccountingServerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingServerAddressType
*
* @purpose Set 'RadiusAccountingServerAddressType'
*
* @description [RadiusAccountingServerAddressType]: IP Address Type of the
*              configured RADIUS accounting server. This object cannot be
*              changed after creation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingServerAddressType (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingServerAddressTypeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusAccountingServerAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objRadiusAccountingServerAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusAccountingServerAddressTypeValue, owa.len);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);


  owa.l7rc = L7_SUCCESS; /* Return success always since it will be set only in the row status object handler */      
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingPort
*
* @purpose Get 'RadiusAccountingPort'
*
* @description [RadiusAccountingPort]: Port number for the RADIUS accounting
*              server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingPort (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingPortValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAccountingHostNameServerPortNumGet (L7_UNIT_CURRENT,
                                                   addrValueStr,addrType,
                                                   &objRadiusAccountingPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusAccountingPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusAccountingPortValue,
                           sizeof (objRadiusAccountingPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingPort
*
* @purpose Set 'RadiusAccountingPort'
*
* @description [RadiusAccountingPort]: Port number for the RADIUS accounting
*              server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingPortValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusAccountingPort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusAccountingPortValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusAccountingPortValue, owa.len);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAccountingHostNameServerPortNumSet (L7_UNIT_CURRENT,
                                                   addrValueStr,addrType,
                                                   objRadiusAccountingPortValue);
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
* @function fpObjGet_securityRadius_RadiusAccountingSecret
*
* @purpose Get 'RadiusAccountingSecret'
*
* @description [RadiusAccountingSecret]: Configured shared sercret for the
*              RADIUS accounting server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingSecret (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusAccountingSecretValue;
  L7_BOOL isConfigured;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc =
    usmDbRadiusAccountingHostNameServerSharedSecretConfigured (L7_UNIT_CURRENT,
                                                        addrValueStr,addrType,
                                                        &isConfigured);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(isConfigured == L7_TRUE)
  {
  
    owa.l7rc =
        usmDbRadiusAccountingHostNameServerSharedSecretGet (L7_UNIT_CURRENT,
                                                        addrValueStr,addrType,
                                                        objRadiusAccountingSecretValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  	

  /* return the object value: RadiusAccountingSecret */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRadiusAccountingSecretValue,
                           strlen (objRadiusAccountingSecretValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingSecret
*
* @purpose Set 'RadiusAccountingSecret'
*
* @description [RadiusAccountingSecret]: Configured shared sercret for the
*              RADIUS accounting server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingSecret (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusAccountingSecretValue;
  xLibStr256_t tempObjRadiusAccountingSecretValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  xLibU32_t  wait_time=10;/*waiting time*/
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusAccountingSecret */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objRadiusAccountingSecretValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRadiusAccountingSecretValue, owa.len);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /*owa.l7rc =
    usmDbRadiusAccountingHostNameServerSharedSecretConfigured (L7_UNIT_CURRENT,
                                                        addrValueStr,addrType,
                                                        &isConfigured);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }*/

  /*if(isConfigured == L7_TRUE)
  {*/
	  owa.l7rc =
	    usmDbRadiusAccountingHostNameServerSharedSecretSet (L7_UNIT_CURRENT,
	                                                        addrValueStr,addrType,
	                                                        objRadiusAccountingSecretValue);
   memset(tempObjRadiusAccountingSecretValue,0x00,sizeof(tempObjRadiusAccountingSecretValue));
   while (wait_time &&usmDbRadiusAccountingHostNameServerSharedSecretGet (L7_UNIT_CURRENT,
                                                        addrValueStr,addrType,
                                                        tempObjRadiusAccountingSecretValue)!= L7_SUCCESS)
   {
 
      if(strncmp(tempObjRadiusAccountingSecretValue,objRadiusAccountingSecretValue,sizeof(objRadiusAccountingSecretValue))!=0)   
      break;
  
            /* sleep for a microsecond to give the Radius component
                    time to process the add request */
           osapiSleepUSec(1);
           wait_time--;
   }

 /* }
  else
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }*/
  
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
* @function fpObjGet_securityRadius_RadiusAccountingIsSecretSet
*
* @purpose Get 'RadiusAccountingIsSecretSet'
*
* @description [RadiusAccountingIsSecretSet] Is shared sercret configured for the RADIUS accounting server.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingIsSecretSet (void *wap, void *bufp)
{

  fpObjWa_t kwaRadiusAccountingServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;

  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingIsSecretSetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwaRadiusAccountingServerIndex.rc =
    xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                   (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                   &kwaRadiusAccountingServerIndex.len);
  if (kwaRadiusAccountingServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusAccountingServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusAccountingServerIndex);
    return kwaRadiusAccountingServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue,
                           kwaRadiusAccountingServerIndex.len);

  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAccountingHostNameServerSharedSecretConfigured (L7_UNIT_CURRENT,
                                                        addrValueStr,addrType,
                                                        &objRadiusAccountingIsSecretSetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusAccountingIsSecretSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusAccountingIsSecretSetValue,
                           sizeof (objRadiusAccountingIsSecretSetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingStatus
*
* @purpose Get 'RadiusAccountingStatus'
*
* @description [RadiusAccountingStatus]: Creates or destroys a RADIUS accounting
*              server entry During creation, the next available index
*              is specified by theRadiusAccountingIndexNextValid object.
*               Rows creation using a different value for RadiusAccountingServerIndex
*              will fail.active(1)This entry is active.createAndGo(4)
*              Creates a new entry.destroy(6)Deletes an entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                          (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue, kwa.len);

  objRadiusAccountingStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RadiusAccountingStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusAccountingStatusValue,
                           sizeof (objRadiusAccountingStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingStatus
*
* @purpose Set 'RadiusAccountingStatus'
*
* @description [RadiusAccountingStatus]: Creates or destroys a RADIUS accounting
*              server entry During creation, the next available index
*              is specified by theRadiusAccountingIndexNextValid object.
*               Rows creation using a different value for RadiusAccountingServerIndex
*              will fail.active(1)This entry is active.createAndGo(4)
*              Creates a new entry.destroy(6)Deletes an entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingStatusValue;
  fpObjWa_t kwaRadiusAccountingServerAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRadiusAccountingServerAddressValue;
  xLibU32_t keyRadiusAccountingServerIndexValue;

  fpObjWa_t kwaRadiusAccountingServerName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t kwaRadiusAccountingServerNameValue;

  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t tempRadiusAccountingServerAddressType;
  L7_uint32 wait_time = 10; /* controls how long to wait for Radius server to process create request */
  L7_uint32 ipAddr;
  L7_uint32 tempServIndex;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: RadiusAccountingStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusAccountingStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusAccountingStatusValue, owa.len);
  
  /* call the usmdb only for add and delete */
  if (objRadiusAccountingStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    
    owa.l7rc =  fpObjUtil_securityRadiusAccServIndexNextValidGet(&tempServIndex);
    if ((owa.l7rc != L7_SUCCESS))
    {
      owa.rc = XLIBRC_RADIUS_ACCOUNTING_SERVER_ADD_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    keyRadiusAccountingServerIndexValue = tempServIndex;

    /* retrieve  RadiusAccountingServerAddress */
    kwaRadiusAccountingServerAddress.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerAddress,
                                                         (xLibU8_t *) keyRadiusAccountingServerAddressValue,
                                                         &kwaRadiusAccountingServerAddress.len);
    if (kwaRadiusAccountingServerAddress.rc != XLIBRC_SUCCESS)
    {
      kwaRadiusAccountingServerAddress.rc =XLIBRC_RADIUS_ACCOUNTING_SERVER_ADD_FAILURE ;
      FPOBJ_TRACE_EXIT (bufp, kwaRadiusAccountingServerAddress);
      return kwaRadiusAccountingServerAddress.rc;
    }

   /*Changes done to handle the situation where do  not get Type valu from web.
     Have To determine manually*/

    kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerAddressType,
                            (xLibU8_t *) & tempRadiusAccountingServerAddressType,
                            &kwa.len);
    if(usmDbIPHostAddressValidate(keyRadiusAccountingServerAddressValue, &ipAddr,&addrType) != L7_SUCCESS)
    {
         owa.rc = XLIBRC_INVALID_IP_ADDRESS;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
    }

    if (kwa.rc !=  XLIBRC_SUCCESS)
    {
       tempRadiusAccountingServerAddressType = addrType;
    }
    else
    {
       if(addrType != tempRadiusAccountingServerAddressType)
      {
        owa.rc = XLIBRC_INVALID_IP_ADDRESS;     /*TODO: Add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
   
    /* retrieve  RadiusAccountingServerName */
    kwaRadiusAccountingServerName.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingName,
                                                      (xLibU8_t *) kwaRadiusAccountingServerNameValue,
                                                      &kwaRadiusAccountingServerName.len);
    if (kwaRadiusAccountingServerName.rc != XLIBRC_SUCCESS)
    {
      kwaRadiusAccountingServerName.rc = XLIBRC_RADIUS_ACCOUNTING_SERVER_ADD_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaRadiusAccountingServerName);
      return kwaRadiusAccountingServerName.rc;
    }

	  
	  if(usmDbRadiusAccountingHostNameServerAdd (L7_UNIT_CURRENT,
	                                             keyRadiusAccountingServerAddressValue,
	                                             tempRadiusAccountingServerAddressType,
	                                             kwaRadiusAccountingServerNameValue) == L7_SUCCESS)
	  {
	      while (wait_time && usmDbRadiusAccountingServerIPHostNameByIndexGet
               (L7_UNIT_CURRENT, keyRadiusAccountingServerIndexValue,
                keyRadiusAccountingServerAddressValue, &addrType) != L7_SUCCESS)
	      {
	          /* sleep for a microsecond to give the Radius component 
	                  time to process the add request */
	         osapiSleepUSec(1);
	         wait_time--;
	      }
    }
    else
    {
      owa.rc = XLIBRC_RADIUS_ACCOUNTING_SERVER_ADD_FAILURE;    /* TODO: Add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    /* Creation of index successful Push the index into the filter*/
    /* Pass type as 0 */
    owa.rc = xLibFilterSet(wap,XOBJ_securityRadius_RadiusAccountingServerIndex,0,
                          (xLibU8_t *) &keyRadiusAccountingServerIndexValue,
                          sizeof(keyRadiusAccountingServerIndexValue)); 

    if(owa.rc !=XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  else if (objRadiusAccountingStatusValue == L7_ROW_STATUS_DESTROY)
  {


 /* retrieve  RadiusAccountingServerAddress */
    kwaRadiusAccountingServerAddress.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerAddress,
                                                         (xLibU8_t *) keyRadiusAccountingServerAddressValue,
                                                         &kwaRadiusAccountingServerAddress.len);
    if (kwaRadiusAccountingServerAddress.rc != XLIBRC_SUCCESS)
    {
      kwaRadiusAccountingServerAddress.rc =XLIBRC_RADIUS_ACCOUNTING_SERVER_DEL_FAILURE ;
      FPOBJ_TRACE_EXIT (bufp, kwaRadiusAccountingServerAddress);
      return kwaRadiusAccountingServerAddress.rc;
    }

   /*Changes done to handle the situation where do  not get Type valu from web.
     Have To determine manually*/

    kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerAddressType,
                            (xLibU8_t *) & tempRadiusAccountingServerAddressType,
                            &kwa.len);
    if(usmDbIPHostAddressValidate(keyRadiusAccountingServerAddressValue, &ipAddr,&addrType) != L7_SUCCESS)
    {
         owa.rc = XLIBRC_RADIUS_ACCOUNTING_SERVER_DEL_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
    }

    if (kwa.rc !=  XLIBRC_SUCCESS)
    {
       tempRadiusAccountingServerAddressType = addrType;
    }
    else
    {
       if(addrType != tempRadiusAccountingServerAddressType)
      {
        owa.rc = XLIBRC_RADIUS_ACCOUNTING_SERVER_DEL_FAILURE;     /*TODO: Add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }


	
    /* Delete the existing row */
    owa.l7rc = usmDbRadiusAccountingHostNameServerRemove(L7_UNIT_CURRENT, 
              keyRadiusAccountingServerAddressValue, tempRadiusAccountingServerAddressType);
    if(owa.l7rc != L7_SUCCESS )
   {
     owa.rc = XLIBRC_RADIUS_ACCOUNTING_SERVER_DEL_FAILURE;  /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingName
*
* @purpose Get 'RadiusAccountingName'
 *@description  [RadiusAccountingName] Configured identification name for the
* RADIUS Accounting server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingName (void *wap, void *bufp)
{

  fpObjWa_t kwaRadiusAccountingServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusAccountingNameValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusAccountingServerIndex */
  kwaRadiusAccountingServerIndex.rc =
    xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                   (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                   &kwaRadiusAccountingServerIndex.len);
  if (kwaRadiusAccountingServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusAccountingServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusAccountingServerIndex);
    return kwaRadiusAccountingServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue,
                           kwaRadiusAccountingServerIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc = usmDbRadiusServerAcctHostNameGet (L7_UNIT_CURRENT, addrValueStr,addrType,
                                                                          objRadiusAccountingNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusAccountingName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRadiusAccountingNameValue,
                           strlen (objRadiusAccountingNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingName
*
* @purpose Set 'RadiusAccountingName'
 *@description  [RadiusAccountingName] Configured identification name for the
* RADIUS Accounting server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusAccountingNameValue;
  xLibStr256_t tempObjRadiusAccountingNameValue;
  fpObjWa_t kwaRadiusAccountingServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusAccountingServerIndexValue;
 xLibU32_t  wait_time= 10;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusAccountingName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objRadiusAccountingNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRadiusAccountingNameValue, owa.len);

  /* retrieve key: RadiusAccountingServerIndex */
  kwaRadiusAccountingServerIndex.rc =
    xLibFilterGet (wap, XOBJ_securityRadius_RadiusAccountingServerIndex,
                   (xLibU8_t *) & keyRadiusAccountingServerIndexValue,
                   &kwaRadiusAccountingServerIndex.len);
  if (kwaRadiusAccountingServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusAccountingServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusAccountingServerIndex);
    return kwaRadiusAccountingServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusAccountingServerIndexValue,
                           kwaRadiusAccountingServerIndex.len);

  /* set the value in application */

  owa.l7rc = usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                                     keyRadiusAccountingServerIndexValue,
                                                     addrValueStr,
                                                     &addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  owa.l7rc = usmDbRadiusAccountingServerNameSet (L7_UNIT_CURRENT, addrValueStr,addrType,
                                                 objRadiusAccountingNameValue);

  memset(tempObjRadiusAccountingNameValue,0x00,sizeof(tempObjRadiusAccountingNameValue));
  while((wait_time && (usmDbRadiusServerAcctHostNameGet (L7_UNIT_CURRENT, addrValueStr,addrType,
                                                         tempObjRadiusAccountingNameValue)==L7_SUCCESS)))
  {
     if (strncmp(tempObjRadiusAccountingNameValue,objRadiusAccountingNameValue,sizeof(tempObjRadiusAccountingNameValue))==0)
        break;
     osapiSleepUSec(1);
     wait_time--;
  }
 
 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerIndex
*
* @purpose Get 'RadiusServerIndex'
*
* @description [RadiusServerIndex]: Unique index of the configured RADIUS
*              server 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerIndexValue;
  xLibU32_t nextObjRadiusServerIndexValue;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & objRadiusServerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjRadiusServerIndexValue = 0;
    objRadiusServerIndexValue = 0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objRadiusServerIndexValue, owa.len);
  owa.l7rc =
		fpObjUtil_securityRadiusServEntryNextGet(L7_UNIT_CURRENT, &objRadiusServerIndexValue);
  
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjRadiusServerIndexValue = objRadiusServerIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRadiusServerIndexValue, owa.len);

  /* return the object value: RadiusServerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRadiusServerIndexValue,
                           sizeof (objRadiusServerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerAddress
*
* @purpose Get 'RadiusServerAddress'
*
* @description [RadiusServerAddress]: IP Address of the configured RADIUS
*              server. This object cannot be changed after creation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusServerAddressValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     objRadiusServerAddressValue,&addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRadiusServerAddressValue,
                           strlen (objRadiusServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerAddress
*
* @purpose Set 'RadiusServerAddress'
*
* @description [RadiusServerAddress]: IP Address of the configured RADIUS
*              server. This object cannot be changed after creation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusServerAddressValue;
  
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusServerAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objRadiusServerAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRadiusServerAddressValue, owa.len);

   /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);
  
 /* retrieve row status object value */
 /* kwaServStatus.rc =
    xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerStatus,
                   (xLibU8_t *) &objRadiusServerStatusValue, &kwaServStatus.len);
  if (kwaServStatus.rc != XLIBRC_SUCCESS)
  {
    kwaServStatus.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaServStatus);
    return kwaServStatus.rc;
  }*/
  owa.rc = XLIBRC_SUCCESS;    /*return success as it will be handled in the row status object */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerAddressType
*
* @purpose Get 'RadiusServerAddressType'
*
* @description [RadiusServerAddressType]: IP Address Type of the configured
*              RADIUS server. This object cannot be changed after creation.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerAddressType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerAddressTypeValue;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&objRadiusServerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusServerAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerAddressTypeValue,
                           sizeof (objRadiusServerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerAddressType
*
* @purpose Set 'RadiusServerAddressType'
*
* @description [RadiusServerAddressType]: IP Address Type of the configured
*              RADIUS server. This object cannot be changed after creation.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusServerAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusServerAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusServerAddressTypeValue, owa.len);

  /* retrieve row status object value */
 /* kwaServStatus.rc =
    xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerStatus,
                   (xLibU8_t *) &objRadiusServerStatusValue, &kwaServStatus.len);
  if (kwaServStatus.rc != XLIBRC_SUCCESS)
  {
    kwaServStatus.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaServStatus);
    return kwaServStatus.rc;
  }*/
 
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerPort
*
* @purpose Get 'RadiusServerPort'
*
* @description [RadiusServerPort]: Port number for the RADIUS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerPort (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerPortValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusHostNameServerPortNumGet (L7_UNIT_CURRENT,
                                         addrValueStr,addrType,
                                         &objRadiusServerPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusServerPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerPortValue,
                           sizeof (objRadiusServerPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerPort
*
* @purpose Set 'RadiusServerPort'
*
* @description [RadiusServerPort]: Port number for the RADIUS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerPortValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t tempObjRadiusServerPortValue;
  xLibU32_t keyRadiusServerIndexValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);
  L7_uint32 wait_time = 10; /* controls how long to wait for Radius server to process create request */

  /* retrieve object: RadiusServerPort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusServerPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusServerPortValue, owa.len);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusHostNameServerPortNumSet (L7_UNIT_CURRENT,
                                         addrValueStr,addrType,
                                         objRadiusServerPortValue);
     
   while((wait_time && (usmDbRadiusHostNameServerPortNumGet(L7_UNIT_CURRENT,addrValueStr,
                                                            addrType,&tempObjRadiusServerPortValue)==L7_SUCCESS)))
  {
     if (tempObjRadiusServerPortValue==objRadiusServerPortValue)
        break;
     osapiSleepUSec(1);
     wait_time--;
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
* @function fpObjGet_securityRadius_RadiusServerSecret
*
* @purpose Get 'RadiusServerSecret'
*
* @description [RadiusServerSecret]: Configured shared sercret for the RADIUS
*              server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerSecret (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusServerSecretValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  L7_BOOL isconfigured;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t dummyVaribale;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if((usmDbRadiusHostNameServerSharedSecretConfigured( addrValueStr,&isconfigured) == L7_SUCCESS) && isconfigured == L7_TRUE)
  {
	  owa.l7rc =
	    usmDbRadiusHostNameServerSharedSecretGet (
	                                              addrValueStr,
	                                              objRadiusServerSecretValue,
	                                              &dummyVaribale);
	  if (owa.l7rc != L7_SUCCESS)
	  {
	    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
  }
  else
  {
	owa.rc = XLIBRC_FAILURE;    /* TODO: Add error string */
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
  }

  /* return the object value: RadiusServerSecret */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRadiusServerSecretValue,
                           strlen (objRadiusServerSecretValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerSecret
*
* @purpose Set 'RadiusServerSecret'
*
* @description [RadiusServerSecret]: Configured shared sercret for the RADIUS
*              server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerSecret (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusServerSecretValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  xLibStr256_t addrValueStr;
  xLibStr256_t tempObjRadiusServerSecretValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t  wait_time = 10;/*time for the radius server to process command*/
  /* retrieve object: RadiusServerSecret */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objRadiusServerSecretValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRadiusServerSecretValue, owa.len);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  /*need to know wether this is needed*/ 
 /* if((usmDbRadiusHostNameServerSharedSecretConfigured( addrValueStr,&isconfigured) == L7_SUCCESS) && isconfigured == L7_TRUE)
  {*/
  
    owa.l7rc =
       usmDbRadiusHostNameServerSharedSecretSet (
                                              addrValueStr,
                                              objRadiusServerSecretValue,
                                              L7_RADIUSLOCAL);
      memset(tempObjRadiusServerSecretValue,0x00,sizeof(tempObjRadiusServerSecretValue));
     while((wait_time && (usmDbRadiusHostNameServerSharedSecretGet (
                                                addrValueStr,
                                                objRadiusServerSecretValue,
                                                &paramStatus)==L7_SUCCESS)))
     {
         if (strncmp(objRadiusServerSecretValue,tempObjRadiusServerSecretValue,sizeof(objRadiusServerSecretValue))==0)
           break;
         osapiSleepUSec(1);
         wait_time--;
    }

  /*}
  else
  {
      owa.rc = XLIBRC_FAILURE;    
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }*/

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
* @function fpObjGet_securityRadius_RadiusServerPrimaryMode
*
* @purpose Get 'RadiusServerPrimaryMode'
*
* @description [RadiusServerPrimaryMode]: Configure the RADIUS server to be
*              the primary server. If there is any other server that is
*              configured to be primary, that server is set to be a seconday
*              server and this entry is set Primary. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerPrimaryMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerPrimaryModeValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusHostNameServerEntryTypeGet (L7_UNIT_CURRENT,
                                           addrValueStr,addrType,
                                           &objRadiusServerPrimaryModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusServerPrimaryMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerPrimaryModeValue,
                           sizeof (objRadiusServerPrimaryModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerPrimaryMode
*
* @purpose Set 'RadiusServerPrimaryMode'
*
* @description [RadiusServerPrimaryMode]: Configure the RADIUS server to be
*              the primary server. If there is any other server that is
*              configured to be primary, that server is set to be a seconday
*              server and this entry is set Primary. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerPrimaryMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerPrimaryModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibU32_t tempObjRadiusServerPrimaryModeValue;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  xLibU32_t  wait_time = 10;/*time for the radius server to process command*/
  FPOBJ_TRACE_ENTER (bufp);
 

  /* retrieve object: RadiusServerPrimaryMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusServerPrimaryModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusServerPrimaryModeValue, owa.len);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc =
    usmDbRadiusHostNameServerEntryTypeSet (L7_UNIT_CURRENT,
                                           addrValueStr,addrType,
                                           objRadiusServerPrimaryModeValue);
    while((wait_time && (usmDbRadiusHostNameServerEntryTypeGet (L7_UNIT_CURRENT,
                                           addrValueStr,addrType,
                                           &tempObjRadiusServerPrimaryModeValue)==L7_SUCCESS)))
   {
         if (tempObjRadiusServerPrimaryModeValue==objRadiusServerPrimaryModeValue)
           break;
         osapiSleepUSec(1);
         wait_time--;
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
* @function fpObjGet_securityRadius_RadiusServerCurrentMode
*
* @purpose Get 'RadiusServerCurrentMode'
*
* @description [RadiusServerCurrentMode]: Indicate if the RADIUS server is
*              the current server in user for authentication. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerCurrentMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerCurrentModeValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  xLibStr256_t addrValueStrTemp;
  memset(addrValueStrTemp,0x00,sizeof(addrValueStrTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbRadiusCurrentAuthHostNameServerGet (L7_UNIT_CURRENT,
                                     addrValueStrTemp);
  if( owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(strcmp(addrValueStrTemp,addrValueStr) != 0)
  {
    objRadiusServerCurrentModeValue = L7_FALSE;
  }
  else
  {
    objRadiusServerCurrentModeValue = L7_TRUE;
  }
  
  /* return the object value: RadiusServerCurrentMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerCurrentModeValue,
                           sizeof (objRadiusServerCurrentModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerMsgAuth
*
* @purpose Get 'RadiusServerMsgAuth'
*
* @description [RadiusServerMsgAuth]: Enable or disable the message authenticator
*              attribute for this RADIUS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerMsgAuth (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerMsgAuthValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibU32_t tempStatusVal;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusHostNameServerIncMsgAuthModeGet (L7_UNIT_CURRENT,
                                                addrValueStr,addrType,
                                                &tempStatusVal);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(tempStatusVal == L7_ENABLE)
  {
    objRadiusServerMsgAuthValue = L7_ENABLE;
  }
  else
  {
    objRadiusServerMsgAuthValue = L7_DISABLE;
  }

  /* return the object value: RadiusServerMsgAuth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerMsgAuthValue,
                           sizeof (objRadiusServerMsgAuthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerMsgAuth
*
* @purpose Set 'RadiusServerMsgAuth'
*
* @description [RadiusServerMsgAuth]: Enable or disable the message authenticator
*              attribute for this RADIUS server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerMsgAuth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerMsgAuthValue;  
  xLibU32_t tempObjRadiusServerMsgAuthValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibU32_t  wait_time = 10;/*time for the radius server to process command*/
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusServerMsgAuth */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusServerMsgAuthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusServerMsgAuthValue, owa.len);

 
  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusHostNameServerIncMsgAuthModeSet (L7_UNIT_CURRENT,
                                                addrValueStr,addrType,
                                                objRadiusServerMsgAuthValue);
   while((wait_time && (usmDbRadiusHostNameServerIncMsgAuthModeGet (L7_UNIT_CURRENT,
                                                addrValueStr,addrType,
                                                &tempObjRadiusServerMsgAuthValue)==L7_SUCCESS )))
   {
         if (objRadiusServerMsgAuthValue==tempObjRadiusServerMsgAuthValue)
           break;
         osapiSleepUSec(1);
         wait_time--;
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
* @function fpObjGet_securityRadius_RadiusServerIsCurrentAuthServer
*
* @purpose Get 'RadiusServerIsCurrentAuthServer'
*
* @description [RadiusServerIsCurrentAuthServer] Check if the server is current active Authentication server.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerIsCurrentAuthServer (void *wap, void *bufp)
{

  fpObjWa_t kwaRadiusServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  xLibStr256_t addrValueStr;
  xLibStr256_t addrValueStrTemp;
  xLibStr256_t objRadiusServerNameValue;
  xLibU32_t objRadiusServerIsCurrentAuthServerValue;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  memset(addrValueStr,0x00,sizeof(addrValueStr));
  memset(addrValueStrTemp,0x00,sizeof(addrValueStrTemp));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwaRadiusServerIndex.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                                           (xLibU8_t *) & keyRadiusServerIndexValue,
                                           &kwaRadiusServerIndex.len);
  if (kwaRadiusServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusServerIndex);
    return kwaRadiusServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwaRadiusServerIndex.len);


  /* Get the DNS-Host-Name or Host-IP-Address of the RADIUS Server pointed by keyRadiusServerIndexValue. */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Get the ServerName of the RadiusServer pointed by keyRadiusServerIndexValue. */
  owa.l7rc = usmDbRadiusServerHostNameGet (L7_UNIT_CURRENT, addrValueStr, addrType,
                                                           objRadiusServerNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Now get the DNS-Host-Name or Host-IP-Address of the Current-active Server in the group of RADIUS Servers
   which have the same group ServerName as that of the RADIUS Server pointed by keyRadiusServerIndexValue.*/
  owa.l7rc = usmDbRadiusAuthServerNameCorrespondingCurrentServerGet(L7_UNIT_CURRENT,
                                          objRadiusServerNameValue, addrValueStrTemp);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Now check if the Radius-Sever pointed by keyRadiusServerIndexValue is the Current active RADIUS Server
     in its group that bear the same name. */
  if(osapiStrncmp(addrValueStrTemp,addrValueStr,sizeof(addrValueStr)) != 0)
  {
    objRadiusServerIsCurrentAuthServerValue = L7_FALSE;
  }
  else
  {
    objRadiusServerIsCurrentAuthServerValue = L7_TRUE;
  }

  /* return the object value: RadiusServerIsCurrentAuthServer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerIsCurrentAuthServerValue,
                           sizeof (objRadiusServerIsCurrentAuthServerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerStatus
*
* @purpose Get 'RadiusServerStatus'
*
* @description [RadiusServerStatus]: Creates or destroys a RADIUS Authentication
*              server entry.During creation, the next available index
*              is specified by the RadiusServerIndexNextValid object. 
*              Rows creation usinga different value for RadiusServerIndex
*              will fail.active(1)  - This entry is active. createAndGo(4)
*               - Creates a new entry. destroy(6)- Deletes an entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                          (xLibU8_t *) & keyRadiusServerIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwa.len);

  /* get the value from application */
 objRadiusServerStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RadiusServerStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerStatusValue,
                           sizeof (objRadiusServerStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerStatus
*
* @purpose Set 'RadiusServerStatus'
*
* @description [RadiusServerStatus]: Creates or destroys a RADIUS Authentication
*              server entry.During creation, the next available index
*              is specified by the RadiusServerIndexNextValid object. 
*              Rows creation usinga different value for RadiusServerIndex
*              will fail.active(1)  - This entry is active. createAndGo(4)
*               - Creates a new entry. destroy(6)- Deletes an entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerStatusValue;
  xLibU32_t keyRadiusServerIndexValue;
  
  fpObjWa_t kwaAddrType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t tempRadiusServerAddrTypeValue;

  fpObjWa_t kwaServAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t tempRadiusServerAddressValue;
  
  fpObjWa_t kwaServName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t tempServName;
  
  FPOBJ_TRACE_ENTER (bufp);

  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 ipaddrCheck;
  L7_uint32 tempServIndex;
  L7_uint32 wait_time = 10;

  /* retrieve object: RadiusServerStatus */
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *) & objRadiusServerStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusServerStatusValue, owa.len);


  /* call the usmdb only for add and delete */
  if (objRadiusServerStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc =  fpObjUtil_securityRadiusServIndexNextValidGet(&tempServIndex);
    if ((owa.l7rc != L7_SUCCESS))
    {
      owa.rc = XLIBRC_RADIUS_SERVER_ADD_FAILURE;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    keyRadiusServerIndexValue = tempServIndex;

    kwaServAddr.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerAddress,
                          (xLibU8_t *) & tempRadiusServerAddressValue, &kwaServAddr.len);
    if (kwaServAddr.rc != XLIBRC_SUCCESS)
    {
      kwaServAddr.rc = XLIBRC_RADIUS_SERVER_ADD_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, kwaServAddr);
      return kwaServAddr.rc;
    }

    kwaAddrType.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerAddressType,
	                      (xLibU8_t *) & tempRadiusServerAddrTypeValue, &kwaAddrType.len);
   /*Changes done to handle the situation where do  not get Type valu from web.
     Have To determine manually*/

   /* We should validate ipaddress. wheather it is dns name or dotted format ip address */
    if(usmDbIPHostAddressValidate(tempRadiusServerAddressValue,&ipaddrCheck, &addrType) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_INVALID_IP_ADDRESS;    /* TODO: Add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if (kwaAddrType.rc != XLIBRC_SUCCESS)
    {
      tempRadiusServerAddrTypeValue = addrType;
    }
    else
    {
      if(addrType != tempRadiusServerAddrTypeValue)
      {
        owa.rc = XLIBRC_INVALID_IP_ADDRESS;    /* TODO: Add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

    }

    kwaServName.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerName,
                          (xLibU8_t *) tempServName, &kwaServName.len);
    if (kwaServName.rc != XLIBRC_SUCCESS)
    {
      kwaServName.rc = XLIBRC_RADIUS_SERVER_ADD_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, kwaServName);
      return kwaServName.rc;
    }


    if(usmDbRadiusIPHostNameServerAdd(L7_UNIT_CURRENT, tempRadiusServerAddressValue,
                     tempRadiusServerAddrTypeValue,tempServName) == L7_SUCCESS)
    {
      while (wait_time && usmDbRadiusServerIPHostNameByIndexGet(L7_UNIT_CURRENT, 
              keyRadiusServerIndexValue, tempRadiusServerAddressValue, &addrType) != L7_SUCCESS)
      {
      /* sleep for a microsecond to give the Radius component time 
         to process the add request */
         osapiSleepUSec(1);
         wait_time--;
      }
    }
    else
    {
      owa.rc = XLIBRC_RADIUS_SERVER_ADD_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    
    owa.rc = xLibFilterSet(wap,XOBJ_securityRadius_RadiusServerIndex,0,(xLibU8_t *) &keyRadiusServerIndexValue,
                           sizeof(keyRadiusServerIndexValue)); 
    if(owa.rc !=XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
	
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRadiusServerStatusValue == L7_ROW_STATUS_DESTROY)
  {

	  /* retrieve key: RadiusServerAddress */
	    kwaServAddr.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerAddress,
	                          (xLibU8_t *) & tempRadiusServerAddressValue, &kwaServAddr.len);
	    if (kwaServAddr.rc != XLIBRC_SUCCESS)
	    {
	      kwaServAddr.rc = XLIBRC_RADIUS_SERVER_DEL_FAILURE;    /* TODO: Change if required */
	      FPOBJ_TRACE_EXIT (bufp, kwaServAddr);
	      return kwaServAddr.rc;
	    }

	    kwaAddrType.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerAddressType,
		                      (xLibU8_t *) & tempRadiusServerAddrTypeValue, &kwaAddrType.len);
	   /*Changes done to handle the situation where do  not get Type valu from web.
	     Have To determine manually*/

	   /* We should validate ipaddress. wheather it is dns name or dotted format ip address */
	    if(usmDbIPHostAddressValidate(tempRadiusServerAddressValue,&ipaddrCheck, &addrType) != L7_SUCCESS)
	    {
	      owa.rc = XLIBRC_RADIUS_SERVER_DEL_FAILURE;    /* TODO: Add error string */
	      FPOBJ_TRACE_EXIT (bufp, owa);
	      return owa.rc;
	    }

	    if (kwaAddrType.rc != XLIBRC_SUCCESS)
	    {
	      tempRadiusServerAddrTypeValue = addrType;
	    }
	    else
	    {
	      if(addrType != tempRadiusServerAddrTypeValue)
	      {
	        owa.rc = XLIBRC_RADIUS_SERVER_DEL_FAILURE;    /* TODO: Add error string */
	        FPOBJ_TRACE_EXIT (bufp, owa);
	        return owa.rc;
	      }

	    }
    /* Delete the existing row */
	  owa.l7rc = usmDbRadiusAuthHostNameServerRemove(L7_UNIT_CURRENT,
		               tempRadiusServerAddressValue, tempRadiusServerAddrTypeValue);	    

	  if (owa.l7rc != L7_SUCCESS)
	  {
		  owa.rc = XLIBRC_RADIUS_SERVER_DEL_FAILURE;  /* TODO: Change if required */
		  FPOBJ_TRACE_EXIT (bufp, owa);
		  return owa.rc;
	  }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerName
*
* @purpose Get 'RadiusServerName'
 *@description  [RadiusServerName] Configured identification name for the RADIUS
* Accounting server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerName (void *wap, void *bufp)
{

  fpObjWa_t kwaRadiusServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusServerNameValue;

  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwaRadiusServerIndex.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                                           (xLibU8_t *) & keyRadiusServerIndexValue,
                                           &kwaRadiusServerIndex.len);
  if (kwaRadiusServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusServerIndex);
    return kwaRadiusServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwaRadiusServerIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc = usmDbRadiusServerHostNameGet (L7_UNIT_CURRENT, addrValueStr, addrType,
                                                                     objRadiusServerNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RadiusServerName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRadiusServerNameValue,
                           strlen (objRadiusServerNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusServerName
*
* @purpose Set 'RadiusServerName'
 *@description  [RadiusServerName] Configured identification name for the RADIUS
* Accounting server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusServerName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRadiusServerNameValue;

  fpObjWa_t kwaRadiusServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;

  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusServerName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objRadiusServerNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRadiusServerNameValue, owa.len);

  /* retrieve key: RadiusServerIndex */
  kwaRadiusServerIndex.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                                           (xLibU8_t *) & keyRadiusServerIndexValue,
                                           &kwaRadiusServerIndex.len);
  if (kwaRadiusServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusServerIndex);
    return kwaRadiusServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwaRadiusServerIndex.len);
 
  /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
 /* set the value in application */
  owa.l7rc = usmDbRadiusServerNameSet (L7_UNIT_CURRENT, addrValueStr, addrType, objRadiusServerNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_securityRadius_IsRadiusServerSharedSecretConfiguredSet
*
* @purpose Get 'IsRadiusServerSharedSecretConfiguredSet'
*
* @description [IsRadiusServerSharedSecretConfiguredSet] To check whether Radius HostName Server Shared Secret is configured.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_IsRadiusServerSharedSecretConfiguredSet (void *wap, void *bufp)
{

  fpObjWa_t kwaRadiusServerIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRadiusServerIndexValue;

  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsRadiusServerSharedSecretConfiguredSetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RadiusServerIndex */
  kwaRadiusServerIndex.rc = xLibFilterGet (wap, XOBJ_securityRadius_RadiusServerIndex,
                                           (xLibU8_t *) & keyRadiusServerIndexValue,
                                           &kwaRadiusServerIndex.len);
  if (kwaRadiusServerIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRadiusServerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRadiusServerIndex);
    return kwaRadiusServerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRadiusServerIndexValue, kwaRadiusServerIndex.len);

      /* get the value from application */
  owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     keyRadiusServerIndexValue,
                                     addrValueStr,&addrType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  /* get the value from application */
  owa.l7rc = usmDbRadiusHostNameServerSharedSecretConfigured ( addrValueStr,
                               &objIsRadiusServerSharedSecretConfiguredSetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsRadiusServerSharedSecretConfiguredSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsRadiusServerSharedSecretConfiguredSetValue,
                           sizeof (objIsRadiusServerSharedSecretConfiguredSetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusAccountingIndexNextValid
*
* @purpose Get 'RadiusAccountingIndexNextValid'
*
* @description [RadiusAccountingIndexNextValid]: Indicates the next valid
*              index into the RadiusAccountingConfigTable for creation. If
*              no additional entries are allowed, this will be 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingIndexNextValid (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingIndexNextValidValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                              &objRadiusAccountingIndexNextValidValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusAccountingIndexNextValidValue,
                     sizeof (objRadiusAccountingIndexNextValidValue));

  /* return the object value: RadiusAccountingIndexNextValid */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusAccountingIndexNextValidValue,
                    sizeof (objRadiusAccountingIndexNextValidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusStatsClear
*
* @purpose Set 'RadiusStatsClear'
*
* @description [RadiusStatsClear]: When set to enable(1), all Radius statistics
*              will be reset. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusStatsClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusStatsClearValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusStatsClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusStatsClearValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbRadiusStatsClear (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    objRadiusStatsClearValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objRadiusStatsClearValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusServerIndexNextValid
*
* @purpose Get 'RadiusServerIndexNextValid'
*
* @description [RadiusServerIndexNextValid]: Indicates the next valid index
*              into the RadiusServerConfigTable for creation. If no additional
*              entries are allowed, this will be 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusServerIndexNextValid (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusServerIndexNextValidValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    fpObjUtil_securityRadiusServEntryNextGet (L7_UNIT_CURRENT,
                               &objRadiusServerIndexNextValidValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusServerIndexNextValidValue,
                     sizeof (objRadiusServerIndexNextValidValue));

  /* return the object value: RadiusServerIndexNextValid */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusServerIndexNextValidValue,
                    sizeof (objRadiusServerIndexNextValidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_RadiusMaxTransmit
*
* @purpose Get 'RadiusMaxTransmit'
*
* @description [RadiusMaxTransmit]: Maximum number of retransmissions of a
*              RADIUS request packet 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusMaxTransmit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusMaxTransmitValue;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  L7_uchar8 strIpAddr[20];
  FPOBJ_TRACE_ENTER (bufp);

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerRetransGet (strIpAddr, &objRadiusMaxTransmitValue,&paramStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusMaxTransmitValue,
                     sizeof (objRadiusMaxTransmitValue));

  /* return the object value: RadiusMaxTransmit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusMaxTransmitValue,
                           sizeof (objRadiusMaxTransmitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusMaxTransmit
*
* @purpose Set 'RadiusMaxTransmit'
*
* @description [RadiusMaxTransmit]: Maximum number of retransmissions of a
*              RADIUS request packet 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusMaxTransmit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusMaxTransmitValue;
  L7_uchar8 strIpAddr[20];
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RadiusMaxTransmit */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusMaxTransmitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusMaxTransmitValue, owa.len);

  /* set the value in application */
  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);
  owa.l7rc =
    usmDbRadiusServerRetransSet (strIpAddr, objRadiusMaxTransmitValue, L7_RADIUSGLOBAL);
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
* @function fpObjGet_securityRadius_RadiusTimeout
*
* @purpose Get 'RadiusTimeout'
*
* @description [RadiusTimeout]: Time out duration (in seconds) before packets
*              are retransmitted 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusTimeoutValue;
	
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  L7_uchar8 strIpAddr[20];

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);
	
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerTimeOutGet (strIpAddr, &objRadiusTimeoutValue,&paramStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusTimeoutValue,
                     sizeof (objRadiusTimeoutValue));

  /* return the object value: RadiusTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusTimeoutValue,
                           sizeof (objRadiusTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusTimeout
*
* @purpose Set 'RadiusTimeout'
*
* @description [RadiusTimeout]: Time out duration (in seconds) before packets
*              are retransmitted 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusTimeoutValue;
  xLibU32_t tempObjRadiusTimeoutValue;
	
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  L7_uchar8 strIpAddr[20];

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);
	
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t  wait_time = 10;/*time for the radius server to process command*/
  /* retrieve object: RadiusTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusTimeoutValue, owa.len);

  /* set the value in application */
    
  owa.l7rc = usmDbRadiusServerTimeOutSet (strIpAddr, objRadiusTimeoutValue, L7_RADIUSGLOBAL);
  while((wait_time && (usmDbRadiusServerTimeOutGet (strIpAddr, &tempObjRadiusTimeoutValue,&paramStatus))==L7_SUCCESS))
  {
     if (tempObjRadiusTimeoutValue==objRadiusTimeoutValue)
        break;
     osapiSleepUSec(1);
     wait_time--;
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
* @function fpObjGet_securityRadius_RadiusAccountingMode
*
* @purpose Get 'RadiusAccountingMode'
*
* @description [RadiusAccountingMode]: Identifies if RADIUS Accounting has
*              been enabled or not 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_RadiusAccountingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingModeGet (L7_UNIT_CURRENT,
                                  &objRadiusAccountingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusAccountingModeValue,
                     sizeof (objRadiusAccountingModeValue));

  /* return the object value: RadiusAccountingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRadiusAccountingModeValue,
                           sizeof (objRadiusAccountingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securityRadius_RadiusAccountingMode
*
* @purpose Set 'RadiusAccountingMode'
*
* @description [RadiusAccountingMode]: Identifies if RADIUS Accounting has
*              been enabled or not 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_RadiusAccountingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRadiusAccountingModeValue;
  xLibU32_t tempObjRadiusAccountingModeValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t wait_time = 10;/*time for radius server to process req*/
  /* retrieve object: RadiusAccountingMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRadiusAccountingModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRadiusAccountingModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbRadiusAccountingModeSet (L7_UNIT_CURRENT,
                                  objRadiusAccountingModeValue);
     while((wait_time && (usmDbRadiusAccountingModeGet(L7_UNIT_CURRENT, &tempObjRadiusAccountingModeValue)==L7_SUCCESS)))
  {
     if (tempObjRadiusAccountingModeValue==objRadiusAccountingModeValue)
        break;
     osapiSleepUSec(1);
     wait_time--;
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
* @function fpObjGet_securityRadius_radiusAccServerIndex
*
* @purpose Get 'radiusAccServerIndex'
*
* @description [radiusAccServerIndex]: A number uniquely identifying each
*              RADIUS Accounting server with which this client communicates.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccServerIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccServerIndexValue;
  xLibU32_t nextObjradiusAccServerIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & objradiusAccServerIndexValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjradiusAccServerIndexValue = 0;
    owa.l7rc = fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                     &nextObjradiusAccServerIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objradiusAccServerIndexValue, owa.len);
    owa.l7rc =
      fpObjUtil_securityRadiusAccConfigEntryNextGet (L7_UNIT_CURRENT,
                                                       &objradiusAccServerIndexValue);
    nextObjradiusAccServerIndexValue = objradiusAccServerIndexValue;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjradiusAccServerIndexValue, owa.len);

  /* return the object value: radiusAccServerIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjradiusAccServerIndexValue,
                    sizeof (objradiusAccServerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccServerAddress
*
* @purpose Get 'radiusAccServerAddress'
*
* @description [radiusAccServerAddress]: The IP address of the RADIUS accounting
*              server referred to in this table entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccServerAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccServerAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
    
  owa.l7rc =
    usmDbRadiusAccountingServerIPByIndexGet (L7_UNIT_CURRENT,
                                             keyradiusAccServerIndexValue,
                                             &objradiusAccServerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAccServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccServerAddressValue,
                           sizeof (objradiusAccServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientServerPortNumber
*
* @purpose Get 'radiusAccClientServerPortNumber'
*
* @description [radiusAccClientServerPortNumber]: The UDP port the client
*              is using to send requests to this server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientServerPortNumber (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientServerPortNumberValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAccountingHostNameServerPortNumGet (L7_UNIT_CURRENT,
                                                   addrValueStr, addrType,
                                                   &objradiusAccClientServerPortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAccClientServerPortNumber */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAccClientServerPortNumberValue,
                    sizeof (objradiusAccClientServerPortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientRoundTripTime
*
* @purpose Get 'radiusAccClientRoundTripTime'
*
* @description [radiusAccClientRoundTripTime]: The time interval between the
*              most recent Accounting-Response and the Accounting-Request
*              that matched it from this RADIUS accounting server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientRoundTripTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientRoundTripTimeValue;
  L7_double64 rTTinSec; 
  xLibStr256_t roundTripTime;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatRTTGet (L7_UNIT_CURRENT,
                                     addrValueStr,
                                     &objradiusAccClientRoundTripTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  rTTinSec = objradiusAccClientRoundTripTimeValue;
  rTTinSec = rTTinSec/100;
  osapiSnprintf(roundTripTime, sizeof (roundTripTime), "%.2f ", rTTinSec);

  /* return the object value in string: radiusAccClientRoundTripTime */
  owa.rc =
    xLibBufDataSet (bufp, roundTripTime,
                    strlen(roundTripTime));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientRequests
*
* @purpose Get 'radiusAccClientRequests'
*
* @description [radiusAccClientRequests]: The number of RADIUS Accounting-Request
*              packets sent. This does not include retransmissions.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientRequests (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientRequestsValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbRadiusAcctServerStatReqGet(L7_UNIT_CURRENT, addrValueStr, 
                                      &objradiusAccClientRequestsValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  
  /* return the object value: radiusAccClientRequests */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientRequestsValue,
                           sizeof (objradiusAccClientRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientRetransmissions
*
* @purpose Get 'radiusAccClientRetransmissions'
*
* @description [radiusAccClientRetransmissions]: The number of RADIUS Accounting-Request
*              packets retransmitted to this RADIUS accounting
*              server. Retransmissions include retries where the Identifier
*              and Acct-Delay have been updated, as well as those in
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientRetransmissions (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientRetransmissionsValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatRetransGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientRetransmissionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAccClientRetransmissions */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientRetransmissionsValue,
                    sizeof (objradiusAccClientRetransmissionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientResponses
*
* @purpose Get 'radiusAccClientResponses'
*
* @description [radiusAccClientResponses]: The number of RADIUS packets received
*              on the accounting port from this server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientResponses (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientResponsesValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */

  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatResponseGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 

  /* return the object value: radiusAccClientResponses */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientResponsesValue,
                    sizeof (objradiusAccClientResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientMalformedResponses
*
* @purpose Get 'radiusAccClientMalformedResponses'
*
* @description [radiusAccClientMalformedResponses]: The number of malformed
*              RADIUS Accounting-Response packets received from this server.
*              Malformed packets include packets with an invalid length.
*              Bad authenticators and unknown types are not included as
*              m 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientMalformedResponses (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientMalformedResponsesValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
   owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatMalformedResponseGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientMalformedResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   /* return the object value: radiusAccClientMalformedResponses */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAccClientMalformedResponsesValue,
                    sizeof (objradiusAccClientMalformedResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientBadAuthenticators
*
* @purpose Get 'radiusAccClientBadAuthenticators'
*
* @description [radiusAccClientBadAuthenticators]: The number of RADIUS Accounting-Response
*              packets which contained invalid authenticators
*              received from this server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientBadAuthenticators (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientBadAuthenticatorsValue;
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatBadAuthGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientBadAuthenticatorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   /* return the object value: radiusAccClientBadAuthenticators */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAccClientBadAuthenticatorsValue,
                    sizeof (objradiusAccClientBadAuthenticatorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientPendingRequests
*
* @purpose Get 'radiusAccClientPendingRequests'
*
* @description [radiusAccClientPendingRequests]: The number of RADIUS Accounting-Request
*              packets sent to this server that have not yet
*              timed out or received a response. This variable is incremented
*              when an Accounting-Request is sent and decremented due
*              to 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientPendingRequests (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientPendingRequestsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatPendingReqGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientPendingRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 

  /* return the object value: radiusAccClientPendingRequests */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientPendingRequestsValue,
                    sizeof (objradiusAccClientPendingRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientTimeouts
*
* @purpose Get 'radiusAccClientTimeouts'
*
* @description [radiusAccClientTimeouts]: The number of accounting timeouts
*              to this server. After a timeout the client may retry to the
*              same server, send to a different server, or give up. A retry
*              to the same server is counted as a retransmit as well
*              as 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientTimeouts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientTimeoutsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatTimeoutsGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientTimeoutsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

    /* return the object value: radiusAccClientTimeouts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientTimeoutsValue,
                           sizeof (objradiusAccClientTimeoutsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientUnknownTypes
*
* @purpose Get 'radiusAccClientUnknownTypes'
*
* @description [radiusAccClientUnknownTypes]: The number of RADIUS packets
*              of unknown type which were received from this server on the
*              accounting port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientUnknownTypes (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientUnknownTypesValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
 kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatUnknownTypeGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientUnknownTypesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  /* return the object value: radiusAccClientUnknownTypes */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientUnknownTypesValue,
                    sizeof (objradiusAccClientUnknownTypesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientPacketsDropped
*
* @purpose Get 'radiusAccClientPacketsDropped'
*
* @description [radiusAccClientPacketsDropped]: The number of RADIUS packets
*              which were received from this server on the accounting port
*              and dropped for some other reason. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientPacketsDropped (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAccServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientPacketsDroppedValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAccServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccServerIndex,
                          (xLibU8_t *) & keyradiusAccServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAccServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAccountingServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAccServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusAcctServerStatPktsDroppedGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAccClientPacketsDroppedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  

  /* return the object value: radiusAccClientPacketsDropped */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientPacketsDroppedValue,
                    sizeof (objradiusAccClientPacketsDroppedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientIdentifier
*
* @purpose Get 'radiusAccClientIdentifier'
*
* @description [radiusAccClientIdentifier]: The NAS-Identifier of the RADIUS
*              accounting client. This is not necessarily the same as sysName
*              in MIB II. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientIdentifier (void *wap,
                                                            void *bufp)
{
  xLibStr256_t objradiusAccClientIdentifierValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t sizeOfStr;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  usmDbRadiusNASIdentifierGet (L7_UNIT_CURRENT,
                                 objradiusAccClientIdentifierValue,&sizeOfStr);
  owa.l7rc = L7_SUCCESS;
	
  
  FPOBJ_TRACE_VALUE (bufp, objradiusAccClientIdentifierValue,
                     nasIdSize);

  /* return the object value: radiusAccClientIdentifier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objradiusAccClientIdentifierValue,
                           strlen(objradiusAccClientIdentifierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientAddressMode
*
* @purpose Get 'radiusAccClientAddressMode'
*
* @description [radiusAccClientAddressMode] Enable or Disable  NAS-IP-Address of the RADIUS authentication client referred to in this table entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientAddressMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientAddressModeValue;

  xLibU32_t tempradiusAccClientAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRadiusAttribute4Get (L7_UNIT_CURRENT, &objradiusAccClientAddressModeValue,
  	                       &tempradiusAccClientAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objradiusAccClientAddressModeValue,
                     sizeof (objradiusAccClientAddressModeValue));

  /* return the object value: radiusAccClientAddressMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientAddressModeValue,
                           sizeof (objradiusAccClientAddressModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityRadius_radiusAccClientAddress
*
* @purpose Set 'radiusAccClientAddress'
*
* @description [radiusAccClientAddress]: The NAS-IP-Address of the RADIUS
*              authentication client referred to in this table entry. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_radiusAccClientAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientAddressValue;
  xLibU32_t objradiusAccClientAddressMode;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: radiusAccClientAddressMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objradiusAccClientAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objradiusAccClientAddressValue, owa.len);

  objradiusAccClientAddressMode = L7_TRUE;
  owa.l7rc = usmDbRadiusAttribute4Set (L7_UNIT_CURRENT,objradiusAccClientAddressMode,objradiusAccClientAddressValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
 /*always return success here as the value is being set in radiusAccClientAddress object */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}




/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientAddress
*
* @purpose Get 'radiusAccClientAddress'
*
* @description [radiusAccClientAddress]: The NAS-IP-Address of the RADIUS
*              authentication client referred to in this table entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientAddressValue;
  L7_BOOL nasIpMode = L7_TRUE;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAttribute4Get (L7_UNIT_CURRENT, &nasIpMode,&objradiusAccClientAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objradiusAccClientAddressValue,
                     sizeof (objradiusAccClientAddressValue));

  /* return the object value: radiusAccClientAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAccClientAddressValue,
                           sizeof (objradiusAccClientAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securityRadius_radiusAccClientAddressMode
*
* @purpose Set 'radiusAccClientAddressMode' and 'radiusAccClientAddress'
*
* @description [radiusAccClientAddressMode] Enable or Disable  NAS-IP-Address of the RADIUS authentication client referred to
*                                           in this table entry.
*              [radiusAccClientAddress] The NAS-IP-Address of the RADIUS authentication client referred to in this table entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityRadius_radiusAccClientAddressMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientAddressValue=0;
  fpObjWa_t kwaMode = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t nasIpMode;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t  wait_time=10;/*waiting time*/
  /* retrieve object: radiusAccClientAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &nasIpMode, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objradiusAccClientAddressValue, owa.len);

   /* retrieve key: radiusAccServerIndex */
  if(nasIpMode == L7_TRUE)
  {
    kwaMode.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAccClientAddress,
                          (xLibU8_t *) & objradiusAccClientAddressValue,
                          &kwaMode.len);
    if (kwaMode.rc != XLIBRC_SUCCESS)
    {
      kwaMode.rc = XLIBRC_FAILURE;    
      FPOBJ_TRACE_EXIT (bufp, kwaMode);
      return kwaMode.rc;
    }
  } 

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbRadiusAttribute4Set (L7_UNIT_CURRENT, nasIpMode,objradiusAccClientAddressValue);

  while((wait_time && (usmDbRadiusAttribute4Get(L7_UNIT_CURRENT, &nasIpMode,&objradiusAccClientAddressValue)!=L7_SUCCESS)))
  {
    osapiSleepUSec(1);
    wait_time--;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAccClientInvalidServerAddresses
*
* @purpose Get 'radiusAccClientInvalidServerAddresses'
*
* @description [radiusAccClientInvalidServerAddresses]: The number of RADIUS
*              Accounting-Response packets received from unknown addresses.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAccClientInvalidServerAddresses (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAccClientInvalidServerAddressesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusAcctServerStatInvalidAddressesGet (L7_UNIT_CURRENT,
                                                  &objradiusAccClientInvalidServerAddressesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objradiusAccClientInvalidServerAddressesValue,
                     sizeof (objradiusAccClientInvalidServerAddressesValue));

  /* return the object value: radiusAccClientInvalidServerAddresses */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objradiusAccClientInvalidServerAddressesValue,
                    sizeof (objradiusAccClientInvalidServerAddressesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthServerIndex
*
* @purpose Get 'radiusAuthServerIndex'
*
* @description [radiusAuthServerIndex]: A number uniquely identifying each
*              RADIUS Authentication server with which this client communicates.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthServerIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthServerIndexValue;
  xLibU32_t nextObjradiusAuthServerIndexValue;
  
  xLibStr256_t addrValueStr;
  L7_IP_ADDRESS_TYPE_t addrType;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & objradiusAuthServerIndexValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjradiusAuthServerIndexValue = 1;
    owa.l7rc = usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                     nextObjradiusAuthServerIndexValue,
                                     addrValueStr, &addrType);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objradiusAuthServerIndexValue, owa.len);
    nextObjradiusAuthServerIndexValue = objradiusAuthServerIndexValue +1;
    owa.l7rc =
      usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                             nextObjradiusAuthServerIndexValue,
                                             addrValueStr, &addrType);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjradiusAuthServerIndexValue, owa.len);

  /* return the object value: radiusAuthServerIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjradiusAuthServerIndexValue,
                    sizeof (objradiusAuthServerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthServerAddress
*
* @purpose Get 'radiusAuthServerAddress'
*
* @description [radiusAuthServerAddress]: The IP address of the RADIUS authentication
*              server referred to in this table entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthServerAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthServerAddressValue;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPByIndexGet (L7_UNIT_CURRENT,
                                   keyradiusAuthServerIndexValue,
                                   &objradiusAuthServerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthServerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthServerAddressValue,
                           sizeof (objradiusAuthServerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientServerPortNumber
*
* @purpose Get 'radiusAuthClientServerPortNumber'
*
* @description [radiusAuthClientServerPortNumber]: The UDP port the client
*              is using to send requests to this server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientServerPortNumber (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientServerPortNumberValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusHostNameServerPortNumGet (L7_UNIT_CURRENT,
                                         addrValueStr, addrType,
                                         &objradiusAuthClientServerPortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientServerPortNumber */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAuthClientServerPortNumberValue,
                    sizeof (objradiusAuthClientServerPortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientRoundTripTime
*
* @purpose Get 'radiusAuthClientRoundTripTime'
*
* @description [radiusAuthClientRoundTripTime]: The time interval (in hundredths
*              of a second) between the most recent Access-Reply/Access-Challenge
*              and the Access-Request that matched it from
*              this RADIUS authentication server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientRoundTripTime (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientRoundTripTimeValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  L7_double64 rTTinSec;
  xLibStr256_t roundTripTime;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatRTTGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientRoundTripTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  rTTinSec = objradiusAuthClientRoundTripTimeValue;
  rTTinSec = rTTinSec/100;
  osapiSnprintf(roundTripTime, sizeof (roundTripTime), "%.2f ", rTTinSec);

  /* return the object value in string: radiusAccClientRoundTripTime */
  owa.rc =
    xLibBufDataSet (bufp, roundTripTime,
                    strlen(roundTripTime));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientAccessRequests
*
* @purpose Get 'radiusAuthClientAccessRequests'
*
* @description [radiusAuthClientAccessRequests]: The number of RADIUS Access-Request
*              packets sent to this server. This does not include
*              retransmissions. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientAccessRequests (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientAccessRequestsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatAccessReqGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientAccessRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientAccessRequests */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthClientAccessRequestsValue,
                    sizeof (objradiusAuthClientAccessRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientAccessRetransmissions
*
* @purpose Get 'radiusAuthClientAccessRetransmissions'
*
* @description [radiusAuthClientAccessRetransmissions]: The number of RADIUS
*              Access-Request packets retransmitted to this RADIUS authentication
*              server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientAccessRetransmissions (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientAccessRetransmissionsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatAccessRetransGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientAccessRetransmissionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientAccessRetransmissions */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objradiusAuthClientAccessRetransmissionsValue,
                    sizeof (objradiusAuthClientAccessRetransmissionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientAccessAccepts
*
* @purpose Get 'radiusAuthClientAccessAccepts'
*
* @description [radiusAuthClientAccessAccepts]: The number of RADIUS Access-Accept
*              packets (valid or invalid) received from this server.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientAccessAccepts (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientAccessAcceptsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatAccessAcceptGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientAccessAcceptsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientAccessAccepts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthClientAccessAcceptsValue,
                    sizeof (objradiusAuthClientAccessAcceptsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientAccessRejects
*
* @purpose Get 'radiusAuthClientAccessRejects'
*
* @description [radiusAuthClientAccessRejects]: The number of RADIUS Access-Reject
*              packets (valid or invalid) received from this server.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientAccessRejects (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientAccessRejectsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatAccessRejectGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientAccessRejectsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientAccessRejects */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthClientAccessRejectsValue,
                    sizeof (objradiusAuthClientAccessRejectsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientAccessChallenges
*
* @purpose Get 'radiusAuthClientAccessChallenges'
*
* @description [radiusAuthClientAccessChallenges]: The number of RADIUS Access-Challenge
*              packets (valid or invalid) received from this
*              server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientAccessChallenges (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientAccessChallengesValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatAccessChallengeGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientAccessChallengesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientAccessChallenges */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAuthClientAccessChallengesValue,
                    sizeof (objradiusAuthClientAccessChallengesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientMalformedAccessResponses
*
* @purpose Get 'radiusAuthClientMalformedAccessResponses'
*
* @description [radiusAuthClientMalformedAccessResponses]: The number of malformed
*              RADIUS Access-Response packets received from this
*              server. Malformed packets include packets with an invalid length.
*              Bad authenticators or Signature attributes or unknown
*              types are 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientMalformedAccessResponses (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientMalformedAccessResponsesValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatMalformedAccessResponseGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientMalformedAccessResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientMalformedAccessResponses */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objradiusAuthClientMalformedAccessResponsesValue,
                    sizeof (objradiusAuthClientMalformedAccessResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientBadAuthenticators
*
* @purpose Get 'radiusAuthClientBadAuthenticators'
*
* @description [radiusAuthClientBadAuthenticators]: The number of RADIUS Access-Response
*              packets containing invalid authenticators or
*              Signature attributes received from this server. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientBadAuthenticators (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientBadAuthenticatorsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatBadAuthGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientBadAuthenticatorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientBadAuthenticators */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAuthClientBadAuthenticatorsValue,
                    sizeof (objradiusAuthClientBadAuthenticatorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientPendingRequests
*
* @purpose Get 'radiusAuthClientPendingRequests'
*
* @description [radiusAuthClientPendingRequests]: The number of RADIUS Access-Request
*              packets destined for this server that have not
*              yet timed out or received a response. This variable is incremented
*              when an Access-Request is sent and decremented due
*              to 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientPendingRequests (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientPendingRequestsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatPendingReqGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientPendingRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientPendingRequests */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objradiusAuthClientPendingRequestsValue,
                    sizeof (objradiusAuthClientPendingRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientTimeouts
*
* @purpose Get 'radiusAuthClientTimeouts'
*
* @description [radiusAuthClientTimeouts]: The number of authentication timeouts
*              to this server. After a timeout the client may retry
*              to the same server, send to a different server, or give up.
*              A retry to the same server is counted as a retransmi 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientTimeouts (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientTimeoutsValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatTimeoutsGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientTimeoutsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientTimeouts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthClientTimeoutsValue,
                    sizeof (objradiusAuthClientTimeoutsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientUnknownTypes
*
* @purpose Get 'radiusAuthClientUnknownTypes'
*
* @description [radiusAuthClientUnknownTypes]: The number of RADIUS packets
*              of unknown type which were received from this server on the
*              authentication port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientUnknownTypes (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientUnknownTypesValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatUnknownTypeGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientUnknownTypesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: radiusAuthClientUnknownTypes */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthClientUnknownTypesValue,
                    sizeof (objradiusAuthClientUnknownTypesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientPacketsDropped
*
* @purpose Get 'radiusAuthClientPacketsDropped'
*
* @description [radiusAuthClientPacketsDropped]: The number of RADIUS packets
*              of which were received from this server on the authentication
*              port and dropped for some other reason. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientPacketsDropped (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyradiusAuthServerIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientPacketsDroppedValue;
  L7_IP_ADDRESS_TYPE_t addrType;
  xLibStr256_t addrValueStr;
  memset(addrValueStr,0x00,sizeof(addrValueStr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: radiusAuthServerIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_securityRadius_radiusAuthServerIndex,
                          (xLibU8_t *) & keyradiusAuthServerIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyradiusAuthServerIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusServerIPHostNameByIndexGet (L7_UNIT_CURRENT,
                                              keyradiusAuthServerIndexValue,
                                              addrValueStr, &addrType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =
    usmDbRadiusServerStatPktsDroppedGet (L7_UNIT_CURRENT,
                                         addrValueStr,
                                         &objradiusAuthClientPacketsDroppedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: radiusAuthClientPacketsDropped */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objradiusAuthClientPacketsDroppedValue,
                    sizeof (objradiusAuthClientPacketsDroppedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientIdentifier
*
* @purpose Get 'radiusAuthClientIdentifier'
*
* @description [radiusAuthClientIdentifier]: The NAS-Identifier of the RADIUS
*              authentication client. This is not necessarily the same
*              as sysName in MIB II. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientIdentifier (void *wap,
                                                             void *bufp)
{
   fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objradiusAuthClientIdentifierValue;
  xLibU32_t nasIdSize;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objradiusAuthClientIdentifierValue,0x00,sizeof(objradiusAuthClientIdentifierValue));

  /* get the value from application */
  usmDbRadiusNASIdentifierGet (L7_UNIT_CURRENT,
                                 objradiusAuthClientIdentifierValue,&nasIdSize);
 
  owa.l7rc = L7_SUCCESS;

  FPOBJ_TRACE_VALUE (bufp, objradiusAuthClientIdentifierValue,
                     strlen (objradiusAuthClientIdentifierValue));

  /* return the object value: radiusAuthClientIdentifier */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objradiusAuthClientIdentifierValue,
                    strlen (objradiusAuthClientIdentifierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusAuthClientInvalidServerAddresses
*
* @purpose Get 'radiusAuthClientInvalidServerAddresses'
*
* @description [radiusAuthClientInvalidServerAddresses]: The number of RADIUS
*              Access-Response packets received from unknown addresses.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusAuthClientInvalidServerAddresses (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusAuthClientInvalidServerAddressesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRadiusStatInvalidAddressesGet (L7_UNIT_CURRENT,
                                        &objradiusAuthClientInvalidServerAddressesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objradiusAuthClientInvalidServerAddressesValue,
                     sizeof (objradiusAuthClientInvalidServerAddressesValue));

  /* return the object value: radiusAuthClientInvalidServerAddresses */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objradiusAuthClientInvalidServerAddressesValue,
                    sizeof (objradiusAuthClientInvalidServerAddressesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securityRadius_radiusServerConfiguredServersCount
*
* @purpose Get 'radiusServerConfiguredServersCount'
*
* @description [radiusServerConfiguredServersCount] The number of configured RADIUS servers.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusServerConfiguredServersCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objradiusServerConfiguredServersCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRadiusServerCountGet (L7_UNIT_CURRENT, &objradiusServerConfiguredServersCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objradiusServerConfiguredServersCountValue,
                     sizeof (objradiusServerConfiguredServersCountValue));

  /* return the object value: radiusServerConfiguredServersCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusServerConfiguredServersCountValue,
                           sizeof (objradiusServerConfiguredServersCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securityRadius_radiusServerCurrentActiveAuthServer
*
* @purpose Get 'radiusServerCurrentActiveAuthServer'
*
* @description [radiusServerCurrentActiveAuthServer] The current active auth server.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusServerCurrentActiveAuthServer (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objradiusServerCurrentActiveAuthServerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRadiusCurrentAuthHostNameServerGet (L7_UNIT_CURRENT, objradiusServerCurrentActiveAuthServerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objradiusServerCurrentActiveAuthServerValue,
                     strlen (objradiusServerCurrentActiveAuthServerValue));

  /* return the object value: radiusServerCurrentActiveAuthServer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objradiusServerCurrentActiveAuthServerValue,
                           strlen (objradiusServerCurrentActiveAuthServerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityRadius_radiusServerConfiguredAuthServersCount
*
* @purpose Get 'radiusServerConfiguredAuthServersCount'
 *@description  [radiusServerConfiguredAuthServersCount] The number of
* configured Auth RADIUS servers.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusServerConfiguredAuthServersCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objradiusServerConfiguredAuthServersCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRadiusConfiguredServersCountGet(L7_UNIT_CURRENT, RADIUS_SERVER_TYPE_AUTH, &objradiusServerConfiguredAuthServersCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objradiusServerConfiguredAuthServersCountValue,
                     sizeof (objradiusServerConfiguredAuthServersCountValue));

  /* return the object value: radiusServerConfiguredAuthServersCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusServerConfiguredAuthServersCountValue,
                           sizeof (objradiusServerConfiguredAuthServersCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityRadius_radiusServerConfiguredAcctServersCount
*
* @purpose Get 'radiusServerConfiguredAcctServersCount'
 *@description  [radiusServerConfiguredAcctServersCount] The number of
* configured Acct RADIUS servers.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusServerConfiguredAcctServersCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objradiusServerConfiguredAcctServersCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
 owa.l7rc = usmDbRadiusConfiguredServersCountGet(L7_UNIT_CURRENT, RADIUS_SERVER_TYPE_ACCT, &objradiusServerConfiguredAcctServersCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objradiusServerConfiguredAcctServersCountValue,
                     sizeof (objradiusServerConfiguredAcctServersCountValue));

  /* return the object value: radiusServerConfiguredAcctServersCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusServerConfiguredAcctServersCountValue,
                           sizeof (objradiusServerConfiguredAcctServersCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityRadius_radiusServerAuthServerGroupsCount
*
* @purpose Get 'radiusServerAuthServerGroupsCount'
 *@description  [radiusServerAuthServerGroupsCount] The number of configured
* RADIUS server Auth groups.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusServerAuthServerGroupsCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objradiusServerAuthServerGroupsCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRadiusConfiguredNamedServersGroupCountGet(L7_UNIT_CURRENT, RADIUS_SERVER_TYPE_AUTH, &objradiusServerAuthServerGroupsCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objradiusServerAuthServerGroupsCountValue,
                     sizeof (objradiusServerAuthServerGroupsCountValue));

  /* return the object value: radiusServerAuthServerGroupsCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusServerAuthServerGroupsCountValue,
                           sizeof (objradiusServerAuthServerGroupsCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_securityRadius_radiusServerAcctServerGroupsCount
*
* @purpose Get 'radiusServerAcctServerGroupsCount'
 *@description  [radiusServerAcctServerGroupsCount] The number of configured
* RADIUS server Acct groups.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityRadius_radiusServerAcctServerGroupsCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objradiusServerAcctServerGroupsCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
 owa.l7rc = usmDbRadiusConfiguredNamedServersGroupCountGet(L7_UNIT_CURRENT, RADIUS_SERVER_TYPE_ACCT, &objradiusServerAcctServerGroupsCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objradiusServerAcctServerGroupsCountValue,
                     sizeof (objradiusServerAcctServerGroupsCountValue));

  /* return the object value: radiusServerAcctServerGroupsCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objradiusServerAcctServerGroupsCountValue,
                           sizeof (objradiusServerAcctServerGroupsCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

