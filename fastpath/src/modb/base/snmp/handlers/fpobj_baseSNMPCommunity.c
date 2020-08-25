
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baseSNMPCommunity.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  07 March 2009, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseSNMPCommunity_obj.h"
#include "snmp_exports.h"

#include "usmdb_snmp_confapi_api.h"
#include "usmdb_util_api.h"

xLibRC_t fpObjUtil_baseSNMPCommunity_IndexFromCommunityNameGet(L7_char8 *strCommunityName,
                                     L7_char8 *charIndex)
{
  L7_char8 snmpCommunityIndex[L7_SNMP_NAME_SIZE_VALUE+1];
  L7_char8 communityName[L7_SNMP_NAME_SIZE_VALUE+1];
  L7_RC_t rc, rc1;

  memset(snmpCommunityIndex, 0x00, sizeof(snmpCommunityIndex));
  rc = usmDbSnmpConfapiSnmpCommunityTableNextGet(snmpCommunityIndex);

  while (rc == L7_SUCCESS)
  {
    memset(communityName, 0x00, sizeof(communityName));
    rc1 = usmDbSnmpConfapiSnmpCommunityNameGet(snmpCommunityIndex,
                                               communityName);
    if (rc1 == L7_SUCCESS)
    {
      if (strcmp(communityName, strCommunityName) == L7_NULL)
      {
        strcpy(charIndex, snmpCommunityIndex);
        return XLIBRC_SUCCESS;
      }
    }
    else
      return XLIBRC_FAILURE;
		
    rc = usmDbSnmpConfapiSnmpCommunityTableNextGet(snmpCommunityIndex);
  }
  return XLIBRC_FAILURE;
}





/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_CommunityIndex
*
* @purpose Get 'CommunityIndex'
 *@description  [CommunityIndex] SNMP community index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_CommunityIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objCommunityIndexValue;
  xLibStr256_t nextObjCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (objCommunityIndexValue);
  FPOBJ_CLR_STR256 (nextObjCommunityIndexValue);  

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (objCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) objCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	FPOBJ_CLR_STR256 (objCommunityIndexValue);
    owa.l7rc = usmDbSnmpConfapiSnmpCommunityTableNextGet ( objCommunityIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objCommunityIndexValue, owa.len);
    owa.l7rc = usmDbSnmpConfapiSnmpCommunityTableNextGet ( objCommunityIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_CLR_STR256 (nextObjCommunityIndexValue);
  FPOBJ_CPY_STR256 (nextObjCommunityIndexValue, objCommunityIndexValue);

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjCommunityIndexValue, owa.len);

  /* return the object value: CommunityIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjCommunityIndexValue,
                           strlen (nextObjCommunityIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_CommunityName
*
* @purpose Get 'CommunityName'
 *@description  [CommunityName] SNMP  Community Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_CommunityName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objCommunityNameValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (objCommunityNameValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpConfapiSnmpCommunityNameGet ( keyCommunityIndexValue, objCommunityNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objCommunityNameValue, strlen (objCommunityNameValue));

  /* return the object value: CommunityName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCommunityNameValue,
                           strlen (objCommunityNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPCommunity_CommunityName
*
* @purpose Set 'CommunityName'
 *@description  [CommunityName] SNMP  Community Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPCommunity_CommunityName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t objCommunityNameValue;
  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objCommunityNameValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve object: CommunityName */
  owa.len = sizeof (objCommunityNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objCommunityNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCommunityNameValue, owa.len);

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);


  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_MgmtStation
