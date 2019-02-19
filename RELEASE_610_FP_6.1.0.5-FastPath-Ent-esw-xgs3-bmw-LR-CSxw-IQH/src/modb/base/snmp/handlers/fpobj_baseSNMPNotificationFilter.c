
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baseSNMPNotificationFilter.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  08 March 2009, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseSNMPNotificationFilter_obj.h"

#include "snmp_exports.h"
#include <ctype.h>
#include "usmdb_snmp_confapi_api.h"
#include "usmdb_util_api.h"

L7_RC_t fpObjUtil_ConvertSNMPOIDPrefix(L7_char8 *input, L7_char8 *output)
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

L7_RC_t fpObjUtil_ValidateSNMPASNString(L7_char8 *input, L7_uchar8 *mask, L7_uint32 *mask_len)
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


/*******************************************************************************
* @function fpObjGet_baseSNMPNotificationFilter_FilterName
*
* @purpose Get 'FilterName'
 *@description  [FilterName] SNMP  Community Name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPNotificationFilter_FilterName (void *wap, void *bufp)
{


  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objFilterNameValue;
  xLibStr256_t nextObjFilterNameValue;
  xLibStr256_t objOIDTreeValue;
  xLibStr256_t nextObjOIDTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objFilterNameValue);
  FPOBJ_CLR_STR256 (objOIDTreeValue);
  FPOBJ_CLR_STR256 (nextObjFilterNameValue);
  FPOBJ_CLR_STR256 (nextObjOIDTreeValue);
  
  /* retrieve key: ViewName */
  owa.len = sizeof (objFilterNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_FilterName,
                          (xLibU8_t *) objFilterNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	
    owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterTableNextGet (nextObjFilterNameValue,
                                     nextObjOIDTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objFilterNameValue, owa.len);
    FPOBJ_CPY_STR256 (nextObjFilterNameValue, objFilterNameValue);
    do
    {
      FPOBJ_CPY_STR256 (nextObjOIDTreeValue, objOIDTreeValue);
		
      owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterTableNextGet (nextObjFilterNameValue,
                                      nextObjOIDTreeValue);
      FPOBJ_CPY_STR256 (objOIDTreeValue, nextObjOIDTreeValue);
    }
    while(!(FPOBJ_CMP_STR256 (objFilterNameValue, nextObjFilterNameValue)) && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjFilterNameValue, owa.len);
  /* return the object value: ViewName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjFilterNameValue, strlen (nextObjFilterNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPNotificationFilter_OIDTree
*
* @purpose Get 'OIDTree'
 *@description  [OIDTree] SNMP  Community management station   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPNotificationFilter_OIDTree (void *wap, void *bufp)
{


  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objOIDTreeValue;
  xLibStr256_t nextObjOIDTreeValue;
  xLibStr256_t objFilterNameValue;
  xLibStr256_t nextObjViewNameValue;
  xLibStr256_t subTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (objOIDTreeValue);
  FPOBJ_CLR_STR256 (nextObjOIDTreeValue);
  FPOBJ_CLR_STR256 (objFilterNameValue);
  FPOBJ_CLR_STR256 (nextObjViewNameValue);
  FPOBJ_CLR_STR256 (subTreeValue);

  /* retrieve key: ViewName */
  owa.len = sizeof (objFilterNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_FilterName,
                          (xLibU8_t *) objFilterNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objFilterNameValue, owa.len);

  /* retrieve key: OIDSubTree */
  owa.len = sizeof (objOIDTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_OIDTree,
                          (xLibU8_t *) objOIDTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_STR256 (objOIDTreeValue);
  	FPOBJ_CLR_STR256 (nextObjOIDTreeValue);
    FPOBJ_CPY_STR256 (nextObjViewNameValue, objFilterNameValue);
    owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterTableNextGet (nextObjViewNameValue,
                                     nextObjOIDTreeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objOIDTreeValue, owa.len);
    FPOBJ_CPY_STR256 (nextObjViewNameValue, objFilterNameValue);
   	FPOBJ_CPY_STR256 (nextObjOIDTreeValue, objOIDTreeValue);
	
    owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterTableNextGet (
                                    nextObjViewNameValue,
                                    nextObjOIDTreeValue);

  }

  if (((FPOBJ_CMP_STR256 (objFilterNameValue, nextObjViewNameValue)) ||
       !(FPOBJ_CMP_STR256 (objOIDTreeValue,nextObjOIDTreeValue)))||
       (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjOIDTreeValue, owa.len);

  FPOBJ_CLR_STR256 (subTreeValue);

  if(fpObjUtil_ConvertSNMPOIDPrefix(nextObjOIDTreeValue,subTreeValue) != L7_SUCCESS)
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
* @function fpObjGet_baseSNMPNotificationFilter_Type
*
* @purpose Get 'Type'
 *@description  [Type] SNMP  Community management station   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPNotificationFilter_Type (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTypeValue;

  xLibStr256_t keyFilterNameValue;
  xLibStr256_t keyOIDTreeValue;
  xLibStr256_t subTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (keyFilterNameValue);
  FPOBJ_CLR_STR256 (keyOIDTreeValue);
  FPOBJ_CLR_STR256 (subTreeValue);

  /* retrieve key: FilterName */
  owa.len = sizeof (keyFilterNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_FilterName,
                          (xLibU8_t *) keyFilterNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFilterNameValue, owa.len);

  /* retrieve key: OIDTree */
  owa.len = sizeof (keyOIDTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_OIDTree,
                          (xLibU8_t *) keyOIDTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(fpObjUtil_ConvertSNMPOIDPrefix(keyOIDTreeValue,subTreeValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDTreeValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterTypeGet(keyFilterNameValue, subTreeValue,
                         &objTypeValue);
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
* @function fpObjSet_baseSNMPNotificationFilter_Type
*
* @purpose Set 'Type'
 *@description  [Type] SNMP  Community management station   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPNotificationFilter_Type (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTypeValue;

  xLibStr256_t keyFilterNameValue;
  xLibStr256_t keyOIDTreeValue;
  xLibStr256_t subTreeValue;


  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (keyFilterNameValue);
  FPOBJ_CLR_STR256 (keyOIDTreeValue);
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

  /* retrieve key: FilterName */
  owa.len = sizeof (keyFilterNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_FilterName,
                          (xLibU8_t *) keyFilterNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFilterNameValue, owa.len);

  /* retrieve key: OIDTree */
  owa.len = sizeof (keyOIDTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_OIDTree,
                          (xLibU8_t *) keyOIDTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDTreeValue, owa.len);

  FPOBJ_CLR_STR256 (subTreeValue);
  if(fpObjUtil_ConvertSNMPOIDPrefix(keyOIDTreeValue,subTreeValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterTypeSet ( keyFilterNameValue, subTreeValue, objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSNMPNotificationFilter_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Notification Filters.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSNMPNotificationFilter_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyFilterNameValue;
  xLibStr256_t keyOIDTreeValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  FPOBJ_CLR_STR256 (keyFilterNameValue);
  FPOBJ_CLR_STR256 (keyOIDTreeValue);

  /* retrieve key: FilterName */
  owa.len = sizeof (keyFilterNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_FilterName,
                          (xLibU8_t *) keyFilterNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFilterNameValue, owa.len);

  /* retrieve key: OIDTree */
  owa.len = sizeof (keyOIDTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_OIDTree,
                          (xLibU8_t *) keyOIDTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDTreeValue, owa.len);

  /* get the value from application */
  objRowStatusValue = L7_ROW_STATUS_ACTIVE;
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));
	
  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSNMPNotificationFilter_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] Create or Delete the SNMP Notification Filters.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSNMPNotificationFilter_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue;

  xLibStr256_t keyFilterNameValue;
  xLibStr256_t keyOIDTreeValue;

  xLibStr256_t subtree;
  xLibStr256_t OidString;
  xLibStr256_t vacmViewTreeFamilyMask;
  L7_uint32 vacmViewTreeFamilyMask_len;  

  FPOBJ_TRACE_ENTER (bufp);

  FPOBJ_CLR_STR256 (subtree);
  FPOBJ_CLR_STR256 (OidString);
  FPOBJ_CLR_STR256 (vacmViewTreeFamilyMask);


  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: FilterName */
  owa.len = sizeof (keyFilterNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_FilterName,
                          (xLibU8_t *) keyFilterNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyFilterNameValue, owa.len);

  /* retrieve key: OIDTree */
  owa.len = sizeof (keyOIDTreeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSNMPNotificationFilter_OIDTree,
                          (xLibU8_t *) keyOIDTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyOIDTreeValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  
  osapiStrncpySafe (subtree, keyOIDTreeValue, sizeof (subtree));
  if (fpObjUtil_ConvertSNMPOIDPrefix (subtree, OidString) == L7_SUCCESS)
  {
    if (fpObjUtil_ValidateSNMPASNString(OidString, vacmViewTreeFamilyMask, 
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
      if(usmDbSnmpConfapiSnmpNotifyFilterTableCheckValid(keyFilterNameValue,
            OidString) == L7_SUCCESS)
      {
	      owa.rc = XLIBRC_NOTIFICATIONFILTER_ALREADY_EXISTS;
	      FPOBJ_TRACE_EXIT (bufp, owa);
	      return owa.rc;
      }

      if(usmDbSnmpConfapiSnmpNotifyFilterRowStatusSet(keyFilterNameValue,OidString,
            snmpRowStatus_createAndWait) != L7_SUCCESS)
      {
		    owa.rc = XLIBRC_FAILURE;
		    FPOBJ_TRACE_EXIT (bufp, owa);
		    return owa.rc;
      }
      else
      {
        if (vacmViewTreeFamilyMask_len > 0)
        {
          if (usmDbSnmpConfapiSnmpNotifyFilterMaskSet (keyFilterNameValue,OidString, 
                vacmViewTreeFamilyMask, vacmViewTreeFamilyMask_len) != L7_SUCCESS)
          {
           usmDbSnmpConfapiSnmpNotifyFilterRowStatusSet(keyFilterNameValue,OidString,
                         snmpRowStatus_active);
		    owa.rc = XLIBRC_FAILURE;
		    FPOBJ_TRACE_EXIT (bufp, owa);
		    return owa.rc;
          }
        }
      }

      if(usmDbSnmpConfapiSnmpNotifyFilterTypeSet(keyFilterNameValue,OidString, 
            snmpNotifyFilterType_excluded) != L7_SUCCESS)
      {
           usmDbSnmpConfapiSnmpNotifyFilterRowStatusSet(keyFilterNameValue,OidString,
                         snmpRowStatus_active);
		    owa.rc = XLIBRC_FAILURE;
		    FPOBJ_TRACE_EXIT (bufp, owa);
		    return owa.rc;
      }
      if(usmDbSnmpConfapiSnmpNotifyFilterRowStatusSet(keyFilterNameValue,OidString,
            snmpRowStatus_active) != L7_SUCCESS)
      {
		    owa.rc = XLIBRC_FAILURE;
		    FPOBJ_TRACE_EXIT (bufp, owa);
		    return owa.rc;
      }

  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
	      owa.l7rc = usmDbSnmpConfapiSnmpNotifyFilterRowStatusSet(keyFilterNameValue,OidString, snmpRowStatus_destroy);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
