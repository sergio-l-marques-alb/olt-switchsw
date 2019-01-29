
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSNMPViews.c
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
#include "_xe_baseSNMPViews_obj.h"

#include <stdlib.h>
#include <ctype.h>
#include "usmdb_snmp_confapi_api.h"
#include "usmdb_snmp_trap_api.h"


L7_RC_t fpObjUtil_ConvertOIDPrefix(L7_char8 *input, L7_char8 *output)
{
  L7_uint32 i, len;

  len = strlen(input);

  /* look for first '.' */
  for (i = 0; i < len; i++)
  {
    if (input[i] == '.')
    {
      input[i] = '\0';
      break;
    }
  }

  /* convert prefix into numeric oid form */
  if (usmDbSnmpConfapiOIDFromObject(input, output) == L7_SUCCESS)
  {
    /* append rest of oid buffer */
    if (i < len)
    {
      sprintf(output + strlen(output), ".%s", input + (i + 1));
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t fpObjUtil_ValidateASNString(L7_char8 *input, L7_uchar8 *mask, L7_uint32 *mask_len)
{
  L7_uint32 i;
  L7_uint32 oid_index = 0;

  *mask_len = 0;

  for (i = 0; i < strlen(input); i++)
  {
    if (input[i] == '.')
    {
      oid_index++;
    }
    else if (input[i] == '*')
    {
      input[i] = '0';

      /* turn on wildcard bit */
      mask[(oid_index/8)] |= 0x80 >> (oid_index%8);

      /* we had a wildcard bit, set the length */
      *mask_len = (oid_index/8) + 1;
    }
    else if (!isdigit((L7_uchar8)input[i]))
    {
      return L7_FAILURE;
    }
  }

  /* invert the mask */
  for (i = 0; i < *mask_len; i++)
  {
    mask[i] = ~mask[i];
  }

  return L7_SUCCESS;
}

L7_RC_t fpObjUtil_ConvertOIDToPrefix(L7_char8 *input, L7_uchar8 *mask, L7_uint32 mask_len, L7_char8 *output)
{
  
  L7_uint32 tail_len, oid_index, len, i;
  L7_char8 temp_input[L7_CLI_MAX_STRING_LENGTH];

  /* convert oid into prefix form */
  if (usmDbSnmpConfapiObjectFromOID(input, output) == L7_SUCCESS)
  {
    len = strlen(output);
    tail_len = 0;
    /* look for first '.' */
    for (i = 0; i < len; i++)
    {
      if (output[i] == '.')
      {
        tail_len = strlen(output + i);
        output[i] = '\0';
        break;
      }
    }

    osapiStrncpySafe(temp_input, input, sizeof(temp_input));
    len = strlen(temp_input);
    oid_index = 0;
    /* convert 0's to asterisk's */
    for (i = 0; i < strlen(temp_input); i++)
    {
      if (temp_input[i] == '.')
      {
        oid_index++;
      }
      else if ((mask_len > (oid_index/8)) && ((~(mask[oid_index/8])) & (0x80 >> (oid_index%8))))
      {
        temp_input[i] = '*';
      }
    }

    /* append rest of oid buffer */
    if (tail_len)
    {
      strcpy(output + strlen(output), temp_input + (strlen(temp_input) - tail_len));
    }

    return L7_SUCCESS;
  }
  return L7_FAILURE;

}

L7_RC_t fpObjUtil_ConvertOIDMastkToPrefix(L7_char8 *vacmViewTreeFamilyViewName,L7_char8 *vacmViewTreeFamilySubtree,
                                          L7_char8 *OidString)
{

  xLibU32_t vacmViewTreeFamilyMask_len;
  xLibStr256_t vacmViewTreeFamilyMask;

  FPOBJ_CLR_STR256 (vacmViewTreeFamilyMask);

  vacmViewTreeFamilyMask_len =  L7_NULL;
    
  if((vacmViewTreeFamilyViewName == L7_NULL ) ||(vacmViewTreeFamilySubtree == L7_NULL)||
     (OidString == L7_NULL ))
  { 
    return L7_FAILURE;
  }

  if ((usmDbSnmpConfapiVacmViewTreeFamilyMaskGet(vacmViewTreeFamilyViewName,
                                                     vacmViewTreeFamilySubtree,
                                                     vacmViewTreeFamilyMask,
                                                     &vacmViewTreeFamilyMask_len)== L7_SUCCESS) &&
     (fpObjUtil_ConvertOIDToPrefix(vacmViewTreeFamilySubtree,vacmViewTreeFamilyMask,
                                   vacmViewTreeFamilyMask_len,OidString) == L7_SUCCESS)) 
      
  {
    return L7_SUCCESS;
  }
  else if (usmDbSnmpConfapiObjectFromOID(vacmViewTreeFamilySubtree,
                         OidString) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    osapiStrncpy(OidString,vacmViewTreeFamilySubtree,strlen(vacmViewTreeFamilySubtree)); 
    return L7_SUCCESS;
  }
}

/*******************************************************************************
* @function fpObjGet_baseSNMPViews_ViewName
*
* @purpose Get 'ViewName'
 *@description  [ViewName] SNMP View Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPViews_ViewName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objViewNameValue;
  xLibStr256_t nextObjViewNameValue;
  xLibStr256_t objOIDSubTreeValue;
  xLibStr256_t nextObjOIDSubTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objViewNameValue);
  FPOBJ_CLR_STR256 (objOIDSubTreeValue);
  FPOBJ_CLR_STR256 (nextObjViewNameValue);
  FPOBJ_CLR_STR256 (nextObjOIDSubTreeValue);
  
  /* retrieve key: ViewName */
  owa.len = sizeof (objViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) objViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	
    owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (nextObjViewNameValue,
                                     nextObjOIDSubTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objViewNameValue, owa.len);
    FPOBJ_CPY_STR256 (nextObjViewNameValue, objViewNameValue);
    do
    {
      FPOBJ_CPY_STR256 (nextObjOIDSubTreeValue, objOIDSubTreeValue);
		
      owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (nextObjViewNameValue,
                                      nextObjOIDSubTreeValue);
      FPOBJ_CPY_STR256 (objOIDSubTreeValue, nextObjOIDSubTreeValue);
    }
    while(!(FPOBJ_CMP_STR256 (objViewNameValue, nextObjViewNameValue)) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjViewNameValue, owa.len);
  /* return the object value: ViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjViewNameValue, strlen (nextObjViewNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPViews_OIDSubTree
*
* @purpose Get 'OIDSubTree'
 *@description  [OIDSubTree] SNMP OID SubTree.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPViews_OIDSubTree (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objOIDSubTreeValue;
  xLibStr256_t nextObjOIDSubTreeValue;
  xLibStr256_t objViewNameValue;
  xLibStr256_t nextObjViewNameValue;
  xLibStr256_t subTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objOIDSubTreeValue);
  FPOBJ_CLR_STR256 (nextObjOIDSubTreeValue);
  FPOBJ_CLR_STR256 (objViewNameValue);
  FPOBJ_CLR_STR256 (nextObjViewNameValue);
  FPOBJ_CLR_STR256 (subTreeValue);

  /* retrieve key: ViewName */
  owa.len = sizeof (objViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) objViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objViewNameValue, owa.len);

  /* retrieve key: OIDSubTree */
  owa.len = sizeof (objOIDSubTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_OIDSubTree,
                          (xLibU8_t *) objOIDSubTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_STR256 (objOIDSubTreeValue);
  	FPOBJ_CLR_STR256 (nextObjOIDSubTreeValue);
    FPOBJ_CPY_STR256 (nextObjViewNameValue, objViewNameValue);
    owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (nextObjViewNameValue,
                                     nextObjOIDSubTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objOIDSubTreeValue, owa.len);
    FPOBJ_CPY_STR256 (nextObjViewNameValue, objViewNameValue);
   	FPOBJ_CPY_STR256 (nextObjOIDSubTreeValue, objOIDSubTreeValue);
	
    owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet (
                                    nextObjViewNameValue,
                                    nextObjOIDSubTreeValue);

  }

  if (((FPOBJ_CMP_STR256 (objViewNameValue, nextObjViewNameValue)) ||
       !(FPOBJ_CMP_STR256 (objOIDSubTreeValue,nextObjOIDSubTreeValue)))||
       (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjOIDSubTreeValue, owa.len);

  FPOBJ_CLR_STR256 (subTreeValue);

  if(fpObjUtil_ConvertOIDPrefix(nextObjOIDSubTreeValue,subTreeValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: OIDSubTree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) subTreeValue,
                           strlen (subTreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPViews_Type
*
* @purpose Get 'Type'
 *@description  [Type] SNMP view Type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPViews_Type (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTypeValue;

  xLibStr256_t keyViewNameValue;
  xLibStr256_t keyOIDSubTreeValue;
  xLibStr256_t subTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyViewNameValue);
  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);
  FPOBJ_CLR_STR256 (subTreeValue);

  /* retrieve key: ViewName */
  owa.len = sizeof (keyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) keyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyViewNameValue, owa.len);

  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);
  /* retrieve key: OIDSubTree */
  owa.len = sizeof (keyOIDSubTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_OIDSubTree,
                          (xLibU8_t *) keyOIDSubTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_CLR_STR256 (subTreeValue);
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDSubTreeValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTypeGet ( keyViewNameValue, keyOIDSubTreeValue, &objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, sizeof (objTypeValue));
  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeValue, sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPViews_Type
*
* @purpose Set 'Type'
 *@description  [Type] SNMP view Type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPViews_Type (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTypeValue;

  xLibStr256_t keyViewNameValue;
  xLibStr256_t keyOIDSubTreeValue;
  xLibStr256_t subTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyViewNameValue);
  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);
  FPOBJ_CLR_STR256 (subTreeValue);

  /* retrieve object: Type */
  owa.len = sizeof (objTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, owa.len);

  FPOBJ_CLR_STR256 (keyViewNameValue);
  /* retrieve key: ViewName */
  owa.len = sizeof (keyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) keyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyViewNameValue, owa.len);

  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);
  /* retrieve key: OIDSubTree */
  owa.len = sizeof (keyOIDSubTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_OIDSubTree,
                          (xLibU8_t *) keyOIDSubTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDSubTreeValue, owa.len);

  FPOBJ_CLR_STR256 (subTreeValue);
  if(fpObjUtil_ConvertOIDPrefix(keyOIDSubTreeValue,subTreeValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   /* set the value in application */
  owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTypeSet ( keyViewNameValue, subTreeValue, objTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPViews_OIDSubTreePrefix
*
* @purpose Get 'OIDSubTreePrefix'
 *@description  [Type] SNMP view OIDSubTreePrefix.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPViews_OIDSubTreePrefix (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t keyViewNameValue;
  xLibStr256_t keyOIDSubTreeValue;
  xLibStr256_t subTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyViewNameValue);
  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);
  FPOBJ_CLR_STR256 (subTreeValue);

  /* retrieve key: ViewName */
  owa.len = sizeof (keyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) keyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyViewNameValue, owa.len);


  /* retrieve key: OIDSubTree */
  owa.len = sizeof (keyOIDSubTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_OIDSubTree,
                          (xLibU8_t *) keyOIDSubTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDSubTreeValue, owa.len);

  /* get the value from application */
  owa.l7rc = fpObjUtil_ConvertOIDMastkToPrefix ( keyViewNameValue, keyOIDSubTreeValue, subTreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, subTreeValue, strlen (subTreeValue));
  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) subTreeValue, strlen (subTreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseSNMPViews_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP view.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPViews_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyViewNameValue;
  xLibStr256_t keyOIDSubTreeValue;

  FPOBJ_TRACE_ENTER (bufp);
 
  FPOBJ_CLR_STR256 (keyViewNameValue);
  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);

  /* retrieve key: ViewName */
  owa.len = sizeof (keyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) keyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyViewNameValue, owa.len);

  /* retrieve key: OIDSubTree */
  owa.len = sizeof (keyOIDSubTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_OIDSubTree,
                          (xLibU8_t *) keyOIDSubTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDSubTreeValue, owa.len);


  objRowStatusValue = L7_ROW_STATUS_ACTIVE;
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPViews_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP view.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPViews_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyViewNameValue;
  xLibStr256_t keyOIDSubTreeValue;
  xLibU32_t keyTypeValue;
  xLibStr256_t subtree;
  xLibStr256_t OidString;
  xLibStr256_t vacmViewTreeFamilyMask={};
  L7_uint32 vacmViewTreeFamilyMask_len;  

  l7_snmpStorageType_t storageType;

  vacmViewTreeFamilyMask_len = 0;
	
  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyViewNameValue);
  FPOBJ_CLR_STR256 (keyOIDSubTreeValue);
  FPOBJ_CLR_STR256 (subtree);
  FPOBJ_CLR_STR256 (OidString);
  memset(&storageType,0x00,sizeof(storageType));  

  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: ViewName */
  owa.len = sizeof (keyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) keyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyViewNameValue, owa.len);

  /* retrieve key: OIDSubTree */
  owa.len = sizeof (keyOIDSubTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_OIDSubTree,
                          (xLibU8_t *) keyOIDSubTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDSubTreeValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  
  osapiStrncpySafe (subtree, keyOIDSubTreeValue, sizeof (subtree));
  if (fpObjUtil_ConvertOIDPrefix (subtree, OidString) == L7_SUCCESS)
  {
    if (fpObjUtil_ValidateASNString(OidString, vacmViewTreeFamilyMask, 
                           &vacmViewTreeFamilyMask_len) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_INVALID_OIDSTRING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.rc = XLIBRC_INVALID_OIDSTRING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
	 if(usmDbSnmpConfapiVacmViewTreeFamilyTableCheckValid(keyViewNameValue, 
                                                    OidString)==L7_SUCCESS)
    {
        owa.rc = XLIBRC_VIEW_SNMP_SERVER_CREATE_ROW;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    if(usmDbSnmpConfapiVacmViewTreeFamilyStatusSet(keyViewNameValue, OidString, 
	                            snmpRowStatus_createAndWait)!=L7_SUCCESS)
    {
         owa.rc = XLIBRC_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
    }
    if (vacmViewTreeFamilyMask_len > 0)
    {
        owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyMaskSet (keyViewNameValue, OidString, 
	                            vacmViewTreeFamilyMask, vacmViewTreeFamilyMask_len);
    }
    if(owa.l7rc == L7_SUCCESS)
    {
      /* retrieve key: OIDSubTree */
      owa.len = sizeof (keyTypeValue);
      owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_Type,
                             (xLibU8_t *) &keyTypeValue, &owa.len);
      if (owa.rc != XLIBRC_SUCCESS)
      {
           owa.rc = XLIBRC_FILTER_MISSING;
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
      }
    }
    if(owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyTypeSet(keyViewNameValue,
                            OidString, keyTypeValue ) ;
      if(owa.l7rc == L7_SUCCESS)
      {
        owa.l7rc = usmDbSnmpConfapiVacmViewTreeFamilyStatusSet(keyViewNameValue,OidString,
	                            snmpRowStatus_active);
      }
    }
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    if ((usmDbSnmpConfapiVacmViewTreeFamilyStorageTypeGet(keyViewNameValue,
            OidString,
            &storageType) == L7_SUCCESS) &&
        (storageType == 5 /* snmpStorageType_readOnly */))
    {
        owa.rc = XLIBRC_VIEW_SNMP_SERVER_DEFAULT_REMOVE;
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
    }

    /* remove entry */
    if (usmDbSnmpConfapiVacmViewTreeFamilyStatusSet(keyViewNameValue,
			          OidString,snmpRowStatus_destroy) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseSNMPViews_DeleteView
*
* @purpose Set 'DeleteView'
 *@description  [DeleteView] Delete the SNMP view.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPViews_DeleteView (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyViewNameValue;
  xLibStr256_t strViewName; 
  xLibStr256_t subtree;
  l7_snmpStorageType_t storageType;
	
  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyViewNameValue);
  FPOBJ_CLR_STR256 (strViewName);
  FPOBJ_CLR_STR256 (subtree);
  memset(&storageType,0x00,sizeof(storageType));  

  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: ViewName */
  owa.len = sizeof (keyViewNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPViews_ViewName,
                          (xLibU8_t *) keyViewNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyViewNameValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objRowStatusValue == L7_ENABLE)
  {
	  while (usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet(strViewName,subtree) == L7_SUCCESS)
    {
      if (!(FPOBJ_CMP_STR256 (strViewName, keyViewNameValue)))
      {
        /* don't delete default entries */
        if ((usmDbSnmpConfapiVacmViewTreeFamilyStorageTypeGet(strViewName,
                subtree,
                &storageType) == L7_SUCCESS) &&
            (storageType == 5 /* snmpStorageType_readOnly */))
        {
           owa.rc = XLIBRC_VIEW_SNMP_SERVER_DEFAULT_REMOVE;
  			   FPOBJ_TRACE_EXIT (bufp, owa);
    	     return owa.rc;
        }
        /* remove entry */
        if (usmDbSnmpConfapiVacmViewTreeFamilyStatusSet(strViewName,subtree,snmpRowStatus_destroy) != L7_SUCCESS)
        {
           owa.rc = XLIBRC_FAILURE;
			     FPOBJ_TRACE_EXIT (bufp, owa);
			     return owa.rc;
        }
      }
    }
  }
 return owa.rc;
}

