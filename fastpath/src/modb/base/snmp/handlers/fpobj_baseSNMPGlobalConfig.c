
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSNMPGlobalConfig.c
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
#include "_xe_baseSNMPGlobalConfig_obj.h"

#include <stdlib.h>
#include <ctype.h>

#include "usmdb_snmp_confapi_api.h"
#include "usmdb_snmp_trap_api.h"

void  fpobjUtil_ConvertToLowerCase(L7_char8 *buf)
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

L7_RC_t fpobjUtil_ConvertEngineId(L7_char8 *strPrivKey, L7_char8 *strPrivKey_Hexa, L7_uint32 *length, L7_uint32 pad)
{
  L7_uint32 digit_count = 0;
  L7_uint32 hexdigit = 0;
  L7_uint32 i, val;
  L7_uchar8 c;

  memset(strPrivKey_Hexa, 0x00, sizeof(strPrivKey_Hexa));

  fpobjUtil_ConvertToLowerCase(strPrivKey);
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



/*******************************************************************************
* @function fpObjGet_baseSNMPGlobalConfig_LocalEngineID
*
* @purpose Get 'LocalEngineID'
 *@description  [LocalEngineID] SNMP Local engine ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGlobalConfig_LocalEngineID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objLocalEngineIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  L7_char8        stat[8];
  xLibStr256_t       engineId;
  L7_uint32       len = 0;
  L7_uint32       i;
  L7_uint32       algorithm;
	
  FPOBJ_CLR_STR256(engineId);
  owa.l7rc = usmDbSnmpConfapiSnmpServerLocalEngineIDGet(engineId, &len, &algorithm);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_CLR_STR256(objLocalEngineIDValue);
  for(i=0; i<len; i++)
  {
    osapiSnprintf(stat, sizeof(stat), "%02x", engineId[i]);
    osapiStrncat(objLocalEngineIDValue, stat, sizeof(stat));
  }
	
  FPOBJ_TRACE_VALUE (bufp, objLocalEngineIDValue, strlen (objLocalEngineIDValue));

  /* return the object value: LocalEngineID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLocalEngineIDValue,
                           strlen (objLocalEngineIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPGlobalConfig_LocalEngineID
*
* @purpose Set 'LocalEngineID'
 *@description  [LocalEngineID] SNMP Local engine ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGlobalConfig_LocalEngineID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objLocalEngineIDValue;
  xLibU32_t keyUseAsDefaultSNMPEngineIDValue;

  L7_uint32  engineid_length = 0;
  L7_uint32        algorithm = L7_SNMP_MANUAL_ENGINE;
  xLibStr256_t         engId_Hexa;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LocalEngineID */
  owa.len = sizeof (objLocalEngineIDValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objLocalEngineIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLocalEngineIDValue, owa.len);
  
  owa.len = sizeof (keyUseAsDefaultSNMPEngineIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGlobalConfig_UseAsDefaultSNMPEngineID,
                          (xLibU8_t *) & keyUseAsDefaultSNMPEngineIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUseAsDefaultSNMPEngineIDValue, owa.len);

  /* if row status object is specified and eual to delete return success */
  memset(engId_Hexa, 0x00, sizeof(engId_Hexa));
  if(keyUseAsDefaultSNMPEngineIDValue == L7_TRUE)
  {
      algorithm = L7_SNMP_DEFAULT_ENGINE;
      engineid_length = 0;
	   
  }
  else
  {
    algorithm = L7_SNMP_MANUAL_ENGINE;
    engineid_length = strlen (objLocalEngineIDValue);
    if ((engineid_length < L7_SNMP_MIN_ENGINE_SIZE) ||
          (engineid_length > L7_SNMP_MAX_ENGINE_SIZE) ||
          ((engineid_length % 2) != 0))
    {
	     owa.rc = XLIBRC_ENGINE_ID_OUT_OF_RANGE;
	     FPOBJ_TRACE_EXIT (bufp, owa);
	     return owa.rc;
    } 

	  owa.l7rc = fpobjUtil_ConvertEngineId (objLocalEngineIDValue, engId_Hexa, &engineid_length, 0);
		if (owa.l7rc != L7_SUCCESS)
	  {
	     owa.rc = XLIBRC_INVALID_ENGINE_ID;
	     FPOBJ_TRACE_EXIT (bufp, owa);
	     return owa.rc;
		}
   }
   owa.l7rc = usmDbSnmpConfapiSnmpServerLocalEngineIDSet(engId_Hexa, 
                           engineid_length, algorithm);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSNMPGlobalConfig_UseAsDefaultSNMPEngineID
*
* @purpose Get 'UseAsDefaultSNMPEngineID'
 *@description  [UseAsDefaultSNMPEngineID] uses local Engine ID as default SNMP
* engine ID when selected   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGlobalConfig_UseAsDefaultSNMPEngineID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUseAsDefaultSNMPEngineIDValue;

  xLibStr256_t         engineId;
  L7_uint32         len = 0;
  L7_uint32         algorithm;

  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = usmDbSnmpConfapiSnmpServerLocalEngineIDGet(engineId, &len, &algorithm);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUseAsDefaultSNMPEngineIDValue = ((algorithm == L7_SNMP_DEFAULT_ENGINE) ? L7_TRUE : L7_FALSE);


  FPOBJ_TRACE_VALUE (bufp, &objUseAsDefaultSNMPEngineIDValue,
                     sizeof (objUseAsDefaultSNMPEngineIDValue));

  /* return the object value: UseAsDefaultSNMPEngineID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUseAsDefaultSNMPEngineIDValue,
                           sizeof (objUseAsDefaultSNMPEngineIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPGlobalConfig_UseAsDefaultSNMPEngineID
*
* @purpose Set 'UseAsDefaultSNMPEngineID'
 *@description  [UseAsDefaultSNMPEngineID] uses local Engine ID as default SNMP
* engine ID when selected   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGlobalConfig_UseAsDefaultSNMPEngineID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUseAsDefaultSNMPEngineIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UseAsDefaultSNMPEngineID */
  owa.len = sizeof (objUseAsDefaultSNMPEngineIDValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUseAsDefaultSNMPEngineIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUseAsDefaultSNMPEngineIDValue, owa.len);


  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseSNMPGlobalConfig_SNMPTraps
*
* @purpose Get 'SNMPTraps'
 *@description  [SNMPTraps] SNMP traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGlobalConfig_SNMPTraps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSNMPTrapsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSnmpEnableTrapsGet ( &objSNMPTrapsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objSNMPTrapsValue, sizeof (objSNMPTrapsValue));

  /* return the object value: SNMPTraps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSNMPTrapsValue, sizeof (objSNMPTrapsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPGlobalConfig_SNMPTraps
*
* @purpose Set 'SNMPTraps'
 *@description  [SNMPTraps] SNMP traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGlobalConfig_SNMPTraps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSNMPTrapsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SNMPTraps */
  owa.len = sizeof (objSNMPTrapsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSNMPTrapsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSNMPTrapsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSnmpEnableTrapsSet ( objSNMPTrapsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSNMPGlobalConfig_AuthenticationTraps
*
* @purpose Get 'AuthenticationTraps'
 *@description  [AuthenticationTraps] SNMP Authentication  traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGlobalConfig_AuthenticationTraps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationTrapsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSnmpEnableAuthenTrapsGet (L7_UNIT_CURRENT, &objAuthenticationTrapsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationTrapsValue, sizeof (objAuthenticationTrapsValue));

  /* return the object value: AuthenticationTraps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationTrapsValue,
                           sizeof (objAuthenticationTrapsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPGlobalConfig_AuthenticationTraps
*
* @purpose Set 'AuthenticationTraps'
 *@description  [AuthenticationTraps] SNMP Authentication  traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGlobalConfig_AuthenticationTraps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationTrapsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationTraps */
  owa.len = sizeof (objAuthenticationTrapsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthenticationTrapsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationTrapsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSnmpEnableAuthenTrapsSet (L7_UNIT_CURRENT, objAuthenticationTrapsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