*
* @purpose Get 'MgmtStation'
 *@description  [MgmtStation] SNMP  Community management station   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_MgmtStation (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMgmtStationValue;

  xLibStr256_t keyCommunityIndexValue;
  xLibStr256_t snmpTargetAddrName;
  xLibU32_t indx = 0;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objMgmtStationValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 
  FPOBJ_CLR_STR256 (snmpTargetAddrName);
 
  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);


    memset(objMgmtStationValue, 0x00, sizeof(objMgmtStationValue));
	memset(snmpTargetAddrName, 0x00, sizeof(snmpTargetAddrName));
	sprintf(snmpTargetAddrName, "community_%s", keyCommunityIndexValue);
	owa.l7rc = usmDbSnmpConfapiSnmpTargetAddrTAddressGet (snmpTargetAddrName, objMgmtStationValue);
	  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
		

	indx=0;
	while((objMgmtStationValue[indx]!=':')&&(objMgmtStationValue[indx]!='\0'))
	   indx++;
	
	objMgmtStationValue[indx]='\0';

  FPOBJ_TRACE_VALUE (bufp, objMgmtStationValue, sizeof (objMgmtStationValue));

  /* return the object value: MgmtStation */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objMgmtStationValue, sizeof (objMgmtStationValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPCommunity_MgmtStation
*
* @purpose Set 'MgmtStation'
 *@description  [MgmtStation] SNMP  Community management station   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPCommunity_MgmtStation (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMgmtStationValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objMgmtStationValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve object: MgmtStation */
  owa.len = sizeof (objMgmtStationValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)  objMgmtStationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMgmtStationValue, owa.len);

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  /* set the value in application */

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_AccessMode
*
* @purpose Get 'AccessMode'
 *@description  [AccessMode] SNMP  Access mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_AccessMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAccessModeValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);


  xLibStr256_t          strSecurityName;  
  xLibStr256_t         securityName;  
  xLibStr256_t         strGroupName;  
  xLibStr256_t         contextPrefix ;
  xLibStr256_t         groupName;
  xLibStr256_t         viewName;
  xLibStr256_t         strOidTree;
  L7_uint32         securityModel =1; 
  L7_uint32         securityLevel=0;  
  xLibStr256_t         strSysAttr ;
  xLibStr256_t         strSysAttr1;
  L7_uint32         snmpSecurityModel;
  L7_RC_t           rc, rc1;
  L7_uint32         index;
  objAccessModeValue = L7_XUI_SNMP_COMMUNITY_READ_ONLY;

  memset(strSecurityName,0x00,sizeof(strSecurityName));
  if(usmDbSnmpConfapiSnmpCommunitySecurityNameGet(keyCommunityIndexValue,strSecurityName)==L7_SUCCESS)
  {
    memset(securityName,0x00,sizeof(securityName));
    if((rc = usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(&securityModel,securityName))==L7_SUCCESS)
    {
      while(rc==L7_SUCCESS)
      {
        if(strcmp(securityName,strSecurityName)==L7_NULL)
        {
          memset(strGroupName,0x00,sizeof(strGroupName));
          if(usmDbSnmpConfapiVacmGroupNameGet(securityModel,strSecurityName,strGroupName)==L7_SUCCESS)
          {
            break;
          }
        }
        rc = usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(&securityModel,securityName);
      }
    }

    memset(groupName,0x00,sizeof(groupName));
    memset(contextPrefix,0x00,sizeof(contextPrefix));
    memset(strSysAttr,0x00,sizeof(strSysAttr));
    memset(strSysAttr1,0x00,sizeof(strSysAttr1));

    rc1 = usmDbSnmpConfapiVacmAccessTableNextGet(groupName,contextPrefix,&snmpSecurityModel,&securityLevel);
    while(rc1==L7_SUCCESS)
    {
      if((strcmp(groupName,strGroupName)==L7_NULL)&&(snmpSecurityModel == securityModel))
      {
        memset(viewName, 0x00, sizeof(viewName));
        memset(strOidTree, 0x00, sizeof(strOidTree));
        rc1 = usmDbSnmpConfapiVacmAccessWriteViewNameGet(groupName,contextPrefix,
                                                  snmpSecurityModel,securityLevel,strSysAttr);
        if(strcmp(groupName, "SuperGroup") == L7_NULL)
        {
          objAccessModeValue = L7_XUI_SNMP_COMMUNITY_SUPER_USER;
			break;
        }
        else if(strcmp(strSysAttr,"") != 0)
        {
          index = 0;
          while(usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet(viewName, strOidTree) == L7_SUCCESS)
          {
            index++;
            if(strcmp(strSysAttr, viewName) == L7_NULL)
              break;
          }

          objAccessModeValue = L7_XUI_SNMP_COMMUNITY_READ_WRITE;
        }
        else if(usmDbSnmpConfapiVacmAccessReadViewNameGet(groupName,contextPrefix,
                             snmpSecurityModel,securityLevel,strSysAttr1) == L7_SUCCESS)
        {
          if(strcmp(strSysAttr1,"") != 0)
          {
            index = 0;
            while(usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet(viewName, strOidTree) == L7_SUCCESS)
            {
              index++;
              if(strcmp(strSysAttr1, viewName) == L7_NULL)
                break;
            }

            /* populate access mode in drop down list */
          objAccessModeValue = L7_XUI_SNMP_COMMUNITY_READ_ONLY;
			break;
          }
        }
      }
      rc1 = usmDbSnmpConfapiVacmAccessTableNextGet(groupName,contextPrefix,
                                           &snmpSecurityModel,&securityLevel);
    }
  }


  /* get the value from application */
  owa.rc = XLIBRC_SUCCESS;  

  FPOBJ_TRACE_VALUE (bufp, &objAccessModeValue, sizeof (objAccessModeValue));

  /* return the object value: AccessMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAccessModeValue, sizeof (objAccessModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPCommunity_AccessMode
*
* @purpose Set 'AccessMode'
 *@description  [AccessMode] SNMP  Access mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPCommunity_AccessMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAccessModeValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve object: AccessMode */
  owa.len = sizeof (objAccessModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAccessModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAccessModeValue, owa.len);

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);


  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_ViewName
