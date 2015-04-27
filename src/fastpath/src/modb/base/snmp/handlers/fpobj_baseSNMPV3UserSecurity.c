
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSNMPV3UserSecurity.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  14 December 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseSNMPV3UserSecurity_obj.h"

#include <stdlib.h>
#include <ctype.h>
#include "usmdb_snmp_confapi_api.h"
#include "usmdb_snmp_trap_api.h"
#include "user_mgr_api.h"

#define SNMP_SERVER_AUTH_MD5_KEY_LEN 32
#define SNMP_SERVER_AUTH_SHA_KEY_LEN 40

L7_RC_t fpobjUtil_SNMPV3UserSecurity_ConvertToKey(L7_char8 *strKey, L7_char8 *strKey_Hexa, L7_uint32 *length)
{
  L7_uint32 digit_count = 0;
  L7_uint32 hexdigit = 0;
  L7_uint32 i, val;
  L7_uchar8 c;

  for (i = 0, hexdigit = 0; i < strlen(strKey); i++)
  {
    c = strKey[i];

    digit_count++;
    if (isdigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
    {
      if (digit_count >= 3)
      {
        digit_count = 1;
        hexdigit++;
      }

      if (isdigit(c))
      {
        val = c - '0';
      }
      else
      {
        if ((c >= 'a') && (c <= 'f'))
        {
          val = 10 + (c - 'a');
        }
        else
        {
          val = 10 + (c - 'A');
        }
      }

      strKey_Hexa[hexdigit] = ((strKey_Hexa[hexdigit] * 16) + val);

      continue;
    }
    else
      return L7_FAILURE;
  }

  *length = hexdigit + 1;
  return L7_SUCCESS;
}

void  fpobjUtil_SNMPV3UserSecurity_ConvertToLowerCase(L7_char8 *buf)
{
  L7_char8 c_tmp;
  L7_uint32 i;
  for (i = 0; i < strlen(buf); i++)
  {
    if (buf[i] != '\n' || buf[i] != '\0')
    {
      c_tmp = (L7_char8)tolower(buf[i]);
      buf[i] = c_tmp;
    }
  }
  return;
}

L7_RC_t fpobjUtil_SNMPV3UserSecurity_ConvertEngineId(L7_char8 *strPrivKey, L7_char8 *strPrivKey_Hexa, L7_uint32 *length, L7_uint32 pad)
{
  L7_uint32 digit_count = 0;
  L7_uint32 hexdigit = 0;
  L7_uint32 i, val;
  L7_uchar8 c;

  if(strPrivKey == L7_NULLPTR)
  return L7_FAILURE;
  
  if(strPrivKey_Hexa == L7_NULLPTR)
   return L7_FAILURE;
  
  if(length == L7_NULLPTR)
    return L7_FAILURE;
  
  if ((strlen(strPrivKey) < L7_SNMP_MIN_ENGINE_SIZE) ||
          (strlen(strPrivKey) > L7_SNMP_MAX_ENGINE_SIZE) ||
          ((strlen(strPrivKey)%2) != 0))
    return L7_FAILURE;

  memset(strPrivKey_Hexa, 0x00, sizeof(strPrivKey_Hexa));

  fpobjUtil_SNMPV3UserSecurity_ConvertToLowerCase(strPrivKey);
  for (i = 0, hexdigit = 0; (i < strlen(strPrivKey)) || i < pad; i++)
  {
    if (i<strlen(strPrivKey))
    {
      c = strPrivKey[i];
    }
    else
    {
      c = '0';
    }

    digit_count++;
    if (isdigit(c)||((c >= 'a') && (c <= 'f')))
    {
      if (digit_count >= 3)
      {
        digit_count = 1;
        hexdigit++;
      }

      if (isdigit(c))
      {
        val = c - '0';
      }
      else
      {
        val = 10 + (c - 'a');
      }

      strPrivKey_Hexa[hexdigit] = ((strPrivKey_Hexa[hexdigit] * 16) + val);

      continue;
    }
    else
      return L7_FAILURE;
  }

  *length = hexdigit + 1;
  return L7_SUCCESS;
}

xLibRC_t fpObjUtil_SNMPV3UserSecurity_GetUserInfo(L7_uint32 userIndex, L7_char8 *usmUserEngineID, L7_uint32 *EngineID_len, L7_char8 *usmUserName)
{
  L7_RC_t rc;
  xLibU32_t usrIndex;
	
  if(userIndex < 1) /*1 based index */
		return L7_FAILURE;

  memset(usmUserName, 0x00, sizeof(usmUserName));
  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  *EngineID_len = 0;

  
  for(usrIndex = 1; usrIndex <= userIndex; usrIndex++)
  {
    rc = usmDbSnmpConfapiUsmUserTableNextGet(usmUserEngineID, EngineID_len,
                                  usmUserName);
	if(rc != L7_SUCCESS)
	{
		memset(usmUserName, 0x00, sizeof(usmUserName));
		memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
		*EngineID_len = 0;
		return L7_FAILURE;
	}
  }
  return L7_SUCCESS;
}

L7_RC_t fpObjUtil_SNMPV3UserSecurity_GetUserIndex(L7_char8 *usmUserName, L7_uint32 *userIndex )
{
  L7_RC_t rc;
  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;
  xLibStr256_t userNameTemp;
  *userIndex = 0;
	
  memset(userNameTemp, 0x00, sizeof(userNameTemp));
  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  EngineID_len = 0;

  *userIndex = 0;
  rc = L7_SUCCESS;
  while(rc == L7_SUCCESS)
  {
    rc = usmDbSnmpConfapiUsmUserTableNextGet(usmUserEngineID, &EngineID_len,
                                  userNameTemp);
	if(rc != L7_SUCCESS)
	{
	   *userIndex = 0;
		return L7_FAILURE;
	}
    (*userIndex)++;
	if(strcmp(usmUserName, userNameTemp) == 0 )
         break;	
  }
  return L7_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_UserIndex
*
* @purpose Get 'UserIndex'
 *@description  [UserIndex] SNMP user index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_UserIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUserIndexValue;
  xLibU32_t nextObjUserIndexValue;

  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(usmUserName, 0x00, sizeof(usmUserName));
  EngineID_len = 0;
  nextObjUserIndexValue = 0; /* its 1 based index */
  objUserIndexValue = 0; /* its 1 based index */
	

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (objUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & objUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objUserIndexValue = 1; /* its 1 based index */
    owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(objUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objUserIndexValue, owa.len);
    objUserIndexValue++;
    owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(objUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjUserIndexValue = objUserIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUserIndexValue, owa.len);

  /* return the object value: UserIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUserIndexValue,
                           sizeof (nextObjUserIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_UserName
*
* @purpose Get 'UserName'
 *@description  [UserName] SNMP user name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_UserName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUserNameValue;
  xLibU32_t keyUserIndexValue;

  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(objUserNameValue, 0x00, sizeof(objUserNameValue));
  EngineID_len = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(keyUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  objUserNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objUserNameValue, strlen (objUserNameValue));

  /* return the object value: UserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUserNameValue, strlen (objUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_UserName
*
* @purpose Set 'UserName'
 *@description  [UserName] SNMP user name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_UserName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUserNameValue;
  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UserName */
  owa.len = sizeof (objUserNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserNameValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);
  return owa.rc; /* The username will be added in row-status object */
}

/*******************************************************************************
* @function fpObjList_baseSNMPV3UserSecurity_GroupName
*
* @purpose Get 'GroupName'
 *@description  [GroupName] SNMP group name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSNMPV3UserSecurity_GroupName (void *wap, void *bufp)
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

  /* retrieve key: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_GroupName,
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
* @function fpObjGet_baseSNMPV3UserSecurity_GroupName
*
* @purpose Get 'GroupName'
 *@description  [GroupName] SNMP group name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_GroupName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupNameValue;
  xLibU32_t keyUserIndexValue;
  L7_uint32 vacmSecurityModel;
  xLibStr256_t vacmSecurityName;

  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;
  xLibStr256_t  usmUserEngineID;

  FPOBJ_TRACE_ENTER (bufp);
  vacmSecurityModel=0;
  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(usmUserName, 0x00, sizeof(usmUserName));
  EngineID_len = 0;

  /* get the value from application */
  owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(keyUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
  memset(vacmSecurityName,0x00,sizeof(vacmSecurityName));
  owa.l7rc = L7_FAILURE;
  while(usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(&vacmSecurityModel,
	                                                vacmSecurityName) == L7_SUCCESS)
  {
		if(strcmp(vacmSecurityName,usmUserName) == L7_SUCCESS)
		{
		     owa.l7rc = usmDbSnmpConfapiVacmGroupNameGet(vacmSecurityModel, vacmSecurityName,
		                                                     objGroupNameValue);
			  break;
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
* @function fpObjSet_baseSNMPV3UserSecurity_GroupName
*
* @purpose Set 'GroupName'
 *@description  [GroupName] SNMP group name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_GroupName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupNameValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupNameValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  return owa.rc; /* This object will be added in row-status object */

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_EngineID
*
* @purpose Get 'EngineID'
 *@description  [EngineID] SNMP EngineID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_EngineID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objEngineIDValue;
  xLibU32_t keyUserIndexValue;
  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;
  xLibU32_t indx;
  xLibStr6_t stat;
  xLibStr256_t  usmUserEngineID;

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(objEngineIDValue, 0x00, sizeof(objEngineIDValue));
  memset(usmUserName, 0x00, sizeof(usmUserName));
  EngineID_len = 0;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(keyUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
  for(indx=0; indx<EngineID_len; indx++)
  {
    osapiSnprintf(stat, sizeof(stat), "%02x", usmUserEngineID[indx]);
    osapiStrncat(objEngineIDValue, stat, sizeof(stat));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objEngineIDValue, strlen (objEngineIDValue));

  /* return the object value: EngineID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objEngineIDValue, strlen (objEngineIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_EngineID
*
* @purpose Set 'EngineID'
 *@description  [EngineID] SNMP EngineID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_EngineID (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objEngineIDValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EngineID */
  owa.len = sizeof (objEngineIDValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objEngineIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objEngineIDValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* set the value in application */
  return owa.rc;  /* will be set in row-status object */

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_EngineIDType
*
* @purpose Get 'EngineIDType'
 *@description  [EngineIDType] SNMP EngineID Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_EngineIDType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEngineIDTypeValue;

  xLibU32_t keyUserIndexValue;

  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;
  xLibStr256_t localEngineID;
	
  xLibU32_t length;
  xLibU32_t algorithm; 

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* get the value from application */

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(usmUserName, 0x00, sizeof(usmUserName));
  EngineID_len = 0;

  owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(keyUserIndexValue,
		                  usmUserEngineID,
		                  &EngineID_len,
		                  usmUserName);
  if (owa.l7rc != L7_SUCCESS)
  {
	owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
  }	
  memset(localEngineID,0x00,sizeof(localEngineID));
  length = 0;
  owa.l7rc = usmDbSnmpConfapiSnmpServerLocalEngineIDGet(localEngineID, &length, &algorithm);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   if(strcmp( usmUserEngineID, localEngineID) == L7_NULL )
        objEngineIDTypeValue=L7_ENABLE;
   else
        objEngineIDTypeValue=L7_DISABLE;  
	 
  FPOBJ_TRACE_VALUE (bufp, &objEngineIDTypeValue, sizeof (objEngineIDTypeValue));

  /* return the object value: EngineIDType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEngineIDTypeValue,
                           sizeof (objEngineIDTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_EngineIDType
*
* @purpose Set 'EngineIDType'
 *@description  [EngineIDType] SNMP EngineID Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_EngineIDType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEngineIDTypeValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EngineIDType */
  owa.len = sizeof (objEngineIDTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEngineIDTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEngineIDTypeValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* set the value in application */
  return owa.rc; /*this value will be set in the row-status object */

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_AccessMode
*
* @purpose Get 'AccessMode'
 *@description  [AccessMode] SNMP Access Mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_AccessMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAccessModeValue;

  xLibU32_t keyUserIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  owa.l7rc = userMgrLoginUserSnmpv3AccessModeGet(keyUserIndexValue, &objAccessModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAccessModeValue, sizeof (objAccessModeValue));

  /* return the object value: AccessMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAccessModeValue,
                           sizeof (objAccessModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_AccessMode
*
* @purpose Set 'AccessMode'
 *@description  [AccessMode] SNMPAccess Mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_AccessMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAccessModeValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AccessMode */
  owa.len = sizeof (objAccessModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAccessModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAccessModeValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = userMgrLoginUserSnmpv3AccessModeSet(keyUserIndexValue, objAccessModeValue);

  if(owa.l7rc !=L7_SUCCESS)
  {
     owa.rc = L7_FAILURE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);
 /* set the value in application */
  return owa.rc; /*this value will be set in the row-status object */

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_AuthenticationMethod
*
* @purpose Get 'AuthenticationMethod'
 *@description  [AuthenticationMethod] SNMP Authentication Method   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_AuthenticationMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t tempAuthenticationMethodValue;

  xLibU32_t keyUserIndexValue;
  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;
  xLibU32_t objAuthenticationMethodValue;

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(usmUserName, 0x00, sizeof(usmUserName));
  EngineID_len = 0;

 

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(keyUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
		 

  owa.l7rc = usmDbSnmpConfapiUsmUserAuthProtocolGet(usmUserEngineID,
                                            EngineID_len,usmUserName,&tempAuthenticationMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  switch(tempAuthenticationMethodValue)
  {
   case usmUserAuthProtocol_none:
        objAuthenticationMethodValue = L7_USM_USER_AUTH_PROTO_NONE;
       break;
   case usmUserAuthProtocol_sha:
        objAuthenticationMethodValue = L7_USM_USER_AUTH_PROTO_SHA;
       break;
   case usmUserAuthProtocol_md5:
        objAuthenticationMethodValue = L7_USM_USER_AUTH_PROTO_MD5;
       break;
   default:
        objAuthenticationMethodValue = L7_USM_USER_AUTH_PROTO_NONE;
       break;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationMethodValue, sizeof (objAuthenticationMethodValue));

  /* return the object value: AuthenticationMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationMethodValue,
                           sizeof (objAuthenticationMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_AuthenticationMethod
*
* @purpose Set 'AuthenticationMethod'
 *@description  [AuthenticationMethod] SNMP Authentication Method   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_AuthenticationMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationMethodValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationMethod */
  owa.len = sizeof (objAuthenticationMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthenticationMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationMethodValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* set the value in application */
  return owa.rc;

}



/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_Password
*
* @purpose Set 'Password'
 *@description  [Password] SNMP password   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_Password (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objPasswordValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Password */
  owa.len = sizeof (objPasswordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objPasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objPasswordValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* set the value in application */
  return owa.rc; /*this value will be handled in row-status object */

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_PrivacyType
*
* @purpose Get 'PrivacyType'
 *@description  [PrivacyType] SNMP Privacy Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_PrivacyType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPrivacyTypeValue;

  xLibU32_t keyUserIndexValue;
  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;

  memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
  memset(usmUserName, 0x00, sizeof(usmUserName));
  EngineID_len = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(keyUserIndexValue,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  owa.l7rc = usmDbSnmpConfapiUsmUserPrivProtocolGet(usmUserEngineID,
								   EngineID_len, usmUserName,
                               &objPrivacyTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objPrivacyTypeValue, sizeof (objPrivacyTypeValue));

  /* return the object value: PrivacyType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPrivacyTypeValue, sizeof (objPrivacyTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_PrivacyType
*
* @purpose Set 'PrivacyType'
 *@description  [PrivacyType] SNMP Privacy Type   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_PrivacyType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPrivacyTypeValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PrivacyType */
  owa.len = sizeof (objPrivacyTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPrivacyTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPrivacyTypeValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* set the value in application */
  return owa.rc; /*this value will be handled in row-status object */

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_AuthenticationKey
*
* @purpose Set 'AuthenticationKey'
 *@description  [AuthenticationKey] SNMP Authentication key   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_AuthenticationKey (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthenticationKeyValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationKey */
  owa.len = sizeof (objAuthenticationKeyValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)  objAuthenticationKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthenticationKeyValue, owa.len);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* set the value in application */
  return owa.rc; /* this value will be handled in row-status object */

}

/*******************************************************************************
* @function fpObjGet_baseSNMPV3UserSecurity_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Groups.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPV3UserSecurity_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibU32_t keyUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  owa.len = sizeof (keyUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserIndex,
                          (xLibU8_t *) & keyUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, owa.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_INVALID;
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPV3UserSecurity_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Groups.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPV3UserSecurity_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t usmUserEngineID;
  xLibU32_t EngineID_len;
  xLibStr256_t usmUserName;

  xLibStr256_t objUserNameValue;
  xLibStr256_t objGroupNameValue;

  xLibU32_t objEngineIDTypeValue;
  xLibU32_t objAuthenticationMethodValue;
  xLibStr256_t objPasswordValue;
  xLibU32_t objPrivacyTypeValue;
  xLibStr256_t objAuthenticationKeyValue;
  xLibU32_t length;
  xLibU32_t algorithm;
  L7_RC_t rc;
  xLibU32_t passwordLength;
  xLibU32_t passwordAuthKeyLength;
  xLibU32_t tmpIndex;

  xLibStr256_t authKey;  
  L7_uint32 authKey_len=0; 
	
  xLibStr256_t privKey;  
  L7_uint32 privKey_len=0; 
  L7_uint32 new_ndx;

  xLibStr256_t strEngineId;
  xLibU32_t tempAuthenticationMethodValue;
  /* This magic number(128) is derived from L7_CLI_MAX_STRING_LENGTH
   * this need to be changed. This is defined basically to solve XLIB
   * type casting issues
   */
  xLibU8_t  objRemoteEngineIDValue[128];                                         
 

  FPOBJ_TRACE_ENTER (bufp);

  /* Initialize the fields first before doing any thing */
   FPOBJ_CLR_STR256 (usmUserEngineID);
   FPOBJ_CLR_STR256 (usmUserName);
   FPOBJ_CLR_STR256 (objUserNameValue);
   FPOBJ_CLR_STR256 (objGroupNameValue);
   FPOBJ_CLR_STR256 (objPasswordValue);
   FPOBJ_CLR_STR256 (objAuthenticationKeyValue);
   FPOBJ_CLR_STR256 (strEngineId);
   FPOBJ_CLR_STR256 (authKey);
   FPOBJ_CLR_STR256 (privKey);
   FPOBJ_CLR_U32(length);
   FPOBJ_CLR_U32(tmpIndex);
   FPOBJ_CLR_U32(passwordLength);
   FPOBJ_CLR_U32(passwordAuthKeyLength);
   FPOBJ_CLR_STR256 (tempAuthenticationMethodValue);
   FPOBJ_CLR_STR256 (objRemoteEngineIDValue);

   /* retrieve object: RowStatus */
   owa.len = sizeof (objRowStatusValue);
   owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);
  owa.l7rc = L7_SUCCESS;
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
      /*Get all the required objects here */
      /*get the type of the user */
      /* retrieve object: EngineIDType */
      owa.len = sizeof (objEngineIDTypeValue);
      owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_EngineIDType,
                          (xLibU8_t *) & objEngineIDTypeValue, &owa.len);
      if (owa.rc != XLIBRC_SUCCESS)
      {
       owa.rc = XLIBRC_FILTER_MISSING;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
      }
      FPOBJ_TRACE_VALUE (bufp, &objEngineIDTypeValue, owa.len);

      /*get the user name */
      /* retrieve object: UserName */
      owa.len = sizeof (objUserNameValue);
      owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserName,
                          (xLibU8_t *) objUserNameValue, &owa.len);

      if (owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, owa);
       	return owa.rc;
      }
      FPOBJ_TRACE_VALUE (bufp, objUserNameValue, owa.len);

      if(objEngineIDTypeValue == 0 ) /*Local engineID */
      {
        owa.l7rc = usmDbSnmpConfapiSnmpServerLocalEngineIDGet(
		 	               strEngineId, &length, &algorithm);
        if (owa.l7rc != L7_SUCCESS)
        {
           owa.rc = XLIBRC_FILTER_MISSING;
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
        }
        FPOBJ_TRACE_VALUE (bufp, strEngineId, strlen (strEngineId));
      }
      else
      {
         /* retrieve key: objEngineIDValue */
        owa.len = sizeof (objRemoteEngineIDValue);
        owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_EngineID,
                             (xLibU8_t *) objRemoteEngineIDValue, &owa.len);
        if (owa.rc != XLIBRC_SUCCESS)
        {
          owa.rc = XLIBRC_FILTER_MISSING;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }

        FPOBJ_TRACE_CURRENT_KEY (bufp, objRemoteEngineIDValue, owa.len);
          
        owa.l7rc = fpobjUtil_SNMPV3UserSecurity_ConvertEngineId(
			                objRemoteEngineIDValue, strEngineId, &length, 0);
        if (owa.l7rc != L7_SUCCESS)
        {
          owa.rc = XLIBRC_ENGINE_ID_OUT_OF_RANGE;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      } /*remote engineId end */
    
      /*check if the row is already exists */
      if( fpObjUtil_SNMPV3UserSecurity_GetUserIndex(objUserNameValue,&tmpIndex) == L7_SUCCESS)
      {
        owa.rc = XLIBRC_SNMPV3_SECURITY_USER_EXISTS;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      owa.l7rc = usmDbSnmpConfapiUsmUserStatusSet(
				strEngineId,length, objUserNameValue, snmpRowStatus_createAndWait);
      if(owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }
      owa.l7rc = usmDbSnmpConfapiUsmUserSecurityNameSet(
				            strEngineId,length, objUserNameValue,objUserNameValue);
      if(owa.l7rc !=L7_SUCCESS)
      {
        usmDbSnmpConfapiUsmUserStatusSet(
				strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
        owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

	/*read authtype and password objects */
	/* retrieve object: AuthenticationMethod */
	owa.len = sizeof (objAuthenticationMethodValue);
        owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_AuthenticationMethod,
                          (xLibU8_t *) & objAuthenticationMethodValue, &owa.len);
	if (owa.rc != XLIBRC_SUCCESS)
	{
	  usmDbSnmpConfapiUsmUserStatusSet(
			strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  owa.rc = XLIBRC_FILTER_MISSING;
	  return owa.rc;
	}
	FPOBJ_TRACE_VALUE (bufp, &objAuthenticationMethodValue, owa.len);
        
  /* retrieve object: Password only for Authentication types*/
  if(objAuthenticationMethodValue != 0)
  {
    owa.len = sizeof (objPasswordValue);
    owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_Password,
                         (xLibU8_t *) objPasswordValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
       usmDbSnmpConfapiUsmUserStatusSet(
       strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
       FPOBJ_TRACE_EXIT (bufp, owa);
       owa.rc = XLIBRC_FILTER_MISSING;
       return owa.rc;
    }
 	  FPOBJ_TRACE_VALUE (bufp, objPasswordValue, owa.len);
    passwordLength = strlen(objPasswordValue);     
  }
  
   switch(objAuthenticationMethodValue)
   {
     case 0:
            owa.rc = XLIBRC_SUCCESS; /* just to pass on this check*/
            tempAuthenticationMethodValue = usmUserAuthProtocol_none;
            break;
     case 1:
            if (passwordLength < 1 || passwordLength > 32)
            owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_PASSWD_OUT_OF_RANGE;
            tempAuthenticationMethodValue =  usmUserAuthProtocol_md5;
            break;
     case 2:
	         if (passwordLength < 1 || passwordLength > 32)
           owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_PASSWD_OUT_OF_RANGE;
           tempAuthenticationMethodValue =  usmUserAuthProtocol_sha;
            break;
     case 3:
            if(passwordLength != SNMP_SERVER_AUTH_MD5_KEY_LEN)
            owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_AUTH_MD5_KEY_LENGTH;
            tempAuthenticationMethodValue =  usmUserAuthProtocol_md5;
            /* Need to handle this return value down the line */
            fpobjUtil_SNMPV3UserSecurity_ConvertToKey(objPasswordValue, authKey, &authKey_len);
             break;
     case 4:
            if(passwordLength != SNMP_SERVER_AUTH_SHA_KEY_LEN)
            owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_AUTH_SHA_KEY_LEN;
            tempAuthenticationMethodValue =  usmUserAuthProtocol_sha;
               /* Need to handle this return value down the line */
               fpobjUtil_SNMPV3UserSecurity_ConvertToKey(objPasswordValue, authKey, &authKey_len);
              break;
     default:
                owa.rc = XLIBRC_FAILURE;
                tempAuthenticationMethodValue =  usmUserAuthProtocol_last;
              break;
   }/* First switch case End*/

   /* Destroy the key if any of the above checks fails at any level */  
   if (owa.rc != XLIBRC_SUCCESS)
	 {
	   usmDbSnmpConfapiUsmUserStatusSet(
		  strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
	      FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	 }
   if(tempAuthenticationMethodValue != 0)
   {
     if(strlen(authKey))
     {
       owa.l7rc = usmDbSnmpConfapiUsmUserAuthProtocolKeySet(
                            strEngineId,length,objUserNameValue,
                         tempAuthenticationMethodValue, authKey, authKey_len);
     }
     else
     {
        owa.l7rc = usmDbSnmpConfapiUsmUserAuthProtocolSet(
                            strEngineId,length,objUserNameValue,
                            tempAuthenticationMethodValue, objPasswordValue);
     }
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_USER_FAILURE;
        usmDbSnmpConfapiUsmUserStatusSet(
                  strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
   }
        /* retrieve object: PrivacyType */
        owa.len = sizeof (objPrivacyTypeValue);
        owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_PrivacyType,
                          (xLibU8_t *) &objPrivacyTypeValue, &owa.len);
	if (owa.rc != XLIBRC_SUCCESS)
	{
	   FPOBJ_TRACE_EXIT (bufp, owa);
           owa.rc = XLIBRC_FILTER_MISSING;
           return owa.rc;
	}
        FPOBJ_TRACE_VALUE (bufp, &objPrivacyTypeValue, owa.len);
        if(objPrivacyTypeValue != 0)
        { 
            /* retrieve object: AuthenticationKey */
	    owa.len = sizeof (objAuthenticationKeyValue);
            owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_AuthenticationKey,
                       (xLibU8_t *) objAuthenticationKeyValue, &owa.len);
            if (owa.rc != XLIBRC_SUCCESS)
	    {
	      FPOBJ_TRACE_EXIT (bufp, owa);
              owa.rc = XLIBRC_FILTER_MISSING;
              return owa.rc;
	    }
            passwordAuthKeyLength = strlen(objAuthenticationKeyValue);
        }
            
        if((tempAuthenticationMethodValue != 0 ) &&(objPrivacyTypeValue != 0))
        {
            switch(objPrivacyTypeValue)
            {
               case 1:
                      if (passwordAuthKeyLength < 1 || passwordAuthKeyLength > 32)
                      owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_PASSWD_OUT_OF_RANGE;
                      break;
               case 2:
                      switch(tempAuthenticationMethodValue)
                      {
                        case usmUserAuthProtocol_md5:
                             if (passwordAuthKeyLength != SNMP_SERVER_AUTH_MD5_KEY_LEN)
                             owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_AUTH_MD5_KEY_LENGTH;
                             break;
                        case usmUserAuthProtocol_sha:
                             if (passwordAuthKeyLength != SNMP_SERVER_AUTH_SHA_KEY_LEN)
                             owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_AUTH_SHA_KEY_LEN;
                             break;
 
                        default:
                             owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_INVALID_PASSWORD;
                             break; 
                      }
                      /* Need to handle this return value down the line */
                      fpobjUtil_SNMPV3UserSecurity_ConvertToKey(objAuthenticationKeyValue, privKey, &privKey_len);
                      break;
                default:
                      owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_INVALID_PASSWORD;
                      break;
            }/* Second switch case check end*/        
           
            /* Destroy the key if any of the above checks fails at any level */  
            if (owa.rc != XLIBRC_SUCCESS)
	          {
	            usmDbSnmpConfapiUsmUserStatusSet(
				      strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
		          FPOBJ_TRACE_EXIT (bufp, owa);
		          return owa.rc;
	          }
            if(objPrivacyTypeValue != 0)
            {
              if (strlen(privKey))
              {
                owa.l7rc = usmDbSnmpConfapiUsmUserPrivProtocolKeySet(
	              strEngineId,length,objUserNameValue, 
		            usmUserPrivProtocol_des, privKey, privKey_len);
	            }
              else
	            {
	              owa.l7rc = usmDbSnmpConfapiUsmUserPrivProtocolSet(
		 	          strEngineId,length,objUserNameValue, 
		 	          usmUserPrivProtocol_des, objAuthenticationKeyValue);
	            }
              if (owa.l7rc != L7_SUCCESS)
              {
                owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_USER_PRIV_KEY_FAILURE;
	              usmDbSnmpConfapiUsmUserStatusSet(
            	      strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
	              FPOBJ_TRACE_EXIT (bufp, owa);
	              return owa.rc;
	            }
            }
        }/* End of authKey and Privacy Type non null if check*/     
     	
  /* retrieve object: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_GroupName,
                          (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupNameValue, owa.len);

  owa.l7rc  = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(
				snmpSecurityModel_usm,objUserNameValue,
        			snmpRowStatus_createAndWait);

  if(owa.l7rc  != L7_SUCCESS)
  {
    owa.rc = XLIBRC_SNMPV3_SECURITY_USER_EXISTS;
    usmDbSnmpConfapiUsmUserStatusSet(
	  strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;         
  }

  owa.l7rc = usmDbSnmpConfapiVacmGroupNameSet(
	snmpSecurityModel_usm,objUserNameValue,objGroupNameValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_GROUP_FAIL;
    usmDbSnmpConfapiUsmUserStatusSet(
	  strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
    FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;      
  }

   owa.l7rc = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(
	 snmpSecurityModel_usm,objUserNameValue, snmpRowStatus_active);
   if (owa.l7rc != L7_SUCCESS)
   {
	   owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_GROUP_FAILURE;
	   usmDbSnmpConfapiUsmUserStatusSet(
		 strEngineId,length,objUserNameValue, snmpRowStatus_destroy);
	   usmDbSnmpConfapiVacmSecurityToGroupStatusSet(
		 snmpSecurityModel_usm, objUserNameValue, snmpRowStatus_destroy);
	   FPOBJ_TRACE_EXIT (bufp, owa);
	   return owa.rc;        
   } 
   else
   {
     rc = fpObjUtil_SNMPV3UserSecurity_GetUserIndex(
  	 objUserNameValue, &new_ndx);
     if(rc != L7_FAILURE)
     {
	   	/* push key into filter */
   	   xLibFilterSet(wap,XOBJ_baseSNMPV3UserSecurity_UserIndex,0,
	                       (xLibU8_t *) &new_ndx,
	                       sizeof(new_ndx));
     }
     else
     {
 	     owa.rc = XLIBRC_FAILURE;
	     FPOBJ_TRACE_EXIT (bufp, owa);
	     return owa.rc;        
     }
   }
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
  /*get the user name */
      /* retrieve object: UserName */
      owa.len = sizeof (objUserNameValue);
      owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPV3UserSecurity_UserName,
                          (xLibU8_t *) objUserNameValue, &owa.len);

      if (owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, owa);
       	return owa.rc;
      }
      FPOBJ_TRACE_VALUE (bufp, objUserNameValue, owa.len);
      /* Now get the keyUserIndex form User name */
      owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserIndex(objUserNameValue,&tmpIndex);
      if (owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
     /* retrieve key: UserIndex */
     memset(usmUserEngineID, 0x00, sizeof(usmUserEngineID));
     memset(usmUserName, 0x00, sizeof(usmUserName));
     EngineID_len = 0;
     owa.l7rc = fpObjUtil_SNMPV3UserSecurity_GetUserInfo(tmpIndex,
			                  usmUserEngineID,
			                  &EngineID_len,
			                  usmUserName);
     if (owa.l7rc != L7_SUCCESS)
     {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
	
     owa.l7rc =  usmDbSnmpConfapiUsmUserStatusSet(usmUserEngineID,
                                          EngineID_len,
                                          usmUserName,
                                          snmpRowStatus_destroy);
     if(owa.l7rc  == L7_SUCCESS)
     {
    	owa.l7rc = usmDbSnmpConfapiVacmSecurityToGroupStatusSet(snmpSecurityModel_usm,
                                                     usmUserName,
                                                     snmpRowStatus_destroy);
     }
     if (owa.l7rc != L7_SUCCESS)
     {
       owa.rc = XLIBRC_SNMPV3_SECURITY_ERROR_GENERIC_DEL_FAILURE;    /* TODO: Change if required */
     }
     FPOBJ_TRACE_EXIT (bufp, owa);
   }

 return owa.rc;

}
