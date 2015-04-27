/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basesnmpCommunityConfig.c
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
#include "_xe_basesnmpCommunityConfig_obj.h"
#include "usmdb_snmp_api.h"

/*******************************************************************************
* @function fpObjGet_basesnmpCommunityConfig_Index
*
* @purpose Get 'Index'
*
* @description Snmp Community Index 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_Index (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIndexValue;
  xLibU32_t nextObjIndexValue;
  xLibU32_t status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  owa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & objIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjIndexValue = 0;
    do
    {
      usmDbSnmpCommunityStatusGet(L7_UNIT_CURRENT, nextObjIndexValue, &status);
      if (status ==L7_SNMP_COMMUNITY_STATUS_DELETE)
      {
        nextObjIndexValue++;
        owa.l7rc = L7_FAILURE;
      }
      else
      {
        owa.l7rc = L7_SUCCESS;
        break;
      }
    }while(nextObjIndexValue < L7_MAX_SNMP_COMM);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIndexValue, owa.len);
    owa.l7rc = usmDbSnmpCommunityEntryNext (L7_UNIT_CURRENT, &objIndexValue);
    if(owa.l7rc  == L7_SUCCESS)
    {
      nextObjIndexValue = objIndexValue;
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIndexValue, owa.len);

  /* return the object value: Index */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIndexValue,
                           sizeof (objIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesnmpCommunityConfig_Name
*
* @purpose Get 'Name'
*
* @description Snmp Community Name 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_Name (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpCommunityNameGet (L7_UNIT_CURRENT, keyIndexValue,
                                        objNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Name */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objNameValue,
                           strlen (objNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesnmpCommunityConfig_Name
*
* @purpose Set 'Name'
*
* @description Snmp Community Name 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesnmpCommunityConfig_Name (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Name */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objNameValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  if(usmDbSnmpCommunityStatusGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &objStatusValue) == L7_SUCCESS)
  {
      owa.l7rc = usmDbSnmpCommunityNameSet (L7_UNIT_CURRENT, keyIndexValue,
                                        objNameValue);
      if((objStatusValue == L7_SNMP_COMMUNITY_STATUS_CONFIG) && (owa.l7rc != L7_SUCCESS))
      {
         usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, L7_SNMP_COMMUNITY_STATUS_DELETE);
      }

  }
  else
  {
    owa.l7rc = L7_FAILURE;
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
* @function fpObjGet_basesnmpCommunityConfig_IPAddr
*
* @purpose Get 'IPAddr'
*
* @description Snmp Community Ip Address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_IPAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpCommunityIpAddrGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &objIPAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IPAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPAddrValue,
                           sizeof (objIPAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesnmpCommunityConfig_IPAddr
*
* @purpose Set 'IPAddr'
*
* @description Snmp Community Ip Address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesnmpCommunityConfig_IPAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPAddrValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  xLibU32_t currStatus; 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IPAddr */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIPAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIPAddrValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
    
  owa.l7rc = L7_FAILURE;
  if(usmDbSnmpCommunityStatusGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
    if(usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, L7_SNMP_COMMUNITY_STATUS_CONFIG) == L7_SUCCESS)
       owa.l7rc = usmDbSnmpCommunityIpAddrSet (L7_UNIT_CURRENT, keyIndexValue,
                                          objIPAddrValue);
    usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, currStatus);
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
* @function fpObjGet_basesnmpCommunityConfig_IpMask
*
* @purpose Get 'IpMask'
*
* @description Snmo community IP Mask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_IpMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpCommunityIpMaskGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &objIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpMaskValue,
                           sizeof (objIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesnmpCommunityConfig_IpMask
*
* @purpose Set 'IpMask'
*
* @description Snmo community IP Mask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesnmpCommunityConfig_IpMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpMaskValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  xLibU32_t currStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpMask */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpMaskValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(usmDbSnmpCommunityStatusGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
    if(usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, L7_SNMP_COMMUNITY_STATUS_CONFIG) == L7_SUCCESS)
       owa.l7rc = usmDbSnmpCommunityIpMaskSet (L7_UNIT_CURRENT, keyIndexValue,
                                          objIpMaskValue);
    usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, currStatus);
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
* @function fpObjGet_basesnmpCommunityConfig_AccessMode
*
* @purpose Get 'AccessMode'
*
* @description Snmp Community Access Mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_AccessMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAccessModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpCommunityAccessLevelGet (L7_UNIT_CURRENT, keyIndexValue,
                                               &objAccessModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AccessMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAccessModeValue,
                           sizeof (objAccessModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesnmpCommunityConfig_AccessMode
*
* @purpose Set 'AccessMode'
*
* @description Snmp Community Access Mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesnmpCommunityConfig_AccessMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAccessModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  xLibU32_t currStatus;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AccessMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAccessModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAccessModeValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if(usmDbSnmpCommunityStatusGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &currStatus) == L7_SUCCESS)
  {
    if(usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, L7_SNMP_COMMUNITY_STATUS_CONFIG) == L7_SUCCESS)
       owa.l7rc = usmDbSnmpCommunityAccessLevelSet (L7_UNIT_CURRENT, keyIndexValue,
                                               objAccessModeValue);
    usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, keyIndexValue, currStatus);
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
* @function fpObjGet_basesnmpCommunityConfig_Owner
*
* @purpose Get 'Owner'
*
* @description Snmp Community Owner 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_Owner (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objOwnerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpCommunityOwnerGet (L7_UNIT_CURRENT, keyIndexValue,
                                         objOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Owner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objOwnerValue,
                           strlen (objOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesnmpCommunityConfig_Owner
*
* @purpose Set 'Owner'
*
* @description Snmp Community Owner 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesnmpCommunityConfig_Owner (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objOwnerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Owner */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objOwnerValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbSnmpCommunityOwnerSet (L7_UNIT_CURRENT, keyIndexValue,
                                         objOwnerValue);
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
* @function fpObjGet_basesnmpCommunityConfig_Status
*
* @purpose Get 'Status'
*
* @description snmp Community Status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesnmpCommunityConfig_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpCommunityStatusGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesnmpCommunityConfig_Status
*
* @purpose Set 'Status'
*
* @description snmp Community Status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesnmpCommunityConfig_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbSnmpCommunityStatusSet (L7_UNIT_CURRENT, keyIndexValue,
                                          objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



xLibRC_t fpObjGet_basesnmpCommunityConfig_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  xLibU32_t currStatus;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  if((owa.l7rc = usmDbSnmpCommunityStatusGet (L7_UNIT_CURRENT, keyIndexValue,
                                          &currStatus)) == L7_SUCCESS)
  {
     if( currStatus != L7_SNMP_COMMUNITY_STATUS_DELETE )
    {
       objRowStatusValue = L7_ROW_STATUS_ACTIVE;	     
    }
    else
    {  
       objRowStatusValue = L7_ROW_STATUS_INVALID;
    }      
          	    
  }	  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
                           sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjSet_basesnmpCommunityConfig_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  xLibU32_t status = 0, i=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_basesnmpCommunityConfig_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);

  if ((kwa.rc != XLIBRC_SUCCESS) && (objRowStatusValue !=L7_ROW_STATUS_CREATE_AND_GO))
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);
  if( objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)  
  {
    i=0;
    while (i < L7_MAX_SNMP_COMM && status!=L7_SNMP_COMMUNITY_STATUS_DELETE)
    {
      owa.l7rc = usmDbSnmpCommunityStatusGet(L7_UNIT_CURRENT, i++, &status);
    }

    if(status == L7_SNMP_COMMUNITY_STATUS_DELETE)
    { 
      i--;
      /* status must be set to config before other fields can be set */
      if(usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, i, L7_SNMP_COMMUNITY_STATUS_CONFIG) == L7_SUCCESS)
      {
          if(xLibFilterSet(wap,XOBJ_basesnmpCommunityConfig_Index,0,
                              (xLibU8_t *) &i,
                              sizeof(i)) == XLIBRC_SUCCESS)
              owa.l7rc = L7_SUCCESS;
      } 
      else
        owa.l7rc = L7_FAILURE;
    }
    else
      owa.l7rc = L7_FAILURE;
     
  }
  else
  {
     i = keyIndexValue;
     usmDbSnmpCommunityIpAddrSet(L7_UNIT_CURRENT, i, 0);
     usmDbSnmpCommunityIpMaskSet(L7_UNIT_CURRENT, i, 0);
     owa.l7rc = usmDbSnmpCommunityStatusSet(L7_UNIT_CURRENT, i, L7_SNMP_COMMUNITY_STATUS_DELETE);
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
