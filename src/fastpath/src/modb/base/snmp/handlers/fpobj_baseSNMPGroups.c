
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSNMPGroups.c
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
#include "_xe_baseSNMPGroups_obj.h"

#include <stdlib.h>
#include <ctype.h>
#include "usmdb_snmp_confapi_api.h"
#include "usmdb_snmp_trap_api.h"


/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_GroupName
*
* @purpose Get 'GroupName'
 *@description  [GroupName] SNMP group Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_GroupName (void *wap, void *bufp)
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
  FPOBJ_CLR_STR256 (objContextPrefixValue);
  FPOBJ_CLR_U32 (objVersionValue);
  FPOBJ_CLR_U32 (objSecurityLevelValue);
  FPOBJ_CLR_STR256 (nextObjGroupNameValue);
  FPOBJ_CLR_STR256 (nextObjContextPrefixValue);
  FPOBJ_CLR_U32 (nextObjVersionValue);
  FPOBJ_CLR_U32 (nextObjSecurityLevelValue);
	
  /* retrieve key: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	
    owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (
                                     nextObjGroupNameValue,
                                     nextObjContextPrefixValue, &nextObjVersionValue,
                                     &nextObjSecurityLevelValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objGroupNameValue, owa.len);
    FPOBJ_CPY_STR256 (nextObjGroupNameValue, objGroupNameValue);
    do
    {
      owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (nextObjGroupNameValue,
                                      nextObjContextPrefixValue, &nextObjVersionValue,
                                      &nextObjSecurityLevelValue);
    }
    while (!(FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
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
* @function fpObjGet_baseSNMPGroups_ContextPrefix
*
* @purpose Get 'ContextPrefix'
 *@description  [ContextPrefix] SNMP Context Prefix.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_ContextPrefix (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objContextPrefixValue;
  xLibStr256_t nextObjContextPrefixValue;

  xLibStr256_t objGroupNameValue;
  xLibStr256_t nextObjGroupNameValue;
  xLibU32_t objVersionValue;
  xLibU32_t nextObjVersionValue;
  xLibU32_t objSecurityLevelValue;
  xLibU32_t nextObjSecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objGroupNameValue);
  FPOBJ_CLR_STR256 (objContextPrefixValue);
  FPOBJ_CLR_U32 (objVersionValue);
  FPOBJ_CLR_U32 (objSecurityLevelValue);
  FPOBJ_CLR_STR256 (nextObjGroupNameValue);
  FPOBJ_CLR_STR256 (nextObjContextPrefixValue);
  FPOBJ_CLR_U32 (nextObjVersionValue);
  FPOBJ_CLR_U32 (nextObjSecurityLevelValue);

	
  /* retrieve key: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objGroupNameValue, owa.len);

  FPOBJ_CPY_STR256 (nextObjGroupNameValue, objGroupNameValue);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (objContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) objContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
 	
    owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (nextObjGroupNameValue,
                                     nextObjContextPrefixValue,
                                     &nextObjVersionValue,
                                     &nextObjSecurityLevelValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objContextPrefixValue, owa.len);
    FPOBJ_CPY_U32 (nextObjVersionValue, objVersionValue);
    FPOBJ_CPY_U32 (nextObjSecurityLevelValue,objSecurityLevelValue);
    FPOBJ_CPY_STR256 (nextObjContextPrefixValue, objContextPrefixValue);
		
    do
    {
      owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (nextObjGroupNameValue,
                                      nextObjContextPrefixValue, 
                                      &nextObjVersionValue,
                                      &nextObjSecurityLevelValue);
    }while ((!(FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)))
            &&(!(FPOBJ_CMP_STR256 (objContextPrefixValue, nextObjContextPrefixValue)))
           && (owa.l7rc == L7_SUCCESS));
  }

  if (((FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue))) ||
      (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjContextPrefixValue, owa.len);

  /* return the object value: ContextPrefix */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjContextPrefixValue,
                           strlen (nextObjContextPrefixValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_Version
*
* @purpose Get 'Version'
 *@description  [Version] SNMP Version.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_Version (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVersionValue;
  xLibU32_t nextObjVersionValue;

  xLibStr256_t objGroupNameValue;
  xLibStr256_t nextObjGroupNameValue;
  xLibStr256_t objContextPrefixValue;
  xLibStr256_t nextObjContextPrefixValue;
  xLibU32_t objSecurityLevelValue;
  xLibU32_t nextObjSecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objGroupNameValue);
  FPOBJ_CLR_STR256 (objContextPrefixValue);
  FPOBJ_CLR_U32 (objVersionValue);
  FPOBJ_CLR_U32 (objSecurityLevelValue);
  FPOBJ_CLR_STR256 (nextObjGroupNameValue);
  FPOBJ_CLR_STR256 (nextObjContextPrefixValue);
  FPOBJ_CLR_U32 (nextObjVersionValue);
  FPOBJ_CLR_U32 (nextObjSecurityLevelValue);

  /* retrieve key: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objGroupNameValue, owa.len);

  /* Copy Group name value here */
  FPOBJ_CPY_STR256 (nextObjGroupNameValue, objGroupNameValue);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (objContextPrefixValue);
  xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) objContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, objContextPrefixValue, owa.len);

  /* Copy Context Prefix value here */
  FPOBJ_CPY_STR256 (nextObjContextPrefixValue, objContextPrefixValue);

  /* retrieve key: Version */
  owa.len = sizeof (objVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & objVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
 		
    owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (
                                     nextObjGroupNameValue,
                                     nextObjContextPrefixValue, &nextObjVersionValue,
                                     &nextObjSecurityLevelValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVersionValue, owa.len);

    FPOBJ_CPY_U32 (nextObjVersionValue, objVersionValue);
    
    do
    {
      FPOBJ_CPY_U32 (nextObjSecurityLevelValue,objSecurityLevelValue);
      FPOBJ_CPY_STR256 (nextObjContextPrefixValue, objContextPrefixValue); 
      owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (
                                      nextObjGroupNameValue,
                                      nextObjContextPrefixValue, &nextObjVersionValue,
                                      &nextObjSecurityLevelValue);
      FPOBJ_CPY_U32 (objSecurityLevelValue,nextObjSecurityLevelValue);
      FPOBJ_CPY_STR256 (objContextPrefixValue, nextObjContextPrefixValue); 
    }while ((!(FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)))
/*         &&(!(FPOBJ_CMP_STR256 (objContextPrefixValue, nextObjContextPrefixValue))) */
           &&((FPOBJ_CMP_U32 (objVersionValue, nextObjVersionValue)))
           && (owa.l7rc == L7_SUCCESS));
  }

  if((FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)) ||
     (owa.l7rc != L7_SUCCESS) )
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVersionValue, owa.len);

  /* return the object value: Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVersionValue, sizeof (nextObjVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_SecurityLevel
*
* @purpose Get 'SecurityLevel'
 *@description  [SecurityLevel] SNMP SecurityLevel.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_SecurityLevel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSecurityLevelValue;
  xLibU32_t nextObjSecurityLevelValue;

  xLibStr256_t objGroupNameValue;
  xLibStr256_t nextObjGroupNameValue;
  xLibStr256_t objContextPrefixValue;
  xLibStr256_t nextObjContextPrefixValue;
  xLibU32_t objVersionValue;
  xLibU32_t nextObjVersionValue;


  FPOBJ_CLR_STR256 (objGroupNameValue);
  FPOBJ_CLR_STR256 (objContextPrefixValue);
  FPOBJ_CLR_U32 (objVersionValue);
  FPOBJ_CLR_U32 (objSecurityLevelValue);
  FPOBJ_CLR_STR256 (nextObjGroupNameValue);
  FPOBJ_CLR_STR256 (nextObjContextPrefixValue);
  FPOBJ_CLR_U32 (nextObjVersionValue);
  FPOBJ_CLR_U32 (nextObjSecurityLevelValue);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupName */
  owa.len = sizeof (objGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) objGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objGroupNameValue, owa.len);

  /* Copy Group name value here */
  FPOBJ_CPY_STR256 (nextObjGroupNameValue, objGroupNameValue);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (objContextPrefixValue);
  xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) objContextPrefixValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, objContextPrefixValue, owa.len);

  /* Copy Group name value here */
  FPOBJ_CPY_STR256 (nextObjGroupNameValue, objGroupNameValue);

  /* retrieve key: Version */
  owa.len = sizeof (objVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & objVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVersionValue, owa.len);

  /* Copy Version value here */
  FPOBJ_CPY_U32 (nextObjVersionValue, objVersionValue);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (objSecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & objSecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (
                                     nextObjGroupNameValue,
                                     nextObjContextPrefixValue, &nextObjVersionValue,
                                     &nextObjSecurityLevelValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSecurityLevelValue, owa.len);
    
    FPOBJ_CPY_U32 (nextObjSecurityLevelValue,objSecurityLevelValue);

    do
    {
      FPOBJ_CPY_U32 (nextObjVersionValue, objVersionValue);
      FPOBJ_CPY_STR256 (nextObjContextPrefixValue, objContextPrefixValue);  
      owa.l7rc = usmDbSnmpConfapiVacmAccessTableNextGet (
                                    nextObjGroupNameValue,
                                    nextObjContextPrefixValue, 
                                    &nextObjVersionValue,
                                    &nextObjSecurityLevelValue);
      FPOBJ_CPY_U32 (objVersionValue, nextObjVersionValue);
      FPOBJ_CPY_STR256 (objContextPrefixValue, nextObjContextPrefixValue);
   
     }while ((!(FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)))
/*           &&(!(FPOBJ_CMP_STR256 (objContextPrefixValue, nextObjContextPrefixValue)))
           &&((FPOBJ_CMP_U32 (objVersionValue, nextObjVersionValue))) */ 
           &&((FPOBJ_CMP_U32 (objSecurityLevelValue, nextObjSecurityLevelValue))) 
           && (owa.l7rc == L7_SUCCESS));
  
  }
 
  if((FPOBJ_CMP_STR256 (objGroupNameValue, nextObjGroupNameValue)) ||
      (owa.l7rc != L7_SUCCESS) )

  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSecurityLevelValue, owa.len);

  /* return the object value: SecurityLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSecurityLevelValue,
                           sizeof (nextObjSecurityLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_ReadViewName
*
* @purpose Get 'ReadViewName'
 *@description  [ReadViewName] SNMP read view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_ReadViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objReadViewNameValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objReadViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);


  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpConfapiVacmAccessReadViewNameGet ( keyGroupNameValue,
                              keyContextPrefixValue,
                              keyVersionValue, keySecurityLevelValue, objReadViewNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objReadViewNameValue, strlen (objReadViewNameValue));

  /* return the object value: ReadViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objReadViewNameValue, strlen (objReadViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseSNMPGroups_ReadViewName
*
* @purpose List 'ReadViewName'
 *@description  [ReadViewName] SNMP read view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSNMPGroups_ReadViewName (void *wap, void *bufp)
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
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ReadViewName,
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
* @function fpObjSet_baseSNMPGroups_ReadViewName
*
* @purpose Set 'ReadViewName'
 *@description  [ReadViewName] SNMP read view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGroups_ReadViewName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objReadViewNameValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objReadViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);

  /* retrieve object: ReadViewName */
  owa.len = sizeof (objReadViewNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objReadViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objReadViewNameValue, owa.len);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSnmpConfapiVacmAccessReadViewNameSet ( keyGroupNameValue,
                              keyContextPrefixValue,
                              keyVersionValue, keySecurityLevelValue, objReadViewNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_WriteViewName
*
* @purpose Get 'WriteViewName'
 *@description  [WriteViewName] SNMP write view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_WriteViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objWriteViewNameValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objWriteViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpConfapiVacmAccessWriteViewNameGet (keyGroupNameValue,
                              keyContextPrefixValue,
                              keyVersionValue, keySecurityLevelValue, objWriteViewNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objWriteViewNameValue, strlen (objWriteViewNameValue));

  /* return the object value: WriteViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objWriteViewNameValue,
                           strlen (objWriteViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseSNMPGroups_WriteViewName
*
* @purpose List 'WriteViewName'
 *@description  [WriteViewName] SNMP write view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSNMPGroups_WriteViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objWriteViewNameValue;
  xLibStr256_t nextObjWriteViewNameValue;
  xLibStr256_t objOIDSubtree;
  xLibStr256_t nextObjOIDSubTreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objWriteViewNameValue);
  FPOBJ_CLR_STR256 (nextObjWriteViewNameValue);
  FPOBJ_CLR_STR256 (objOIDSubtree);
  FPOBJ_CLR_STR256 (nextObjOIDSubTreeValue);
  
  owa.len = sizeof (objWriteViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_WriteViewName,
                          (xLibU8_t *) objWriteViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet ( nextObjWriteViewNameValue, nextObjOIDSubTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objWriteViewNameValue, owa.len);
    FPOBJ_CLR_STR256 (objOIDSubtree);
    FPOBJ_CPY_STR256(nextObjWriteViewNameValue,objWriteViewNameValue);
    do
    {
     FPOBJ_CPY_STR256 (nextObjOIDSubTreeValue,objOIDSubtree);
     owa.l7rc =usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (nextObjWriteViewNameValue, nextObjOIDSubTreeValue);
     FPOBJ_CPY_STR256 (objOIDSubtree,nextObjOIDSubTreeValue);
    }while (!(FPOBJ_CMP_STR256 (objWriteViewNameValue, nextObjWriteViewNameValue)) && (owa.l7rc == L7_SUCCESS));
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjWriteViewNameValue, owa.len);

  /* return the object value: ReadViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjWriteViewNameValue,
                           strlen (nextObjWriteViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSNMPGroups_WriteViewName
*
* @purpose Set 'WriteViewName'
 *@description  [WriteViewName] SNMP write view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGroups_WriteViewName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objWriteViewNameValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objWriteViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);

  /* retrieve object: WriteViewName */
  owa.len = sizeof (objWriteViewNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objWriteViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objWriteViewNameValue, owa.len);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSnmpConfapiVacmAccessWriteViewNameSet ( keyGroupNameValue,
                              keyContextPrefixValue,
                              keyVersionValue, keySecurityLevelValue, objWriteViewNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_NotifyViewName
*
* @purpose Get 'NotifyViewName'
 *@description  [NotifyViewName] SNMP notify view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_NotifyViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objNotifyViewNameValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objNotifyViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpConfapiVacmAccessNotifyViewNameGet ( keyGroupNameValue,
                              keyContextPrefixValue,
                              keyVersionValue, keySecurityLevelValue, objNotifyViewNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objNotifyViewNameValue, strlen (objNotifyViewNameValue));

  /* return the object value: NotifyViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objNotifyViewNameValue,
                           strlen (objNotifyViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseSNMPGroups_NotifyViewName
*
* @purpose List 'NotifyViewName'
 *@description  [NotifyViewName] SNMP notify view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSNMPGroups_NotifyViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objNotifyViewNameValue;
  xLibStr256_t nextObjNotifyViewNameValue;
  xLibStr256_t objOIDSubtree;
  xLibStr256_t nextObjOIDSubTreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objNotifyViewNameValue);
  FPOBJ_CLR_STR256 (nextObjNotifyViewNameValue);
  FPOBJ_CLR_STR256 (objOIDSubtree);
  FPOBJ_CLR_STR256 (nextObjOIDSubTreeValue);
  
  owa.len = sizeof (objNotifyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_NotifyViewName,
                          (xLibU8_t *) objNotifyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet ( nextObjNotifyViewNameValue, nextObjOIDSubTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objNotifyViewNameValue, owa.len);
    FPOBJ_CLR_STR256 (objOIDSubtree);
    FPOBJ_CPY_STR256(nextObjNotifyViewNameValue,objNotifyViewNameValue);
    do
    {
      FPOBJ_CPY_STR256 (nextObjOIDSubTreeValue,objOIDSubtree);
      owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (nextObjNotifyViewNameValue, nextObjOIDSubTreeValue);
      FPOBJ_CPY_STR256 (objOIDSubtree,nextObjOIDSubTreeValue);
    }while (!(FPOBJ_CMP_STR256 (objNotifyViewNameValue, nextObjNotifyViewNameValue)) && (owa.l7rc == L7_SUCCESS));
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjNotifyViewNameValue, owa.len);

  /* return the object value: ReadViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjNotifyViewNameValue,
                           strlen (nextObjNotifyViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
	
}

/*******************************************************************************
* @function fpObjSet_baseSNMPGroups_NotifyViewName
*
* @purpose Set 'NotifyViewName'
 *@description  [NotifyViewName] SNMP notify view name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGroups_NotifyViewName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objNotifyViewNameValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objNotifyViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);

 /* retrieve object: NotifyViewName */
  owa.len = sizeof (objNotifyViewNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objNotifyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objNotifyViewNameValue, owa.len);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbSnmpConfapiVacmAccessNotifyViewNameSet ( keyGroupNameValue,
                              keyContextPrefixValue,
                              keyVersionValue, keySecurityLevelValue, objNotifyViewNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPGroups_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Groups.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPGroups_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);

   FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPGroups_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Groups.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPGroups_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyGroupNameValue;
  xLibStr256_t keyContextPrefixValue;
  xLibU32_t keyVersionValue;
  xLibU32_t keySecurityLevelValue;
  xLibStr256_t objReadViewNameValue;
  l7_snmpStorageType_t storageType;

  FPOBJ_CLR_STR256 (keyGroupNameValue);
  FPOBJ_CLR_STR256 (keyContextPrefixValue);
  FPOBJ_CLR_STR256 (objReadViewNameValue);
  FPOBJ_CLR_U32 (keyVersionValue);
  FPOBJ_CLR_U32 (keySecurityLevelValue);
  memset(&storageType,0x00,sizeof(storageType));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: GroupName */
  owa.len = sizeof (keyGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_GroupName,
                          (xLibU8_t *) keyGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyGroupNameValue, owa.len);

  /* retrieve key: ContextPrefix */
  owa.len = sizeof (keyContextPrefixValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_ContextPrefix,
                          (xLibU8_t *) keyContextPrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyContextPrefixValue, owa.len);

  /* retrieve key: Version */
  owa.len = sizeof (keyVersionValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_Version,
                          (xLibU8_t *) & keyVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVersionValue, owa.len);

  /* retrieve key: SecurityLevel */
  owa.len = sizeof (keySecurityLevelValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPGroups_SecurityLevel,
                          (xLibU8_t *) & keySecurityLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySecurityLevelValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Check if the group already exits */
   owa.l7rc = usmDbSnmpConfapiVacmAccessTableCheckValid(keyGroupNameValue, keyContextPrefixValue,
                                  keyVersionValue, keySecurityLevelValue);
   if(owa.l7rc == L7_SUCCESS)
   {
     owa.rc = XLIBRC_ACCESS_CONTROL_GROUP_NAME_EXIST;
     owa.l7rc = L7_FAILURE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   /* If not then create the group */
   owa.l7rc = usmDbSnmpConfapiVacmAccessStatusSet(keyGroupNameValue, keyContextPrefixValue,
                                    keyVersionValue, keySecurityLevelValue,
                                          snmpRowStatus_createAndWait);
   if(owa.l7rc != L7_SUCCESS)
   {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

    owa.l7rc= usmDbSnmpConfapiVacmAccessContextMatchSet(keyGroupNameValue,
                                     keyContextPrefixValue,keyVersionValue, keySecurityLevelValue,
                                       vacmAccessContextMatch_prefix);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbSnmpConfapiVacmAccessStatusSet(keyGroupNameValue,
                                     keyContextPrefixValue,keyVersionValue, keySecurityLevelValue,
                                     snmpRowStatus_active);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    if ((usmDbSnmpConfapiVacmAccessStorageTypeGet(keyGroupNameValue,
                                     keyContextPrefixValue,
                                     keyVersionValue, keySecurityLevelValue,
                                          &storageType) == L7_SUCCESS) &&
      (storageType == snmpStorageType_readOnly))
    {
      owa.rc = XLIBRC_ACCESS_CONTROL_GROUP_DEFAULT_REMOVE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      owa.l7rc  = usmDbSnmpConfapiVacmAccessStatusSet (keyGroupNameValue,
                                     keyContextPrefixValue,
                                     keyVersionValue, keySecurityLevelValue,
                         snmpRowStatus_destroy);
      if(owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_ACCESS_CONTROL_DELETE_GROUP_NAME;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
  }	
  else
  {
    owa.l7rc = L7_FAILURE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
