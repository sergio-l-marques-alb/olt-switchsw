
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingPbvlanGroupConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  25 May 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingPbvlanGroupConfig_obj.h"
#include "usmdb_pbvlan_api.h"
#include "usmdb_util_api.h"
#include "pbvlan_exports.h"
#include <ctype.h>


extern L7_RC_t usmDbConvertIntfListToMask(L7_uint32 *list, L7_uint32 listNum, L7_INTF_MASK_t *mask);
extern L7_RC_t usmDbPbVlanGroupProtocolGetExact(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 proto);

extern L7_BOOL cliConvertEtypeCustomValue(const L7_uchar8 *buf, L7_uint32 *val);

void  fpobjUtil_ProtoStrConvertToLowerCase(L7_char8 *buf)
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
/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupCreate
*
* @purpose Get 'GroupCreate'
*
* @description [GroupCreate]: Set to non-empty string to create a protocol-based
*              VLAN group 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupCreate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupCreateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
#if 0
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, objGroupCreateValue);
#else
  owa.l7rc = L7_FAILURE;
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupCreateValue, strlen (objGroupCreateValue));

  /* return the object value: GroupCreate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupCreateValue,
                           strlen (objGroupCreateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupCreate
*
* @purpose Set 'GroupCreate'
 *@description  [GroupCreate] Set to non-empty string to create a protocol-based
* VLAN group   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupCreate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupCreateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupCreate */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objGroupCreateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupCreateValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (usmDbStringAlphaNumericCheck(objGroupCreateValue) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPbVlanGroupNameCreate(L7_UNIT_CURRENT, objGroupCreateValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_UnConfigGroupId
*
* @purpose Get 'UnConfigGroupId'
 *@description  [UnConfigGroupId] Non configured Group identifiers of the
* protocol-based VLAN entry
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_UnConfigGroupId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUnConfigGroupIdValue;
  xLibU32_t nextObjUnConfigGroupIdValue;
  L7_BOOL flag = L7_TRUE;
  L7_uint32 i;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnConfigGroupId */
  owa.len = sizeof (objUnConfigGroupIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_UnConfigGroupId,
                          (xLibU8_t *) & objUnConfigGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    for(i=L7_PBVLAN_MIN_NUM_GROUPS; i<= L7_PBVLAN_MAX_NUM_GROUPS; i++)
    {
       if(usmDbPbVlanGroupGetExact(L7_UNIT_CURRENT,i)== L7_FAILURE)
       {
         nextObjUnConfigGroupIdValue =i;
         flag = L7_FALSE;
         break;
       }
    }

    if(flag == L7_TRUE)
    {
      owa.l7rc = L7_FAILURE;
    }

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objUnConfigGroupIdValue, owa.len);
    
    for(i=objUnConfigGroupIdValue + 1; i<= L7_PBVLAN_MAX_NUM_GROUPS; i++)
    {
       if(usmDbPbVlanGroupGetExact(L7_UNIT_CURRENT,i)== L7_FAILURE)
       {
         
         nextObjUnConfigGroupIdValue =i;
         flag = L7_FALSE;
         break;
       }
    }

    if(flag == L7_TRUE)
    {
      owa.l7rc = L7_FAILURE;
    }
    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUnConfigGroupIdValue, owa.len);

  /* return the object value: UnConfigGroupId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUnConfigGroupIdValue,
                           sizeof (nextObjUnConfigGroupIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupId
*
* @purpose Get 'GroupId'
 *@description  [GroupId] Group identifier of the protocol-based VLAN entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupIdValue;
  xLibU32_t nextObjGroupIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                          (xLibU8_t *) & objGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbPbVlanGroupGetFirst(L7_UNIT_CURRENT, &nextObjGroupIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGroupIdValue, owa.len);
    owa.l7rc = usmDbPbVlanGroupGetNext(L7_UNIT_CURRENT, objGroupIdValue, &nextObjGroupIdValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjGroupIdValue, owa.len);

  /* return the object value: GroupId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjGroupIdValue, sizeof (objGroupIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupName
*
* @purpose Get 'GroupName'
 *@description  [GroupName] Group name of the protocol-based VLAN entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupName (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* get the value from application */
  owa.l7rc = usmDbPbVlanGroupNameGet(L7_UNIT_CURRENT, keyGroupIdValue, objGroupNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupNameValue, strlen (objGroupNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupName
*
* @purpose Set 'GroupName'
 *@description  [GroupName] Group name of the protocol-based VLAN entry
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupNameValue;

  xLibU32_t keyGroupIdValue;

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

  /* retrieve key: GroupId */
  owa.len = sizeof (keyGroupIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbPbVlanGroupNameSet(L7_UNIT_CURRENT, keyGroupIdValue, objGroupNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupVlanId
*
* @purpose Get 'GroupVlanId'
 *@description  [GroupVlanId] VLAN associated with the protocol-based VLAN
* entry. Set to 0 to remove.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupVlanId (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* get the value from application */
  owa.l7rc = usmDbPbVlanGroupVIDGet(L7_UNIT_CURRENT, keyGroupIdValue, &objGroupVlanIdValue);
  if(objGroupVlanIdValue<L7_DOT1Q_MIN_VLAN_ID)
  {
    return owa.rc;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupVlanIdValue, sizeof (objGroupVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupVlanId
*
* @purpose Set 'GroupVlanId'
 *@description  [GroupVlanId] VLAN associated with the protocol-based VLAN
* entry. Set to 0 to remove.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupVlanId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupVlanIdValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupVlanId */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGroupVlanIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupVlanIdValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  if(objGroupVlanIdValue == 0)
  {
    /* get the configured value from the application */
    owa.l7rc = usmDbPbVlanGroupVIDGet(L7_UNIT_CURRENT, keyGroupIdValue, &objGroupVlanIdValue);
    
    if(owa.l7rc == L7_SUCCESS) 
    {
      /* remove the configured vlan to specified group-id */    
      owa.l7rc = usmDbPbVlanGroupVlanIDDelete(L7_UNIT_CURRENT, keyGroupIdValue, objGroupVlanIdValue);
    }
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbPbVlanGroupVlanIDAdd(L7_UNIT_CURRENT, keyGroupIdValue, objGroupVlanIdValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolIP
*
* @purpose Get 'GroupProtocolIP'
 *@description  [GroupProtocolIP] The status of the IP protocol association with
* the protocol-based VLAN entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolIP (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupProtocolIPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* get the value from application */
  owa.l7rc = usmDbPbVlanGroupProtocolGet(L7_UNIT_CURRENT, keyGroupIdValue, protocol_buffer, type);
  
  if (owa.l7rc == L7_SUCCESS)
  {
     objGroupProtocolIPValue = L7_DISABLE;
     for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
     {
        if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_HEX_IP)
            objGroupProtocolIPValue = L7_ENABLE;
     }
  }
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupProtocolIP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupProtocolIPValue,
                           sizeof (objGroupProtocolIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolIP
*
* @purpose Set 'GroupProtocolIP'
 *@description  [GroupProtocolIP] The status of the IP protocol association with
* the protocol-based VLAN entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolIP (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupProtocolIPValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupProtocolIP */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGroupProtocolIPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupProtocolIPValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* set the value in application */
  if(objGroupProtocolIPValue == L7_ENABLE)
    owa.l7rc = usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_IP, 0);
  else
    owa.l7rc = usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_HEX_IP);

  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_TABLE_IS_FULL;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolIPX
*
* @purpose Get 'GroupProtocolIPX'
 *@description  [GroupProtocolIPX] The status of the IPX protocol association
* with the protocol-based VLAN entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolIPX (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupProtocolIPXValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* get the value from application */
  owa.l7rc = usmDbPbVlanGroupProtocolGet(L7_UNIT_CURRENT, keyGroupIdValue, protocol_buffer, type);
  
  if (owa.l7rc == L7_SUCCESS)
  {
     objGroupProtocolIPXValue = L7_DISABLE;
     for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
     {
        if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_HEX_IPX)
            objGroupProtocolIPXValue = L7_ENABLE;
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupProtocolIPX */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupProtocolIPXValue,
                           sizeof (objGroupProtocolIPXValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolIPX
*
* @purpose Set 'GroupProtocolIPX'
 *@description  [GroupProtocolIPX] The status of the IPX protocol association
* with the protocol-based VLAN entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolIPX (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupProtocolIPXValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupProtocolIPX */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGroupProtocolIPXValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupProtocolIPXValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* set the value in application */
  if(objGroupProtocolIPXValue == L7_ENABLE)
    owa.l7rc = usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_IPX, 0);
  else 
    owa.l7rc = usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_HEX_IPX);

  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_TABLE_IS_FULL;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjList_SwitchingPbvlanGroupConfig_GroupProtocol (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupProtocol;

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupProtocol,
                          (xLibU8_t *) objGroupProtocol, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    
    memset(objGroupProtocol, 0x00, sizeof(objGroupProtocol));
    strcpy(objGroupProtocol,"IP"); 
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    if( !strcmp(objGroupProtocol,"IP") )  
    {
       memset(objGroupProtocol, 0x00, sizeof(objGroupProtocol));
       strcpy(objGroupProtocol,"ARP");
       owa.l7rc = L7_SUCCESS;
    }
    else if(!strcmp(objGroupProtocol,"ARP") )
    {
       memset(objGroupProtocol, 0x00, sizeof(objGroupProtocol));
       strcpy(objGroupProtocol,"IPX");
       owa.l7rc = L7_SUCCESS;
    }
    else
       owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objGroupProtocol,
                           sizeof (objGroupProtocol));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocol (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupProtocol;
  L7_BOOL flag = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* get the value from application */
  owa.l7rc = usmDbPbVlanGroupProtocolGet(L7_UNIT_CURRENT, keyGroupIdValue, protocol_buffer, type);

  memset(objGroupProtocol, 0x00, sizeof(objGroupProtocol));

  if (owa.l7rc == L7_SUCCESS)
  {
     for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
     {
        if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_HEX_IP)
        {
            (void) osapiStrncat(objGroupProtocol,"IP", sizeof(objGroupProtocol)-strlen(objGroupProtocol)-1) ;
            flag = L7_TRUE;
            break;
        }   
     }
     for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
     {
        if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_HEX_ARP)
        {
            if (flag == L7_TRUE)
            {
              (void) osapiStrncat(objGroupProtocol,",ARP", sizeof(objGroupProtocol)-strlen(objGroupProtocol)-1) ;
            }
            else
            {
              (void) osapiStrncat(objGroupProtocol,"ARP", sizeof(objGroupProtocol)-strlen(objGroupProtocol)-1) ;
              flag = L7_TRUE;
            }
            break;
        }
     }
     for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
     {
        if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_HEX_IPX)
        {
            if (flag == L7_TRUE)
            {
              (void) osapiStrncat(objGroupProtocol,",IPX", sizeof(objGroupProtocol)-strlen(objGroupProtocol)-1) ;
            }
            else
            {
              (void) osapiStrncat(objGroupProtocol,"IPX", sizeof(objGroupProtocol)-strlen(objGroupProtocol)-1) ;
              flag = L7_TRUE;
            }

            break;
        }
     }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupProtocolIPX */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupProtocol,
                           sizeof (objGroupProtocol));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocol(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objGroupProtocol;

  xLibU8_t *temp1 = NULL;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupProtocolIPX */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objGroupProtocol, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupProtocolIPXValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

 /* set the value in application */
 (void) usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_HEX_IPX);
 (void) usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_HEX_IP);
 (void) usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_HEX_ARP);


  /* set the value in application */
  for(temp1 = strtok(objGroupProtocol,","); (temp1 != NULL); temp1 = strtok(NULL,","))
  {
     if(!strcmp(temp1,"IPX"))
     {
        owa.l7rc = usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_IPX, 0);

        if(owa.l7rc != L7_SUCCESS)
        {
          if(owa.l7rc == L7_TABLE_IS_FULL)
          {
            owa.rc = XLIBRC_TABLE_IS_FULL;
            break;
          }
          else
          {
            owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
            break;
          }
        }
     }

     if(!strcmp(temp1,"IP"))
     {
        owa.l7rc = usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_IP, 0);

        if(owa.l7rc != L7_SUCCESS)
        {
          if(owa.l7rc == L7_TABLE_IS_FULL)
          {
            owa.rc = XLIBRC_TABLE_IS_FULL;
            break;
          }
          else
          {
            owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
            break;
          }
        }
     }

     if(!strcmp(temp1,"ARP"))
     {
        owa.l7rc = usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_ARP, 0);

        if(owa.l7rc != L7_SUCCESS)
        {
          if(owa.l7rc == L7_TABLE_IS_FULL)
          {
            owa.rc = XLIBRC_TABLE_IS_FULL;
            break;
          }
          else
          {
            owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
            break;
          }
        }
     }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolList
*
* @purpose Get 'GroupProtocolList'
 *@description  [GroupProtocolList] The protocol association with the
* protocol-based VLAN entry in the form of hexadecimal numbers separated by
* commas.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupProtocolListValue;

  xLibU32_t keyGroupIdValue;
  L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_BOOL commaFlag = L7_FALSE;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  memset(objGroupProtocolListValue, 0x0, sizeof(objGroupProtocolListValue));
  owa.len = sizeof (keyGroupIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, owa.len);

  /* get the value from application */
  if ((usmDbPbVlanGroupProtocolGet(L7_UNIT_CURRENT, keyGroupIdValue, protocol, type)) == L7_SUCCESS)
  {
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      memset(buf, 0x00, sizeof(buf));
      if(protocol[i] != L7_PBVLAN_INVALID_PROTOCOL)
      {
        if(type[i] == 0)
        {
          if(protocol[i] == L7_USMDB_PROTOCOL_HEX_IP)
          {
            OSAPI_STRNCAT(buf, "IP");
          }
          else if(protocol[i] == L7_USMDB_PROTOCOL_HEX_ARP)
          {
            OSAPI_STRNCAT(buf, "ARP");
          }
          else if(protocol[i] == L7_USMDB_PROTOCOL_HEX_IPX)
          {
            OSAPI_STRNCAT(buf, "IPX");
          }
        }
        else
        {
          sprintf(buf, "0x%4.4x", protocol[i]);
        }
        if(commaFlag == L7_TRUE)
        {
          strcat(objGroupProtocolListValue, ",");
          strcat(objGroupProtocolListValue, buf);
        }
        else
        {
          strcat(objGroupProtocolListValue, buf);
          commaFlag = L7_TRUE;
        }
      }
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

  FPOBJ_TRACE_VALUE (bufp, objGroupProtocolListValue, strlen (objGroupProtocolListValue));

  /* return the object value: GroupProtocolList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objGroupProtocolListValue,
                           strlen (objGroupProtocolListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolList
*
* @purpose Set 'GroupProtocolList'
 *@description  [GroupProtocolList] The protocol association with the
* protocol-based VLAN entry in the form of hexadecimal numbers separated by
* commas.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolList (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objGroupProtocolListValue;
  xLibU32_t keyGroupIdValue;
  xLibU8_t *temp1 = NULL;
  L7_uint32 protocolListPrev[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocolListPrev1[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 i,count,protocol;
  L7_RC_t rc;
  L7_BOOL flag = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);
 
  memset(protocolListPrev, 0x00, sizeof(protocolListPrev));

  /* retrieve object: GroupProtocolList */
  owa.len = sizeof (objGroupProtocolListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objGroupProtocolListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objGroupProtocolListValue, owa.len);

  /* retrieve key: GroupId */
  owa.len = sizeof (keyGroupIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* First we have to delete previous list and then we have to add new list */
  memset(protocolListPrev1, 0x00, sizeof(protocolListPrev1));
  rc = usmDbPbVlanGroupProtocolGet(L7_UNIT_CURRENT, keyGroupIdValue, protocolListPrev1, type);

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if(protocolListPrev1[i] != L7_PBVLAN_INVALID_PROTOCOL)
    {
       usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, protocolListPrev1[i]);
    }
  }

  if(strlen(objGroupProtocolListValue) == 0)
  {
    owa.l7rc = L7_SUCCESS;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(protocolListPrev, 0x00, sizeof(protocolListPrev));

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, owa.len);


  for(temp1 = strtok(objGroupProtocolListValue,","), i = 0; (temp1 != NULL); temp1 = strtok(NULL,","))
  {
    fpobjUtil_ProtoStrConvertToLowerCase(temp1);
    if (strcmp(temp1, "ip") == 0)
    {
      protocol = L7_USMDB_PROTOCOL_HEX_IP;
      protocolListPrev[i] = protocol;
      type[i] = 0;
      i++;
      continue;
    }
    else if (strcmp(temp1, "arp") == 0)
    {
      protocol = L7_USMDB_PROTOCOL_HEX_ARP;
      protocolListPrev[i] = protocol;
      type[i] = 0;
      i++;
      continue;
    }
    else if (strcmp(temp1, "ipx") == 0)
    {
      protocol = L7_USMDB_PROTOCOL_HEX_IPX;
      protocolListPrev[i] = protocol;
      type[i] = 0;
      i++;
      continue;
    }
    else if((temp1[0] != '0' ) && ((temp1[1] != 'x') || (temp1[1] != 'X')))
    {
       if (convertTo32BitUnsignedInteger(temp1, &protocol) != L7_SUCCESS)
       {
         owa.l7rc = L7_FAILURE;
         break;
       }
    }
    else if (cliConvertEtypeCustomValue(temp1, &protocol) != L7_TRUE)
    {
       owa.l7rc = L7_FAILURE;
       break;
    } 

    if ((protocol< L7_PBVLAN_CUSTOM_PROTOCOL_MIN) ||
        (protocol> L7_PBVLAN_CUSTOM_PROTOCOL_MAX))
    {
       owa.l7rc = L7_FAILURE;
       break;
    }
    protocolListPrev[i] = protocol;
    type[i] = 1;
    i++; 
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  for(count=0; count < i; count++)
  {
    /* If the given protocol is already exist in that group, just skip */ 
    if(usmDbPbVlanGroupProtocolGetExact(L7_UNIT_CURRENT, keyGroupIdValue, protocolListPrev[count]) == L7_SUCCESS)
      continue; 
    owa.l7rc =  usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, protocolListPrev[count], type[count]);

    if(owa.l7rc != L7_SUCCESS)
    {
      if(owa.l7rc == L7_TABLE_IS_FULL)
      {
        /*Table is full. Exit from for loop. Do not attempt adding any more protocols*/
        owa.rc = XLIBRC_TABLE_IS_FULL;
        flag = L7_TRUE;
        break;
      }
      else
      {
        /*continue adding other protocols*/
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        flag = L7_TRUE;
      }
    }
  }
  
  if(flag == L7_TRUE)
  {
    owa.l7rc = L7_FAILURE;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolARP
*
* @purpose Get 'GroupProtocolARP'
 *@description  [GroupProtocolARP] The status of the ARP protocol association
* with the protocol-based VLAN entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupProtocolARP (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
  L7_uint32 protocol_buffer[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 protocol_index;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupProtocolARPValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* get the value from application */
  owa.l7rc = usmDbPbVlanGroupProtocolGet(L7_UNIT_CURRENT, keyGroupIdValue, protocol_buffer, type);

  if (owa.l7rc == L7_SUCCESS)
  {
     objGroupProtocolARPValue = L7_DISABLE;
     for (protocol_index = 0;  protocol_index < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; protocol_index++)
     {
        if (protocol_buffer[protocol_index] == L7_USMDB_PROTOCOL_HEX_ARP)
            objGroupProtocolARPValue = L7_ENABLE;
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: GroupProtocolARP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupProtocolARPValue,
                           sizeof (objGroupProtocolARPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolARP
*
* @purpose Set 'GroupProtocolARP'
 *@description  [GroupProtocolARP] The status of the ARP protocol association
* with the protocol-based VLAN entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupProtocolARP (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupProtocolARPValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupProtocolARP */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGroupProtocolARPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupProtocolARPValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  /* set the value in application */
  if(objGroupProtocolARPValue == L7_ENABLE)
    owa.l7rc = usmDbPbVlanGroupProtocolAdd(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_ARP, 0);
  else
    owa.l7rc = usmDbPbVlanGroupProtocolDelete(L7_UNIT_CURRENT, keyGroupIdValue, L7_USMDB_PROTOCOL_HEX_ARP);

  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_TABLE_IS_FULL;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
  }
		
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingPbvlanGroupConfig_GroupStatus
*
* @purpose Get 'GroupStatus'
 *@description  [GroupStatus] The status of the protocol-based VLAN entry.
* active(1) - this entry is active destroy(6) - remove this entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);

  objGroupStatusValue = L7_ROW_STATUS_ACTIVE;
  owa.l7rc = L7_SUCCESS;

  #if 0
  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyGroupIdValue, &objGroupStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif
  /* return the object value: GroupStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGroupStatusValue, sizeof (objGroupStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingPbvlanGroupConfig_GroupStatus
*
* @purpose Set 'GroupStatus'
 *@description  [GroupStatus] The status of the protocol-based VLAN entry.
* active(1) - this entry is active destroy(6) - remove this entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGroupStatusValue;

  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GroupStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGroupStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupStatusValue, owa.len);

  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_PBVLAN_GROUPID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
  if (objGroupStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {

    /*We are creating entry in seperate object handler */

    owa.l7rc =  usmDbPbVlanGroupCreate(L7_UNIT_CURRENT, keyGroupIdValue);
  }
  else if (objGroupStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbPbVlanGroupDelete(L7_UNIT_CURRENT, keyGroupIdValue);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_PBVLAN_GROUP_DEL_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;     
      }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjList_SwitchingPbvlanGroupConfig_GroupInterface (void *wap, void *bufp)
{

  L7_INTF_MASK_t interfacePortMask;
  L7_uint32 interfacePort; 
  L7_uint32 nextPort, flag;
  L7_INTF_MASK_t mask;
 
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
 
  flag = L7_TRUE;
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
 
  xLibU16_t bufLen = sizeof (interfacePortMask);
  memset(&interfacePortMask,0x0,sizeof(interfacePortMask)); 
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupInterface,
        (xLibU8_t *) &interfacePortMask, &bufLen))
  {
    interfacePort = 0;
    while((usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, interfacePort, &nextPort) == L7_SUCCESS))
    {
      if(usmDbPbVlanGroupPortGetExact(L7_UNIT_CURRENT, keyGroupIdValue, nextPort) == L7_SUCCESS)
      {
        flag =L7_FALSE;
        break;
      }
      interfacePort = nextPort;
    }
  }
  else
  {
    interfacePort = fpObjPortFromMaskGet(interfacePortMask);   
    if(interfacePort != -1)
    { 
      while((usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, interfacePort, &nextPort) == L7_SUCCESS))
      {
        if(usmDbPbVlanGroupPortGetExact(L7_UNIT_CURRENT, keyGroupIdValue, nextPort) == L7_SUCCESS)
        {
          flag =L7_FALSE;
          break;
        }
        interfacePort = nextPort;
      }
    }
    else
    {
      flag = L7_TRUE;
    }
  }
  if (flag == L7_TRUE)
  { 
    return XLIBRC_ENDOF_TABLE;
  }
 
  memset(&mask,0x0,sizeof(mask));
  L7_INTF_SETMASKBIT(mask,nextPort);
  return  xLibBufDataSet (bufp, (xLibU8_t *) &mask, sizeof (mask));
 
}

xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupInterface (void *wap, void *bufp)
{
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t portMask;
 
  L7_uint32 intfList[L7_FILTER_MAX_INTF]={0};
  L7_uint32 intfpbVlan;
  L7_uint32 numIntf;
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
 

  numIntf = 0;
  intfpbVlan = 0;
  while (usmDbPbVlanGroupPortGetNext(L7_UNIT_CURRENT, keyGroupIdValue, intfpbVlan, &intfpbVlan) == L7_SUCCESS)
  {
    intfList[numIntf] = intfpbVlan;
    numIntf++;
  }

  memset(&portMask, 0, sizeof(portMask));

  if (numIntf != 0)
  { 
    owa.l7rc =  usmDbConvertIntfListToMask(intfList, numIntf, &portMask);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
 
  /* return the object value: GroupInterface*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &portMask,
                           sizeof (portMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}
 

xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupInterface(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t intfMask;
  memset(&intfMask,0x0,sizeof(intfMask));
 
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
 
  L7_uint32 numIntf, intIfNum, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;
 

  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
  L7_BOOL errorFound;
  L7_RC_t rc;
 
  errorFound = L7_FALSE;
 
  FPOBJ_TRACE_ENTER (bufp);
 
    /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
 
 
  /* retrieve object: GroupInterface */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupInterfaceIPXValue, owa.len);
 
  if(usmDbConvertMaskToList(&intfMask,intfList,&intfCount) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  memset(intfListPrev, 0, sizeof(intfListPrev));
  memset(intfListToRemove, 0, sizeof(intfListToRemove));
  memset(intfListToAdd, 0, sizeof(intfListToAdd));

  /* Get the list of interfaces already configured with the group */
  intIfNum = 0;
  numIntfPrev = 0;
  while (usmDbPbVlanGroupPortGetNext(L7_UNIT_CURRENT, keyGroupIdValue, intIfNum, &intIfNum) == L7_SUCCESS)
  {
    intfListPrev[numIntfPrev++] = intIfNum;
  }
 
  numIntf = intfCount;
  if (numIntf > 0)
  {
    /* Figure out which ports to remove */
    numIntfToRemove = 0;
    for (i=0; i<numIntfPrev; i++)
    {
       intfFound = L7_FALSE;
       for (j=1; j<=numIntf; j++)
       {
         if (intfListPrev[i] == intfList[j])
         {
           intfFound = L7_TRUE;
           break;
         }
       }
       if (intfFound == L7_FALSE)
       {
         intfListToRemove[numIntfToRemove++] = intfListPrev[i];
       }
    }
 
    /* Figure out which ports to add */
    numIntfToAdd = 0;
    for (i=1; i<=numIntf; i++)
    {
      intfFound = L7_FALSE;
      for (j=0; j<numIntfPrev; j++)
      {
        if (intfList[i] == intfListPrev[j])
        {
          intfFound = L7_TRUE;
          break;
        }
      }
      if (intfFound == L7_FALSE)
      {
        intfListToAdd[numIntfToAdd++] = intfList[i];
      }
    }
    errorFound = L7_FALSE;
 
    for (i=0; i<numIntfToAdd; i++)
    {
      rc = usmDbPbVlanGroupPortAdd(L7_UNIT_CURRENT, keyGroupIdValue, intfListToAdd[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
    
    for (i=0; i<numIntfToRemove; i++)
    {
      rc = usmDbPbVlanGroupPortDelete(L7_UNIT_CURRENT, keyGroupIdValue, intfListToRemove[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
        break;
      }
    }
  }
  else    /* Remove all configured Interfaces */
  {
    numIntfToRemove = 0;
    errorFound = L7_FALSE;
 
    for (i=0; i<numIntfPrev; i++)
    {
      rc = usmDbPbVlanGroupPortDelete(L7_UNIT_CURRENT, keyGroupIdValue, intfListPrev[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
  }
 
  if(errorFound == L7_TRUE )
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
* @function fpObjList_SwitchingPbvlanGroupConfig_GroupUnConfigInterface
*
* @purpose List 'GroupUnConfigInterface'
 *@description  [GroupUnConfigInterface] The interface number of this instance
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupUnConfigInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGroupUnConfigInterfaceValue;
  xLibU32_t nextObjGroupUnConfigInterfaceValue;
  xLibU32_t keyGroupIdValue;
  L7_uint32 interface, flag;

  FPOBJ_TRACE_ENTER (bufp);

  flag = L7_TRUE;
   /* retrieve key: GroupId */
  owa.len = sizeof (keyGroupIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                          (xLibU8_t *) & keyGroupIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.len = sizeof (objGroupUnConfigInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupUnConfigInterface,
                          (xLibU8_t *) & objGroupUnConfigInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
   
    interface = 0;

    while((usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, interface, &interface) == L7_SUCCESS))
    {
      if(usmDbPbVlanGroupPortGetExact(L7_UNIT_CURRENT, keyGroupIdValue, interface) != L7_SUCCESS)
      {
        flag =L7_FALSE;
        break;
      }
    }

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGroupUnConfigInterfaceValue, owa.len);
   
    interface = objGroupUnConfigInterfaceValue;

    while((usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0, interface, &interface) == L7_SUCCESS))
    {
      if(usmDbPbVlanGroupPortGetExact(L7_UNIT_CURRENT, keyGroupIdValue, interface) != L7_SUCCESS)
      {
        flag =L7_FALSE;
        break;
      }
    }

  }

  if(flag == L7_TRUE)
  {
    owa.l7rc = L7_FAILURE;
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
  }
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjGroupUnConfigInterfaceValue = interface;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjGroupUnConfigInterfaceValue, owa.len);

  /* return the object value: GroupUnConfigInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjGroupUnConfigInterfaceValue,
                           sizeof (nextObjGroupUnConfigInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjList_SwitchingPbvlanGroupConfig_GroupInterface2 (void *wap, void *bufp)
{
  L7_RC_t rv;
  L7_INTF_MASK_t interfacePortMask;
  L7_uint32 interfacePort; 
  L7_uint32 nextPort;
  L7_INTF_MASK_t mask;
 
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
 
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
 
  xLibU16_t bufLen = sizeof (interfacePortMask);
  memset(&interfacePortMask,0x0,sizeof(interfacePortMask)); 
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupInterface2,
        (xLibU8_t *) &interfacePortMask, &bufLen))
  {
    rv = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextPort);
  }
  else
  {
    interfacePort = fpObjPortFromMaskGet(interfacePortMask);   
    if(interfacePort != -1){ 
      rv = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, interfacePort, &nextPort);
    }else{
      rv = L7_FAILURE;
    }
  }
 
  if (rv != L7_SUCCESS)
  {
    return XLIBRC_ENDOF_TABLE;
  }
 
  memset(&mask,0x0,sizeof(mask));
  L7_INTF_SETMASKBIT(mask,nextPort);
  return  xLibBufDataSet (bufp, (xLibU8_t *) &mask, sizeof (mask));
 
}
 

xLibRC_t fpObjGet_SwitchingPbvlanGroupConfig_GroupInterface2 (void *wap, void *bufp)
{
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t portMask;
 
  L7_uint32 intfList[L7_FILTER_MAX_INTF]={0};
  L7_uint32 intfpbVlan;
  L7_uint32 numIntf;
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
 

  numIntf = 0;
  intfpbVlan = 0;
  while (usmDbPbVlanGroupPortGetNext(L7_UNIT_CURRENT, keyGroupIdValue, intfpbVlan, &intfpbVlan) == L7_SUCCESS)
  {
    intfList[numIntf] = intfpbVlan;
    numIntf++;
  }

  memset(&portMask, 0, sizeof(portMask));

  if (numIntf != 0)
  { 
    owa.l7rc =  usmDbConvertIntfListToMask(intfList, numIntf, &portMask);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
 
  /* return the object value: GroupInterface*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &portMask,
                           sizeof (portMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}
 

xLibRC_t fpObjSet_SwitchingPbvlanGroupConfig_GroupInterface2(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  L7_INTF_MASK_t intfMask;
  memset(&intfMask,0x0,sizeof(intfMask));
 
  fpObjWa_t kwaGroupId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyGroupIdValue;
 
  L7_uint32 numIntf, intIfNum, numIntfPrev,intfCount,i, j, numIntfToRemove, numIntfToAdd;
 

  L7_uint32 intfList[L7_FILTER_MAX_INTF];        /*L7_MAX_PHYSICAL_PORTS_PER_SLOT*/
  L7_uint32 intfListPrev[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToAdd[L7_FILTER_MAX_INTF];
  L7_uint32 intfListToRemove[L7_FILTER_MAX_INTF];
  L7_BOOL intfFound;
  L7_BOOL errorFound;
  L7_RC_t rc;
 
  errorFound = L7_FALSE;
 
  FPOBJ_TRACE_ENTER (bufp);
 
    /* retrieve key: GroupId */
  kwaGroupId.rc = xLibFilterGet (wap, XOBJ_SwitchingPbvlanGroupConfig_GroupId,
                                 (xLibU8_t *) & keyGroupIdValue, &kwaGroupId.len);
  if (kwaGroupId.rc != XLIBRC_SUCCESS)
  {
    kwaGroupId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaGroupId);
    return kwaGroupId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGroupIdValue, kwaGroupId.len);
 
 
  /* retrieve object: GroupInterface */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGroupInterfaceIPXValue, owa.len);
 
  if(usmDbConvertMaskToList(&intfMask,intfList,&intfCount) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  memset(intfListPrev, 0, sizeof(intfListPrev));
  memset(intfListToRemove, 0, sizeof(intfListToRemove));
  memset(intfListToAdd, 0, sizeof(intfListToAdd));

  /* Get the list of interfaces already configured with the group */
  intIfNum = 0;
  numIntfPrev = 0;
  while (usmDbPbVlanGroupPortGetNext(L7_UNIT_CURRENT, keyGroupIdValue, intIfNum, &intIfNum) == L7_SUCCESS)
  {
    intfListPrev[numIntfPrev++] = intIfNum;
  }
 
  numIntf = intfCount;
  if (numIntf > 0)
  {
    /* Figure out which ports to remove */
    numIntfToRemove = 0;
    for (i=0; i<numIntfPrev; i++)
    {
       intfFound = L7_FALSE;
       for (j=1; j<=numIntf; j++)
       {
         if (intfListPrev[i] == intfList[j])
         {
           intfFound = L7_TRUE;
           break;
         }
       }
       if (intfFound == L7_FALSE)
       {
         intfListToRemove[numIntfToRemove++] = intfListPrev[i];
       }
    }
 
    /* Figure out which ports to add */
    numIntfToAdd = 0;
    for (i=1; i<=numIntf; i++)
    {
      intfFound = L7_FALSE;
      for (j=0; j<numIntfPrev; j++)
      {
        if (intfList[i] == intfListPrev[j])
        {
          intfFound = L7_TRUE;
          break;
        }
      }
      if (intfFound == L7_FALSE)
      {
        intfListToAdd[numIntfToAdd++] = intfList[i];
      }
    }
    errorFound = L7_FALSE;
 
    for (i=0; i<numIntfToAdd; i++)
    {
      rc = usmDbPbVlanGroupPortAdd(L7_UNIT_CURRENT, keyGroupIdValue, intfListToAdd[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
    
    for (i=0; i<numIntfToRemove; i++)
    {
      rc = usmDbPbVlanGroupPortDelete(L7_UNIT_CURRENT, keyGroupIdValue, intfListToRemove[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
        break;
      }
    }
  }
  else    /* Remove all configured Interfaces */
  {
    numIntfToRemove = 0;
    errorFound = L7_FALSE;
 
    for (i=0; i<numIntfPrev; i++)
    {
      rc = usmDbPbVlanGroupPortDelete(L7_UNIT_CURRENT, keyGroupIdValue, intfListPrev[i]);
      if (rc != L7_SUCCESS)
      {
        errorFound = L7_TRUE;
      }
    }
  }
 
  if(errorFound == L7_TRUE )
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