*
* @purpose Get 'ViewName'
 *@description  [ViewName] SNMP  View Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_ViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objViewNameValue;

  xLibStr256_t keyCommunityIndexValue;

  xLibStr256_t strSecurityName;
  xLibStr256_t securityName;
  xLibStr256_t stringGroupName;
  xLibStr256_t groupname;
  xLibStr256_t contextPrefix;
  L7_uint32 securityModel, version, securityLevelValue, countIndexValue = 0;


  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objViewNameValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 
  FPOBJ_CLR_STR256 (strSecurityName); 
  FPOBJ_CLR_STR256 (securityName); 
  FPOBJ_CLR_STR256 (groupname); 
  FPOBJ_CLR_STR256 (contextPrefix); 

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  owa.l7rc = L7_FAILURE;

  memset(strSecurityName, 0x00, sizeof(strSecurityName));
  if (usmDbSnmpConfapiSnmpCommunitySecurityNameGet(keyCommunityIndexValue,
                                                   strSecurityName) == L7_SUCCESS)
  {
    memset(securityName, 0x00, sizeof(securityName));
    securityModel = 1;
    while (usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(&securityModel,
                                                           securityName) == L7_SUCCESS)
    {
      if (strcmp(securityName, strSecurityName) ==L7_NULL)
      {
        memset(stringGroupName, 0x00, sizeof(stringGroupName));
        if (usmDbSnmpConfapiVacmGroupNameGet(securityModel, strSecurityName,
                                             stringGroupName) == L7_SUCCESS)
        {
          owa.l7rc = L7_SUCCESS;
			break;
        }
      }
    }
		
    memset(groupname, 0x00, sizeof(groupname));
    memset(contextPrefix, 0x00, sizeof(contextPrefix));
    version =1;
    while (usmDbSnmpConfapiVacmAccessTableNextGet(groupname, contextPrefix,
                                                  &version, &securityLevelValue) == L7_SUCCESS)
    {
      countIndexValue++;
      if (strcmp(groupname, stringGroupName) == L7_NULL)
        break;
    }
    memset(objViewNameValue, 0x00, sizeof(objViewNameValue));
    if ((usmDbSnmpConfapiVacmAccessWriteViewNameGet(groupname, contextPrefix,
                                                    version, securityLevelValue, objViewNameValue) == L7_SUCCESS)&&
        (strlen(objViewNameValue) != L7_NULL))
    {
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      memset(objViewNameValue, 0x00, sizeof(objViewNameValue));
      if ((usmDbSnmpConfapiVacmAccessReadViewNameGet(groupname, contextPrefix,
                                                     version, securityLevelValue, objViewNameValue) == L7_SUCCESS)&&
          (strlen(objViewNameValue) != L7_NULL))
      {
        owa.l7rc = L7_SUCCESS;
      }
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objViewNameValue, strlen (objViewNameValue));

  /* return the object value: ViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objViewNameValue, strlen (objViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseSNMPCommunity_ViewName
*
* @purpose List 'ViewName'
 *@description  [ViewName] SNMP  View Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSNMPCommunity_ViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objReadViewNameValue;
  xLibStr256_t nextObjReadViewNameValue;
  xLibStr256_t objOIDSubtree;
  xLibStr256_t nextObjOIDSubTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objReadViewNameValue);
  FPOBJ_CLR_STR256 (nextObjReadViewNameValue);
  FPOBJ_CLR_STR256 (objOIDSubtree);
  FPOBJ_CLR_STR256 (nextObjOIDSubTreeValue);

  owa.len = sizeof (objReadViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_ViewName,
                          (xLibU8_t *) objReadViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet ( nextObjReadViewNameValue, nextObjOIDSubTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objReadViewNameValue, owa.len);
    FPOBJ_CPY_STR256(nextObjReadViewNameValue,objReadViewNameValue);
    do
    {
      FPOBJ_CPY_STR256 (nextObjOIDSubTreeValue,objOIDSubtree);
      owa.l7rc =usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (nextObjReadViewNameValue, nextObjOIDSubTreeValue);
      FPOBJ_CPY_STR256 (objOIDSubtree,nextObjOIDSubTreeValue);
    }while (!(FPOBJ_CMP_STR256 (objReadViewNameValue, nextObjReadViewNameValue)) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjReadViewNameValue, owa.len);

  /* return the object value: ReadViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjReadViewNameValue,
                           strlen (nextObjReadViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
	
}

/*******************************************************************************
* @function fpObjSet_baseSNMPCommunity_ViewName
*
* @purpose Set 'ViewName'
 *@description  [ViewName] SNMP  View Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPCommunity_ViewName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objViewNameValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objViewNameValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve object: ViewName */
  owa.len = sizeof (objViewNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objViewNameValue, owa.len);

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_GroupName
*
* @purpose Get 'GroupName'
 *@description  [GroupName] SNMP  Group Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_GroupName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupNameValue;

  xLibStr256_t keyCommunityIndexValue;
  xLibStr256_t strSecurityName;
  L7_uint32 securityModel;
  xLibStr256_t securityName;
  L7_RC_t           rc;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objGroupNameValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 
  FPOBJ_CLR_STR256 (strSecurityName); 
  FPOBJ_CLR_STR256 (securityName);
 
  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  memset(strSecurityName,0x00,sizeof(strSecurityName));
  if(usmDbSnmpConfapiSnmpCommunitySecurityNameGet(keyCommunityIndexValue,strSecurityName)==L7_SUCCESS)
  {
    memset(securityName,0x00,sizeof(securityName));
    if((rc = usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(&securityModel,securityName))==L7_SUCCESS)
    {
      while(rc==L7_SUCCESS)
      {
        if(strcmp(securityName,strSecurityName)==L7_NULL)
        {
          memset(objGroupNameValue,0x00,sizeof(objGroupNameValue));
          if(usmDbSnmpConfapiVacmGroupNameGet(securityModel,strSecurityName,objGroupNameValue)==L7_SUCCESS)
          {
            owa.l7rc = L7_SUCCESS;
          }
        }
        rc = usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(&securityModel,securityName);
      }
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objGroupNameValue, strlen (objGroupNameValue));

  /* return the object value: GroupName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupNameValue, strlen (objGroupNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseSNMPCommunity_GroupName
*
* @purpose List 'GroupName'
 *@description  [GroupName] SNMP  Group Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSNMPCommunity_GroupName (void *wap, void *bufp)
{

 fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupNameValue;
  xLibStr256_t nextObjGroupNameValue;

  xLibStr256_t objContextPrefixValue;
  xLibStr256_t nextObjContextPrefixValue;
  xLibU32_t objVersionValue;
  xLibU32_t nextObjVersionValue;
  xLibU32_t objSecurityLevelValue;
  xLibU32_t nextObjSecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (objGroupNameValue); 
  FPOBJ_CLR_STR256 (nextObjGroupNameValue); 
  FPOBJ_CLR_STR256 (objContextPrefixValue); 
  FPOBJ_CLR_STR256 (nextObjContextPrefixValue);

  /* retrieve key: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_GroupName,
                          (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_STR256 (objGroupNameValue);
    FPOBJ_CLR_STR256 (objContextPrefixValue);
    FPOBJ_CLR_U32 (objVersionValue);
    FPOBJ_CLR_U32 (objSecurityLevelValue);

    FPOBJ_CLR_STR256 (nextObjGroupNameValue);
    FPOBJ_CLR_STR256 (nextObjContextPrefixValue);
    FPOBJ_CLR_U32 (nextObjVersionValue);
    FPOBJ_CLR_U32 (nextObjSecurityLevelValue);

    owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (
                                     nextObjGroupNameValue,
                                     nextObjContextPrefixValue, &nextObjVersionValue,
                                     &nextObjSecurityLevelValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objGroupNameValue, owa.len);
    FPOBJ_CLR_STR256 (objContextPrefixValue);
    FPOBJ_CLR_U32 (objVersionValue);
    FPOBJ_CLR_U32 (objSecurityLevelValue);
    FPOBJ_CPY_STR256 (nextObjGroupNameValue, objGroupNameValue);
    FPOBJ_CPY_STR256 (nextObjContextPrefixValue, objContextPrefixValue);
    FPOBJ_CPY_U32 (nextObjVersionValue, objVersionValue);
    FPOBJ_CPY_U32 (nextObjSecurityLevelValue,objSecurityLevelValue);
    do
    {
      owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (nextObjGroupNameValue,
                                      nextObjContextPrefixValue, &nextObjVersionValue,
                                      &nextObjSecurityLevelValue);
    }
    while (!(FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjGroupNameValue, owa.len);

  /* return the object value: GroupName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjGroupNameValue,
                           strlen (nextObjGroupNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSNMPCommunity_GroupName
*
* @purpose Set 'GroupName'
 *@description  [GroupName] SNMP  Group Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPCommunity_GroupName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupNameValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (objGroupNameValue); 
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 

  /* retrieve object: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupNameValue, owa.len);

  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPCommunity_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Communities.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPCommunity_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyCommunityIndexValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 
  /* retrieve key: CommunityIndex */
  owa.len = sizeof (keyCommunityIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityIndex,
                          (xLibU8_t *) keyCommunityIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyCommunityIndexValue, owa.len);

  objRowStatusValue = L7_ROW_STATUS_ACTIVE;
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPCommunity_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Communities.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPCommunity_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyCommunityIndexValue;

  fpObjWa_t    kwaObj = FPOBJ_INIT_WA2 ();
  xLibU32_t     objAccessModeValue;
  xLibStr256_t objViewNameValue;
  xLibStr256_t objGroupNameValue;

  xLibStr256_t tempObjMgmtStationValue;
  xLibBool_t    isMgmtStationAll = XLIB_TRUE;
  xLibBool_t    isGroupNameSet = XLIB_FALSE;
  xLibBool_t    isViewNameSet = XLIB_FALSE;
  L7_uint32     securityLevelValue=snmpSecurityLevel_noAuthNoPriv;
  L7_uint32         securityLevel=0;  
  xLibStr256_t         contextPrefix;
  xLibStr256_t         strSysAttr;
  xLibBool_t    viewNameFound = XLIB_TRUE;
  xLibStr256_t          strOidTree;
  L7_RC_t rc,rc1=L7_FAILURE,rc2=L7_FAILURE;

  xLibU32_t     objMgmtStationIPADDRValue;
   /* This magic number(128) is derived from L7_CLI_MAX_STRING_LENGTH
   * this need to be changed. This is defined basically to solve XLIB
   * type casting issues
   */
  xLibU8_t  objMgmtStationValue[128];
  xLibU8_t  objCommunityNameValue[128];
  xLibU8_t  tempTargetAddrName[128];
  xLibU8_t  tempAddrString[128];
	
  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (keyCommunityIndexValue); 
  FPOBJ_CLR_STR256 (objViewNameValue); 
  FPOBJ_CLR_STR256 (objGroupNameValue); 
  FPOBJ_CLR_STR256 (tempObjMgmtStationValue); 
  FPOBJ_CLR_STR256 (contextPrefix); 
  FPOBJ_CLR_STR256 (strSysAttr); 
  FPOBJ_CLR_STR256 (strOidTree); 

  memset(objMgmtStationValue,0x00,sizeof(objMgmtStationValue));
  memset(objCommunityNameValue,0x00,sizeof(objCommunityNameValue));
  memset(tempTargetAddrName,0x00,sizeof(tempTargetAddrName));
  memset(tempAddrString,0x00,sizeof(tempAddrString));

  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);


  if(objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
   /* Get the community name, mgmtStation Name, access mode, view name, group name */
  /* Community Name */
		kwaObj.len = sizeof (objCommunityNameValue);
		kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityName,
			            (xLibU8_t *)  objCommunityNameValue, &kwaObj.len);
		if (kwaObj.rc != XLIBRC_SUCCESS)
		{
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
	   }
		FPOBJ_TRACE_VALUE (bufp, objCommunityNameValue, kwaObj.len);



		/* Check if community name already exists */
		if(usmDbSnmpConfapiSnmpCommunityTableCheckValid(objCommunityNameValue) == L7_SUCCESS)
		{
		  owa.rc = XLIBRC_SNMP_COMMUNITYNAME_ALREADY_EXISTS;
		  FPOBJ_TRACE_EXIT (bufp, owa);
		  return owa.rc;
		}
		
  /* objMgmtStationValue  */
		kwaObj.len = sizeof (objMgmtStationValue);
		kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_MgmtStation,
			                            (xLibU8_t *)  objMgmtStationValue, &kwaObj.len);
		if (kwaObj.rc != XLIBRC_SUCCESS)
		{
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
		}
		FPOBJ_TRACE_VALUE (bufp, objMgmtStationValue, kwaObj.len);

  /* Check if MGMTStation is a valid IP Addr */
	   memset(tempObjMgmtStationValue, 0x00, sizeof(tempObjMgmtStationValue));
	   if(strcmp(objMgmtStationValue,"0.0.0.0")  == 0 )
	   {
			isMgmtStationAll = XLIB_TRUE;
			strcpy(tempObjMgmtStationValue, "0.0.0.0:162");
		}
		else
		{
			if (usmDbInetAton(objMgmtStationValue, &objMgmtStationIPADDRValue) != L7_SUCCESS)
			{
				  owa.rc = XLIBRC_SNMP_MGMTSTATION_IPADDR_INVALID;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
			}
			strcat(tempObjMgmtStationValue, ":162");
			isMgmtStationAll = XLIB_FALSE;
		}

  /* objAccessModeValue  */
		kwaObj.len = sizeof (objAccessModeValue);
		kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_AccessMode,
			                      (xLibU8_t *)  &objAccessModeValue, &kwaObj.len);
		if (kwaObj.rc != XLIBRC_SUCCESS)
		{
		  objAccessModeValue = L7_XUI_SNMP_COMMUNITY_READ_ONLY;
		}
		FPOBJ_TRACE_VALUE (bufp, &objAccessModeValue, kwaObj.len);

  /* objViewNameValue  */
		kwaObj.len = sizeof (objViewNameValue);
		kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_ViewName,
			                             (xLibU8_t *)  objViewNameValue, &kwaObj.len);
		if (kwaObj.rc != XLIBRC_SUCCESS)
		{
		  isViewNameSet = XLIB_FALSE;
		}
		else
		{
		   isViewNameSet = XLIB_TRUE;
		}

  /* objMgmtStationValue  */
		kwaObj.len = sizeof (objGroupNameValue);
		kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_GroupName,
			                  (xLibU8_t *)  objGroupNameValue, &kwaObj.len);
		if (kwaObj.rc != XLIBRC_SUCCESS)
		{
		  isGroupNameSet = XLIB_FALSE;
		}
		else
		{
		  isGroupNameSet = XLIB_TRUE;
		}

		 rc = usmDbSnmpConfapiSnmpCommunityStatusSet(objCommunityNameValue,
                            snmpRowStatus_createAndWait);
        if( rc  != L7_SUCCESS)
        {
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
        }


		if( isGroupNameSet == XLIB_FALSE) /* If basic setting */
		{
			 rc = usmDbSnmpConfapiSnmpCommunityNameSet(
						objCommunityNameValue, objCommunityNameValue);
	        if( rc != L7_SUCCESS)
	        {
	             usmDbSnmpConfapiSnmpCommunityStatusSet(objCommunityNameValue,snmpRowStatus_active);
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
	        }
	         
	         rc = usmDbSnmpConfapiSnmpCommunitySecurityNameSet(
					 	objCommunityNameValue, objCommunityNameValue);

	        if(rc  != L7_SUCCESS)
	        {
	             usmDbSnmpConfapiSnmpCommunityStatusSet(objCommunityNameValue,snmpRowStatus_active);
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
	        }

			 if(isMgmtStationAll == XLIB_TRUE )
	        	rc = usmDbSnmpConfapiSnmpCommunityTransportTagSet(objCommunityNameValue, "");
	        else
	        	rc = usmDbSnmpConfapiSnmpCommunityTransportTagSet(
	                                    objCommunityNameValue, objCommunityNameValue); 

				
	        if(rc != L7_SUCCESS)
	        {
	             usmDbSnmpConfapiSnmpCommunityStatusSet(objCommunityNameValue,snmpRowStatus_active);
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
	        }
	     
	         rc = usmDbSnmpConfapiSnmpCommunityStatusSet(objCommunityNameValue,snmpRowStatus_active);

	        if( rc != L7_SUCCESS)
	        {
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
	        }

           /* START OF NOT ALL STATIONS  */
			 if(isMgmtStationAll == XLIB_FALSE)
			 {
					memset(tempTargetAddrName, 0x00, sizeof(tempTargetAddrName));
					sprintf(tempTargetAddrName,"community_%s",objCommunityNameValue);


		           rc = usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
		                                   snmpRowStatus_createAndWait);
		           if(rc != L7_SUCCESS)
		           {
		                usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
					                                 snmpRowStatus_active);
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
		            }
		            
		            rc = usmDbSnmpConfapiSnmpTargetAddrTDomainSet(tempTargetAddrName,"snmpUDPDomain");
		           if(rc != L7_SUCCESS)
		           {
		                usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
					                                 snmpRowStatus_active);
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
		           }
		           memset(tempAddrString, 0x00, sizeof(tempAddrString));
		           sprintf(tempAddrString, "%s:0", objMgmtStationValue);
		           rc = usmDbSnmpConfapiSnmpTargetAddrTAddressSet(tempTargetAddrName,tempAddrString);
		           if(rc != L7_SUCCESS)
		          {
		                usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
					                                 snmpRowStatus_active);
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
		          }

					if(usmDbSnmpConfapiSnmpTargetAddrTagListSet(tempTargetAddrName, tempObjMgmtStationValue)
					                                             != L7_SUCCESS)
					{
					     usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
					                                 snmpRowStatus_active);
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
					}

					if(usmDbSnmpConfapiSnmpTargetAddrParamsSet(tempTargetAddrName,"")
					                                             != L7_SUCCESS)
					{
					     usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
					                                 snmpRowStatus_active);
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
					}

					if(usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
					                                 snmpRowStatus_active) != L7_SUCCESS)
					{
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
					} 
			 	}
			 
           /* ENd OF NOT ALL STATIONS  */

			    if(rc == L7_SUCCESS)
       
			    {  
			     rc = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv1,
					 	                objCommunityNameValue,
			                           snmpRowStatus_createAndWait);
			    }

			    if(rc == L7_SUCCESS)
				{
						  owa.rc = XLIBRC_FAILURE;
						  FPOBJ_TRACE_EXIT (bufp, owa);
						  return owa.rc;
				} 

					 
	          /**** START IF VIEW NAME NOT SET NOT SET *****/
	          if( isViewNameSet == XLIB_FALSE)
	          {
	              rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv1,objCommunityNameValue, objCommunityNameValue);
	          }
	          else
	          { 
	               switch(objAccessModeValue)
					 {
						 case  L7_XUI_SNMP_COMMUNITY_READ_WRITE:
						 	 rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv1,
		                                                         objCommunityNameValue, "DefaultWrite"); 
						 	break;
						 case  L7_XUI_SNMP_COMMUNITY_SUPER_USER:
						 	rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv1,
		                                                        objCommunityNameValue,"DefaultSuper");
						 	break;
						 case  L7_XUI_SNMP_COMMUNITY_READ_ONLY:
						 default:
						 	rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv1,
		                                                        objCommunityNameValue, "DefaultRead");
					 }
	            }

			    if(rc == L7_SUCCESS)
	           {   
	             rc = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv1, 
	                                                          objCommunityNameValue, snmpRowStatus_active);             
	           } /*END OF V1 */

	           if(rc  != L7_SUCCESS)
	           {
					  owa.rc = XLIBRC_FAILURE;
					  FPOBJ_TRACE_EXIT (bufp, owa);
					  return owa.rc;
	            }
	          /**** START IF VIEW NAME NOT SET NOT SET *****/


				/** START of V2 ***/

				if(rc == L7_SUCCESS)
				{
					rc = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv2c,
						         objCommunityNameValue,  snmpRowStatus_createAndWait);
				}

	           if(rc  != L7_SUCCESS)
	           {
					  owa.rc = XLIBRC_FAILURE;
					  FPOBJ_TRACE_EXIT (bufp, owa);
					  return owa.rc;
	            }


				if( isViewNameSet == XLIB_FALSE)
				{
				    rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv2c,
							         objCommunityNameValue, objCommunityNameValue);
				}
				else
				{ 
				     	switch(objAccessModeValue)
						{
							 case  L7_XUI_SNMP_COMMUNITY_READ_WRITE:
							 	 rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv2c,
							                                                 objCommunityNameValue, "DefaultWrite"); 
							 	break;
							 case  L7_XUI_SNMP_COMMUNITY_SUPER_USER:
							 	rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv2c,
							                                                objCommunityNameValue,"DefaultSuper");
							 	break;
							 case  L7_XUI_SNMP_COMMUNITY_READ_ONLY:
							 default:
							 	rc = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv2c,
							                                                objCommunityNameValue, "DefaultRead");
						}
				 }

				if(rc == L7_SUCCESS)
				 {   
				   rc = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv2c, 
				                                                objCommunityNameValue, snmpRowStatus_active);             
				 } /*END OF V2 */

				 if(rc  != L7_SUCCESS)
				 {
						owa.rc = XLIBRC_FAILURE;
						FPOBJ_TRACE_EXIT (bufp, owa);
						return owa.rc;
				  }
				 
		        securityLevel = (char) securityLevelValue;
		        memset(contextPrefix,0x00,sizeof(contextPrefix));
		        if( isViewNameSet == XLIB_TRUE )
		        {
		            if (rc == L7_SUCCESS)
				    {
			              rc1 = usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"",
			             		snmpSecurityModel_snmpv1,securityLevelValue, snmpRowStatus_createAndWait);
				    }
							
			        if (rc == L7_SUCCESS)
				    {
			              rc2 = usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"",
			              snmpSecurityModel_snmpv2c,securityLevelValue, snmpRowStatus_createAndWait);
				    }

		        memset(strSysAttr,0x00,sizeof(strSysAttr));
		        memset(strOidTree,0x00,sizeof(strOidTree));

				  viewNameFound = XLIB_FALSE;
				  rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet(strSysAttr, strOidTree);
				  while (rc == L7_SUCCESS)
				  {
				    if (strcmp(strSysAttr, objViewNameValue) == L7_NULL)
				    {
				       viewNameFound = XLIB_TRUE;
						break;
				    }
				    rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet(strSysAttr, strOidTree);
				  }

				  if(viewNameFound != XLIB_TRUE)
				  {
						owa.rc = XLIBRC_FAILURE;
						FPOBJ_TRACE_EXIT (bufp, owa);
						return owa.rc;
				  }

		          if ((rc1 == L7_SUCCESS) )
		          {
		            rc1 = usmDbSnmpConfapiVacmAccessReadViewNameSet(objCommunityNameValue,"",
		          		snmpSecurityModel_snmpv1,securityLevelValue,strSysAttr);
			   		}
		  
		          if (rc2 == L7_SUCCESS)
				    {
			              rc2 = usmDbSnmpConfapiVacmAccessReadViewNameSet(objCommunityNameValue,"",
			            		snmpSecurityModel_snmpv2c,securityLevelValue,strSysAttr);
				    }

		          if((objAccessModeValue== L7_XUI_SNMP_COMMUNITY_READ_WRITE) || (objAccessModeValue == L7_XUI_SNMP_COMMUNITY_SUPER_USER))
		          {
		             rc1 = usmDbSnmpConfapiVacmAccessWriteViewNameSet(objCommunityNameValue,
		                               "",snmpSecurityModel_snmpv1,
		                               securityLevelValue,strSysAttr);
								 
		            if(rc1 != L7_SUCCESS)
		            {
		              usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"", snmpSecurityModel_snmpv1,
						                  securityLevelValue, snmpRowStatus_active);
						owa.rc = XLIBRC_FAILURE;
						FPOBJ_TRACE_EXIT (bufp, owa);
						return owa.rc;
		            }
		         
		             if(rc2 == L7_SUCCESS)
		             {
							rc2 = usmDbSnmpConfapiVacmAccessWriteViewNameSet(objCommunityNameValue,"",
							                 snmpSecurityModel_snmpv2c,  securityLevelValue,strSysAttr);
							if(rc2!= L7_SUCCESS)
							{
							   usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"", snmpSecurityModel_snmpv2c,
						                  securityLevelValue, snmpRowStatus_active);
								owa.rc = XLIBRC_FAILURE;
								FPOBJ_TRACE_EXIT (bufp, owa);
								return owa.rc;
							}
		            }
		          }

					if(rc1 == L7_SUCCESS)
					{
						rc1 = usmDbSnmpConfapiVacmAccessNotifyViewNameSet(objCommunityNameValue,"",
						                           snmpSecurityModel_snmpv1,securityLevelValue,strSysAttr);
						if (rc1 !=L7_SUCCESS)
						{
						   usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"", snmpSecurityModel_snmpv1,
						                  securityLevelValue, snmpRowStatus_active);
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
						}
					}

					if(rc2 == L7_SUCCESS)
					{
						rc2 = usmDbSnmpConfapiVacmAccessNotifyViewNameSet(objCommunityNameValue,"",
						                                        snmpSecurityModel_snmpv2c,securityLevelValue,strSysAttr);
						if (rc2 !=L7_SUCCESS)
						{
						    usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"", snmpSecurityModel_snmpv2c,
						                  securityLevelValue, snmpRowStatus_active);
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
						}
					}

					if(rc1 == L7_SUCCESS)
					{

						rc1 = usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"", snmpSecurityModel_snmpv1,
						                  securityLevelValue, snmpRowStatus_active);
						if ( rc1 !=L7_SUCCESS)
						{
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
						}
					}

					rc2 = usmDbSnmpConfapiVacmAccessStatusSet(objCommunityNameValue,"",
					       snmpSecurityModel_snmpv2c,securityLevelValue, snmpRowStatus_active);
					if ( rc2 !=L7_SUCCESS)
					{
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
					}
							
		        }      


			 }/*end of else for isgroupnameset == XLIB_FALSE */
			 else
			 {
	            /* create community */
	           if (rc == L7_SUCCESS)
	           {
	             rc = usmDbSnmpConfapiSnmpCommunityNameSet(objCommunityNameValue,objCommunityNameValue);
	            }
	            if (rc == L7_SUCCESS)
	            {
	             rc = usmDbSnmpConfapiSnmpCommunitySecurityNameSet(objCommunityNameValue,objCommunityNameValue);
	            }
	            if (rc == L7_SUCCESS)
	            {
	              if (isMgmtStationAll == XLIB_FALSE)
	              {
	               rc = usmDbSnmpConfapiSnmpCommunityTransportTagSet(objCommunityNameValue,objCommunityNameValue);
	              }
	            else
	              {
	               rc = usmDbSnmpConfapiSnmpCommunityTransportTagSet(objCommunityNameValue, "");
	               }
	            }
	             if (rc == L7_SUCCESS)
	            {
	             rc = usmDbSnmpConfapiSnmpCommunityStatusSet(objCommunityNameValue, snmpRowStatus_active);
	            }


			     if(isMgmtStationAll == XLIB_FALSE)
				 {
				    memset(tempTargetAddrName, 0x00, sizeof(tempTargetAddrName));
		           sprintf(tempTargetAddrName, "community_%s", objCommunityNameValue);
		           rc = usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
		                                   snmpRowStatus_createAndWait);
		           if(rc != L7_SUCCESS)
		           {
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
		            }
		           if( rc == L7_SUCCESS )
		           {
			           rc = usmDbSnmpConfapiSnmpTargetAddrTDomainSet(tempTargetAddrName,"snmpUDPDomain");
			           if ( rc != L7_SUCCESS)
			           {
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
			           }
		           }

                  memset(tempAddrString, 0x00, sizeof(tempAddrString));
		           sprintf(tempAddrString, "%s:0", objMgmtStationValue);

				
		           if( rc == L7_SUCCESS )
		           {
			           rc = usmDbSnmpConfapiSnmpTargetAddrTAddressSet(tempTargetAddrName,tempAddrString);
			           if ( rc != L7_SUCCESS)
			          {
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
			          }
		           }

		           if( rc == L7_SUCCESS )
		           {
			            rc = usmDbSnmpConfapiSnmpTargetAddrTagListSet(tempTargetAddrName,objCommunityNameValue);
			            if ( rc != L7_SUCCESS)
				        {
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
				        }
		           }

		           if( rc == L7_SUCCESS )
		           {
				        rc = usmDbSnmpConfapiSnmpTargetAddrParamsSet(tempTargetAddrName,"");
				        if( rc != L7_SUCCESS) 

				         {
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
				        }
		           }

		           if( rc == L7_SUCCESS )
		           {
				        rc = usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName,
				                                         snmpRowStatus_active);
				        if ( rc != L7_SUCCESS)
				        {
							owa.rc = XLIBRC_FAILURE;
							FPOBJ_TRACE_EXIT (bufp, owa);
							return owa.rc;
				        }
		           }
		      }


	    /* v1 */
				if (rc == L7_SUCCESS)
				{
				 rc1 = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv1,
				                             objCommunityNameValue,snmpRowStatus_createAndWait);
				}
				if (rc1 == L7_SUCCESS)
				{
				 rc1 = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv1,objCommunityNameValue,objGroupNameValue);
				}
				if (rc1 == L7_SUCCESS)
				{
				 rc1 = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv1, 
				                                                      objCommunityNameValue, snmpRowStatus_active);
				}
				/* v2c */
				if (rc == L7_SUCCESS)
				{
				 rc2 = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv2c, 
				                                                objCommunityNameValue, snmpRowStatus_createAndWait);
				}
				if (rc2 == L7_SUCCESS)
				{
				rc2 = usmDbSnmpConfapiVacmGroupNameSet(snmpSecurityModel_snmpv2c,objCommunityNameValue,objGroupNameValue);
				}
				if (rc2 == L7_SUCCESS)
				{
				rc2 = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_snmpv2c, 
				                                                     objCommunityNameValue,snmpRowStatus_active);
				}

				/* get the community index for the given community name and push it to filter */
             owa.rc = fpObjUtil_baseSNMPCommunity_IndexFromCommunityNameGet(objCommunityNameValue,
		                                                                                    keyCommunityIndexValue);
			  if(owa.rc == XLIBRC_SUCCESS)
			  {
			  		owa.rc = xLibFilterSet(wap,XOBJ_baseSNMPCommunity_CommunityIndex,0,
							           (xLibU8_t *) keyCommunityIndexValue, sizeof(keyCommunityIndexValue)); 
	              if(owa.rc !=XLIBRC_SUCCESS)
	              {
						owa.rc = XLIBRC_FAILURE;
						FPOBJ_TRACE_EXIT (bufp, owa);
						return owa.rc;
	              }  
			  }
			  else
			  {
						owa.rc = XLIBRC_FAILURE;
						FPOBJ_TRACE_EXIT (bufp, owa);
						return owa.rc;
			  }
				

				
 	 } /*end of  for isgroupnameset == XLIB_FALSE */
	 
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = L7_SUCCESS;
    /* Community Name */
		kwaObj.len = sizeof (objCommunityNameValue);
		kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_CommunityName,
			            (xLibU8_t *)  objCommunityNameValue, &kwaObj.len);
		if (kwaObj.rc != XLIBRC_SUCCESS)
		{
				  owa.rc = XLIBRC_FAILURE;
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
	   }
		FPOBJ_TRACE_VALUE (bufp, objCommunityNameValue, kwaObj.len);

		/* Now get the community index for given community name */

		/*Get the community name from the index */
		owa.rc = fpObjUtil_baseSNMPCommunity_IndexFromCommunityNameGet(objCommunityNameValue,
		                                                                                    keyCommunityIndexValue);
		if (owa.rc != XLIBRC_SUCCESS)
		{
			owa.rc = XLIBRC_FAILURE;
			FPOBJ_TRACE_EXIT (bufp, owa);
			return owa.rc;
		}

	   if(usmDbSnmpConfapiSnmpCommunityStatusSet(keyCommunityIndexValue,snmpRowStatus_destroy) != L7_SUCCESS)
      {
			owa.rc = XLIBRC_FILTER_MISSING;
			FPOBJ_TRACE_EXIT (bufp, owa);
			return owa.rc;
      }
      
      /* objMgmtStationValue  */
      kwaObj.len = sizeof (objMgmtStationValue);
      kwaObj.rc = xLibFilterGet (wap, XOBJ_baseSNMPCommunity_MgmtStation,
                                                (xLibU8_t *)  objMgmtStationValue, &kwaObj.len);
      if (kwaObj.rc != XLIBRC_SUCCESS)
      {
         owa.rc = XLIBRC_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }
      FPOBJ_TRACE_VALUE (bufp, objMgmtStationValue, kwaObj.len);

      /* We are not getting any value filled in for objMgmtStationValue form filter 
       * and this happens when objMgmtStationValue is of type Any. Hence the return
       * cacher of the below routine is removed for now
       */ 
        memset(tempTargetAddrName, 0x00, sizeof(tempTargetAddrName));
        sprintf(tempTargetAddrName,"community_%s",keyCommunityIndexValue);
        usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(tempTargetAddrName, snmpRowStatus_destroy); 
  }
  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
